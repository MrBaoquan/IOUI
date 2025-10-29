/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <string>
#include <memory>
#include <queue>
#include <atomic>
#include <set>
#include <chrono>
#include <condition_variable>
#include "IOUI.h"
#include "Paths.hpp"
#include "modbus/modbus.h"
#include "mIni/mini/ini.h"
#include "Util.hpp"

#pragma comment(lib,"modbus.lib")

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 255;
    devInfo.OutputCount = 255;
    devInfo.AxisCount = 255;
    return &devInfo;
}

// 单个通道的配置
struct ChannelConfig {
    int address;           // 寄存器起始地址
    std::string dataType;  // 数据类型: uint16, int16, uint32, int32, float32
    std::string byteOrder; // 字节序: AB, BA
    float scale;           // 缩放系数
    int funcCode;          // 功能码
    
    ChannelConfig() 
        : address(0), dataType("uint16"), byteOrder("AB"), scale(1.0f), funcCode(0x04) {}
};

struct ModbusArgs
{
public:
    int SlaveAddr;
    int DIFuncCode;
    int DIReadAddr;
    int DIReadCount;
    int AIFuncCode;
    int TimeoutMs;         // 超时(ms)
    int RetryWaitMs;       // 重试等待(ms)
    int DoAddr;            // DO写操作起始地址偏移
    int PollIntervalMs;    // 轮询间隔(ms) - 每轮读取完成后的等待时间
    int CommandIntervalMs; // 命令间隔(ms) - 连续读取命令之间的间隔
    int DefaultDoFunc;     // 默认DO功能码
    int MaxConsecutiveFailures; // 断线检测阈值 - 连续失败多少次判定为断线
    
    // AI通道映射配置（统一格式）
    std::map<int, ChannelConfig> channelConfigs;

    ModbusArgs() {
        SlaveAddr = 1;
        DIReadAddr = 0;
        DIReadCount = 0;
        AIFuncCode = 0x04;
        TimeoutMs = 100;
        RetryWaitMs = 20;
        PollIntervalMs = 100;     // 默认100ms轮询一次
        CommandIntervalMs = 10;   // 默认命令间隔10ms
        DoAddr = 0;
        DefaultDoFunc = 5;
        MaxConsecutiveFailures = 3; // 默认连续失败3次判定为断线
    }
};

struct WriteTask {
    int writeType;   // 5=写线圈, 6=写寄存器
    int writeAddr;
    int writeData;
};

std::map<uint8, modbus_t*> g_ctxs;
std::map<uint8, ModbusArgs> g_args;

std::map<uint8, uint16_t*> g_readAIData;
std::map<uint8, uint8_t*> g_readDIData;
std::map<uint8, int> g_diReadCount;
std::map<uint8, int> g_aiReadCount;

// 用于保护读取数据的互斥锁（修复读写线程数据竞争）
std::map<uint8, std::mutex> g_dataMutexes;

std::map<uint8, std::queue<WriteTask>> g_writeQueues;
std::map<uint8, std::mutex> g_queueMutexes;
std::map<uint8, std::condition_variable> g_writeQueueConditions;

// --- 新增：以共享连接Key为单位管理互斥锁，保证共享连接线程安全 ---
std::map<std::string, std::mutex> g_connectionMutexes;

std::map<int, std::vector<short>> lastDOStatusMap;

std::map<std::string, modbus_t*> g_connectionMap;
std::map<std::string, int> g_connectionRefCount;
std::map<uint8, std::string> g_deviceConnectionKey;

std::mutex connectionMutex;

std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

template<typename Func>
int execModbusWithRetry(Func func, int maxRetries, int retryDelayMs) {
    int ret = -1;
    for (int i = 0; i <= maxRetries; ++i) {
        ret = func();
        if (ret != -1) return ret;
        if (i < maxRetries) std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
    }
    return ret;
}

// 新增：线程管理和退出控制
std::map<uint8, std::thread> g_threads;
std::map<uint8, std::atomic<bool>> g_stopFlags;

// 新增：连接状态管理（断线重连机制）
enum class ConnectionState {
    Connected,      // 已连接
    Disconnected,   // 已断线
    Reconnecting    // 重连中
};

std::map<uint8, ConnectionState> g_connectionStates;
std::map<uint8, int> g_consecutiveFailures;  // 连续失败次数
std::map<uint8, std::chrono::steady_clock::time_point> g_lastReconnectAttempt;

