#pragma once
#include <vector>
#include <deque>
#include <cstdint>
#include <chrono>
#include <map>

namespace IOHub {

// 帧配置
struct FrameConfig {
    uint8_t header = 0xFE;      // 帧头
    uint8_t tail = 0xFF;        // 帧尾
    size_t length = 4;          // 帧长度
    uint8_t channelIndex = 1;   // 通道号在帧中的索引
    uint8_t valueIndex = 2;     // 值在帧中的索引
    int timeoutMs = 1000;       // 输入通道超时时间（毫秒）
    int channelOffset = 0;      // 通道号偏移量（0=从0开始，1=从1开始）
    
    // 输出值编码（用于 buildFrame）
    uint8_t outputValueOnCode = 0x01;  // 输出高电平编码（默认0x01）
    uint8_t outputValueOffCode = 0x00; // 输出低电平编码（默认0x00）
    
    // 输入值编码（用于 parseFrame）
    uint8_t inputValueOnCode = 0x01;   // 输入高电平编码（默认0x01）
    uint8_t inputValueOffCode = 0x00;  // 输入低电平编码（默认0x00）
};

// 帧处理器（用于串口协议）
class FrameProcessor {
public:
    explicit FrameProcessor(const FrameConfig& config);
    
    // 添加接收数据到缓冲区
    void addReceivedData(const uint8_t* data, size_t size);
    
    // 尝试从缓冲区提取一个完整帧
    bool extractFrame(std::vector<uint8_t>& outFrame);
    
    // 构造标准帧（用于发送）
    std::vector<uint8_t> buildFrame(uint8_t channel, uint8_t value) const;
    
    // 解析标准帧（提取通道号和值）
    bool parseFrame(const std::vector<uint8_t>& frame, uint8_t& outChannel, uint8_t& outValue) const;
    
    // 清空接收缓冲区
    void clearBuffer();
    
    // 更新通道时间戳
    void updateChannelTimestamp(uint8_t channel);
    
    // 检查并清除超时的通道状态
    void checkTimeouts(std::vector<uint8_t>& diStatus);
    
private:
    FrameConfig config_;
    std::deque<uint8_t> recvBuffer_;
    std::map<uint8_t, std::chrono::steady_clock::time_point> channelTimestamps_;
};

} // namespace IOHub