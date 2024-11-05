/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include "IOUI.h"
#include "USB840DLL.h"
#include "Paths.hpp"


#pragma comment(lib,"USB840DLL64.lib")

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 16;
	devInfo.OutputCount = 16;
	devInfo.AxisCount = 0;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{

#ifdef WIN_64
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\USB840DLL64.dll";
#else
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\USB840DLL32.dll";
#endif

    return DeviceOpen(deviceIndex) ? 0 : 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    DeviceClose(deviceIndex);
    return 1;
}


IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{

	static int m_out;
	static int m_mask[] = { 0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1000,0x2000,0x4000,0x8000 };
	for (int i = 0; i < 16; i++)
	{
		if (InDOStatus[i] == 1)
		{
			m_out |= m_mask[i];
		}
		else
		{
			m_out &= ~m_mask[i];
		}
	}


	bool ret1 = USB840SetDO(deviceIndex, 1, (m_out & 0xFF));
	bool ret2 = USB840SetDO(deviceIndex, 2, ((m_out >> 8) & 0xFF));

	return ret1 && ret2 ? 1 : 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{

	static unsigned char din[2];

	din[1] = USB840ReadDI(deviceIndex, 2);
	din[0] = USB840ReadDI(deviceIndex, 1);

	unsigned short data = 0;
	data |= din[1] & 0x00ff;
	data = data << 8;
	data |= din[0] & 0x00ff;

	for (int i = 0; i < 16; i++)
	{
		OutDIStatus[i] = data >> i & 1;
	}

    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
