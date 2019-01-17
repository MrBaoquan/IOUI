#ifndef _PCI8735_DEVICE_
#define _PCI8735_DEVICE_

#include "windows.h"

//#################### AD硬件参数PCI8735_PARA_AD定义 #####################
// 用于AD采样的实际硬件参数
typedef struct _PCI8735_PARA_AD     
{
	LONG FirstChannel;      // 首通道[0, 31]
	LONG LastChannel;       // 末通道[0, 31],要求末通道必须大于或等于首通道
	LONG InputRange;		// 模拟量输入量程范围,其取值请见下面相关常量定义
	LONG GroundingMode;		// 接地方式(单端或双端选择),其取值请见下面相关常量定义
	LONG Gains;
} PCI8735_PARA_AD, *PPCI8735_PARA_AD;

//***********************************************************
// AD硬件参数PCI8735_PARA_AD中的InputRange模拟量输入范围所使用的选项
const long PCI8735_INPUT_N10000_P10000mV= 0x00; // ±10000mV
const long PCI8735_INPUT_N5000_P5000mV	= 0x01; // ±5000mV
const long PCI8735_INPUT_N2500_P2500mV	= 0x02; // ±2500mV
const long PCI8735_INPUT_0_P10000mV		= 0x03; // 0～10000mV

//***********************************************************
// AD参数PCI8735_PARA_AD中的Gains使用的硬件增益选项
const long PCI8735_GAINS_1MULT			= 0x00; // 1倍增益
const long PCI8735_GAINS_2MULT			= 0x01; // 2倍增益
const long PCI8735_GAINS_4MULT			= 0x02; // 4倍增益
const long PCI8735_GAINS_8MULT			= 0x03; // 8倍增益

//***********************************************************
// AD参数PCI8735_PARA_AD中的GroundingMode使用的模拟信号接地方式选项
const long PCI8735_GNDMODE_SE			= 0x00;	// 单端方式(SE:Single end)
const long PCI8735_GNDMODE_DI			= 0x01;	// 双端方式(DI:Differential)

//***********************************************************
// 用户函数接口
#ifndef _PCI8735_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## 设备对象管理函数 ##############################
	HANDLE DEVAPI FAR PASCAL PCI8735_CreateDevice(int DeviceLgcID = 0);     // 用逻辑号创建设备对象
	HANDLE DEVAPI FAR PASCAL PCI8735_CreateDeviceEx(int DevicePhysID = 0);  // 用物理号创建设备对象
	int DEVAPI FAR PASCAL PCI8735_GetDeviceCount(HANDLE hDevice); // 取得设备总台数
	BOOL DEVAPI FAR PASCAL PCI8735_GetDeviceCurrentID( // 获取逻辑ID号和物理ID号
									HANDLE hDevice,	   // 设备对象句柄,它由CreateDevice函数创建	
									PLONG DeviceLgcID, // 逻辑ID号
									PLONG DevicePhysID); // 本卡物理ID号无效
	BOOL DEVAPI FAR PASCAL PCI8735_ListDeviceDlg(HANDLE hDevice); // 以对话框窗体方式列表系统当中的所有的该PCI设备
    BOOL DEVAPI FAR PASCAL PCI8735_ReleaseDevice(HANDLE hDevice); // 仅释放设备对象

	//####################### AD数据读取函数 #################################
	// 适于大多数普通用户，这些接口最简单、最快捷、最可靠，让用户不必知道设备
	// 低层复杂的硬件控制协议和繁多的软件控制编程，仅用下面的初始化设备和读取
	// AD数据两个函数便能轻松高效地实现高速、连续的数据采集

	BOOL DEVAPI FAR PASCAL PCI8735_InitDeviceAD(			// 初始化设备，当返回TRUE后,设备即准备就绪.
									HANDLE hDevice,			// 设备对象,它由CreateDevice函数创建
									PPCI8735_PARA_AD pADPara); // 硬件参数, 它仅在此函数中决定硬件状态

	BOOL DEVAPI FAR PASCAL PCI8735_ReadDeviceAD(			// 读取设备上的AD数据
									HANDLE hDevice,			// 设备句柄,它由CreateDevice函数创建
									USHORT ADBuffer[],		// 接受原始AD数据的用户缓冲区
									LONG nReadSizeWords,	// 相对于偏位点后读入的数据长度(字)
									PLONG nRetSizeWords = NULL);	// 返回实际读取的长度(字)

	BOOL DEVAPI FAR PASCAL PCI8735_ReleaseDeviceAD(HANDLE hDevice); // 停止释放AD对象
									
	//##################### AD的硬件参数操作函数 ###########################
	BOOL DEVAPI FAR PASCAL PCI8735_SaveParaAD(HANDLE hDevice, PPCI8735_PARA_AD pADPara); // 将当前的AD采样参数保存至系统中
    BOOL DEVAPI FAR PASCAL PCI8735_LoadParaAD(HANDLE hDevice, PPCI8735_PARA_AD pADPara); // 将AD采样参数从系统中读出
    BOOL DEVAPI FAR PASCAL PCI8735_ResetParaAD(HANDLE hDevice, PPCI8735_PARA_AD pADPara); // 将AD采样参数恢复至出厂默认值

	//####################### 数字I/O输入输出函数 #################################
	// 用户可以使用WriteRegisterULong和ReadRegisterULong等函数直接控制寄存器进行I/O
	// 输入输出，但使用下面两个函数更省事，它不需要您关心寄存器分配和位操作等，而只
	// 需象VB等语言的属性操作那么简单地实现各开关量通道的控制。
	BOOL DEVAPI FAR PASCAL PCI8735_GetDeviceDI(						// 取得开关量状态     
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建								        
											BYTE bDISts[16]);		// 开关输入状态(注意: 必须定义为16个字节元素的数组)

    BOOL DEVAPI FAR PASCAL PCI8735_SetDeviceDO(						// 输出开关量状态
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建        
											BYTE bDOSts[16]);		// 开关输出状态(注意: 必须定义为16个字节元素的数组)

    BOOL DEVAPI FAR PASCAL PCI8735_RetDeviceDO(						// 回读开关量输出状态
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建        
											BYTE bDOSts[16]);		// 获得开关输出状态(注意: 必须定义为16个字节元素的数组)

