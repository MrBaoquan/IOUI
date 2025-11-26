#include "ChannelMapping.h"
#include <algorithm>
#include <cctype>

namespace IOHub {

void ChannelMapping::addInputMapping(uint8_t channel, const MappingItem& item) {
    inputMap_[item.data] = channel;
}

void ChannelMapping::addOutputMapping(uint8_t channel, const MappingItem& item) {
    outputMap_[channel] = item;
}

bool ChannelMapping::findInputChannel(const std::vector<uint8_t>& recvData, uint8_t& outChannel) const {
    // 1. 首先尝试精确匹配
    auto it = inputMap_.find(recvData);
    if (it != inputMap_.end()) {
        outChannel = it->second;
        return true;
    }
    
    // 2. 如果精确匹配失败，尝试模糊匹配
    for (const auto& mapping : inputMap_) {
        const auto& configData = mapping.first;
        const auto& channel = mapping.second;
        
        // 尝试在接收数据中查找配置的数据模式
        if (recvData.size() >= configData.size()) {
            // 检查是否包含配置的数据（支持数据包含多个内容的情况）
            auto pos = std::search(recvData.begin(), recvData.end(),
                                 configData.begin(), configData.end());
            if (pos != recvData.end()) {
                outChannel = channel;
                return true;
            }
        }
        
        // 对于ASCII数据，尝试大小写不敏感匹配
        if (configData.size() == recvData.size()) {
            bool match = true;
            for (size_t i = 0; i < configData.size(); ++i) {
                char configChar = static_cast<char>(configData[i]);
                char recvChar = static_cast<char>(recvData[i]);
                if (std::tolower(configChar) != std::tolower(recvChar)) {
                    match = false;
                    break;
                }
            }
            if (match) {
                outChannel = channel;
                return true;
            }
        }
    }
    
    return false;
}

bool ChannelMapping::findOutputData(uint8_t channel, MappingItem& outItem) const {
    auto it = outputMap_.find(channel);
    if (it != outputMap_.end()) {
        outItem = it->second;
        return true;
    }
    return false;
}

void ChannelMapping::clear() {
    inputMap_.clear();
    outputMap_.clear();
}

} // namespace IOHub