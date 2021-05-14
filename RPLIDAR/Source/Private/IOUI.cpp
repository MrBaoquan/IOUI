/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <windows.h>
#include "RPLidarWrapper.h"
#include <mutex>
#include <thread>
#include "Paths.hpp"

RPLidarWrapper* rpLidar;

DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 16;
	devInfo.OutputCount = 16;
	devInfo.AxisCount = 192;
    return &devInfo;
}

std::mutex g_mutex;
bool bExit = false;
short doStatus[192];
int debugStatus=-1000;

void __cdecl Update() {
	rpLidar = new RPLidarWrapper();
	///////// 非常重要 !!! Opencv imshow 窗口  destroyWindow 窗口控制必须在同一个线程中进行
	rpLidar->OpenLidar();
	while (!bExit)
	{
		{
			std::lock_guard<std::mutex> lock(g_mutex);
			rpLidar->Update();
			auto _newTouchPoints = rpLidar->getTouchPoints();
			memcpy(doStatus, _newTouchPoints, sizeof(short) * 192);
			if(debugStatus!=-1000)
				rpLidar->SetDebugMode(debugStatus);
		}
		std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(20));
	}
	rpLidar->CloseLidar();
	delete rpLidar;
}

std::thread rplidarThread;
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
#ifdef _DEBUG
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\opencv_world440d.dll";
#else
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\opencv_world440.dll";
#endif // WIN_64
	auto _module = LoadLibraryA(_path.data());
	rplidarThread = std::thread(Update);
	return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	if (rplidarThread.joinable()) {
		bExit = true;

		rplidarThread.join();
	}
	return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	if (InDOStatus[0] != -1000) {
		debugStatus = InDOStatus[0];
		return 1;
	}
    return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
	if (rpLidar && rpLidar->rpConfig.config.debugMode != -1000) {
		OutDOStatus[0] = rpLidar->rpConfig.config.debugMode;
		return 1;
	}
	OutDOStatus[0] = -1000;
    return 0;
}
// 
IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		memcpy(OutADStatus, doStatus, sizeof(short) * 192);
	}
	
	return 1;
}