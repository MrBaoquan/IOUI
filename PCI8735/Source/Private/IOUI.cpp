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
long nRetSizeWords;
long nReadSizeWords;
const short axisCount = 32;

DeviceInfo g_DeviceInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	g_DeviceInfo.InputCount = 0;
	g_DeviceInfo.OutputCount = 0;
	g_DeviceInfo.AxisCount = 32;

	return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    HANDLE hHandle = PCI8735_CreateDevice(deviceIndex);
    if (hHandle != INVALID_HANDLE_VALUE)
    {
        PCIManager::Instance().AddDevice(deviceIndex, DeviceData(hHandle,g_DeviceInfo));
        PCI8735_PARA_AD param=
        {
            0,
            31,
			PCI8735_INPUT_N5000_P5000mV,
            PCI8735_GNDMODE_SE,
            PCI8735_GAINS_1MULT
        };
        nReadSizeWords = 512 - 512 % g_DeviceInfo.AxisCount;
        return PCI8735_InitDeviceAD(hHandle, &param) ? 1 : 0;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	HANDLE _handle = PCIManager::Instance().GetHandle(deviceIndex);
	return PCI8735_ReleaseDevice(_handle) ? 1 : 0;
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
	unsigned short realADStatus[1024 * 8];// = {};
    if(PCI8735_ReadDeviceAD(hHandle, realADStatus, nReadSizeWords, &nRetSizeWords))
    {
        for (uint8 index = 0;index < g_DeviceInfo.AxisCount;++index)
        {
			unsigned short ADData = realADStatus[index] & 0x1FFF;
			float _result = (float)((10000.00 / 8192) * ADData - 5000.00);
           OutADStatus[index] = static_cast<short>(_result);
        }
        return 1;
    }
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
