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
    
    // 初始化自定义映射缓冲区时间戳
    lastBufferUpdateTime_ = now;
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

bool DeviceContext::getDO(short* doStatus, size_t count) {
    if (count != outputCount_) {
        return false;
    }
    
    std::copy(lastDOStatus_.begin(), lastDOStatus_.end(), doStatus);
    return true;
}

void DeviceContext::resetDO() {
    std::fill(lastDOStatus_.begin(), lastDOStatus_.end(), 0);
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
        bool dataHandled = false;  // 标记数据是否被成功处理
        
        // === 策略1: 优先尝试直接匹配（快速路径，适用于单次接收完整数据） ===
        uint8_t channel = 0;
        bool directMappingFound = mapping_.findInputChannel(recvData, channel);
        
        if (directMappingFound && channel < inputCount_) {
            // 找到直接映射
            diStatus_[channel] = 1;
            diTimestamps_[channel] = now;
            dataHandled = true;
            
            recvData.clear();
            continue;  // 继续处理下一个数据包
        }
        
        // === 策略2: 使用帧处理器（适用于标准帧格式） ===
        if (frameProcessor_) {
            frameProcessor_->addReceivedData(recvData.data(), recvData.size());
            
            std::vector<uint8_t> frame;
            while (frameProcessor_->extractFrame(frame)) {
                // 尝试在提取的帧中查找自定义映射
                uint8_t frameChannel = 0;
                if (mapping_.findInputChannel(frame, frameChannel)) {
                    if (frameChannel < inputCount_) {
                        diStatus_[frameChannel] = 1;
                        diTimestamps_[frameChannel] = now;
                        dataHandled = true;
                    }
                }
                // 否则尝试解析标准帧（通道号+值）
                else {
                    uint8_t standardChannel = 0;
                    uint8_t standardValue = 0;
                    if (frameProcessor_->parseFrame(frame, standardChannel, standardValue)) {
                        if (standardChannel < inputCount_) {
                            diStatus_[standardChannel] = standardValue;
                            diTimestamps_[standardChannel] = now;
                            dataHandled = true;
                        }
                    }
                }
            }
        }
        
        // === 策略3: 数据累积缓冲（适用于分包接收的自定义映射） ===
        // 如果前两种策略都没有成功处理数据，使用缓冲区累积
        if (!dataHandled) {
            // 将新接收的数据追加到缓冲区
            customMappingBuffer_.insert(
                customMappingBuffer_.end(),
                recvData.begin(),
                recvData.end()
            );
            
            // 更新缓冲区时间戳
            lastBufferUpdateTime_ = now;
            
            // 限制缓冲区大小，防止内存溢出
            if (customMappingBuffer_.size() > MAX_CUSTOM_BUFFER_SIZE) {
                // 删除最旧的数据
                size_t excess = customMappingBuffer_.size() - MAX_CUSTOM_BUFFER_SIZE;
                customMappingBuffer_.erase(
                    customMappingBuffer_.begin(),
                    customMappingBuffer_.begin() + excess
                );
            }
            
            // 尝试在累积的缓冲区中查找映射
            uint8_t bufferChannel = 0;
            if (mapping_.findInputChannel(customMappingBuffer_, bufferChannel)) {
                if (bufferChannel < inputCount_) {
                    diStatus_[bufferChannel] = 1;
                    diTimestamps_[bufferChannel] = now;
                    
                    // 找到映射后清空缓冲区，准备接收下一个命令
                    customMappingBuffer_.clear();
                }
            }
            // 如果缓冲区中还没找到映射，继续累积（不清空）
            // 等待更多数据到达后再次尝试匹配
        }
        
        recvData.clear();
    }
    
    // 检查缓冲区是否超时，如果超时则清空（防止垃圾数据累积）
    if (!customMappingBuffer_.empty()) {
        auto bufferAge = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastBufferUpdateTime_).count();
        
        if (bufferAge > CUSTOM_BUFFER_TIMEOUT_MS) {
            // 缓冲区超时，可能是垃圾数据，清空它
            customMappingBuffer_.clear();
        }
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
            
            // 等待（避免粘包问题，0=不等待）
            if (writeWaitMs_ > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(writeWaitMs_));
            }
        }
    }
}

} // namespace IOHub