#include <stdlib.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "IOUI.h"
#include "snap7.h"
#include "Paths.hpp"
#include "mIni/mini/ini.h"
#include "Util.hpp"
#include "ExpressionParser/ExpressionParser.h"

#pragma comment(lib, "snap7.lib")

DeviceInfo devInfo;

// ==================== 数据结构定义 ====================

// 单个通道配置
struct ChannelConfig {
    int address;           // 寄存器起始地址（字节偏移）
    std::string dataType;  // 数据类型: uint16, int16, uint32, int32, float32
    std::string byteOrder; // 字节序: AB(大端), BA(小端)
    std::string area;      // 存储区: PE/PA/MK/DB
    int dbNumber;          // DB块号（仅DB区使用）
    
    // 表达式支持（使用 ExpressionParser）
    std::shared_ptr<CompiledExpression> compiledExpr;  // 预编译表达式
    
    ChannelConfig() 
        : address(0), dataType("uint16"), byteOrder("AB"), 
          area("DB"), dbNumber(1) {}
};

// DO output mode enumeration
enum class DOOutputMode {
    BitMode = 0,      // Bit mode - continuous area bit output (channel index -> bit)
    ByteMode = 1,     // Byte mode - index is address (channel index -> byte address)
    WordMode = 2,     // Word mode - index is address (channel index -> word address)
    DWordMode = 3     // DWord mode - index is address (2 channels -> 1 dword address)
};

// S7 configuration parameters
struct S7Args {
    std::string ip;
    int rack;
    int slot;
    int timeoutMs;
    int retryWaitMs;
    int pollIntervalMs;
    int commandIntervalMs;
    int maxConsecutiveFailures;
    
    // DI配置
    std::string diArea;
    int diDbNumber;
    int diStart;
    int diBytes;
    
    // DO配置
    std::string doArea;
    int doDbNumber;
    int doStart;
    int doBytes;           // 位模式：连续字节数；其他模式：忽略
    DOOutputMode doMode;   // 输出模式
    
    // AI通道映射
    std::map<int, ChannelConfig> channelConfigs;
    
    S7Args() : rack(0), slot(1), timeoutMs(1000), retryWaitMs(20),
               pollIntervalMs(100), commandIntervalMs(10), maxConsecutiveFailures(3),
               diArea("PE"), diDbNumber(0), diStart(0), diBytes(8),
               doArea("DB"), doDbNumber(1), doStart(10), doBytes(8),
               doMode(DOOutputMode::BitMode) {}
};

// 写任务结构
struct WriteTask {
    std::string area;   // PE/PA/MK/DB
    int dbNumber;       // DB块号
    int address;        // 起始地址
    std::vector<BYTE> data;  // 数据
};

// Connection state enumeration
enum class ConnectionState {
    Connected,
    Disconnected,
    Reconnecting
};

// ==================== Global Variables ====================

std::map<uint8, std::shared_ptr<TS7Client>> g_clientMap;
std::map<uint8, S7Args> g_args;

// 数据缓存
std::map<uint8, std::vector<BYTE>> g_readDIData;
std::map<uint8, std::vector<uint16_t>> g_readAIData;
std::map<uint8, std::vector<short>> g_lastDOStatus;
std::map<uint8, bool> g_firstDOWrite;  // 首次写入标志

// 线程管理
std::map<uint8, std::thread> g_threads;
std::map<uint8, std::atomic<bool>> g_stopFlags;

// 写队列
std::map<uint8, std::queue<WriteTask>> g_writeQueues;
std::map<uint8, std::mutex> g_queueMutexes;
std::map<uint8, std::condition_variable> g_writeQueueConditions;

// 数据保护互斥锁
std::map<uint8, std::mutex> g_dataMutexes;

// 连接状态管理
std::map<uint8, ConnectionState> g_connectionStates;
std::map<uint8, int> g_consecutiveFailures;
std::map<uint8, std::chrono::steady_clock::time_point> g_lastReconnectAttempt;

// 配置文件
std::shared_ptr<mINI::INIFile> g_iniFile;
std::shared_ptr<mINI::INIStructure> g_iniStructure;

// 全局表达式解析器
ExpressionParser g_exprParser;

// 常量定义
const int RECONNECT_INTERVAL_MS = 5000;
const int RECONNECT_FAST_RETRY_MS = 100;
const int MAX_WRITE_RETRIES = 2;

// 特殊功能通道定义（参考 MODBUS 模块）
const BYTE funcChannel = 240;      // 特殊功能通道起始 (240-249)
const BYTE writeFuncChannel = 250; // 写入功能码通道 (250-254)

IOUI_API DeviceInfo* __stdcall Initialize() {
    devInfo.InputCount = 255;
    devInfo.OutputCount = 255;
    devInfo.AxisCount = 255;
    return &devInfo;
}

// ==================== 辅助函数 ====================

// 将区域字符串转换为S7区域常量
byte parseAreaType(const std::string& area) {
    if (area == "PE" || area == "I") return S7AreaPE;
    if (area == "PA" || area == "Q") return S7AreaPA;
    if (area == "MK" || area == "M") return S7AreaMK;
    if (area == "DB") return S7AreaDB;
    if (area == "CT" || area == "C") return S7AreaCT;
    if (area == "TM" || area == "T") return S7AreaTM;
    return S7AreaDB; // 默认
}

// 获取数据类型需要的寄存器数量（字数）
// 返回值: 0=单字节(uint8/int8), 1=单字(uint16/int16), 2=双字(uint32/int32/float32)
int getRegisterCount(const std::string& dataType) {
    if (dataType == "uint8" || dataType == "int8") return 0;  // 单字节
    if (dataType == "uint16" || dataType == "int16") return 1;
    if (dataType == "uint32" || dataType == "int32" || dataType == "float32") return 2;
    return 1;
}

