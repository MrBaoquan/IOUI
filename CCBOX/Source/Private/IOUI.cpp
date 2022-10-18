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
#include "DataCollect.h"
#include "Paths.hpp"

#pragma comment(lib,"DataCollect.lib")

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.OutputCount = 8;
	devInfo.AxisCount = 0;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\DataCollect.dll";
	auto _module = LoadLibraryA(_path.data());
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    if (InDOStatus[0] <=0 ) return 0;
    DataCollect::IncTypeClickedNum(InDOStatus[1],InDOStatus[2]);
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
	 for (int _idx=0; _idx<devInfo.OutputCount; ++_idx)
	 {
		 OutDOStatus[_idx] = 0;
	 }
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
