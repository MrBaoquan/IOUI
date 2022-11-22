#ifndef _PCI2310_DEVICE_
#define _PCI2310_DEVICE_

//######################## 常量定义 #################################
// CreateFileObject所用的文件操作方式控制字(可通过或指令实现多种方式并操作)
#define PCI2310_modeRead				0x0000		// 只读文件方式
#define PCI2310_modeWrite				0x0001		// 只写文件方式
#define	PCI2310_modeReadWrite			0x0002		// 既读又写文件方式
#define PCI2310_modeCreate				0x1000		// 如果文件不存可以创建该文件，如果存在，则重建此文件，并清0
#define PCI2310_typeText				0x4000		// 以文本方式操作文件

//***********************************************************

//***********************************************************
// 用户函数接口
#ifndef _PCI2310_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## 常规通用函数 #################################
	// 适用于本设备的最基本操作
	HANDLE DEVAPI FAR PASCAL PCI2310_CreateDevice(int DeviceID = 0);		// 创建设备对象
	int DEVAPI FAR PASCAL PCI2310_GetDeviceCount(HANDLE hDevice);			// 取得设备总台数
	int DEVAPI FAR PASCAL PCI2310_GetDeviceCurrentID(HANDLE hDevice);
	BOOL DEVAPI FAR PASCAL PCI2310_ListDeviceDlg(HANDLE hDevice);			// 列表系统当中的所有的该PCI设备
    BOOL DEVAPI FAR PASCAL PCI2310_ReleaseDevice(HANDLE hDevice);			// 关闭设备,禁止传输,且释放资源

	//####################### 数字I/O输入输出函数 #################################
	// 用户可以使用WriteRegisterULong和ReadRegisterULong等函数直接控制寄存器进行I/O
	// 输入输出，但使用下面两个函数更省事，它不需要您关心寄存器分配和位操作等，而只
	// 需象VB等语言的属性操作那么简单地实现各开关量通道的控制。
	BOOL DEVAPI FAR PASCAL PCI2310_SetDeviceDO(             // 设置数字量输出状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDOSts[32]);	// 开关状态

	BOOL DEVAPI FAR PASCAL PCI2310_GetDeviceDI(             // 取得数字量输入状态     
										HANDLE hDevice,     // 设备句柄								        
										BYTE bDISts[32]);	// 开关状态	

	//####################### 中断函数 #################################
	// 它由硬件信号的状态变化引起CPU产生中断事件hEventInt。
	BOOL DEVAPI FAR PASCAL PCI2310_InitDeviceInt(			// 初始化中断
										HANDLE hDevice,     // 设备句柄	
										HANDLE hEventInt);	// 中断事件句柄,它由CreateSystemEvent创建
	LONG DEVAPI FAR PASCAL PCI2310_GetDeviceIntCount(HANDLE hDevice);  // 在中断初始化后，用它取得中断服务程序产生的次数
	BOOL DEVAPI FAR PASCAL PCI2310_ReleaseDeviceInt(HANDLE hDevice); // 释放中断资源

	//################# 内存映射寄存器直接操作及读写函数 ########################
	// 适用于用户对本设备更直接、更特殊、更低层、更复杂的控制。比如根据特殊的
	// 控制对象需要特殊的控制流程和控制效率时，则用户可以使用这些接口予以实现。
	BOOL DEVAPI FAR PASCAL PCI2310_GetDeviceAddr(			// 取得指定的指定设备寄存器组的线性基地址和物理地址
									HANDLE hDevice,			// 设备对象句柄,它由CreateDevice函数创建
									PUCHAR* LinearAddr,		// 返回指定寄存器组的线性地址
									PUCHAR* PhysAddr,		// 返回指定寄存器组的物理地址
									int RegisterID = 0);	// 设备寄存器组的ID号（0-5）

    BOOL DEVAPI FAR PASCAL PCI2310_WritePortByte(HANDLE hDevice, PUCHAR pbPort, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI2310_WritePortWord(HANDLE hDevice, PUCHAR pbPort, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI2310_WritePortULong(HANDLE hDevice, PUCHAR pbPort, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI2310_ReadPortByte(HANDLE hDevice, PUCHAR pbPort);
    WORD DEVAPI FAR PASCAL PCI2310_ReadPortWord(HANDLE hDevice, PUCHAR pbPort);
    ULONG DEVAPI FAR PASCAL PCI2310_ReadPortULong(HANDLE hDevice, PUCHAR pbPort);

	//########################### 线程操作函数 ######################################
	HANDLE DEVAPI FAR PASCAL PCI2310_CreateSystemEvent(void);			// 创建内核事件对象，供InitDeviceInt和VB子线程等函数使用
	BOOL DEVAPI FAR PASCAL PCI2310_ReleaseSystemEvent(HANDLE hEvent);	// 释放内核事件对象


#ifdef __cplusplus
}
#endif

// 自动包含驱动函数导入库
#ifndef _PCI2310_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI2310_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2310_32.dll...")
		#pragma message("======== Successfully linked with PCI2310_32.dll")
	#else
		#pragma comment(lib, "PCI2310_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2310_64.dll...")
		#pragma message("======== Successfully linked with PCI2310_64.dll")
	#endif

#endif

#endif // _PCI2310_DEVICE_
