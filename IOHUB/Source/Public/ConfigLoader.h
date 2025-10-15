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
    
    // 加载帧配置（串口专用）
    bool loadFrameConfig(FrameConfig& config);
    
    int getInputHoldMs() const { return inputHoldMs_; }
    
    // 刷新配置文件
    void reload();

private:
    mINI::INIFile file_;
    mINI::INIStructure ini_;
    std::string configPath_;
    int inputHoldMs_{1000};  // 默认1秒（0=永久保持）

    std::map<std::string, std::string> getMergedConfig(uint8_t deviceIndex);
};

} // namespace IOHub