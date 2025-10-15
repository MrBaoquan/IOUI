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
#include <chrono>
#include "Protocol.h"
#include "ChannelMapping.h"
#include "FrameProcessor.h"

namespace IOHub {

class DeviceContext {
public:
    DeviceContext(uint8_t deviceIndex, 
                  std::unique_ptr<IProtocol> protocol,
                  size_t inputCount,
                  size_t outputCount);
    ~DeviceContext();
    
    void start();
    void stop();
    
    bool setDO(const short* doStatus, size_t count);
    bool getDI(uint8_t* diStatus, size_t count);
    
    void setFrameProcessor(std::unique_ptr<FrameProcessor> processor);
    
    void setWriteWaitMs(int ms) { writeWaitMs_ = ms; }
    
    void setInputTimeout(int timeoutMs);
    
    ChannelMapping& getMapping() { return mapping_; }
    
private:
    static constexpr size_t MAX_QUEUE_SIZE = 100;
    
    void processOutputLoop();
    void processDirtyStatus(const std::map<int, short>& dirtyData);
    bool isSerialProtocol() const;
    
    uint8_t deviceIndex_;
    std::unique_ptr<IProtocol> protocol_;
    std::unique_ptr<FrameProcessor> frameProcessor_;
    ChannelMapping mapping_;
    
    size_t inputCount_;
    size_t outputCount_;
    
    // 输出状态
    std::vector<short> lastDOStatus_;
    
    // ✅ 输入状态（持久化）
    std::vector<uint8_t> diStatus_;
    std::vector<std::chrono::steady_clock::time_point> diTimestamps_;
    int inputTimeoutMs_;
    std::mutex diMutex_;
    
    // 脏数据队列
    std::queue<std::map<int, short>> dirtyQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    // 工作线程
    std::thread workerThread_;
    std::atomic<bool> stopFlag_{false};
    
    int writeWaitMs_{60};
};

} // namespace IOHub