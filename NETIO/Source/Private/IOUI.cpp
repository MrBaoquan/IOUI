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
#include "NetIO.h"


DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 255;
	devInfo.OutputCount = 255;
	devInfo.AxisCount = 255;
    return &devInfo;
}

std::map<int, std::shared_ptr<NetIO>> netIOMap;
std::map<int, std::vector<short>> lastDOStatusMap;
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
   int _port = 20000 + deviceIndex;

   const auto& _netIO = std::make_shared<NetIO>(std::to_string(_port));
   if (_netIO->isServiceCreated()) {

       if (netIOMap.find(deviceIndex) == netIOMap.end()) {
            lastDOStatusMap[deviceIndex] = std::vector<short>(devInfo.InputCount, 0);
       }

        netIOMap[deviceIndex] = _netIO;
        _netIO->start();
        return 1;
   }
   
   return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    if (netIOMap.find(deviceIndex) != netIOMap.end()) {
		netIOMap[deviceIndex]->stop();
        netIOMap.erase(deviceIndex);
    }

    if (lastDOStatusMap.find(deviceIndex) != lastDOStatusMap.end()) {
        lastDOStatusMap.erase(deviceIndex);
    }

    return 1;
}

const BYTE maxInputCount = 240;
const BYTE funcChannel = 240;
const BYTE portChannel = funcChannel + 5;


IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    // 获取InDOStatus 与 lastDOStatus 之间有变动的通道号
    static std::vector<int> changedChannel;
    changedChannel.clear();

    auto& lastDOStatus = lastDOStatusMap[deviceIndex];

    for (int i = 0; i < maxInputCount; i++) {
        if (InDOStatus[i] != lastDOStatus[i]) {
            changedChannel.push_back(i);
        }
    }

    std::copy(InDOStatus, InDOStatus + devInfo.InputCount, lastDOStatus.begin());

    const auto& netIO = netIOMap[deviceIndex];
    const short funcID = InDOStatus[funcChannel];   // 0 SetAll  1 SetDI  2 SetAD  10 ZeroAll  11 ZeroDI  12 ZeroAD

    if (funcID > 0 || changedChannel.size()>0) {
        Event _netEventArg;
        _netEventArg.evt = "SetAll";

        for (int i : changedChannel) {
            const auto key = std::to_string(i);
            _netEventArg.data[key] = std::to_string(InDOStatus[i]);
        }

        if (funcID == 0) {
            _netEventArg.evt = "SetAll";
            if (changedChannel.size() <= 0) return 1;
        }
        else if (funcID == 1) {
            _netEventArg.evt = "SetDI";
            if (changedChannel.size() <= 0) return 1;
        }
        else if (funcID == 2) {
			_netEventArg.evt = "SetAD";
            if (changedChannel.size() <= 0) return 1;
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

		std::string _ip = std::to_string(_ip1) + "." + std::to_string(_ip2) + "." + std::to_string(_ip3) + "." + std::to_string(_ip4);

        int _port = InDOStatus[portChannel];
        _port = _port == 0 ? 20000 : _port;

        netIO->sendEvent(_netEventArg, _ip, std::to_string(_port));
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
	auto& lastDOStatus = lastDOStatusMap[deviceIndex];
    std::copy(lastDOStatus.begin(), lastDOStatus.end(), OutDOStatus);
    return 1;
}
static Event netEvent;
IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    const auto& netIO = netIOMap[deviceIndex];
    netIO->checkAndBroadcast("please ignore", 5);

    if (netIO->popEvent(netEvent)) {
        if (netEvent.evt == "") return 1;

        if (netEvent.evt == "ZeroDI" || netEvent.evt == "ZeroAll") {
            std::fill_n(OutDIStatus, devInfo.InputCount, 0);
        }
        else if (netEvent.evt == "SetDI" || netEvent.evt == "SetAll") {
            for (const auto& [key, value] : netEvent.data) {
                const auto channel = std::stoi(key);
                const auto val = std::stoi(value);
                if (channel >= 0 && channel < devInfo.InputCount) {
					OutDIStatus[channel] = val > 0 ? 1 : 0;
                }
            }
        }
    }
    else {
        netEvent.evt = "";
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    if (netEvent.evt == "") return 1;

    if (netEvent.evt == "ZeroAD" || netEvent.evt == "ZeroAll") {
        std::fill_n(OutADStatus, devInfo.AxisCount, 0);
    }
    else if (netEvent.evt == "SetAD" || netEvent.evt=="SetAll") {
        for (const auto& [key, value] : netEvent.data) {
            const auto channel = std::stoi(key);
            const auto val = std::stoi(value);
            if (channel >= 0 && channel < devInfo.AxisCount) {
                OutADStatus[channel] = val;
            }
        }
    }
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) 
{
	return 0;
}
