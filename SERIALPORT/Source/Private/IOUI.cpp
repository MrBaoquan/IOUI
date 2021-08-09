/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Comm.h"
namespace dh = DevelopHelper;

DeviceInfo devInfo;
Comm g_Comm;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 255;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 0;
    return &devInfo;
}

int channelIndex = 0;
int valueIndex = 1;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\COMDEV\\config.ini";
	const char* app = "/PCISettings";
	DWORD _baudRate = GetPrivateProfileIntA(app, "BaudRate", 9600, config_file_path.data());

	channelIndex = GetPrivateProfileIntA(app, "channelIndex", 1, config_file_path.data());
	valueIndex = GetPrivateProfileIntA(app, "valueIndex", 2, config_file_path.data());

	g_Comm.Init(deviceIndex,_baudRate);
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{	
    return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	static char _data[MAX_PATH];
	DWORD _count=0;
	g_Comm.ReadCom(_data, MAX_PATH, &_count);
	if (_count != 0) {
		uint8 _channel = _data[channelIndex];
		uint8 _status = _data[valueIndex];
		if (_status == 0x00) {
			OutDIStatus[_channel] = 0;
		}
		else {
			OutDIStatus[_channel] = 1;
		}
	}
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
