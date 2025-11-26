#include "ConfigLoader.h"
#include "DataFormatter.h"
#include "ProtocolUri.h"
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
    
    // 加载全局 input_hold_ms 配置
    if (ini_.has("default") && ini_["default"].has("input_hold_ms")) {
        try {
            inputHoldMs_ = std::stoi(ini_["default"]["input_hold_ms"]);
        } catch (const std::exception& e) {
            inputHoldMs_ = 1000; // 使用默认值
        }
    }
    
    // 加载全局 output_hold 配置
    if (ini_.has("default") && ini_["default"].has("output_hold")) {
        std::string value = ini_["default"]["output_hold"];
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        outputHold_ = (value == "true" || value == "1");
    }
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

bool ConfigLoader::loadOutputHold(uint8_t deviceIndex) {
    auto config = getMergedConfig(deviceIndex);
    
    if (config.find("output_hold") != config.end()) {
        std::string value = config["output_hold"];
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value == "true" || value == "1");
    }
    
    // 如果设备配置中没有，返回全局默认值
    return outputHold_;
}

int ConfigLoader::loadInputHoldMs(uint8_t deviceIndex) {
    auto config = getMergedConfig(deviceIndex);
    
    if (config.find("input_hold_ms") != config.end()) {
        try {
            return std::stoi(config["input_hold_ms"]);
        } catch (const std::exception& e) {
            // 解析失败，返回全局默认值
        }
    }
    
    // 如果设备配置中没有，返回全局默认值
    return inputHoldMs_;
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
    
    // 获取写入等待时间（0=不等待，适用于现代硬件）
    outWriteWaitMs = 0; // 默认不等待
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
                // 忽略解析错误
            }
        }
    }
    
    // 加载输出映射
    if (ini_.has("OutputMapping")) {
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
                // 忽略解析错误
            }
        }
    }
    
    return hasMapping;
}

bool ConfigLoader::loadFrameConfig(uint8_t deviceIndex, FrameConfig& frameConfig) {
    auto config = getMergedConfig(deviceIndex); // 使用设备索引加载配置
    
    // 判断是否真正配置了帧格式参数
    bool hasFrameConfig = false;
    
    if (config.count("frame_header")) {
        std::string headerStr = config["frame_header"];
        // 忽略空字符串和特殊禁用值
        if (!headerStr.empty() && headerStr != "none" && headerStr != "disabled") {
            try {
                auto bytes = DataFormatter::hexToBytes(headerStr);
                if (!bytes.empty()) {
                    frameConfig.header = bytes[0];
                    hasFrameConfig = true;  // 标记为已配置帧格式
                }
            } catch (...) {
                // 忽略解析错误
            }
        }
    }
    
    if (config.count("frame_tail")) {
        std::string tailStr = config["frame_tail"];
        // 忽略空字符串和特殊禁用值
        if (!tailStr.empty() && tailStr != "none" && tailStr != "disabled") {
            try {
                auto bytes = DataFormatter::hexToBytes(tailStr);
                if (!bytes.empty()) {
                    frameConfig.tail = bytes[0];
                    hasFrameConfig = true;  // 标记为已配置帧格式
                }
            } catch (...) {
                // 忽略解析错误
            }
        }
    }
    
    if (config.count("frame_length")) {
        std::string lengthStr = config["frame_length"];
        // 忽略空字符串和特殊禁用值
        if (!lengthStr.empty() && lengthStr != "none" && lengthStr != "disabled") {
            try {
                frameConfig.length = static_cast<size_t>(std::stoi(lengthStr));
                if (frameConfig.length > 0) {
                    hasFrameConfig = true;  // 标记为已配置帧格式
                }
            } catch (...) {
                // 忽略解析错误
            }
        }
    }
    
    if (config.count("channel_index")) {
        try {
            frameConfig.channelIndex = static_cast<uint8_t>(std::stoi(config["channel_index"]));
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    if (config.count("value_index")) {
        try {
            frameConfig.valueIndex = static_cast<uint8_t>(std::stoi(config["value_index"]));
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    if (config.count("input_hold_ms")) {
        try {
            frameConfig.timeoutMs = std::stoi(config["input_hold_ms"]);
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    if (config.count("channel_offset")) {
        try {
            frameConfig.channelOffset = std::stoi(config["channel_offset"]);
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    // 输出值编码配置
    if (config.count("output_value_on_code")) {
        try {
            auto bytes = DataFormatter::hexToBytes(config["output_value_on_code"]);
            if (!bytes.empty()) {
                frameConfig.outputValueOnCode = bytes[0];
            }
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    if (config.count("output_value_off_code")) {
        try {
            auto bytes = DataFormatter::hexToBytes(config["output_value_off_code"]);
            if (!bytes.empty()) {
                frameConfig.outputValueOffCode = bytes[0];
            }
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    // 输入值编码配置
    if (config.count("input_value_on_code")) {
        try {
            auto bytes = DataFormatter::hexToBytes(config["input_value_on_code"]);
            if (!bytes.empty()) {
                frameConfig.inputValueOnCode = bytes[0];
            }
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    if (config.count("input_value_off_code")) {
        try {
            auto bytes = DataFormatter::hexToBytes(config["input_value_off_code"]);
            if (!bytes.empty()) {
                frameConfig.inputValueOffCode = bytes[0];
            }
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    // 缓冲区大小配置
    if (config.count("max_buffer_size")) {
        try {
            frameConfig.maxBufferSize = static_cast<size_t>(std::stoi(config["max_buffer_size"]));
        } catch (...) {
            // 忽略解析错误
        }
    }
    
    // 只有真正配置了帧格式参数时才创建 FrameProcessor
    return hasFrameConfig;
}

} // namespace IOHub