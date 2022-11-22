/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-30 9:17
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <bitset>
#include "Paths.hpp"
#include "PCI8407.H"

#pragma comment(lib,"pci8407.lib")


 /** 设备基本信息 */
DeviceInfo g_DeviceInfo;

const uint8 ChannelCount = 32;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_DeviceInfo.InputCount = 0;
    g_DeviceInfo.OutputCount = ChannelCount;
    g_DeviceInfo.AxisCount = 0;

    return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    // DLL 延迟加载
#ifdef WIN_64
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\pci8407.dll";
#else
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\pci8407.dll";
#endif

    auto _module = LoadLibraryA(_path.data());

    return ZT8407_OpenDevice(static_cast<uint8>(deviceIndex)) == 0 ? 1 : 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return ZT8407_CloseDevice(static_cast<uint8>(deviceIndex)) == 0 ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    std::bitset<ChannelCount> DOStatus;
    for (uint8 index = 0;index < ChannelCount;index++)
    {
        DOStatus[index] = InDOStatus[index] >= 1 ? true : false;
    }

    return ZT8407_DOAll(deviceIndex, DOStatus.to_ulong()) == 0 ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    long doStatus = ZT8407_GetLastDO(deviceIndex);
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
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
