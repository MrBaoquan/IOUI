/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-30 10:55
 *  Email: mrma617@gmail.com
 */

#pragma once
#include <map>
#include <vector>
#include <windows.h>
#include "IOUI.h"
#include "Util.hpp"

class LessKey
{
public:
	bool operator()(const uint8& lhs, const uint8& rhs) const
	{
		return lhs < rhs;
	}
};

class DeviceData {
public:
	DeviceData(HANDLE InHandle,DeviceInfo InDevInfo):
		handle(InHandle)
		,devInfo(InDevInfo)
	{
		_DOStatus.assign(devInfo.OutputCount, 0);
	}
	DeviceData(){}

	BYTE* DOStatus() const{
		return const_cast<BYTE*>(_DOStatus.data());
	}
	
public:
	HANDLE handle;
	DeviceInfo devInfo;

private:
	std::vector<BYTE> _DOStatus;
};

class PCIManager
{
public:
    static PCIManager& Instance()
    {
        static PCIManager Instance;
        return Instance;
    }

	bool AddDevice(uint8 InDevIndex, const DeviceData& InDevData) 
	{
		if (DeviceDatas.count(InDevIndex)) { return false; }
		DeviceDatas.insert(std::pair<uint8, DeviceData>(InDevIndex, InDevData));
		return true;
	}

	const DeviceData* const GetDeviceData(uint8 InDevIndex) const
	{
		if (DeviceDatas.count(InDevIndex)) {
			return &DeviceDatas[InDevIndex];
		}
		return nullptr;
	}

    HANDLE GetHandle(uint8 deviceIndex)
    {
		const DeviceData* _devData = GetDeviceData(deviceIndex);
		if (!_devData) { return INVALID_HANDLE_VALUE; }
		return _devData->handle;
    }

private:
    PCIManager() = default;
    ~PCIManager() = default;

private:
	static std::map<uint8, DeviceData, LessKey> DeviceDatas;

    std::map<uint8, HANDLE> Devices;
	std::map<uint8, std::vector<BYTE>> DOStatus;
};

__declspec(selectany) std::map<uint8, DeviceData, LessKey> PCIManager::DeviceDatas;
