#if !defined(EM9703_H)
#define EM9703_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZT_Type.h"


#define EM9703_USB_MAX_COUNT                        16 //最大可支持USB模块个数
#define EM9703_BASE_FREQ                            50000000.0//基础时钟，50M
#define EM9703_PWM_BASE_FREQ                        10000000.0//PWM基础时钟，10M
#define EM9703_MAXIOCHCNT                           64 //IO最大通道数
#define EM9703_MAXCTCHCNT                           16 //计数器最大通道数
#define EM9703_MAXECCHCNT                           4 //编码器最大通道数
#define EM9703_MAXPWMCHCNT                          8 //PWM最大通道数

//启动方式
#define EM9703_STARTMode_ONCE                       0 //单次，EM9703会每100ms启动一次转换，用户可以读回最近一次的转换结果
#define EM9703_STARTMode_ICLK                       1 //内时钟启动采集
#define EM9703_STARTMode_ECLK                       2 //外时钟启动采集

//计数器工作方式
#define EM9703_CT_MODE_COUNT                         0 //计数
#define EM9703_CT_MODE_HFREQ                         1 //测高频
#define EM9703_CT_MODE_LFREQ                         3 //测低频

//IO方向
#define EM9703_IO_IN                                 0 //输入
#define EM9703_IO_OUT                                1 //输出

extern "C"
{
	//创建一个设备
	HANDLE _stdcall EM9703_CreateDevice();
	//关闭设备，同时释放设备占用的内存资源
	void _stdcall EM9703_CloseDevice(HANDLE pDev);
	//得到错误号
	I32 _stdcall EM9703_GetErrorCode(HANDLE pDev);
	//连接命令端口
	I32 _stdcall EM9703_CmdConnect( HANDLE pDev, char* strIP, int port );
	//关闭命令端口
	void _stdcall EM9703_CmdClose( HANDLE pDev );
	//使能通讯看门狗,当下位机在overTimeS秒内没有接收到任何命令将会视为通讯中断，此时上位机需要重新调用EM9703_CmdConnect函数。
	I32 _stdcall EM9703_CmdEnableWTD( HANDLE pDev, U16 overTimeS );
	//连接数据端口
	I32 _stdcall EM9703_DataConnect( HANDLE pDev, int port );
	//关闭数据端口
	void _stdcall EM9703_DataClose( HANDLE pDev );
	
	//设定定时采集频率
	I32 _stdcall EM9703_HCSetFreq( HANDLE pDev,  F64 groupFreq, F64* realFreq );
	//启动采集（定时、外触发）
	I32 _stdcall EM9703_HCStart( HANDLE pDev, I32 startMode );
	//停止采集（定时、外触发）
	I32 _stdcall EM9703_HCStop( HANDLE pDev );
	//从数据端口读取下位机的定时采集数据
	I32 _stdcall EM9703_HCReadCode( HANDLE pDev, I32 dataCount, U8* dataBuffer, I32 outtime = 500 );
	
	//设置开关量方向，注意此函数对EM9703N无效
	I32 _stdcall EM9703_IoSetDir( HANDLE pDev, I8 dir[2] );
	//得到所有开关量的状态，注意iStatus[0]~iStatus[31]对应DI1~DI32,iStatus[32]~iStatus[63]对应DO1~DO32,
	I32 _stdcall EM9703_IoGetAll( HANDLE pDev, I8 iStatus[EM9703_MAXIOCHCNT] );
	//设置所有开关量的状态，注意oStatus[0]~oStatus[31]对应DI1~DI32,oStatus[32]~oStatus[63]对应DO1~DO32,
	I32 _stdcall EM9703_IoSetAll( HANDLE pDev, I8 oStatus[EM9703_MAXIOCHCNT] );
	//设置下位机上电时初值，注意oStatus[0]~oStatus[31]对应DI1~DI32,oStatus[32]~oStatus[63]对应DO1~DO32
	I32 _stdcall EM9703_IoSetInitStatus( HANDLE pDev, I8 initStatus[EM9703_MAXIOCHCNT] );
	//得到下位机上电时初值，注意oStatus[0]~oStatus[31]对应DI1~DI32,oStatus[32]~oStatus[63]对应DO1~DO32
	I32 _stdcall EM9703_IoGetInitStatus( HANDLE pDev, I8 initStatus[EM9703_MAXIOCHCNT] );
	
	//计数器工作方式
	I32 _stdcall EM9703_CtSetModeAll( HANDLE pDev, I8 ctMode[EM9703_MAXCTCHCNT] );
	//清零计数器
	I32 _stdcall EM9703_CtClear( HANDLE pDev, I32 chNo );
	//清零所有计数器
	I32 _stdcall EM9703_CtClearAll( HANDLE pDev );
	//设置测频基准，以ms为单位，只有当计数器工作在测频方式下时才有用
	I32 _stdcall EM9703_CtSetFreqBase( HANDLE pDev, I32 chNo, F64 freqBase_ms, F64* real_ms );
	//读回所有通道的计数器当前值
	I32 _stdcall EM9703_CtReadCodeAll( HANDLE pDev, U32 ctCode[EM9703_MAXCTCHCNT] );
	
	//读回所有通道的编码器值
	I32 _stdcall EM9703_EcReadAll( HANDLE pDev, I32 encoderAB[EM9703_MAXECCHCNT], I32 encoderZ[EM9703_MAXECCHCNT] );
	//编码器清零，isClear[0]~isClear[3]对应编码器1~编码器4,0表示不清零，1表示清零
	I32 _stdcall EM9703_EcClearAll( HANDLE pDev, I8 isClear[EM9703_MAXECCHCNT] );
	
	//设置指定通道的PWM脉冲输出到个数，如果设置为0，则表示启动PWM后一直输出。
	I32 _stdcall EM9703_PwmSetCount( HANDLE pDev, I32 chNo, U32 setCount );
	//PWM脉冲是否输出到指定个数
	I32 _stdcall EM9703_PwmIsOver( HANDLE pDev, I8 isOver[EM9703_MAXPWMCHCNT] );
	//设置PWM脉冲的周期和占空比
	I32 _stdcall EM9703_PwmSetPulse( HANDLE pDev, I32 chNo, double freq, double dutyCycle, double* realFreq, double* realDutyCycle );
	//启动或者停止PWM输出，is90表示相应的通道是否延时90°
	I32 _stdcall EM9703_PwmStartAll( HANDLE pDev, I8 startOrStop[EM9703_MAXPWMCHCNT], I8 is90[EM9703_MAXPWMCHCNT] );
	
	//得到上位机USB虚拟网卡的配置
	I32 _stdcall EM9703_GetUSBStatus( char usbName[][256], unsigned char ip[][4], long ipCount );
	//得到上位机USB虚拟网卡的配置
	I32 _stdcall EM9703_SetUSBHostIp( long devInx, char* ip );
	//设置上位机USB虚拟网卡的IP地址
	I32 _stdcall EM9703_SetClientIP( HANDLE pDev, const char* strIP );
};

#endif // !defined(EM9703_H)
