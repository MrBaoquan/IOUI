/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-30 9:17
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <bitset>
#include "PCI8408.h"

#pragma comment(lib,"pci8408.lib")


 /** 设备基本信息 */
DeviceInfo g_DeviceInfo;

const uint8 ChannelCount = 16;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_DeviceInfo.InputCount = ChannelCount;
    g_DeviceInfo.OutputCount = ChannelCount;
    g_DeviceInfo.AxisCount = 0;

    return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    return ZT8408_OpenDevice(static_cast<uint8>(deviceIndex)) == 0 ? 1 : 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return ZT8408_CloseDevice(static_cast<uint8>(deviceIndex)) == 0 ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    std::bitset<ChannelCount> DOStatus;
    for (uint8 index = 0;index < ChannelCount;index++)
    {
        DOStatus[index] = InDOStatus[index] == 1 ? true : false;
    }

    return ZT8408_DOAll(deviceIndex, DOStatus.to_ulong()) == 0 ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    long doStatus = ZT8408_GetLastDO(deviceIndex);
    if (doStatus == -1) { return 0; }
    std::bitset<ChannelCount> FinalStatus(doStatus);
    for (uint8 index = 0;index < ChannelCount;index++)
    {
        OutDOStatus[index] = FinalStatus[index];
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    long diStatus = ZT8408_DIAll(deviceIndex);
    if (diStatus == -1) { return 0; }
    std::bitset<ChannelCount> BIStatus(diStatus);
    for (uint8 index = 0;index < ChannelCount;index++)
    {
        OutDIStatus[index] = BIStatus[index];
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
