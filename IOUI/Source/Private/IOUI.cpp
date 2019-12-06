/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include <stdlib.h>
#include "IOUI.h"
#include "PCIManager.hpp"
DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 32;
	devInfo.OutputCount = 32;
	devInfo.AxisCount = 32;
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

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	OutputDebugStringA(ToString(InDOStatus,devInfo.OutputCount).data());
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}
float timer = 1.0f;
short current = 0;
int num = 0;
IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	timer += 0.05f;
	if (timer >= 5) {
		num++;
		timer = 0;
	}

	if (num % 2 == 0) {
		float _random = RandomFloat(-200.f, 200.f);
		current = _random;
		OutADStatus[0] = current;
	}
	else {
		float _random = RandomFloat(5500.f, 10500.f);
		current = _random;
		OutADStatus[0] = current;
	}
	
    return 1;
}
