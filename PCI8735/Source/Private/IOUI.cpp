/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include "PCIManager.hpp"
#include "PCI8735.h"

#ifdef WIN_64
#pragma comment(lib,"PCI8735_64.lib")
#else
#pragma comment(lib,"PCI8735_32.lib")
#endif

DeviceInfo devInfo;
const uint8 AxisCount = 32;
long nRetSizeWords;
long nReadSizeWords;

IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 0;
    devInfo.OutputCount = 0;
    devInfo.AxisCount = AxisCount;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    HANDLE hHandle = PCI8735_CreateDevice(deviceIndex);
    if (hHandle != INVALID_HANDLE_VALUE)
    {
        PCIManager::Instance().AddHandle(deviceIndex, hHandle);
        PCI8735_PARA_AD param=
        {
            0,
            31,
            PCI8735_INPUT_N10000_P10000mV,
            PCI8735_GNDMODE_SE,
            PCI8735_GAINS_1MULT
        };
        nReadSizeWords = 512 - 512 % AxisCount;
        return PCI8735_InitDeviceAD(hHandle, &param) ? 1 : 0;
    }
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
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    unsigned short realADStatus[AxisCount] = {};
    if(PCI8735_ReadDeviceAD(hHandle, realADStatus, nReadSizeWords, &nRetSizeWords))
    {
        for (uint8 index = 0;index < AxisCount;++index)
        {
            OutADStatus[index] = static_cast<short>(realADStatus[index]) / 10;
        }
        return 1;
    }
    return 0;
}
