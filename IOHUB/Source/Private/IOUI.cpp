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
        std::cout << "[IOHub] Starting IO thread..." << std::endl;
        g_ioThread = std::thread([]() {
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
                workGuard(g_ioContext.get_executor());
            std::cout << "[IOHub] IO thread started" << std::endl;
            g_ioContext.run();
            std::cout << "[IOHub] IO thread stopped" << std::endl;
        });
    }
}

void stopIOThread() {
    std::lock_guard<std::mutex> lock(g_ioMutex);
    if (g_ioThread.joinable()) {
        std::cout << "[IOHub] Stopping IO thread..." << std::endl;
        g_ioContext.stop();
        g_ioThread.join();
        g_ioContext.restart();
    }
}

IOUI_API int __stdcall OpenDevice(uint8_t deviceIndex)
{
    try {
        std::cout << "[IOHub] Opening device " << (int)deviceIndex << std::endl;
        
        // 关闭已存在的设备
        if (g_devices.count(deviceIndex)) {
            std::cout << "[IOHub] Device " << (int)deviceIndex << " already exists, closing first" << std::endl;
            g_devices[deviceIndex]->stop();
            g_devices.erase(deviceIndex);
        }
        
        // 加载配置
        std::string modulePath = dh::Paths::Instance().GetModuleDir();
        std::string configPath = modulePath + "Config\\IOHUB\\config.ini";
        
        ConfigLoader configLoader(configPath);
        
        // 加载协议配置
        std::unique_ptr<ProtocolConfig> protocolConfig;
        int writeWaitMs = 60;
        if (!configLoader.loadDeviceConfig(deviceIndex, protocolConfig, writeWaitMs)) {
            std::cout << "[IOHub] Failed to load device config" << std::endl;
            return 0;
        }
        
        // 确保网络协议的IO线程运行
        if (protocolConfig->type == ProtocolType::UDP || 
            protocolConfig->type == ProtocolType::TCP) {
            ensureIOThread();
        }
        
        // 创建协议实例
        auto protocol = ProtocolFactory::create(*protocolConfig, &g_ioContext);
        if (!protocol) {
            std::cout << "[IOHub] Failed to create protocol" << std::endl;
            return 0;
        }
        
        std::cout << "[IOHub] Protocol created: ";
        switch (protocolConfig->type) {
            case ProtocolType::UDP:
                std::cout << "UDP";
                break;
            case ProtocolType::TCP:
                std::cout << "TCP";
                break;
            case ProtocolType::SERIAL:
                std::cout << "SERIAL";
                break;
        }
        std::cout << std::endl;
        
        // 创建设备上下文
        auto device = std::make_unique<DeviceContext>(
            deviceIndex, 
            std::move(protocol),
            g_devInfo.InputCount,
            g_devInfo.OutputCount);
        
        device->setWriteWaitMs(writeWaitMs);
        
        // 加载通道映射
        DataFormat defaultFormat = DataFormat::AUTO;
        if (!configLoader.loadChannelMapping(device->getMapping(), defaultFormat)) {
            std::cout << "[IOHub] Warning: No channel mapping loaded" << std::endl;
        }
        
        // 如果是串口协议，加载帧配置
        if (protocolConfig->type == ProtocolType::SERIAL) {
            FrameConfig frameConfig;
            if (configLoader.loadFrameConfig(frameConfig)) {
                auto frameProcessor = std::make_unique<FrameProcessor>(frameConfig);
                device->setFrameProcessor(std::move(frameProcessor));
                std::cout << "[IOHub] Frame processor configured" << std::endl;
            }
        }
        
        // 启动设备
        device->start();
        
        g_devices[deviceIndex] = std::move(device);
        
        std::cout << "[IOHub] Device " << (int)deviceIndex << " opened successfully" << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cout << "[IOHub] Exception in OpenDevice: " << e.what() << std::endl;
        return 0;
    }
    catch (...) {
        std::cout << "[IOHub] Unknown exception in OpenDevice" << std::endl;
        return 0;
    }
}

IOUI_API int __stdcall CloseDevice(uint8_t deviceIndex)
{
    std::cout << "[IOHub] Closing device " << (int)deviceIndex << std::endl;
    
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
