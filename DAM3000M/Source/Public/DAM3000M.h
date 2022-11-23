﻿#ifndef  _ART_DAM3000M_SERIAL_
#define _ART_DAM3000M_SERIAL_

#include <windows.h>

// ********************* 开关量输出的参数结构 ****************************
typedef struct _DAM3000M_PARA_DO			// 数字量输出参数
{
	BYTE DO0;       	// 0通道
	BYTE DO1;       	// 1通道
	BYTE DO2;       	// 2通道
	BYTE DO3;       	// 3通道
	BYTE DO4;       	// 4通道
	BYTE DO5;       	// 5通道
	BYTE DO6;       	// 6通道
	BYTE DO7;       	// 7通道
	BYTE DO8;			// 8通道
	BYTE DO9;			// 9通道
	BYTE DO10;      	// 10通道
	BYTE DO11;      	// 11通道
	BYTE DO12;      	// 12通道
	BYTE DO13;			// 13通道
	BYTE DO14;			// 14通道
	BYTE DO15;			// 15通道
} DAM3000M_PARA_DO, *PDAM3000M_PARA_DO;


// ********************* 开关量输入的参数结构 *******************************
typedef struct  _DAM3000M_PARA_DI		// 数字量输入参数(1为高电平)
{
	BYTE DI0;			// 0通道
	BYTE DI1;			// 1通道
	BYTE DI2;			// 2通道
	BYTE DI3;			// 3通道
	BYTE DI4;			// 4通道
	BYTE DI5;			// 5通道
	BYTE DI6;			// 6通道
	BYTE DI7;			// 7通道
	BYTE DI8;			// 8通道
	BYTE DI9;			// 9通道
	BYTE DI10;			// 10通道
	BYTE DI11;			// 11通道
	BYTE DI12;			// 12通道
	BYTE DI13;			// 13通道
	BYTE DI14;			// 14通道
	BYTE DI15;			// 15通道
	BYTE DI16;			// 16通道
	BYTE DI17;			// 17通道
	BYTE DI18;			// 18通道
	BYTE DI19;			// 19通道
	BYTE DI20;			// 20通道
	BYTE DI21;			// 21通道
	BYTE DI22;			// 22通道
	BYTE DI23;			// 23通道
	BYTE DI24;			// 24通道
	BYTE DI25;			// 25通道
	BYTE DI26;			// 26通道
	BYTE DI27;			// 27通道
	BYTE DI28;			// 28通道
	BYTE DI29;			// 29通道
	BYTE DI30;			// 30通道
	BYTE DI31;			// 31通道
} DAM3000M_PARA_DI, *PDAM3000M_PARA_DI;

typedef struct  _DAM3000M_PARA_LATCH      // 数字量锁存参数(1为锁存)
{
	BYTE Latch0;		// 0通道
	BYTE Latch1;        // 1通道
	BYTE Latch2;        // 2通道
	BYTE Latch3;        // 3通道
	BYTE Latch4;        // 4通道
	BYTE Latch5;        // 5通道
	BYTE Latch6;        // 6通道
	BYTE Latch7;        // 7通道
	BYTE Latch8;        // 8通道
	BYTE Latch9;        // 9通道
	BYTE Latch10;       // 10通道
	BYTE Latch11;       // 11通道
	BYTE Latch12;       // 12通道
	BYTE Latch13;       // 13通道
	BYTE Latch14;       // 14通道
	BYTE Latch15;       // 15通道
	BYTE Latch16;       // 16通道
	BYTE Latch17;       // 17通道
	BYTE Latch18;       // 18通道
	BYTE Latch19;       // 19通道
	BYTE Latch20;       // 20通道
	BYTE Latch21;       // 21通道
	BYTE Latch22;       // 22通道
	BYTE Latch23;       // 23通道
	BYTE Latch24;       // 24通道
	BYTE Latch25;       // 25通道
	BYTE Latch26;       // 26通道
	BYTE Latch27;       // 27通道
	BYTE Latch28;       // 28通道
	BYTE Latch29;       // 29通道
	BYTE Latch30;       // 30通道
	BYTE Latch31;       // 31通道
} DAM3000M_PARA_LATCH, *PDAM3000M_PARA_LATCH;


// ****************** 模拟量输入通道配置结构体 *************************
typedef struct _DAM3000M_ADCHANNEL_ARRAY
{
	BYTE bChannelEnable[16];	// [0~15]分别对应channel0~channel15    1，有效；0，无效
}DAM3000M_ADCHANNEL_ARRAY, *PDAM3000M_ADCHANNEL_ARRAY;


// ****************** 计数器参数配置结构体 ******************************
typedef struct _DAM3000M_PARA_CNT			// 基于各通道的计数器参数结构体
{
	LONG WorkMode;			// 计数器/频率工作模式
	LONG FreqBuildTime;		// 测频器建立时间, 单位: s
	LONG InputMode;			// 计数器/频率输入方式	0: 非隔离	1: 隔离
	ULONG InitVal;			// 计数器初始值
	ULONG MaxVal;			// 计数器最大值
	LONG GateSts;			// 门槛值状态(计数模式)
} DAM3000M_PARA_CNT, *PDAM3000M_PARA_CNT;

typedef struct _DAM3000M_CNT_ALARM
{
	LONG AlarmMode;			// 报警方式	
	LONG EnableAlarm0;		// 0通道报警使能
	LONG EnableAlarm1;		// 1通道报警使能
	ULONG Alarm0Val;		// 0通道报警值
	ULONG Alarm1Val;		// 1通道报警值
	ULONG Alarm0HiHiVal;	// 0通道上上限(Hi-Hi)报警值, 报警方式1有效
} DAM3000M_CNT_ALARM, *PDAM3000M_CNT_ALARM;

typedef struct _DAM3000M_PARA_FILTER		// 用于计数器滤波的参数结构体
{
	LONG TrigLevelHigh;		// 触发高电平(非隔离输入)
	LONG TrigLevelLow;		// 触发低电平(非隔离输入)
	LONG MinWidthHigh;		// 高电平最小输入信号宽度
	LONG MinWidthLow;		// 低电平最小输入信号宽度
	LONG bEnableFilter;		// 使能滤波
} DAM3000M_PARA_FILTER, *PDAM3000M_PARA_FILTER;
//	LONG DisplayChannel;	// 设置显示通道		0：0通道计数/频率，1：1通道计数/频率


typedef struct _DAM3000M_CNT_STATUS			// 计数器硬件参数状态结构体
{
	LONG WorkMode;			// 计数器/频率工作模式*
	LONG FreqBuildTime;		// 测频器建立时间, 单位: s*
	LONG InputMode;			// 计数器/频率输入方式	0: 非隔离	1: 隔离*
	LONG bCNTSts;			// 计数/频率器的状态(起停状态)*
	LONG FilterSts;			// 计数器的滤波状态*
	LONG MinWidthHigh;		// 高电平最小输入信号宽度*
	LONG MinWidthLow;		// 低电平最小输入信号宽度*
	LONG TrigLevelHigh;		// 触发高电平(非隔离输入)*
	LONG TrigLevelLow;		// 触发低电平(非隔离输入)*
	LONG GateSts;			// 门槛值设置状态(计数模式)*
	ULONG MaxVal;			// 计数器最大值*
	ULONG InitVal;			// 计数器初始值*
	LONG bOverflowSts;		// 计数器溢出状态*
	LONG AlarmMode;			// 计数器报警方式*
	LONG EnableAlarm0;		// 计数器0报警使能状态*
	LONG EnableAlarm1;		// 计数器1报警使能状态*
	ULONG Alarm0Val;		// 0通道报警值*
	ULONG Alarm1Val;		// 1通道报警值*
	ULONG Alarm1HiHiVal;	// 报警方式1上上限(Hi-Hi)报警值*
	LONG bDO0;				// DO0*
	LONG bDO1;				// DO1*
} DAM3000M_CNT_STATUS, *PDAM3000M_CNT_STATUS;


// ****************** 设备基本信息的结构体 ******************************
typedef struct _DAM3000M_DEVICE_INFO
{
	LONG    DeviceType;		// 模块类型 
	LONG    TypeSuffix;		// 类型后缀
	LONG	ModusType;		// M
	LONG	VesionID;		// 版本号(2字节)
	LONG	DeviceID;		// 模块ID号(SetDeviceInfo时，为设备的新ID)
	LONG	BaudRate;		// 波特率
	LONG	bParity;		// 0:无校验 1:偶校验 2:奇校验(只有这3个值才能表示该模块有可能支持此功能，设置时此值不为0 1 2表示不设置此参数)
} DAM3000M_DEVICE_INFO, *PDAM3000M_DEVICE_INFO;


// ****************** 测温模块传感器参数的结构体 ******************************
typedef struct _DAM3000M_SENSOR_PARA
{
	BYTE	SerialNumber;	// 编号
	BYTE	Channel;		// 所在通道号
	BYTE	SequenceNumber;	// 通道内顺序号
	WORD	Temperature;	// 温度
	BYTE	ID[8];			// ID号
	BYTE	AlarmMark;		// 报警标志
} DAM3000M_SENSOR_PARA, *PDAM3000M_SENSOR_PARA;

// FOR DAM3090 ******************************
typedef struct _DAM3000M_ADTEST_PARA
{
	BYTE	PowerFre;	    // 电源频率(0:50Hz；1:60Hz)
	BYTE	TimeCon;		// 数字滤波时间常数(0~20)
	BYTE	CoSelect;	    // 冷端选择(0：固定值；1：内部冷端传感器；2：外部通道)
	WORD	CoValue;	    // 冷端固定值(0~60)
	BYTE	ChNum;			// 冷端通道号(0~5)
	WORD	ModVlaue;		// 冷端补偿修正值(5000~15000)
	BYTE	AdMode;		    // 测量模式(0：普通模式；1：慢速模式)
} DAM3000M_ADTEST_PARA, *PDAM3000M_ADTEST_PARA;


// 模拟量输入类型(电压类型) 供DAM3000M_SetModeAD函数中的lMode参数使用
#define DAM3000M_VOLT_N15_P15					0x01 //  -15～+15mV
#define DAM3000M_VOLT_N50_P50					0x02 //  -50～+50mV
#define DAM3000M_VOLT_N100_P100					0x03 // -100～+100mV
#define DAM3000M_VOLT_N150_P150					0x04 // -150～+150mV
#define DAM3000M_VOLT_N500_P500					0x05 // -500～+500mV
#define DAM3000M_VOLT_N1_P1						0x06 //   -1～+1V
#define DAM3000M_VOLT_N0_P1						0x89 //   0～+1V
#define DAM3000M_VOLT_N25_P25					0x07 // -2.5～+2.5V
#define DAM3000M_VOLT_N5_P5						0x08 //   -5～+5V
#define DAM3000M_VOLT_N10_P10					0x09 //  -10～+10V
#define DAM3000M_VOLT_N0_P5						0x0D //    0～+5V
#define DAM3000M_VOLT_N0_P10					0x0E //    0～+10V
#define DAM3000M_VOLT_N0_P25					0x0F //    0～+2.5V
#define DAM3000M_VOLT_N20_P20					0x81 //    -20～+20mV
#define DAM3000M_VOLT_N1_P5						0x82 //    1～+5V
#define DAM3000M_VOLT_N30_P30V					0x83 //  -30～+30V
#define DAM3000M_VOLT_N0_P30V					0x84 //    0～+30V
#define DAM3000M_VOLT_N500_P500V				0x8A // -500～+500V
#define DAM3000M_VOLT_N30_P30					0x8B // -30mV～+30mV
#define DAM3000M_VOLT_N0_P12					0x8C // 0～12V
#define DAM3000M_VOLT_N0_P20					0x8D // 0～20V
#define DAM3000M_VOLT_N0_P15					0x55 // 0～+15V（同研华协议下0～+15V）

