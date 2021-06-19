#ifndef _PCI2324_DEVICE_
#define _PCI2324_DEVICE_

//***********************************************************
// 中断函数PCI2324_InitDeviceInt中的INTSrc使用的中断选择选项
const long PCI2324_INTSRC_DI0INT		= 0x00;	// 使能DI0中断(数字量输入通道0上升沿产生中断), =TRUE使能, =FALSE禁止
const long PCI2324_INTSRC_DI1INT		= 0x01;	// 使能DI1中断(数字量输入通道1上升沿产生中断), =TRUE使能, =FALSE禁止
const long PCI2324_INTSRC_DI01INT		= 0x02;	// 使能DI0与DI1中断(数字量输入通道0或1上升沿产生中断), =TRUE使能, =FALSE禁止

// 中断状态参数
typedef struct _PCI2324_STATUS_INT	// 中断信息控制参数
{
	LONG bDI0INT;						// DI0上升沿触发中断
	LONG bDI1INT;						// DI1上升沿触发中断
} PCI2324_STATUS_INT, *PPCI2324_STATUS_INT;
//######################## 常量定义 #################################
// CreateFileObject所用的文件操作方式控制字(可通过或指令实现多种方式并操作)
#define PCI2324_modeRead          0x0000			// 只读文件方式
#define PCI2324_modeWrite         0x0001			// 只写文件方式
#define	PCI2324_modeReadWrite     0x0002			// 既读又写文件方式
#define PCI2324_modeCreate        0x1000			// 如果文件不存可以创建该文件，如果存在，则重建此文件，并清0
#define PCI2324_typeText          0x4000			// 以文本方式操作文件

//***********************************************************
// 用于DA的复位方式（适用于PCI2324_InitDevProDA函数的ResetMode参数）
#define PCI2324_RESET_MODE_NEGATIVE        0x0000	// 负满度（-5V或-10V...）
#define PCI2324_RESET_MODE_ZERO			   0x0001	// 零点(0V)

