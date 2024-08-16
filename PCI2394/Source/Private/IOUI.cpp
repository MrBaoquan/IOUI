/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <windows.h>
#include "PCIManager.hpp"
#include "PCI2394.h"
#include "mIni/mini/ini.h"
#include "Paths.hpp"

#ifdef WIN_64
#pragma comment(lib,"PCI2394_64.lib")
#else
#pragma comment(lib,"PCI2394.lib")
#endif

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = MAX_CHANNEL_COUNT;
	devInfo.OutputCount = MAX_CHANNEL_COUNT;
	devInfo.AxisCount = MAX_CHANNEL_COUNT * 2;
    return &devInfo;
}


mINI::INIFile* g_iniFIle = nullptr;
mINI::INIStructure* g_iniStructure = nullptr;

int g_maxDelta = 999;
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{


#ifdef WIN_64
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\PCI2394_64.dll";
#else
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\PCI2394.dll";
#endif // WIN_64
	auto _module = LoadLibraryA(_path.data());

	HANDLE hHandle = PCI2394_CreateDevice(deviceIndex);
	
	std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\PCI2394\\config.ini";
	g_iniFIle = new  mINI::INIFile(config_file_path);
	g_iniStructure = new  mINI::INIStructure();
	g_iniFIle->read(*g_iniStructure);
	auto& ini = *g_iniStructure;
	g_maxDelta = std::stoi(ini["PCISettings"]["maxDelta"]);
	OutputDebugStringA(ini["PCISettings"]["maxDelta"].data());
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		PCI2394_PARA_CNT CNTPara[MAX_CHANNEL_COUNT];
		for (int _index = 0; _index < 4; _index++)
		{

			CNTPara[_index].lCNTMode = std::stol(ini["PCISettings"]["lCNTMode"]);  // PCI2394_CNTMODE_QUADRATURE_X4;// PCI2394_CNTMODE_1_PULSE;
			CNTPara[_index].lResetMode = PCI2394_RESETMODE_ZERO; // ��������λ��0x00000000
			CNTPara[_index].bOverflowLock = FALSE; // ��������
			CNTPara[_index].bUnderflowLock = FALSE; // ��������
			CNTPara[_index].bDigitFilter = TRUE; // ��������Ƿ���������˲�(DF) TRUE=����
			CNTPara[_index].lLatchMode = PCI2394_LATCHMODE_SOFT; // ����������������� SoftWare

			PCI2394_InitDeviceCNT(hHandle, &CNTPara[_index], _index);
		}

		PCIManager::Instance().AddDevice(deviceIndex, DeviceData(hHandle, devInfo));
		return 1;
	}
	return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	delete g_iniFIle;
	delete g_iniStructure;
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	return PCI2394_ReleaseDevice(hHandle) ? 1 : 0;
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
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	ZeroMemory(OutDIStatus, devInfo.InputCount);
	return PCI2394_GetDeviceDI(hHandle, OutDIStatus) ? 1 : 0;
}

unsigned long lastADStatus[MAX_CHANNEL_COUNT];
IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
	unsigned long realADStatus[MAX_CHANNEL_COUNT] = {};

	for (int _index = 0; _index < MAX_CHANNEL_COUNT; _index++)
	{
		if (!PCI2394_GetDeviceCNT(hHandle, &realADStatus[_index], _index)) {
			continue;
		}
	}
	
	for (uint8 _index = 0; _index < MAX_CHANNEL_COUNT; ++_index)
	{
		unsigned short _delta = static_cast<short>(realADStatus[_index] - lastADStatus[_index]);
		if (std::abs(_delta) > g_maxDelta) {
			_delta = 0;
		}
		OutADStatus[_index] = _delta;
		OutADStatus[_index + 4] = realADStatus[_index];
		lastADStatus[_index] = realADStatus[_index];
	}
	return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