#define DAM3000M_VOLT_YH_N10_P10				0x08 // 研华协议下-10～+10V
#define DAM3000M_VOLT_YH_N5_P5					0x09 // 研华协议下-5～+5V
#define DAM3000M_VOLT_YH_N1_P1					0x0A // 研华协议下-1～+1V
#define DAM3000M_VOLT_YH_N500_P500V				0x0B // 研华协议下-500～+500mV
#define DAM3000M_VOLT_YH_N150_P150V				0x0C // 研华协议下-150～+150mV
#define DAM3000M_VOLT_YH_N0_P10					0x48 // 研华协议下0～+10V
#define DAM3000M_VOLT_YH_N0_P5					0x49 // 研华协议下0～+5V
#define DAM3000M_VOLT_YH_N0_P1					0x4A // 研华协议下0～+1V
#define DAM3000M_VOLT_YH_N15_P15				0x15 // 研华协议下-15～+15V
#define DAM3000M_VOLT_YH_N0_P15					0x55 // 研华协议下0～+15V

// 模拟量输入类型(电流类型) 供DAM3000M_SetModeAD函数中的lMode参数使用
#define DAM3000M_CUR_N0_P10						0x00 //   0～10mA
#define DAM3000M_CUR_N20_P20					0x0A // -20～+20mA
#define DAM3000M_CUR_N0_P20						0x0B //   0～20mA
#define DAM3000M_CUR_N4_P20						0x0C //   4～20mA
#define DAM3000M_CUR_N0_P22						0x80 //   0～22mA
#define DAM3000M_CUR_N10_P10A					0x85 // -10～+10A
#define DAM3000M_CUR_N0_P10A					0x86 //   0～+10A
#define DAM3000M_CUR_N50_P50A					0x87 // -50～+50A
#define DAM3000M_CUR_N0_P50A					0x88 //   0～+50A
#define DAM3000M_CUR_YH_N0_P20A					0x4D // 研华协议下0～20mA
#define DAM3000M_CUR_YH_N4_P20A					0x07 // 研华协议下4～20mA
#define DAM3000M_CUR_YH_N20_P20A				0x0D // 研华协议下-20～20mA

// 模拟量输入类型(电流类型) 供DAM3000M_SetModeAD函数中的lMode参数使用
#define DAM3000M_R_N0_R400Ω					0x88 // 远传压力表电阻   0～400Ω

// 模拟量输入类型(热电偶类型) 供DAM3000M_SetModeAD函数中的lMode参数使用
#define DAM3000M_TMC_J							0x10 // J型热电偶   0～1200℃
#define DAM3000M_TMC_K							0x11 // K型热电偶   0～1300℃
#define DAM3000M_TMC_T							0x12 // T型热电偶 -200～400℃
#define DAM3000M_TMC_E							0x13 // E型热电偶   0～1000℃
#define DAM3000M_TMC_R							0x14 // R型热电偶 0～1700℃
#define DAM3000M_TMC_S							0x15 // S型热电偶 0～1768℃
#define DAM3000M_TMC_B							0x16 // B型热电偶 0～1800℃
#define DAM3000M_TMC_N							0x17 // N型热电偶   0～1300℃
#define DAM3000M_TMC_C							0x18 // C型热电偶   0～2090℃
#define DAM3000M_TMC_WRE						0x19 // 钨铼5-钨铼26 0～2310℃
#define DAM3000M_TMC_K_EX						0x70 // K型热电偶   -40～1300℃
#define DAM3000M_TMC_B_N						0x71 // B型热电偶 250～1800℃

// 模拟量输入类型(热电阻类型) 供DAM3000M_SetModeAD函数中的lMode参数使用
#define DAM3000M_RTD_PT100_385_N200_P850		0x20 // Pt100(385)热电阻 -200℃～850℃
#define DAM3000M_RTD_PT100_385_N100_P100		0x21 // Pt100(385)热电阻 -100℃～100℃
#define DAM3000M_RTD_PT100_385_N0_P100			0x22 // Pt100(385)热电阻    0℃～100℃
#define DAM3000M_RTD_PT100_385_N0_P200			0x23 // Pt100(385)热电阻    0℃～200℃
#define DAM3000M_RTD_PT100_385_N0_P600			0x24 // Pt100(385)热电阻    0℃～600℃
#define DAM3000M_RTD_PT100_3916_N200_P850		0x25 // Pt100(3916)热电阻-200℃～850℃
#define DAM3000M_RTD_PT100_3916_N100_P100		0x26 // Pt100(3916)热电阻-100℃～100℃
#define DAM3000M_RTD_PT100_3916_N0_P100			0x27 // Pt100(3916)热电阻   0℃～100℃
#define DAM3000M_RTD_PT100_3916_N0_P200			0x28 // Pt100(3916)热电阻   0℃～200℃
#define DAM3000M_RTD_PT100_3916_N0_P600			0x29 // Pt100(3916)热电阻   0℃～600℃
#define DAM3000M_RTD_PT1000						0x30 // Pt1000热电阻     -200℃～850℃
#define DAM3000M_RTD_CU50						0x40 // Cu50热电阻        -50℃～150℃
#define DAM3000M_RTD_CU100						0x41 // Cu100热电阻       -50℃～150℃
#define DAM3000M_RTD_BA1						0x42 // BA1热电阻        -200℃～650℃
#define DAM3000M_RTD_BA2						0x43 // BA2热电阻        -200℃～650℃
#define DAM3000M_RTD_G53						0x44 // G53热电阻         -50℃～150℃
#define DAM3000M_RTD_Ni50						0x45 // Ni50热电阻        100℃
#define DAM3000M_RTD_Ni508						0x46 // Ni508热电阻         0℃～100℃
#define DAM3000M_RTD_Ni1000						0x47 // Ni1000热电阻      -60℃～160℃
#define DAM3000M_RTD_103AT						0x60 // 103AT电阻		  -50℃～110℃

// 模块量输出斜率类型	供DAM3000M_SetModeDA函数中的参数 lType 使用
#define DAM3000M_SLOPE_IMMEDIATE				0x00 // Immediate
#define DAM3000M_SLOPE_POINT125					0x01 // 0.125 mA/S
#define DAM3000M_SLOPE_POINT25					0x02 // 0.25  mA/S
#define DAM3000M_SLOPE_POINT5					0x03 // 0.5  mA/S
#define DAM3000M_SLOPE_1						0x04 // 1.0  mA/S
#define DAM3000M_SLOPE_2						0x05 // 2.0  mA/S
#define DAM3000M_SLOPE_4						0x06 // 4.0  mA/S
#define DAM3000M_SLOPE_8						0x07 // 8.0  mA/S
#define DAM3000M_SLOPE_16						0x08 // 16.0  mA/S
#define DAM3000M_SLOPE_32						0x09 // 32.0  mA/S
#define DAM3000M_SLOPE_64						0x0A // 64.0  mA/S
#define DAM3000M_SLOPE_128						0x0B // 128.0  mA/S
#define DAM3000M_SLOPE_256						0x0C // 256.0  mA/S
#define DAM3000M_SLOPE_512						0x0D // 512.0  mA/S
#define DAM3000M_SLOPE_1024						0x0E // 1024.0  mA/S
#define DAM3000M_SLOPE_2048						0x0F // 2048.0  mA/S

// DI计数方式 供DAM3000M_SetDeviceMode函数中的lMode参数使用
#define DAM3000M_DI_MODE_DI						0x00 // DI方式
#define DAM3000M_DI_MODE_COUNT					0x01 // 计数方式
#define DAM3000M_DI_MODE_LATCH					0x02 // 锁存方式

// DI计数方式 供DAM3000M_SetDeviceMode函数中的lEdgeMode参数使用
#define DAM3000M_DIR_FALLING					0x00 // 下降沿
#define DAM3000M_DIR_RISING						0x01 // 上升沿

//########################## 计数器 ###################################
// 模块的工作模式 供DAM3000M_SetDevWorkMode函数中的lMode参数使用
#define DAM3000M_WORKMODE_CNT					0x00 // 计数器
#define DAM3000M_WORKMODE_FREQ					0x01 // 频率器

// 计数器/频率的输入方式 供DAM3000M_PARA_CNT结构体中的lInputMode参数使用
#define DAM3000M_UNISOLATED						0x00 // 非隔离
#define DAM3000M_ISOLATED						0x01 // 隔离

// 门槛值状态 供DAM3000M_PARA_CNT结构体中的GateSts参数使用
#define DAM3000M_GATE_LOW						0x00 // 门槛值为低电平
#define DAM3000M_GATE_HIGH						0x01 // 门槛值为高电平
#define DAM3000M_GATE_NULL						0x02 // 门槛值无效

// 报警方式 供DAM3000M_CNT_ALARM结构体中的AlarmMode参数使用
#define CNT_ALARM_MODE0							0x00 // 报警方式0	0通道-1通道上限
#define CNT_ALARM_MODE1							0x01 // 报警方式1	0通道上限 / 上上限

// 报警方式0使能 供DAM3000M_CNT_ALARM结构体中的EnableAlarm0 和 EnableAlarm1参数使用
#define CNT_ALAMODE0_DISABLE					0x00 // 报警方式0禁止报警
#define CNT_ALAMODE0_ENABLE						0x01 // 报警方式0允许报警

// 报警方式1使能 供DAM3000M_CNT_ALARM结构体中的EnableAlarm0参数使用
#define CNT_ALAMODE1_DISABLE					0x00 // 报警方式1 计数器0 禁止报警
#define CNT_ALAMODE1_INSTANT					0x01 // 报警方式1 计数器0 瞬间报警允许
#define CNT_ALAMODE1_LATCH						0x02 // 报警方式1 计数器0 闭锁报警允许

// 滤波状态使能 供DAM3000M_PARA_FILTER结构体中的bEnableFilter参数使用
#define DAM3000M_FILTER_DISABLE					0x00 // 禁止滤波
#define DAM3000M_FILTER_ENABLE					0x01 // 允许滤波
//-----------------------------------------------------

//########################## 电量 ###################################
#define DAM3000M_GET_I_RMS						0x00 // 获得电流有效值
#define DAM3000M_GET_V_RMS						0x01 // 获得电压有效值
#define DAM3000M_GET_PHVOLTAGE					0x01 // 获得电压有效值、相电压
#define DAM3000M_GET_POWER						0x02 // 获得有功功率
#define DAM3000M_GET_VAR						0x03 // 获得无功功率
#define DAM3000M_GET_VA							0x04 // 获得视在功率
#define DAM3000M_GET_WATTHR						0x05 // 获得正相有功电度
#define DAM3000M_GET_RWATTHR					0x06 // 获得反相有功电度
#define DAM3000M_GET_VARHR						0x07 // 获得正相无功电度
#define DAM3000M_GET_RVARHR						0x08 // 获得反相无功电度
#define DAM3000M_GET_PF							0x09 // 获得功率因数
#define DAM3000M_GET_FREQ						0x0A // 获得输入信号频率
#define DAM3000M_GET_VAWATTHR					0x0B // 获得电度
#define DAM3000M_GET_LINEVOLTAGE				0x0C // 获得线电压


