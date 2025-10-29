#pragma once
#include <string>
#include <map>
#include <memory>
#include "mIni/mini/ini.h"
#include "ProtocolFactory.h"
#include "ChannelMapping.h"
#include "FrameProcessor.h"

namespace IOHub {

// 配置加载器
class ConfigLoader {
public:
    explicit ConfigLoader(const std::string& configFilePath);
    
    // 加载设备配置
    bool loadDeviceConfig(uint8_t deviceIndex, 
                         std::unique_ptr<ProtocolConfig>& outProtocolConfig,
                         int& outWriteWaitMs);
    
    // 加载通道映射
    bool loadChannelMapping(ChannelMapping& mapping, DataFormat& defaultFormat);
    
    // 加载帧配置（支持所有协议，按设备索引加载）
    bool loadFrameConfig(uint8_t deviceIndex, FrameConfig& config);
    
    // 加载设备级别的 output_hold 配置
    bool loadOutputHold(uint8_t deviceIndex);
    
    // 加载设备级别的 input_hold_ms 配置
    int loadInputHoldMs(uint8_t deviceIndex);
    
    int getInputHoldMs() const { return inputHoldMs_; }
    bool getOutputHold() const { return outputHold_; }
    
    // 刷新配置文件
    void reload();

private:
    mINI::INIFile file_;
    mINI::INIStructure ini_;
    std::string configPath_;
    int inputHoldMs_{1000};  // 默认1秒（0=永久保持）
    bool outputHold_{false}; // 默认不保持输出状态（读取后重置）

    std::map<std::string, std::string> getMergedConfig(uint8_t deviceIndex);
};

} // namespace IOHub