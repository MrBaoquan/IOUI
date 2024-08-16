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
#include "IOUI.h"
#include <memory>
#include "Paths.hpp"
#include "modbus/modbus.h"
#include "mIni/mini/ini.h"
#include "Util.hpp"
#include "Debug.hpp"

#pragma comment(lib,"modbus.lib")

struct ModbusArgs
{
public:
    int SlaveAddr;

    int DIFuncCode;
    int DIReadAddr;
    int DIReadCount;
    int DIAsyncRead;

    int AIFuncCode;
    int AIReadAddr;
    int AIReadCount;
    int AIAsyncRead;

    int Divisor;
    int ParseMode;
    int JumpThreshold;
    
    ModbusArgs() {
        SlaveAddr = 1;
        DIReadAddr = 0;
        DIReadCount = 2;
        Divisor = 1;
        ParseMode = 0;
        DIAsyncRead = 0;
    }
};


// 全局modbus设备相关
std::map<uint8,modbus_t*> g_ctxs;
std::map<uint8, ModbusArgs> g_args;


// 读取保持寄存器数据
std::map<uint8,uint16_t*> g_readAIData;
// 读取离散输入数据
std::map<uint8,uint8_t*> g_readDIData;

// di 线程读取计数
std::map<uint8,int> g_diReadCount;

// ai 线程读取计数
std::map<uint8,int> g_aiReadCount;

// 配置文件相关
std::shared_ptr<mINI::INIFile> g_iniFile;
std::shared_ptr<mINI::INIStructure> g_iniStructure;

// 读取线程相关
std::map<uint8,std::atomic<int>> g_threadFinishers;


DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 128;
	devInfo.OutputCount = 128;
	devInfo.AxisCount = 16;
	return &devInfo;
}


