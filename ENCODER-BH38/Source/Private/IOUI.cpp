/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include "IOUI.h"
#include <intsafe.h>
#include <memory>
#include <map>
#include "Paths.hpp"
#include "Serial.hpp"
#include "Util.hpp"

namespace dh = DevelopHelper;

DeviceInfo devInfo;

// 串口实例指针
std::map<uint8, std::shared_ptr<Serial>> g_serialPorts;
// 单圈分辨率
std::map<uint8, int> g_sResolutions;

std::map<uint8, short> g_lastAngles;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 8;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\ENCODER-BH38\\config.ini";
	const char* app = "/Settings";
	DWORD _baudRate = GetPrivateProfileIntA(app, BuildDeviceAttribute("BaudRate",deviceIndex).data(), 19200, config_file_path.data());
	int _sResolution = GetPrivateProfileIntA(app, BuildDeviceAttribute("SR",deviceIndex).data(), 16384, config_file_path.data());
	g_sResolutions.insert(std::pair<uint8,int>(deviceIndex, _sResolution));
	
	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex),_baudRate);
		g_serialPorts.insert(std::pair<uint8, std::shared_ptr<Serial>>(deviceIndex, _serialPort));	
		g_lastAngles.insert(std::pair<uint8, short>(deviceIndex, SHORT_MAX));
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
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	auto _serialPort = g_serialPorts[deviceIndex];
	int _sResolution = g_sResolutions[deviceIndex];

	static char _data[MAX_PATH];
	DWORD _count = 0;
	auto _recevCount = _serialPort->read(_data, MAX_PATH, false);
	std::vector<uint8> _recvDatas(std::begin(_data), std::begin(_data) + _recevCount);

	while (_recvDatas.size() >= 10) {
		if (_recvDatas[0] != 0xAB || _recvDatas[1] != 0xCD) {
			_recvDatas.erase(_recvDatas.begin());
			continue;
		}

		int _raw = _recvDatas[3] << 8 | _recvDatas[4];
		int _angle = _raw * 360 / _sResolution;

		short& _lastAngle = g_lastAngles[deviceIndex];
		
		if (_lastAngle == SHORT_MAX) {
			_lastAngle = _angle;
		}
		
		int _deltaAngel = _angle - _lastAngle;
		if (abs(_deltaAngel) >= 180) {
			_deltaAngel = 360 * (_deltaAngel > 0 ? -1 : 1) + _deltaAngel;
		}
		OutADStatus[0] = _angle;
		OutADStatus[1] = _deltaAngel;
		_lastAngle = _angle;
		
		break;
		/*_recvDatas.erase(std::begin(_recvDatas), std::begin(_recvDatas) + 10);*/
	}
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
