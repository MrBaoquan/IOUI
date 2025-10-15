#pragma once
#include <string>
#include <vector>
#include <memory>
#include <deque>
#include <mutex>
#include <cstdint>

namespace IOHub {

// 协议类型枚举
enum class ProtocolType {
    UDP,
    TCP,
    TCP_SERVER,
    SERIAL
};

// 协议接口基类
class IProtocol {
public:
    virtual ~IProtocol() = default;
    
    // 发送数据
    virtual bool send(const uint8_t* data, size_t size) = 0;
    
    // 接收数据（非阻塞）返回接收到的字节数
    virtual size_t receive(std::vector<uint8_t>& outData) = 0;
    
    // 清空接收缓冲区
    virtual void flush() = 0;
    
    // 停止协议
    virtual void stop() = 0;
    
    // 检查连接状态
    virtual bool isConnected() const = 0;
    
    // 获取协议类型
    virtual ProtocolType getType() const = 0;
};

} // namespace IOHub