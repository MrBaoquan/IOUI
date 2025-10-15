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
#include <deque>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <Windows.h>

#include <boost/asio.hpp>

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Util.hpp"
#include "mIni/mini/ini.h"

namespace dh = DevelopHelper;
using boost::asio::ip::udp;
using boost::asio::ip::tcp;

typedef unsigned char BYTE;

DeviceInfo devInfo;

// 协议类型枚举
enum class ProtocolType {
    UDP,
    TCP
};

// 全局设备上下文管理
std::map<uint8_t, std::unique_ptr<struct DeviceContext>> g_devices;

// 接收数据ASCII字符串 → DI通道号 映射
std::map<std::string, uint8_t> g_receiveDataToDIChannelMap;

// DO通道号 → 发送数据包内容（字符串）
std::map<int, std::string> g_doChannelToSendDataMap;

// 网络客户端基类
struct NetworkClient {
    std::mutex recv_mutex;
    std::deque<std::string> recv_data;

    virtual ~NetworkClient() = default;
    virtual void sendData(const char* data, size_t size) = 0;
    virtual bool getData(std::string& outStr) {
        std::lock_guard<std::mutex> lock(recv_mutex);
        if (!recv_data.empty()) {
            outStr = std::move(recv_data.front());
            recv_data.pop_front();
            return true;
        }
        return false;
    }
    virtual void flush() {
        std::lock_guard<std::mutex> lock(recv_mutex);
        recv_data.clear();
    }
    virtual void stop() = 0;
    virtual bool isConnected() = 0;
};

// UDP客户端
struct UdpClient : public NetworkClient {
    udp::socket socket;
    udp::endpoint remote_endpoint;
    udp::endpoint sender_endpoint_;
    std::array<uint8_t, 1024> recv_buffer;

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
                    // 构造字符串
                    std::string recvStr(reinterpret_cast<char*>(recv_buffer.data()), bytes_recvd);

                    // 移除尾部的所有空字节
                    while (!recvStr.empty() && recvStr.back() == '\0') {
                        recvStr.pop_back();
                    }

                    // 只有非空字符串才加入队列
                    if (!recvStr.empty()) {
                        std::lock_guard<std::mutex> lock(recv_mutex);
                        recv_data.push_back(std::move(recvStr));
                    }
                }
                if (socket.is_open())
                    startReceive();
            });
    }

    void sendData(const char* data, size_t size) override {
        boost::system::error_code ignored_ec;
        socket.send_to(boost::asio::buffer(data, size), remote_endpoint, 0, ignored_ec);
    }

    void stop() override {
        boost::system::error_code ec;
        socket.cancel(ec);
        socket.close(ec);
    }

    bool isConnected() override {
        return socket.is_open();
    }
};

// TCP客户端（支持断线重连）
struct TcpClient : public NetworkClient {
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    tcp::endpoint remote_endpoint_;
    std::array<uint8_t, 1024> recv_buffer_;

    std::atomic<bool> connected_{ false };
    std::atomic<bool> stop_flag_{ false };
    std::thread reconnect_thread_;
    int reconnect_interval_ms_ = 3000;
    bool enable_keepalive_ = true;

    // 添加接收状态标志
    std::atomic<bool> receiving_{ false };

    TcpClient(boost::asio::io_context& io_context,
        const tcp::endpoint& remote,
        int reconnectIntervalMs = 3000,
        bool enableKeepalive = true)
        : io_context_(io_context)
        , socket_(io_context)
        , remote_endpoint_(remote)
        , reconnect_interval_ms_(reconnectIntervalMs)
        , enable_keepalive_(enableKeepalive)
    {
    }

    void start() {
        stop_flag_.store(false);
        // 启动连接
        doConnect();

        // 启动重连线程
        reconnect_thread_ = std::thread([this]() {
            reconnectLoop();
            });
    }

    ~TcpClient() {
        stop();
    }

    void doConnect() {
        boost::system::error_code ec;

        // 关闭旧连接
        socket_.close(ec);
        socket_ = tcp::socket(io_context_);

        std::cout << "Attempting to connect to " << remote_endpoint_ << std::endl;

        socket_.connect(remote_endpoint_, ec);

        if (!ec) {
            connected_.store(true);
            std::cout << "Connected successfully to " << remote_endpoint_ << std::endl;

            // 设置TCP keepalive
            if (enable_keepalive_) {
                boost::asio::socket_base::keep_alive option(true);
                socket_.set_option(option, ec);
                if (!ec) {
                    std::cout << "TCP keepalive enabled" << std::endl;
                }
            }

            // 连接成功后立即开始接收
            startReceive();
        }
        else {
            connected_.store(false);
            std::cout << "Failed to connect: " << ec.message() << std::endl;
        }
    }

