/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include "windows.h"
#include "Paths.hpp"
#include "EM9703.h"
#include "PCIManager.hpp"

#pragma comment(lib,"EM9703.lib")

namespace dh = DevelopHelper;

/** 设备基本信息 */
DeviceInfo g_DeviceInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	g_DeviceInfo.InputCount = 32;
	g_DeviceInfo.OutputCount = 32;
	g_DeviceInfo.AxisCount = 0;

	return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "EM9703\\config.ini";
	HANDLE handle = EM9703_CreateDevice();
    if (handle != INVALID_HANDLE_VALUE)
    {
        PCIManager::Instance().AddDevice(deviceIndex, DeviceData(handle,g_DeviceInfo));
        char ip[MAX_PATH] = "";
        int port;
        char* app_name = "/PCISettings";
        GetPrivateProfileStringA(app_name, "IP", "127.0.0.1", ip, MAX_PATH, config_file_path.data());
        port = GetPrivateProfileIntA(app_name, "Port", 0, config_file_path.data());
        return EM9703_CmdConnect(handle, ip, port) == 0? 1 : 0;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	EM9703_CloseDevice(hHandle);
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    char all_status[64];
    if (EM9703_IoGetAll(hHandle, all_status)==0)
    {
        for (size_t chIndex = 32; chIndex < 64; chIndex++)
        {
            all_status[chIndex] = static_cast<char>(InDOStatus[chIndex - 32]);
        }
        if(EM9703_IoSetAll(hHandle, all_status)==0)
        {
            return 1;
        }
    }
    
    return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    char all_status[64];
    if (EM9703_IoGetAll(hHandle, all_status) == 0)
    {
        for (size_t chIndex = 32;chIndex < 64;++chIndex)
        {
            OutDOStatus[chIndex] = all_status[chIndex];
        }
        return 1;
    }
    
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	char all_status[64];
	if(EM9703_IoGetAll(hHandle, all_status)==0)
    {
        for (int chIndex = 0;chIndex < 32;chIndex++)
        {
            OutDIStatus[chIndex] = all_status[chIndex];
        }
        return 1;
    }
	
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
