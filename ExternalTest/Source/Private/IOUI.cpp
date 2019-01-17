/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"

DeviceInfo devInfo;
const uint8 IOCount = 32;
BYTE InStatus[IOCount];
BYTE OutStatus[IOCount];

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = IOCount;
    devInfo.OutputCount = IOCount;
    devInfo.AxisCount = IOCount;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, BYTE* InDOStatus)
{
    for (uint8 index = 0;index < IOCount;++index)
    {
        InStatus[index] = InDOStatus[index];
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, BYTE* OutDOStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    for (uint8 index = 0;index < IOCount;++index)
    {
        OutDIStatus[index] = InStatus[index];
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    static float val = 0;
    OutADStatus[0] = ++val;
    return 1;
}
