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

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.OutputCount = 253;
	devInfo.AxisCount = 0;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{	
	try
	{
		auto _serialPort = new Serial("COM" + std::to_string(deviceIndex), 9600);
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
	static std::vector<short> _lastDOStatus(devInfo.OutputCount, 0);
	std::map<int, short> _dirtyDOStatus;
	for (auto _idx = 0; _idx < _lastDOStatus.size(); ++_idx) {
		if (_lastDOStatus[_idx] != InDOStatus[_idx]) {
			_dirtyDOStatus.insert(std::pair<int, short>(_idx, InDOStatus[_idx]>0?1:0));
			_lastDOStatus[_idx] = InDOStatus[_idx];
		}
	}
	static char _data[MAX_PATH] = { };
	for  (auto _doItem : _dirtyDOStatus)
	{
		auto _idx = std::distance(_dirtyDOStatus.begin(), _dirtyDOStatus.find(_doItem.first));
		_data[_idx * 4] = 0xA0;
		_data[_idx * 4 + 1] = _doItem.first + 1;
		_data[_idx * 4 + 2] = _doItem.second;
		_data[_idx * 4 + 3] = (0xA0 + _doItem.first + 1 + _doItem.second) % 0x100;
	}
	return g_serialPorts[deviceIndex]->write(_data, 4 * _dirtyDOStatus.size()) == (4 * _dirtyDOStatus.size());
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
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
