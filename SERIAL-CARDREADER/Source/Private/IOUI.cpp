/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <algorithm>
#include "IOUI.h"
#include "Paths.hpp"
#include <utility>
#include <limits.h>
#include "windows.h"
#include "mIni/mini/ini.h"
#include <map>
#include <chrono>
#include "Serial.hpp"
#include "Util.hpp"
#include <iomanip>  // 用于格式化输出
#include <sstream>  // 用于字符串流
#include <memory>
#include <mutex>

namespace dh = DevelopHelper;

// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

// 串口参数结构体，支持每设备独立配置和状态缓存
struct DeviceContext
{
    std::shared_ptr<Serial> serialPort;

    int timeout = 500;
    int comboMode = 1;
    int dataBits = 11;
    std::string startBits = "20";

    std::vector<uint8> recvBuffer;

    std::map<uint8, std::chrono::system_clock::time_point> lastUpdateTime;
};

DeviceInfo devInfo;

// map键为设备索引，值为设备上下文
std::map<uint8, DeviceContext> g_deviceContexts;

// 全局组合键映射（组合键字符串 -> 通道id），共用，也可根据需要每设备独立
std::map<std::string, int> g_comboKeysMap;

mINI::INIFile* g_iniFIle = nullptr;
mINI::INIStructure* g_iniStructure = nullptr;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 255;
    devInfo.OutputCount = 0;
    devInfo.AxisCount = 0;
    return &devInfo;
}

std::string convert_char(unsigned char ch, int mode) {
    if (mode == 0) {
        // mode 0: 返回ASCII字符
        if (ch >= 32 && ch <= 126) {
            return std::string(1, ch);  // 将字符转换为长度为1的std::string
        }
        else {
            return ".";
        }
    }
    else if (mode == 1) {
        // mode 1: 返回16进制字符串
        std::ostringstream oss;
        oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
        return oss.str();  // 返回格式化的16进制字符串
    }
    else {
        // 非法模式
        return "";
    }
}

// 将 unsigned char 数组转换为16进制字符串
std::string to_hex_string(const unsigned char* data, size_t length) {
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(data[i]);
    }
    return oss.str();
}

// 判断数组的16进制表示是否以指定的字符串开头
bool starts_with_hex(const unsigned char* data, size_t length, const std::string& startBitsHex)
{
    // 假设startBitsHex为"20"表示0x20
    // 先把startBitsHex转字节
    static std::vector<unsigned char> startBytes;
    if (startBytes.empty()) {
        for (size_t i = 0; i < startBitsHex.length(); i += 2) {
            std::string byteString = startBitsHex.substr(i, 2);
            unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
            startBytes.push_back(byte);
        }
    }
    if (length < startBytes.size()) return false;
    return std::equal(startBytes.begin(), startBytes.end(), data);
}

// 将前count个字节转换为指定格式的字符串，并用空格分隔
std::string vector_to_hex_string(const std::vector<unsigned char>& data, size_t count = 11, int mode = 1) {
    std::ostringstream oss;
    size_t length = std::min<size_t>(data.size(), count);
    for (size_t i = 0; i < length; ++i) {
        oss << convert_char(data[i], mode);
        if (i < length - 1) {
            oss << " ";
        }
    }
    return oss.str();
}

