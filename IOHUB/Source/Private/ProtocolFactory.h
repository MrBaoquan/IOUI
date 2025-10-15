#pragma once
#include "Protocol.h"
#include <memory>
#include <string>
#include <cstdint>

namespace boost {
    namespace asio {
        class io_context;
    }
}

namespace IOHub {

// 协议配置基类
struct ProtocolConfig {
    ProtocolType type;
    virtual ~ProtocolConfig() = default;
};

// UDP配置
struct UdpConfig : public ProtocolConfig {
    std::string remoteIp = "127.0.0.1";
    uint16_t remotePort = 4000;
    uint16_t localPort = 5000;
    
    UdpConfig() { type = ProtocolType::UDP; }
};

// TCP客户端配置
struct TcpConfig : public ProtocolConfig {
    std::string remoteIp = "127.0.0.1";
    uint16_t remotePort = 4000;
    int reconnectIntervalMs = 3000;
    bool enableKeepalive = true;
    
    TcpConfig() { type = ProtocolType::TCP; }
};

// TCP服务器配置
struct TcpServerConfig : public ProtocolConfig {
    std::string listenIp = "0.0.0.0";
    uint16_t listenPort = 8080;
    bool enableKeepalive = true;
    
    TcpServerConfig() { type = ProtocolType::TCP_SERVER; }
};

// 串口配置（扩展）
struct SerialConfig : public ProtocolConfig {
    std::string portName;
    int baudRate = 57600;
    int dataBits = 8;          // 数据位 (5, 6, 7, 8)
    int stopBits = 1;          // 停止位 (1, 2)
    std::string parity = "none"; // 校验位 (none, odd, even, mark, space)
    
    SerialConfig() { type = ProtocolType::SERIAL; }
};

// 协议工厂
class ProtocolFactory {
public:
    // 创建协议实例
    static std::unique_ptr<IProtocol> create(
        const ProtocolConfig& config,
        boost::asio::io_context* ioContext = nullptr);  // UDP/TCP需要
};

} // namespace IOHub