// 断线检测和重连配置
const int RECONNECT_INTERVAL_MS = 5000;      // 5秒尝试一次重连
const int RECONNECT_FAST_RETRY_MS = 100;     // 断线后快速等待时间
const int MAX_WRITE_RETRIES = 2;             // 写操作最大重试次数（降低阻塞时间）

// 辅助函数：解析通道配置字符串
// 格式: 地址, 数据类型[, 缩放系数][, 字节序][, 功能码]
// 默认值: 缩放系数=1.0, 字节序=AB, 功能码=defaultFuncCode
ChannelConfig parseChannelConfig(const std::string& configStr, int defaultFuncCode) {
    ChannelConfig config;
    config.scale = 1.0f;           // 默认缩放系数
    config.byteOrder = "AB";       // 默认大端序
    config.funcCode = defaultFuncCode;  // 默认功能码
    
    // 分割字符串: "0x0000, uint16" 或 "0x0000, uint16, 0.1" 或 "0x0000, uint16, 0.1, BA, 0x04"
    std::vector<std::string> parts;
    std::stringstream ss(configStr);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // 去除前后空格
        item.erase(0, item.find_first_not_of(" \t\r\n"));
        item.erase(item.find_last_not_of(" \t\r\n") + 1);
        if (!item.empty()) {
            parts.push_back(item);
        }
    }
    
    // 必需参数
    if (parts.size() >= 1) {
        config.address = std::stoi(parts[0], nullptr, 0);  // 支持0x前缀
    }
    if (parts.size() >= 2) {
        config.dataType = parts[1];
    }
    
    // 可选参数
    if (parts.size() >= 3) {
        config.scale = std::stof(parts[2]);  // 缩放系数
    }
    if (parts.size() >= 4) {
        config.byteOrder = parts[3];  // 字节序
    }
    if (parts.size() >= 5) {
        config.funcCode = std::stoi(parts[4], nullptr, 0);  // 功能码
    }
    
    return config;
}

// 辅助函数：获取数据类型需要的寄存器数量
int getRegisterCount(const std::string& dataType) {
    if (dataType == "uint16" || dataType == "int16") {
        return 1;
    } else if (dataType == "uint32" || dataType == "int32" || dataType == "float32" || dataType == "fixed16_16") {
        return 2;
    }
    return 1;
}

// 辅助函数：解析寄存器值 - 返回uint32作为通用载体
uint32_t parseRegisterValue(const uint16_t* regs, const std::string& dataType, const std::string& byteOrder) {
    if (dataType == "uint16") {
        return regs[0];
    }
    else if (dataType == "int16") {
        return static_cast<uint16_t>(regs[0]);  // 保持位模式
    }
    else if (dataType == "uint32" || dataType == "int32" || dataType == "float32") {
        uint32_t value;
        if (byteOrder == "BA") {
            value = (static_cast<uint32_t>(regs[1]) << 16) | regs[0];
        } else {  // AB (默认)
            value = (static_cast<uint32_t>(regs[0]) << 16) | regs[1];
        }
        return value;
    }
    return 0;
}

// 辅助函数：解析 Q16.16 固定点数格式
// 格式：[符号位1][整数15位][小数16位]
// 算法：value / 65536，最高位为符号位
float parseFixed16_16(const uint16_t* regs, const std::string& byteOrder) {
    // 组合32位数据
    uint32_t rawValue;
    if (byteOrder == "BA") {
        rawValue = (static_cast<uint32_t>(regs[1]) << 16) | regs[0];
    } else {  // AB (默认大端序)
        rawValue = (static_cast<uint32_t>(regs[0]) << 16) | regs[1];
    }
    
    // 检查符号位（最高位bit31）
    bool isNegative = (rawValue & 0x80000000) != 0;
    
    // 清除符号位，保留数值部分（31位）
    if (isNegative) {
        rawValue = rawValue & 0x7FFFFFFF;
    }
    
    // 算法2：直接除以65536
    // 例如：0x0007ADA7 = 503207 → 503207/65536 = 7.678
    float result = static_cast<float>(rawValue) / 65536.0f;
    
    return isNegative ? -result : result;
}

