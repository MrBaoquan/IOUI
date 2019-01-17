/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <windows.h>
#include "PCIManager.hpp"
#include "PCI2312A.h"

#ifdef WIN_64
#pragma comment(lib,"PCI2312A_64.lib")
#else
#pragma comment(lib,"PCI2312A_32.lib")
#endif

/** 设备基本信息 */
DeviceInfo g_DeviceInfo; 

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_DeviceInfo.InputCount = 16;
    g_DeviceInfo.OutputCount = 16;
    g_DeviceInfo.AxisCount = 0;

    return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    HANDLE hHandle = PCI2312A_CreateDevice(deviceIndex);
    if (hHandle!=INVALID_HANDLE_VALUE)
    {
        PCIManager::Instance().AddHandle(deviceIndex, hHandle);
        return 1;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    return PCI2312A_ReleaseDevice(hHandle) ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, BYTE* InDOStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    return PCI2312A_SetDeviceDO(hHandle, InDOStatus) ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, BYTE* OutDOStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    return PCI2312A_GetDeviceDO(hHandle, OutDOStatus) ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    return PCI2312A_GetDeviceDI(hHandle, OutDIStatus) ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
