#include "ProtocolFactory.h"
#include <boost/asio.hpp>
#include "Serial.hpp"
#include <thread>

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace IOHub {

// UDP协议实现
class UdpProtocol : public IProtocol {
public:
    UdpProtocol(boost::asio::io_context& ioContext, const UdpConfig& config)
        : socket_(ioContext, udp::endpoint(udp::v4(), config.localPort))
        , remoteEndpoint_(boost::asio::ip::address::from_string(config.remoteIp), config.remotePort)
    {
        startReceive();
    }
    
    ~UdpProtocol() override {
        stop();
    }
    
    bool send(const uint8_t* data, size_t size) override {
        try {
            boost::system::error_code ec;
            socket_.send_to(boost::asio::buffer(data, size), remoteEndpoint_, 0, ec);
            return !ec;
        } catch (...) {
            return false;
        }
    }
    
    size_t receive(std::vector<uint8_t>& outData) override {
        std::lock_guard<std::mutex> lock(recvMutex_);
        if (!recvQueue_.empty()) {
            outData = std::move(recvQueue_.front());
            recvQueue_.pop_front();
            return outData.size();
        }
        return 0;
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(recvMutex_);
        recvQueue_.clear();
    }
    
    void stop() override {
        boost::system::error_code ec;
        socket_.cancel(ec);
        socket_.close(ec);
    }
    
    bool isConnected() const override {
        return socket_.is_open();
    }
    
    ProtocolType getType() const override {
        return ProtocolType::UDP;
    }

private:
    void startReceive() {
        socket_.async_receive_from(
            boost::asio::buffer(recvBuffer_), senderEndpoint_,
            [this](boost::system::error_code ec, std::size_t bytesRecvd) {
                if (!ec && bytesRecvd > 0) {
                    std::lock_guard<std::mutex> lock(recvMutex_);
                    recvQueue_.emplace_back(recvBuffer_.begin(), recvBuffer_.begin() + bytesRecvd);
                }
                if (!ec) {
                    startReceive();
                }
            });
    }
    
    udp::socket socket_;
    udp::endpoint remoteEndpoint_;
    udp::endpoint senderEndpoint_;
    std::array<uint8_t, 1024> recvBuffer_;
    std::mutex recvMutex_;
    std::deque<std::vector<uint8_t>> recvQueue_;
};

// TCP协议实现
class TcpProtocol : public IProtocol {
public:
    TcpProtocol(boost::asio::io_context& ioContext, const TcpConfig& config)
        : ioContext_(ioContext)
        , socket_(ioContext)
        , remoteEndpoint_(boost::asio::ip::address::from_string(config.remoteIp), config.remotePort)
        , reconnectIntervalMs_(config.reconnectIntervalMs)
        , enableKeepalive_(config.enableKeepalive)
    {
        stopFlag_.store(false);
        doConnect();
        
        // 启动重连线程
        reconnectThread_ = std::thread([this]() {
            while (!stopFlag_.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(reconnectIntervalMs_));
                if (!connected_.load() && !stopFlag_.load()) {
                    doConnect();
                }
            }
        });
    }
    
    ~TcpProtocol() override {
        stop();
    }
    
    bool send(const uint8_t* data, size_t size) override {
        if (!connected_.load()) {
            return false;
        }
        
        try {
            boost::system::error_code ec;
            boost::asio::write(socket_, boost::asio::buffer(data, size), ec);
            if (ec) {
                connected_.store(false);
                return false;
            }
            return true;
        } catch (...) {
            connected_.store(false);
            return false;
        }
    }
    
    size_t receive(std::vector<uint8_t>& outData) override {
        std::lock_guard<std::mutex> lock(recvMutex_);
        if (!recvQueue_.empty()) {
            outData = std::move(recvQueue_.front());
            recvQueue_.pop_front();
            return outData.size();
        }
        return 0;
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(recvMutex_);
        recvQueue_.clear();
    }
    
    void stop() override {
        stopFlag_.store(true);
        
        if (reconnectThread_.joinable()) {
            reconnectThread_.join();
        }
        
        boost::system::error_code ec;
        socket_.cancel(ec);
        socket_.close(ec);
        
        connected_.store(false);
    }
    
    bool isConnected() const override {
        return connected_.load();
    }
    
    ProtocolType getType() const override {
        return ProtocolType::TCP;
    }

private:
    void doConnect() {
        boost::system::error_code ec;
        
        socket_.close(ec);
        socket_ = tcp::socket(ioContext_);
        
        socket_.connect(remoteEndpoint_, ec);
        
        if (!ec) {
            connected_.store(true);
            
            // 设置keepalive
            if (enableKeepalive_) {
                boost::asio::socket_base::keep_alive option(true);
                socket_.set_option(option, ec);
            }
            
            startReceive();
        } else {
            connected_.store(false);
        }
    }
    
    void startReceive() {
        if (!socket_.is_open() || !connected_.load()) {
            return;
        }
        
        socket_.async_read_some(
            boost::asio::buffer(recvBuffer_),
            [this](boost::system::error_code ec, std::size_t bytesRecvd) {
                if (!ec && bytesRecvd > 0) {
                    std::lock_guard<std::mutex> lock(recvMutex_);
                    recvQueue_.emplace_back(recvBuffer_.begin(), recvBuffer_.begin() + bytesRecvd);
                    startReceive();
                } else {
                    connected_.store(false);
                }
            });
    }
    
    boost::asio::io_context& ioContext_;
    tcp::socket socket_;
    tcp::endpoint remoteEndpoint_;
    int reconnectIntervalMs_;
    bool enableKeepalive_;
    
    std::atomic<bool> connected_{false};
    std::atomic<bool> stopFlag_{false};
    std::thread reconnectThread_;
    
    std::array<uint8_t, 1024> recvBuffer_;
    std::mutex recvMutex_;
    std::deque<std::vector<uint8_t>> recvQueue_;
};

