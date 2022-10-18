#pragma once
#include "TypeID.h"

#ifdef NETPACKET_EXPORTS
#define DATCOLLECT_API _declspec(dllexport)
#else
#ifdef DONTIMPORT
#define DATCOLLECT_API
#else
#define DATCOLLECT_API _declspec(dllimport)
#endif
#endif

class DATCOLLECT_API DataCollect
{
public:
	// 按钮增加次数  _nTypeNumber 第几个设备(类型的号码)
	static void IncButtonClicked(int _nTypeNumber = 0);
	// 参与次数
	static void IncParticketimeNum(int _nTypeNumber = 0);
	// 手柄 
	static void IncHandleNum(int _nTypeNumber = 0);
	// 遥感  
	static void IncRockerNum(int _nTypeNumber = 0);
	// 电机启动 
	static void IncEmachineNum(int _nTypeNumber = 0);
	// 拉杆 
	static void IncPullrodNum(int _nTypeNumber = 0);
	// 旋钮
	static void IncKnobNum(int _nTypeNumber = 0);
	// 传感器
	static void IncTensorNum(int _nTypeNumber = 0);
	// 手轮 
	static void IncHandwheelNum(int _nTypeNumber = 0);
	// 摄像头
	static void IncCameraNum(int _nTypeNumber = 0);
	// 雷达 
	static void IncRadarNum(int _nTypeNumber = 0);
	// 触摸键
	static void IncTouchkeyNum(int _nTypeNumber = 0);
	// 按键  
	static void IncKeyNum(int _nTypeNumber = 0);
	// rfid卡
	static void IncRfidNum(int _nTypeNumber = 0);
	// 转盘  
	static void IncTurntableNum(int _nTypeNumber = 0);
	// 滑杆  
	static void IncSlidebarNum(int _nTypeNumber = 0);
	// 体感器
	static void IncSomatosensoryNum(int _nTypeNumber = 0);

	// 增加类型点击次数 ETypeID 类型ID _nTypeNumber 第几个按钮(类型的号码)
	static void IncTypeClickedNum(int _eTypeID, int _nTypeNumber = 0);

	// 以下为报警数据
	static void Alarm(int eAlarmType, int _nTypeNumber = 0);

	// 摄像头报警 _nTypeNumber 第几个(类型的号码)
	static void AlarmCamera(int _nTypeNumber = 0);
	// 机器人报警
	static void AlarmRobot(int _nTypeNumber = 0);
	// 板卡报警
	static void AlarmPciCard(int _nTypeNumber = 0);
};


