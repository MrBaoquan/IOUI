/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <unordered_map>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Serial.hpp"
#include "Util.hpp"
#include "mIni/mini/ini.h"

namespace dh = DevelopHelper;

DeviceInfo devInfo;

struct DeviceContext {
    Serial* serialPort = nullptr;
    std::thread deviceThread;
    std::atomic<bool> stopThread{ false };
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::queue<std::map<int, short>> dirtyQueue;
    std::vector<short> lastDOStatus;
    int waitMs = 60;
};

static std::mutex g_devicesMutex;
static std::unordered_map<uint8_t, DeviceContext*> g_deviceMap;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 128;
    devInfo.OutputCount = 128;
    devInfo.AxisCount = 128;
    return &devInfo;
}

static void processDirtyStatus(DeviceContext* ctx)
{
    while (!ctx->stopThread) {
        std::map<int, short> _dirtyDOStatus;
        {
            std::unique_lock<std::mutex> lock(ctx->queueMutex);
            ctx->queueCV.wait(lock, [ctx] { return !ctx->dirtyQueue.empty() || ctx->stopThread; });
            if (ctx->stopThread && ctx->dirtyQueue.empty()) {
                return; // 退出线程
            }
            _dirtyDOStatus = ctx->dirtyQueue.front();
            ctx->dirtyQueue.pop();
        }

        for (auto& _doItem : _dirtyDOStatus) {
            char _data[4] = {};
            _data[0] = 0xAA;
            _data[1] = static_cast<char>(_doItem.first + 1);  // 通道号从1开始
            _data[2] = (_doItem.second == 0) ? 0x02 : 0x01;   // 0->低电平 其他->高电平
            _data[3] = 0x55;

            if (ctx->serialPort) {
                ctx->serialPort->write(_data, 4);
            }

            if (ctx->waitMs > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ctx->waitMs));
            }
        }
    }
}

std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

IOUI_API int __stdcall OpenDevice(uint8_t deviceIndex)
{
    std::lock_guard<std::mutex> lock(g_devicesMutex);
    if (g_deviceMap.count(deviceIndex)) {
        // 设备已打开
        return 1;
    }

    std::string path = dh::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\SERIAL-TDDZ\\config.ini";

    try {
        if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
        if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
        g_iniFile->read(*g_iniStructure);
        auto& ini = *g_iniStructure;

        // 1. 构造设备节名，如 device_1
        const auto& deviceSectionName = BuildDeviceAttribute("device", deviceIndex);

        // 2. 取default节点
        auto& defaultSection = ini["default"];

        // 3. 拷贝default节点配置到defaultConfig（以便后续覆盖）
        std::map<std::string, std::string> mergedConfig; // “merged”配置
        for (auto& kv : defaultSection) {
            mergedConfig[kv.first] = kv.second;
        }

        // 4. 取设备节点配置覆盖defaultConfig
        auto& deviceSectionMap = ini[deviceSectionName];
        for (auto& kv : deviceSectionMap) {
            mergedConfig[kv.first] = kv.second;
        }

        // 5. 从mergedConfig读取参数，缺省值写死或根据需要调整
        int baudRate = 57600;
        int waitTimeMs = 60;

        if (mergedConfig.count("baud_rate")) baudRate = std::stoi(mergedConfig["baud_rate"]);
        if (mergedConfig.count("write_wait_ms")) waitTimeMs = std::stoi(mergedConfig["write_wait_ms"]);

        // 这里从配置中读取port_name，如果没有则默认使用之前的规则
        // 读取串口名，默认按原规则处理
        std::string portName;
        if (mergedConfig.count("port_name")) portName = mergedConfig["port_name"];
        portName = NormalizePortName(mergedConfig["port_name"], deviceIndex);
       
        // 7. 创建设备上下文
        DeviceContext* ctx = new DeviceContext;
        ctx->lastDOStatus.resize(devInfo.OutputCount, 0);
        ctx->waitMs = waitTimeMs;

        try {
            ctx->serialPort = new Serial(portName, baudRate);
        }
        catch (const char* /*err*/) {
            delete ctx;
            return 0; // 打开失败
        }

        try {
            ctx->deviceThread = std::thread(processDirtyStatus, ctx);
        }
        catch (...) {
            if (ctx->serialPort) {
                delete ctx->serialPort;
            }
            delete ctx;
            return 0;
        }

        g_deviceMap[deviceIndex] = ctx;
        return 1;
    }
    catch (...) {
        // 配置文件加载、解析异常等错误
        return 0;
    }
}