    void reconnectLoop() {
        while (!stop_flag_.load()) {
            if (!connected_.load() && !stop_flag_.load()) {
                std::cout << "Connection lost, attempting to reconnect..." << std::endl;
                doConnect();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_interval_ms_));
        }
        std::cout << "Reconnect loop stopped" << std::endl;
    }

    void startReceive() {
        // 防止重复启动接收
        if (receiving_.exchange(true)) {
            return;
        }

        if (!socket_.is_open() || !connected_.load()) {
            receiving_.store(false);
            return;
        }

        std::cout << "Starting async receive..." << std::endl;

        socket_.async_read_some(
            boost::asio::buffer(recv_buffer_),
            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                receiving_.store(false); // 重置接收状态

                if (!ec && bytes_recvd > 0) {
                    // 构造字符串
                    std::string recvStr(reinterpret_cast<char*>(recv_buffer_.data()), bytes_recvd);

                    // 移除尾部的所有空字节
                    while (!recvStr.empty() && recvStr.back() == '\0') {
                        recvStr.pop_back();
                    }

                    std::cout << "TCP received " << bytes_recvd << " bytes: " << recvStr << std::endl;

                    // 只有非空字符串才加入队列
                    if (!recvStr.empty()) {
                        std::lock_guard<std::mutex> lock(recv_mutex);
                        recv_data.push_back(std::move(recvStr));
                    }

                    // 继续接收下一个数据包
                    if (connected_.load() && !stop_flag_.load()) {
                        startReceive();
                    }
                }
                else {
                    // 连接出错，标记为断开
                    if (ec) {
                        std::cout << "TCP receive error: " << ec.message() << std::endl;
                    }
                    connected_.store(false);
                    boost::system::error_code ec_close;
                    socket_.close(ec_close);
                }
            });
    }

    void sendData(const char* data, size_t size) override {
        if (!connected_.load()) {
            std::cout << "Cannot send data: not connected" << std::endl;
            return;
        }

        boost::system::error_code ec;
        std::cout << "Sending TCP data: " << std::string(data, size) << std::endl;

        boost::asio::write(socket_, boost::asio::buffer(data, size), ec);

        if (ec) {
            // 发送失败，标记为断开
            std::cout << "TCP send error: " << ec.message() << std::endl;
            connected_.store(false);
            boost::system::error_code ec_close;
            socket_.close(ec_close);
        }
        else {
            std::cout << "TCP data sent successfully" << std::endl;
        }
    }

    void stop() override {
        std::cout << "Stopping TCP client..." << std::endl;
        stop_flag_.store(true);
        connected_.store(false);

        boost::system::error_code ec;
        socket_.cancel(ec);
        socket_.close(ec);

        if (reconnect_thread_.joinable()) {
            reconnect_thread_.join();
        }
        std::cout << "TCP client stopped" << std::endl;
    }

    bool isConnected() override {
        return connected_.load();
    }
};

const size_t MAX_QUEUE_SIZE = 100;

boost::asio::io_context g_io_context;
std::thread g_ioThread;

std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

struct DeviceContext {
    uint8_t deviceIndex;
    ProtocolType protocolType;
    std::shared_ptr<NetworkClient> networkClient;
    std::thread workerThread;
    std::atomic<bool> stopFlag{ false };

    std::queue<std::map<int, short>> dirtyQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    int waitMs = 60;
    std::vector<short> lastDOStatus;

    DeviceContext(uint8_t idx, int waitTime, ProtocolType protocol, std::shared_ptr<NetworkClient> client)
        : deviceIndex(idx), waitMs(waitTime), protocolType(protocol), networkClient(std::move(client)) {
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
        std::cout << "Device " << (int)deviceIndex << " process loop started" << std::endl;

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

            if (dirtyData.empty()) continue;

            if (!networkClient || !networkClient->isConnected()) {
                std::cout << "Device " << (int)deviceIndex << " network not ready" << std::endl;
                continue;
            }

            for (const auto& item : dirtyData) {
                if (!networkClient->isConnected()) break;
                int channel = item.first;

                auto itSend = g_doChannelToSendDataMap.find(channel);
                if (itSend == g_doChannelToSendDataMap.end()) continue;

                const auto& dataToSendStr = itSend->second;
                try {
                    networkClient->sendData(dataToSendStr.data(), dataToSendStr.size());
                }
                catch (...) {
                    std::cout << "Exception during send data" << std::endl;
                    break;
                }

                if (waitMs > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
            }
        }

        std::cout << "Device " << (int)deviceIndex << " process loop stopped" << std::endl;
    }

    void stop() {
        std::cout << "Stopping device " << (int)deviceIndex << std::endl;
        stopFlag.store(true, std::memory_order_release);
        queueCV.notify_all();

        if (workerThread.joinable())
            workerThread.join();

        if (networkClient)
            networkClient->stop();
    }
};

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 255;
    devInfo.OutputCount = 255;
    devInfo.AxisCount = 0;
    return &devInfo;
}