// 数显表小数点位置  供DAM3000M_SetDecimalPoint和DAM3000M_GetDecimalPoint中使用###################################

#define DAM3000M_DECPOINT_ABit					0x00 // 小数点在个位
#define DAM3000M_DECPOINT_Ten					0x01 // 小数点在十位
#define DAM3000M_DECPOINT_Hundred				0x02 // 小数点在百位
#define DAM3000M_DECPOINT_Thousand				0x03 // 小数点在千位
#define DAM3000M_DECPOINT_NULL					0x04 // 无小数点


// 串口号(以此类推) 供DAM3000M_CreateDevice使用，可根据自身需要扩充
#define DAM3000M_COM1							0x01 // COM1
#define DAM3000M_COM2							0x02 // COM2
#define DAM3000M_COM3							0x03 // COM3
#define DAM3000M_COM4							0x04 // COM4
#define DAM3000M_COM5							0x05 // COM5

// 波特率选择 供DAM3000M_SetDeviceInfo和DAM3000M_GetDeviceInfo中的PDAM3000M_DEVICE_INFO使用
#define DAM3000M_BAUD_1200						0x00
#define DAM3000M_BAUD_2400						0x01
#define DAM3000M_BAUD_4800						0x02
#define DAM3000M_BAUD_9600						0x03
#define DAM3000M_BAUD_19200						0x04
#define DAM3000M_BAUD_38400						0x05
#define DAM3000M_BAUD_57600						0x06
#define DAM3000M_BAUD_115200					0x07


// 波特率选择 供DAM3000M_SetDeviceInfo和DAM3000M_GetDeviceInfo中的PDAM3000M_DEVICE_INFO(bParity)使用
#define DAM3000M_PARITY_NONE					0x00
#define DAM3000M_PARITY_EVEN					0x01
#define DAM3000M_PARITY_ODD						0x02

#define DAM3000M_DEFAULT_TIMEOUT                 -1

// 驱动函数接口
#ifndef DEFINING
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
//####################### 设备对象管理函数 #################################	
	//****************************************************************************	
	// 说明：创建设备对象
	// 参数：返回句柄，供与模块通信使用
	//****************************************************************************	
	HANDLE DEVAPI FAR PASCAL DAM3000M_CreateDevice(
								LONG lPortNum);			// 串口号

	//****************************************************************************	
	// 说明：初始化串口（初始与模块之间的通信参数）
	// 参数：波特率、奇偶校验、超时时间
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_InitDevice(						
									HANDLE	hDevice,	// 设备对象句柄
									LONG    lBaud,		// 波特率
									LONG    lParity,	// 校验方式
									LONG    lTimeOut = DAM3000M_DEFAULT_TIMEOUT);	// 超时时间，主要用于接收数据，如果为-1 则使用默认超时时间

	//****************************************************************************	
	// 说明：释放设备对象（串口），执行后无法操作串口
	// 参数：句柄
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReleaseDevice(HANDLE hDevice);		 

	//****************************************************************************	
	// 说明：重启设备
	// 功能码：0x10
	// 地址：0x206 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_RebootDevice( 
									HANDLE	hDevice,	// 设备对象句柄
									LONG	lDeviceID,	// 设备地址
									BOOL	bReboot);	// 重启设备标志 =TRUE 重启 =FALSE 无操作

	//****************************************************************************	
	// 说明：复位设置（恢复出厂设置）
	// 功能码：0x10
	// 地址：0x207 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_ResetDevice(						 
									HANDLE	hDevice,	// 设备对象句柄
									LONG	lDeviceID,	// 设备地址
									BOOL	bReset);	// 复位设备标志 =TRUE 复位 =FALSE 无操作

	//****************************************************************************	
	// 使模块进入(退出)校准模式（保留功能，不推荐用户使用）
	// 功能码：0x10
	// 地址：0x208 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_EnterCalibration( 
									HANDLE	hDevice,	// 设备对象句柄
									LONG	lDeviceID,	// 设备地址
									BOOL	bCalibrate);// 进入校准标志  =TRUE 进入校准模式 =FALSE 不操作

