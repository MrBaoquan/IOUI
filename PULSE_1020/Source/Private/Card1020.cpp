#include "Card1020.h"


#pragma comment(lib, "PCI1020_64.lib")
CCard1020::CCard1020()
{
	memset(m_bDOSts, 0x00, sizeof(m_bDOSts));
	memset(m_bDISts, 0x00, sizeof(m_bDISts));
	memset(m_DO_Para, 0x00, sizeof(m_DO_Para));
	for(int i = 0 ; i < _MAX_CARD_1020 ; i ++)
		m_hDevice1020[i] = NULL;
}


CCard1020::~CCard1020()
{
}

void CCard1020::InitCard()
{
	for (int i = 0; i < _MAX_CARD_1020; i++)
	{
		m_hDevice1020[i] = PCI1020_CreateDevice(i); // 创建设备对象，保存在App中，可供其他任何子窗体或子功能使用
		if (m_hDevice1020[i] != INVALID_HANDLE_VALUE)
		{
			PCI1020_SetDeviceDO(m_hDevice1020[i], PCI1020_XAXIS, &m_DO_Para[i]);
			PCI1020_SetDeviceDO(m_hDevice1020[i], PCI1020_YAXIS, &m_DO_Para[i]);
			PCI1020_SetDeviceDO(m_hDevice1020[i], PCI1020_ZAXIS, &m_DO_Para[i]);
			PCI1020_SetDeviceDO(m_hDevice1020[i], PCI1020_UAXIS, &m_DO_Para[i]);
		}
		if(i == 0)
			m_n1020Count = PCI1020_GetDeviceCount(m_hDevice1020[i]);
	}
}

void CCard1020::ExitCard()
{
	for (int i = 0; i < _MAX_CARD_1020; i++)
	{
		if (m_hDevice1020[i] != INVALID_HANDLE_VALUE)
			PCI1020_ReleaseDevice(m_hDevice1020[i]);
	}
}

void CCard1020::OpenIO(int nChan, BOOL bOpen)
{
	int nCard = nChan / 32;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return;
	nChan = nChan % 32;
	int nAxis = nChan / 8;
	nChan = nChan % 8;
	switch (nChan)
	{
	case 0:
		m_DO_Para[nCard].OUT0 = bOpen;
		break;
	case 1:
		m_DO_Para[nCard].OUT1 = bOpen;
		break;
	case 2:
		m_DO_Para[nCard].OUT2 = bOpen;
		break;
	case 3:
		m_DO_Para[nCard].OUT3 = bOpen;
		break;
	case 4:
		m_DO_Para[nCard].OUT4 = bOpen;
		break;
	case 5:
		m_DO_Para[nCard].OUT5 = bOpen;
		break;
	case 6:
		m_DO_Para[nCard].OUT6 = bOpen;
		break;
	case 7:
		m_DO_Para[nCard].OUT7 = bOpen;
		break;
	default:
		break;
	}
	PCI1020_SetDeviceDO(hDevice, nAxis, &m_DO_Para[nCard]);
}