// 辅助函数：限制到short范围
short clampToShort(float value) {
    if (value > 32767.0f) return 32767;
    if (value < -32768.0f) return -32768;
    return static_cast<short>(value);
}

// 辅助函数：尝试重连设备
bool attemptReconnect(uint8 deviceIndex) {
    if (!g_deviceConnectionKey.count(deviceIndex)) return false;
    
    auto connectionKey = g_deviceConnectionKey[deviceIndex];
    auto& _args = g_args[deviceIndex];
    
    // 检查是否到了重连时间
    auto now = std::chrono::steady_clock::now();
    if (g_lastReconnectAttempt.count(deviceIndex)) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_lastReconnectAttempt[deviceIndex]).count();
        if (elapsed < RECONNECT_INTERVAL_MS) {
            return false;  // 还没到重连时间
        }
    }
    
    g_lastReconnectAttempt[deviceIndex] = now;
    g_connectionStates[deviceIndex] = ConnectionState::Reconnecting;
    
    // 获取连接上下文
    auto itCtx = g_ctxs.find(deviceIndex);
    if (itCtx == g_ctxs.end()) return false;
    modbus_t* ctx = itCtx->second;
    
    // 尝试重新连接
    std::lock_guard<std::mutex> lock(g_connectionMutexes[connectionKey]);
    
    // 先关闭旧连接
    modbus_close(ctx);
    
    // 尝试重连
    if (modbus_connect(ctx) != -1) {
        // 重连成功
        g_connectionStates[deviceIndex] = ConnectionState::Connected;
        g_consecutiveFailures[deviceIndex] = 0;
        
        // 重新设置超时
        modbus_set_response_timeout(ctx, _args.TimeoutMs / 1000, (_args.TimeoutMs % 1000) * 1000);
        
        return true;
    }
    
    // 重连失败
    g_connectionStates[deviceIndex] = ConnectionState::Disconnected;
    return false;
}

