#ifndef _PCI8201_DEVICE_
#define _PCI8201_DEVICE_

//***********************************************************
// DA输出函数WriteDeviceDA的模拟量输出范围参数OutputRange所使用的选项
const long PCI8201_OUTPUT_0_P5000mV			= 0x00;		// 0～5000mV
const long PCI8201_OUTPUT_0_P10000mV		= 0x01;		// 0～10000mV
const long PCI8201_OUTPUT_0_P10800mV		= 0x02;		// 0～10800mV
const long PCI8201_OUTPUT_N5000_P5000mV		= 0x03;		// ±5000mV
const long PCI8201_OUTPUT_N10000_P10000mV	= 0x04;		// ±10000mV
const long PCI8201_OUTPUT_N10800_P10800mV	= 0x05;		// ±10800mV
const long PCI8201_OUTPUT_0_P10mA			= 0x06;		// 0～10mA
const long PCI8201_OUTPUT_4_P20mA			= 0x07;		// 4～20mA

//***********************************************************
// 用户函数接口
#ifndef _PCI8201_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## 常规通用函数 #################################
	// 适用于本设备的最基本操作
	HANDLE DEVAPI FAR PASCAL PCI8201_CreateDevice(int DeviceID = 0);		// 创建设备对象
	HANDLE DEVAPI FAR PASCAL PCI8201_CreateDeviceEx(LONG DevicePhysID = 0);  // 用物理号创建设备对象
	int DEVAPI FAR PASCAL PCI8201_GetDeviceCount(HANDLE hDevice);			// 取得设备总台数
	BOOL DEVAPI FAR PASCAL PCI8201_GetDeviceCurrentID(HANDLE hDevice, PLONG DeviceLgcID, PLONG DevicePhysID);
	BOOL DEVAPI FAR PASCAL PCI8201_ListDeviceDlg(HANDLE hDevice);			// 列表系统当中的所有的该PCI设备
    BOOL DEVAPI FAR PASCAL PCI8201_ReleaseDevice(HANDLE hDevice);			// 关闭设备,禁止传输,且释放资源

	//####################### DA数据输出函数 #################################
	// 适于大多数普通用户，这些接口最简单、最快捷、最可靠，让用户不必知道设备
	// 低层复杂的硬件控制协议和繁多的软件控制编程，仅用下面一个函数便能轻
	// 松实现高速、连续的DA数据输出
	BOOL DEVAPI FAR PASCAL PCI8201_WriteDeviceDA(							// 写DA数据
									HANDLE hDevice,							// 设备对象句柄,它由CreateDevice函数创建
									LONG OutputRange,						// 输出量程，具体定义请参考上面的常量定义部分
									SHORT nDAData,							// 输出的DA原始数据[0, 4095]
									int nDAChannel);						// DA输出通道[0-7]

	//################# 内存映射寄存器直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	BOOL DEVAPI FAR PASCAL PCI8201_GetDeviceBar(					// 取得指定的指定设备寄存器组BAR地址
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											__int64 pbPCIBar[6]);	// 返回PCI BAR所有地址,具体PCI BAR中有多少可用地址请看硬件说明书
	BOOL DEVAPI FAR PASCAL PCI8201_GetDevVersion(					// 获取设备固件及程序版本
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											PULONG pulFmwVersion,	// 固件版本
											PULONG pulDriverVersion);// 驱动版本

	BOOL DEVAPI FAR PASCAL PCI8201_WriteRegisterByte(		// 往指定寄存器空间位置写入单节字数据
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											__int64 LinearAddr,		// 指定寄存器的线性基地址,它等于GetDeviceAddr中的LinearAddr参数返回值
											__int64 OffsetBytes,		// 相对于线性基地址基地址的偏移位置(字节)
											BYTE Value);			// 往指定地址写入单字节数据（其地址由线性基地址和偏移位置决定）

	BOOL DEVAPI FAR PASCAL PCI8201_WriteRegisterWord(		// 写双字节数据（其余同上）
											HANDLE hDevice, 
											__int64 LinearAddr, 
											__int64 OffsetBytes,  
											WORD Value);

	BOOL DEVAPI FAR PASCAL PCI8201_WriteRegisterULong(		// 写四节字数据（其余同上）
											HANDLE hDevice, 
											__int64 LinearAddr, 
											__int64 OffsetBytes,  
											ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI8201_ReadRegisterByte(		// 读入单字节数据（其余同上）
											HANDLE hDevice, 
											__int64 LinearAddr, 
											ULONG OffsetBytes);

	WORD DEVAPI FAR PASCAL PCI8201_ReadRegisterWord(		// 读入双字节数据（其余同上）
											HANDLE hDevice, 
											__int64 LinearAddr, 
											ULONG OffsetBytes);

	ULONG DEVAPI FAR PASCAL PCI8201_ReadRegisterULong(		// 读入四字节数据（其余同上）
											HANDLE hDevice, 
											__int64 LinearAddr, 
											ULONG OffsetBytes);

	//################# I/O端口直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	// 但这些函数主要适用于传统设备，如ISA总线、并口、串口等设备，不能用于本PCI设备

	BOOL DEVAPI FAR PASCAL PCI8201_WritePortByte(HANDLE hDevice, __int64 pbPort, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI8201_WritePortWord(HANDLE hDevice, __int64 pbPort, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI8201_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI8201_ReadPortByte(HANDLE hDevice, __int64 pbPort);
    WORD DEVAPI FAR PASCAL PCI8201_ReadPortWord(HANDLE hDevice, __int64 pbPort);
    ULONG DEVAPI FAR PASCAL PCI8201_ReadPortULong(HANDLE hDevice, __int64 pbPort);

	//########################### 线程操作函数 ######################################
	HANDLE DEVAPI FAR PASCAL PCI8201_CreateSystemEvent(void);			// 创建内核事件对象，供InitDeviceInt和VB子线程等函数使用
	BOOL DEVAPI FAR PASCAL PCI8201_ReleaseSystemEvent(HANDLE hEvent);	// 释放内核事件对象


#ifdef __cplusplus
}
#endif

// 自动包含驱动函数导入库
#ifndef _PCI8201_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI8201_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8201_32.dll...")
		#pragma message("======== Successfully linked with PCI8201_32.dll")
	#else
		#pragma comment(lib, "PCI8201_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8201_64.dll...")
		#pragma message("======== Successfully linked with PCI8201_64.dll")
	#endif

#endif

#endif // _PCI8201_DEVICE_
