/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <windows.h>
#include "RPLidarWrapper.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include <memory>
#include <cstring>
#include "Paths.hpp"

 // 常量定义
constexpr int kAxisCount = 192;
constexpr int kTimeoutSeconds = 5;

// 设备信息全局变量按原样保留
DeviceInfo devInfo;

// 设备上下文结构体
struct DeviceContext {
    int deviceID = -1;
    int configIndex = -1;
    std::unique_ptr<RPLidarWrapper> lidar;

    std::atomic<bool> bExit{ false };          // 标识设备请求关闭
    std::atomic<bool> bClosed{ false };        // 标识设备已安全关闭并释放lidar资源

    bool openAttempted = false;
    bool openSuccess = false;

    std::mutex dataMutex;
    std::mutex openMutex;
    std::condition_variable openCV;

    std::mutex closeMutex;
    std::condition_variable closeCV;

    short doStatus[kAxisCount]{ 0 };
    int debugStatus = -1000;

    DeviceContext() {}
};

std::mutex g_devicesMutex;
std::unordered_map<uint8_t, std::shared_ptr<DeviceContext>> g_devices;

std::atomic<bool> g_exitFlag{ false };
std::thread g_updateThread;

std::atomic<int> g_deviceNumberCounter{ 0 };

void UpdateAllDevices()
{
    while (!g_exitFlag.load())
    {
        std::unordered_map<uint8_t, std::shared_ptr<DeviceContext>> devicesCopy;
        {
            std::lock_guard<std::mutex> lock(g_devicesMutex);
            devicesCopy = g_devices; // 拷贝智能指针，增加引用计数，防止悬空
        }

        for (auto& pair : devicesCopy)
        {
            auto& context = pair.second;

            if (context->bExit.load())
            {
                if (context->lidar)
                {
                    context->lidar->CloseLidar();
                    context->lidar.reset(); // 释放智能指针管理的对象
                }
                if (!context->bClosed.exchange(true))
                {
                    std::lock_guard<std::mutex> closeLock(context->closeMutex);
                    context->closeCV.notify_one();
                }
                continue;
            }

            if (!context->openAttempted)
            {
                context->openAttempted = true;

                context->lidar = std::make_unique<RPLidarWrapper>(context->deviceID);
                bool opened = context->lidar->OpenLidar();

                {
                    std::lock_guard<std::mutex> openLock(context->openMutex);
                    context->openSuccess = opened;
                }
                context->openCV.notify_one();

                // 如果你需要在打开失败时做特殊处理，可以在这里拓展
            }

            if (context->lidar)
            {
                std::lock_guard<std::mutex> dataLock(context->dataMutex);
                context->lidar->Update();

                auto points = context->lidar->getTouchPoints();
                if (points)
                {
                    memcpy(context->doStatus, points, sizeof(short) * kAxisCount);
                }

                if (context->debugStatus != -1000)
                {
                    context->lidar->SetDebugMode(context->debugStatus);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

// Initialize 保持不变
IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 16;
    devInfo.OutputCount = 255;
    devInfo.AxisCount = kAxisCount;
    return &devInfo;
}
bool cfgInited = false;
HMODULE g_hOpenCvModule = nullptr;
IOUI_API int __stdcall OpenDevice(uint8_t deviceIndex)
{

    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);

        if (!cfgInited) {
            RPConfigMgr::Instance().Read();
            cfgInited = true;
        }
        if (g_devices.find(deviceIndex) != g_devices.end())
            return 1;

        auto context = std::make_shared<DeviceContext>();
        
        context->deviceID = g_deviceNumberCounter.fetch_add(1);
        // context->configIndex = g_deviceNumberCounter.fetch_add(1);
        context->bExit = false;

#ifdef _DEBUG
        std::string opencvPath = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\opencv_world440d.dll";
#else
        std::string opencvPath = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\opencv_world440.dll";
#endif
        
        if (g_hOpenCvModule == nullptr) {
            g_hOpenCvModule = LoadLibraryA(opencvPath.c_str());
        }

        /* 如果你之前注释了rplidar_driver.dll加载，根据需要恢复：
        std::string rplidarDll = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\rplidar_driver.dll";
        LoadLibraryA(rplidarDll.c_str()); */

        g_devices[deviceIndex] = context;
    }

    if (!g_updateThread.joinable())
    {
        g_exitFlag.store(false);
        g_updateThread = std::thread(UpdateAllDevices);
    }

    {
        std::unique_lock<std::mutex> lock(g_devices[deviceIndex]->openMutex);
        if (!g_devices[deviceIndex]->openCV.wait_for(lock, std::chrono::seconds(kTimeoutSeconds),
            [&]() { return g_devices[deviceIndex]->openAttempted; }))
        {
            return 1;
        }
    }

    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8_t deviceIndex)
{
    std::shared_ptr<DeviceContext> context;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        auto it = g_devices.find(deviceIndex);
        if (it == g_devices.end())
            return 0;
        context = it->second;
        context->bExit.store(true);
    }

    {
        std::unique_lock<std::mutex> closeLock(context->closeMutex);
        if (!context->closeCV.wait_for(closeLock, std::chrono::seconds(kTimeoutSeconds),
            [&]() { return context->bClosed.load(); }))
        {
            // 等待超时，可以根据需要做处理
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        g_devices.erase(deviceIndex);
    }

    // 智能指针自动释放内存，无需手动delete

    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        if (g_devices.empty() && g_updateThread.joinable())
        {
            cfgInited = false;
            g_exitFlag.store(true);
            g_updateThread.join();
            if (g_hOpenCvModule)
            {
                FreeLibrary(g_hOpenCvModule);
                g_hOpenCvModule = nullptr;
            }
            g_deviceNumberCounter.store(0);
        }
    }

    return 1;
}

const int funcChannel = 240;
IOUI_API int __stdcall SetDeviceDO(uint8_t deviceIndex, short* InDOStatus)
{
    const short funcCode = InDOStatus[funcChannel];
    if (funcChannel == -1000) return 1;

    // 240  0 debug&preview窗口 1 debug窗口 2 preview窗口
    std::shared_ptr<DeviceContext> context;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        auto it = g_devices.find(deviceIndex);
        if (it == g_devices.end())
            return 0;
        context = it->second;
    }

    std::lock_guard<std::mutex> dataLock(context->dataMutex);
    context->debugStatus = funcCode;
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8_t deviceIndex, short* OutDOStatus)
{

    std::shared_ptr<DeviceContext> context;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        auto it = g_devices.find(deviceIndex);
        if (it == g_devices.end())
            return 0;
        context = it->second;
    }

    std::lock_guard<std::mutex> dataLock(context->dataMutex);
    if (context->debugStatus != -1000) {
        OutDOStatus[funcChannel] = context->debugStatus;
        return 1;
    }

    OutDOStatus[funcChannel] = -1000;
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8_t deviceIndex, BYTE* OutDIStatus)
{
    // 目前不支持设备DI状态，多设备情况下同样不支持，返回0
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8_t deviceIndex, short* OutADStatus)
{
    std::shared_ptr<DeviceContext> context;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        auto it = g_devices.find(deviceIndex);
        if (it == g_devices.end())
            return 0;
        context = it->second;
    }

    std::lock_guard<std::mutex> dataLock(context->dataMutex);
    memcpy(OutADStatus, context->doStatus, sizeof(short) * kAxisCount);

    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8_t deviceIndex, BYTE* Data, unsigned int Size) {
    // 当前不支持流数据刷新，返回0
    return 0;
}