// 构造UDP endpoint
udp::endpoint BuildUdpEndpoint(const std::string& ip, uint16_t port) {
    return udp::endpoint(boost::asio::ip::address::from_string(ip), port);
}

// 构造TCP endpoint
tcp::endpoint BuildTcpEndpoint(const std::string& ip, uint16_t port) {
    return tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
}

// 加载InputMapping和OutputMapping映射配置
void LoadMappings(mINI::INIStructure& ini) {
    g_receiveDataToDIChannelMap.clear();
    g_doChannelToSendDataMap.clear();

    if (ini.has("InputMapping")) {
        auto& inputMap = ini["InputMapping"];
        for (const auto& kv : inputMap) {
            int channel = std::stoi(kv.first);
            std::string asciiData = kv.second;
            g_receiveDataToDIChannelMap[asciiData] = (uint8_t)channel;
            std::cout << "Input mapping: '" << asciiData << "' -> DI channel " << channel << std::endl;
        }
    }

    if (ini.has("OutputMapping")) {
        auto& outputMap = ini["OutputMapping"];
        for (const auto& kv : outputMap) {
            int channel = std::stoi(kv.first);
            std::string strData = kv.second;
            g_doChannelToSendDataMap[channel] = strData;
            std::cout << "Output mapping: DO channel " << channel << " -> '" << strData << "'" << std::endl;
        }
    }
}

IOUI_API int __stdcall OpenDevice(uint8_t deviceIndex)
{
    try {
        std::cout << "Opening device " << (int)deviceIndex << std::endl;

        if (g_devices.count(deviceIndex)) {
            std::cout << "Device " << (int)deviceIndex << " already exists, closing first" << std::endl;
            g_devices[deviceIndex]->stop();
            g_devices.erase(deviceIndex);
        }

        std::string path = dh::Paths::Instance().GetModuleDir();
        std::string config_file_path = path + "Config\\NET-BRIDGE\\config.ini";
        std::cout << "Loading config from: " << config_file_path << std::endl;

        if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
        if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
        g_iniFile->read(*g_iniStructure);
        auto& ini = *g_iniStructure;

        auto deviceSectionName = BuildDeviceAttribute("device", deviceIndex);
        auto& defaultSection = ini["default"];
        std::map<std::string, std::string> mergedConfig;
        for (auto& kv : defaultSection) mergedConfig[kv.first] = kv.second;
        auto& deviceSectionMap = ini[deviceSectionName];
        for (auto& kv : deviceSectionMap) mergedConfig[kv.first] = kv.second;

        int waitTimeMs = 60;
        if (mergedConfig.count("write_wait_ms")) waitTimeMs = std::stoi(mergedConfig["write_wait_ms"]);

        // 读取协议类型
        ProtocolType protocol = ProtocolType::UDP;
        if (mergedConfig.count("protocol")) {
            std::string protocolStr = mergedConfig["protocol"];
            std::transform(protocolStr.begin(), protocolStr.end(), protocolStr.begin(), ::tolower);
            if (protocolStr == "tcp") {
                protocol = ProtocolType::TCP;
            }
        }

        std::string remoteIp = "127.0.0.1";
        uint16_t remotePort = 4000 + deviceIndex;
        uint16_t localPort = 5000 + deviceIndex;

        if (mergedConfig.count("remote_ip")) remoteIp = mergedConfig["remote_ip"];
        if (mergedConfig.count("remote_port")) remotePort = static_cast<uint16_t>(std::stoi(mergedConfig["remote_port"]));
        if (mergedConfig.count("local_port")) localPort = static_cast<uint16_t>(std::stoi(mergedConfig["local_port"]));

        // TCP专用配置
        int tcpReconnectInterval = 3000;
        bool tcpKeepalive = true;
        if (mergedConfig.count("tcp_reconnect_interval_ms"))
            tcpReconnectInterval = std::stoi(mergedConfig["tcp_reconnect_interval_ms"]);
        if (mergedConfig.count("tcp_keepalive")) {
            std::string keepaliveStr = mergedConfig["tcp_keepalive"];
            std::transform(keepaliveStr.begin(), keepaliveStr.end(), keepaliveStr.begin(), ::tolower);
            tcpKeepalive = (keepaliveStr == "true" || keepaliveStr == "1");
        }

        std::cout << "Protocol: " << (protocol == ProtocolType::TCP ? "TCP" : "UDP") << std::endl;
        std::cout << "Remote: " << remoteIp << ":" << remotePort << std::endl;
        std::cout << "Local port: " << localPort << std::endl;

        // 确保io_context在独立线程中运行
        if (!g_ioThread.joinable()) {
            std::cout << "Starting IO thread..." << std::endl;
            g_ioThread = std::thread([]() {
                boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
                    work_guard(g_io_context.get_executor());

                std::cout << "IO thread started" << std::endl;
                g_io_context.run();
                std::cout << "IO thread stopped" << std::endl;
                });
        }

        LoadMappings(ini);

        std::shared_ptr<NetworkClient> networkClient;

        if (protocol == ProtocolType::UDP) {
            udp::endpoint remoteEndpoint = BuildUdpEndpoint(remoteIp, remotePort);
            networkClient = std::make_shared<UdpClient>(g_io_context, remoteEndpoint, localPort);
            std::cout << "UDP client created" << std::endl;
        }
        else {
            tcp::endpoint remoteEndpoint = BuildTcpEndpoint(remoteIp, remotePort);
            auto tcpClient = std::make_shared<TcpClient>(g_io_context, remoteEndpoint,
                tcpReconnectInterval, tcpKeepalive);
            tcpClient->start();  // 调用start方法启动连接和重连线程
            networkClient = tcpClient;
            std::cout << "TCP client created and started" << std::endl;
        }

        auto deviceContext = std::make_unique<DeviceContext>(deviceIndex, waitTimeMs, protocol, networkClient);
        deviceContext->workerThread = std::thread(&DeviceContext::processLoop, deviceContext.get());

        g_devices[deviceIndex] = std::move(deviceContext);

        std::cout << "Device " << (int)deviceIndex << " opened successfully" << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cout << "Exception in OpenDevice: " << e.what() << std::endl;
        return 0;
    }
    catch (...) {
        std::cout << "Unknown exception in OpenDevice" << std::endl;
        return 0;
    }
}