//***********************************************************
// 用户函数接口
#ifndef _PCI2324_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## 常规通用函数 #################################
	// 适用于本设备的最基本操作
	HANDLE DEVAPI FAR PASCAL PCI2324_CreateDevice(int DeviceID = 0);		// 创建设备对象
	int DEVAPI FAR PASCAL PCI2324_GetDeviceCount(HANDLE hDevice);			// 取得设备总台数
	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceCurrentID(HANDLE hDevice, PLONG DeviceLgcID, PLONG DevicePhysID);
	BOOL DEVAPI FAR PASCAL PCI2324_ListDeviceDlg(HANDLE hDevice);			// 列表系统当中的所有的该PCI设备
    BOOL DEVAPI FAR PASCAL PCI2324_ReleaseDevice(HANDLE hDevice);			// 关闭设备,禁止传输,且释放资源

	//####################### 数字I/O输入输出函数 #################################
	// 用户可以使用WriteRegisterULong和ReadRegisterULong等函数直接控制寄存器进行I/O
	// 输入输出，但使用下面两个函数更省事，它不需要您关心寄存器分配和位操作等，而只
	// 需象VB等语言的属性操作那么简单地实现各开关量通道的控制。
	BOOL DEVAPI FAR PASCAL PCI2324_SetDeviceDO(             // 设置数字量输出状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDOSts[32]);	// 开关状态	

	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceDI(             // 取得数字量输入状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDISts[2]);	// 开关状态	

	//####################### 中断函数 #################################
	// 它由硬件信号的状态变化引起CPU产生中断事件hEventInt。
	BOOL DEVAPI FAR PASCAL PCI2324_InitDeviceInt(			// 初始化中断
										HANDLE hDevice,     // 设备句柄	
										HANDLE hEventInt,	// 中断事件句柄,它由CreateSystemEvent创建
										LONG INTSrc);		// 中断源选择
	LONG DEVAPI FAR PASCAL PCI2324_GetDeviceIntCount(HANDLE hDevice);  // 在中断初始化后，用它取得中断服务程序产生的次数
	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceIntSrc(HANDLE hDevice, PPCI2324_STATUS_INT pINTSts);  // 在中断初始化后，用它取得中断源
	BOOL DEVAPI FAR PASCAL PCI2324_ReleaseDeviceInt(HANDLE hDevice); // 释放中断资源

	//################# 内存映射寄存器直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceBar(					// 取得指定的指定设备寄存器组BAR地址
										HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
										__int64 pbPCIBar[6]);	// 返回PCI BAR所有地址,具体PCI BAR中有多少可用地址请看硬件说明书

	BOOL DEVAPI FAR PASCAL PCI2324_GetDevVersion(					// 获取设备固件及程序版本
										HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
										PULONG pulFmwVersion,	// 固件版本
										PULONG pulDriverVersion);// 驱动版本

	BOOL DEVAPI FAR PASCAL PCI2324_WriteRegisterByte(			// 往指定寄存器空间位置写入单节字数据
										HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
										__int64 pbLinearAddr,	// 指定寄存器的线性基地址,它等于GetDeviceAddr中的pbLinearAddr参数返回值
										ULONG OffsetBytes,		// 相对于线性基地址基地址的偏移位置(字节)
										BYTE Value);			// 往指定地址写入单字节数据（其地址由线性基地址和偏移位置决定）

	BOOL DEVAPI FAR PASCAL PCI2324_WriteRegisterWord(			// 写双字节数据（其余同上）
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes,  
										WORD Value);

	BOOL DEVAPI FAR PASCAL PCI2324_WriteRegisterULong(			// 写四节字数据（其余同上）
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes,  
										ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI2324_ReadRegisterByte(			// 读入单字节数据（其余同上）
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes);

	WORD DEVAPI FAR PASCAL PCI2324_ReadRegisterWord(			// 读入双字节数据（其余同上）
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes);

	ULONG DEVAPI FAR PASCAL PCI2324_ReadRegisterULong(			// 读入四字节数据（其余同上）
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes);

	//################# I/O端口直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	// 但这些函数主要适用于传统设备，如ISA总线、并口、串口等设备，不能用于本PCI设备
	BOOL DEVAPI FAR PASCAL PCI2324_WritePortByte(HANDLE hDevice, __int64 pbPort, BYTE Value);
	BOOL DEVAPI FAR PASCAL PCI2324_WritePortWord(HANDLE hDevice, __int64 pbPort, WORD Value);
	BOOL DEVAPI FAR PASCAL PCI2324_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI2324_ReadPortByte(HANDLE hDevice, __int64 pbPort);
	WORD DEVAPI FAR PASCAL PCI2324_ReadPortWord(HANDLE hDevice, __int64 pbPort);
	ULONG DEVAPI FAR PASCAL PCI2324_ReadPortULong(HANDLE hDevice, __int64 pbPort);


	//########################### 线程操作函数 ######################################
	HANDLE DEVAPI FAR PASCAL PCI2324_CreateSystemEvent(void);			// 创建内核事件对象，供InitDeviceInt和VB子线程等函数使用
	BOOL DEVAPI FAR PASCAL PCI2324_ReleaseSystemEvent(HANDLE hEvent);	// 释放内核事件对象	

#ifdef __cplusplus
}
#endif

// 自动包含驱动函数导入库
#ifndef _PCI2324_DRIVER_
#ifndef _WIN64
#pragma comment(lib, "PCI2324_32.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with PCI2324_32.dll...")
#pragma message("======== Successfully linked with PCI2324_32.dll")
#else
#pragma comment(lib, "PCI2324_64.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with PCI2324_64.dll...")
#pragma message("======== Successfully linked with PCI2324_64.dll")
#endif

#endif

#endif // _PCI2324_DEVICE_
