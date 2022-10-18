#pragma once

class TypeID
{
public:
	enum ETypeID
	{
		// 按钮
		BUTTON = 0,
		// 参与次数
		PARTICKETIME = 1,
		// 手柄
		HANDLE = 2,
		// 遥感
		ROCKER = 3,
		// 电机启动
		EMACHINE = 4,
		// 拉杆
		PULLROD = 5,
		// 旋钮
		KNOB = 6,
		// 传感器
		TENSOR = 7,
		// 手轮
		HANDWHEEL = 8,
		// 摄像头
		CAMERA = 9,
		// 雷达
		RADAR = 10,
		// 触摸键
		TOUCHKEY = 11,
		// 按键
		KEY = 12,
		// RFID卡
		RFID = 13,
		// 转盘
		TURNTABLE = 14,
		// 滑杆
		SLIDEBAR = 15,
		// 体感器
		SOMATOSENSORY = 16,

		ETID_MAX,
	};

	//自定义类型的 ETypeID
#define ETID_CUSTOM (ETID_MAX+1)

	// 报警类型
	enum EAlarmType {
		EAT_TouchScreen, //触摸屏
		EAT_Sensor, //传感器
		EAT_Camera, //摄像头
		EAT_Robot, //机器人
		EAT_Kinect, // 体感
		EAT_PciCard, // 板卡
		EAT_ConveyorFailure, // 传送带故障
		KinectDisconnected, // Kinect连接断开
		PlatformSystemAlarm, // 平台系统报警
		CoilOverheating, // 线圈过热
		MechanismAlarm, // 机构报警
		MicrophoneDisconnected, // 麦克风连接断开
		EncoderDamaged, // 编码器损坏
		CodeScannerDisconnected, // 扫码器连接断开
		InsufficientScrewSupply, // 螺钉供料不足
		ChargingModuleAlarm, // 充电模块报警
		HelmetConnectionFailure, // 头盔连接失败报警
		LowPowerAlarm, // 低电量报警
		RobotFishAlarm, // 机器鱼报警
		MotorFault, // 电机1故障
		WindmillModelDamageDetection, // 风车模型检测损坏
		SlideBlockDamaged, // 滑块损坏
		RollerDamaged, // 滚轮损坏
		BatteryModelDetectionDamage, // 电池模型检测损坏
		ElectrodeMaterialDamage, // 电极材料模型检测损坏
		BlockDamaged, // 拨块损坏
		RotaryTableDamaged, // 转盘损坏
		EAT_MAX,
	};

	// 用户自定义警报ID
#define EAT_CUSTOM (EAT_MAX+1)
};
