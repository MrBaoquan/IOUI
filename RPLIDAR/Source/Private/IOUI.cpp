/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <windows.h>
#include "opencv2/opencv.hpp"
#include "PCIManager.hpp"
#include "rplidar.h"

#pragma  comment(lib,"rplidar_driver.lib")
#ifdef _DEBUG
#pragma comment(lib,"opencv_world440d.lib")
#else
#pragma comment(lib,"opencv_world440.lib")
#endif // WIN_64

using namespace rp::standalone::rplidar;

DeviceInfo devInfo;
RPlidarDriver* rpDriver = nullptr;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 16;
	devInfo.OutputCount = 16;
	devInfo.AxisCount = 16;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	rpDriver = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
	if (!rpDriver) {
		return 0;
	}
	rplidar_response_device_info_t _deviceInfo;
	if (IS_OK(rpDriver->connect("COM3", 115200))) {
		if (IS_OK(rpDriver->getDeviceInfo(_deviceInfo))) {
			rpDriver->startMotor();
			rpDriver->startScan(0, 1);
			return 1;
		}
		else {
			delete rpDriver;
			rpDriver = nullptr;
		}
	};
	
	return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	if (rpDriver) {
		rpDriver->stop();
		rpDriver->stopMotor();
		RPlidarDriver::DisposeDriver(rpDriver);
		return 1;
	}
	
	return 0;
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
	return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	rplidar_response_measurement_node_t nodes[8192];
	size_t   count = _countof(nodes);

	auto _result = rpDriver->grabScanData(nodes, count);

	if (IS_OK(_result)) {
		rpDriver->ascendScanData(nodes, count);
		for (int pos = 0; pos < (int)count; ++pos) {
			printf("%s theta: %03.2f Dist: %08.2f Q: %d \n",
				(nodes[pos].sync_quality & RPLIDAR_RESP_MEASUREMENT_SYNCBIT) ? "S " : "  ",
				(nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f,
				nodes[pos].distance_q2 / 4.0f,
				nodes[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);

			// 1. 极坐标转笛卡尔坐标
			float _angle = (nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f;
			float _length = nodes[pos].distance_q2 / 4.0f;

			float _x = _length * std::sin(_angle);
			float _y = _length * std::sin(_angle);
			
			// 2. 获取透视变换矩阵
		/*	cv::Point2f 
			cv::getPerspectiveTransform()*/
			// 3. 获取转换后的坐标
			// 4. 映射点坐标到屏幕坐标
		}
	}
	return 0;
}
