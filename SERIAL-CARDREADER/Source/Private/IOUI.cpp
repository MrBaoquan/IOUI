/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <algorithm>
#include "IOUI.h"
#include "Paths.hpp"
#include <utility>
#include <limits.h>
#include "windows.h"
#include "mIni/mini/ini.h"
#include <map>
#include <chrono>
#include "Serial.hpp"
#include "Util.hpp"
#include <iomanip>  // 用于格式化输出
#include <sstream>  // 用于字符串流

namespace dh = DevelopHelper;

extern HINSTANCE DLL_INSTANCE;
HHOOK g_hHook = NULL;
std::string g_terminalKey;
// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

// 串口实例指针
std::map<uint8, std::shared_ptr<Serial>> g_serialPorts;

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 255;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 0;
    return &devInfo;
}


std::string convert_char(unsigned char ch, int mode) {
	if (mode == 0) {
		// mode 0: 返回ASCII字符
		if (ch >= 32 && ch <= 126) {
			return std::string(1, ch);  // 将字符转换为长度为1的std::string
		}
		else {
			return ".";
		}
	}
	else if (mode == 1) {
		// mode 1: 返回16进制字符串
		std::ostringstream oss;
		oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
		return oss.str();  // 返回格式化的16进制字符串
	}
	else {
		// 非法模式
		return "";
	}
}

// 将 unsigned char 数组转换为16进制字符串
std::string to_hex_string(const unsigned char* data, size_t length) {
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(data[i]);
    }
    return oss.str();
}

// 判断数组的16进制表示是否以指定的字符串开头
bool starts_with_hex(const unsigned char* data, size_t length, const std::string& input) {
    std::string hex_string = to_hex_string(data, length);
    return hex_string.rfind(input, 0) == 0;  // 检查是否以input开头
}

// 将前count个字节转换为指定格式的字符串，并用 | 分隔
std::string vector_to_hex_string(const std::vector<unsigned char>& data, size_t count = 11, int mode = 1) {
    std::ostringstream oss;

    // 取前count个字节（最多11个，如果vector长度不足则取实际长度）
    size_t length = min(data.size(), count);
    for (size_t i = 0; i < length; ++i) {
        // 使用 convert_char 函数转换每个字节
        oss << convert_char(data[i], mode);

        if (i < length - 1) {
            oss << " ";  // 添加 | 分隔符，最后一个元素后不加
        }
    }

    return oss.str();
}

std::map<std::string, int> g_comboKeysMap;
mINI::INIFile* g_iniFIle = nullptr;
mINI::INIStructure* g_iniStructure = nullptr;

int AppendComboKeys(const std::string& comboKey) {
	if (g_comboKeysMap.count(comboKey) > 0) return -1;

	auto& ini = *g_iniStructure;
	auto& file = *g_iniFIle;
	for (int _idx=0;_idx<devInfo.InputCount;++_idx)
	{
		std::string _key = "k" + std::to_string(_idx);
		if (!ini["ComboKeys"].has(_key)) {
			ini["ComboKeys"][_key] = comboKey;
			g_comboKeysMap.insert(std::pair<std::string, int>(comboKey, _idx));
			file.write(ini);
			return _idx;
		}
	}
	return -1;
}

