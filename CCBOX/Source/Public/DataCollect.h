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
	// ��ť���Ӵ���  _nTypeNumber �ڼ����豸(���͵ĺ���)
	static void IncButtonClicked(int _nTypeNumber = 0);
	// �������
	static void IncParticketimeNum(int _nTypeNumber = 0);
	// �ֱ� 
	static void IncHandleNum(int _nTypeNumber = 0);
	// ң��  
	static void IncRockerNum(int _nTypeNumber = 0);
	// ������� 
	static void IncEmachineNum(int _nTypeNumber = 0);
	// ���� 
	static void IncPullrodNum(int _nTypeNumber = 0);
	// ��ť
	static void IncKnobNum(int _nTypeNumber = 0);
	// ������
	static void IncTensorNum(int _nTypeNumber = 0);
	// ���� 
	static void IncHandwheelNum(int _nTypeNumber = 0);
	// ����ͷ
	static void IncCameraNum(int _nTypeNumber = 0);
	// �״� 
	static void IncRadarNum(int _nTypeNumber = 0);
	// ������
	static void IncTouchkeyNum(int _nTypeNumber = 0);
	// ����  
	static void IncKeyNum(int _nTypeNumber = 0);
	// rfid��
	static void IncRfidNum(int _nTypeNumber = 0);
	// ת��  
	static void IncTurntableNum(int _nTypeNumber = 0);
	// ����  
	static void IncSlidebarNum(int _nTypeNumber = 0);
	// �����
	static void IncSomatosensoryNum(int _nTypeNumber = 0);

	// �������͵������ ETypeID ����ID _nTypeNumber �ڼ�����ť(���͵ĺ���)
	static void IncTypeClickedNum(int _eTypeID, int _nTypeNumber = 0);

	// ����Ϊ��������
	static void Alarm(int eAlarmType, int _nTypeNumber = 0);

	// ����ͷ���� _nTypeNumber �ڼ���(���͵ĺ���)
	static void AlarmCamera(int _nTypeNumber = 0);
	// �����˱���
	static void AlarmRobot(int _nTypeNumber = 0);
	// �忨����
	static void AlarmPciCard(int _nTypeNumber = 0);
};


