#include "FrameProcessor.h"
#include <algorithm>

namespace IOHub {

FrameProcessor::FrameProcessor(const FrameConfig& config)
    : config_(config)
{
}

void FrameProcessor::addReceivedData(const uint8_t* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        recvBuffer_.push_back(data[i]);
    }
}

bool FrameProcessor::extractFrame(std::vector<uint8_t>& outFrame) {
    if (recvBuffer_.size() < config_.length) {
        return false;
    }
    
    // 查找帧头
    auto headerIt = std::find(recvBuffer_.begin(), recvBuffer_.end(), config_.header);
    
    if (headerIt == recvBuffer_.end()) {
        // 没找到帧头，清空缓冲区
        recvBuffer_.clear();
        return false;
    }
    
    // 删除帧头之前的数据
    if (headerIt != recvBuffer_.begin()) {
        recvBuffer_.erase(recvBuffer_.begin(), headerIt);
    }
    
    // 检查是否有足够的数据
    if (recvBuffer_.size() < config_.length) {
        return false;
    }
    
    // 验证帧尾
    if (recvBuffer_[config_.length - 1] != config_.tail) {
        // 帧尾不匹配，删除当前帧头，继续查找
        recvBuffer_.pop_front();
        return extractFrame(outFrame); // 递归查找下一个
    }
    
    // 提取完整帧
    outFrame.clear();
    outFrame.reserve(config_.length);
    for (size_t i = 0; i < config_.length; ++i) {
        outFrame.push_back(recvBuffer_[i]);
    }
    
    // 从缓冲区删除已提取的帧
    recvBuffer_.erase(recvBuffer_.begin(), recvBuffer_.begin() + config_.length);
    
    return true;
}

std::vector<uint8_t> FrameProcessor::buildFrame(uint8_t channel, uint8_t value) const {
    std::vector<uint8_t> frame(config_.length, 0);
    
    frame[0] = config_.header;
    frame[config_.length - 1] = config_.tail;
    
    if (config_.channelIndex < config_.length) {
        // 应用通道偏移量（例如淘达电子从1开始，偏移量为1）
        frame[config_.channelIndex] = channel + config_.channelOffset;
    }
    
    if (config_.valueIndex < config_.length) {
        // 应用输出值映射（例如淘达电子输出：0->0x02, 非0->0x01）
        if (value == 0) {
            frame[config_.valueIndex] = config_.outputValueOffCode;
        } else {
            frame[config_.valueIndex] = config_.outputValueOnCode;
        }
    }
    
    return frame;
}

bool FrameProcessor::parseFrame(const std::vector<uint8_t>& frame, uint8_t& outChannel, uint8_t& outValue) const {
    if (frame.size() != config_.length) {
        return false;
    }
    
    if (frame[0] != config_.header || frame[config_.length - 1] != config_.tail) {
        return false;
    }
    
    if (config_.channelIndex < config_.length) {
        // 减去通道偏移量，转换为从0开始的通道号
        uint8_t rawChannel = frame[config_.channelIndex];
        if (rawChannel >= config_.channelOffset) {
            outChannel = rawChannel - config_.channelOffset;
        } else {
            return false;  // 无效的通道号
        }
    }
    
    if (config_.valueIndex < config_.length) {
        // 应用输入值映射反向转换（例如淘达电子输入：0x00->0, 0x01->1）
        uint8_t rawValue = frame[config_.valueIndex];
        if (rawValue == config_.inputValueOffCode) {
            outValue = 0;  // OFF/低电平
        } else if (rawValue == config_.inputValueOnCode) {
            outValue = 1;  // ON/高电平
        } else {
            // 对于其他值，保持原样（兼容模式）
            outValue = rawValue;
        }
    }
    
    return true;
}

void FrameProcessor::clearBuffer() {
    recvBuffer_.clear();
}

void FrameProcessor::updateChannelTimestamp(uint8_t channel) {
    channelTimestamps_[channel] = std::chrono::steady_clock::now();
}

void FrameProcessor::checkTimeouts(std::vector<uint8_t>& diStatus) {
    auto now = std::chrono::steady_clock::now();
    std::vector<uint8_t> timedOutChannels;
    
    for (auto& kv : channelTimestamps_) {
        uint8_t channel = kv.first;
        auto& lastUpdate = kv.second;
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
        
        if (elapsed > config_.timeoutMs) {
            if (channel < diStatus.size() && diStatus[channel] != 0) {
                diStatus[channel] = 0;
                timedOutChannels.push_back(channel);
            }
        }
    }
    
    // 清理超时的时间戳
    for (uint8_t channel : timedOutChannels) {
        channelTimestamps_.erase(channel);
    }
}

} // namespace IOHub