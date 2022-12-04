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
//std::map<uint8, int> g_sResolutions;

//std::map<uint8, short> g_lastAngles;

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
	std::string config_file_path = path + "Config\\ENCODER-BRT38\\config.ini";
	const char* app = "/Settings";
	DWORD _baudRate = GetPrivateProfileIntA(app, BuildDeviceAttribute("BaudRate",deviceIndex).data(), 9600, config_file_path.data());
	//int _sResolution = GetPrivateProfileIntA(app, BuildDeviceAttribute("SR",deviceIndex).data(), 1024, config_file_path.data());
	//g_sResolutions.insert(std::pair<uint8,int>(deviceIndex, _sResolution));
	
	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex),_baudRate);
		g_serialPorts.insert(std::pair<uint8, std::shared_ptr<Serial>>(deviceIndex, _serialPort));	
		//g_lastAngles.insert(std::pair<uint8, short>(deviceIndex, SHORT_MAX));

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
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	auto _serialPort = g_serialPorts[deviceIndex];
	//int _sResolution = g_sResolutions[deviceIndex];

	static char _data[MAX_PATH];
	DWORD _count = 0;

	static int _flag = -1;
	if (_flag == -1) {
		static char _readCircle[8]{ 0x01,0x03,0x00,0x02,0x00,0x01,0x25,0xca };
		_serialPort->write(_readCircle, 8);
		_flag = 0;
	}

	
	auto _recevCount = _serialPort->read(_data, MAX_PATH, false);
	std::vector<uint8> _recvDatas(std::begin(_data), std::begin(_data) + _recevCount);

	static std::vector<uint8> _stashDatas;
	if (_recevCount > 0) {
		_stashDatas.insert(_stashDatas.end(), _recvDatas.begin(), _recvDatas.end());
		OutputDebugStringA(std::to_string(_recevCount).data());
		OutputDebugStringA("\r\n");
	}
	
	
	while (_stashDatas.size() >= 7) {
		if (_stashDatas[0] != 0x01 || _stashDatas[1] != 0x03) {
			_stashDatas.erase(_stashDatas.begin());
			continue;
		}
		// 疑问： 如何区分接收到的数据是圈数还是速度值
		int _raw = _stashDatas[3] << 8 | _stashDatas[4];
		if (_flag == 0) {
			OutADStatus[0] = _raw;
			static char _readCircle[8]{ 0x01,0x03,0x00,0x03,0x00,0x01,0x74,0x0a };		// 开始读速度
			_serialPort->write(_readCircle, 8);
			_flag = 1;
		}
		else if (_flag == 1) {
			OutADStatus[1] = _raw;
			static char _readCircle[8]{ 0x01,0x03,0x00,0x02,0x00,0x01,0x25,0xca }; // 开始读圈数
			_serialPort->write(_readCircle, 8);
			_flag = 0;
		}
		_stashDatas.erase(_stashDatas.begin(), _stashDatas.begin() + 7);
		break;
		/*_recvDatas.erase(std::begin(_recvDatas), std::begin(_recvDatas) + 10);*/
	}
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