// 串口协议实现 - 修复版
class SerialProtocol : public IProtocol {
public:
    explicit SerialProtocol(const SerialConfig& config)
        : isOpen_(false)
    {
        try {
            // Serial类在构造函数中打开串口
            serialPort_ = std::make_unique<Serial>(
                config.portName,
                config.baudRate,
                ONESTOPBIT
            );
            
            isOpen_ = true;
        }
        catch (const char* errMsg) {
            throw std::runtime_error(std::string("Failed to open serial port: ") + errMsg);
        }
        catch (const std::exception& e) {
            throw;
        }
        catch (...) {
            throw std::runtime_error("Unknown error opening serial port");
        }
    }
    
    ~SerialProtocol() override {
        stop();
    }
    
    bool send(const uint8_t* data, size_t size) override {
        if (!isOpen_ || !serialPort_) {
            return false;
        }
        
        try {
            int written = serialPort_->write(reinterpret_cast<const char*>(data), static_cast<int>(size));
            return written == static_cast<int>(size);
        } 
        catch (...) {
            return false;
        }
    }
    
    size_t receive(std::vector<uint8_t>& outData) override {
        if (!isOpen_ || !serialPort_) {
            outData.clear();
            return 0;
        }
        
        const size_t bufferSize = 1024;
        outData.resize(bufferSize);
        
        try {
            // Serial::read() 默认会添加null终止符，所以我们需要传false
            int bytesRead = serialPort_->read(reinterpret_cast<char*>(outData.data()), 
                                             static_cast<int>(bufferSize), 
                                             false);  // 不要null终止
            
            if (bytesRead > 0) {
                outData.resize(bytesRead);
                return static_cast<size_t>(bytesRead);
            }
        } 
        catch (...) {
            // 读取失败
        }
        
        outData.clear();
        return 0;
    }
    
    void flush() override {
        if (isOpen_ && serialPort_) {
            try {
                serialPort_->flush();
            }
            catch (...) {
                // 忽略flush错误
            }
        }
    }
    
    void stop() override {
        if (isOpen_) {
            isOpen_ = false;
            // Serial的析构函数会自动关闭串口
            serialPort_.reset();
        }
    }
    
    bool isConnected() const override {
        return isOpen_;
    }
    
    ProtocolType getType() const override {
        return ProtocolType::SERIAL;
    }

private:
    std::unique_ptr<Serial> serialPort_;
    bool isOpen_;
};

// TCP服务器协议实现（多客户端模式）
class TcpServerProtocol : public IProtocol {
public:
    TcpServerProtocol(boost::asio::io_context& ioContext, const TcpServerConfig& config)
        : ioContext_(ioContext)
        , acceptor_(ioContext)
        , config_(config)
    {
        OutputDebugStringA("[TCP-Server] Constructor called\n");
        
        tcp::endpoint endpoint(
            boost::asio::ip::address::from_string(config.listenIp),
            config.listenPort
        );
        
        char msg[256];
        sprintf_s(msg, "[TCP-Server] Binding to %s:%d\n", 
                 config.listenIp.c_str(), config.listenPort);
        OutputDebugStringA(msg);
        
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        
        sprintf_s(msg, "[TCP-Server] Listening on port %d\n", config.listenPort);
        OutputDebugStringA(msg);
        
        startAccept();
        
        OutputDebugStringA("[TCP-Server] Constructor finished\n");
    }
    
    ~TcpServerProtocol() override {
        stop();
    }
    
