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
#include <array>
#include <iostream>

#include <boost/asio.hpp>

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Util.hpp"
#include "mIni/mini/ini.h"

namespace dh = DevelopHelper;
using boost::asio::ip::udp;

DeviceInfo devInfo;

// UDP客户端封装，支持指定本地UDP端口号
struct UdpClient {
    udp::socket socket;
    udp::endpoint remote_endpoint;
    udp::endpoint sender_endpoint_;    // 接收时用来存储远端地址
    std::array<uint8_t, 1024> recv_buffer;
    std::mutex recv_mutex;
    std::vector<uint8_t> recv_data;

    UdpClient(boost::asio::io_context& io_context, const udp::endpoint& remote, unsigned short localPort)
        : socket(io_context, udp::endpoint(udp::v4(), localPort))
        , remote_endpoint(remote)
    {
        startReceive();
    }

    void startReceive() {
        socket.async_receive_from(
            boost::asio::buffer(recv_buffer), sender_endpoint_,
            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    std::lock_guard<std::mutex> lock(recv_mutex);
                    recv_data.insert(recv_data.end(), recv_buffer.begin(), recv_buffer.begin() + bytes_recvd);
                }
                // 只有socket未关闭才继续接收
                if (socket.is_open())
                    startReceive();
            });
    }

    void sendData(const char* data, size_t size) {
        boost::system::error_code ignored_ec;
        socket.send_to(boost::asio::buffer(data, size), remote_endpoint, 0, ignored_ec);
    }

    bool getData(std::vector<uint8_t>& out, size_t size) {
        if (size != 4) {
            // 这里只支持4字节数据包的提取
            return false;
        }
        std::lock_guard<std::mutex> lock(recv_mutex);

        while (recv_data.size() >= size) {
            // 如果当前位置不是0xFE，说明是无效数据，舍弃直到找到0xFE
            if (recv_data[0] != 0xFE) {
                // 丢弃第一个字节，继续尝试匹配
                recv_data.erase(recv_data.begin());
                continue;
            }

            // 当前位置是0xFE，检查包尾是否为0xFF
            if (recv_data[size - 1] == 0xFF) {
                // 找到有效包，取出返回
                out.assign(recv_data.begin(), recv_data.begin() + size);
                recv_data.erase(recv_data.begin(), recv_data.begin() + size);
                return true;
            }
            else {
                // 包尾不正确，舍弃第一个字节继续查找
                recv_data.erase(recv_data.begin());
            }
        }

        // 未找到符合要求的数据包
        return false;
    }

    void flush() {
        std::lock_guard<std::mutex> lock(recv_mutex);
        recv_data.clear();
    }

    // 新增停止方法，安全关闭socket及取消异步操作
    void stop() {
        boost::system::error_code ec;
        socket.cancel(ec);
        socket.close(ec);
    }
};

const size_t MAX_QUEUE_SIZE = 100;

boost::asio::io_context g_io_context;
std::thread g_ioThread;

std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

struct DeviceContext {
    uint8 deviceIndex;

    std::unique_ptr<UdpClient> udpClient;
    std::thread workerThread;
    std::atomic<bool> stopFlag{ false };

    std::queue<std::map<int, short>> dirtyQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    int waitMs = 60;

    std::vector<short> lastDOStatus;

    DeviceContext(uint8 idx, int waitTime, std::unique_ptr<UdpClient> client)
        : deviceIndex(idx), waitMs(waitTime), udpClient(std::move(client)) {
        lastDOStatus.resize(devInfo.OutputCount, 0);
    }

    void pushDirtyStatus(const std::map<int, short>& dirtyData) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (dirtyQueue.size() >= MAX_QUEUE_SIZE) {
            auto& last = dirtyQueue.back();
            for (const auto& kv : dirtyData) {
                last[kv.first] = kv.second;
            }
        }
        else {
            dirtyQueue.push(dirtyData);
        }
        queueCV.notify_one();
    }

    void processLoop() {
        while (true) {
            std::map<int, short> dirtyData;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                queueCV.wait(lock, [this] {
                    return !dirtyQueue.empty() || stopFlag.load(std::memory_order_acquire);
                    });

                if (stopFlag.load(std::memory_order_acquire) && dirtyQueue.empty())
                    break;

                if (!dirtyQueue.empty()) {
                    dirtyData = std::move(dirtyQueue.front());
                    dirtyQueue.pop();
                }
            }
            if (dirtyData.empty())
                continue;

            if (!udpClient || !udpClient->socket.is_open())
                continue;

            for (const auto& item : dirtyData) {
                if (!udpClient->socket.is_open())
                    break;

                char data[4] = { 0xFE, 0, 0, 0xFF };
                data[1] = static_cast<char>(item.first);
                data[2] = static_cast<char>(item.second);

                try {
                    udpClient->sendData(data, 4);
                }
                catch (...) {
                    break;
                }
                if (waitMs > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
            }
        }
    }

    void stop() {
        stopFlag.store(true, std::memory_order_release);
        queueCV.notify_all();
        if (workerThread.joinable())
            workerThread.join();

        if (udpClient)
            udpClient->stop();
    }
};

