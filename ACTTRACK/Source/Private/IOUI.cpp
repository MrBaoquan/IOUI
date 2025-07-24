/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <chrono>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include "IOUI.h"
#include "mIni/mini/ini.h"
#include "Util.hpp"
#include "Paths.hpp"
#include "json/single_include/nlohmann/json.hpp" // JSON 头文件

 // 设备信息全局变量
DeviceInfo devInfo;

// 设备上下文结构体，存储设备配置信息及交互计数
struct DeviceContext
{
    std::string remoteIP;
    int remotePort;
    int localPort;

    // 按钮类型和ID统计交互计数
    std::map<int, std::map<int, int>> interactionCounts;

    // 日志打印周期，单位秒
    int reportIntervalSeconds;

    // 心跳间隔，单位秒，0表示不发送心跳
    int heartbeatIntervalSeconds;

    // 上次打印日志时间点
    std::chrono::steady_clock::time_point lastReportTime;

    // 上次心跳时间点
    std::chrono::steady_clock::time_point lastHeartbeatTime;

    // 交互相关状态
    bool isInteracting = false;
    std::chrono::steady_clock::time_point interactStartTime;

    // 交互次数
    int interactionCount = 0;

    // 每次交互时长，单位秒
    std::vector<double> interactionDurations;

    // 上一次交互状态（0或1）
    int lastInteractStatus = 0;

    // 报警缓存，存储错误码（16位）
    std::vector<uint16_t> alarmCodes;

    // Boost.Asio相关成员，实现socket复用
    std::shared_ptr<boost::asio::io_context> ioContext;
    std::unique_ptr<boost::asio::ip::udp::socket> udpSocket;

    // 心跳包单独地址和端口
    std::string heartbeatIP;
    int heartbeatPort;

    // 当前进程全路径，UTF-8编码字符串，用于心跳包
    std::string processPath;

    DeviceContext()
        : remoteIP("127.0.0.1"), remotePort(12321), localPort(12333),
        reportIntervalSeconds(5), heartbeatIntervalSeconds(0),
        lastReportTime(std::chrono::steady_clock::now()),
        lastHeartbeatTime(std::chrono::steady_clock::now()),
        ioContext(std::make_shared<boost::asio::io_context>()),
        heartbeatIP(""), heartbeatPort(0)
    {
    }

    // 初始化并绑定socket
    bool InitSocket()
    {
        try
        {
            udpSocket = std::make_unique<boost::asio::ip::udp::socket>(*ioContext);
            boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::udp::v4(), localPort);
            udpSocket->open(boost::asio::ip::udp::v4());
            udpSocket->bind(localEndpoint);
            return true;
        }
        catch (std::exception&)
        {
            // 这里可以加日志
            return false;
        }
    }

    // 发送数据
    bool SendData(const std::vector<uint8_t>& data, const std::string& ip, int port) const
    {
        if (!udpSocket)
            return false;

        try
        {
            boost::asio::ip::udp::endpoint remoteEndpoint(boost::asio::ip::make_address(ip), port);
            size_t sent = udpSocket->send_to(boost::asio::buffer(data), remoteEndpoint);
            return sent == data.size();
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    // 发送数据，发送到远端上报地址
    bool SendData(const std::vector<uint8_t>& data) const
    {
        return SendData(data, remoteIP, remotePort);
    }

    // 发送心跳数据包，使用心跳地址和端口，通过JSON字符串形式发送
    bool SendHeartbeatData() const
    {
        if (!udpSocket)
            return false;

        try
        {
            nlohmann::json j;
            j["evt"] = 1221;
            j["data"]["process"] = processPath;

            std::string jsonStr = j.dump();  // 生成utf8 JSON字符串

            std::string ip = heartbeatIP.empty() ? remoteIP : heartbeatIP;
            int port = heartbeatPort == 0 ? remotePort : heartbeatPort;

            boost::asio::ip::udp::endpoint heartbeatEndpoint(boost::asio::ip::make_address(ip), port);
            size_t sent = udpSocket->send_to(boost::asio::buffer(jsonStr), heartbeatEndpoint);
            return sent == jsonStr.size();
        }
        catch (std::exception&)
        {
            return false;
        }
    }
};

// 维护设备索引到设备上下文的映射
std::map<uint8, DeviceContext> g_deviceContextMap;

// Ini文件相关全局变量
std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

// 初始化设备信息
IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 0;
    devInfo.OutputCount = 255;
    devInfo.AxisCount = 0;
    return &devInfo;
}

