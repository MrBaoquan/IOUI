/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return 0;
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
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
