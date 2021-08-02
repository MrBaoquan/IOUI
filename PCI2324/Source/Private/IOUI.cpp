/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "PCI2324.h"

#ifdef WIN_64
#pragma comment(lib,"PCI2324_64.lib")
#else
#pragma comment(lib,"PCI2324_32.lib")
#endif

/** 设备基本信息 */
DeviceInfo g_DeviceInfo; 

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_DeviceInfo.InputCount = 2;
    g_DeviceInfo.OutputCount = 32;
    g_DeviceInfo.AxisCount = 0;

    return &g_DeviceInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{

    // DLL 延迟加载
#ifdef WIN_64
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\PCI2324_64.dll";
#else
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\PCI2324_32.dll";
#endif
    
    auto _module = LoadLibraryA(_path.data());

    HANDLE hHandle = PCI2324_CreateDevice(deviceIndex);
    if (hHandle!=INVALID_HANDLE_VALUE)
    {
		PCIManager::Instance().AddDevice(deviceIndex, DeviceData(hHandle, g_DeviceInfo));
        return 1;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    return PCI2324_ReleaseDevice(hHandle) ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	const DeviceData* _devData = PCIManager::Instance().GetDeviceData(deviceIndex);
	if (!_devData) { return 0; }
	CopyTo(InDOStatus, _devData->DOStatus_Byte(),g_DeviceInfo.OutputCount);
    return PCI2324_SetDeviceDO(_devData->handle, _devData->DOStatus_Byte()) ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{	
	/*const DeviceData* _devData = PCIManager::Instance().GetDeviceData(deviceIndex);
	if (!_devData) { return 0; }
    int _retCode = PCI2324_GetDeviceDO(_devData->handle, _devData->DOStatus()) ? 1 : 0;
	CopyTo(_devData->DOStatus(), OutDOStatus, g_DeviceInfo.OutputCount);*/
	return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	ZeroMemory(OutDIStatus, g_DeviceInfo.InputCount);
    int _retCode = PCI2324_GetDeviceDI(hHandle, OutDIStatus) ? 1 : 0;
	return _retCode;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