// 解析寄存器值为double（用于后续缩放）
// 注意：regs[] 数组中的每个 uint16_t 已经通过 _byteswap_ushort 转换为本地字节序
double parseRegisterValue(const uint16_t* regs, const std::string& dataType, const std::string& byteOrder) {
    if (dataType == "uint8") {
        // regs[0] 存储的是单字节数据，只取低 8 位
        return static_cast<double>(regs[0] & 0xFF);
    }
    else if (dataType == "int8") {
        // regs[0] 存储的是单字节数据，转换为有符号字节
        int8_t byteValue = static_cast<int8_t>(regs[0] & 0xFF);
        return static_cast<double>(byteValue);
    }
    else if (dataType == "uint16") {
        return static_cast<double>(regs[0]);
    }
    else if (dataType == "int16") {
        return static_cast<double>(static_cast<int16_t>(regs[0]));
    }
    else if (dataType == "uint32") {
        uint32_t value;
        // SNAP7 返回正确的 Word 数值
        // AB 模式：PLC 大端序，regs[0]=高Word，regs[1]=低Word
        if (byteOrder == "BA") {
            // BA: PLC 小端序，regs[0]=低Word，regs[1]=高Word
            value = (static_cast<uint32_t>(regs[1]) << 16) | regs[0];
        } else {
            // AB (默认): PLC 大端序，regs[0]=高Word
            value = (static_cast<uint32_t>(regs[0]) << 16) | regs[1];
        }
        return static_cast<double>(value);
    }
    else if (dataType == "int32") {
        uint32_t rawValue;
        // AB 模式：PLC 大端序，regs[0]=高Word，regs[1]=低Word
        if (byteOrder == "BA") {
            // BA: PLC 小端序，regs[0]=低Word
            rawValue = (static_cast<uint32_t>(regs[1]) << 16) | regs[0];
        } else {
            // AB (默认): PLC 大端序，regs[0]=高Word
            rawValue = (static_cast<uint32_t>(regs[0]) << 16) | regs[1];
        }
        return static_cast<double>(static_cast<int32_t>(rawValue));
    }
    else if (dataType == "float32") {
        uint32_t rawValue;
        // AB 模式：PLC 大端序，regs[0]=高Word，regs[1]=低Word
        if (byteOrder == "BA") {
            // BA: PLC 小端序，regs[0]=低Word
            rawValue = (static_cast<uint32_t>(regs[1]) << 16) | regs[0];
        } else {
            // AB (默认): PLC 大端序，regs[0]=高Word
            rawValue = (static_cast<uint32_t>(regs[0]) << 16) | regs[1];
        }
        float fValue;
        memcpy(&fValue, &rawValue, sizeof(float));
        return static_cast<double>(fValue);
    }
    return 0.0;
}

// 解析通道配置字符串
// 格式: 区域地址, 数据类型[, 表达式][, 字节序]
// 区域地址格式:
//   - DB1.20 或 DB1.DBW20 → DB块1，字节偏移20
//   - IW10 或 PE.10 → 输入区，字节偏移10
//   - QW20 或 PA.20 → 输出区，字节偏移20
//   - MW30 或 MK.30 → 标志位区，字节偏移30
// 
// 示例:
//   "DB1.20, int16, x*0.1" → DB1.DBW20，有符号整型，应用表达式
//   "IW10, uint16" → IW10，无符号整型，不应用表达式
//   "DB2.100, float32, x+1000" → DB2.DBD100，浮点数，加偏移
ChannelConfig parseChannelConfig(const std::string& configStr, const S7Args& defaultArgs) {
    ChannelConfig config;
    config.area = "DB";
    config.dbNumber = 1;
    config.byteOrder = "AB";
    
    std::vector<std::string> parts;
    std::stringstream ss(configStr);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item.erase(0, item.find_first_not_of(" \t\r\n"));
        item.erase(item.find_last_not_of(" \t\r\n") + 1);
        if (!item.empty()) {
            parts.push_back(item);
        }
    }
    
    if (parts.size() < 1) return config;
    
    // 解析第一个参数：区域地址
    std::string addrStr = parts[0];
    
    // 检查是否包含区域前缀（DB1.20, PE.10, IW10等）
    size_t dotPos = addrStr.find('.');
    if (dotPos != std::string::npos) {
        // 格式: DB1.20 或 PE.10
        std::string prefix = addrStr.substr(0, dotPos);
        std::string offsetStr = addrStr.substr(dotPos + 1);
        
        // 去除可能的DBW/DBD等前缀（DB1.DBW20 → 提取20）
        size_t numStart = 0;
        for (size_t i = 0; i < offsetStr.size(); ++i) {
            if (std::isdigit(offsetStr[i]) || offsetStr[i] == 'x') {
                numStart = i;
                break;
            }
        }
        offsetStr = offsetStr.substr(numStart);
        
        // 解析区域类型
        if (prefix.find("DB") == 0) {
            config.area = "DB";
            // 提取DB号（DB1 → 1）
            std::string dbNumStr = prefix.substr(2);
            if (!dbNumStr.empty()) {
                config.dbNumber = std::stoi(dbNumStr);
            }
        } else if (prefix == "PE" || prefix == "I" || prefix == "E") {
            config.area = "PE";
            config.dbNumber = 0;
        } else if (prefix == "PA" || prefix == "Q" || prefix == "A") {
            config.area = "PA";
            config.dbNumber = 0;
        } else if (prefix == "MK" || prefix == "M") {
            config.area = "MK";
            config.dbNumber = 0;
        }
        
        config.address = std::stoi(offsetStr, nullptr, 0);
    } 
    else if (addrStr[0] == 'I' || addrStr[0] == 'E') {
        // 格式: IW10, IB10, ID10
        config.area = "PE";
        config.dbNumber = 0;
        std::string offsetStr = addrStr.substr(1);
        // 去除W/B/D前缀
        if (offsetStr[0] == 'W' || offsetStr[0] == 'B' || offsetStr[0] == 'D') {
            offsetStr = offsetStr.substr(1);
        }
        config.address = std::stoi(offsetStr, nullptr, 0);
    }
    else if (addrStr[0] == 'Q' || addrStr[0] == 'A') {
        // 格式: QW10, QB10, QD10
        config.area = "PA";
        config.dbNumber = 0;
        std::string offsetStr = addrStr.substr(1);
        if (offsetStr[0] == 'W' || offsetStr[0] == 'B' || offsetStr[0] == 'D') {
            offsetStr = offsetStr.substr(1);
        }
        config.address = std::stoi(offsetStr, nullptr, 0);
    }
    else if (addrStr[0] == 'M') {
        // 格式: MW10, MB10, MD10
        config.area = "MK";
        config.dbNumber = 0;
        std::string offsetStr = addrStr.substr(1);
        if (offsetStr[0] == 'W' || offsetStr[0] == 'B' || offsetStr[0] == 'D') {
            offsetStr = offsetStr.substr(1);
        }
        config.address = std::stoi(offsetStr, nullptr, 0);
    }
    else {
        // 不支持的格式，使用默认值
        config.address = 0;
        config.area = "DB";
        config.dbNumber = 1;
    }
    
    // 解析其他参数
    if (parts.size() >= 2) config.dataType = parts[1];
    
    // 第3个参数：表达式（使用 ExpressionParser）
    if (parts.size() >= 3) {
        std::string exprStr = parts[2];
        auto expr = g_exprParser.parse(exprStr);
        if (expr && expr->isCompiled()) {
            config.compiledExpr = expr;
        } else {
            // 表达式编译失败，使用默认表达式 "x"
            std::string errMsg = expr ? expr->getError() : "parse() returned nullptr";
            fprintf(stderr, "[SNAP7] Expression compile failed for '%s': %s\n", exprStr.c_str(), errMsg.c_str());
            
            auto defaultExpr = g_exprParser.parse("x");
            if (defaultExpr && defaultExpr->isCompiled()) {
                config.compiledExpr = defaultExpr;
            }
        }
    }
    
    // 第4个参数：字节序
    if (parts.size() >= 4) config.byteOrder = parts[3];
    
    return config;
}

