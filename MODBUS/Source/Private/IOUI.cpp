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

struct ModbusArgs
{
public:
    int SlaveAddr;
    int DIFuncCode;
    int DIReadAddr;
    int DIReadCount;
    int AIFuncCode;
    int AIReadAddr;
    int AIReadCount;
    int Divisor;
    int ParseMode;
    int JumpThreshold;
    int TimeoutMs;      // 新增：超时(ms)
    int RetryWaitMs;    // 新增：重试等待(ms)
    int DoAddr;         // 新增：写线圈地址偏移
    int ReadWaitMs;     // 读取指令等待(ms)

    ModbusArgs() {
        SlaveAddr = 1;
        DIReadAddr = 0;
        DIReadCount = 2;
        Divisor = 1;
        ParseMode = 0;
        JumpThreshold = 500;
        TimeoutMs = 100;     // 默认100ms
        RetryWaitMs = 20;    // 默认20ms
        ReadWaitMs = 20;
        DoAddr = 0;          // 默认偏移0
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

std::map<uint8, std::queue<WriteTask>> g_writeQueues;
std::map<uint8, std::mutex> g_queueMutexes;
std::map<uint8, std::condition_variable> g_writeQueueConditions;
std::map<uint8, std::condition_variable> g_readConditions;

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

int queryModbusRegisters(uint8 deviceIndex, int functionCode)
{
    auto itCtx = g_ctxs.find(deviceIndex);
    if (itCtx == g_ctxs.end()) return 0;
    modbus_t* ctx = itCtx->second;

    auto& _args = g_args[deviceIndex];
    auto connectionKey = g_deviceConnectionKey[deviceIndex];

    // 使用共享连接对应互斥锁，保证切换从机及读写不会被竞争
    std::lock_guard<std::mutex> lock(g_connectionMutexes[connectionKey]);

    if (modbus_set_slave(ctx, _args.SlaveAddr) == -1) {
        return -1;
    }

    if (_args.DIReadAddr < 0) return 0;

    if (functionCode == 0x01) {
        auto _readData = g_readDIData[deviceIndex];
        int _readCount = modbus_read_bits(ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
        if (_readCount == -1) return -1;
        g_diReadCount[deviceIndex] = _readCount;
        return _readCount;
    }
    else if (functionCode == 0x02) {
        auto _readData = g_readDIData[deviceIndex];
        int _readCount = modbus_read_input_bits(ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
        if (_readCount == -1) return -1;
        g_diReadCount[deviceIndex] = _readCount;
        return _readCount;
    }
    else if (functionCode == 0x03) {
        auto _readData = g_readAIData[deviceIndex];
        int _readCount = modbus_read_registers(ctx, _args.AIReadAddr, _args.AIReadCount, _readData);
        if (_readCount == -1) return -1;
        g_aiReadCount[deviceIndex] = _readCount;
        return _readCount;
    }
    else if (functionCode == 0x04) {
        auto _readData = g_readAIData[deviceIndex];
        int _readCount = modbus_read_input_registers(ctx, _args.AIReadAddr, _args.AIReadCount, _readData);
        if (_readCount == -1) return -1;
        g_aiReadCount[deviceIndex] = _readCount;
        return _readCount;
    }
    return -1;
}

void queryModbusRegistersThread(uint8 deviceIndex) {
    auto& _args = g_args[deviceIndex];
    auto connectionKey = g_deviceConnectionKey[deviceIndex];

    // 读线程
    std::thread _diThread([deviceIndex, &_args, connectionKey]() {
        static std::map<uint8, std::mutex> _mutexes;
        while (!g_stopFlags[deviceIndex].load())
        {
            // wait通知改为超时等待，避免死锁
            std::unique_lock<std::mutex> lock(_mutexes[deviceIndex]);
            g_readConditions[deviceIndex].wait_for(lock, std::chrono::milliseconds(100), [deviceIndex] {
                return g_stopFlags[deviceIndex].load();
                });
            lock.unlock();

            if (g_stopFlags[deviceIndex].load()) break;

            if (_args.DIReadCount > 0) {
                queryModbusRegisters(deviceIndex, _args.DIFuncCode);
                std::this_thread::sleep_for(std::chrono::milliseconds(_args.ReadWaitMs));
            }

            if (_args.AIReadCount > 0) {
                queryModbusRegisters(deviceIndex, _args.AIFuncCode);
                std::this_thread::sleep_for(std::chrono::milliseconds(_args.ReadWaitMs));
            }
        }
        });

    // 写线程
    std::thread _aiThread([deviceIndex, &_args, connectionKey]() {
        while (!g_stopFlags[deviceIndex].load())
        {
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
                if (task.writeType == 5) {
                    int _bitValue = task.writeData == 0 ? 0 : 1;
                    int addrWithOffset = task.writeAddr + _args.DoAddr;
                    execModbusWithRetry([&]() {
                        return modbus_write_bit(ctx, addrWithOffset, _bitValue);
                        }, 5, _args.RetryWaitMs);
                }
                else if (task.writeType == 6) {
                    execModbusWithRetry([&]() {
                        return modbus_write_register(ctx, task.writeAddr, task.writeData);
                        }, 5, _args.RetryWaitMs);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(_args.RetryWaitMs));
            }
        }
        });
    _diThread.join();
    _aiThread.join();
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
    if (defaultConfig.count("slave_addr")) _slaveAddr = std::stoi(defaultConfig["slave_addr"]);
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
        uint16_t* aiData = new uint16_t[devInfo.InputCount];
        std::fill(aiData, aiData + devInfo.InputCount, 0);
        g_readAIData[deviceIndex] = aiData;

        uint8_t* diData = new uint8_t[devInfo.InputCount];
        g_readDIData[deviceIndex] = diData;

        g_diReadCount[deviceIndex] = 0;
        g_aiReadCount[deviceIndex] = 0;

        lastDOStatusMap[deviceIndex] = std::vector<short>(devInfo.OutputCount, 0);
    }

    auto& _args = g_args[deviceIndex];
    _args.SlaveAddr = _slaveAddr;
    _args.DIFuncCode = defaultConfig.count("di_func") ? std::stoi(defaultConfig["di_func"], nullptr, 16) : 0x02;
    _args.DIReadAddr = defaultConfig.count("di_addr") ? std::stoi(defaultConfig["di_addr"], nullptr, 16) : 0x0000;
    _args.DIReadCount = defaultConfig.count("di_num") ? std::stoi(defaultConfig["di_num"], nullptr, 16) : 0x0000;
    _args.AIFuncCode = defaultConfig.count("ai_func") ? std::stoi(defaultConfig["ai_func"], nullptr, 16) : 0x03;
    _args.AIReadAddr = defaultConfig.count("ai_addr") ? std::stoi(defaultConfig["ai_addr"], nullptr, 16) : 0x0000;
    _args.AIReadCount = defaultConfig.count("ai_num") ? std::stoi(defaultConfig["ai_num"], nullptr, 16) : 0x0000;
    _args.ParseMode = defaultConfig.count("parse_mode") ? std::stoi(defaultConfig["parse_mode"]) : 0;
    _args.Divisor = defaultConfig.count("divisor") ? std::stoi(defaultConfig["divisor"]) : 1;
    _args.JumpThreshold = defaultConfig.count("jump_threshold") ? std::stoi(defaultConfig["jump_threshold"]) : 500;

    // 读取超时和重试等待配置
    if (defaultConfig.count("timeout_ms"))
        _args.TimeoutMs = std::stoi(defaultConfig["timeout_ms"]);
    if (defaultConfig.count("retry_wait_ms"))
        _args.RetryWaitMs = std::stoi(defaultConfig["retry_wait_ms"]);

    if (defaultConfig.count("read_wait_ms"))
        _args.ReadWaitMs = std::stoi(defaultConfig["read_wait_ms"]);

    // 读取写线圈地址偏移配置
    if (defaultConfig.count("do_addr"))
        _args.DoAddr = std::stoi(defaultConfig["do_addr"], nullptr, 16);
    else
        _args.DoAddr = 0;

    // 设置modbus超时
    modbus_set_response_timeout(ctx, _args.TimeoutMs / 1000, (_args.TimeoutMs % 1000) * 1000);

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

    // 通知线程以防阻塞
    g_writeQueueConditions[deviceIndex].notify_all();
    g_readConditions[deviceIndex].notify_all();

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
                g_connectionMutexes.erase(connectionKey); // 删除对应锁
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
    lastDOStatusMap.erase(deviceIndex);

    g_ctxs.erase(deviceIndex);
    g_args.erase(deviceIndex);

    /*auto _msg = std::to_string(deviceIndex) + " closed. \n";
    OutputDebugStringA(_msg.data());*/

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
        auto funcCode = InDOStatus[writeFuncChannel];
        funcCode = funcCode == 0 ? 5 : funcCode;
        for (short i : changedChannel) {
            WriteTask task;
            task.writeType = funcCode;
            task.writeAddr = i;
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

    g_readConditions[deviceIndex].notify_one();

    auto _recvCount = g_diReadCount[deviceIndex];
    if (_recvCount <= 0) return 0;

    auto& _readData = g_readDIData[deviceIndex];

    for (uint8 _idx = 0; _idx < _recvCount; ++_idx) {
        auto _byteValue = _readData[_idx];
        OutDIStatus[_idx] = _byteValue;
        if (_idx >= devInfo.InputCount) break;
    }

    g_diReadCount[deviceIndex] = -1;
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    auto _args = g_args[deviceIndex];
    auto _recvCount = g_aiReadCount[deviceIndex];
    if (_recvCount <= 0) return 0;

    auto& tab_reg = g_readAIData[deviceIndex];

    if (_args.ParseMode == 0) {
        for (uint8 _idx = 0; _idx < _recvCount; ++_idx) {
            OutADStatus[_idx] = tab_reg[_idx] / _args.Divisor;
        }
    }
    else if (_args.ParseMode == 1) {
        static uint32_t _lastValue = 0;
        uint32_t _value = 0;
        for (uint8 _idx = 0; _idx < _recvCount; ++_idx) {
            _value = (_value << 16) | tab_reg[_idx];
        }
        _value /= _args.Divisor;
        int _delta = _value - _lastValue;
        if (abs(_delta) > _args.JumpThreshold) {
            _delta = 0;
        }
        _lastValue = _value;
        OutADStatus[0] = _value;
        OutADStatus[1] = _delta;
    }
    g_aiReadCount[deviceIndex] = -1;
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
    return 0;
}
