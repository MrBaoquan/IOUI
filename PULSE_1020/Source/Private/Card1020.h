#pragma once
#include "Pci1020.h"

#define _MAX_CARD_1020 4

class CCard1020
{
public:
	int m_n1020Count;
	HANDLE m_hDevice1020[_MAX_CARD_1020];  // 
	PCI1020_PARA_DO m_DO_Para[_MAX_CARD_1020];
	BYTE m_bDISts[16], m_bDOSts[16];

	void InitCard();//初始化
	void ExitCard();

	void OpenIO(int nChan, BOOL b);
	BOOL IsIOOpen(int nChan);
	void ActiveAxis(int nChan, int nPulse, int nSpeed); //nChan 轴 nPulse 多少脉冲 负值反向 0  停止  // nSpeed 速度
    void ActiveAxis2(int nChan1, int nPulse1, int nChan2, int nPulse2, int nChan3, int nPulse3, int nSpeed);
    void ActiveAxis3(int nChan1, int nPulse1, int nChan2, int nPulse2, int nChan3, int nPulse3, int nSpeed);
    BOOL GetAxisLimit(int nChan, int nLimit); //获得当前限位信号   nLimit 1 正限位 -1 负限位
	BOOL IsAxisZero(int nChan); //判断是否在原点
	BOOL IsAxisStop(int nChan); //判断电机是否静止
	int GetAxisPos(int nChan);  //获得当前电机位置 
	void ZeroAxisPos(int nChan); //将当前电机位置清零
	CCard1020();
	~CCard1020();
};

