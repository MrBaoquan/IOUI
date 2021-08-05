/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#pragma once
#include <stdlib.h>
#include "IOUI.h"
#include "ChairDevManager.h"

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 32;
	devInfo.OutputCount = 32;
	devInfo.AxisCount = 0;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	int _retCode = ChairDevManager::Instance().InitDevice();
	ChairDevManager::Instance().Reset();
	return _retCode;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	ChairDevManager::Instance().Reset();
	ChairDevManager::Instance().Exit();
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	ChairDevManager::Instance().Speed(static_cast<unsigned char>(InDOStatus[0]));
	ChairDevManager::Instance().Pitch(InDOStatus[1]);
	ChairDevManager::Instance().Roll(InDOStatus[3]);
	ChairDevManager::Instance().DoAction();
    return 1;
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
