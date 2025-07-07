#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Serial.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <atomic>
#include <memory>

#include "Util.hpp"
#include "mIni/mini/ini.h"

namespace dh = DevelopHelper;

DeviceInfo devInfo;

// 多设备管理相关容器
std::map<uint8, std::unique_ptr<Serial>> g_serialPorts;
std::map<uint8, std::thread> g_deviceThreads;
std::map<uint8, std::atomic<bool>> g_stopThreads;
std::map<uint8, std::queue<std::map<int, short>>> g_dirtyQueues;
std::map<uint8, std::mutex> g_queueMutexes;
std::map<uint8, std::condition_variable> g_queueCVs;
// 设备独立等待时间
std::map<uint8, int> g_waitTimes;

const size_t MAX_QUEUE_SIZE = 100;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 128;
    devInfo.OutputCount = 128;
    devInfo.AxisCount = 128;
    return &devInfo;
}

void processDirtyStatus(uint8 deviceIndex) {
    while (!g_stopThreads[deviceIndex].load(std::memory_order_acquire)) {
        std::map<int, short> _dirtyDOStatus;
        {
            std::unique_lock<std::mutex> lock(g_queueMutexes[deviceIndex]);
            g_queueCVs[deviceIndex].wait(lock, [deviceIndex] {
                return !g_dirtyQueues[deviceIndex].empty() || g_stopThreads[deviceIndex].load(std::memory_order_acquire);
                });

            if (g_stopThreads[deviceIndex].load(std::memory_order_acquire) && g_dirtyQueues[deviceIndex].empty()) {
                return; // 线程退出
            }

            if (!g_dirtyQueues[deviceIndex].empty()) {
                _dirtyDOStatus = std::move(g_dirtyQueues[deviceIndex].front());
                g_dirtyQueues[deviceIndex].pop();
            }
        }

        if (_dirtyDOStatus.empty())
            continue;

        auto& serialPort = g_serialPorts[deviceIndex];
        if (!serialPort)
            continue;

        int waitMs = 60;
        if (g_waitTimes.count(deviceIndex))
            waitMs = g_waitTimes[deviceIndex];

        for (const auto& _doItem : _dirtyDOStatus) {
            char _data[4] = { 0 };
            _data[0] = 0xFE;
            _data[1] = static_cast<char>(_doItem.first);
            _data[2] = static_cast<char>(_doItem.second);
            _data[3] = 0xFF;

            try {
                serialPort->write(_data, 4);
            }
            catch (...) {
            }
            if (waitMs > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
        }
    }
}

std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    try
    {
        if (g_serialPorts.count(deviceIndex) && g_serialPorts[deviceIndex]) {
            g_stopThreads[deviceIndex] = true;
            g_queueCVs[deviceIndex].notify_all();
            if (g_deviceThreads[deviceIndex].joinable())
                g_deviceThreads[deviceIndex].join();
            g_serialPorts[deviceIndex]->flush();
            g_serialPorts.erase(deviceIndex);
            g_deviceThreads.erase(deviceIndex);
            g_stopThreads.erase(deviceIndex);
            {
                std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                while (!g_dirtyQueues[deviceIndex].empty()) g_dirtyQueues[deviceIndex].pop();
            }
            g_dirtyQueues.erase(deviceIndex);
            g_queueMutexes.erase(deviceIndex);
            g_queueCVs.erase(deviceIndex);
            g_waitTimes.erase(deviceIndex);
        }

        std::string path = dh::Paths::Instance().GetModuleDir();
        std::string config_file_path = path + "Config\\SERIAL-IOFE\\config.ini";

        if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
        if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
        g_iniFile->read(*g_iniStructure);
        auto& ini = *g_iniStructure;

        auto deviceSectionName = BuildDeviceAttribute("device", deviceIndex);
        auto& defaultSection = ini["default"];
        std::map<std::string, std::string> mergedConfig;

        for (const auto& kv : defaultSection)
            mergedConfig[kv.first] = kv.second;
        auto& deviceSectionMap = ini[deviceSectionName];
        for (const auto& kv : deviceSectionMap)
            mergedConfig[kv.first] = kv.second;

        int baudRate = 57600;
        int waitTimeMs = 60;
        if (mergedConfig.count("baud_rate")) baudRate = std::stoi(mergedConfig["baud_rate"]);
        if (mergedConfig.count("write_wait_ms")) waitTimeMs = std::stoi(mergedConfig["write_wait_ms"]);

        g_waitTimes[deviceIndex] = waitTimeMs;

        std::string portName;
        if (mergedConfig.count("port_name")) portName = mergedConfig["port_name"];
        portName = NormalizePortName(portName, deviceIndex);

        g_serialPorts[deviceIndex] = std::make_unique<Serial>(portName, baudRate);

        g_stopThreads[deviceIndex] = false;
        {
            std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
            while (!g_dirtyQueues[deviceIndex].empty()) g_dirtyQueues[deviceIndex].pop();
        }

        g_deviceThreads[deviceIndex] = std::thread(processDirtyStatus, deviceIndex);
    }
    catch (...)
    {
        return 0;
    }
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    if (g_serialPorts.count(deviceIndex) && g_serialPorts[deviceIndex]) {
        g_stopThreads[deviceIndex] = true;
        g_queueCVs[deviceIndex].notify_all();
        if (g_deviceThreads[deviceIndex].joinable())
            g_deviceThreads[deviceIndex].join();
        g_serialPorts[deviceIndex]->flush();
        g_serialPorts.erase(deviceIndex);
        g_deviceThreads.erase(deviceIndex);
        g_stopThreads.erase(deviceIndex);
        {
            std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
            while (!g_dirtyQueues[deviceIndex].empty()) g_dirtyQueues[deviceIndex].pop();
        }
        g_dirtyQueues.erase(deviceIndex);
        g_queueMutexes.erase(deviceIndex);
        g_queueCVs.erase(deviceIndex);
        g_waitTimes.erase(deviceIndex);
    }
    return 1;
}