// 尝试重连设备
bool attemptReconnect(uint8 deviceIndex) {
    auto now = std::chrono::steady_clock::now();
    if (g_lastReconnectAttempt.count(deviceIndex)) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_lastReconnectAttempt[deviceIndex]).count();
        if (elapsed < RECONNECT_INTERVAL_MS) {
            return false;
        }
    }
    
    g_lastReconnectAttempt[deviceIndex] = now;
    g_connectionStates[deviceIndex] = ConnectionState::Reconnecting;
    
    auto it = g_clientMap.find(deviceIndex);
    if (it == g_clientMap.end()) return false;
    auto& client = it->second;
    auto& args = g_args[deviceIndex];
    
    client->Disconnect();
    
    if (client->ConnectTo(args.ip.c_str(), args.rack, args.slot) == 0) {
        g_connectionStates[deviceIndex] = ConnectionState::Connected;
        g_consecutiveFailures[deviceIndex] = 0;
        return true;
    }
    
    g_connectionStates[deviceIndex] = ConnectionState::Disconnected;
    return false;
}

// 读取DI数据
// 返回值: 1=成功(或跳过), -1=失败, 0=不应使用
int queryS7DI(uint8 deviceIndex) {
    auto it = g_clientMap.find(deviceIndex);
    if (it == g_clientMap.end()) return -1;
    auto& client = it->second;
    auto& args = g_args[deviceIndex];
    
    if (args.diBytes <= 0) return 1;  // 没有配置DI，视为跳过（非失败）
    
    byte area = parseAreaType(args.diArea);
    
    // 使用临时缓冲区读取数据，避免长时间持有锁
    std::vector<BYTE> tempBuffer(args.diBytes);
    int result = client->ReadArea(area, args.diDbNumber, args.diStart, args.diBytes, S7WLByte, tempBuffer.data());
    
    // 读取成功后更新到全局缓冲区（加锁保护）
    if (result == 0) {
        std::lock_guard<std::mutex> lock(g_dataMutexes[deviceIndex]);
        std::copy(tempBuffer.begin(), tempBuffer.end(), g_readDIData[deviceIndex].begin());
        return 1;  // 成功
    }
    
    return -1;  // 失败
}

