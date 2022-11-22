/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <bitset>
#include <vector>
#include <algorithm>
#include "Util.hpp"
#include "Paths.hpp"
#include "PCI8409.h"

#pragma comment(lib,"pci8409.lib")

namespace dh = DevelopHelper;

uint8 GetGroupLength(uint8 groupIndex)
{
    uint8 group_length = 16;
    if (groupIndex == 5 || groupIndex == 6)
    {
        group_length = 8;
    }
    return group_length;
}

long SetDeviceDOByGroup(uint8 deviceIndex, uint8 groupIndex, BYTE* InDOStatus)
{
    size_t group_length = GetGroupLength(groupIndex);

    uint8 last_group_index = groupIndex - 1;
    size_t pos = 0;
    if (last_group_index == 5 || last_group_index == 6)
    {
        pos = 8;
    }
    else
    {
        pos = 16;
    }
    if (groupIndex == 1)
    {
        pos = 0;
    }

    static BYTE* lastPos = nullptr;
    if (lastPos == nullptr)
    {
        lastPos = InDOStatus;
    }

    BYTE* currentPos = lastPos + pos;
    lastPos = currentPos;

    if (groupIndex == 10)
    {
        lastPos = nullptr;
    }

    std::bitset<16> bits;

    for (uint8 index = 0;index < group_length;++index)
    {
        bits[index] = currentPos[index] ? true : false;
    }
    return ZT8409_DOAll(deviceIndex, groupIndex, bits.to_ulong()) == 0 ? 1 : 0;
}


DeviceInfo g_DeviceInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	g_DeviceInfo.InputCount = 144;
	g_DeviceInfo.OutputCount = 144;
	g_DeviceInfo.AxisCount = 0;
	
	return &g_DeviceInfo;
}


IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	int _retCode = ZT8409_OpenDevice(deviceIndex) == 0 ? 1 : 0;

	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\PCI8409\\config.ini";
	const char* app = "/PCISettings";
	for (uint8 _groupIndex = 1; _groupIndex <= 10; ++_groupIndex) {
		int _ioMode = 0;
		char data[MAX_PATH];
		std::string _key = "group_" + std::to_string(_groupIndex) + "_mode";
		GetPrivateProfileStringA(app, _key.data(), "input", data, MAX_PATH, config_file_path.data());
		if (std::string(data) == "output") {
			_ioMode = 1;
		}
		ZT8409_SetIOMode(deviceIndex, _groupIndex, _ioMode);
	}
	return _retCode;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return ZT8409_CloseDevice(deviceIndex) == 0 ? 1 : 0;
}

BYTE DOStatus[144];
IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	CopyTo(InDOStatus, DOStatus, g_DeviceInfo.OutputCount);
    for (uint8 index = 1;index <= 10;++index)
    {
        if(!SetDeviceDOByGroup(deviceIndex, index, DOStatus))
        {
            return 0;
        }
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    uint8 accumulator = 0;
    for (uint8 group_index = 1;group_index <= 10;group_index++)
    {
        uint8 group_length = GetGroupLength(group_index);
        long _doStatus = ZT8409_GetLastDO(deviceIndex, group_index);
		if (_doStatus == -1) {
			int _error = ZT8409_GetLastErr();
			ZT8409_ClearLastErr();
		}
		_doStatus = _doStatus == -1 ? 0 : _doStatus;

        std::bitset<16> bits(_doStatus);
        for (uint8 ch_index = 0;ch_index < group_length;++ch_index)
        {
            OutDOStatus[accumulator++] = bits[ch_index] ? 1 : 0;
        }
    }
    
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	uint8 _accumulator = 0;
	for (uint8 _groupIndex = 1; _groupIndex <= 10; ++_groupIndex) {
		uint8 _group_length = GetGroupLength(_groupIndex);
		long _diStatus = ZT8409_DIAll(deviceIndex, _groupIndex);
		if (_diStatus == -1) {
			int _error = ZT8409_GetLastErr();
			ZT8409_ClearLastErr();
		}
		_diStatus = _diStatus == -1 ? 0 : _diStatus;

		std::bitset<16> bits(_diStatus);
		for (uint8 _chIndex = 0; _chIndex < _group_length; ++_chIndex) {
			OutDIStatus[_accumulator++] = bits[_chIndex] ? 1 : 0;
		}
	}
	
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