// 辅助函数：读取DI数据
int queryModbusDI(uint8 deviceIndex) {
    auto itCtx = g_ctxs.find(deviceIndex);
    if (itCtx == g_ctxs.end()) return 0;
    modbus_t* ctx = itCtx->second;

    auto& _args = g_args[deviceIndex];
    auto connectionKey = g_deviceConnectionKey[deviceIndex];

    std::lock_guard<std::mutex> lock(g_connectionMutexes[connectionKey]);

    if (modbus_set_slave(ctx, _args.SlaveAddr) == -1) {
        return -1;
    }

    if (_args.DIReadAddr < 0 || _args.DIReadCount <= 0) return 0;

    auto _readData = g_readDIData[deviceIndex];
    int _readCount = -1;
    
    if (_args.DIFuncCode == 0x01) {
        _readCount = modbus_read_bits(ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
    } else if (_args.DIFuncCode == 0x02) {
        _readCount = modbus_read_input_bits(ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
    }
    
    if (_readCount == -1) return -1;
    
    // 线程安全地更新读取计数
    {
        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
        g_diReadCount[deviceIndex] = _readCount;
    }
    return _readCount;
}

// 辅助函数：读取AI数据（支持单通道和多通道模式）
int queryModbusAI(uint8 deviceIndex) {
    auto itCtx = g_ctxs.find(deviceIndex);
    if (itCtx == g_ctxs.end()) return 0;
    modbus_t* ctx = itCtx->second;

    auto& _args = g_args[deviceIndex];
    auto connectionKey = g_deviceConnectionKey[deviceIndex];

    std::lock_guard<std::mutex> lock(g_connectionMutexes[connectionKey]);

    if (modbus_set_slave(ctx, _args.SlaveAddr) == -1) {
        return -1;
    }

    // 统一的多通道模式
    if (_args.channelConfigs.empty()) return 0;
    
    int totalRead = 0;
    for (const auto& pair : _args.channelConfigs) {
        int channelIndex = pair.first;
        const ChannelConfig& config = pair.second;
        
        int regCount = getRegisterCount(config.dataType);
        uint16_t* channelBuffer = g_readAIData[deviceIndex] + channelIndex * 2;  // 每个通道最多2个寄存器
        int _readCount = -1;
        
        if (config.funcCode == 0x03) {
            _readCount = modbus_read_registers(ctx, config.address, regCount, channelBuffer);
        } else if (config.funcCode == 0x04) {
            _readCount = modbus_read_input_registers(ctx, config.address, regCount, channelBuffer);
        }
        
        if (_readCount != -1) {
            totalRead += _readCount;
        }
        
        // 通道之间添加命令间隔，避免设备响应不过来
        std::this_thread::sleep_for(std::chrono::milliseconds(_args.CommandIntervalMs));
    }
    
    // 线程安全地更新读取计数
    {
        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
        g_aiReadCount[deviceIndex] = totalRead;
    }
    return totalRead;
}

void queryModbusRegistersThread(uint8 deviceIndex) {
    auto connectionKey = g_deviceConnectionKey[deviceIndex];

    // 读线程 - 纯主动轮询模式，支持断线重连
    std::thread _readThread([deviceIndex, connectionKey]() {
        while (!g_stopFlags[deviceIndex].load()) {
            auto& _args = g_args[deviceIndex]; // 从全局获取，避免悬空引用
            
            // 如果处于断线状态，尝试重连
            if (g_connectionStates[deviceIndex] == ConnectionState::Disconnected) {
                if (attemptReconnect(deviceIndex)) {
                    // 重连成功，继续正常读取
                } else {
                    // 重连失败，等待后再试
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    continue;
                }
            }
            
            bool hasAnySuccess = false;
            bool hasAnyAttempt = false;
            
            // 读取DI数据
            if (_args.DIReadCount > 0) {
                hasAnyAttempt = true;
                int ret = queryModbusDI(deviceIndex);
                if (ret != -1) {
                    hasAnySuccess = true;
                }
                
                // 如果还有AI要读，添加命令间隔
                if (!_args.channelConfigs.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_args.CommandIntervalMs));
                }
            }

            // 读取AI数据
            if (!_args.channelConfigs.empty()) {
                hasAnyAttempt = true;
                int ret = queryModbusAI(deviceIndex);
                if (ret != -1) {
                    hasAnySuccess = true;
                }
            }
            
            // 断线检测逻辑（改进：只有尝试了但全部失败才计数）
            if (hasAnyAttempt && !hasAnySuccess) {
                // 全部失败，增加失败计数
                g_consecutiveFailures[deviceIndex]++;
                if (g_consecutiveFailures[deviceIndex] >= _args.MaxConsecutiveFailures) {
                    // 判定为断线
                    g_connectionStates[deviceIndex] = ConnectionState::Disconnected;
                    // 清空读取计数，避免应用层拿到旧数据
                    {
                        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
                        g_diReadCount[deviceIndex] = -1;
                        g_aiReadCount[deviceIndex] = -1;
                    }
                    // 下一轮循环会尝试重连，这里先快速等待
                    std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_FAST_RETRY_MS));
                    continue;
                }
            } else if (hasAnySuccess) {
                // 只要有任意一次成功，就重置失败计数
                g_consecutiveFailures[deviceIndex] = 0;
                if (g_connectionStates[deviceIndex] != ConnectionState::Connected) {
                    g_connectionStates[deviceIndex] = ConnectionState::Connected;
                }
            }
            
            // 一轮读取完成后，等待轮询间隔
            std::this_thread::sleep_for(std::chrono::milliseconds(_args.PollIntervalMs));
        }
    });

    // 写线程 - 支持断线处理
    std::thread _writeThread([deviceIndex, connectionKey]() {
        while (!g_stopFlags[deviceIndex].load())
        {
            auto& _args = g_args[deviceIndex]; // 从全局获取，避免悬空引用
            
            // 如果设备断线，清空写队列，避免积压
            if (g_connectionStates[deviceIndex] == ConnectionState::Disconnected) {
                {
                    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                    while (!g_writeQueues[deviceIndex].empty()) {
                        g_writeQueues[deviceIndex].pop();  // 丢弃写任务
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            
            std::unique_lock<std::mutex> lock(g_queueMutexes[deviceIndex]);
            g_writeQueueConditions[deviceIndex].wait_for(lock, std::chrono::milliseconds(100), [deviceIndex] {
                return !g_writeQueues[deviceIndex].empty() || g_stopFlags[deviceIndex].load();
                });

            if (g_stopFlags[deviceIndex].load()) break;

            auto& _writeQueue = g_writeQueues[deviceIndex];
            if (_writeQueue.empty()) continue;

            auto ctx = g_ctxs[deviceIndex];

            // 取出所有写任务
            std::vector<WriteTask> tasks;
            while (!_writeQueue.empty()) {
                tasks.push_back(_writeQueue.front());
                _writeQueue.pop();
            }
            lock.unlock();

            std::lock_guard<std::mutex> lockConn(g_connectionMutexes[connectionKey]);
            if (modbus_set_slave(ctx, _args.SlaveAddr) == -1) {
                continue;
            }

            for (const auto& task : tasks) {
                // 如果设备断线，停止写入
                if (g_connectionStates[deviceIndex] == ConnectionState::Disconnected) {
                    break;
                }
                
                int ret = -1;
                if (task.writeType == 5) {
                    int _bitValue = task.writeData == 0 ? 0 : 1;
                    // 修复：task.writeAddr 已经包含偏移，不需要再次添加
                    // 降低重试次数，减少断线时的阻塞时间
                    ret = execModbusWithRetry([&]() {
                        return modbus_write_bit(ctx, task.writeAddr, _bitValue);
                        }, MAX_WRITE_RETRIES, _args.RetryWaitMs);
                }
                else if (task.writeType == 6) {
                    ret = execModbusWithRetry([&]() {
                        return modbus_write_register(ctx, task.writeAddr, task.writeData);
                        }, MAX_WRITE_RETRIES, _args.RetryWaitMs);
                }
                
                // 如果写入失败，增加失败计数
                if (ret == -1) {
                    g_consecutiveFailures[deviceIndex]++;
                    if (g_consecutiveFailures[deviceIndex] >= _args.MaxConsecutiveFailures) {
                        // 判定为断线
                        g_connectionStates[deviceIndex] = ConnectionState::Disconnected;
                        break;  // 停止处理剩余写任务
                    }
                } else {
                    // 写入成功，重置失败计数
                    g_consecutiveFailures[deviceIndex] = 0;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(_args.RetryWaitMs));
            }
        }
    });
    
    _readThread.join();
    _writeThread.join();
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\MODBUS\\config.ini";
    if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
    if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
    g_iniFile->read(*g_iniStructure);
    auto& ini = *g_iniStructure;

    const auto& deviceSection = BuildDeviceAttribute("device", deviceIndex);
    auto& defaultSection = ini["default"];

    std::map<std::string, std::string> defaultConfig;
    for (auto& kv : defaultSection) defaultConfig[kv.first] = kv.second;
    auto& deviceSectionMap = ini[deviceSection];
    for (auto& kv : deviceSectionMap) defaultConfig[kv.first] = kv.second;

    std::string _modbusDriver = "modbus_rtu";
    std::string _port = "1";
    std::string _ip = "127.0.0.1";
    int _slaveAddr = 1;
    int _baudRate = 9600;
    int _dataBit = 8;
    int _stopBit = 1;
    std::string _parity = "N";
    int _tcpPort = 502;

    if (defaultConfig.count("driver")) _modbusDriver = defaultConfig["driver"];
    if (defaultConfig.count("port")) _port = defaultConfig["port"];
    if (defaultConfig.count("ip")) _ip = defaultConfig["ip"];
    if (defaultConfig.count("slave_addr")) _slaveAddr = std::stoi(defaultConfig["slave_addr"], nullptr, 0);
    if (defaultConfig.count("baud_rate")) _baudRate = std::stoi(defaultConfig["baud_rate"]);
    if (defaultConfig.count("data_bit")) _dataBit = std::stoi(defaultConfig["data_bit"]);
    if (defaultConfig.count("stop_bit")) _stopBit = std::stoi(defaultConfig["stop_bit"]);
    if (defaultConfig.count("parity")) _parity = defaultConfig["parity"];
    if (defaultConfig.count("tcp_port")) _tcpPort = std::stoi(defaultConfig["tcp_port"]);

    std::string connectionKey;
    if (_modbusDriver == "modbus_rtu") connectionKey = "rtu_" + _port;
    else if (_modbusDriver == "modbus_tcp") connectionKey = "tcp_" + _ip + ":" + std::to_string(_tcpPort);
    else return 0;

    modbus_t* ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(connectionMutex);
        if (g_connectionMap.count(connectionKey)) {
            ctx = g_connectionMap[connectionKey];
            g_connectionRefCount[connectionKey]++;
        }
        else {
            if (_modbusDriver == "modbus_rtu") {
                std::string serialName = "\\\\.\\COM" + _port;
                ctx = modbus_new_rtu(serialName.c_str(), _baudRate, _parity[0], _dataBit, _stopBit);
            }
            else if (_modbusDriver == "modbus_tcp") {
                ctx = modbus_new_tcp(_ip.c_str(), _tcpPort);
            }
            if (ctx == nullptr) return 0;
            if (modbus_connect(ctx) == -1) {
                modbus_free(ctx);
                return 0;
            }

            g_connectionMap[connectionKey] = ctx;
            g_connectionRefCount[connectionKey] = 1;

            // 新增共享连接互斥锁
            g_connectionMutexes[connectionKey];
        }
    }

    g_deviceConnectionKey[deviceIndex] = connectionKey;
    g_ctxs[deviceIndex] = ctx;

    if (!g_args.count(deviceIndex)) {
        g_args[deviceIndex] = ModbusArgs();
        
        // 每个通道最多需要2个寄存器，确保缓冲区足够大
        // 使用 devInfo.InputCount 作为最大通道数限制
        int bufferSize = devInfo.InputCount * 2;
        uint16_t* aiData = new uint16_t[bufferSize];
        std::fill(aiData, aiData + bufferSize, 0);
        g_readAIData[deviceIndex] = aiData;

        uint8_t* diData = new uint8_t[devInfo.InputCount];
        g_readDIData[deviceIndex] = diData;

        g_diReadCount[deviceIndex] = 0;
        g_aiReadCount[deviceIndex] = 0;

        lastDOStatusMap[deviceIndex] = std::vector<short>(devInfo.OutputCount, 0);
        
        // 初始化数据互斥锁
        g_dataMutexes[deviceIndex];
    }

    auto& _args = g_args[deviceIndex];
    _args.SlaveAddr = _slaveAddr;
    _args.DIFuncCode = defaultConfig.count("di_func") ? std::stoi(defaultConfig["di_func"], nullptr, 0) : 0x02;
    _args.DIReadAddr = defaultConfig.count("di_addr") ? std::stoi(defaultConfig["di_addr"], nullptr, 0) : 0x0000;
    _args.DIReadCount = defaultConfig.count("di_num") ? std::stoi(defaultConfig["di_num"], nullptr, 0) : 0x0000;
    
    int defaultAIFuncCode = defaultConfig.count("ai_func") ? std::stoi(defaultConfig["ai_func"], nullptr, 0) : 0x04;
    _args.AIFuncCode = defaultAIFuncCode;
    
    // 读取AI通道配置（统一格式）
    _args.channelConfigs.clear();
    
    // 优先检查 ai_channel（默认通道0配置）
    if (defaultConfig.count("ai_channel")) {
        ChannelConfig config = parseChannelConfig(defaultConfig["ai_channel"], defaultAIFuncCode);
        _args.channelConfigs[0] = config;
    }
    
    // 检查 ai_channel_N 通道映射配置
    for (const auto& kv : defaultConfig) {
        if (kv.first.find("ai_channel_") == 0) {
            int channelIndex = std::stoi(kv.first.substr(11));  // "ai_channel_".length() = 11
            ChannelConfig config = parseChannelConfig(kv.second, defaultAIFuncCode);
            _args.channelConfigs[channelIndex] = config;  // 会覆盖 ai_channel 的配置
        }
    }

    // 读取超时和重试等待配置
    if (defaultConfig.count("timeout_ms"))
        _args.TimeoutMs = std::stoi(defaultConfig["timeout_ms"]);
    if (defaultConfig.count("retry_wait_ms"))
        _args.RetryWaitMs = std::stoi(defaultConfig["retry_wait_ms"]);

    // 读取轮询间隔和命令间隔配置
    if (defaultConfig.count("poll_interval_ms"))
        _args.PollIntervalMs = std::stoi(defaultConfig["poll_interval_ms"]);
    if (defaultConfig.count("command_interval_ms"))
        _args.CommandIntervalMs = std::stoi(defaultConfig["command_interval_ms"]);

    // 读取DO写操作起始地址配置
    if (defaultConfig.count("do_addr"))
        _args.DoAddr = std::stoi(defaultConfig["do_addr"], nullptr, 0);
    else
        _args.DoAddr = 0;

    // 读取DO功能码配置（修复：添加第三个参数支持0x前缀）
    if (defaultConfig.count("do_func"))
        _args.DefaultDoFunc = std::stoi(defaultConfig["do_func"], nullptr, 0);
    else
        _args.DefaultDoFunc = 5;

    // 读取断线检测阈值配置
    if (defaultConfig.count("max_consecutive_failures"))
        _args.MaxConsecutiveFailures = std::stoi(defaultConfig["max_consecutive_failures"]);

    // 设置modbus超时
    modbus_set_response_timeout(ctx, _args.TimeoutMs / 1000, (_args.TimeoutMs % 1000) * 1000);

    // 初始化连接状态和断线检测
    g_connectionStates[deviceIndex] = ConnectionState::Connected;
    g_consecutiveFailures[deviceIndex] = 0;

    // 初始化线程退出标志并启动线程
    g_stopFlags[deviceIndex].store(false);
    g_threads[deviceIndex] = std::thread(queryModbusRegistersThread, deviceIndex);

    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    modbus_t* ctx = nullptr;
    std::string connectionKey;

    {
        std::lock_guard<std::mutex> lock(connectionMutex);
        if (g_deviceConnectionKey.count(deviceIndex)) {
            connectionKey = g_deviceConnectionKey[deviceIndex];
            if (g_connectionMap.count(connectionKey)) {
                ctx = g_connectionMap[connectionKey];
            }
        }
    }

    if (ctx == nullptr) return 0;

    // 告诉线程停止运行
    g_stopFlags[deviceIndex].store(true);

    // 通知写线程以防阻塞
    g_writeQueueConditions[deviceIndex].notify_all();

    // 等待线程安全退出
    if (g_threads.count(deviceIndex) && g_threads[deviceIndex].joinable()) {
        g_threads[deviceIndex].join();
        g_threads.erase(deviceIndex);
    }
    g_stopFlags.erase(deviceIndex);

    {
        std::lock_guard<std::mutex> lock(connectionMutex);
        if (g_connectionRefCount.count(connectionKey)) {
            g_connectionRefCount[connectionKey]--;
            if (g_connectionRefCount[connectionKey] <= 0) {
                modbus_close(ctx);
                modbus_free(ctx);
                g_connectionMap.erase(connectionKey);
                g_connectionRefCount.erase(connectionKey);
                g_connectionMutexes.erase(connectionKey);
            }
        }
        g_deviceConnectionKey.erase(deviceIndex);
    }

    delete[] g_readAIData[deviceIndex];
    delete[] g_readDIData[deviceIndex];

    g_readAIData.erase(deviceIndex);
    g_readDIData.erase(deviceIndex);
    g_diReadCount.erase(deviceIndex);
    g_aiReadCount.erase(deviceIndex);
    g_dataMutexes.erase(deviceIndex);
    lastDOStatusMap.erase(deviceIndex);

    // 清理连接状态和断线检测相关数据
    g_connectionStates.erase(deviceIndex);
    g_consecutiveFailures.erase(deviceIndex);
    g_lastReconnectAttempt.erase(deviceIndex);

    g_ctxs.erase(deviceIndex);
    g_args.erase(deviceIndex);

    return 1;
}


const BYTE funcChannel = 240;
const BYTE writeFuncChannel = 250;

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    auto ctxIt = g_ctxs.find(deviceIndex);
    if (ctxIt == g_ctxs.end()) return 0;
    auto& _ctx = ctxIt->second;
    auto connectionKey = g_deviceConnectionKey[deviceIndex];

    static std::vector<short> changedChannel;
    changedChannel.clear();

    auto& lastDOStatus = lastDOStatusMap[deviceIndex];

    for (BYTE i = 0; i < devInfo.InputCount; i++) {
        if (i >= funcChannel && i <= 255) {
            continue;
        }
        if (InDOStatus[i] != lastDOStatus[i]) {
            changedChannel.push_back(i);
        }
    }
    std::copy(InDOStatus, InDOStatus + funcChannel, lastDOStatus.begin());

    bool hasDirtyValue = false;
    if (changedChannel.size() > 0) {
        auto& _args = g_args[deviceIndex];
        auto funcCode = InDOStatus[writeFuncChannel];
        funcCode = funcCode == 0 ? _args.DefaultDoFunc : funcCode;
        
        // 验证功能码合法性，只允许 5 (写线圈) 或 6 (写寄存器)
        if (funcCode != 5 && funcCode != 6) {
            funcCode = _args.DefaultDoFunc;  // 使用默认值
        }
        
        for (short i : changedChannel) {
            WriteTask task;
            task.writeType = funcCode;
            task.writeAddr = _args.DoAddr + i;
            task.writeData = InDOStatus[i];
            {
                std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                g_writeQueues[deviceIndex].push(task);
            }
        }
        hasDirtyValue = true;
    }

    auto _funcCode = InDOStatus[funcChannel];
    if (_funcCode == 1 || _funcCode == 2) {
        WriteTask task;
        task.writeType = _funcCode == 1 ? 5 : 6;
        task.writeAddr = InDOStatus[funcChannel + 1];
        task.writeData = InDOStatus[funcChannel + 2];
        {
            std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
            g_writeQueues[deviceIndex].push(task);
        }
        hasDirtyValue = true;
    }
    // 如果 _funcCode 是其他值(0, 3+)，忽略此操作（防止无效写入）

    if (hasDirtyValue)
        g_writeQueueConditions[deviceIndex].notify_one();

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    std::fill_n(OutDOStatus + funcChannel, 9, 0);
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    if (!g_ctxs.count(deviceIndex)) return 0;

    int _recvCount;
    {
        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
        _recvCount = g_diReadCount[deviceIndex];
        if (_recvCount <= 0) return 0;
    }

    auto& _readData = g_readDIData[deviceIndex];

    for (uint8 _idx = 0; _idx < _recvCount; ++_idx) {
        auto _byteValue = _readData[_idx];
        OutDIStatus[_idx] = _byteValue;
        if (_idx >= devInfo.InputCount) break;
    }

    {
        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
        g_diReadCount[deviceIndex] = -1;
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    auto _args = g_args[deviceIndex];
    
    int _recvCount;
    {
        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
        _recvCount = g_aiReadCount[deviceIndex];
        if (_recvCount <= 0) return 0;
    }

    auto& tab_reg = g_readAIData[deviceIndex];

    // 统一的多通道模式
    if (_args.channelConfigs.empty()) return 0;
    
    // 遍历每个配置的通道
    for (const auto& pair : _args.channelConfigs) {
        int channelIndex = pair.first;
        const ChannelConfig& config = pair.second;
        
        // 边界检查：防止缓冲区溢出
        if (channelIndex >= devInfo.InputCount) {
            continue;
        }
        
        // 获取该通道的数据缓冲区
        uint16_t* channelBuffer = tab_reg + channelIndex * 2;
        
        // 根据数据类型解析并缩放
        float scaledValue;
        
        if (config.dataType == "fixed16_16") {
            // Q16.16 固定点数格式（特殊处理，不使用 parseRegisterValue）
            float rawValue = parseFixed16_16(channelBuffer, config.byteOrder);
            scaledValue = rawValue * config.scale;
        }
        else {
            // 其他数据类型使用通用解析
            uint32_t rawBits = parseRegisterValue(channelBuffer, config.dataType, config.byteOrder);
            
            if (config.dataType == "uint16") {
                scaledValue = static_cast<float>(static_cast<uint16_t>(rawBits)) * config.scale;
            }
            else if (config.dataType == "int16") {
                scaledValue = static_cast<float>(static_cast<int16_t>(rawBits)) * config.scale;
            }
            else if (config.dataType == "uint32") {
                // uint32: 0 ~ 4,294,967,295，需要特别处理大数值
                scaledValue = static_cast<float>(rawBits) * config.scale;
            }
            else if (config.dataType == "int32") {
                scaledValue = static_cast<float>(static_cast<int32_t>(rawBits)) * config.scale;
            }
            else if (config.dataType == "float32") {
                float* fptr = reinterpret_cast<float*>(&rawBits);
                scaledValue = (*fptr) * config.scale;
            }
            else {
                scaledValue = 0.0f;
            }
        }
        
        // 限制到short范围并输出
        OutADStatus[channelIndex] = clampToShort(scaledValue);
    }
    
    {
        std::lock_guard<std::mutex> dataLock(g_dataMutexes[deviceIndex]);
        g_aiReadCount[deviceIndex] = -1;
    }
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
    return 0;
}
