/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Serial.hpp"
namespace dh = DevelopHelper;

DeviceInfo devInfo;
Serial* g_serialPort = nullptr;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 128;
	devInfo.OutputCount = 128;
	devInfo.AxisCount = 128;
    return &devInfo;
}

//int channelIndex = 1;
//int valueIndex = 2;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\COMDEV\\config.ini";
	const char* app = "/PCISettings";
	DWORD _baudRate = GetPrivateProfileIntA(app, "BaudRate", 115200, config_file_path.data());

	/*channelIndex = GetPrivateProfileIntA(app, "channelIndex", 1, config_file_path.data());
	valueIndex = GetPrivateProfileIntA(app, "valueIndex", 2, config_file_path.data());*/
	try
	{
		g_serialPort = new Serial("COM" + std::to_string(deviceIndex));
	}
	catch (const char* _err)
	{
		return 0;
	}
	
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	g_serialPort->flush();
	delete g_serialPort;
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{	
	static std::vector<short> _lastDOStatus(devInfo.OutputCount,0);
	std::map<int,short> _dirtyDOStatus;
	for (auto _idx = 0; _idx < _lastDOStatus.size();++_idx) {
		if (_lastDOStatus[_idx] != InDOStatus[_idx]) {
			_dirtyDOStatus.insert(std::pair<int,short> (_idx, InDOStatus[_idx]));
			_lastDOStatus[_idx] = InDOStatus[_idx];
		}
	}


	static char _data[MAX_PATH] = { };
	for each (auto _doItem in _dirtyDOStatus)
	{
		auto _idx = std::distance(_dirtyDOStatus.begin(), _dirtyDOStatus.find(_doItem.first));
		_data[_idx * 4] = 0xFE;
		_data[_idx * 4 + 1] = _doItem.first;
		_data[_idx * 4 + 2] = _doItem.second;
		_data[_idx * 4 + 3] = 0xFF;
	}
	return g_serialPort->write(_data, 4 * _dirtyDOStatus.size())==(4 * _dirtyDOStatus.size());
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	static char _data[MAX_PATH];
	static std::vector<uint8> _recvDatas;
	DWORD _count=0;
	auto _recevCount = g_serialPort->read(_data, MAX_PATH, false);
	for (int _idx = 0;_idx < _recevCount;++_idx) {
		_recvDatas.push_back(_data[_idx]);
	}
	while (_recvDatas.size()>=4) {
		if (_recvDatas[0] != 0xFE) {
			_recvDatas.erase(_recvDatas.begin());
			continue;
		}
		std::vector<uint8> _content(_recvDatas.begin(), _recvDatas.begin() + 4);
		uint8 _channel = _content[1];
		uint8 _status = _content[2];
		if (_status == 0x00) {
			OutDIStatus[_channel] = 0;
		}
		else {
			OutDIStatus[_channel] = 1;
		}
		_recvDatas.erase(_recvDatas.begin(), _recvDatas.begin() + 4);
	}
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