// 读取AI数据
// 返回值: 1=成功(或跳过), -1=失败
int queryS7AI(uint8 deviceIndex) {
    auto it = g_clientMap.find(deviceIndex);
    if (it == g_clientMap.end()) return -1;
    auto& client = it->second;
    auto& args = g_args[deviceIndex];
    
    if (args.channelConfigs.empty()) return 1;  // 没有配置AI，视为跳过（非失败）
    
    bool hasAnyReadSuccess = false;
    
    // 使用临时缓冲区读取数据，避免长时间持有锁
    std::vector<std::pair<int, std::vector<uint16_t>>> tempData;
    
    for (const auto& pair : args.channelConfigs) {
        int channelIndex = pair.first;
        const ChannelConfig& config = pair.second;
        
        int regCount = getRegisterCount(config.dataType);
        byte area = parseAreaType(config.area);
        
        // 根据数据类型确定读取方式
        int result = -1;
        std::vector<uint16_t> tempBuffer;
        
        if (regCount == 0) {
            // uint8/int8: 读取单字节
            tempBuffer.resize(1);
            BYTE byteData;
            result = client->ReadArea(area, config.dbNumber, config.address, 1, S7WLByte, &byteData);
            if (result == 0) {
                tempBuffer[0] = byteData;  // 存储到 uint16_t 的低字节
            }
        } else {
            // uint16/int16/uint32/int32/float32: 读取字
            tempBuffer.resize(regCount);
            result = client->ReadArea(area, config.dbNumber, config.address, regCount, S7WLWord, tempBuffer.data());
        }
        
        if (result == 0) {
            tempData.push_back({channelIndex, std::move(tempBuffer)});
            hasAnyReadSuccess = true;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(args.commandIntervalMs));
    }
    
    // 一次性更新到全局缓冲区（加锁保护）
    if (!tempData.empty()) {
        std::lock_guard<std::mutex> lock(g_dataMutexes[deviceIndex]);
        for (const auto& item : tempData) {
            int channelIndex = item.first;
            const auto& data = item.second;
            uint16_t* channelBuffer = g_readAIData[deviceIndex].data() + channelIndex * 2;
            
            const ChannelConfig& config = args.channelConfigs.at(channelIndex);
            int regCount = getRegisterCount(config.dataType);
            
            if (regCount == 0) {
                // uint8/int8: 字节数据不需要交换，直接存储
                channelBuffer[0] = data[0];
            } else {
                // uint16/int16/uint32/int32/float32: SNAP7 S7WLWord 返回按本地字节序解释的 Word
                // 需要交换字节以获得正确的数值
                // 例如：PLC 字节 [0x00, 0x64] → Windows 解释为 0x6400 → 交换为 0x0064 = 100
                for (size_t i = 0; i < data.size(); ++i) {
                    channelBuffer[i] = _byteswap_ushort(data[i]);
                }
            }
        }
    }
    
    return hasAnyReadSuccess ? 1 : -1;  // 至少一个通道成功则返回1，全失败返回-1
}