IOUI_API int __stdcall CloseDevice(uint8_t deviceIndex)
{
    std::lock_guard<std::mutex> lock(g_devicesMutex);
    auto it = g_deviceMap.find(deviceIndex);
    if (it == g_deviceMap.end()) {
        return 0; // 设备未打开
    }

    DeviceContext* ctx = it->second;

    ctx->stopThread = true;
    ctx->queueCV.notify_one();

    if (ctx->deviceThread.joinable()) {
        ctx->deviceThread.join();
    }

    if (ctx->serialPort) {
        ctx->serialPort->flush();
        delete ctx->serialPort;
        ctx->serialPort = nullptr;
    }

    delete ctx;
    g_deviceMap.erase(it);

    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8_t deviceIndex, short* InDOStatus)
{
    std::lock_guard<std::mutex> lock(g_devicesMutex);
    auto it = g_deviceMap.find(deviceIndex);
    if (it == g_deviceMap.end()) {
        return 0; // 设备未打开
    }

    DeviceContext* ctx = it->second;

    std::map<int, short> dirtyDOStatus;
    for (size_t i = 0; i < ctx->lastDOStatus.size(); ++i) {
        if (ctx->lastDOStatus[i] != InDOStatus[i]) {
            dirtyDOStatus[(int)i] = InDOStatus[i];
            ctx->lastDOStatus[i] = InDOStatus[i];
        }
    }

    if (!dirtyDOStatus.empty()) {
        {
            std::lock_guard<std::mutex> lockQ(ctx->queueMutex);
            ctx->dirtyQueue.push(dirtyDOStatus);
        }
        ctx->queueCV.notify_one();
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8_t deviceIndex, BYTE* OutDIStatus)
{
    std::lock_guard<std::mutex> lock(g_devicesMutex);
    auto it = g_deviceMap.find(deviceIndex);
    if (it == g_deviceMap.end() || it->second->serialPort == nullptr) {
        return 0; // 设备未打开或无串口
    }

    DeviceContext* ctx = it->second;

    // 接收缓存每设备单独维护
    static std::unordered_map<uint8_t, std::vector<uint8_t>> recvBuffers;

    auto& recvBuffer = recvBuffers[deviceIndex];

    char tempBuffer[MAX_PATH];
    int bytesRead = ctx->serialPort->read(tempBuffer, MAX_PATH, false);
    for (int i = 0; i < bytesRead; ++i) {
        recvBuffer.push_back(static_cast<uint8_t>(tempBuffer[i]));
    }

    while (!recvBuffer.empty()) {
        // 找起始标志 0xAA
        auto startIt = std::find(recvBuffer.begin(), recvBuffer.end(), 0xAA);
        if (startIt == recvBuffer.end()) {
            recvBuffer.clear();
            break;
        }
        // 找结束标志 0x55
        auto endIt = std::find(startIt + 1, recvBuffer.end(), 0x55);
        if (endIt == recvBuffer.end()) {
            // 不完整帧，裁剪起始前数据
            if (startIt != recvBuffer.begin()) {
                recvBuffer.erase(recvBuffer.begin(), startIt);
            }
            break;
        }

        size_t frameLen = std::distance(startIt, endIt) + 1;
        if (frameLen <= 4) {
            // 异常帧，去掉起始标志再继续
            recvBuffer.erase(startIt);
            continue;
        }

        size_t dataLen = frameLen - 2; // 减去AA和55

        size_t maxChannels = devInfo.InputCount;
        size_t channelsToUpdate = (dataLen < maxChannels) ? dataLen : maxChannels;

        auto dataIt = startIt + 1;
        for (size_t ch = 0; ch < channelsToUpdate; ++ch) {
            uint8_t value = *(dataIt + ch);
            OutDIStatus[ch] = (value == 0) ? 0 : 1;
        }

        recvBuffer.erase(recvBuffer.begin(), endIt + 1);
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8_t deviceIndex, short* OutDOStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8_t deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8_t deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}
