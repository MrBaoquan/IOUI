#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <algorithm>
#include <deque>
#include <cctype>
#include <cstdio>

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Serial.hpp"
#include "Util.hpp"
#include "mIni/mini/ini.h"

namespace dh = DevelopHelper;

DeviceInfo devInfo;
constexpr size_t MAX_QUEUE_SIZE = 100;

class DeviceContext {
public:
    explicit DeviceContext(uint8 deviceIndex)
        : deviceIndex(deviceIndex), stopThread(false),
        waitTimeMs(60), frameHeader(0xFE), frameTail(0xFF),
        chIndex(1), valIndex(2), frameLength(4),
        inputChannelTimeoutMs(1000)
    {
        lastDOStatus.resize(devInfo.OutputCount, 0);
    }

    ~DeviceContext() {
        stop();
    }

    void startThread(std::function<void(uint8)> fn) {
        stopThread.store(false);
        deviceThread = std::thread(fn, deviceIndex);
    }

    void stop() {
        stopThread.store(true);
        queueCV.notify_all();
        if (deviceThread.joinable())
            deviceThread.join();
        if (serialPort)
            serialPort->flush();
    }

    uint8 deviceIndex;
    std::unique_ptr<Serial> serialPort;

    std::atomic<bool> stopThread;

    std::queue<std::map<int, short>> dirtyQueues;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    int waitTimeMs;

    std::vector<short> lastDOStatus;

    uint8 frameHeader;
    uint8 frameTail;
    uint8 chIndex;
    uint8 valIndex;
    size_t frameLength;

    // 输入映射帧数据，key=输入通道号，value=帧字节序列
    std::map<uint8, std::vector<uint8>> inputFrameMap;

    // 输入通道上次更新时间，key=输入通道号，value=时间点
    std::map<uint8, std::chrono::steady_clock::time_point> inputChannelLastUpdateTime;

    // 输入通道超时时间（毫秒）
    int inputChannelTimeoutMs;

    // 输出映射帧数据，key=输出通道号，value=完整帧字节序列
    std::map<uint8, std::vector<uint8>> outputFrameMap;

    void updateInputChannelTimestamp(uint8 ch) {
        inputChannelLastUpdateTime[ch] = std::chrono::steady_clock::now();
    }

    void checkInputTimeouts(BYTE* OutDIStatus) {
        auto now = std::chrono::steady_clock::now();
        for (const auto& kv : inputChannelLastUpdateTime) {
            uint8 ch = kv.first;
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - kv.second).count();
            if (durationMs > inputChannelTimeoutMs) {
                if (OutDIStatus[ch] != 0) {
                    OutDIStatus[ch] = 0;
                    // 可添加日志/事件
                }
            }
        }
    }

private:
    std::thread deviceThread;
};

std::map<uint8, std::unique_ptr<DeviceContext>> g_deviceContexts;
std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

// 去除字符串中所有空白字符（空格、制表符等）
static std::string RemoveSpaces(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char ch : str) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            result.push_back(ch);
        }
    }
    return result;
}

void LoadFrameMap(mINI::INIStructure& ini, const std::string& sectionName,
    std::map<uint8, std::vector<uint8>>& outMap, size_t maxChannelCount)
{
    auto& mapSection = ini[sectionName];
    for (const auto& kv : mapSection) {
        try {
            uint8 channel = static_cast<uint8>(std::stoi(kv.first));
            if (channel >= maxChannelCount)
                continue;
            // 去掉空白字符
            std::string hexStrNoSpaces = RemoveSpaces(kv.second);

            if ((hexStrNoSpaces.size() % 2) != 0)
                continue;

            std::vector<uint8> frameBytes;
            for (size_t i = 0; i < hexStrNoSpaces.size(); i += 2) {
                uint8 byte = static_cast<uint8>(std::stoi(hexStrNoSpaces.substr(i, 2), nullptr, 16));
                frameBytes.push_back(byte);
            }
            if (!frameBytes.empty()) {
                outMap[channel] = std::move(frameBytes);
            }
        }
        catch (...) {}
    }
}

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 128;
    devInfo.OutputCount = 128;
    devInfo.AxisCount = 128;
    return &devInfo;
}

