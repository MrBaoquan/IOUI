/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <windows.h>
#include "PCIManager.hpp"
#include "PCI2394.h"

#ifdef WIN_64
#pragma comment(lib,"PCI2394_64.lib")
#else
#pragma comment(lib,"PCI2394.lib")
#endif

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = MAX_CHANNEL_COUNT;
	devInfo.OutputCount = MAX_CHANNEL_COUNT;
	devInfo.AxisCount = MAX_CHANNEL_COUNT;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	HANDLE hHandle = PCI2394_CreateDevice(deviceIndex);

	if (hHandle != INVALID_HANDLE_VALUE)
	{
		PCI2394_PARA_CNT CNTPara[MAX_CHANNEL_COUNT];
		for (int _index = 0; _index < 4; _index++)
		{

			CNTPara[_index].lCNTMode = PCI2394_CNTMODE_1_PULSE;
			CNTPara[_index].lResetMode = PCI2394_RESETMODE_ZERO; // 计数器复位到0x00000000
			CNTPara[_index].bOverflowLock = TRUE; // 上溢锁定
			CNTPara[_index].bUnderflowLock = TRUE; // 下溢锁定
			CNTPara[_index].bDigitFilter = TRUE; // 差分输入是否进行数字滤波(DF) TRUE=过滤
			CNTPara[_index].lLatchMode = PCI2394_LATCHMODE_SOFT; // 软件锁存计数器数据 SoftWare

			PCI2394_InitDeviceCNT(hHandle, &CNTPara[_index], _index);
		}

		PCIManager::Instance().AddHandle(deviceIndex, hHandle);
		return 1;
	}
	return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	return PCI2394_ReleaseDevice(hHandle) ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, BYTE* InDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, BYTE* OutDOStatus)
{
	OutDOStatus[0] = 1;
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	ZeroMemory(OutDIStatus, devInfo.InputCount);
	return PCI2394_GetDeviceDI(hHandle, OutDIStatus) ? 1 : 0;
}

unsigned long lastADStatus[MAX_CHANNEL_COUNT];
IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	unsigned long realADStatus[MAX_CHANNEL_COUNT] = {};

	for (int _index = 0; _index < devInfo.AxisCount; _index++)
	{
		if (!PCI2394_GetDeviceCNT(hHandle, &realADStatus[_index], _index)) {
			continue;
		}
	}
	
	for (uint8 _index = 0; _index < devInfo.AxisCount; ++_index)
	{
		OutADStatus[_index] = static_cast<short>(realADStatus[_index] - lastADStatus[_index]);
		lastADStatus[_index] = realADStatus[_index];
	}
	return 1;
}
