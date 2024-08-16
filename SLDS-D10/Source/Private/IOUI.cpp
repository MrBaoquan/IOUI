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

namespace dh = DevelopHelper;

DeviceInfo devInfo;

// ����ʵ��ָ��
std::map<uint8, std::shared_ptr<Serial>> g_serialPorts;
// ��Ȧ�ֱ���
std::map<uint8, int> g_sResolutions;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 8;
    return &devInfo;
}
std::map<int, std::chrono::system_clock::time_point> g_lastSentTime;
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\SLDS-D10\\config.ini";
	const char* app = "/Settings";
	DWORD _baudRate = GetPrivateProfileIntA(app, BuildDeviceAttribute("BaudRate",deviceIndex).data(), 19200, config_file_path.data());
	g_lastSentTime.insert(std::pair<uint8, std::chrono::system_clock::time_point>(deviceIndex, std::chrono::system_clock::now()));
	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex),_baudRate, TWOSTOPBITS);
		// 上电自启
		const char _powerOn[8] = { 0x01,0x06,0x01,0x8C,0x00,0x01,0x88,0x1D };
		_serialPort->write(_powerOn, sizeof(_powerOn));
		// 连续测量
		const char _continueMesure[8]{ 0x01,0x06,0x01,0x90,0x00,0x02,0x09,0xDA };
		_serialPort->write(_continueMesure, sizeof(_continueMesure));

		const char _refreshInterval[13]{ 0x01,0x10,0x01,0x70 ,0x00 ,0x02 ,0x04 ,0x00 ,0x00 ,0x00 ,0x32 ,0x78, 0xCE };
		_serialPort->write(_refreshInterval, sizeof(_refreshInterval));
		
		g_serialPorts.insert(std::pair<uint8, std::shared_ptr<Serial>>(deviceIndex, _serialPort));	
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
	static const char _readData[8]{0x01,0x03,0x00,0x94,0x00,0x02,0x85,0xE7};
	auto _serialPort = g_serialPorts[deviceIndex];

	auto _now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> _elapsed = (_now - g_lastSentTime[deviceIndex]);
	if (_elapsed.count() >= 50) {
		_serialPort->write(_readData, sizeof(_readData));
	}

	static char _data[MAX_PATH];
	DWORD _count = 0;

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

		unsigned int _raw =(_stashDatas[3]<<24)|(_stashDatas[4]<<16)|(_stashDatas[5]<<8)|(_stashDatas[6]);
		OutADStatus[0] = _raw / 10;
		_stashDatas.erase(_stashDatas.begin(), _stashDatas.begin() + 9);
		break;
	}
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