// 通道超时值 ms
int g_timeout = 500;
int g_comboMode = 1;
int g_dataBits = 11;
std::string g_startBits = "20";

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\SERIAL-CARDREADER\\config.ini";

	g_iniFIle = new  mINI::INIFile(config_file_path);
	g_iniStructure = new  mINI::INIStructure();
	g_iniFIle->read(*g_iniStructure);
	auto& ini = *g_iniStructure;

	const char* app = "Serial";
	auto& _baudRateString = ini["Serial"][BuildDeviceAttribute("BaudRate", deviceIndex)];
	DWORD _baudRate = _baudRateString==""?9600: std::stoi(_baudRateString);
	g_timeout = std::stoi(ini["Serial"]["Timeout"]!=""? ini["Serial"]["Timeout"] : "500");

	g_comboMode = std::stoi(ini["Serial"]["ComboMode"]!=""? ini["Serial"]["ComboMode"] : "1");
	g_dataBits = std::stoi(ini["Serial"]["DataBits"]!=""? ini["Serial"]["DataBits"] : "11");
	g_startBits = ini["Serial"]["StartBits"]!=""? ini["Serial"]["StartBits"] : "20";

	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex), _baudRate);
		g_serialPorts.insert(std::pair<uint8, std::shared_ptr<Serial>>(deviceIndex, _serialPort));

		for (int _idx = 0; _idx < devInfo.InputCount; ++_idx)
		{
			std::string _key = "k" + std::to_string(_idx);
			std::string& _val = ini["ComboKeys"][_key];
			if (_val == "") {
				ini["ComboKeys"].remove(_key);
				continue;
			}
			g_comboKeysMap.insert(std::pair<std::string, int>(_val, _idx));
		}
	}
	catch (const char*)
	{
		delete g_iniFIle;
		delete g_iniStructure;
		return 0;
	}

	return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	delete g_iniFIle;
	delete g_iniStructure;
	g_serialPorts[deviceIndex]->flush();
	g_serialPorts.erase(deviceIndex);
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	static std::map<uint8, std::map<uint8, std::chrono::system_clock::time_point>> _AllDILastUpdateTime;
	static std::map<uint8, std::vector<uint8>> _allDevData;
	if (_allDevData.count(deviceIndex) <= 0) {
		_allDevData.insert(std::pair<uint8, std::vector<uint8>>(deviceIndex, std::vector<uint8>()));
	}
	if (_AllDILastUpdateTime.count(deviceIndex) <= 0) {
		_AllDILastUpdateTime.insert(std::pair<uint8, std::map<uint8, std::chrono::system_clock::time_point>>(deviceIndex, std::map<uint8, std::chrono::system_clock::time_point>()));
	}

	auto _serialPort = g_serialPorts[deviceIndex];

	static char _tempRecv[MAX_PATH];
	auto _recvCount = _serialPort->read(_tempRecv, MAX_PATH, false);

	auto& _devData = _allDevData.at(deviceIndex);
	if (_recvCount > 0) {
		std::vector<uint8> _tempRecvData(std::begin(_tempRecv), std::begin(_tempRecv) + _recvCount);
		_devData.insert(_devData.end(), _tempRecvData.begin(), _tempRecvData.end());
	}


	auto& _diLastUpdateTime = _AllDILastUpdateTime.at(deviceIndex);
	while (_devData.size() >= g_dataBits)
	{
		if(!starts_with_hex(_devData.data(), _devData.size(), g_startBits)){
			_devData.erase(_devData.begin());
			continue;
		}

		g_cachedComboKeys.push_back(vector_to_hex_string(_devData, g_dataBits,g_comboMode));
		_devData.erase(_devData.begin(), _devData.begin() + g_dataBits);
	}
	
	if (g_timeout <= 0) {
		ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
	}

	for (const std::string& _comboKey :g_cachedComboKeys)
	{
		if (g_comboKeysMap.count(_comboKey) <= 0)AppendComboKeys(_comboKey);
		if (g_comboKeysMap.count(_comboKey) <= 0) continue;
		auto _channel = g_comboKeysMap[_comboKey];
		OutDIStatus[_channel] = 1;
		if(g_timeout<=0) continue;

		if (_diLastUpdateTime.count(_channel) <= 0) {
			_diLastUpdateTime.insert(std::pair<uint8, std::chrono::system_clock::time_point>(_channel, std::chrono::system_clock::now()));
		}
		_diLastUpdateTime[_channel] = std::chrono::system_clock::now();
	}
	if (g_timeout >= 50) {
		auto _now = std::chrono::system_clock::now();
		for (int _idx = 0; _idx < devInfo.InputCount; ++_idx)
		{
			if (OutDIStatus[_idx] > 0) {
				std::chrono::duration<double, std::milli> _elapsed = (_now - _diLastUpdateTime[_idx]);
				if (_elapsed.count() >= g_timeout) {
					OutDIStatus[_idx] = 0;
				}
			}
		}
	}
	g_cachedComboKeys.clear();
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}