// 通过设备索引找到上下文后调用上下文上的发送方法
static bool SendUdpData(uint8 deviceIndex, const std::vector<uint8_t>& data)
{
    auto it = g_deviceContextMap.find(deviceIndex);
    if (it == g_deviceContextMap.end())
        return false;

    DeviceContext& context = it->second;
    return context.SendData(data);
}

// 构造并发送交互计数数据，发送成功则清空计数
static bool SendInteractionCounts(uint8 deviceIndex, DeviceContext& context)
{
    if (context.interactionCounts.empty() && context.interactionCount == 0 && context.alarmCodes.empty())
        return false;

    std::vector<uint8_t> udpBuffer;

    if (context.interactionCount > 0)
    {
        // 仅当interactionCount大于0时，才添加固定标识和interactionCount
        udpBuffer.push_back(0xFF);
        udpBuffer.push_back(0xFF);

        uint16_t totalCount = static_cast<uint16_t>(context.interactionCount);
        udpBuffer.push_back(static_cast<uint8_t>((totalCount >> 8) & 0xFF));
        udpBuffer.push_back(static_cast<uint8_t>(totalCount & 0xFF));
    }

    // 插入报警数据，格式 EE00 + 错误码(2字节)
    for (uint16_t code : context.alarmCodes)
    {
        udpBuffer.push_back(0xEE);
        udpBuffer.push_back(0x00);
        udpBuffer.push_back(static_cast<uint8_t>((code >> 8) & 0xFF));
        udpBuffer.push_back(static_cast<uint8_t>(code & 0xFF));
    }

    // 添加按钮交互计数数据，无论interactionCount是否为0，只要有按钮交互数据就发送
    for (const auto& typePair : context.interactionCounts)
    {
        int btnType = typePair.first + 0x9F;
        for (const auto& idPair : typePair.second)
        {
            int btnID = idPair.first;
            int count = idPair.second;

            udpBuffer.push_back(static_cast<uint8_t>(btnType));
            udpBuffer.push_back(static_cast<uint8_t>(btnID));

            uint16_t count16 = static_cast<uint16_t>(count);
            udpBuffer.push_back(static_cast<uint8_t>((count16 >> 8) & 0xFF));
            udpBuffer.push_back(static_cast<uint8_t>(count16 & 0xFF));
        }
    }

    if (udpBuffer.empty())
    {
        return false;
    }

    bool bSent = SendUdpData(deviceIndex, udpBuffer);
    if (bSent)
    {
        context.interactionCounts.clear();
        if (context.interactionCount > 0)
        {
            context.interactionCount = 0;
            context.interactionDurations.clear();
        }
        context.alarmCodes.clear();  // 清空报警缓存
        return true;
    }
    else
    {
        return false;
    }
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\ACTTRACK\\config.ini";

    if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
    if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
    g_iniFile->read(*g_iniStructure);
    auto& ini = *g_iniStructure;

    const auto& deviceSection = BuildDeviceAttribute("device", deviceIndex);
    auto& defaultSection = ini["default"];

    std::map<std::string, std::string> mergedConfig;
    for (auto& kv : defaultSection) mergedConfig[kv.first] = kv.second;
    auto& deviceSectionMap = ini[deviceSection];
    for (auto& kv : deviceSectionMap) mergedConfig[kv.first] = kv.second;

    DeviceContext context;

    if (mergedConfig.count("server_ip"))
        context.remoteIP = mergedConfig["server_ip"];
    if (mergedConfig.count("server_port"))
        context.remotePort = std::stoi(mergedConfig["server_port"]);
    if (mergedConfig.count("local_port"))
        context.localPort = std::stoi(mergedConfig["local_port"]);
    if (mergedConfig.count("report_interval"))
        context.reportIntervalSeconds = std::stoi(mergedConfig["report_interval"]);
    if (mergedConfig.count("heartbeat_interval"))
        context.heartbeatIntervalSeconds = std::stoi(mergedConfig["heartbeat_interval"]);

    // 读取心跳ip和端口，缺省用上报地址
    if (mergedConfig.count("heartbeat_ip"))
        context.heartbeatIP = mergedConfig["heartbeat_ip"];
    else
        context.heartbeatIP = context.remoteIP;

    if (mergedConfig.count("heartbeat_port"))
        context.heartbeatPort = std::stoi(mergedConfig["heartbeat_port"]);
    else
        context.heartbeatPort = context.remotePort;

    auto now = std::chrono::steady_clock::now();
    context.lastReportTime = now;
    context.lastHeartbeatTime = now;

    if (!context.InitSocket())
    {
        return 0;
    }

    // 获取当前进程完整路径，赋值给上下文
    context.processPath = getCurrentProcessFullPath();

    g_deviceContextMap[deviceIndex] = std::move(context);

    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    auto it = g_deviceContextMap.find(deviceIndex);
    if (it == g_deviceContextMap.end())
    {
        // 设备上下文不存在，关闭失败
        return 0;
    }

    DeviceContext& context = it->second;

    try
    {
        if (context.udpSocket->is_open())
        {
            context.udpSocket->close();
        }
    }
    catch (...)
    {
        // 关闭异常，这里可以添加日志
    }

    // 从map中移除上下文，释放资源
    g_deviceContextMap.erase(it);

    return 1;
}


