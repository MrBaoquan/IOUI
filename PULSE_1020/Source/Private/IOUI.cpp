/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Card1020.h"

#ifdef WIN_64
#pragma comment(lib,"PCI1020_64.lib")
#else
#pragma comment(lib,"PCI1020_32.lib")
#endif

/** �豸������Ϣ */
DeviceInfo g_DeviceInfo; 
CCard1020* _pci2010 = new CCard1020();

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_DeviceInfo.InputCount = 32 + 4;
    g_DeviceInfo.OutputCount = 32 + 32 + 4;  // 32�����������ͨ�� + 4���������ͨ��
    g_DeviceInfo.AxisCount = 0;

    return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    _pci2010->InitCard();
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    _pci2010->ExitCard();
    delete _pci2010;
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    auto _activeAxis = InDOStatus[0]; // 标识
    auto _channel = InDOStatus[1];  // 通道
    auto _pulse = InDOStatus[2];    // 脉冲
    auto _scaler = InDOStatus[3];   // 脉冲值缩放系数
    _pulse *= _scaler;
    auto _speed = InDOStatus[4];    // 电机速度
    if (_activeAxis>0) {
        _pci2010->ActiveAxis(_channel, _pulse, _speed);
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    OutDOStatus[0] = 0;
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
