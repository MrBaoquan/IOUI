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
#include "Serial.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <atomic>
namespace dh = DevelopHelper;

DeviceInfo devInfo;
Serial* g_serialPort = nullptr;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 128;
	devInfo.OutputCount = 128;
	devInfo.AxisCount = 128;
    return &devInfo;
}

// 线程相关变量
std::queue<std::map<int, short>> dirtyQueue;
std::mutex queueMutex;
std::condition_variable queueCV;
std::atomic<bool> stopThread(false);

std::thread deviceThread;
int g_wait = 60;

// 处理设备命令的线程
void processDirtyStatus() {
	while (!stopThread) {
		std::map<int, short> _dirtyDOStatus;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			queueCV.wait(lock, [] { return !dirtyQueue.empty() || stopThread; });
			if (stopThread && dirtyQueue.empty()) {
				return; // 退出线程
			}
			_dirtyDOStatus = dirtyQueue.front();
			dirtyQueue.pop();
		}

		// 处理_dirtyDOStatus
		for (auto& _doItem : _dirtyDOStatus) {
			static char _data[4] = { };
			_data[0] = 0xFE;
			_data[1] = _doItem.first;
			_data[2] = _doItem.second;
			_data[3] = 0xFF;
			g_serialPort->write(_data, 4);
			if (g_wait > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(g_wait));
		}
	}
}

//int channelIndex = 1;
//int valueIndex = 2;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\SERIAL-IOFE\\config.ini";
	const char* app = "/PCISettings";
	DWORD _baudRate = GetPrivateProfileIntA(app, "BaudRate", 115200, config_file_path.data());
	g_wait = GetPrivateProfileIntA(app, "waitTimeMs", 0, config_file_path.data());
	

	deviceThread = std::thread(processDirtyStatus);

	try
	{
		g_serialPort = new Serial("COM" + std::to_string(deviceIndex), _baudRate);
	}
	catch (const char* _err)
	{
		return 0;
	}
	
	
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopThread = true;  // 设置标志，通知线程停止
    }
    queueCV.notify_one();  // 唤醒线程以便它能及时退出

    // 检查线程是否有效且可join
    if (deviceThread.joinable()) {
        deviceThread.join();  // 等待线程结束
    }

	g_serialPort->flush();
	delete g_serialPort;
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{	
	static std::vector<short> _lastDOStatus(devInfo.OutputCount,0);
	std::map<int,short> _dirtyDOStatus;
	for (auto _idx = 0; _idx < _lastDOStatus.size();++_idx) {
		if (_lastDOStatus[_idx] != InDOStatus[_idx]) {
			_dirtyDOStatus.insert(std::pair<int,short> (_idx, InDOStatus[_idx]));
			_lastDOStatus[_idx] = InDOStatus[_idx];
		}
	}

 	if (!_dirtyDOStatus.empty()) {
        // 将_dirtyDOStatus 插入线程安全队列
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            dirtyQueue.push(_dirtyDOStatus);
        }
        queueCV.notify_one(); // 唤醒处理线程
    }
	
	return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	static char _data[MAX_PATH];
	static std::vector<uint8> _recvDatas;
	DWORD _count=0;
	auto _recevCount = g_serialPort->read(_data, MAX_PATH, false);
	for (int _idx = 0;_idx < _recevCount;++_idx) {
		_recvDatas.push_back(_data[_idx]);
	}
	while (_recvDatas.size()>=4) {
		if (_recvDatas[0] != 0xFE) {
			_recvDatas.erase(_recvDatas.begin());
			continue;
		}
		std::vector<uint8> _content(_recvDatas.begin(), _recvDatas.begin() + 4);
		uint8 _channel = _content[1];
		uint8 _status = _content[2];
		if (_status == 0x00) {
			OutDIStatus[_channel] = 0;
		}
		else {
			OutDIStatus[_channel] = 1;
		}
		_recvDatas.erase(_recvDatas.begin(), _recvDatas.begin() + 4);
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
