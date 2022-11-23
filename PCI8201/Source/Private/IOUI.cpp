/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <string>
#include "PCIManager.hpp"
#include "PCI8201.h"

#ifdef WIN_64
#pragma comment(lib,"PCI8201_64.lib")
#else
#pragma comment(lib,"PCI8201_32.lib")
#endif

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.AxisCount = 0;
	devInfo.OutputCount = 8;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	HANDLE _handle = PCI8201_CreateDevice(deviceIndex);
	if (_handle == INVALID_HANDLE_VALUE) {
		return 0;
	}
	PCIManager::Instance().AddDevice(deviceIndex, DeviceData(_handle,devInfo));
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	HANDLE _handle = PCIManager::Instance().GetHandle(deviceIndex);
	return PCI8201_ReleaseDevice(_handle) ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	HANDLE _handle = PCIManager::Instance().GetHandle(deviceIndex);
	if (_handle == INVALID_HANDLE_VALUE) { return 0; }
	for (int _index = 0; _index < devInfo.OutputCount; ++_index)
	{
		PCI8201_WriteDeviceDA(_handle, PCI8201_OUTPUT_0_P5000mV, InDOStatus[_index], _index);
	}
	
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
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