// 后台轮询线程
void pollingThread(uint8 deviceIndex) {
    // 读线程
    std::thread readThread([deviceIndex]() {
        while (!g_stopFlags[deviceIndex].load()) {
            auto& args = g_args[deviceIndex];
            
            if (g_connectionStates[deviceIndex] == ConnectionState::Disconnected) {
                if (attemptReconnect(deviceIndex)) {
                    // 重连成功
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    continue;
                }
            }
            
            bool hasAnySuccess = false;
            bool hasAnyAttempt = false;
            
            // 读取DI
            int diRet = 0;  // 0=未尝试, 1=成功/跳过, -1=失败
            if (args.diBytes > 0 || args.channelConfigs.empty()) {
                // 如果配置了DI，或者只有DI没有AI，则尝试读取
                diRet = queryS7DI(deviceIndex);
                if (diRet == 1) hasAnySuccess = true;
                else if (diRet == -1) hasAnyAttempt = true;
                
                if (!args.channelConfigs.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(args.commandIntervalMs));
                }
            }
            
            // 读取AI
            int aiRet = 0;  // 0=未尝试, 1=成功/跳过, -1=失败
            if (!args.channelConfigs.empty() || args.diBytes == 0) {
                // 如果配置了AI，或者只有AI没有DI，则尝试读取
                aiRet = queryS7AI(deviceIndex);
                if (aiRet == 1) hasAnySuccess = true;
                else if (aiRet == -1) hasAnyAttempt = true;
            }
            
            // 断线检测：只有当至少一个读取操作返回失败(-1)时才认为是失败
            if (args.maxConsecutiveFailures > 0) {
                bool hasFailed = (diRet == -1 || aiRet == -1);
                
                if (hasFailed) {
                    g_consecutiveFailures[deviceIndex]++;
                    if (g_consecutiveFailures[deviceIndex] >= args.maxConsecutiveFailures) {
                        g_connectionStates[deviceIndex] = ConnectionState::Disconnected;
                        std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_FAST_RETRY_MS));
                        continue;
                    }
                } else if (hasAnySuccess) {
                    g_consecutiveFailures[deviceIndex] = 0;
                    if (g_connectionStates[deviceIndex] != ConnectionState::Connected) {
                        g_connectionStates[deviceIndex] = ConnectionState::Connected;
                    }
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(args.pollIntervalMs));
        }
    });
    
    // 写线程
    std::thread writeThread([deviceIndex]() {
        while (!g_stopFlags[deviceIndex].load()) {
            auto& args = g_args[deviceIndex];
            
            if (g_connectionStates[deviceIndex] == ConnectionState::Disconnected) {
                {
                    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                    while (!g_writeQueues[deviceIndex].empty()) {
                        g_writeQueues[deviceIndex].pop();
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            
            std::unique_lock<std::mutex> lock(g_queueMutexes[deviceIndex]);
            g_writeQueueConditions[deviceIndex].wait(lock, [deviceIndex] {
                return !g_writeQueues[deviceIndex].empty() || g_stopFlags[deviceIndex].load();
            });
            
            if (g_stopFlags[deviceIndex].load()) break;
            
            auto& writeQueue = g_writeQueues[deviceIndex];
            if (writeQueue.empty()) continue;
            
            std::vector<WriteTask> tasks;
            while (!writeQueue.empty()) {
                tasks.push_back(writeQueue.front());
                writeQueue.pop();
            }
            lock.unlock();
            
            auto it = g_clientMap.find(deviceIndex);
            if (it == g_clientMap.end()) continue;
            auto& client = it->second;
            
            for (const auto& task : tasks) {
                if (g_connectionStates[deviceIndex] == ConnectionState::Disconnected) break;
                
                byte area = parseAreaType(task.area);
                int result = client->WriteArea(area, task.dbNumber, task.address, 
                                               task.data.size(), S7WLByte, 
                                               const_cast<BYTE*>(task.data.data()));
                
                if (args.maxConsecutiveFailures > 0) {
                    if (result != 0) {
                        g_consecutiveFailures[deviceIndex]++;
                        if (g_consecutiveFailures[deviceIndex] >= args.maxConsecutiveFailures) {
                            g_connectionStates[deviceIndex] = ConnectionState::Disconnected;
                        }
                    } else {
                        g_consecutiveFailures[deviceIndex] = 0;
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(args.retryWaitMs));
            }
        }
    });
    
    readThread.join();
    writeThread.join();
}

// ==================== API函数实现 ====================

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex) {
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\snap7.dll";
    auto _module = LoadLibraryA(_path.data());
    if (!_module) return 0;

    // 加载配置文件
    if (g_iniFile == nullptr || g_iniStructure == nullptr) {
        std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
        std::string config_file_path = path + "Config\\SNAP7\\config.ini";
        g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
        g_iniStructure = std::make_shared<mINI::INIStructure>();
        g_iniFile->read(*g_iniStructure);
        
        // 加载表达式库（只加载一次）
        auto& ini = *g_iniStructure;
        if (ini.has("expressions")) {
            std::map<std::string, std::string> exprs;
            for (const auto& kv : ini["expressions"]) {
                exprs[kv.first] = kv.second;
            }
            g_exprParser.loadExpressions(exprs);
        }
    }
    
    auto& ini = *g_iniStructure;
    const std::string deviceSectionName = BuildDeviceAttribute("device", deviceIndex);
    
    // 合并default和device配置
    auto& defaultSection = ini["default"];
    std::map<std::string, std::string> mergedConfig;
    for (auto& kv : defaultSection) mergedConfig[kv.first] = kv.second;
    auto& deviceSection = ini[deviceSectionName];
    for (auto& kv : deviceSection) mergedConfig[kv.first] = kv.second;
    
    // 初始化参数（连接失败时需要清理）
    bool resourcesInitialized = false;
    if (!g_args.count(deviceIndex)) {
        g_args[deviceIndex] = S7Args();
        g_readDIData[deviceIndex].resize(devInfo.InputCount, 0);
        g_readAIData[deviceIndex].resize(devInfo.InputCount * 2, 0);
        g_lastDOStatus[deviceIndex].resize(devInfo.OutputCount, 0);
        g_firstDOWrite[deviceIndex] = true;  // 标记需要首次写入
        // 初始化所有互斥锁、队列和条件变量
        g_dataMutexes[deviceIndex];
        g_queueMutexes[deviceIndex];
        g_writeQueueConditions[deviceIndex];
        g_writeQueues[deviceIndex];  // 初始化写队列
        resourcesInitialized = true;
    }
    
    auto& args = g_args[deviceIndex];
    
    // 解析连接参数
    args.ip = mergedConfig.count("ip") ? mergedConfig["ip"] : "192.168.2.1";
    args.rack = mergedConfig.count("rack") ? std::stoi(mergedConfig["rack"]) : 0;
    args.slot = mergedConfig.count("slot") ? std::stoi(mergedConfig["slot"]) : 1;
    args.timeoutMs = mergedConfig.count("timeout_ms") ? std::stoi(mergedConfig["timeout_ms"]) : 1000;
    args.retryWaitMs = mergedConfig.count("retry_wait_ms") ? std::stoi(mergedConfig["retry_wait_ms"]) : 20;
    args.pollIntervalMs = mergedConfig.count("poll_interval_ms") ? std::stoi(mergedConfig["poll_interval_ms"]) : 100;
    args.commandIntervalMs = mergedConfig.count("command_interval_ms") ? std::stoi(mergedConfig["command_interval_ms"]) : 10;
    args.maxConsecutiveFailures = mergedConfig.count("max_consecutive_failures") ? std::stoi(mergedConfig["max_consecutive_failures"]) : 3;
    
    // 解析DI配置
    args.diArea = mergedConfig.count("di_area") ? mergedConfig["di_area"] : "PE";
    args.diDbNumber = mergedConfig.count("di_db_number") ? std::stoi(mergedConfig["di_db_number"], nullptr, 0) : 0;
    args.diStart = mergedConfig.count("di_start") ? std::stoi(mergedConfig["di_start"], nullptr, 0) : 0;
    args.diBytes = mergedConfig.count("di_bytes") ? std::stoi(mergedConfig["di_bytes"]) : 8;
    
    // 解析DO配置
    args.doArea = mergedConfig.count("do_area") ? mergedConfig["do_area"] : "DB";
    args.doDbNumber = mergedConfig.count("do_db_number") ? std::stoi(mergedConfig["do_db_number"], nullptr, 0) : 1;
    args.doStart = mergedConfig.count("do_start") ? std::stoi(mergedConfig["do_start"], nullptr, 0) : 10;
    args.doBytes = mergedConfig.count("do_bytes") ? std::stoi(mergedConfig["do_bytes"]) : 8;
    
    // Parse DO output mode
    if (mergedConfig.count("do_mode")) {
        std::string modeStr = mergedConfig["do_mode"];
        // Convert to lowercase for comparison
        std::transform(modeStr.begin(), modeStr.end(), modeStr.begin(), ::tolower);
        
        if (modeStr == "byte") {
            args.doMode = DOOutputMode::ByteMode;
        } else if (modeStr == "word") {
            args.doMode = DOOutputMode::WordMode;
        } else if (modeStr == "dword") {
            args.doMode = DOOutputMode::DWordMode;
        } else if (modeStr == "bit" || modeStr == "0") {
            args.doMode = DOOutputMode::BitMode;
        } else {
            // Backward compatibility for numeric config
            int modeValue = std::stoi(modeStr);
            args.doMode = static_cast<DOOutputMode>(modeValue);
        }
    } else {
        args.doMode = DOOutputMode::BitMode;  // Default: bit mode
    }
    
    // 解析AI通道配置
    args.channelConfigs.clear();
    if (mergedConfig.count("ai_channel")) {
        ChannelConfig config = parseChannelConfig(mergedConfig["ai_channel"], args);
        args.channelConfigs[0] = config;
    }
    for (const auto& kv : mergedConfig) {
        if (kv.first.find("ai_channel_") == 0) {
            int channelIndex = std::stoi(kv.first.substr(11));
            ChannelConfig config = parseChannelConfig(kv.second, args);
            args.channelConfigs[channelIndex] = config;
        }
    }
    
    // 创建并连接TS7Client
    auto client = std::make_shared<TS7Client>();
    int connectResult = client->ConnectTo(args.ip.c_str(), args.rack, args.slot);
    if (connectResult != 0) {
        // 获取详细错误信息
        /*std::string errorText = CliErrorText(connectResult);
        OutputDebugStringA(errorText.c_str());*/
        
        // 连接失败，清理已初始化的资源
        if (resourcesInitialized) {
            // 清理所有容器（不需要手动清理channelConfigs，map的析构函数会自动处理）
            g_args.erase(deviceIndex);
            g_readDIData.erase(deviceIndex);
            g_readAIData.erase(deviceIndex);
            g_lastDOStatus.erase(deviceIndex);
            g_firstDOWrite.erase(deviceIndex);
            g_dataMutexes.erase(deviceIndex);
        }
        return 0;
    }
    
    g_clientMap[deviceIndex] = client;
    
    // 初始化连接状态
    g_connectionStates[deviceIndex] = ConnectionState::Connected;
    g_consecutiveFailures[deviceIndex] = 0;
    
    // 启动后台线程
    g_stopFlags[deviceIndex].store(false);
    g_threads[deviceIndex] = std::thread(pollingThread, deviceIndex);
    
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex) {
    // 1. 设置停止标志
    if (g_stopFlags.count(deviceIndex)) {
        g_stopFlags[deviceIndex].store(true);
    }
    
    // 2. 清空写队列（避免线程继续处理任务）
    if (g_writeQueues.count(deviceIndex) && g_queueMutexes.count(deviceIndex)) {
        std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
        while (!g_writeQueues[deviceIndex].empty()) {
            g_writeQueues[deviceIndex].pop();
        }
    }
    
    // 3. 通知写线程退出（必须在清空队列后）
    if (g_writeQueueConditions.count(deviceIndex)) {
        g_writeQueueConditions[deviceIndex].notify_all();
    }
    
    // 4. 等待线程安全退出（必须在删除资源前完成）
    if (g_threads.count(deviceIndex) && g_threads[deviceIndex].joinable()) {
        g_threads[deviceIndex].join();
        g_threads.erase(deviceIndex);
    }
    g_stopFlags.erase(deviceIndex);
    
    // 5. 断开PLC连接
    auto it = g_clientMap.find(deviceIndex);
    if (it != g_clientMap.end()) {
        it->second->Disconnect();
        g_clientMap.erase(it);
    }
    
    // 6. 清理所有资源（此时线程已安全退出，无竞态条件）
    g_args.erase(deviceIndex);
    g_readDIData.erase(deviceIndex);
    g_readAIData.erase(deviceIndex);
    g_lastDOStatus.erase(deviceIndex);
    g_firstDOWrite.erase(deviceIndex);
    g_dataMutexes.erase(deviceIndex);
    g_writeQueues.erase(deviceIndex);
    g_queueMutexes.erase(deviceIndex);
    g_writeQueueConditions.erase(deviceIndex);
    g_connectionStates.erase(deviceIndex);
    g_consecutiveFailures.erase(deviceIndex);
    g_lastReconnectAttempt.erase(deviceIndex);
    
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus) {
    if (!g_args.count(deviceIndex)) return 0;
    auto& args = g_args[deviceIndex];
    auto& lastStatus = g_lastDOStatus[deviceIndex];
    bool isFirstWrite = g_firstDOWrite[deviceIndex];
    
    bool hasDirtyValue = false;
    
    // ========== 1. 批量DO输出（通道 0-239）==========
    // 检测变化的通道（排除特殊功能通道 240-255）
    std::vector<int> changedChannels;
    for (int i = 0; i < funcChannel && i < devInfo.OutputCount; i++) {
        // BitMode: 首次写入强制写入所有通道（整块写入，必须初始化）
        // 其他模式: 只写入变化的通道（惰性写入，避免启动时大量无效操作）
        bool shouldWrite = (InDOStatus[i] != lastStatus[i]);
        if (args.doMode == DOOutputMode::BitMode && isFirstWrite) {
            shouldWrite = true;  // 位模式首次强制写入
        }
        
        if (shouldWrite) {
            changedChannels.push_back(i);
            lastStatus[i] = InDOStatus[i];
        }
    }
    
    // 清除首次写入标志
    if (isFirstWrite) {
        g_firstDOWrite[deviceIndex] = false;
    }
    
    if (!changedChannels.empty()) {
        // 根据输出模式处理
        switch (args.doMode) {
            case DOOutputMode::BitMode: {
                // 位模式：连续区域按位输出
                std::vector<BYTE> tempBuffer(args.doBytes, 0);
                
                for (int i = 0; i < args.doBytes * 8 && i < funcChannel; i++) {
                    int byteIdx = i / 8;
                    int bitIdx = i % 8;
                    if (InDOStatus[i] > 0) {
                        tempBuffer[byteIdx] |= (1 << bitIdx);
                    }
                }
                
                WriteTask task;
                task.area = args.doArea;
                task.dbNumber = (args.doArea == "DB") ? args.doDbNumber : 0;
                task.address = args.doStart;
                task.data = tempBuffer;
                
                {
                    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                    g_writeQueues[deviceIndex].push(task);
                }
                hasDirtyValue = true;
                break;
            }
            
            case DOOutputMode::ByteMode: {
                // 字节模式：每个通道索引即字节地址
                for (int ch : changedChannels) {
                    BYTE value = static_cast<BYTE>(InDOStatus[ch] & 0xFF);
                    
                    WriteTask task;
                    task.area = args.doArea;
                    task.dbNumber = (args.doArea == "DB") ? args.doDbNumber : 0;
                    task.address = args.doStart + ch;
                    task.data = {value};
                    
                    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                    g_writeQueues[deviceIndex].push(task);
                }
                hasDirtyValue = true;
                break;
            }
            
            case DOOutputMode::WordMode: {
                // 字模式：每个通道索引即字地址（2字节）
                for (int ch : changedChannels) {
                    uint16_t value = static_cast<uint16_t>(InDOStatus[ch] & 0xFFFF);
                    
                    WriteTask task;
                    task.area = args.doArea;
                    task.dbNumber = (args.doArea == "DB") ? args.doDbNumber : 0;
                    task.address = args.doStart + ch;
                    task.data = {
                        static_cast<BYTE>((value >> 8) & 0xFF),  // 高字节
                        static_cast<BYTE>(value & 0xFF)          // 低字节
                    };
                    
                    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                    g_writeQueues[deviceIndex].push(task);
                }
                hasDirtyValue = true;
                break;
            }
            
            case DOOutputMode::DWordMode: {
                // 双字模式：每2个通道组成1个双字（4字节）
                // 收集需要写入的双字地址
                std::set<int> dwordAddresses;
                for (int ch : changedChannels) {
                    dwordAddresses.insert(ch / 2);
                }
                
                for (int dwordAddr : dwordAddresses) {
                    int ch0 = dwordAddr * 2;
                    int ch1 = dwordAddr * 2 + 1;
                    
                    // 组合2个short为1个DWord（高16位 << 16 | 低16位）
                    uint32_t value = (static_cast<uint32_t>(InDOStatus[ch0] & 0xFFFF) << 16) |
                                     (static_cast<uint32_t>(InDOStatus[ch1] & 0xFFFF));
                    
                    WriteTask task;
                    task.area = args.doArea;
                    task.dbNumber = (args.doArea == "DB") ? args.doDbNumber : 0;
                    task.address = args.doStart + dwordAddr;
                    task.data = {
                        static_cast<BYTE>((value >> 24) & 0xFF),  // 字节0
                        static_cast<BYTE>((value >> 16) & 0xFF),  // 字节1
                        static_cast<BYTE>((value >> 8) & 0xFF),   // 字节2
                        static_cast<BYTE>(value & 0xFF)           // 字节3
                    };
                    
                    std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
                    g_writeQueues[deviceIndex].push(task);
                }
                hasDirtyValue = true;
                break;
            }
        }
    }
    
    // ========== 2. 特殊功能通道（240-249）==========
    // 通道240: 区域码
    //   1 = DB区    - 通道241=DB号, 242=地址, 243=功能码, 244+=数据
    //   2 = PA区    - 通道241=地址, 242=功能码, 243+=数据
    //   3 = PE区    - 通道241=地址, 242=功能码, 243+=数据
    //   4 = MK区    - 通道241=地址, 242=功能码, 243+=数据
    //
    // 功能码:
    //   1 = 写字节 (Byte)   - 1字节数据
    //   2 = 写字 (Word)     - 2字节数据（大端序）
    //   3 = 写双字 (DWord)  - 4字节数据（大端序）
    //   4 = 写位 (Bit)      - 位地址(0-7) + 位值(0/1)，使用lastStatus重构字节
    auto _areaCode = InDOStatus[funcChannel];
    
    if (_areaCode == 1) {
        // DB区操作
        int dbNumber = InDOStatus[funcChannel + 1];
        int address = InDOStatus[funcChannel + 2];
        int funcCode = InDOStatus[funcChannel + 3];
        
        WriteTask task;
        task.area = "DB";
        task.dbNumber = dbNumber;
        task.address = address;
        
        if (funcCode == 1) {
            // 写字节
            BYTE value = static_cast<BYTE>(InDOStatus[funcChannel + 4] & 0xFF);
            task.data = {value};
        }
        else if (funcCode == 2) {
            // 写字
            uint16_t value = static_cast<uint16_t>(InDOStatus[funcChannel + 4] & 0xFFFF);
            task.data = {
                static_cast<BYTE>((value >> 8) & 0xFF),
                static_cast<BYTE>(value & 0xFF)
            };
        }
        else if (funcCode == 3) {
            // 写双字
            uint32_t value = (static_cast<uint32_t>(InDOStatus[funcChannel + 4] & 0xFFFF) << 16) |
                             (static_cast<uint32_t>(InDOStatus[funcChannel + 5] & 0xFFFF));
            task.data = {
                static_cast<BYTE>((value >> 24) & 0xFF),
                static_cast<BYTE>((value >> 16) & 0xFF),
                static_cast<BYTE>((value >> 8) & 0xFF),
                static_cast<BYTE>(value & 0xFF)
            };
        }
        else if (funcCode == 4) {
            // 写位 (使用lastStatus重构字节，避免读-改-写操作)
            int bitOffset = InDOStatus[funcChannel + 4] & 0x07;  // 位偏移 (0-7)
            int bitValue = InDOStatus[funcChannel + 5] & 0x01;   // 位值 (0/1)
            
            // 从lastStatus重构目标字节的当前状态
            BYTE currentByte = 0;
            int byteChannelStart = address * 8;  // 目标字节对应的通道起始位置
            for (int bit = 0; bit < 8; bit++) {
                int channelIndex = byteChannelStart + bit;
                if (channelIndex < funcChannel && lastStatus[channelIndex] > 0) {
                    currentByte |= (1 << bit);
                }
            }
            
            // 修改指定位
            if (bitValue) {
                currentByte |= (1 << bitOffset);
            } else {
                currentByte &= ~(1 << bitOffset);
            }
            
            task.data = {currentByte};
        }
        else {
            return 1; // 未知功能码
        }
        
        {
            std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
            g_writeQueues[deviceIndex].push(task);
        }
        hasDirtyValue = true;
    }
    else if (_areaCode >= 2 && _areaCode <= 4) {
        // PA/PE/MK区操作
        int address = InDOStatus[funcChannel + 1];
        int funcCode = InDOStatus[funcChannel + 2];
        
        WriteTask task;
        task.dbNumber = 0;
        task.address = address;
        
        // 设置区域
        switch (_areaCode) {
            case 2: task.area = "PA"; break;
            case 3: task.area = "PE"; break;
            case 4: task.area = "MK"; break;
        }
        
        if (funcCode == 1) {
            // 写字节
            BYTE value = static_cast<BYTE>(InDOStatus[funcChannel + 3] & 0xFF);
            task.data = {value};
        }
        else if (funcCode == 2) {
            // 写字
            uint16_t value = static_cast<uint16_t>(InDOStatus[funcChannel + 3] & 0xFFFF);
            task.data = {
                static_cast<BYTE>((value >> 8) & 0xFF),
                static_cast<BYTE>(value & 0xFF)
            };
        }
        else if (funcCode == 3) {
            // 写双字
            uint32_t value = (static_cast<uint32_t>(InDOStatus[funcChannel + 3] & 0xFFFF) << 16) |
                             (static_cast<uint32_t>(InDOStatus[funcChannel + 4] & 0xFFFF));
            task.data = {
                static_cast<BYTE>((value >> 24) & 0xFF),
                static_cast<BYTE>((value >> 16) & 0xFF),
                static_cast<BYTE>((value >> 8) & 0xFF),
                static_cast<BYTE>(value & 0xFF)
            };
        }
        else if (funcCode == 4) {
            // 写位 (使用lastStatus重构字节，避免读-改-写操作)
            int bitOffset = InDOStatus[funcChannel + 3] & 0x07;  // 位偏移 (0-7)
            int bitValue = InDOStatus[funcChannel + 4] & 0x01;   // 位值 (0/1)
            
            // 从lastStatus重构目标字节的当前状态
            BYTE currentByte = 0;
            int byteChannelStart = address * 8;  // 目标字节对应的通道起始位置
            for (int bit = 0; bit < 8; bit++) {
                int channelIndex = byteChannelStart + bit;
                if (channelIndex < funcChannel && lastStatus[channelIndex] > 0) {
                    currentByte |= (1 << bit);
                }
            }
            
            // 修改指定位
            if (bitValue) {
                currentByte |= (1 << bitOffset);
            } else {
                currentByte &= ~(1 << bitOffset);
            }
            
            task.data = {currentByte};
        }
        else {
            return 1; // 未知功能码
        }
        
        {
            std::lock_guard<std::mutex> lock(g_queueMutexes[deviceIndex]);
            g_writeQueues[deviceIndex].push(task);
        }
        hasDirtyValue = true;
    }
    
    // 通知写线程
    if (hasDirtyValue) {
        g_writeQueueConditions[deviceIndex].notify_one();
    }
    
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus) {
    if (!g_lastDOStatus.count(deviceIndex)) return 0;
    
    std::lock_guard<std::mutex> lock(g_dataMutexes[deviceIndex]);
    auto& lastStatus = g_lastDOStatus[deviceIndex];
    std::copy(lastStatus.begin(), lastStatus.end(), OutDOStatus);
    
    // 重置特殊功能通道（240-249）为0，避免重复执行
    std::fill_n(OutDOStatus + funcChannel, 10, 0);
    
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus) {
    if (!g_readDIData.count(deviceIndex)) return 0;
    if (!g_args.count(deviceIndex)) return 0;
    
    auto& args = g_args[deviceIndex];
    if (args.diBytes <= 0) return 0;
    
    std::lock_guard<std::mutex> lock(g_dataMutexes[deviceIndex]);
    auto& buffer = g_readDIData[deviceIndex];
    
    // 将字节数据解析为位数据
    for (size_t _byte = 0; _byte < static_cast<size_t>(args.diBytes); _byte++) {
        const BYTE& data = buffer[_byte];
        for (size_t _bit = 0; _bit < 8; _bit++) {
            int ch = _byte * 8 + _bit;
            if (ch >= devInfo.InputCount) break;
            OutDIStatus[ch] = (data & (1 << _bit)) ? 1 : 0;
        }
    }
    
    return 1;
}

IOUI_API int __stdcall GetDeviceAD_INT(uint8 deviceIndex, int32_t* OutADStatus) {
    if (!g_readAIData.count(deviceIndex)) return 0;
    if (!g_args.count(deviceIndex)) return 0;
    
    auto& args = g_args[deviceIndex];
    if (args.channelConfigs.empty()) return 0;
    
    std::lock_guard<std::mutex> lock(g_dataMutexes[deviceIndex]);
    auto& buffer = g_readAIData[deviceIndex];
    
    // 遍历每个配置的通道
    for (const auto& pair : args.channelConfigs) {
        int channelIndex = pair.first;
        const ChannelConfig& config = pair.second;
        
        if (channelIndex >= devInfo.InputCount) continue;
        
        // 获取该通道的数据缓冲区
        uint16_t* channelBuffer = buffer.data() + channelIndex * 2;
        
        // 解析原始值
        double rawValue = parseRegisterValue(channelBuffer, config.dataType, config.byteOrder);
        
        // 应用表达式转换（如果有）
        double scaledValue = rawValue;
        if (config.compiledExpr) {
            scaledValue = config.compiledExpr->eval(rawValue);
        }
        
        // 转换为int32_t（不截断，支持更大范围）
        OutADStatus[channelIndex] = static_cast<int32_t>(scaledValue);
    }
    
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
    return 0; // 不支持
}
