#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <atomic>
#include <vector>
#include <cstdint>
#include "Protocol.h"
#include "ChannelMapping.h"
#include "FrameProcessor.h"

namespace IOHub {

// 设备上下文
class DeviceContext {
public:
    DeviceContext(uint8_t deviceIndex, 
                  std::unique_ptr<IProtocol> protocol,
                  size_t inputCount,
                  size_t outputCount);
    ~DeviceContext();
    
    // 启动工作线程
    void start();
    
    // 停止设备
    void stop();
    
    // 设置DO状态
    bool setDO(const short* doStatus, size_t count);
    
    // 获取DI状态
    bool getDI(uint8_t* diStatus, size_t count);
    
    // 获取通道映射管理器
    ChannelMapping& getMapping() { return mapping_; }
    
    // 设置帧处理器（串口专用）
    void setFrameProcessor(std::unique_ptr<FrameProcessor> processor);
    
    // 设置写入等待时间
    void setWriteWaitMs(int ms) { writeWaitMs_ = ms; }
    
    // 检查协议是否为串口
    bool isSerialProtocol() const;

private:
    void processOutputLoop();
    void processDirtyStatus(const std::map<int, short>& dirtyData);
    
    uint8_t deviceIndex_;
    std::unique_ptr<IProtocol> protocol_;
    ChannelMapping mapping_;
    std::unique_ptr<FrameProcessor> frameProcessor_;  // 串口专用
    
    size_t inputCount_;
    size_t outputCount_;
    int writeWaitMs_ = 60;
    
    std::thread workerThread_;
    std::atomic<bool> stopFlag_{false};
    
    std::queue<std::map<int, short>> dirtyQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    std::vector<short> lastDOStatus_;
    
    static constexpr size_t MAX_QUEUE_SIZE = 100;
};

} // namespace IOHub