//################################### 模块信息取得/修改函数 ############################
	//****************************************************************************	
	// 读取模块信息(类型、地址、波特率、校验模式等)
	// 功能码：0x03
	// 地址：0x80 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetDeviceInfo(				
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PDAM3000M_DEVICE_INFO pInfo);	// 设备信息

	//****************************************************************************	
	// 修改模块信息(地址、波特率、校验)
	// 功能码：0x10
	// 地址：0x80 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetDeviceInfo(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									DAM3000M_DEVICE_INFO& Info);	// 设备信息
	
	//****************************************************************************	
	// 获取模块安全通信时间
	// 功能码：0x03
	// 地址：0x240 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetDevSafeTime(						
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PLONG	lSafeTime);				// 获取安全通信时间(0~65535)

	//****************************************************************************	
	// 设置模块安全通信时间
	// 功能码：0x10
	// 地址：0x240 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetDevSafeTime(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LONG	lSafeTime);				// 设置安全通信时间参数(0~65535)

	//****************************************************************************	
	// 获取模块采样率
	// 功能码：0x03
	// 地址：0x243
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetDevSampleRate(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PLONG	lSampleRate);			// 模块采样率 0=500Hz 1=1KHz 2=2KHz

	//****************************************************************************	
	// 设置模块采样率
	// 功能码：0x10
	// 地址：0x243
	// 采样率定义：
	//			DAM3911H 0=500Hz 1=1KHz 2=2KHz 
	//			DAM3159HB 4=10Hz 5=100Hz
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetDevSampleRate(					 
									HANDLE hDevice,					// 设备对象句柄
									LONG lDeviceID,					// 模块地址
									LONG lSampleRate);				// 模块采样率 

	//****************************************************************************	
	// 获取上传数据时间间隔
	// 功能码：0x03
	// 地址：0x221 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetUploadDataTime(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PWORD	pwUploadDataTime);		// 获取数据上传时间间隔

	//****************************************************************************	
	// 设置上传数据时间间隔
	// 功能码：0x10
	// 地址：0x221 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_SetUploadDataTime(					
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									WORD	wUploadDataTime);		// 设置数据上传时间间隔

	//****************************************************************************	
	// 设置测试参数(DAM3090)
	// 功能码：0x10
	// 地址：0xA0 ~ 0xA6 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetTestPara(	                    
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									PDAM3000M_ADTEST_PARA pTestPARA);	// 测试参数

	//****************************************************************************	
	// 设置测试参数(DAM3090)
	// 功能码：0x03
	// 地址：0xA0 ~ 0xA6 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetTestPara(						
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									PDAM3000M_ADTEST_PARA pTestPARA);	// 测试参数

	//****************************************************************************	
	// 获取模块通讯模式
	// 功能码：0x03
	// 地址：0x242 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCommunicationMode(				
									HANDLE	hDevice,					// 设备句柄
									LONG	lDeviceID,					// 模块地址
									PLONG	lMode);						// 通讯模式 0:RS485 1:RS232

	//****************************************************************************	
	// 设置模块通讯模式
	// 功能码：0x10
	// 地址：0x242
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCommunicationMode(				
										HANDLE hDevice,					// 设备句柄
										LONG lDeviceID,					// 模块地址
										LONG lMode);					// 通讯模式 0:RS485 1:RS232

	
	//####################### AD数据读取函数 ###################################	
	//****************************************************************************	
	// 读取AD模拟量输入(一个AD数据占2个地址(4个byte))
	// 功能码：0x04
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadDeviceAD(						
									HANDLE	hDevice,					// 设备对象句柄	
									LONG	lDeviceID,					// 模块地址
									LONG	lpADBuffer[],				// 接收AD数据的用户缓冲区 注意:lpADBuffer最好大于等于lLastChannel - lFirstChannel +1
									LONG	lFirstChannel = 0,			// 首通道
									LONG	lLastChannel = 0);			// 末通道

	//****************************************************************************	
	// 读取AD模拟量输入(一个AD数据占1个地址(2个byte)(地址连续))
	// 功能码：0x04
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadDeviceAD_WORD(
									HANDLE hDevice,				// 设备对象句柄	
									LONG   lDeviceID,			// 模块地址
									WORD   lADBuffer[],			// 接受AD数据的用户缓冲区 
									LONG   lFirstChannel = 0,	// 首通道
									LONG   lLastChannel = 0);	// 末通道

	//****************************************************************************	
	// 读取AD模拟量输入（连续地址：一个AD数据占1个地址(2个byte)）
	// 功能码：0x04
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadDeviceAD_WORD_Addr(
									HANDLE hDevice,				// 设备对象句柄	
									LONG   lDeviceID,			// 模块地址
									LONG   lAddr,				// 读取AD值首地址（AD0通道地址）
									WORD   lADBuffer[],			// 接受AD数据的用户缓冲区 
									LONG   lFirstChannel = 0,	// 首通道
									LONG   lLastChannel = 0);	// 末通道

	//****************************************************************************	
	// 获得模拟量输入模式（仅支持操作一个通道量程）
	// 功能码：0x03
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetModeAD(							  
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lDeviceID,			// 模块地址
									PLONG	lpMode,				// AD模式
									LONG	lChannel = 0);		// 通道号
	
	//****************************************************************************	
	// 获得模拟量输入模式（可支持同时操作多个通道量程）
	// 功能码：0x03
	// 地址：0x100
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetModeAD_Multi(
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lDeviceID,			// 模块地址
									LONG	lADMode[],			// AD模式（量程）
									LONG	lFirstChannel = 0,	// 首通道
									LONG	lLastChannel = 0);	// 末通道

	//****************************************************************************	
	// 获得模拟量输入模式（仅支持操作一个通道量程）
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetModeAD_Addr(
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lAddr,				// AD0通道对应地址
									LONG	lDeviceID,			// 模块地址
									PLONG	lpADMode,			// AD模式（量程）
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 设置AD输入模式（仅支持操作一个通道量程）
	// 功能码：0x10
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetModeAD(					  
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lDeviceID,			// 模块地址
									LONG	lMode,				// AD模式 
									LONG	lChannel = 0);		// 通道号
	
	//****************************************************************************	
	//设置模拟量输入模式（可支持同时操作多个通道量程）
	// 功能码：0x10
	// 地址：0x100
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetModeAD_Multi(
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lDeviceID,			// 模块地址
									LONG	lADMode[],			// AD模式（量程）
									LONG	lFirstChannel = 0,	// 首通道
									LONG	lLastChannel = 0);	// 末通道

	//****************************************************************************	
	// 设置AD输入模式（仅支持操作一个通道量程）
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetModeAD_Addr(
									HANDLE  hDevice,			// 设备对象句柄	
									LONG	lAddr,				// AD0通道对应地址
									LONG	lDeviceID,			// 模块地址
									LONG	lADMode,			// AD模式（量程）
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 获得通道接地模式(只对可软件配置单/双端输入模块有效)
	// 功能码：0x03
	// 地址：0xF8 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetGroundingAD(			 
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lDeviceID,			// 模块地址
									PLONG	lpGrounding,		// AD通道接地模式 0:双端 1:单端
									LONG	lChannel = 0);		// 通道号
	
	//****************************************************************************	
	// 设置通道接地模式(只对可软件配置单/双端输入模块有效)
	// 功能码：0x10
	// 地址：0x0F8 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetGroundingAD(			 
									HANDLE	hDevice,			// 设备对象句柄	
									LONG	lDeviceID,			// 模块地址
									LONG	lGrounding,			// AD通道接地模式 0:双端 1:单端
									LONG	lChannel = 0);		// 通道号
	
	//****************************************************************************	
	// 获得模拟量输入报警下限值（每通道占用2个地址(4字节);只支持1个通道）
	// 功能码：0x03
	// 地址：0x118 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetLowLimitVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lLowLimit[],		// 下限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 获得模拟量输入报警上限值（每通道占用2个地址(4字节);只支持1个通道）
	// 功能码：0x03
	// 地址：0x11A 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetHighLimitVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lHighLimit[],		// 上限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 设置下限报警值（每通道占用2个地址(4字节);只支持1个通道）
	// 功能码：0x10
	// 地址：0x118 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetLowLimitVal(				 
									HANDLE	hDevice,
									LONG	lDeviceID,			// 模块地址
									LONG	lLowVal[],			// 下限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号
		
	//****************************************************************************	
	// 设置上限报警值（每通道占用2个地址(4字节);只支持1个通道）
	// 功能码：0x10
	// 地址：0x11A 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetHighLimitVal(			 
									HANDLE	hDevice,
									LONG	lDeviceID,			// 模块地址
									LONG	lHighVal[],			// 上限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号
		
	//****************************************************************************	
	// 获得报警电平(使用1个地址;仅支持一个通道)
	// 功能码：0x03
	// 地址：0x11C 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetAlarmPulse(				 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									PLONG	lpAlarmVal,			// 报警电平,0:低电平, 1:高电平
									LONG	lChannel = 0);		// 通道号
	
	//****************************************************************************	
	// 设置报警电平(使用1个地址;仅支持一个通道)
	// 功能码：0x10
	// 地址：0x11C 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetAlarmPulse(				 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lAlarmVal,			// 报警电平,0:低电平, 1:高电平	
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 获得报警状态(使用1个地址;仅支持一个通道)
	// 功能码：0x03
	// 地址：0x12B 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetAlarmSts(				 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									PLONG	lpAlarmSts,			// 报警状态
									LONG	lChannel = 0);		// 读取通道

	//****************************************************************************	
	// 获得模拟量输入报警下限值（每通道使用1个地址;支持多通道）
	// 功能码：0x03
	// 地址：306 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetADLowLimitVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lLowLimit[],		// 下限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 设置下限报警值（每通道使用1个地址;支持多通道）
	// 功能码：0x10
	// 地址：306
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetADLowLimitVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lLowVal[],			// 下限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 获得模拟量输入报警上限值（每通道使用1个地址;支持多通道）
	// 功能码：0x03
	// 地址：289 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetADHighLimitVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lHighLimit[],		// 上限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 设置上限报警值（每通道使用1个地址;支持多通道）
	// 功能码：0x10
	// 地址：289
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetADHighLimitVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lHighVal[],			// 上限报警值
									LONG    lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号
	
	//****************************************************************************	
	// 获得多通道报警状态
	// 功能码：0x01
	// 地址：400 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetADAlarmSts(					 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lpAlarmSts[],		// 报警状态 0:未报警 1:报警
									LONG	lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 设置多通道报警状态
	// 功能码：0x0F
	// 地址：400
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetADAlarmSts(				 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lAlarmSts[],		// 报警状态 0:清除报警
									LONG	lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 获得当前报警模式
	// 功能码：0x04
	// 地址：600 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetADAlarmMode(  
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lAlarmMode[],		// 报警模式 0:不报警模式 1:锁存报警模式 2:实时报警模式
									LONG	lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 设置当前报警模式
	// 功能码：0x10
	// 地址：600
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetADAlarmMode(  
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lAlarmMode[],		// 报警模式 0:不报警模式 1:锁存报警模式 2:实时报警模式
									LONG	lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 回读模拟量输入通道使能状态
	// 功能码：0x01
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetDeviceADStatus(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lChannelCount,				// 通道数
									PDAM3000M_ADCHANNEL_ARRAY pADArray);// 通道使能状态 0:未使能 1:使能采集

	//****************************************************************************	
	// 设置模拟量输入通道使能状态
	// 功能码：0x0F
	// 地址： 0x100 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetDeviceADEanble(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									BYTE	byADEn[],					// AD使能状态 0:未使能 1:使能采集
									LONG    lFirstChannel = 0,			// 首通道号
									LONG	lLastChannel = 0);			// 末通道号

	//****************************************************************************	
	// 主动上传使能
	// 功能码：0x10
	// 地址：0x241
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_AD_AutoUpload(						 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									BOOL	bUpload);					// 主动上传 0:不主动上传 1:主动上传 


	//****************************************************************************	
	// 读取AD模拟量输入值（每通道1个地址（2字节））
	// 功能码：0x04
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_AMM_AD_ReadWORD(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									LONG	lpADBuffer[],				// 接收AD数据的用户缓冲区 注意数据存放格式，从lpADBuffer[lFirstChannel]开始连续存放返回的数据
									LONG	lFirstChannel = 0,			// 首通道
									LONG	lLastChannel = 0);			// 末通道

	//****************************************************************************	
	// 读取AD模拟量输入历史最大值和最小值（每通道1个地址（2字节））
	// 功能码：0x04
	// 地址：最大值0x20 最小值0x40 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_AMM_AD_ReadHistoryData(			 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									LONG	lpADBuffer[],				// 接收AD数据的用户缓冲区 注意数据存放格式，从lpADBuffer[lFirstChannel]开始连续存放返回的数据
									LONG	lMaxOrMin,					// 读取历史最大值还是最小值 1-最大值 2-最小值
									LONG	lFirstChannel = 0,			// 首通道
									LONG	lLastChannel = 0);			// 末通道

	//****************************************************************************	
	// 读取AD模拟量所有通道平均值（每通道1个地址（2字节））
	// 功能码：0x04
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_AMM_AD_ReadAverageData(			 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									PLONG	lAverage);					// 通道平均值

	//****************************************************************************	
	// 设置AD模拟量输入量程
	// 功能码：0x10
	// 地址：0xC8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_AMM_AD_SetInputRange(				 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									LONG	lMode,						// AD模式 
									LONG	lChannel = 0);				// 通道号 从0开始

	//****************************************************************************	
	// 获取AD模拟量输入量程
	// 功能码：0x04
	// 地址：0xC8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_AMM_AD_GetInputRange(				 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									PLONG	lpMode,						// AD模式
									LONG	lChannel = 0);				// 通道号 从0开始

	//****************************************************************************	
	// 设置AD模拟量输入通道使能状态
	// 功能码：0x10
	// 地址：0xDC 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_AMM_AD_SetChanEnableSts(			 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									BYTE	lADEn[],					// 设置AD使能状态 lADEn[lFirstChannel]对应lFirstChannel通道 lADEn[lLastChannel]对应lLastChannel通道
									LONG	lChannelCount);				// 通道个数

	//****************************************************************************	
	// 获取AD模拟量输入通道使能状态
	// 功能码：0x04
	// 地址：0xDC
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_AMM_AD_GetChanEnableSts(			 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									BYTE	lADEn[],					// 设置AD使能状态 lADEn[lFirstChannel]对应lFirstChannel通道 lADEn[lLastChannel]对应lLastChannel通道
									LONG	lFirstChannel = 0,			// 首通道号 从0开始
									LONG	lLastChannel = 0);			// 末通道号 从0开始

	//****************************************************************************	
	// 复位AD的历史最大值（最小值）
	// 功能码：0x10
	// 地址：最大值0x101  最小值0x102
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_AMM_AD_ResetHistoryData(			 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lMaxOrMin,					// 复位最大值还是最小值，1-最大值 2-最小值
									LONG	lFirstChannel = 0,			// 首通道 从0开始
									LONG	lLastChannel = 0);			// 末通道 从0开始


	//####################### DA数据读取函数 ###################################
	//****************************************************************************	
	// 回读DA输出值（每个通道占用2个地址）
	// 功能码：0x04
	// 地址：0x160 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetDeviceDAVal(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									PLONG	lpDAValue,          		// DA输出当前值
									LONG	lChannel = 0);				// 通道号
	//****************************************************************************	
	// 回读DA输出值（每个通道占用1个地址）
	// 功能码：0x04
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetDeviceDAVal_Addr(
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 自定义地址
									PLONG	lpDAValue,          		// DA输出当前值
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 设定单通道DA（每个通道占用2个地址）
	// 功能码：0x10
	// 地址：0x160 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_WriteDeviceDA(						 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lDAData,					// DA输出值
									LONG	lChannel = 0);      		// 通道号
	//****************************************************************************	
	// 设定单通道DA（每个通道占用1个地址）
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_WriteDeviceDA_Addr(				 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 通道0对应地址
									LONG	lDAData,					// DA输出值
									LONG	lChannel = 0);      		// 通道号
	
	//****************************************************************************	
	// 读取模拟量输出量程
	// 功能码：0x03
	// 地址：0x110 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetOutPutRangeDA(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									PLONG	lpRange,					// 输出量程
									LONG	lChannel = 0);				// 通道号
	//****************************************************************************	
	// 读取模拟量输出量程
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetOutPutRangeDA_Addr(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 通道0对应地址
									PLONG	lpRange,					// 输出量程
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 设置模拟量输出量程
	// 功能码：0x10
	// 地址：0x110 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetOutPutRangeDA(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lRange,						// 输出量程
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 设置模拟量输出类型
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetOutPutRangeDA_Addr( 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 自定义地址
									LONG	lRange,						// 输出量程
									LONG	lChannel = 0);				// 通道号
	
	//****************************************************************************	
	// 获得DA上电值（每通道占用2个地址）
	// 功能码：0x03
	// 地址：0x180 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetPowerOnValueDA(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									PLONG	lpPowerOnVal,       		// 上电值
									LONG	lChannel = 0);				// 通道号
	//****************************************************************************	
	// 获得DA上电值（每通道占用1个地址）
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL DAM3000M_GetPowerOnValueDA_Addr(
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 自定义通信地址
									PLONG	lpPowerOnVal,       		// 上电值
									LONG	lChannel = 0);				// 通道号
	//****************************************************************************	
	// 设置DA上电值（每通道占用2个地址）
	// 功能码：0x10
	// 地址：0x180 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetPowerOnValueDA(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lPowerOnVal,				// 上电值
									LONG	lChannel = 0);				// 通道号
	//****************************************************************************	
	// 设置DA上电值（每通道占用1个地址）
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetPowerOnValueDA_Addr(
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 通道0地址
									LONG	lPowerOnVal,				// 上电值
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 获得DA安全值（每通道占用2个地址）
	// 功能码：0x03
	// 地址：0x1A0 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetSafeValueDA(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									PLONG	lpSafeVal,					// 安全值
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 获得DA安全值（每通道占用1个地址）
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetSafeValueDA_Addr(
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 地址
									PLONG	lpSafeVal,					// 安全值
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 设置DA安全值（每通道占用2个地址）
	// 功能码：0x10
	// 地址：0x1A0
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetSafeValueDA(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lSafeVal,					// 安全值
									LONG	lChannel = 0);				// 通道号
	//****************************************************************************	
	// 设置DA安全值（每通道占用1个地址）
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetSafeValueDA_Addr(
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	Addr,						// 地址
									LONG	lSafeVal,					// 安全值
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 读模拟量输出斜率（每通道占用1个地址）
	// 功能码：0x03
	// 地址：0x1C0 
	// 斜率定义：0:Immediate 1:0.125mA/S 2:0.25mA/S 3:0.5mA/S 4:1.0mA/S 5:2.0mA/S 6:4.0mA/S 7:8.0mA/S 8:16.0mA/S 9:32.0mA/S 10:64.0mA/S 11:128.0mA/S 12:256.0mA/S 13:512.0mA/S 14:1024.0mA/S 15:2048.0mA/S;
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetSlopeDA(						 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									PLONG	lpSlopeType,				// 输出斜率类型 
									LONG	lChannel = 0);				// 通道号
	
	//****************************************************************************	
	// 修改模拟量输出斜率（每通道占用1个地址）
	// 功能码：0x10
	// 地址：0x1C0 
	// 斜率定义：0:Immediate 1:0.125mA/S 2:0.25mA/S 3:0.5mA/S 4:1.0mA/S 5:2.0mA/S 6:4.0mA/S 7:8.0mA/S 8:16.0mA/S 9:32.0mA/S 10:64.0mA/S 11:128.0mA/S 12:256.0mA/S 13:512.0mA/S 14:1024.0mA/S 15:2048.0mA/S;
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetSlopeDA(						 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lSlopeType,					// 输出斜率类型
									LONG	lChannel = 0);				// 通道号

	//####################### DI输入输出操作函数 ##############################
	//****************************************************************************	
	// 读取数字量输入的工作模式
	// 功能码：0x03
	// 地址：0x8C
	// 说明：lMode =0代表普通DI模式，=1代表计数模式，=2代表锁存模式；
	//		 其中第4位置0表示下降沿、置1表示上升沿。
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetModeDI(							 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lMode[],					// 输入的工作模式,0:DI模式,1:计数方式,2:锁存方式
									LONG	lEdgeMode[],				// 边沿方式,0:低电平,1:高电平
									LONG	lFirstChannel = 0,			// 首通道
									LONG	lLastChannel = 0);			// 末通道

	//****************************************************************************	
	// 设置数字量输入的工作模式
	// 功能码：0x10
	// 地址：0x8C 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_SetModeDI(							 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lMode,						// 输入的工作模式,0:DI模式,1:计数方式,2:锁存方式
									LONG	lEdgeMode,					// 边沿方式,0:低电平,1:高电平
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 设置CNT工作模式
	// 功能码：0x10
	// 地址：0x50 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetModeCNT(						
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD	lMode,						// 模式
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 获取CNT工作模式
	// 功能码：0x03
	// 地址：0x50 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetModeCNT(						
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD*	lMode,						// 模式
									LONG	lChannel = 0);				// 通道号
	
	//****************************************************************************	
	// 设置CNT启停
	// 功能码：0x10
	// 地址：0x62
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_EnableCNT(						 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD	lEnable);					// 启停

	//****************************************************************************	
	// 获取CNT启停
	// 功能码：0x03
	// 地址：0x62 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetEnableCNT(				 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD*	lEnable);					// 启停

	//****************************************************************************	
	// 下限报警使能
	// 功能码：0x10
	// 地址：0x64 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_LowerLimitAlarmEnable(		 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD	lEnable);					// 使能

	//****************************************************************************	
	// 获取下限报警使能
	// 功能码：0x03
	// 地址：0x64 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetLowerLimitAlarmEnable( 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD*	lEnable);					// 使能

	//****************************************************************************	
	// 上限报警使能
	// 功能码：0x10
	// 地址：0x63 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_UpperLimitAlarmEnable( 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD	lEnable);					// 使能

	//****************************************************************************	
	// 获取上限报警使能
	// 功能码：0x03
	// 地址：0x63
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetUpperLimitAlarmEnable( 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD*	lEnable);					// 使能

	//****************************************************************************	
	// 获取计数值断电保存
	// 功能码：0x03
	// 地址：0x205 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetPower_Save(				 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD*	wEnable);					// 使能

	//****************************************************************************	
	// 计数值断电保存
	// 功能码：0x10
	// 地址：0x205 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetPower_Save(				
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 模块地址
									WORD	wEnable);					// 使能

	//****************************************************************************	
	// 报警方式
	// 功能码：0x10
	// 地址：0x65 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetLimitAlarmMode( 
									HANDLE	hDevice,		// 设备对象句柄
									LONG	lDeviceID,		// 模块地址
									WORD	lMode);			// 模式 0:通道瞬时报警 1:通道锁存报警
	
	//****************************************************************************	
	// 获取报警方式
	// 功能码：0x03
	// 地址：0x65 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetLimitAlarmMode( 
									HANDLE	hDevice,		// 设备对象句柄
									LONG	lDeviceID,		// 模块地址
									WORD*	lMode);			// 模式 0:通道瞬时报警 1:通道锁存报警

	//****************************************************************************	
	// 获取LED显示通道
	// 功能码：0x03
	// 地址：0x3B 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetLed(
									HANDLE	hDevice,		// 设备对象句柄
									LONG	lDeviceID,		// 模块地址
									WORD*	wChannel);		// 通道号

	//****************************************************************************	
	// 设置LED显示通道
	// 功能码：0x10
	// 地址：0x3B 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetLed(
									HANDLE	hDevice,		// 设备对象句柄
									LONG	lDeviceID,		// 模式
									WORD	wChannel);		// 通道号
	
	//****************************************************************************	
	// 获取CNT计数值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址： 0x00 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCNTValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	plValue[],			// 计数值
									LONG	lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	//****************************************************************************	
	// 设置CNT计数值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x00 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCNTValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lValue,				// 最小值
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 设置上限警报值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0xA0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetUpperLimitAlarmVal(			
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lValue,				// 上限值
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 设置下限报警值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0xC0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetLowerLimitAlarmVal(		
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lValue,				// 上限值
									LONG	lChannel = 0);		// 通道号

	
	//****************************************************************************	
	// 获取上限报警值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0xA0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetUpperLimitAlarmVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	plValue[],			// 上限值
									LONG	lFirstChannel = 0,	// 首通道号
									LONG	lLastChannel = 0);	// 末通道号

	
	//****************************************************************************	
	// 获取下限报警值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0xC0
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetLowerLimitAlarmVal(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	plValue[],			// 下限值
									LONG	lFirstChannel = 0,
									LONG	lLastChannel = 0);	

	//****************************************************************************	
	// 获取CNT计数值最大值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x20 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCNTMaxValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	plMaxValue[],		// 计数值最大值
									LONG	lFirstChannel = 0,
									LONG	lLastChannel = 0);					

	//****************************************************************************	
	// 获取CNT计数值最小值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x40 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCNTMinValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	plMinValue[],		// 计数值最小值
									LONG	lFirstChannel = 0,
									LONG	lLastChannel = 0);					

	//****************************************************************************	
	// 设置CNT计数值最大值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x20
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCNTMaxValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lMaxValue,			// 最大值
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 设置CNT计数值最小值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x40
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCNTMinValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lMinValue,			// 最小值
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 设置CNT计数值初值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x10 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCNTInitValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	lInitValue,			// 初值
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 获取CNT计数值初值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x10 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCNTInitValue(
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 模块地址
									LONG	plInitValue[],		// 计数值初值
									LONG	lFirstChannel = 0,
									LONG	lLastChannel = 0);	// 通道号

	//****************************************************************************	
	// 读取开关量输入,读取16个寄存器
	// 功能码：0x02
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetDeviceDI(						  
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 设备地址
									PDAM3000M_PARA_DI pDIPara,	// DI值
									LONG	lBufferSize);		// 通道大小
		
	//****************************************************************************	
	// 读取开关量输入 可以自定义地址
	// 功能码：0x02
	// 地址：自定义
	//****************************************************************************	
	//------------------------------------------------------------------------------
	// 读取开关量输入 
	//------------------------------------------------------------------------------
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetDeviceDI_Addr(
		HANDLE hDevice, 
		LONG lDeviceID, 
		LONG lAddr,					// 地址（通道0对应地址）
		BYTE byDI[],	
		LONG lFirstChannel = 0,		// 首通道
		LONG lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 启动DI计数
	// 功能码：0x03
	// 地址：0xB0 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_StartDeviceDI(					 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 设备地址
									LONG	lChannel = 0);		// 通道号
		
	//****************************************************************************	
	// 停止DI计数
	// 功能码：0x10
	// 地址：0xB0 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_StopDeviceDI(				 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 设备地址
									LONG	lChannel = 0);		// 通道号
		
	//****************************************************************************	
	// 读取DI计数器值(每个通道占用2个地址)
	// 功能码：0x04
	// 地址：0x120 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetCNTDI(							 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 设备地址
									PLONG	lpCounterValue,     // 范围(0~65535)
									LONG	lFirstChannel = 0,	// 首通道
									LONG	lLastChannel = 0);	// 末通道

	//****************************************************************************	
	// 设置DI计数器初始值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL  DAM3000M_SetCNTDI(					 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 设备地址
									LONG	lInitValue,			// 计数初值
									LONG	lChannel = 0);		// 通道号

	//****************************************************************************	
	// 读取DI计数器初值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x00 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCNTDI_InitValue(			 
									HANDLE	hDevice,			// 设备对象句柄
									LONG	lDeviceID,			// 设备地址
									LONG	lCNTInitValue[],	// 计数器初值数组
									LONG	lFirstChannel = 0,	// 首通道
									LONG	lLastChannel = 0);	// 末通道

	//****************************************************************************	
	// 启动锁存
	// 功能码：0x10
	// 地址：0xAC
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_StartLatch(						
									HANDLE	 hDevice,			// 设备句柄
									LONG	 lDeviceID,			// 设备地址
									LONG	 lChannel = 0);		// 通道号
	
	//****************************************************************************	
	// 停止锁存
	// 功能码：0x10
	// 地址：0xAC
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_StopLatch(						 
									HANDLE	 hDevice,			// 设备句柄
									LONG	 lDeviceID,			// 设备地址
									LONG	 lChannel = 0);		// 通道号

	//****************************************************************************	
	// 读锁存状态
	// 功能码：0x02
	// 地址：0x20
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL  DAM3000M_GetLatchStatus(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lLatchType,					// 锁存类型，分为上升沿和下降沿锁存
									PDAM3000M_PARA_LATCH pLatchStatus,	// 锁存状态
									LONG	lBufferSize);				// 通道大小

	//****************************************************************************	
	// 清除计数值
	// 功能码：0x10
	// 地址：0xB4 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL  DAM3000M_ClearCNTVal(						 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 清除锁存状态
	// 功能码：0x10
	// 地址：0xB6
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL  DAM3000M_ClearLatchStatus(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lChannel = 0);				// 通道号

	//****************************************************************************	
	// 读自动读写标志位
	// 功能码：0x03
	// 地址：0x220 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetAutoReadFlag(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PWORD	plAutoReadFlag);		// 自动读标志

	//****************************************************************************	
	// 设置自动读写标志位
	// 功能码：0x10
	// 地址：0x220
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_SetAutoReadFlag(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									WORD	lAutoReadFlag);			// 自动读标志

	//****************************************************************************	
	// 获取DI锁存状态
	// 功能码：0x01
	// 地址：0x278
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetLatchStatus(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PBYTE	lStatus);				// 状态值,1-锁存 0-未锁存

	//****************************************************************************	
	// 设置DI锁存状态
	// 功能码：0x05
	// 地址：0x278
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetLatchStatus(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									BYTE	lStatus);				// 状态值,1-锁存 0-未锁存

	//****************************************************************************	
	// 获取DI过滤使能状态
	// 功能码：0x01
	// 地址：0x298
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetFilterEnable(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PBYTE	lStatus);				// 状态值,1-使能 0-不使能

	//****************************************************************************	
	// 设置DI过滤使能状态
	// 功能码：0x0F
	// 地址：0x298
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetFilterEnable(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									BYTE	lStatus[]);				// 状态值,1-使能 0-不使能

	//****************************************************************************	
	// 获取DI反向状态
	// 功能码：0x01
	// 地址：0x2A8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetReverseValue(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PBYTE	lValue);				// 状态值,1-反向 0-不反向

	//****************************************************************************	
	// 设置DI反向状态
	// 功能码：0x0F
	// 地址：0x2A8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetReverseValue(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									BYTE	lValue[]);				// 状态值,1-反向 0-不反向

	//****************************************************************************	
	// 获取DI计数器启停状态
	// 功能码：0x01
	// 地址：0x2B9 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetCounterStatus(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PBYTE	lStatus);				// 状态值,1-开始计数 0-停止计数

	//****************************************************************************	
	// 设置DI计数器启停状态
	// 功能码：0x05
	// 地址：0x2B9
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetCounterStatus(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									BYTE	lStatus);				// 状态值,1-开始计数 0-停止计数

	//****************************************************************************	
	// 获取DI溢出状态
	// 功能码：0x01
	// 地址：0x2D8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetOverflowStatus(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PBYTE	lStatus);				// 状态值,1-溢出 0-未溢出

	//****************************************************************************	
	// 设置DI溢出状态
	// 功能码：0x05
	// 地址：0x2D8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetOverflowStatus(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									BYTE	lStatus);				// 状态值,1-溢出 0-未溢出

	//****************************************************************************	
	// 获取DI工作模式
	// 功能码：0x03
	// 地址：0x670 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetMode(						
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LONG	lMode[],				// 模式
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DI工作模式
	// 功能码：0x10
	// 地址：0x670
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetMode(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LONG	lMode,					// 模式
									LONG	lChannel = 0);			// 通道号

	//****************************************************************************	
	// 获取DI高电平宽度(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x6A0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetHighLevelTime(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DI高电平宽度(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x6A0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetHighLevelTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 获取DI低电平宽度(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x6C0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetLowLevelTime(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DI低电平宽度(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x6C0 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetLowLevelTime(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 获取DI计数值/频率值(每个通道占用2个地址)
	// 功能码：0x03
	// 地址：0x120
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_GetValue(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 数值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DI计数值/频率值(每个通道占用2个地址)
	// 功能码：0x10
	// 地址：0x120 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DI_SetValue(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 数值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//####################### DO数字量输出函数 ###################################	
	//****************************************************************************	
	// 回读开关量输出（固定读16个通道）
	// 功能码：0x01
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetDeviceDO(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PDAM3000M_PARA_DO pDOPara);		// 当前DO输出值

	//****************************************************************************	
	// 回读开关量输出
	// 功能码：0x01
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL DAM3000M_GetDeviceDO_Addr(
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									ULONG   Addr,
									BYTE	byDOSts[],
									LONG    lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号
	
	//****************************************************************************	
	// 设置DO开关量输出值 
	// 功能码：0x0F
	// 地址：0x00 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetDeviceDO(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	byDOSts[],				// 设置DO输出值
									LONG    lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号

	//****************************************************************************	
	// 设置DO开关量输出值
	// 功能码：0x0F
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetDeviceDO_Addr(
									HANDLE  hDevice, 
									LONG	lDeviceID, 
									ULONG   Addr,
									BYTE	byDOSts[],
									LONG    lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号

	//****************************************************************************	
	// 获取DO上电初始值(不建议使用，读2个寄存器,不成功后,读1个寄存器)
	// 功能码：0x03
	// 地址：0xB8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetPowerOnValueDO(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PDAM3000M_PARA_DO pPowerOnPara);// 上电值 

	//****************************************************************************	
	// 设置DO上电初始值(不建议使用)
	// 功能码：0x10
	// 地址：0xB8 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetPowerOnValueDO(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									DAM3000M_PARA_DO& PowerOnPara);	// 上电值

	//****************************************************************************	
	// 获取DO上电初始值（操作16个通道）
	// 功能码：0x03
	// 地址：0xB8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetDOPowerOnValue(		 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	pPowerOn[],				// 上电值
									LONG	lChannelCount = 0);		// 设备通道总数（不大于16）

	//****************************************************************************	
	// 设置DO上电初始值（操作16个通道）
	// 功能码：0x10
	// 地址：0xB8 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetDOPowerOnValue(			  
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	pPowerOn[],				// 上电值
									LONG	lChannelCount = 0);		// 设备通道总数（不大于16）

	//****************************************************************************	
	// 读DO安全值(不建议使用，读2个寄存器,不成功后,读1个寄存器)
	// 功能码：0x03
	// 地址：0xBA 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetSafeValueDO(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PDAM3000M_PARA_DO pDOSafePara);	// 安全值

	//****************************************************************************	
	// 设置安全值(不建议使用)
	// 功能码：0x10
	// 地址：0xBA 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetSafeValueDO(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									DAM3000M_PARA_DO& DOSafePara);	// 安全值
	
	
	//****************************************************************************	
	// 读DO安全值 (操作16通道) 
	// 功能码：0x03
	// 地址：0xBA
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetDOSafeValue(				// 读DO安全值
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	pDOSafe[],				// 安全值
									LONG	lChannelCount = 0);		// 设备通道总数（不大于16）

	//****************************************************************************	
	// 设置安全值 (操作16通道)
	// 功能码：0x10
	// 地址：0xBA
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetDOSafeValue(	 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	pDOSafe[],				// 安全值
									LONG	lChannelCount = 0);		// 设备通道总数（不大于16）
	
	//****************************************************************************	
	// 获取紧急输出使能标志（16个通道）
	// 功能码：0x01
	// 地址：0x80 
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_GetDeviceSafeOutENDO(				
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PDAM3000M_PARA_DO pDOPara);		// DO输出值
	
	//****************************************************************************	
	// 设置紧急输出通道使能标志
	// 功能码：0x05
	// 地址：0x80
	//****************************************************************************	
	BOOL DEVAPI	FAR PASCAL	DAM3000M_SetDeviceSafeOutENDO(				 
									HANDLE  hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	byDOSts,				// 通道状态
									LONG    lChannel = 0);			// 通道号

	//****************************************************************************	
	// 获取DO输出值（操作16个通道）
	// 功能码：0x01
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetValue_Addr(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lAddr,					// DO0通道地址
									LONG	lDeviceID,				// 设备地址
									PDAM3000M_PARA_DO pDOPara);		// 当前DO输出值

	//****************************************************************************	
	// 获取DO输出值(多通道) 
	// 功能码：0x01
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetValue_Multi_Addr(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lAddr,					// DO0通道地址 
									LONG	lDeviceID,				// 设备地址
									BYTE	pDOPara[],				// 当前DO输出值
									LONG	lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号

	//****************************************************************************	
	// 设置DO输出值
	// 功能码：0x0F
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetValue_Addr(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lAddr,					// DO0通道地址
									LONG	lDeviceID,				// 设备地址
									BYTE	byDOSts[],				// 设置DO输出值
									LONG	lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号

	//****************************************************************************	
	// 获取DO上电输出状态
	// 功能码：0x01
	// 地址：0x20 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetPowerOnStatus(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PBYTE	lSts);					// 状态值，1-高电平 0-低电平

	//****************************************************************************	
	// 设置DO上电输出状态
	// 功能码：0x0F
	// 地址：0x20
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetPowerOnStatus(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	lSts);					// 状态值，1-高电平 0-低电平

	//****************************************************************************	
	// 获取DO安全输出状态
	// 功能码：0x01
	// 地址：0x40 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetSafeStatus(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PBYTE	lSts);					// 状态值，1-高电平 0-低电平

	//****************************************************************************	
	// 设置DO安全输出状态
	// 功能码：0x05
	// 地址：0x40
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetSafeStatus(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	lSts);					// 状态值，1-高电平 0-低电平

	//****************************************************************************	
	// 获取DO工作模式
	// 功能码：0x03
	// 地址：0x3E8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetMode(							 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lMode[],				// DO工作模式,lMode[0]-0通道
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO工作模式
	// 功能码：0x10
	// 地址：0x3E8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetMode(							 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lMode,					// DO工作模式
									LONG	lChannel = 0);			// 通道号

	//****************************************************************************	
	// 获取DO输出高电平宽度(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：0x408
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetHighLevelTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO输出高电平宽度(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：0x408
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetHighLevelTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************
	// 获取DO输出低电平宽度(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：0x428
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetLowLevelTime(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO输出低电平宽度(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：0x428
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetLowLevelTime(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lTimes[],				// 宽度值
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 获取DO脉冲输出个数(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：0x468
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetPulseCount(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 个数
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO脉冲输出个数(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：0x468
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetPulseCount(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 个数
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 获取DO低到高输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：0x488
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetLToHDelayTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 时间
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO低到高输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：0x488
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetLToHDelayTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 时间
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 获取DO高到低输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：0x4A8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetHToLDelayTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 时间
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO高到低输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：0x4A8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetHToLDelayTime(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 时间
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 获取DO脉冲输出增加个数(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：0x4C8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetPulseAddCount(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 个数
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 设置DO脉冲输出增加个数(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：0x4C8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetPulseAddCount(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lValue[],				// 个数
									LONG	lFirstChannel = 0,		// 首通道
									LONG	lLastChannel = 0);		// 末通道

	//***************************************************************************
	// 获取DO脉冲输出高电平宽度(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：自定义
	//***************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetHighLevelTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lTimes[],					// 宽度值
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 设置DO脉冲输出高电平宽度(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetHighLevelTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lTimes[],					// 宽度值
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 获取DO脉冲输出低电平宽度(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetLowLevelTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lTimes[],					// 宽度值
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 设置DO脉冲输出低电平宽度(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetLowLevelTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lTimes[],					// 宽度值
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//***************************************************************************
	// 获取DO脉冲输出个数(每通道占2个寄存器)
	// 功能码：0x03
	// 地址：自定义
	//***************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetPulseCount_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 个数
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 设置DO脉冲输出个数(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetPulseCount_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 个数
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 获取DO低到高输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetLToHDelayTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 时间
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 设置DO低到高输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetLToHDelayTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 时间
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 获取DO高到低输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetHToLDelayTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 时间
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 设置DO高到低输出延迟时间(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetHToLDelayTime_Addr(
									HANDLE hDevice,					// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 时间
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 获取DO脉冲输出增加个数(每个通道占2个寄存器)
	// 功能码：0x03
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_GetPulseAddCount_Addr(
									HANDLE	hDevice,				// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 个数
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//****************************************************************************
	// 设置DO脉冲输出增加个数(每个通道占2个寄存器)
	// 功能码：0x10
	// 地址：自定义
	//****************************************************************************
	BOOL DEVAPI FAR PASCAL DAM3000M_DO_SetPulseAddCount_Addr(
									HANDLE	hDevice,				// 设备对象句柄
									LONG lAddr,						// 寄存器地址
									LONG lDeviceID,					// 设备地址
									LONG lValue[],					// 个数
									LONG lFirstChannel = 0,			// 首通道
									LONG lLastChannel = 0);			// 末通道

	//#############################  计数器  ###################################
	//****************************************************************************	
	// 对各个计数器进行参数设置（工作模式、初值、最大值）
	// 功能码：0x10
	// 地址：0x00 0x10 0x20
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCounterMode(						 
									HANDLE   hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									PDAM3000M_PARA_CNT pCNTPara,	// 基于各通道的计数器参数
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 初始化报警的工作模式
	// 功能码：0x10
	// 地址：0x40 0x44 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_InitCounterAlarm(					 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									PDAM3000M_CNT_ALARM pCNTAlarm);	// 报警参数设置

	//****************************************************************************	
	// 设置计数器报警方式  
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetCounterAlarmMode(				 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lAlarmMode);			// 报警方式

	//****************************************************************************	
	// 获得计数器设备硬件参数状态（通道配置、信号宽度、计数初值等）
	// 功能码：0x03
	// 地址：0x00 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetCounterSts(					 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									PDAM3000M_CNT_STATUS pStsCNT,	// 返回值
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 启动计数器计数
	// 功能码：0x10
	// 地址：0x09 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_StartCounter(						 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 停止计数器计数
	// 功能码：0x03
	// 地址：0x09
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_StopCounter(						 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 取得计数器当前值
	// 功能码：0x04
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetCounterCurVal(					 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG     lDeviceID,				// 设备地址
									PULONG   pulCNTVal,				// 计数值
									LONG     lChannel = 0);			// 通道号

	//****************************************************************************	
	// 取得频率器当前值（每通道占用2个寄存器）
	// 功能码：0x04
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetFreqCurVal(						 
									HANDLE   hDevice,				// 设备对象句柄
									LONG     lDeviceID,				// 设备地址
									PULONG   pulFreqVal,			// 频率值
									LONG     lChannel = 0);			// 通道号

	//****************************************************************************	
	// 计数器复位
	// 功能码：0x10
	// 地址：0x09
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ResetCounter(						 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 初始化滤波
	// 功能码：0x10
	// 地址：0x30 0x38
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_InitCounterFilter(					 
									HANDLE   hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									PDAM3000M_PARA_FILTER pFilter,	// 滤波参数
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 使能滤波状态
	// 功能码：0x10
	// 地址：0x08
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_EnableFilter(						 
									HANDLE   hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									BOOL	 bEnable,				// 使能滤波
									LONG	 lChannel = 0);			// 通道号

	//****************************************************************************	
	// 获得DO及报警状态
	// 功能码：0x03
	// 地址：0x40 0x41 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetCounterAlarmSts(			 
									HANDLE   hDevice,				// 设备对象句柄
									LONG     lDeviceID,				// 设备地址
									PLONG    plEnableAlarm,			// 计数器报警状态
									PLONG    pbDO,					// DO
									LONG	 lChannel = 0);			// 通道号
	
	//****************************************************************************	
	// 设置DO
	// 功能码：0x10
	// 地址：0x41
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetCounterDO(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	byDOSts[],				// DO
									LONG    lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号
										
	//****************************************************************************	
	// 清报警方式1报警输出
	// 功能码：0x10
	// 地址：0x41
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ClearAlarmSts(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID);				// 设备地址
	
	//****************************************************************************	
	// 取得计数器LED显示通道
	// 功能码：0x03
	// 地址：0x08
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetLEDCounterCH(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									PLONG	plChannel);				// 通道号
	
	//****************************************************************************	
	// 设置计数器LED显示通道
	// 功能码：0x10
	// 地址：0x08
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetLEDCounterCH(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									LONG	lChannel = 0);			// 通道号

	//#############################  电量模块  ###################################
	//****************************************************************************	
	// 获得电量值
	// 功能码：0x04
	// 地址：0x00+
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetEnergyVal(						 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lValue[],				// 电量值
									LONG	 lAanlogType,			// 模拟量类型
									LONG	 lFirstChannel = 0,		// 首通道
									LONG	 lLastChannel = 0);		// 末通道
	
	//****************************************************************************	
	// 获得电量值
	// 功能码：0x04
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetEnergyVal_GetULONG(				 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									ULONG	 lValue[],				// 电量值
									LONG	 lAanlogType,			// 模拟量类型
									LONG	 lFirstChannel = 0,		// 首通道
									LONG	 lLastChannel = 0);		// 末通道

	//****************************************************************************	
	// 清能量寄存器
	// 功能码：0x10
	// 地址：0x108
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_ClrEnergyReg(						 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lChannel = 0);			// 通道

	//****************************************************************************	
	// 获得能量单位 
	// 功能码：0x03
	// 地址：0x102 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetEnergyPerLSB(					 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									PLONG	 lpEnergyPerLSB);		// 能量单位

	//****************************************************************************	
	// 设置能量单位
	// 功能码：0x10
	// 地址：0x102
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetEnergyPerLSB(					 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									LONG	 lEnergyPerLSB);		// 能量单位

	//****************************************************************************	
	// 获得输入量程
	// 功能码：0x03
	// 地址：0x100
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetInputRange(					 
									HANDLE	 hDevice,				// 设备对象句柄
									LONG	 lDeviceID,				// 设备地址
									PLONG	 lpInputRangeV,			// 电压输入量程
									PLONG	 lpInputRangeI);		// 电流输入量程

	//****************************************************************************	
	// 设置输入量程
	// 功能码：0x10
	// 地址：0x100 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetInputRange(						 
									HANDLE	hDevice,				// 设备对象句
									LONG	lDeviceID,				// 设备地址
									LONG	lInputRangeV,			// 电压输入量程
									LONG	lInputRangeI);			// 电流输入量程

	//****************************************************************************	
	// 获得多个输入量程
	// 功能码：0x04
	// 地址：0xC8
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetInputRange_Multi(
									HANDLE	hDevice,
									LONG	lDeviceID,
									LONG	lInputRange[],
									LONG	lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号

	//****************************************************************************	
	// 设置输入量程（支持多个通道）
	// 功能码：0x10
	// 地址：0xC8 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_SetInputRange_Multi(		
									HANDLE	hDevice,
									LONG	lDeviceID,
									LONG	lInputRange[],
									LONG	lFirstChannel = 0,		// 首通道号
									LONG	lLastChannel = 0);		// 末通道号

	//****************************************************************************	
	// 设置通道使能状态（此函数操作4XXXX地址，一个地址放多个通道使能状态，所以不推荐使用此函数操作部分通道，以免影响其它通道使能状态）
	// 功能码：0x10
	// 地址：0xDC
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_SetChanEnableSts(
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	lChannelEn[],			// 设置AD使能状态 lADEn[0]对应0通道 lADEn[2]对应2通道...
									LONG	lChannelCount = 1);		// 通道总数

	//****************************************************************************	
	// 获取通道使能状态
	// 功能码：0x04
	// 地址：0xDC
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetChanEnableSts(
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	lChannelEn[],			// 设置AD使能状态 lADEn[0]对应0通道
									LONG	lChannelCount = 1);		// 通道总数

	//****************************************************************************	
	// 获得环境温度
	// 功能码：0x04
	// 地址：0x80
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetEvrmTemp(						 
									HANDLE	hDevice,				// 设备对象句
									LONG	lDeviceID,				// 设备地址
									PLONG	lpEvrmTemp,				// 温度
									LONG	lChannel = 0);			// 通道号

	//****************************************************************************	
	// 获得环境湿度
	// 功能码：0x04
	// 地址：0x81
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetEvrmHum(						 
									HANDLE	hDevice,				// 设备对象句
									LONG	lDeviceID,				// 设备地址
									PLONG	lpEvrmHum,				// 湿度		
									LONG	lChannel = 0);			// 通道号


	//##################################### 看门狗 ################################
	//****************************************************************************	
	// 看门狗保持连接 （下位机无返回信息）
	// 功能码：0x10
	// 地址：0x203
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_HostIsOK(HANDLE hDevice);			 

	//****************************************************************************	
	// 打开主看门狗(先设置超时间隔，再使能看门狗)
	// 功能码：0x10
	// 地址：0x200 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_EnableWatchdog(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID);				// 模块地址

	//****************************************************************************	
	// 禁止看门狗工作
	// 功能码：0x10
	// 地址：0x200
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_CloseWatchdog(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID);				// 模块地址

	//****************************************************************************	
	// 读取主看门狗的状态(1=Host is down; 0=OK)
	// 功能码：0x03
	// 地址：0x201
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetWatchdogStatus(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PLONG	lpWatchdogStatus);		// 看门狗状态

	//****************************************************************************	
	// 复位主看门狗的状态(0)
	// 功能码：0x10
	// 地址：0x201
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_ResetWatchdogStatus(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID);				// 模块地址

	//****************************************************************************	
	// 取得看门狗设置的时间间隔
	// 功能码：0x03
	// 地址：0x202
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetWatchdogTimeoutVal(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PLONG	lpInterval);			// 时间间隔

	//****************************************************************************	
	// 设置看门狗设置的时间间隔
	// 功能码：0x10
	// 地址：0x202
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetWatchdogTimeoutVal(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LONG	lInterval);				// 时间间隔

	// ##################################### DIGIT LED 设置函数 #################################
	//****************************************************************************	
	// 获得显示模式请求
	// 功能码：0x03
	// 地址：0x11D 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetDLedMode(						 
									HANDLE hDevice,					// 设备对象句柄
									LONG   lDeviceID,				// 模块地址
									PLONG  lpDispMode);				// 显示模式 0x00：温度格式,0x01:欧姆值

	//****************************************************************************	
	// 修改显示模式请求
	// 功能码：0x10
	// 地址：0x11D
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetDLedMode(						 
									HANDLE hDevice,					// 设备对象句柄
									LONG   lDeviceID,				// 模块地址
									LONG   lDispMode);				// 显示模式 0x01：温度格式,0x02:欧姆值

	//****************************************************************************	
	// 获得LED显示通道号
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_GetDLedDispChannel(				 
									HANDLE hDevice,					// 设备对象句柄
									LONG   lDeviceID,				// 模块地址
									PLONG  lpChannel);				// 通道号,lpChannel = 0xff:主机控制显示

	//****************************************************************************	
	// 设置LED显示通道号
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetDLedDispChannel(				 
									HANDLE hDevice,					// 设备对象句柄
									LONG   lDeviceID,				// 模块地址
									LONG   lChannel = 0);			// 通道号,lpChannel = 0xff:主机控制显示

	//****************************************************************************	
	// 主机控制显示值
	// 功能码：0x10
	// 地址：0x120
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetDLedValueW(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LPCWSTR	strWriteBuf,			// 显示的字符串
									LONG	llength);				// 数据长度

	//****************************************************************************	
	// 主机控制显示值
	// 功能码：0x10
	// 地址：0x120 
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetDLedValueA(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LPCSTR	strWriteBuf,			// 显示的字符串
									LONG	llength);				// 数据长度

#ifdef UNICODE
#define DAM3000M_SetDLedValue  DAM3000M_SetDLedValueW
#else
#define DAM3000M_SetDLedValue  DAM3000M_SetDLedValueA
#endif // !UNICODE

//####################### 输入输出任意二进制字符 ###########################
	//****************************************************************************	
	// 写设备
	//****************************************************************************	
	int DEVAPI	FAR	PASCAL	DAM3000M_WriteDeviceChar(					 
									HANDLE	hDevice,				// 设备对象句柄
									char*	strWriteBuf,			// 写的数据
									long	llength,				// 数据长度
									long	timeout = 100);			// 超时范围(mS)
	
	//****************************************************************************	
	// 读设备
	//****************************************************************************	
	int	DEVAPI	FAR	PASCAL	DAM3000M_ReadDeviceChar(					 
									HANDLE	hDevice,				// 设备对象句柄
									char*	strReadBuf,				// 读取的数据
									long	llength,				// 数据长度
									long	timeout = 100);			// 超时范围(mS)

	//****************************************************************************	
	// 直接读设备 无符号
	//****************************************************************************	
	int DEVAPI FAR PASCAL DAM3000M_ReadDeviceUnsignedChar(				 
									HANDLE	hDevice,				// 设备对象句柄
									unsigned char*	strReadBuf,		// 读取的数据
									long	llength,				// 数据长度
									long	timeout = 100);			// 超时范围(mS)
											   
//###########################　模块信息确认函数  #################################
// 	//****************************************************************************	
// 	//  
// 	//****************************************************************************	
// 	BOOL DEVAPI	FAR	PASCAL	InitCheckInfo(
// 									HANDLE	hDevice, 
// 									LONG	lDeviceID, 
// 									DWORD	dwNum);
// 
// 	//****************************************************************************	
// 	//  
// 	//****************************************************************************	
// 	BOOL DEVAPI	FAR	PASCAL	ReadCheckInfo(
// 									HANDLE	hDevice, 
// 									LONG	lDeviceID, 
// 									LONG	lIndex, 
// 									BYTE&	byEncrypt);

//####################################### 辅助函数 ####################################
	//****************************************************************************	
	// 微调当前补偿斜率
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_AdjustSlopeVal(					 
									HANDLE  hDevice,				// 设备对象句柄
									LONG    lDeviceID,   			// 模块地址
									LONG    lAdjustVal,				// 微调值
									LONG    lChannel = 0);			// 通道号

	//****************************************************************************	
	// 设置当前值为输出补偿斜率
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_StoreSlopeVal(						 
									HANDLE  hDevice,				// 设备对象句柄
									LONG    lDeviceID,   			// 模块地址
									LONG    lChannel = 0);			// 通道号

	//****************************************************************************	
	// 设定补偿斜率为默认值
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetFaultSlopeVal(					 
									HANDLE  hDevice,				// 设备对象句柄
									LONG    lDeviceID,   			// 模块地址
									LONG    lChannel = 0);			// 通道号	0xFF为所有通道恢复

	//****************************************************************************	
	// 设置零点偏移补偿
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetZeroRepair(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LONG	lZeroRepair,			// 零点值
									LONG	lChannel = 0);			// 通道号

	//****************************************************************************	
	// 设置模块进入测试模式
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_SetDevTestMode(					 
									HANDLE  hDevice,				// 设备对象句柄
									LONG    lDeviceID);				// 模块地址

	//****************************************************************************	
	// 模块软复位
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_ResetModule(						 
									HANDLE  hDevice,				// 设备对象句柄
									LONG    lDeviceID); 			// 模块地址

	//****************************************************************************	
	// 取得环境温度(为取热电偶值作准备)
	// 功能码：0x04
	// 地址：0x18F
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetEnvironmentTemp(				 
									HANDLE	hDevice,
									LONG	lDeviceID,
									PFLOAT	lpETemprt);

	//****************************************************************************	
	// 取得环境温度(为取热电偶值作准备)
	// 功能码：0x10
	// 地址：0x11F
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetAdjustTemp(						 
									HANDLE	hDevice,
									LONG	lDeviceID,
									BYTE	lETemprt);

	//####################################### 测温操作函数 ####################################

	//****************************************************************************	
	// 读取测量值
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadMeasuringValue(			  
									HANDLE	hDevice,				// 设备对象句柄	
									LONG	lDeviceID,				// 模块地址
									WORD	lpADBuffer[]);			// 接收数据的用户缓冲区128

	//****************************************************************************	
	// 读取测量温度值(自定义地址和读取长度)
	// 功能码：0x04
	// 地址：自定义
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_ReadSensorTemp(					 
									HANDLE hDevice,					// 设备对象句柄	
									LONG   lDeviceID,
									LONG   lAddr,					// 温度寄存器地址
									LONG   lCount,					// 温度传感器个数
									WORD   lADBuffer[]);			// 接受数据的用户缓冲区

	//****************************************************************************	
	// 复位测温模块
	// 功能码：0x10
	// 地址：0x013A
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_TempResetModule(				  
									HANDLE	hDevice,				// 设备对象句柄	
									LONG	lDeviceID);				// 模块地址

	//****************************************************************************	
	// 修改传感器编号
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetSensorSerialNumber(			// 修改传感器编号
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 设备地址
									BYTE	bChannel,				// 通道号
									BYTE	bSequenceNumber,		// 通道内顺序号
									BYTE	bNewNumber,				// 新编号
									BYTE	bOldNumber,				// 旧编码
									BOOL	bAuto);					// 是否自动编号  FALSE手动编号 TURE自动编号(通道号 通道内顺序号 新编号 参数不起作用)

	//****************************************************************************	
	// 读取传感器参数
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL	DAM3000M_GetSensorPara(					 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									DAM3000M_SENSOR_PARA pInfo[128]);	// 传感器参数

	//****************************************************************************	
	// 读取单个传感器参数
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_GetSingleSensorPara(				
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lSensorNumber,				// 传感器编号
									DAM3000M_SENSOR_PARA* pSingleInfo);	// 传感器参数

	//****************************************************************************	
	// 导入传感器配置参数到板卡
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL DAM3000M_LoadSensorConfigure(		 
									HANDLE	hDevice,					// 设备对象句柄
									LONG	lDeviceID,					// 设备地址
									LONG	lSensorNumber,				// 传感器编号
									LONG	lChannelNumber,				// 所在通道号
									LONG	lSequenceNumber,			// 通道内顺序号
									BYTE	bSensorID[8]);				// 传感器ID号


//####################################### Modus 基本功能操作函数 ####################################

	//****************************************************************************	
	// 读继电器状态
	// 功能码：0x01
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadCoils(							 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									int		len, 
									BYTE	bCoilsFlag[]);

	//****************************************************************************	
	// 读开关量输入
	// 功能码：0x02
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadDiscretes(						 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									int		len, 
									BYTE	bDIState[]);

	//****************************************************************************	
	// 读保持寄存器
	// 功能码：0x03
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadMultiRegs(						 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									int		len, 
									BYTE	buf[]);

	//****************************************************************************	
	// 读输入寄存器
	// 功能码：0x04
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadInputRegs(						 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									int		len, 
									BYTE	buf[]);

	//****************************************************************************	
	// 设置单个继电器
	// 功能码：0x05
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_WriteCoil(							 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									BYTE	status);

	//****************************************************************************	
	// 设置单个保持寄存器
	// 功能码：0x10
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_WriteSingleReg(					 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									USHORT val);

	//****************************************************************************	
	// 设置多个继电器
	// 功能码：0x0F
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ForceMultiCoils(					 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									int		len, 
									BYTE	bDOState[]);

	//****************************************************************************	
	// 设置多个保持寄存器
	// 功能码：0x10
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_WriteMultiRegs(					 
									HANDLE	hDevice, 
									LONG	lDeviceID, 
									int		addr, 
									int		len, 
									BYTE	buf[]);

	//####################################### 数码管显示 ####################################
	//****************************************************************************	
	// 单显部分 显示数据
	// 功能码：0x10
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_DisplayData(					 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									WORD	wData);					// 数据

	//****************************************************************************	
	// 单显部分 特殊符号
	// 功能码：0x10
	// 地址：0x01 
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_DisplaySpecialSymbols(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									WORD	wSymbols);				// 符号

	//****************************************************************************	
	// 单显部分 读数据
	// 功能码：0x03
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadData(						 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PWORD	pwData);				// 数据

	//****************************************************************************	
	// 单显部分 读特殊符号
	// 功能码：0x03
	// 地址：0x01
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadSpecialSymbols(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PWORD	pwSymbols);				// 符号

	//****************************************************************************	
	// 混显部分 显示数据及特殊符号
	// 功能码：0x10
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI FAR PASCAL  DAM3000M_DisplayDataSymbols(			 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									WORD	wData,					// 数据
									WORD	wSymbols);				// 符号

	//****************************************************************************	
	// 混显部分 读数据及特殊符号
	// 功能码：0x03
	// 地址：0x00
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_ReadDataSymbols(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PWORD	pwData,					// 数据
									PWORD	pwSymbols);				// 符号

	//****************************************************************************	
	// 设置小数点位置
	// 功能码：0x10
	// 地址：0x91
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_SetDecimalPoint(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									LONG	lDecimalPoint);			// 小数点位置

	//****************************************************************************	
	// 读取小数点位置
	// 功能码：0x03
	// 地址：0x91
	//****************************************************************************	
	BOOL DEVAPI	FAR	PASCAL	DAM3000M_GetDecimalPoint(				 
									HANDLE	hDevice,				// 设备对象句柄
									LONG	lDeviceID,				// 模块地址
									PLONG	plDecimalPoint);		// 小数点位置
#ifdef __cplusplus
}
#endif

//#######################################################################
// 自动包含驱动函数导入库
#ifndef DEFINING
#ifndef _WIN64
#pragma comment(lib, "DAM3000M_32.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with DAM3000M_32.dll...")
#pragma message("======== Successfully linked with DAM3000M_32.dll")
#else
#pragma comment(lib, "DAM3000M_64.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with DAM3000M_64.dll...")
#pragma message("======== Successfully linked with DAM3000M_64.dll")
#endif // _WIN64
#endif // DEFINING

#endif // ifndef _ART_DAM3000M_SERIAL_




