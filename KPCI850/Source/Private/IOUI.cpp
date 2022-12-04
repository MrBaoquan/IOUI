/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <algorithm>
#include "IOUI.h"
#include "kpci800.h"
#include "Paths.hpp"
#include <windows.h>

#pragma comment(lib,"kpci800.lib")

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 48;
	devInfo.OutputCount = 16;
	devInfo.AxisCount = 0;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\kpci800.dll";
	auto _module = LoadLibraryA(_path.data());
    int _err = GetLastError();
    return OpenDevice(ULONG(deviceIndex))?1:0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    CloseDevice(ULONG(deviceIndex));
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    for (int _addr = 6; _addr <= 7; ++_addr) {
        int _group = _addr - 6;
        unsigned char _val = 0U;
        _val = (InDOStatus[_group * 8 + 0] > 0 ? 0 : 1) << 0 
            & (InDOStatus[_group * 8 + 1] > 0 ? 0 : 1) << 1
        & (InDOStatus[_group * 8 + 2] > 0 ? 0 : 1) << 2
        & (InDOStatus[_group * 8 + 3] > 0 ? 0 : 1) << 3
        & (InDOStatus[_group * 8 + 4] > 0 ? 0 : 1) << 4
        & (InDOStatus[_group * 8 + 5] > 0 ? 0 : 1) << 5
        & (InDOStatus[_group * 8 + 6] > 0 ? 0 : 1) << 6
        & (InDOStatus[_group * 8 + 7] > 0 ? 0 : 1) << 7;
        WriteByte(deviceIndex, _addr, _val);
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    for (int _addr = 0; _addr <= 5; ++_addr) {
        unsigned char _val = ReadByte(deviceIndex, _addr);
        OutDIStatus[_addr * 8 + 0] = _val >> 0 & 1U ^ 1U;        // 高电平为0  低电平为1
        OutDIStatus[_addr * 8 + 1] = _val >> 1 & 1U ^ 1U;
		OutDIStatus[_addr * 8 + 2] = _val >> 2 & 1U ^ 1U;
		OutDIStatus[_addr * 8 + 3] = _val >> 3 & 1U ^ 1U;
		OutDIStatus[_addr * 8 + 4] = _val >> 4 & 1U ^ 1U;
		OutDIStatus[_addr * 8 + 5] = _val >> 5 & 1U ^ 1U;
		OutDIStatus[_addr * 8 + 6] = _val >> 6 & 1U ^ 1U;
		OutDIStatus[_addr * 8 + 7] = _val >> 7 & 1U ^ 1U;
    }
     
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}