void processDirtyStatus(uint8 deviceIndex) {
    auto it = g_deviceContexts.find(deviceIndex);
    if (it == g_deviceContexts.end()) return;
    DeviceContext* context = it->second.get();

    auto& serialPort = context->serialPort;
    while (!context->stopThread.load(std::memory_order_acquire)) {
        std::map<int, short> dirtyDOStatus;
        {
            std::unique_lock<std::mutex> lock(context->queueMutex);
            context->queueCV.wait(lock, [context] {
                return !context->dirtyQueues.empty() || context->stopThread.load(std::memory_order_acquire);
                });

            if (context->stopThread.load(std::memory_order_acquire) && context->dirtyQueues.empty())
                return;

            if (!context->dirtyQueues.empty()) {
                dirtyDOStatus = std::move(context->dirtyQueues.front());
                context->dirtyQueues.pop();
            }
        }

        if (dirtyDOStatus.empty() || !serialPort)
            continue;

        for (const auto& kv : dirtyDOStatus) {
            int ch = kv.first;
            short val = kv.second;

            auto itFrame = context->outputFrameMap.find(static_cast<uint8>(ch));
            if (itFrame != context->outputFrameMap.end()) {
                const std::vector<uint8>& frameData = itFrame->second;
                try {
                    serialPort->write(reinterpret_cast<const char*>(frameData.data()), static_cast<int>(frameData.size()));
                }
                catch (...) {
                    // 可补充日志
                }
                // 发送后更新状态为0
                context->lastDOStatus[ch] = 0;
            }
            else {
                char data[4] = { static_cast<char>(context->frameHeader), 0, 0, static_cast<char>(context->frameTail) };
                data[1] = static_cast<char>(ch);
                data[2] = static_cast<char>(val);
                try {
                    serialPort->write(data, 4);
                }
                catch (...) {
                    // 忽略异常
                }
                if (context->waitTimeMs > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(context->waitTimeMs));
            }
        }
    }
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    try
    {
        if (g_deviceContexts.count(deviceIndex)) {
            g_deviceContexts[deviceIndex]->stop();
            g_deviceContexts.erase(deviceIndex);
        }

        std::string path = dh::Paths::Instance().GetModuleDir();
        std::string config_file_path = path + "Config\\SERIAL-BRIDGE\\config.ini";

        if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
        if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
        g_iniFile->read(*g_iniStructure);
        auto& ini = *g_iniStructure;

        auto deviceSectionName = BuildDeviceAttribute("device", deviceIndex);
        std::map<std::string, std::string> mergedConfig;

        auto mergeSection = [&mergedConfig](mINI::INIStructure& ini, const std::string& section) {
            for (const auto& kv : ini[section]) {
                mergedConfig[kv.first] = kv.second;
            }
            };

        mergeSection(ini, "default");
        mergeSection(ini, deviceSectionName);

        int baudRate = 57600;
        int waitTimeMs = 60;
        if (mergedConfig.count("baud_rate")) baudRate = std::stoi(mergedConfig["baud_rate"]);
        if (mergedConfig.count("write_wait_ms")) waitTimeMs = std::stoi(mergedConfig["write_wait_ms"]);

        std::string portName;
        if (mergedConfig.count("port_name")) portName = mergedConfig["port_name"];
        portName = NormalizePortName(portName, deviceIndex);

        auto context = std::make_unique<DeviceContext>(deviceIndex);
        context->serialPort = std::make_unique<Serial>(portName, baudRate);
        context->waitTimeMs = waitTimeMs;

        auto parseHexByte = [](const std::string& s, uint8 defaultVal) -> uint8 {
            if (s.empty()) return defaultVal;
            try {
                size_t pos = 0;
                int val = std::stoi(s, &pos, 16);
                if (pos != s.size()) return defaultVal;
                return static_cast<uint8>(val & 0xFF);
            }
            catch (...) {
                return defaultVal;
            }
            };

        if (mergedConfig.count("frame_header"))
            context->frameHeader = parseHexByte(mergedConfig["frame_header"], 0xFE);
        if (mergedConfig.count("frame_tail"))
            context->frameTail = parseHexByte(mergedConfig["frame_tail"], 0xFF);
        if (mergedConfig.count("ch_index"))
            context->chIndex = static_cast<uint8>(std::stoi(mergedConfig["ch_index"]));
        if (mergedConfig.count("val_index"))
            context->valIndex = static_cast<uint8>(std::stoi(mergedConfig["val_index"]));
        if (mergedConfig.count("frame_length"))
            context->frameLength = static_cast<size_t>(std::stoi(mergedConfig["frame_length"]));
        else
            context->frameLength = 4;

        if (mergedConfig.count("hold_timeout_ms")) {
            try {
                int timeout = std::stoi(mergedConfig["hold_timeout_ms"]);
                if (timeout > 0) {
                    context->inputChannelTimeoutMs = timeout;
                }
            }
            catch (...) {}
        }

        LoadFrameMap(ini, "input_frame_map", context->inputFrameMap, devInfo.InputCount);
        LoadFrameMap(ini, "output_frame_map", context->outputFrameMap, devInfo.OutputCount);

        // 输入通道更新时间初始化
        auto now = std::chrono::steady_clock::now();
        for (const auto& kv : context->inputFrameMap) {
            context->inputChannelLastUpdateTime[kv.first] = now;
        }

        g_deviceContexts[deviceIndex] = std::move(context);
        g_deviceContexts[deviceIndex]->startThread(processDirtyStatus);
    }
    catch (...)
    {
        return 0;
    }
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    if (g_deviceContexts.count(deviceIndex)) {
        g_deviceContexts[deviceIndex]->stop();
        g_deviceContexts.erase(deviceIndex);
    }
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    auto it = g_deviceContexts.find(deviceIndex);
    if (it == g_deviceContexts.end()) return 0;

    DeviceContext* context = it->second.get();

    if (context->lastDOStatus.size() < devInfo.OutputCount) {
        context->lastDOStatus.resize(devInfo.OutputCount, 0);
    }

    std::map<int, short> dirtyDOStatus;
    for (size_t idx = 0; idx < context->lastDOStatus.size(); ++idx) {
        short newVal = InDOStatus[idx];
        short oldVal = context->lastDOStatus[idx];
        if (oldVal != newVal) {
            dirtyDOStatus[(int)idx] = newVal;
            context->lastDOStatus[idx] = newVal;
        }
    }

    if (!dirtyDOStatus.empty()) {
        std::lock_guard<std::mutex> lock(context->queueMutex);
        if (context->dirtyQueues.size() >= MAX_QUEUE_SIZE) {
            auto& last = context->dirtyQueues.back();
            for (const auto& kv : dirtyDOStatus) {
                last[kv.first] = kv.second;
            }
        }
        else {
            context->dirtyQueues.push(dirtyDOStatus);
        }
        context->queueCV.notify_one();
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    auto it = g_deviceContexts.find(deviceIndex);
    if (it == g_deviceContexts.end() || !it->second->serialPort)
        return 0;

    DeviceContext* context = it->second.get();

    static std::map<uint8, std::deque<uint8>> recvDatasMap;

    char readBuf[1024] = { 0 };
    int readCount = 0;

    try {
        readCount = context->serialPort->read(readBuf, sizeof(readBuf), false);
    }
    catch (...) {
        return 0;
    }

    auto& recvDatas = recvDatasMap[deviceIndex];

    for (int i = 0; i < readCount; ++i)
        recvDatas.push_back(static_cast<uint8>(readBuf[i]));

    size_t frameLen = context->frameLength;
    if (frameLen == 0) frameLen = 4;

    bool frameFound = false;
    std::vector<uint8> frame;

    while (recvDatas.size() >= frameLen) {
        if (recvDatas.front() != context->frameHeader) {
            recvDatas.pop_front();
            continue;
        }
        if (recvDatas[frameLen - 1] != context->frameTail) {
            recvDatas.pop_front();
            continue;
        }
        frame.assign(recvDatas.begin(), recvDatas.begin() + frameLen);
        frameFound = true;
        break;
    }

    if (!frameFound) {
        constexpr size_t MAX_RECV_BUF_SIZE = 2048;
        if (recvDatas.size() > MAX_RECV_BUF_SIZE)
            recvDatas.clear();
        context->checkInputTimeouts(OutDIStatus);
        return 1;
    }

    for (size_t i = 0; i < frameLen; ++i)
        recvDatas.pop_front();

    bool matched = false;
    for (const auto& kv : context->inputFrameMap) {
        uint8 ch = kv.first;
        const std::vector<uint8>& mappedFrame = kv.second;
        if (mappedFrame.size() == frame.size() && std::equal(mappedFrame.begin(), mappedFrame.end(), frame.begin())) {
            OutDIStatus[ch] = 1;
            context->updateInputChannelTimestamp(ch);
            matched = true;
            break;
        }
    }

    if (!matched) {
        if (context->chIndex < frame.size() && context->valIndex < frame.size()) {
            uint8 channel = frame[context->chIndex];
            uint8 status = frame[context->valIndex];
            if (channel < devInfo.InputCount) {
                OutDIStatus[channel] = (status == 0x00) ? 0 : 1;
            }
        }
    }

    context->checkInputTimeouts(OutDIStatus);

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}