const int funcChannel = 240;      // 1 按钮交互计数
const int btnTypeChannel = 241;
const int btnIDChannel = 242;
const int InteractChannel = 0;    // 用于标识是否进行互动的控制通道

// 报警通道号250，检测报警码并缓存
const int alarmChannel = 250;


IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    auto it = g_deviceContextMap.find(deviceIndex);
    if (it == g_deviceContextMap.end())
        return 1;

    DeviceContext& context = it->second;

    // 交互状态处理
    int currentInteractStatus = InDOStatus[InteractChannel];
    if (context.lastInteractStatus == 0 && currentInteractStatus == 1)
    {
        context.isInteracting = true;
        context.interactStartTime = std::chrono::steady_clock::now();
    }
    else if (context.lastInteractStatus == 1 && currentInteractStatus == 0)
    {
        if (context.isInteracting)
        {
            auto now = std::chrono::steady_clock::now();
            double duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - context.interactStartTime).count();
            context.interactionCount++;
            context.interactionDurations.push_back(duration);
            context.isInteracting = false;
        }
    }
    context.lastInteractStatus = currentInteractStatus;

    int func = InDOStatus[funcChannel];
    bool hasInteraction = false;

    if (func == 1)
    {
        int btnType = InDOStatus[btnTypeChannel];
        int btnID = InDOStatus[btnIDChannel];
        if (btnID != 0)
        {
            context.interactionCounts[btnType][btnID]++;
            hasInteraction = true;
        }
    }

    // 支持自定义按钮类型1~127自增
    for (int btnType = 1; btnType < 128; btnType++)
    {
        int btnID = InDOStatus[btnType];
        if (btnID != 0)
        {
            context.interactionCounts[btnType][btnID]++;
            hasInteraction = true;
        }
    }

    if (InDOStatus[alarmChannel] > 0)
    {
        uint16_t errorCode = static_cast<uint16_t>(InDOStatus[alarmChannel]);
        context.alarmCodes.push_back(errorCode);
    }

    if (context.reportIntervalSeconds <= 0)
    {
        SendInteractionCounts(deviceIndex, context);
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    std::fill_n(OutDOStatus + 1, 254, 0);

    auto it = g_deviceContextMap.find(deviceIndex);
    if (it == g_deviceContextMap.end())
        return 1;

    auto& context = it->second;
    auto now = std::chrono::steady_clock::now();

    if (context.reportIntervalSeconds > 0)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - context.lastReportTime).count();
        if (elapsed >= context.reportIntervalSeconds)
        {
            SendInteractionCounts(deviceIndex, context);
            context.lastReportTime = now;
        }
    }

    if (context.heartbeatIntervalSeconds > 0)
    {
        auto hbElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - context.lastHeartbeatTime).count();
        if (hbElapsed >= context.heartbeatIntervalSeconds)
        {
            context.SendHeartbeatData();
            context.lastHeartbeatTime = now;
        }
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}