void pushDirtyDOStatus(uint8 deviceIndex, const std::map<int, short>& dirtyData) {
    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
    if (g_dirtyQueues[deviceIndex].size() >= MAX_QUEUE_SIZE) {
        auto& last = g_dirtyQueues[deviceIndex].back();
        for (const auto& kv : dirtyData) {
            last[kv.first] = kv.second;
        }
    }
    else {
        g_dirtyQueues[deviceIndex].push(dirtyData);
    }
    g_queueCVs[deviceIndex].notify_one();
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    static std::map<uint8, std::vector<short>> lastDOStatusMap;
    if (!lastDOStatusMap.count(deviceIndex)) {
        lastDOStatusMap[deviceIndex] = std::vector<short>(devInfo.OutputCount, 0);
    }

    std::vector<short>& _lastDOStatus = lastDOStatusMap[deviceIndex];
    std::map<int, short> _dirtyDOStatus;
    for (size_t _idx = 0; _idx < _lastDOStatus.size(); ++_idx) {
        if (_lastDOStatus[_idx] != InDOStatus[_idx]) {
            _dirtyDOStatus[_idx] = InDOStatus[_idx];
            _lastDOStatus[_idx] = InDOStatus[_idx];
        }
    }
    if (!_dirtyDOStatus.empty()) {
        pushDirtyDOStatus(deviceIndex, _dirtyDOStatus);
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    static std::map<uint8, std::vector<uint8>> recvDatasMap;
    char _data[256] = { 0 };
    int recevCount = 0;

    if (!g_serialPorts.count(deviceIndex) || !g_serialPorts[deviceIndex])
        return 0;

    try {
        recevCount = g_serialPorts[deviceIndex]->read(_data, sizeof(_data), false);
    }
    catch (...) {
        return 0;
    }
    auto& _recvDatas = recvDatasMap[deviceIndex];

    for (int i = 0; i < recevCount; ++i) {
        _recvDatas.push_back(static_cast<uint8>(_data[i]));
    }
    while (_recvDatas.size() >= 4) {
        if (_recvDatas[0] != 0xFE) {
            _recvDatas.erase(_recvDatas.begin());
            continue;
        }
        std::vector<uint8> _content(_recvDatas.begin(), _recvDatas.begin() + 4);
        uint8 channel = _content[1];
        uint8 status = _content[2];
        if (channel >= devInfo.InputCount) {
            _recvDatas.erase(_recvDatas.begin(), _recvDatas.begin() + 4);
            continue;
        }
        OutDIStatus[channel] = (status == 0x00) ? 0 : 1;
        _recvDatas.erase(_recvDatas.begin(), _recvDatas.begin() + 4);
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
    return 0;
}
