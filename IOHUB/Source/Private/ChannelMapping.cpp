#include "ChannelMapping.h"

namespace IOHub {

void ChannelMapping::addInputMapping(uint8_t channel, const MappingItem& item) {
    inputMap_[item.data] = channel;
}

void ChannelMapping::addOutputMapping(uint8_t channel, const MappingItem& item) {
    outputMap_[channel] = item;
}

bool ChannelMapping::findInputChannel(const std::vector<uint8_t>& recvData, uint8_t& outChannel) const {
    auto it = inputMap_.find(recvData);
    if (it != inputMap_.end()) {
        outChannel = it->second;
        return true;
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