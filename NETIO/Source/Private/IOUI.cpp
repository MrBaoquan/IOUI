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
#include <memory>
#include "IOUI.h"
#include "NetIO.h"
#include "Volume.h"
#include "mIni/mini/ini.h"
#include "Paths.hpp"
#include "Util.hpp"

namespace dh = DevelopHelper;

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 255;
	devInfo.OutputCount = 255;
	devInfo.AxisCount = 255;
	return &devInfo;
}

// 定义设备上下文结构体，统一管理设备相关状态与配置
struct DeviceContext {
	std::shared_ptr<NetIO> netIO;
	std::vector<short> lastDOStatus;
	Event event;
	bool systemCtrl = false;

	std::string remote_ip = "127.0.0.1";
	int remote_port = 20000;

	int heartbeatIntervalSeconds = 5;  // 默认5秒，配置为0时不广播
};

// 维护设备上下文，deviceIndex -> Context
std::map<int, DeviceContext> deviceContexts;

std::shared_ptr<mINI::INIFile> g_iniFile = nullptr;
std::shared_ptr<mINI::INIStructure> g_iniStructure = nullptr;

const BYTE maxInputCount = 240;
const BYTE funcChannel = 240;
const BYTE portChannel = funcChannel + 5;   // 245
// 246 自定义发送标识  247 channel 值   248 value值
const BYTE systemCtrlChannel = funcChannel + 10;   // 使用DI250标识是否启用系统控制

// OpenDevice函数，读取配置并初始化设备上下文
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	int _port = 20000 + deviceIndex;

	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\NETIO\\config.ini";

	if (!g_iniFile) g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
	if (!g_iniStructure) g_iniStructure = std::make_shared<mINI::INIStructure>();
	g_iniFile->read(*g_iniStructure);
	auto& ini = *g_iniStructure;

	auto deviceSectionName = BuildDeviceAttribute("device", deviceIndex);
	auto& defaultSection = ini["default"];
	std::map<std::string, std::string> mergedConfig;

	for (const auto& kv : defaultSection)
		mergedConfig[kv.first] = kv.second;
	auto& deviceSectionMap = ini[deviceSectionName];
	for (const auto& kv : deviceSectionMap)
		mergedConfig[kv.first] = kv.second;

	auto netIOPtr = std::make_shared<NetIO>(std::to_string(_port));
	if (netIOPtr->isServiceCreated()) {
		// 获取或创建设备上下文
		auto& ctx = deviceContexts[deviceIndex];
		ctx.netIO = netIOPtr;

		if (ctx.lastDOStatus.empty()) {
			ctx.lastDOStatus.resize(devInfo.InputCount, 0);
		}

		ctx.event = Event();
		ctx.systemCtrl = false;

		ctx.remote_ip = "127.0.0.1";
		ctx.remote_port = 20000;

		ctx.heartbeatIntervalSeconds = 5;

		if (mergedConfig.count("remote_ip")) ctx.remote_ip = mergedConfig["remote_ip"];
		if (mergedConfig.count("remote_port")) ctx.remote_port = std::stoi(mergedConfig["remote_port"]);
		if (mergedConfig.count("heartbeat_interval")) ctx.heartbeatIntervalSeconds = std::stoi(mergedConfig["heartbeat_interval"]);

		netIOPtr->setEventLimit("SetAxis", 60);
		netIOPtr->start();
		return 1;
	}
	return 0;
}

