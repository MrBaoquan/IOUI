#pragma once

class TypeID
{
public:
	enum ETypeID
	{
		// ��ť
		BUTTON = 0,
		// �������
		PARTICKETIME = 1,
		// �ֱ�
		HANDLE = 2,
		// ң��
		ROCKER = 3,
		// �������
		EMACHINE = 4,
		// ����
		PULLROD = 5,
		// ��ť
		KNOB = 6,
		// ������
		TENSOR = 7,
		// ����
		HANDWHEEL = 8,
		// ����ͷ
		CAMERA = 9,
		// �״�
		RADAR = 10,
		// ������
		TOUCHKEY = 11,
		// ����
		KEY = 12,
		// RFID��
		RFID = 13,
		// ת��
		TURNTABLE = 14,
		// ����
		SLIDEBAR = 15,
		// �����
		SOMATOSENSORY = 16,

		ETID_MAX,
	};

	//�Զ������͵� ETypeID
#define ETID_CUSTOM (ETID_MAX+1)

	// ��������
	enum EAlarmType {
		EAT_TouchScreen, //������
		EAT_Sensor, //������
		EAT_Camera, //����ͷ
		EAT_Robot, //������
		EAT_Kinect, // ���
		EAT_PciCard, // �忨
		EAT_ConveyorFailure, // ���ʹ�����
		KinectDisconnected, // Kinect���ӶϿ�
		PlatformSystemAlarm, // ƽ̨ϵͳ����
		CoilOverheating, // ��Ȧ����
		MechanismAlarm, // ��������
		MicrophoneDisconnected, // ��˷����ӶϿ�
		EncoderDamaged, // ��������
		CodeScannerDisconnected, // ɨ�������ӶϿ�
		InsufficientScrewSupply, // �ݶ����ϲ���
		ChargingModuleAlarm, // ���ģ�鱨��
		HelmetConnectionFailure, // ͷ������ʧ�ܱ���
		LowPowerAlarm, // �͵�������
		RobotFishAlarm, // �����㱨��
		MotorFault, // ���1����
		WindmillModelDamageDetection, // �糵ģ�ͼ����
		SlideBlockDamaged, // ������
		RollerDamaged, // ������
		BatteryModelDetectionDamage, // ���ģ�ͼ����
		ElectrodeMaterialDamage, // �缫����ģ�ͼ����
		BlockDamaged, // ������
		RotaryTableDamaged, // ת����
		EAT_MAX,
	};

	// �û��Զ��徯��ID
#define EAT_CUSTOM (EAT_MAX+1)
};