std::map<uint8, std::unique_ptr<DeviceContext>> g_devices;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 128;
    devInfo.OutputCount = 128;
    devInfo.AxisCount = 128;
    return &devInfo;
}

udp::endpoint BuildUdpEndpoint(const std::string& ip, uint16_t port) {
    return udp::endpoint(boost::asio::ip::address::from_string(ip), port);
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    try {
        if (g_devices.count(deviceIndex)) {
            g_devices[deviceIndex]->stop();
            g_devices.erase(deviceIndex);
        }

        std::string path = dh::Paths::Instance().GetModuleDir();
        std::string config_file_path = path + "Config\\NET-IOFE\\config.ini";

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

        int waitTimeMs = 60;
        if (mergedConfig.count("write_wait_ms")) waitTimeMs = std::stoi(mergedConfig["write_wait_ms"]);

        std::string remoteIp = "127.0.0.1";
        uint16_t remotePort = 4000 + deviceIndex;
        uint16_t localPort = 5000 + deviceIndex;

        if (mergedConfig.count("remote_ip")) remoteIp = mergedConfig["remote_ip"];
        if (mergedConfig.count("remote_port")) remotePort = static_cast<uint16_t>(std::stoi(mergedConfig["remote_port"]));
        if (mergedConfig.count("local_port")) localPort = static_cast<uint16_t>(std::stoi(mergedConfig["local_port"]));

        udp::endpoint remoteEndpoint = BuildUdpEndpoint(remoteIp, remotePort);

        if (!g_ioThread.joinable()) {
            g_ioThread = std::thread([]() {
                g_io_context.run();
                });
        }

        auto udpClient = std::make_unique<UdpClient>(g_io_context, remoteEndpoint, localPort);

        auto deviceContext = std::make_unique<DeviceContext>(deviceIndex, waitTimeMs, std::move(udpClient));
        deviceContext->workerThread = std::thread(&DeviceContext::processLoop, deviceContext.get());

        g_devices[deviceIndex] = std::move(deviceContext);
    }
    catch (...) {
        return 0;
    }
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    if (g_devices.count(deviceIndex)) {
        g_devices[deviceIndex]->stop();
        g_devices.erase(deviceIndex);
    }

    if (g_devices.empty()) {
        g_io_context.stop();
        if (g_ioThread.joinable())
            g_ioThread.join();

        g_io_context.restart();
    }

    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    auto it = g_devices.find(deviceIndex);
    if (it == g_devices.end())
        return 0;

    auto& ctx = it->second;
    if (!ctx)
        return 0;

    std::map<int, short> dirtyStatus;

    for (size_t i = 0; i < devInfo.OutputCount; ++i) {
        if (i >= ctx->lastDOStatus.size() || ctx->lastDOStatus[i] != InDOStatus[i]) {
            dirtyStatus[i] = InDOStatus[i];
            if (i < ctx->lastDOStatus.size())
                ctx->lastDOStatus[i] = InDOStatus[i];
        }
    }

    if (!dirtyStatus.empty()) {
        ctx->pushDirtyStatus(dirtyStatus);
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    // 您可根据需要完善，本示例暂不实现
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    auto it = g_devices.find(deviceIndex);
    if (it == g_devices.end()) return 0;

    auto& ctx = it->second;
    if (!ctx || !ctx->udpClient) return 0;

    std::vector<uint8_t> recvBuffer;

    while (ctx->udpClient->getData(recvBuffer, 4)) {
        if (recvBuffer.size() != 4)
            break;

        if (recvBuffer[0] != 0xFE) {
            continue;
        }
        uint8 channel = recvBuffer[1];
        uint8 status = recvBuffer[2];

        if (channel >= devInfo.InputCount) {
            continue;
        }
        OutDIStatus[channel] = (status == 0x00) ? 0 : 1;
    }
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

