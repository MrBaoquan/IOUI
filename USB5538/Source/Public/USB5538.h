#ifndef _USB5538_DEVICE_
#define _USB5538_DEVICE_

#include<windows.h>

//***********************************************************
// 驱动函数接口
#ifndef _USB5538_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## 常规通用函数 #################################
	HANDLE DEVAPI FAR PASCAL USB5538_CreateDevice(int DeviceLgcID = 0); // 创建设备对象(该函数使用系统内逻辑设备ID）
	HANDLE DEVAPI FAR PASCAL USB5538_CreateDeviceEx(int DevicePhysID = 0); // 创建设备对象(该函数使用板卡物理设备ID）
	int DEVAPI FAR PASCAL USB5538_GetDeviceCount(HANDLE hDevice);      // 取得USB5538在系统中的设备数量
	BOOL DEVAPI FAR PASCAL USB5538_GetDeviceCurrentID(HANDLE hDevice, PLONG DeviceLgcID, PLONG DevicePhysID); // 取得当前设备的逻辑ID号和物理ID号
	BOOL DEVAPI FAR PASCAL USB5538_ListDeviceDlg(void); // 用对话框列表系统当中的所有USB5538设备
	BOOL DEVAPI FAR PASCAL USB5538_ResetDevice(HANDLE hDevice);		 // 复位整个USB设备
    BOOL DEVAPI FAR PASCAL USB5538_ReleaseDevice(HANDLE hDevice);    // 设备句柄

	//####################### 数字I/O输入输出函数 #################################
	BOOL DEVAPI FAR PASCAL USB5538_GetDeviceDI(					// 取得开关量状态     
									HANDLE hDevice,				// 设备句柄,它应由CreateDevice函数创建								        
									BYTE bDISts[16]);			// 开关输入状态(注意: 必须定义为8个字节元素的数组)

    BOOL DEVAPI FAR PASCAL USB5538_SetDeviceDO(					// 输出开关量状态
									HANDLE hDevice,				// 设备句柄,它应由CreateDevice函数创建								        
									BYTE bDOSts[16]);			// 开关输出状态(注意: 必须定义为8个字节元素的数组)

	BOOL DEVAPI FAR PASCAL USB5538_RetDeviceDO(					// 回读输出开关量状态
									HANDLE hDevice,				// 设备句柄,它应由CreateDevice函数创建								        
									BYTE bDOSts[16]);			// 开关输出状态(注意: 必须定义为8个字节元素的数组)

#ifdef __cplusplus
}
#endif

// 自动包含驱动函数导入库
#ifndef _USB5538_DRIVER_
#ifndef _WIN64
#pragma comment(lib, "USB5538_32.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with USB5538_32.dll...")
#pragma message("======== Successfully linked with USB5538_32.dll")
#else
#pragma comment(lib, "USB5538_64.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with USB5538_64.dll...")
#pragma message("======== Successfully linked with USB5538_64.dll")
#endif

#endif

#endif; // _USB5538_DEVICE_