// CloseDevice函数，释放设备上下文资源
IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	auto it = deviceContexts.find(deviceIndex);
	if (it != deviceContexts.end()) {
		if (it->second.netIO) {
			it->second.netIO->stop();
		}
		deviceContexts.erase(it);
	}

	return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	auto& ctx = deviceContexts[deviceIndex];

	static std::vector<int> changedChannel;
	changedChannel.clear();

	auto& lastDOStatus = ctx.lastDOStatus;

	for (int i = 0; i < devInfo.InputCount; i++) {
		if (i >= maxInputCount && i <= 248) {
			continue;
		}
		if (InDOStatus[i] != lastDOStatus[i]) {
			changedChannel.push_back(i);
		}
	}

	std::copy(InDOStatus, InDOStatus + devInfo.InputCount, lastDOStatus.begin());

	const auto& netIO = ctx.netIO;
	const short funcID = InDOStatus[funcChannel];   // 0 SetAll  1 SetDI  2 SetAD  10 ZeroAll  11 ZeroDI  12 ZeroAD
	const short cusSet = InDOStatus[246];

	if (funcID > 0 || changedChannel.size() > 0 || cusSet > 0) {
		Event _netEventArg;
		_netEventArg.evt = "SetAll";

		for (int i : changedChannel) {
			const auto key = std::to_string(i);
			_netEventArg.data[key] = std::to_string(InDOStatus[i]);
		}

		int _customFlag = InDOStatus[funcChannel + 6];
		int _customChannel = InDOStatus[funcChannel + 7];
		int _customValue = InDOStatus[funcChannel + 8];

		if (_customFlag >= 1) {
			_netEventArg.data[std::to_string(_customChannel)] = std::to_string(_customValue);
		}

		if (funcID == 0) {
			_netEventArg.evt = "SetAll";
			if (_netEventArg.data.size() <= 0) return 1;
		}
		else if (funcID == 1) {
			_netEventArg.evt = "SetDI";
			if (_netEventArg.data.size() <= 0) return 1;
		}
		else if (funcID == 2) {
			_netEventArg.evt = "SetAD";
			if (_netEventArg.data.size() <= 0) return 1;
		}
		else if (funcID == 10) {
			_netEventArg.evt = "ZeroAll";
		}
		else if (funcID == 11) {
			_netEventArg.evt = "ZeroDI";
		}
		else if (funcID == 12) {
			_netEventArg.evt = "ZeroAD";
		}

		int _ip1 = InDOStatus[funcChannel + 1];
		int _ip2 = InDOStatus[funcChannel + 2];
		int _ip3 = InDOStatus[funcChannel + 3];
		int _ip4 = InDOStatus[funcChannel + 4];

		std::string _ip;
		if (_ip1 == 0 && _ip2 == 0 && _ip3 == 0 && _ip4 == 0) {
			_ip = ctx.remote_ip;
		}
		else {
			_ip = std::to_string(_ip1) + "." + std::to_string(_ip2) + "." + std::to_string(_ip3) + "." + std::to_string(_ip4);
		}

		int _port = InDOStatus[portChannel];
		if (_port == 0) {
			_port = ctx.remote_port;
		}

		netIO->sendEvent(_netEventArg, _ip, std::to_string(_port));
	}

	return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
	auto& ctx = deviceContexts[deviceIndex];
	auto& lastDOStatus = ctx.lastDOStatus;
	std::fill_n(lastDOStatus.begin() + 246, 3, 0);
	std::copy(lastDOStatus.begin(), lastDOStatus.end(), OutDOStatus);
	return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	auto& ctx = deviceContexts[deviceIndex];
	const auto& netIO = ctx.netIO;
	if (ctx.heartbeatIntervalSeconds > 0) {
		netIO->checkAndBroadcastHeartbeat("Active Heartbeat", ctx.heartbeatIntervalSeconds);
	}
	
	auto& netEvent = ctx.event;
	netEvent.evt = "";

	auto applyDIEvent = [&](Event InDIEvent) {
		for (const auto& [key, value] : InDIEvent.data) {
			const auto channel = std::stoi(key);
			const auto val = std::stoi(value);
			if (channel >= 0 && channel < devInfo.InputCount) {
				OutDIStatus[channel] = val > 0 ? 1 : 0;

				if (channel == systemCtrlChannel) {
					ctx.systemCtrl = val == 1;
				}

				if (!ctx.systemCtrl) continue;

				if (channel == 9 && val == 1)  // 音量 +
				{
					IncreaseVolume();
				}
				else if (channel == 10 && val == 1)  // 音量 -
				{
					DecreaseVolume();
				}
				else if (channel == 11)             // 静音
				{
					SetMute(val >= 1 ? true : false);
				}
			}
		}
		};

	if (netIO->popEvent(netEvent)) {
		if (netEvent.evt == "") return 0;

		if (netEvent.evt == "ZeroDI" || netEvent.evt == "ZeroAll") {
			std::fill_n(OutDIStatus, devInfo.InputCount, 0);
		}
		else if (netEvent.evt == "SetDI" || netEvent.evt == "SetAll") {
			applyDIEvent(netEvent);
		}
		else if (netEvent.evt == "SetAD") {
			static Event diEvent;
			diEvent.evt = "";

			if (!netIO->peekEvent(diEvent)) return 1;
			if (diEvent.evt != "SetDI") return 1;

			netIO->popEvent(diEvent);
			applyDIEvent(diEvent);
		}
		return 1;
	}
	return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	auto& ctx = deviceContexts[deviceIndex];
	auto& netEvent = ctx.event;
	const auto& netIO = ctx.netIO;

	static std::vector<Event> adEvents;
	adEvents.clear();
	netIO->popEvents(adEvents, "SetAD", 255);

	if (netEvent.evt == "SetAD" || netEvent.evt == "SetAll") {
		adEvents.insert(adEvents.begin(), netEvent);
	}
	else if (netEvent.evt == "ZeroAD" || netEvent.evt == "ZeroAll") {
		std::fill_n(OutADStatus, devInfo.AxisCount, 0);
	}

	if (adEvents.size() <= 0) return 0;

	for (const auto& event : adEvents) {
		for (const auto& [key, value] : event.data) {
			const auto channel = std::stoi(key);
			const auto val = std::stoi(value);
			if (channel >= 0 && channel <= devInfo.InputCount) {
				OutADStatus[channel] = val;

				if (!ctx.systemCtrl) continue;

				if (channel == 11) {    // 通道11控制音量大小
					SetVolume(val / 1000.f);
				}
			}
		}
	}
	return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
	return 0;
}
