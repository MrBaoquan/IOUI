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
#include "Util.hpp"
#include "DAM3000M.h"

#ifdef WIN_64
#pragma comment(lib,"DAM3000M_64.lib")
#else
#pragma comment(lib,"DAM3000M_32.lib")
#endif

namespace dh = DevelopHelper;

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 16;
	devInfo.OutputCount = 19;
	devInfo.AxisCount = 8;
    return &devInfo;
}

int lDeviceID = 1;
int AD_lMode = 0;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	// DLL 延迟加载
#ifdef WIN_64
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\DAM3000M_64.dll";
#else
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\DAM3000M_32.dll";
#endif

	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\DAM3000M\\config.ini";
	const char* app = "/PCISettings";
	DWORD _baudRate = GetPrivateProfileIntA(app, "BaudRate", 3, config_file_path.data());

	lDeviceID = GetPrivateProfileIntA(app, "lDeviceID", 1, config_file_path.data());
	AD_lMode = GetPrivateProfileIntA(app, "AD_lMode", 0, config_file_path.data());
	auto Init_Timeout = GetPrivateProfileIntA(app, "Init_Timeout", 50, config_file_path.data());

	auto hDevice = DAM3000M_CreateDevice(deviceIndex);
	auto bRet = DAM3000M_InitDevice(hDevice, _baudRate, DAM3000M_PARITY_NONE, Init_Timeout);

	if (bRet) {
		PCIManager::Instance().AddDevice(deviceIndex, DeviceData(hDevice, devInfo));

		// 开关量输入设置
		for (int index = 0; index < 16; index++)
		{
			bRet = DAM3000M_SetModeDI(hDevice, lDeviceID, 0, 0, index);
		}

		// 模拟量输入设置
		for (int lChannel = 0; lChannel < 8; lChannel++)
		{

			DAM3000M_SetModeAD(							// 设置AD输入模式 
				hDevice,					// 设备对象句柄	
				lDeviceID,					// 模块地
				AD_lMode,						// AD模式 
				lChannel);				// 通道号	

			Sleep(50);
		}
		return 1;
	}
	return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	auto hDevice = PCIManager::Instance().GetHandle(deviceIndex);
	return DAM3000M_ReleaseDevice(hDevice) ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{	
	static BYTE	 bDOSts[16];
	auto hDevice = PCIManager::Instance().GetHandle(deviceIndex);
	CopyTo(InDOStatus, bDOSts,16);
	auto _ret1 = DAM3000M_SetDeviceDO(hDevice, lDeviceID, bDOSts, 0, 15)?1:0;

	auto _lDAMode = InDOStatus[16];
	LONG _daValue = 0;
	if (_lDAMode == 0) {
		DAM3000M_SetOutPutRangeDA(hDevice, lDeviceID, DAM3000M_CUR_N0_P20, 0);
		_daValue = (LONG)(InDOStatus[17] * 0xFFF / 20.0);
	}
	else {
		DAM3000M_SetOutPutRangeDA(hDevice, lDeviceID, DAM3000M_CUR_N4_P20, 0);
		_daValue = (LONG)((InDOStatus[17] - 4.0) * 0xFFF / 16.0);
	}
	auto _ret2 = DAM3000M_WriteDeviceDA(hDevice, lDeviceID, _daValue, 0);
	auto _ret3 = DAM3000M_WriteDeviceDA(hDevice, lDeviceID, InDOStatus[18]*0xFFF/10.0, 1);
	if (!_ret1) {
		OutputDebugStringA("write failed");
	}
	if (!_ret2) {
		OutputDebugStringA("write failed");
	}

	return _ret1 ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	auto hDevice = PCIManager::Instance().GetHandle(deviceIndex);

	static DAM3000M_PARA_DI DI_Para;
	memset(&DI_Para, 0, sizeof(DAM3000M_PARA_DI));
	PBYTE pDIPara = (PBYTE)&DI_Para;
	if (DAM3000M_GetDeviceDI(hDevice, lDeviceID, &DI_Para, 16)) {
		for (int _index = 0; _index < 16; ++_index) {
			OutDIStatus[_index] = pDIPara[_index];
		}
		return 1;
	}
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	static LONG    lADBuffer[8];          // 采集数据缓冲区
	auto hDevice = PCIManager::Instance().GetHandle(deviceIndex);

	if (DAM3000M_ReadDeviceAD(hDevice, lDeviceID, lADBuffer, 0, 7)) {
		CopyTo(lADBuffer, OutADStatus, 8);
		return 1;
	}
	return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
