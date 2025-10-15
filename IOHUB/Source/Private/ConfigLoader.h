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
    bool loadChannelMapping(ChannelMapping& mapping, DataFormat defaultFormat);
    
    // 加载帧配置（串口专用）
    bool loadFrameConfig(FrameConfig& frameConfig);
    
    // 刷新配置文件
    void reload();

private:
    std::string configPath_;
    std::shared_ptr<mINI::INIStructure> ini_;
    
    std::map<std::string, std::string> getMergedConfig(uint8_t deviceIndex);
};

} // namespace IOHub