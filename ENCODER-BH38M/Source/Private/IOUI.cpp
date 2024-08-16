/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include "IOUI.h"
#include <intsafe.h>
#include <memory>
#include <chrono>
#include <map>
#include "Paths.hpp"
#include "Serial.hpp"
#include "Util.hpp"
#include <cmath>

namespace dh = DevelopHelper;

DeviceInfo devInfo;

// 串口实例指针
std::map<uint8, std::shared_ptr<Serial>> g_serialPorts;
// 分辨率
std::map<uint8, int> g_sResolutions;
std::map<uint8, short> g_cirlces;
std::map<uint8, double> g_lastAngles;
std::map<int, std::chrono::system_clock::time_point> g_lastSentTime;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.OutputCount = 8;
	devInfo.AxisCount = 8;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\ENCODER-BH38M\\config.ini";
	const char* app = "/Settings";
	DWORD _baudRate = GetPrivateProfileIntA(app, BuildDeviceAttribute("BaudRate",deviceIndex).data(), 19200, config_file_path.data());
	
	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex),_baudRate);
		g_serialPorts.insert(std::pair<uint8, std::shared_ptr<Serial>>(deviceIndex, _serialPort));	

		int _sResolution = GetPrivateProfileIntA(app, BuildDeviceAttribute("Resolution", deviceIndex).data(), 4096, config_file_path.data());
		g_sResolutions.insert(std::pair<uint8, int>(deviceIndex, _sResolution));

		int _circle = GetPrivateProfileIntA(app, BuildDeviceAttribute("Circle", deviceIndex).data(), 24, config_file_path.data());
		g_cirlces.insert(std::pair<uint8, int>(deviceIndex, _circle));
		g_lastAngles.insert(std::pair<uint8, int>(deviceIndex, INT_MAX));
		g_lastSentTime.insert(std::pair<uint8, std::chrono::system_clock::time_point>(deviceIndex, std::chrono::system_clock::now()));

		static char _setZeroPoint[8]{ 0x01,0x06,0x00,0x08,0x00,0x01,0xc9,0xc8 };
		_serialPort->write(_setZeroPoint, 8);
	}
	catch (const char* _err)
	{
		return 0;
	}
	
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	g_serialPorts[deviceIndex]->flush();
	g_serialPorts.erase(deviceIndex);
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
	OutDOStatus[0] = 0;
    return 1;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	auto _serialPort = g_serialPorts[deviceIndex];

	static char _data[MAX_PATH];
	DWORD _count = 0;

	auto _now = std::chrono::system_clock::now();
	std::chrono::duration<double,std::milli> _elapsed = (_now - g_lastSentTime[deviceIndex]);
	if (_elapsed.count() >= 100) {
		static char _readCircle[8]{ 0x01,0x03,0x00,0x03,0x00,0x02,0x34,0x0B };		// 开始编码器值
		_serialPort->write(_readCircle, 8);
		g_lastSentTime[deviceIndex] = _now;
	}

	auto _recevCount = _serialPort->read(_data, MAX_PATH, false);
	std::vector<uint8> _recvDatas(std::begin(_data), std::begin(_data) + _recevCount);

	static std::vector<uint8> _stashDatas;
	if (_recevCount > 0) {
		_stashDatas.insert(_stashDatas.end(), _recvDatas.begin(), _recvDatas.end());
	}
	
	while (_stashDatas.size() >= 9) {
		if (_stashDatas[0] != 0x01 || _stashDatas[1] != 0x03) {
			_stashDatas.erase(_stashDatas.begin());
			continue;
		}
		
		int _raw = _stashDatas[3] << 8 | _stashDatas[4] | _stashDatas[5]<<24 | _stashDatas[6]<<16;

		double _circleNum = _raw / 4096.0f;		// 圈数

		if (g_lastAngles[deviceIndex] == INT_MAX) {
			g_lastAngles[deviceIndex] = _circleNum;
		}
		
		double _circleCount;
		int _currentAngle = std::modf(_circleNum,&_circleCount) * 360;

		OutADStatus[0] = _circleCount;	// 整数圈数
		OutADStatus[1] = _currentAngle;	// 当前角度

		auto _lastVal = g_lastAngles[deviceIndex];

		int _delta = (_circleNum - _lastVal) * 360;
		if (abs(_delta) > 150) {
			_delta = 360 + _delta * (_delta > 0 ? -1 : 1);
		}
		OutADStatus[2] = _delta;

		OutputDebugStringA(std::to_string(_raw).data());
		OutputDebugStringA("\r\n");

		g_lastAngles[deviceIndex] = _circleNum;
		_stashDatas.erase(_stashDatas.begin(), _stashDatas.begin() + 9);
		break;
	}
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
