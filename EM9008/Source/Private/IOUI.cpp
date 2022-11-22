/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include "windows.h"
#include "Paths.hpp"
#include "EM9008.h"
#include "PCIManager.hpp"

#pragma comment(lib,"EM9008.lib")

namespace dh = DevelopHelper;

/** 设备基本信息 */
DeviceInfo g_DeviceInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	g_DeviceInfo.InputCount = 8;
	g_DeviceInfo.OutputCount = 8;
	g_DeviceInfo.AxisCount = 0;

	return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\EM9008\\config.ini";

	// DLL 延迟加载
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\EM9008.dll";
	auto _module = LoadLibraryA(_path.data());

	HANDLE handle = EM9008_DeviceCreate();
    if (handle != INVALID_HANDLE_VALUE)
    {
        PCIManager::Instance().AddDevice(deviceIndex, DeviceData(handle,g_DeviceInfo));
        char ip[MAX_PATH] = "";
        int _cmdPort,_dataPort;
        const char* app_name = "/PCISettings";
        GetPrivateProfileStringA(app_name, "IP", "192.168.1.126", ip, MAX_PATH, config_file_path.data());
        _cmdPort = GetPrivateProfileIntA(app_name, "CMDPort", 8000, config_file_path.data());
        _dataPort = GetPrivateProfileIntA(app_name, "DataPort", 8001, config_file_path.data());

        auto _retCode = EM9008_CmdConnect(handle, ip,0, _cmdPort,_dataPort);
        if (_retCode < 0) return 0;
        for (byte _idx = 0; _idx < 8;++_idx) {
            EM9008_DiSetMode(handle, _idx, 0);
            EM9008_DoSetMode(handle, _idx, 0);
        }

        return _retCode == 0 ? 1 : 0;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	EM9008_DeviceClose(hHandle);
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    char all_status[8];
    
    for (size_t chIndex = 0; chIndex < 8; chIndex++)
    { 
        all_status[chIndex] = static_cast<char>(InDOStatus[chIndex]);
    }
    if(EM9008_DoWriteAllOnce(hHandle, all_status)==0)
    {
        return 1;
    }
    return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    char all_status[8];
    if (EM9008_IoReadAllOnce(hHandle, 0,all_status) == 0)
    {
        for (size_t chIndex = 0;chIndex < 8;++chIndex)
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
	char all_status[8];
	if(EM9008_IoReadAllOnce(hHandle, all_status,0)==0)
    {
        for (int chIndex = 0;chIndex < 8;chIndex++)
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


IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