int AppendComboKeys(const std::string& comboKey) 
{
    if (g_comboKeysMap.count(comboKey) > 0) return -1;

    if (!g_iniStructure || !g_iniFIle) return -1; // 防护，避免空指针操作

    auto& ini = *g_iniStructure;
    auto& file = *g_iniFIle;
    for (int idx = 0; idx < devInfo.InputCount; ++idx) {
        std::string key = "k" + std::to_string(idx);
        if (!ini["ComboKeys"].has(key)) {
            ini["ComboKeys"][key] = comboKey;
            g_comboKeysMap.insert(std::pair<std::string, int>(comboKey, idx));
            file.write(ini);
            return idx;
        }
    }
    return -1;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    std::string path = dh::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\SERIAL-CARDREADER\\config.ini";

    try {
        if (!g_iniFIle) g_iniFIle = new mINI::INIFile(config_file_path);
        if (!g_iniStructure) g_iniStructure = new mINI::INIStructure();
        g_iniFIle->read(*g_iniStructure);
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

        // 读取配置参数，带默认值fallback
        int baudRate = 9600;
        int timeout = 500;
        int comboMode = 1;
        int dataBits = 11;
        std::string startBits = "20";
        std::string portName;

        if (mergedConfig.count("baud_rate")) baudRate = std::stoi(mergedConfig["baud_rate"]);
        if (mergedConfig.count("timeout")) timeout = std::stoi(mergedConfig["timeout"]);
        if (mergedConfig.count("combo_mode")) comboMode = std::stoi(mergedConfig["combo_mode"]);
        if (mergedConfig.count("frame_length")) dataBits = std::stoi(mergedConfig["frame_length"]);
        if (mergedConfig.count("start_bits")) startBits = mergedConfig["start_bits"];
        if (mergedConfig.count("port_name")) portName = mergedConfig["port_name"];

        // 如果portName为空，使用默认的COM加设备号
        portName = NormalizePortName(portName, deviceIndex);

        // 初始化设备上下文
        DeviceContext ctx;
        ctx.timeout = timeout;
        ctx.comboMode = comboMode;
        ctx.dataBits = dataBits;
        ctx.startBits = startBits;

        // 创建串口对象
        auto serialPortPtr = std::make_shared<Serial>(portName, baudRate);
        ctx.serialPort = serialPortPtr;

        // 更新全局map
        g_deviceContexts[deviceIndex] = std::move(ctx);

        // 载入 ComboKeys 节，初始化映射（此处共用一个映射，非逐设备）
        // 如果需要支持设备独立映射，则需改为每设备维护
        g_comboKeysMap.clear();
        auto& comboKeysSection = ini["ComboKeys"];
        for (int idx = 0; idx < devInfo.InputCount; ++idx) {
            std::string keyName = "k" + std::to_string(idx);
            if (comboKeysSection.has(keyName)) {
                std::string comboKey = comboKeysSection[keyName];
                if (!comboKey.empty()) {
                    g_comboKeysMap[comboKey] = idx;
                }
            }
        }
    }
    catch (...) {
        if (g_iniFIle) { delete g_iniFIle; g_iniFIle = nullptr; }
        if (g_iniStructure) { delete g_iniStructure; g_iniStructure = nullptr; }
        return 0;
    }

    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    if (g_deviceContexts.count(deviceIndex)) {
        auto& ctx = g_deviceContexts[deviceIndex];
        if (ctx.serialPort) {
            ctx.serialPort->flush();
            ctx.serialPort.reset();
        }
        g_deviceContexts.erase(deviceIndex);
    }
    delete g_iniFIle;
    g_iniFIle = nullptr;
    delete g_iniStructure;
    g_iniStructure = nullptr;

    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    if (g_deviceContexts.count(deviceIndex) == 0) {
        // 设备未打开或不存在
        ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
        return 0;
    }

    auto& ctx = g_deviceContexts[deviceIndex];
    auto& serialPort = ctx.serialPort;

    if (!serialPort) {
        ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
        return 0;
    }

    static std::map<uint8, std::vector<uint8>> allDevData;
    static std::map<uint8, std::map<uint8, std::chrono::system_clock::time_point>> allDiLastUpdateTime;

    if (allDevData.count(deviceIndex) == 0) {
        allDevData[deviceIndex] = std::vector<uint8>();
    }
    if (allDiLastUpdateTime.count(deviceIndex) == 0) {
        allDiLastUpdateTime[deviceIndex] = std::map<uint8, std::chrono::system_clock::time_point>();
    }

    // 串口读取缓冲
    char tempRecv[256]; // 缓冲大小调整为合理值
    const int bufferSize = sizeof(tempRecv);
    int recvCount = serialPort->read(tempRecv, bufferSize, false);
    auto& devData = allDevData[deviceIndex];

    if (recvCount > 0) {
        devData.insert(devData.end(), (uint8*)tempRecv, (uint8*)tempRecv + recvCount);
    }

    auto& diLastUpdateTime = allDiLastUpdateTime[deviceIndex];

    while (devData.size() >= (size_t)ctx.dataBits) {
        if (!starts_with_hex(devData.data(), devData.size(), ctx.startBits)) {
            devData.erase(devData.begin());
            continue;
        }
        g_cachedComboKeys.push_back(vector_to_hex_string(devData, ctx.dataBits, ctx.comboMode));
        devData.erase(devData.begin(), devData.begin() + ctx.dataBits);
    }

    if (ctx.timeout <= 0) {
        ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
    }

    for (const std::string& comboKey : g_cachedComboKeys) {
        if (g_comboKeysMap.count(comboKey) <= 0)
            AppendComboKeys(comboKey);
        if (g_comboKeysMap.count(comboKey) <= 0) continue;
        auto channel = g_comboKeysMap[comboKey];
        OutDIStatus[channel] = 1;
        if (ctx.timeout <= 0) continue;

        if (diLastUpdateTime.count(channel) <= 0) {
            diLastUpdateTime[channel] = std::chrono::system_clock::now();
        }
        diLastUpdateTime[channel] = std::chrono::system_clock::now();
    }

    if (ctx.timeout >= 50) {
        auto now = std::chrono::system_clock::now();
        for (int idx = 0; idx < devInfo.InputCount; ++idx) {
            if (OutDIStatus[idx] > 0) {
                if (diLastUpdateTime.count(idx) == 0) continue;
                std::chrono::duration<double, std::milli> elapsed = now - diLastUpdateTime[idx];
                if (elapsed.count() >= ctx.timeout) {
                    OutDIStatus[idx] = 0;
                }
            }
        }
    }
    g_cachedComboKeys.clear();

    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}
