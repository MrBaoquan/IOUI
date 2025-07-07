#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <memory>
#include "IOUI.h"
#include "snap7.h"
#include "Paths.hpp"
#include "mIni/mini/ini.h"
#include "Util.hpp"

#pragma comment(lib, "snap7.lib")

DeviceInfo devInfo;
std::map<uint8, std::shared_ptr<TS7Client>> clientMap; // 使用 std::map 管理多个 TS7Client 实例

IOUI_API DeviceInfo* __stdcall Initialize() {
    devInfo.InputCount = 64;
    devInfo.OutputCount = 64;
    devInfo.AxisCount = 0;
    return &devInfo;
}

std::shared_ptr<mINI::INIFile> g_iniFile;
std::shared_ptr<mINI::INIStructure> g_iniStructure;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex) {
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\snap7.dll";
    auto _module = LoadLibraryA(_path.data());

    if (!_module) {
        return 0; // 加载库失败
    }

    // 创建并存储 TS7Client 实例
    auto client = std::make_shared<TS7Client>();
    if (g_iniFile == nullptr || g_iniStructure == nullptr) {
        std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
        std::string config_file_path = path + "Config\\SNAP7\\config.ini";

        g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
        g_iniStructure = std::make_shared<mINI::INIStructure>();
        g_iniFile->read(*g_iniStructure);
    }
    auto& ini = *g_iniStructure;
    const std::string deviceSectionName = BuildDeviceAttribute("device", deviceIndex);

    // 读取default节配置
    auto& defaultSection = ini["default"];
    std::map<std::string, std::string> mergedConfig;
    for (auto& kv : defaultSection) {
        mergedConfig[kv.first] = kv.second;
    }

    // 设备专属节覆盖default
    auto& deviceSection = ini[deviceSectionName];
    for (auto& kv : deviceSection) {
        mergedConfig[kv.first] = kv.second;
    }

    // 默认值
    std::string ip = "192.168.2.1"; // 默认 IP 地址
    int rack = 0;                   // 默认 RACK
    int slot = 1;                   // 默认 SLOT

    // 检查配置文件并读取值
    if (mergedConfig.count("ip")) ip = mergedConfig["ip"];
    if (mergedConfig.count("rack")) rack = std::stoi(mergedConfig["rack"]);
    if (mergedConfig.count("slot")) slot = std::stoi(mergedConfig["slot"]);
    
    // 尝试连接到 PLC
    if (client->ConnectTo(ip.c_str(), rack, slot) != 0) {
        return 0; // 连接失败
    }

    clientMap[deviceIndex] = client; // 将 client 存储到 map 中

    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex) {
    auto it = clientMap.find(deviceIndex);
    if (it != clientMap.end()) {
        it->second->Disconnect();
        clientMap.erase(it); // 移除 map 中的特定 client
    }
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus) {
    auto it = clientMap.find(deviceIndex);

    const int DO_Bytes = 8;
    static std::vector<BYTE> tempBuffer(DO_Bytes,0);
    std::fill_n(tempBuffer.begin(), DO_Bytes, 0);

    for (size_t _byte = 0; _byte < DO_Bytes; _byte++) {
        BYTE& data = tempBuffer[_byte];
        for (size_t _bit = 0; _bit < 8; _bit++) {
			int ch = _byte * 8 + _bit;
            int val = InDOStatus[ch] > 0 ? 1 : 0;
			data |= val << _bit;
        }
    }

    if (it != clientMap.end() && it->second->WriteArea(S7AreaDB, 1, 10, DO_Bytes, S7WLByte, tempBuffer.data()) != 0) {
		return 0;
	}

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus) {
    auto it = clientMap.find(deviceIndex);
    
    const int DO_Bytes = 8;
    static std::vector<BYTE> tempBuffer(DO_Bytes,0);

    if (it != clientMap.end() && it->second->ReadArea(S7AreaDB, 1, 10, DO_Bytes, S7WLByte, tempBuffer.data()) != 0) {
        return 0;
    }

    for (size_t _byte = 0; _byte < DO_Bytes; _byte++)
    {
		const BYTE& data = tempBuffer[_byte];
        for (size_t _bit = 0; _bit < 8; _bit++)
        {
			int ch = _byte * 8 + _bit;
			OutDOStatus[ch] = (data & (1 << _bit));
		}
	}
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus) {
    auto it = clientMap.find(deviceIndex);

    const size_t DI_Bytes = 8;
    static std::vector<BYTE> tempBuffer(DI_Bytes,0);

    if (it->second->ReadArea(S7AreaPE, 1, 0, DI_Bytes, S7WLByte, tempBuffer.data()) != 0) {
        return 0;
    }

    for (size_t _byte = 0; _byte < DI_Bytes; _byte++)
    {
        const BYTE& data = tempBuffer[_byte];
        for (size_t _bit = 0; _bit < 8; _bit++)
        {
            int ch = _byte * 8 + _bit;
            OutDIStatus[ch] = (data & (1 << _bit)) ? 1 : 0;
        }
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus) {
    return 1; // 保留功能，没有具体实现
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
    return 1; // 保留功能，没有具体实现
}