/// @brief 读取modbus寄存器
/// @param deviceIndex 
/// @param functionCode 
/// @return 
int queryModbusRegisters(uint8 deviceIndex, int functionCode){
    auto& _ctx = g_ctxs[deviceIndex];
    auto& _args = g_args[deviceIndex];
    
    if (_args.DIReadAddr < 0) return 0;

    if(functionCode==0x01){ // 读取线圈
        auto _readData = g_readDIData[deviceIndex];
        auto _readCount = modbus_read_bits(_ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
        // 错误处理
        if (_readCount == -1) {
            auto _err = modbus_strerror(errno);
            printf("%s", _err);
            return -1;
        }
        g_diReadCount[deviceIndex] = _readCount;
        return _readCount;
    }else if(functionCode==0x02){   // 读取离散输入
        auto _readData = g_readDIData[deviceIndex];
        auto _readCount = modbus_read_input_bits(_ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
        // 错误处理
        if (_readCount == -1) {
            auto _err = modbus_strerror(errno);
            printf("%s", _err);
            return -1;
        }
        g_diReadCount[deviceIndex] = _readCount;
        return _readCount;
    }else if(functionCode==0x03){   // 读取保持寄存器
        auto _readData = g_readAIData[deviceIndex];
        auto _readCount = modbus_read_registers(_ctx, _args.DIReadAddr, _args.AIReadCount, _readData);
        // 错误处理
        if (_readCount == -1) {
            auto _err = modbus_strerror(errno);
            printf("%s", _err);
            return -1;
        }
        g_aiReadCount[deviceIndex] = _readCount;
        return _readCount;
    }else if(functionCode==0x04){   // 读取输入寄存器
        auto _readData = g_readAIData[deviceIndex];
        auto _readCount = modbus_read_input_registers(_ctx, _args.DIReadAddr, _args.DIReadCount, _readData);
        // 错误处理
        if (_readCount == -1) {
            auto _err = modbus_strerror(errno);
            printf("%s", _err);
            return -1;
        }
        g_aiReadCount[deviceIndex] = _readCount;
        return _readCount;
    }
    return -1;
}

void queryModbusRegistersThread(uint8 deviceIndex) {

    auto _args = g_args[deviceIndex];
    std::thread _diThread([deviceIndex,_args](){
        while (g_threadFinishers[deviceIndex].load()!=0)
        {
            if(_args.DIAsyncRead == 1 && _args.DIReadCount > 0){
                queryModbusRegisters(deviceIndex, _args.DIFuncCode); 
            }

			if (_args.AIAsyncRead == 1 && _args.AIReadCount > 0) {
				queryModbusRegisters(deviceIndex, _args.AIFuncCode);
			}
        }
        g_threadFinishers[deviceIndex].store(2);
    });

    _diThread.detach();

    std::thread _aiThread([deviceIndex,_args](){
        while (g_threadFinishers[deviceIndex].load()!=0)
        {
			/*	if (_args.AIAsyncRead == 1 && _args.AIReadCount > 0) {
					queryModbusRegisters(deviceIndex, _args.AIFuncCode);
				}*/
        }
        while (g_threadFinishers[deviceIndex].load() >= 0)
        {
            g_threadFinishers[deviceIndex].store(-1);
        }
    });

    _aiThread.detach();
}


IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{

    // debugFunc(); return 0;
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\modbus.dll";
	auto _module = LoadLibraryA(_path.data());

	std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\MODBUS\\config.ini";
    g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
    g_iniStructure = std::make_shared<mINI::INIStructure>();
	g_iniFile->read(*g_iniStructure);
    auto& ini = *g_iniStructure;
    const auto& _iniApp = BuildDeviceAttribute("modbus", deviceIndex);

    auto& _iniStructure = ini[_iniApp];
  
    auto _slaveAddr = _iniStructure.has("slave_addr")? std::stoi(_iniStructure["slave_addr"]):1;

    auto _diFuncCode = _iniStructure.has("di_func")? std::stoi(_iniStructure["di_func"],nullptr,16):0x02;
    auto _diReadAddr = _iniStructure.has("di_addr")?std::stoi(_iniStructure["di_addr"],nullptr,16):0x0000;
    auto _diReadCount = _iniStructure.has("di_num")? std::stoi(_iniStructure["di_num"],nullptr,16):0x0020;
    auto _diAsyncRead = _iniStructure.has("di_async_read") ? std::stoi(_iniStructure["di_async_read"]) : 0;

    auto _aiFuncCode = _iniStructure.has("ai_func")? std::stoi(_iniStructure["ai_func"],nullptr,16):0x03;
    auto _aiReadAddr = _iniStructure.has("ai_addr")?std::stoi(_iniStructure["ai_addr"],nullptr,16):0x0000;
    auto _aiReadCount = _iniStructure.has("ai_num")? std::stoi(_iniStructure["ai_num"],nullptr,16):0x0002;
    auto _aiAsyncRead = _iniStructure.has("ai_async_read") ? std::stoi(_iniStructure["ai_async_read"]) : 0;

    auto _parseMode = _iniStructure.has("parse_mode") ? std::stoi(_iniStructure["parse_mode"]) : 0;
    auto _divisor = _iniStructure.has("divisor") ? std::stoi(_iniStructure["divisor"]) : 1;
    // 跳变阈值
    auto _jumpThreshold = _iniStructure.has("jump_threshold") ? std::stoi(_iniStructure["jump_threshold"]) : 500;

    auto _writeTimeout = _iniStructure.has("write_timeout") ? std::stoi(_iniStructure["write_timeout"]) : 10;

	if (!g_ctxs.count(deviceIndex)) {
        auto _modbusDriver = _iniStructure.has("driver") ? _iniStructure["driver"] : "modbus_rtu";
        if(_modbusDriver == "modbus_rtu"){
            auto _port = _iniStructure.has("port")?_iniStructure["port"]:"1";
            std::string _serialName = std::string("\\\\.\\COM") + _port;
            auto _baudRate = _iniStructure.has("baud_rate")?std::stoi(_iniStructure["baud_rate"]):9600;
            auto _dataBit = _iniStructure.has("data_bit")?std::stoi(_iniStructure["data_bit"]):8;
            auto _stopBit = _iniStructure.has("stop_bit")?std::stoi(_iniStructure["stop_bit"]):1;
            auto _parity = _iniStructure.has("parity")?_iniStructure["parity"]:"N";
            g_ctxs.insert(std::pair<uint8, modbus_t*>(deviceIndex, modbus_new_rtu(_serialName.data(),_baudRate, _parity[0], _dataBit, _stopBit)));
        }else if(_modbusDriver == "modbus_tcp"){
            auto _ip = _iniStructure.has("ip")?_iniStructure["ip"]:"127.0.0.1";
            auto _port = _iniStructure.has("port")?std::stoi(_iniStructure["port"]):502;
            g_ctxs.insert(std::pair<uint8, modbus_t*>(deviceIndex, modbus_new_tcp(_ip.data(),_port)));
        }else{
            return 0;
        }

		g_args.insert(std::pair<uint8, ModbusArgs>(deviceIndex, ModbusArgs()));
        
        uint16_t* _readData = new uint16_t[devInfo.InputCount];
        std::fill(_readData, _readData + devInfo.InputCount, 0);
        g_readAIData.insert(std::pair<uint8, uint16_t*>(deviceIndex, _readData));
        g_diReadCount.insert(std::pair<uint8, int>(deviceIndex, 0));

        g_readDIData.insert(std::pair<uint8,uint8_t*>(deviceIndex,new uint8_t[devInfo.InputCount]));
	}

    auto& _args = g_args[deviceIndex];
    
    _args.SlaveAddr = _slaveAddr;

    _args.DIFuncCode = _diFuncCode;
    _args.DIReadAddr = _diReadAddr;
    _args.DIReadCount = _diReadCount;
    _args.DIAsyncRead = _diAsyncRead;

    _args.AIFuncCode = _aiFuncCode;
    _args.AIReadAddr = _aiReadAddr;
    _args.AIReadCount = _aiReadCount;
    _args.AIAsyncRead = _aiAsyncRead;

    _args.ParseMode = _parseMode;
    _args.Divisor = _divisor;
    _args.JumpThreshold = _jumpThreshold;
   

    auto ctx = g_ctxs[deviceIndex];
    if (ctx == NULL) {
        return 0;
    }

    // 设置modbus从机地址
    auto _ret = modbus_set_slave(ctx, _args.SlaveAddr);

    // 设置写入超时时间
    modbus_set_response_timeout(ctx, _writeTimeout/1000,_writeTimeout%1000*1000);

    if (modbus_connect(ctx) == -1) {
        modbus_free(ctx);
        return 0;
    }

    // 写入单个保持寄存器的数据
    int _writeID = 0;
    while (_iniStructure.has("write_single_addr_" + std::to_string(_writeID)))
    {
        auto _writeAddr = std::stoi(_iniStructure["write_single_addr_" + std::to_string(_writeID)], nullptr, 16);
        auto _writeData = std::stoi(_iniStructure["write_single_data_" + std::to_string(_writeID)], nullptr, 16);
        modbus_write_register(ctx, _writeAddr, _writeData);
        _writeID++;
    }

    // 写入多个保持寄存器的数据
    _writeID = 0;
    while (_iniStructure.has("write_multi_addr_" + std::to_string(_writeID)))
    {
        auto _writeAddr = std::stoi(_iniStructure["write_multi_addr_" + std::to_string(_writeID)], nullptr, 16);
        auto _writeCount = std::stoi(_iniStructure["write_multi_num_" + std::to_string(_writeID)], nullptr, 16);
        std::vector<uint16_t> _writeData;
        for (int i = 0; i < _writeCount; i++)
        {
            _writeData.push_back(std::stoi(_iniStructure["write_multi_data_" + std::to_string(_writeID) + "_" + std::to_string(i)], nullptr, 16));
        }
        modbus_write_registers(ctx, _writeAddr, _writeCount, _writeData.data());
        _writeID++;
    }
    
    // 设置读取超时时间
    auto _readTimeout = _iniStructure.has("read_timeout") ? std::stoi(_iniStructure["read_timeout"]) : 50;
    modbus_set_response_timeout(ctx, _readTimeout/1000,_readTimeout%1000*1000);

    g_threadFinishers.insert(std::make_pair(deviceIndex, 1));

    if(_args.DIAsyncRead == 1){
        std::thread th([deviceIndex](){
            queryModbusRegistersThread(deviceIndex);
        });
        th.detach();
    }
    
    EnableConsoleDebug();
    
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    auto ctx = g_ctxs[deviceIndex];
    if (ctx == NULL) return 0;

    auto _args = g_args[deviceIndex];
    if (_args.DIAsyncRead == 1) {
        g_threadFinishers[deviceIndex].store(0);
        while(g_threadFinishers[deviceIndex].load() >= 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    modbus_close(ctx);
    modbus_free(ctx);
    
    delete[] g_readAIData[deviceIndex];
    delete[] g_readDIData[deviceIndex];

    g_readAIData.erase(deviceIndex);
    g_readDIData.erase(deviceIndex);
    g_diReadCount.erase(deviceIndex);
    g_aiReadCount.erase(deviceIndex);
    
    g_ctxs.erase(deviceIndex);
    g_args.erase(deviceIndex);

    if(g_ctxs.size() == 0){
        DisableConsoleDebug();
    }
  
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    // 05 写单个线圈
    // 06 写单个寄存器
    auto _writeType = InDOStatus[0];
    auto _writeAddr = InDOStatus[1];
    auto _writeData = InDOStatus[2];

    if (_writeType == 5) {
        int _bitValue = _writeData == 0 ? 0 : 1;
		modbus_write_bit(g_ctxs[deviceIndex], _writeAddr,_bitValue);
	}
    else if (_writeType == 6) {
		modbus_write_register(g_ctxs[deviceIndex], _writeAddr, _writeData);
	}
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    // zero memory
    std::fill(OutDOStatus, OutDOStatus + devInfo.OutputCount, 0);
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    auto _args = g_args[deviceIndex];
	if (_args.DIAsyncRead == 0 && _args.DIReadCount > 0) {
		queryModbusRegisters(deviceIndex, _args.DIFuncCode);
	}
    
    auto _recvCount = g_diReadCount[deviceIndex];
	if (_recvCount <=0) {
		return 0;
	}

    ClearConsole();
    auto& _readData = g_readDIData[deviceIndex];

    // 按位解析
    for(uint8 _idx=0; _idx< _recvCount;++_idx)
    {
        auto _byteValue = _readData[_idx];

        OutDIStatus[_idx] = _byteValue;
        if(_idx >= devInfo.InputCount) break;
    }
    PrintDIData( OutDIStatus, min(_recvCount,devInfo.InputCount));
    
    g_diReadCount[deviceIndex] = -1;
    return 1;
}



IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    PrintLine();
    auto _args = g_args[deviceIndex];
    if (_args.AIAsyncRead == 0 && _args.AIReadCount > 0) {
        queryModbusRegisters(deviceIndex, _args.AIFuncCode);
    }
    
    auto _recvCount = g_aiReadCount[deviceIndex];
    if (_recvCount <= 0) {
        return 0;
    }
    
    auto tab_reg = g_readAIData[deviceIndex];

    if (_args.ParseMode == 0) {
        for (uint8 _idx=0;_idx<_recvCount;++_idx)
        {
            OutADStatus[_idx] = tab_reg[_idx];
        }
    }
    else if (_args.ParseMode == 1) {
        static uint32_t _lastValue = 0;
        // 合并成一个数值
        uint32_t _value = 0;
        for (uint8 _idx = 0; _idx < _recvCount; ++_idx)
        {
            _value = _value << 16;
            _value |= tab_reg[_idx];
        }
        _value /= _args.Divisor;
        int _delta = _value - _lastValue;
        //  值为线圈值, 可能会存在跳变, 此处进行屏蔽
        if (abs(_delta) > _args.JumpThreshold){
			_delta = 0;
		}

        _lastValue = _value;
        OutADStatus[0] = _value;
        OutADStatus[1] = _delta;
    }
    PrintAIData(OutADStatus, 1);

    g_aiReadCount[deviceIndex] = -1;
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