IOUI_API int __stdcall CloseDevice(uint8_t deviceIndex)
{
    std::cout << "Closing device " << (int)deviceIndex << std::endl;

    if (g_devices.count(deviceIndex)) {
        g_devices[deviceIndex]->stop();
        g_devices.erase(deviceIndex);
    }

    if (g_devices.empty()) {
        std::cout << "All devices closed, stopping IO context" << std::endl;
        g_io_context.stop();
        if (g_ioThread.joinable())
            g_ioThread.join();

        g_io_context.restart();
    }
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8_t deviceIndex, short* InDOStatus)
{
    auto it = g_devices.find(deviceIndex);
    if (it == g_devices.end()) {
        std::cout << "Device " << (int)deviceIndex << " not found for SetDeviceDO" << std::endl;
        return 0;
    }

    auto& ctx = it->second;
    if (!ctx) return 0;

    std::map<int, short> dirtyStatus;
    for (size_t i = 0; i < devInfo.OutputCount; ++i) {
        if (InDOStatus[i] > 0) {
            dirtyStatus[i] = InDOStatus[i];
        }
    }

    if (!dirtyStatus.empty()) {
        std::map<int, short> toSend;
        for (const auto& kv : dirtyStatus) {
            int ch = kv.first;
            if (g_doChannelToSendDataMap.count(ch) > 0) {
                toSend[ch] = kv.second;
                std::cout << "Queuing DO channel " << ch << " for send" << std::endl;
            }
        }
        if (!toSend.empty()) {
            ctx->pushDirtyStatus(toSend);
        }
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8_t, short* OutDOStatus) {
    std::fill_n(OutDOStatus, devInfo.OutputCount, 0);
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8_t deviceIndex, BYTE* OutDIStatus)
{
    if (g_devices.count(deviceIndex) == 0) {
        ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
        return 0;
    }

    auto& ctx = g_devices[deviceIndex];
    if (!ctx || !ctx->networkClient) {
        std::cout << "Device " << (int)deviceIndex << " context or network client is null" << std::endl;
        ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
        return 0;
    }

    ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);

    std::string recvStr;
    int messageCount = 0;

    while (ctx->networkClient->getData(recvStr)) {
        messageCount++;
        std::cout << "Device " << (int)deviceIndex << " received: '" << recvStr << "'" << std::endl;

        auto it = g_receiveDataToDIChannelMap.find(recvStr);
        if (it != g_receiveDataToDIChannelMap.end()) {
            uint8_t ch = it->second;
            if (ch < devInfo.InputCount) {
                OutDIStatus[ch] = 1;
                std::cout << "Set DI channel " << (int)ch << " to 1" << std::endl;
            }
        }
        else {
            std::cout << "No mapping found for received data: '" << recvStr << "'" << std::endl;
        }
    }

    if (messageCount == 0) {
        // 只在首次调用时输出，避免日志过多
        static bool firstCall = true;
        if (firstCall) {
            std::cout << "No messages received for device " << (int)deviceIndex << std::endl;
            firstCall = false;
        }
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8_t, short*) { return 0; }
IOUI_API int __stdcall RefreshStreamingData(uint8_t, BYTE*, unsigned int) { return 0; }
