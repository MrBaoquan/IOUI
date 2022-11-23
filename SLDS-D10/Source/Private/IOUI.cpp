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

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\SLDS-D10\\config.ini";
	const char* app = "/Settings";
	DWORD _baudRate = GetPrivateProfileIntA(app, BuildDeviceAttribute("BaudRate",deviceIndex).data(), 19200, config_file_path.data());
	
	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex),_baudRate, TWOSTOPBITS);
		// �ϵ����ģʽ
		const char _powerOn[8] = { 0x01,0x06,0x01,0x8C,0x00,0x01,0x88,0x1D };
		_serialPort->write(_powerOn, sizeof(_powerOn));
		// ��������ģʽ
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
	int _sResolution = g_sResolutions[deviceIndex];

	_serialPort->write(_readData, sizeof(_readData));

	static char _data[MAX_PATH];
	DWORD _count = 0;
	auto _recevCount = _serialPort->read(_data, MAX_PATH, false);
	std::vector<uint8> _recvDatas(std::begin(_data), std::begin(_data) + _recevCount);

	while (_recvDatas.size() >= 9) {
		if (_recvDatas[0] != 0x01 || _recvDatas[1] != 0x03) {
			_recvDatas.erase(_recvDatas.begin());
			continue;
		}
		/*	_recvDatas[3] = 0x00;
			_recvDatas[4] = 0x00;
			_recvDatas[5] = 0x0b;
			_recvDatas[6] = 0x10;*/
		unsigned int _raw =(_recvDatas[3]<<24)|(_recvDatas[4]<<16)|(_recvDatas[5]<<8)|(_recvDatas[6]);
		OutADStatus[0] = _raw;
		/*OutputDebugStringA(std::to_string(_raw).data());
		OutputDebugStringA("\r\n");*/
		break;
		/*_recvDatas.erase(std::begin(_recvDatas), std::begin(_recvDatas) + 10);*/
	}
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
