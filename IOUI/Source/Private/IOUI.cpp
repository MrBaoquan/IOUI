/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include "IOUI.h"
#include "PCIManager.hpp"
#include "TISocket.hpp"
#include "Paths.hpp"
namespace dh = DevelopHelper;

DeviceInfo devInfo;

TISocket sockets[5];
IOUI_API DeviceInfo* __stdcall Initialize()
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\PCI8409\\config.ini";

	char* app_name = "/PCISettings";
	char ip[MAX_PATH] = "";
	GetPrivateProfileStringA(app_name, "input_1_", "127.0.0.1", ip, MAX_PATH, config_file_path.data());


	OutputDebugStringA(config_file_path.c_str());
	devInfo.InputCount = 32;
	devInfo.OutputCount = 32;
	devInfo.AxisCount = 32;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	for (int i = 10000; i < 10020; i++)
	{
		if (sockets[deviceIndex].Initialize(TIType::UDP, i) >= 0) {
			return 1;
		};
	}
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	sockets[deviceIndex].Exit();
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	sockets[deviceIndex].SetRemoteAddr(std::string("127.0.0.1"), 7001);
	std::string _message = ToString(InDOStatus, devInfo.OutputCount);
	sockets[deviceIndex].Send(_message.data(), _message.length());
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