BOOL CCard1020::IsIOOpen(int nChan)
{
	int nCard = nChan / 32;
	BOOL bRes = FALSE;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;
	PCI1020_PARA_RR3 rr3;
	PCI1020_PARA_RR4 rr4;
	nChan = nChan % 32;
	int nAxis = nChan / 16;
	nChan = nChan % 16;
	if (nAxis == 0)
	{
		PCI1020_GetRR3Status(hDevice, &rr3);
		switch (nChan)
		{
		case 0:
			return rr3.XIN0;
		case 1:
			return rr3.XIN1;
		case 2:
			return rr3.XIN2;
		case 3:
			return rr3.XIN3;
		case 4:
			return rr3.XEXPP;
		case 5:
			return rr3.XEXPM;
		case 6:
			return rr3.XINPOS;
		case 7:
			return rr3.XALARM;
		case 8:
			return rr3.YIN0;
		case 9:
			return rr3.YIN1;
		case 10:
			return rr3.YIN2;
		case 11:
			return rr3.YIN3;
		case 12:
			return rr3.YEXPP;
		case 13:
			return rr3.YEXPM;
		case 14:
			return rr3.YINPOS;
		case 15:
			return rr3.YALARM;
		default:
			break;
		}
	}
	else
	{
		PCI1020_GetRR4Status(hDevice, &rr4);
		switch (nChan)
		{
		case 0:
			return rr4.ZIN0;
		case 1:
			return rr4.ZIN1;
		case 2:
			return rr4.ZIN2;
		case 3:
			return rr4.ZIN3;
		case 4:
			return rr4.ZEXPP;
		case 5:
			return rr4.ZEXPM;
		case 6:
			return rr4.ZINPOS;
		case 7:
			return rr4.ZALARM;
		case 8:
			return rr4.UIN0;
		case 9:
			return rr4.UIN1;
		case 10:
			return rr4.UIN2;
		case 11:
			return rr4.UIN3;
		case 12:
			return rr4.UEXPP;
		case 13:
			return rr4.UEXPM;
		case 14:
			return rr4.UINPOS;
		case 15:
			return rr4.UALARM;
		default:
			break;
		}
	}
	return FALSE;
}

void CCard1020::ActiveAxis(int nChan, int nPulse, int nSpeed)
{
	int nCard = nChan / 4;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return;
	nChan /= 4;

	PCI1020_PARA_DataList para1020;
	PCI1020_PARA_LCData data1020;

	para1020.Multiple = 1;            // 倍率
	para1020.StartSpeed = 1000;       // 初始速度
	para1020.DriveSpeed = nSpeed;       // 驱动速度
	para1020.Acceleration = 3000;     // 加速度
	para1020.Deceleration = 3000;

	data1020.AxisNum = nChan;
	data1020.Line_Curve = 0;          // 直线运动
	data1020.PulseMode = PCI1020_CPDIR;          // 直线运动
	data1020.LV_DV = PCI1020_DV;		  // 固定线速度
	data1020.DecMode = 0;		  // 固定线速度
	data1020.nPulseNum = 0;  // X轴插补脉冲数


	if (nPulse >= 0)
	{
		data1020.Direction = PCI1020_PDIRECTION;
		data1020.nPulseNum = nPulse;
	}
	else
	{
		data1020.Direction = PCI1020_MDIRECTION;
		data1020.nPulseNum = -nPulse;
	}
	if (nPulse == 0)
		PCI1020_InstStop(hDevice, data1020.AxisNum);
	else
	{
		PCI1020_InitLVDV(hDevice, &para1020, &data1020);
		PCI1020_StartLVDV(hDevice, data1020.AxisNum);
	}
}


void CCard1020::ActiveAxis2(int nChan1, int nPulse1, int nChan2, int nPulse2, int nChan3, int nPulse3, int nSpeed)
{
    PCI1020_PARA_DataList para1020;
    PCI1020_PARA_LineData paraLine;
    PCI1020_PARA_InterpolationAxis paraAxis;

    para1020.Multiple = 1;            // 倍率
    para1020.StartSpeed = 1000;       // 初始速度
    para1020.DriveSpeed = nSpeed;       // 驱动速度
    para1020.Acceleration = 3000;     // 加速度
    para1020.Deceleration = 3000;

    paraAxis.Axis1 = nChan1;
    paraAxis.Axis2 = nChan2;

    paraLine.Line_Curve = PCI1020_LINE;
    paraLine.ConstantSpeed = 0;
    paraLine.n1AxisPulseNum = nPulse1;
    paraLine.n2AxisPulseNum = nPulse2;

    PCI1020_InitLineInterpolation_2D(m_hDevice1020, &para1020, &paraAxis, &paraLine);
    PCI1020_StartLineInterpolation_2D(m_hDevice1020);
}

