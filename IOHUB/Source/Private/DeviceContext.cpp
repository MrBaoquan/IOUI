#include "DeviceContext.h"
#include "DataFormatter.h"
#include <chrono>
#include <thread>

namespace IOHub {

DeviceContext::DeviceContext(uint8_t deviceIndex, 
                             std::unique_ptr<IProtocol> protocol,
                             size_t inputCount,
                             size_t outputCount)
    : deviceIndex_(deviceIndex)
    , protocol_(std::move(protocol))
    , inputCount_(inputCount)
    , outputCount_(outputCount)
    , inputHoldMs_(1000)  // 默认1秒（0=永久保持）
{
    lastDOStatus_.resize(outputCount_, 0);
    diStatus_.resize(inputCount_, 0);
    diTimestamps_.resize(inputCount_);
    
    // 初始化所有通道时间戳为当前时间
    auto now = std::chrono::steady_clock::now();
    for (auto& ts : diTimestamps_) {
        ts = now;
    }
}

DeviceContext::~DeviceContext() {
    stop();
}

void DeviceContext::start() {
    stopFlag_.store(false);
    workerThread_ = std::thread(&DeviceContext::processOutputLoop, this);
}

void DeviceContext::stop() {
    if (!stopFlag_.exchange(true)) {
        queueCV_.notify_all();
        
        if (workerThread_.joinable()) {
            workerThread_.join();
        }
        
        if (protocol_) {
            protocol_->stop();
        }
    }
}

void DeviceContext::setFrameProcessor(std::unique_ptr<FrameProcessor> processor) {
    frameProcessor_ = std::move(processor);
}

void DeviceContext::setInputTimeout(int timeoutMs) {
    inputHoldMs_ = timeoutMs;
}

bool DeviceContext::isSerialProtocol() const {
    return protocol_ && protocol_->getType() == ProtocolType::SERIAL;
}

bool DeviceContext::setDO(const short* doStatus, size_t count) {
    if (count != outputCount_) {
        return false;
    }
    
    // 检测状态变化
    std::map<int, short> dirtyStatus;
    for (size_t i = 0; i < count; ++i) {
        if (doStatus[i] != lastDOStatus_[i]) {
            dirtyStatus[i] = doStatus[i];
            lastDOStatus_[i] = doStatus[i];
        }
    }
    
    if (dirtyStatus.empty()) {
        return true;
    }
    
    // 推入脏数据队列
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        if (dirtyQueue_.size() >= MAX_QUEUE_SIZE) {
            // 队列满了，合并到最后一个元素
            if (!dirtyQueue_.empty()) {
                auto& last = dirtyQueue_.back();
                for (const auto& kv : dirtyStatus) {
                    last[kv.first] = kv.second;
                }
            }
        } else {
            dirtyQueue_.push(dirtyStatus);
        }
    }
    
    queueCV_.notify_one();
    return true;
}

bool DeviceContext::getDI(uint8_t* diStatus, size_t count) {
    if (count != inputCount_) {
        return false;
    }
    
    if (!protocol_) {
        std::fill_n(diStatus, count, 0);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(diMutex_);
    
    auto now = std::chrono::steady_clock::now();
    
    // 检查保持时间并清零过期通道（inputHoldMs_=0 时永久保持，不清零）
    if (inputHoldMs_ > 0) {
        for (size_t i = 0; i < inputCount_; ++i) {
            if (diStatus_[i] != 0) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - diTimestamps_[i]).count();
                
                if (elapsed > inputHoldMs_) {
                    diStatus_[i] = 0;  // 超时，清零
                }
            }
        }
    }
    
    // 处理所有接收到的数据
    std::vector<uint8_t> recvData;
    while (protocol_->receive(recvData) > 0) {
        if (frameProcessor_) {
            // 帧处理模式：使用帧处理器解析数据
            frameProcessor_->addReceivedData(recvData.data(), recvData.size());
            
            std::vector<uint8_t> frame;
            while (frameProcessor_->extractFrame(frame)) {
                // 先尝试自定义映射
                uint8_t channel = 0;
                if (mapping_.findInputChannel(frame, channel)) {
                    if (channel < inputCount_) {
                        diStatus_[channel] = 1;
                        diTimestamps_[channel] = now;
                    }
                }
                // 否则尝试解析标准帧
                else {
                    uint8_t frameChannel = 0;
                    uint8_t frameValue = 0;
                    if (frameProcessor_->parseFrame(frame, frameChannel, frameValue)) {
                        if (frameChannel < inputCount_) {
                            diStatus_[frameChannel] = frameValue;
                            diTimestamps_[frameChannel] = now;
                        }
                    }
                }
            }
        }
        else {
            // 直接映射模式：不使用帧格式，直接匹配数据
            uint8_t channel = 0;
            if (mapping_.findInputChannel(recvData, channel)) {
                if (channel < inputCount_) {
                    diStatus_[channel] = 1;
                    diTimestamps_[channel] = now;
                }
            }
        }
        
        recvData.clear();
    }
    
    // 复制当前状态到输出
    std::copy(diStatus_.begin(), diStatus_.end(), diStatus);
    
    return true;
}

void DeviceContext::processOutputLoop() {
    while (!stopFlag_.load()) {
        std::map<int, short> dirtyStatus;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCV_.wait(lock, [this] {
                return stopFlag_.load() || !dirtyQueue_.empty();
            });
            
            if (stopFlag_.load()) {
                break;
            }
            
            if (!dirtyQueue_.empty()) {
                dirtyStatus = std::move(dirtyQueue_.front());
                dirtyQueue_.pop();
            }
        }
        
        if (!dirtyStatus.empty()) {
            processDirtyStatus(dirtyStatus);
        }
    }
}

void DeviceContext::processDirtyStatus(const std::map<int, short>& dirtyData) {
    if (!protocol_ || !protocol_->isConnected()) {
        return;
    }
    
    for (const auto& kv : dirtyData) {
        int channel = kv.first;
        short value = kv.second;
        
        if (channel < 0 || channel >= static_cast<int>(outputCount_)) {
            continue;
        }
        
        MappingItem item;
        std::vector<uint8_t> sendData;
        
        // 优先查找自定义映射
        if (mapping_.findOutputData(static_cast<uint8_t>(channel), item)) {
            sendData = item.data;
        }
        // 如果有帧处理器，使用标准帧格式（所有协议均支持）
        else if (frameProcessor_) {
            sendData = frameProcessor_->buildFrame(static_cast<uint8_t>(channel), 
                                                   static_cast<uint8_t>(value));
        }
        else {
            continue;
        }
        
        if (!sendData.empty()) {
            protocol_->send(sendData.data(), sendData.size());
            
            // 等待
            if (writeWaitMs_ > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(writeWaitMs_));
            }
        }
    }
}

} // namespace IOHub