/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include <map>
#include <memory>
#include <thread>
#include <iostream>
#include <Windows.h>
#include <boost/asio.hpp>

#include "IOUI.h"
#include "DeviceContext.h"
#include "ProtocolFactory.h"
#include "ConfigLoader.h"
#include "Paths.hpp"

namespace dh = DevelopHelper;
using namespace IOHub;

DeviceInfo g_devInfo;
std::map<uint8_t, std::unique_ptr<DeviceContext>> g_devices;

// 全局io_context（UDP/TCP共享）
boost::asio::io_context g_ioContext;
std::thread g_ioThread;
std::mutex g_ioMutex;

// 全局保存 work_guard
std::unique_ptr<boost::asio::executor_work_guard<
    boost::asio::io_context::executor_type>> g_workGuard;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_devInfo.InputCount = 255;
    g_devInfo.OutputCount = 255;
    g_devInfo.AxisCount = 0;
    return &g_devInfo;
}

void ensureIOThread() {
    std::lock_guard<std::mutex> lock(g_ioMutex);
    if (!g_ioThread.joinable()) {
        OutputDebugStringA("[IOHub] Starting IO thread...\n");
        
        g_ioThread = std::thread([]() {
            OutputDebugStringA("[IOHub] IO thread started\n");
            
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type> 
                workGuard(g_ioContext.get_executor());
            
            OutputDebugStringA("[IOHub] Calling io_context.run()...\n");
            g_ioContext.run();
            
            OutputDebugStringA("[IOHub] io_context.run() exited\n");
        });
    }
}

void stopIOThread() {
    std::lock_guard<std::mutex> lock(g_ioMutex);
    if (g_ioThread.joinable()) {
        g_workGuard.reset(); // 释放 work guard
        g_ioContext.stop();
        g_ioThread.join();
        g_ioContext.restart();
    }
}

IOUI_API int __stdcall OpenDevice(uint8_t deviceIndex) {
    try {
        // 1. 先清理旧设备
        if (g_devices.count(deviceIndex)) {
            CloseDevice(deviceIndex); // 使用统一的关闭逻辑
        }
        
        // 2. RAII 模式：失败自动回滚
        auto cleanup = [&]() {
            if (g_devices.count(deviceIndex)) {
                g_devices.erase(deviceIndex);
            }
            if (g_devices.empty()) {
                stopIOThread();
            }
        };
        
        // 3. 创建设备上下文
        // 加载配置
        std::string modulePath = dh::Paths::Instance().GetModuleDir();
        std::string configPath = modulePath + "Config\\IOHUB\\config.ini";
        
        ConfigLoader configLoader(configPath);
        
        // 加载协议配置
        std::unique_ptr<ProtocolConfig> protocolConfig;
        int writeWaitMs = 60;
        if (!configLoader.loadDeviceConfig(deviceIndex, protocolConfig, writeWaitMs)) {
            return 0;
        }
        
        // 确保网络协议的IO线程运行
        if (protocolConfig->type == ProtocolType::UDP || 
            protocolConfig->type == ProtocolType::TCP ||
            protocolConfig->type == ProtocolType::TCP_SERVER) {
            ensureIOThread();
        }
        
        // 创建协议实例
        auto protocol = ProtocolFactory::create(*protocolConfig, &g_ioContext);
        if (!protocol) {
            return 0;
        }
        
        auto device = std::make_unique<DeviceContext>(
            deviceIndex, 
            std::move(protocol),
            g_devInfo.InputCount,
            g_devInfo.OutputCount);
        
        device->setWriteWaitMs(writeWaitMs);
        
        // 设置输入保持时间
        device->setInputTimeout(configLoader.getInputHoldMs());
        
        // 加载通道映射
        DataFormat defaultFormat = DataFormat::AUTO;
        if (!configLoader.loadChannelMapping(device->getMapping(), defaultFormat)) {
            // No channel mapping loaded
        }
        
        // 加载帧配置（所有协议都支持）
        FrameConfig frameConfig;
        if (configLoader.loadFrameConfig(deviceIndex, frameConfig)) {
            auto frameProcessor = std::make_unique<FrameProcessor>(frameConfig);
            device->setFrameProcessor(std::move(frameProcessor));
        }
        
        // 启动设备
        device->start();
        
        g_devices[deviceIndex] = std::move(device);
        
        return 1;
    }
    catch (const std::exception& e) {
        return 0;
    }
    catch (...) {
        return 0;
    }
}

IOUI_API int __stdcall CloseDevice(uint8_t deviceIndex)
{
    if (g_devices.count(deviceIndex)) {
        g_devices[deviceIndex]->stop();
        g_devices.erase(deviceIndex);
    }
    
    // 如果所有设备都关闭了，停止IO线程
    if (g_devices.empty()) {
        stopIOThread();
    }
    
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8_t deviceIndex, short* InDOStatus)
{
    auto it = g_devices.find(deviceIndex);
    if (it == g_devices.end()) {
        return 0;
    }
    
    return it->second->setDO(InDOStatus, g_devInfo.OutputCount) ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8_t deviceIndex, BYTE* OutDIStatus)
{
    auto it = g_devices.find(deviceIndex);
    if (it == g_devices.end()) {
        ZeroMemory(OutDIStatus, g_devInfo.InputCount);
        return 0;
    }
    
    return it->second->getDI(OutDIStatus, g_devInfo.InputCount) ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8_t, short* OutDOStatus)
{
    std::fill_n(OutDOStatus, g_devInfo.OutputCount, 0);
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8_t, short*)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8_t, BYTE*, unsigned int)
{
    return 0;
}