void CCard1020::ActiveAxis3(int nChan1, int nPulse1, int nChan2, int nPulse2, int nChan3, int nPulse3, int nSpeed)
{
    PCI1020_PARA_DataList para1020;
    PCI1020_PARA_LineData paraLine;
    PCI1020_PARA_InterpolationAxis paraAxis;

    para1020.Multiple = 1;            // 倍率
    para1020.StartSpeed = 1000;       // 初始速度
    para1020.DriveSpeed = nSpeed;       // 驱动速度
    para1020.Acceleration = 3000;     // 加速度
    para1020.Deceleration = 3000;

    paraAxis.Axis1 = nChan1;
    paraAxis.Axis2 = nChan2;
    paraAxis.Axis3 = nChan3;

    paraLine.Line_Curve = PCI1020_LINE;
    paraLine.ConstantSpeed = 0;
    paraLine.n1AxisPulseNum = nPulse1;
    paraLine.n2AxisPulseNum = nPulse2;
    paraLine.n3AxisPulseNum = nPulse3;

    PCI1020_InitLineInterpolation_3D(m_hDevice1020, &para1020, &paraAxis, &paraLine);
    PCI1020_StartLineInterpolation_3D(m_hDevice1020);
}

BOOL CCard1020::GetAxisLimit(int nChan, int nLimit)
{
	int nCard = nChan / 4;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;
	nChan /= 4;

	PCI1020_PARA_RR1 status;
	if (nChan >= 0)
	{
		PCI1020_GetRR1Status(hDevice, nChan, &status);

		if (nLimit == -1)
			return (status.LMTM == 1);
		if (nLimit == 1)
			return (status.LMTP == 1);
	}
	return FALSE;
}

BOOL CCard1020::IsAxisZero(int nChan)
{
	BOOL bRes = FALSE;
	int nCard = nChan / 4;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;
	nChan /= 4;

	PCI1020_PARA_RR3  RR3;			// 状态寄存器RR3
	PCI1020_PARA_RR4  RR4;			// 状态寄存器RR3

	if (nChan == PCI1020_XAXIS)
	{
		PCI1020_GetRR3Status(		 // 获得主状态寄存器RR0的位状态
			hDevice,		 // 设备句柄
			&RR3);
		return RR3.XINPOS == 0;
	}
	if (nChan == PCI1020_YAXIS)
	{
		PCI1020_GetRR3Status(		 // 获得主状态寄存器RR0的位状态
			hDevice,		 // 设备句柄
			&RR3);
		return RR3.YINPOS == 0;
	}
	if (nChan == PCI1020_ZAXIS)
	{
		PCI1020_GetRR4Status(		 // 获得主状态寄存器RR0的位状态
			hDevice,		 // 设备句柄
			&RR4);
		return RR4.ZINPOS == 0;
	}
	if (nChan == PCI1020_UAXIS)
	{
		PCI1020_GetRR4Status(		 // 获得主状态寄存器RR0的位状态
			hDevice,		 // 设备句柄
			&RR4);
		return RR4.UINPOS == 0;
	}
	return bRes;
}

BOOL CCard1020::IsAxisStop(int nChan)
{
	BOOL bRes = FALSE;
	int nCard = nChan / 4;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;
	nChan /= 4;

	PCI1020_PARA_RR0  RR0;			// 状态寄存器RR3
	PCI1020_GetRR0Status(		 // 获得主状态寄存器RR0的位状态
		hDevice,		 // 设备句柄
		&RR0);
	if (nChan == PCI1020_XAXIS)
	{
		return RR0.XDRV == 0;
	}
	if (nChan == PCI1020_YAXIS)
	{
		return RR0.YDRV == 0;
	}
	if (nChan == PCI1020_ZAXIS)
	{
		return RR0.ZDRV == 0;
	}
	if (nChan == PCI1020_UAXIS)
	{
		return RR0.UDRV == 0;
	}
	return bRes;
}

int CCard1020::GetAxisPos(int nChan)
{
	int nCard = nChan / 4;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return 0;
	nChan /= 4;
	return PCI1020_ReadLP(hDevice, nChan);
}

void CCard1020::ZeroAxisPos(int nChan)
{
	int nCard = nChan / 4;
	HANDLE hDevice = m_hDevice1020[nCard];
	if (hDevice == INVALID_HANDLE_VALUE)
		return;
	PCI1020_SetLP(hDevice, nChan, 0);
}