    bool send(const uint8_t* data, size_t size) override {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        
        if (clients_.empty()) {
            return false;
        }
        
        bool anySuccess = false;
        
        // 向所有连接的客户端发送数据
        for (auto it = clients_.begin(); it != clients_.end(); ) {
            auto& client = *it;
            
            try {
                boost::system::error_code ec;
                boost::asio::write(*client, boost::asio::buffer(data, size), ec);
                
                if (ec) {
                    // 发送失败，移除客户端
                    it = clients_.erase(it);
                } else {
                    anySuccess = true;
                    ++it;
                }
            }
            catch (...) {
                it = clients_.erase(it);
            }
        }
        
        return anySuccess;
    }
    
    size_t receive(std::vector<uint8_t>& outData) override {
        std::lock_guard<std::mutex> lock(recvMutex_);
        if (recvQueue_.empty()) {
            return 0;
        }
        
        outData = std::move(recvQueue_.front());
        recvQueue_.pop_front();
        return outData.size();
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(recvMutex_);
        recvQueue_.clear();
    }
    
    void stop() override {
        boost::system::error_code ec;
        
        {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            for (auto& client : clients_) {
                if (client && client->is_open()) {
                    client->shutdown(tcp::socket::shutdown_both, ec);
                    client->close(ec);
                }
            }
            clients_.clear();
        }
        
        acceptor_.close(ec);
    }
    
    bool isConnected() const override {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        return !clients_.empty();
    }
    
    ProtocolType getType() const override {
        return ProtocolType::TCP_SERVER;
    }
    
private:
    void startAccept() {
        auto newSocket = std::make_shared<tcp::socket>(ioContext_);
        
        acceptor_.async_accept(*newSocket, [this, newSocket](const boost::system::error_code& ec) {
            if (!ec) {
                // 配置 keepalive
                if (config_.enableKeepalive) {
                    boost::system::error_code optEc;
                    newSocket->set_option(tcp::socket::keep_alive(true), optEc);
                }
                
                {
                    std::lock_guard<std::mutex> lock(clientsMutex_);
                    clients_.push_back(newSocket);
                }
                
                startReceive(newSocket);
            }
            
            // 继续接受新连接
            if (acceptor_.is_open()) {
                startAccept();
            }
        });
    }
    
    void startReceive(std::shared_ptr<tcp::socket> client) {
        auto buffer = std::make_shared<std::array<uint8_t, 1024>>();
        
        client->async_read_some(
            boost::asio::buffer(*buffer),
            [this, client, buffer](const boost::system::error_code& ec, std::size_t bytes) {
                if (!ec && bytes > 0) {
                    std::vector<uint8_t> data(buffer->begin(), buffer->begin() + bytes);
                    
                    {
                        std::lock_guard<std::mutex> lock(recvMutex_);
                        recvQueue_.push_back(std::move(data));
                    }
                    
                    // 继续接收
                    startReceive(client);
                }
                else {
                    // 客户端断开，移除
                    std::lock_guard<std::mutex> lock(clientsMutex_);
                    auto it = std::find(clients_.begin(), clients_.end(), client);
                    if (it != clients_.end()) {
                        boost::system::error_code closeEc;
                        (*it)->close(closeEc);
                        clients_.erase(it);
                    }
                }
            }
        );
    }
    
    boost::asio::io_context& ioContext_;
    tcp::acceptor acceptor_;
    TcpServerConfig config_;
    
    mutable std::mutex clientsMutex_;
    std::vector<std::shared_ptr<tcp::socket>> clients_;
    
    std::mutex recvMutex_;
    std::deque<std::vector<uint8_t>> recvQueue_;
};

// 工厂方法实现
std::unique_ptr<IProtocol> ProtocolFactory::create(
    const ProtocolConfig& config,
    boost::asio::io_context* ioContext)
{
    switch (config.type) {
        case ProtocolType::UDP: {
            if (!ioContext) {
                throw std::invalid_argument("io_context required for UDP protocol");
            }
            const auto& udpConfig = static_cast<const UdpConfig&>(config);
            return std::make_unique<UdpProtocol>(*ioContext, udpConfig);
        }
        
        case ProtocolType::TCP: {
            if (!ioContext) {
                throw std::invalid_argument("io_context required for TCP protocol");
            }
            const auto& tcpConfig = static_cast<const TcpConfig&>(config);
            return std::make_unique<TcpProtocol>(*ioContext, tcpConfig);
        }
        
        case ProtocolType::SERIAL: {
            const auto& serialConfig = static_cast<const SerialConfig&>(config);
            return std::make_unique<SerialProtocol>(serialConfig);
        }
        
        case ProtocolType::TCP_SERVER: {
            if (!ioContext) {
                return nullptr;
            }
            const auto& tcpServerConfig = static_cast<const TcpServerConfig&>(config);
            return std::make_unique<TcpServerProtocol>(*ioContext, tcpServerConfig);
        }
        
        default:
            throw std::invalid_argument("Unknown protocol type");
    }
}

} // namespace IOHub