//################# 内存映射寄存器直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	BOOL DEVAPI FAR PASCAL PCI8735_GetDeviceBar(					// 取得指定的指定设备寄存器组BAR地址
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											__int64 pbPCIBar[6]);	// 返回PCI BAR所有地址,具体PCI BAR中有多少可用地址请看硬件说明书
    BOOL DEVAPI FAR PASCAL PCI8735_GetDevVersion(					// 获取设备固件及程序版本
											HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
											PULONG pulFmwVersion,	// 固件版本
											PULONG pulDriverVersion);// 驱动版本
	BOOL DEVAPI FAR PASCAL PCI8735_WriteRegisterByte(				// 往设备的映射寄存器空间指定端口写入单节字数据
											HANDLE hDevice,			// 设备对象
											__int64 pbLinearAddr,	// 指定映射寄存器的线性基地址
											ULONG OffsetBytes,		// 相对于基地址的偏移位置
											BYTE Value);			// 往指定地址写入单字节数据（其地址由线性基地址和偏移位置决定）
	BOOL DEVAPI FAR PASCAL PCI8735_WriteRegisterWord(				// 写双字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// 指定映射寄存器的线性基地址
											ULONG OffsetBytes,  
											WORD Value);
	BOOL DEVAPI FAR PASCAL PCI8735_WriteRegisterULong(				// 写四节字数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// 指定映射寄存器的线性基地址
											ULONG OffsetBytes,  
											ULONG Value);
	BYTE DEVAPI FAR PASCAL PCI8735_ReadRegisterByte(				// 读入单字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// 指定映射寄存器的线性基地址
											ULONG OffsetBytes);
	WORD DEVAPI FAR PASCAL PCI8735_ReadRegisterWord(				// 读入双字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// 指定映射寄存器的线性基地址
											ULONG OffsetBytes);
	ULONG DEVAPI FAR PASCAL PCI8735_ReadRegisterULong(				// 读入四字节数据（其余同上）
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// 指定映射寄存器的线性基地址
											ULONG OffsetBytes);

	//################# I/O端口直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	// 但这些函数主要适用于传统设备，如ISA总线、并口、串口等设备，不能用于本PCI设备
    BOOL DEVAPI FAR PASCAL PCI8735_WritePortByte(HANDLE hDevice, __int64 pbPort, ULONG offserBytes, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI8735_WritePortWord(HANDLE hDevice, __int64 pbPort, ULONG offserBytes, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI8735_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG offserBytes, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI8735_ReadPortByte(HANDLE hDevice, __int64 pbPort, ULONG offserBytes);
    WORD DEVAPI FAR PASCAL PCI8735_ReadPortWord(HANDLE hDevice, __int64 pbPort, ULONG offserBytes);
    ULONG DEVAPI FAR PASCAL PCI8735_ReadPortULong(HANDLE hDevice, __int64 pbPort, ULONG offserBytes);

	//########################### 附加操作函数 ######################################
	HANDLE DEVAPI FAR PASCAL PCI8735_CreateSystemEvent(void);			// 创建内核事件对象，供InitDeviceInt和VB子线程等函数使用
	BOOL DEVAPI FAR PASCAL PCI8735_ReleaseSystemEvent(HANDLE hEvent);	// 释放内核事件对象

#ifdef __cplusplus
}
#endif

// 自动包含驱动函数导入库
#ifndef _PCI8735_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI8735_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8735_32.dll...")
		#pragma message("======== Successfully linked with PCI8735_32.dll")
	#else
		#pragma comment(lib, "PCI8735_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8735_64.dll...")
		#pragma message("======== Successfully linked with PCI8735_64.dll")
	#endif
	
#endif

#endif // _PCI8735_DEVICE_