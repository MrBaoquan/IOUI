#include "ConfigLoader.h"
#include "DataFormatter.h"
#include "ProtocolUri.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace IOHub {

ConfigLoader::ConfigLoader(const std::string& configFilePath)
    : configPath_(configFilePath)
    , file_(configFilePath)
{
    reload();
}

void ConfigLoader::reload() {
    file_.read(ini_);
}

std::map<std::string, std::string> ConfigLoader::getMergedConfig(uint8_t deviceIndex) {
    std::map<std::string, std::string> merged;
    
    // 先加载默认配置
    if (ini_.has("default")) {
        for (const auto& kv : ini_["default"]) {
            merged[kv.first] = kv.second;
        }
    }
    
    // 再加载设备特定配置（覆盖默认值）
    std::string deviceSection = "device_" + std::to_string(deviceIndex);
    if (ini_.has(deviceSection)) {
        for (const auto& kv : ini_[deviceSection]) {
            merged[kv.first] = kv.second;
        }
    }
    
    return merged;
}

bool ConfigLoader::loadDeviceConfig(uint8_t deviceIndex, 
                                   std::unique_ptr<ProtocolConfig>& outProtocolConfig,
                                   int& outWriteWaitMs) {
    auto config = getMergedConfig(deviceIndex);
    
    // 必须有 protocol 配置（可以从 default 继承）
    if (!config.count("protocol")) {
        return false;
    }
    
    std::string uri = config["protocol"];
    
    if (!ProtocolUri::parse(uri, outProtocolConfig)) {
        return false;
    }
    
    // 获取写入等待时间
    outWriteWaitMs = 60; // 默认值
    if (config.count("write_wait_ms")) {
        try {
            outWriteWaitMs = std::stoi(config["write_wait_ms"]);
        } catch (const std::exception& e) {
            // Use default value
        }
    }
    
    return true;
}

bool ConfigLoader::loadChannelMapping(ChannelMapping& mapping, DataFormat& defaultFormat) {
    mapping.clear();
    
    bool hasMapping = false;
    
    // 加载输入映射
    if (ini_.has("InputMapping")) {
        std::cout << "[ConfigLoader] Loading InputMapping..." << std::endl;
        for (const auto& kv : ini_["InputMapping"]) {
            try {
                uint8_t channel = static_cast<uint8_t>(std::stoi(kv.first));
                auto data = DataFormatter::parseFromConfig(kv.second, defaultFormat);
                auto format = DataFormatter::detectFormat(kv.second);
                auto displayStr = DataFormatter::toDisplayString(data, format);
                
                MappingItem item(format, data, displayStr);
                mapping.addInputMapping(channel, item);
                hasMapping = true;
            } catch (const std::exception& e) {
                std::cout << "[ConfigLoader] Error parsing input mapping: " 
                          << kv.first << "=" << kv.second << ", " << e.what() << std::endl;
            }
        }
    }
    
    // 加载输出映射
    if (ini_.has("OutputMapping")) {
        std::cout << "[ConfigLoader] Loading OutputMapping..." << std::endl;
        for (const auto& kv : ini_["OutputMapping"]) {
            try {
                uint8_t channel = static_cast<uint8_t>(std::stoi(kv.first));
                auto data = DataFormatter::parseFromConfig(kv.second, defaultFormat);
                auto format = DataFormatter::detectFormat(kv.second);
                auto displayStr = DataFormatter::toDisplayString(data, format);
                
                MappingItem item(format, data, displayStr);
                mapping.addOutputMapping(channel, item);
                hasMapping = true;
            } catch (const std::exception& e) {
                std::cout << "[ConfigLoader] Error parsing output mapping: " 
                          << kv.first << "=" << kv.second << ", " << e.what() << std::endl;
            }
        }
    }
    
    if (hasMapping) {
        std::cout << "[ConfigLoader] Channel mapping loaded successfully" << std::endl;
    } else {
        std::cout << "[ConfigLoader] No channel mapping configured" << std::endl;
    }
    
    return hasMapping;
}

bool ConfigLoader::loadFrameConfig(FrameConfig& frameConfig) {
    auto config = getMergedConfig(0); // 帧配置使用default配置
    
    if (config.count("frame_header")) {
        try {
            auto bytes = DataFormatter::hexToBytes(config["frame_header"]);
            if (!bytes.empty()) {
                frameConfig.header = bytes[0];
            }
        } catch (...) {
            std::cout << "[ConfigLoader] Error parsing frame_header" << std::endl;
        }
    }
    
    if (config.count("frame_tail")) {
        try {
            auto bytes = DataFormatter::hexToBytes(config["frame_tail"]);
            if (!bytes.empty()) {
                frameConfig.tail = bytes[0];
            }
        } catch (...) {
            std::cout << "[ConfigLoader] Error parsing frame_tail" << std::endl;
        }
    }
    
    if (config.count("frame_length")) {
        try {
            frameConfig.length = static_cast<size_t>(std::stoi(config["frame_length"]));
        } catch (...) {
            std::cout << "[ConfigLoader] Error parsing frame_length" << std::endl;
        }
    }
    
    if (config.count("channel_index")) {
        try {
            frameConfig.channelIndex = static_cast<uint8_t>(std::stoi(config["channel_index"]));
        } catch (...) {
            std::cout << "[ConfigLoader] Error parsing channel_index" << std::endl;
        }
    }
    
    if (config.count("value_index")) {
        try {
            frameConfig.valueIndex = static_cast<uint8_t>(std::stoi(config["value_index"]));
        } catch (...) {
            std::cout << "[ConfigLoader] Error parsing value_index" << std::endl;
        }
    }
    
    if (config.count("input_timeout_ms")) {
        try {
            frameConfig.timeoutMs = std::stoi(config["input_timeout_ms"]);
        } catch (...) {
            std::cout << "[ConfigLoader] Error parsing input_timeout_ms" << std::endl;
        }
    }
    
    std::cout << "[ConfigLoader] Frame config: header=0x" << std::hex << (int)frameConfig.header
              << ", tail=0x" << (int)frameConfig.tail
              << ", length=" << std::dec << frameConfig.length 
              << ", timeout=" << frameConfig.timeoutMs << "ms" << std::endl;
    
    return true;
}

} // namespace IOHub