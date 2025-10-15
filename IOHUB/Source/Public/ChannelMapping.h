#pragma once
#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include "DataFormatter.h"

namespace IOHub {

// 映射项
struct MappingItem {
    DataFormat format;
    std::vector<uint8_t> data;
    std::string displayStr;
    
    MappingItem() : format(DataFormat::AUTO) {}
    MappingItem(DataFormat fmt, const std::vector<uint8_t>& d, const std::string& str = "")
        : format(fmt), data(d), displayStr(str) {}
};

// 通道映射管理器
class ChannelMapping {
public:
    ChannelMapping() = default;
    
    // 输入映射：添加 数据->通道 的映射
    void addInputMapping(uint8_t channel, const MappingItem& item);
    
    // 输出映射：添加 通道->数据 的映射
    void addOutputMapping(uint8_t channel, const MappingItem& item);
    
    // 查找输入通道（根据接收到的数据）
    bool findInputChannel(const std::vector<uint8_t>& recvData, uint8_t& outChannel) const;
    
    // 查找输出数据（根据通道号）
    bool findOutputData(uint8_t channel, MappingItem& outItem) const;
    
    // 清空所有映射
    void clear();
    
    // 获取所有输入映射（用于日志）
    const std::map<std::vector<uint8_t>, uint8_t>& getInputMappings() const { return inputMap_; }
    
    // 获取所有输出映射（用于日志）
    const std::map<uint8_t, MappingItem>& getOutputMappings() const { return outputMap_; }

private:
    std::map<std::vector<uint8_t>, uint8_t> inputMap_;  // 数据 -> 通道
    std::map<uint8_t, MappingItem> outputMap_;           // 通道 -> 数据
};

} // namespace IOHub