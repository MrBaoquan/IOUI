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
#include "Comm.h"
namespace dh = DevelopHelper;

DeviceInfo devInfo;
Comm g_Comm;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 16;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 0;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	g_Comm.Init(deviceIndex,115200);
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
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
	char _data[MAX_PATH];
	DWORD _count=0;
	g_Comm.ReadCom(_data, MAX_PATH, &_count);
	if (_count != 0) {
		uint8 _channel = _data[4];
		uint8 _status = _data[5];
		if (_status == 0x01) {
			OutDIStatus[_channel] = 1;
		}
		else {
			OutDIStatus[_channel] = 0;
		}
	}
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	
    return 1;
}
