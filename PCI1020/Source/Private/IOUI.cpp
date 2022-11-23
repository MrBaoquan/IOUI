/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include "PCIManager.hpp"
#include "Paths.hpp"
#include "Pci1020.h"

#ifdef WIN_64
#pragma comment(lib,"PCI1020_64.lib")
#else
#pragma comment(lib,"PCI1020_32.lib")
#endif

/** �豸������Ϣ */
DeviceInfo g_DeviceInfo; 

IOUI_API DeviceInfo* __stdcall Initialize()
{
    g_DeviceInfo.InputCount = 32 + 4;
    g_DeviceInfo.OutputCount = 32 + 32 + 4;  // 32�����������ͨ�� + 4���������ͨ��
    g_DeviceInfo.AxisCount = 0;

    return &g_DeviceInfo;
}


PCI1020_PARA_LCData LC;
PCI1020_PARA_DataList DL;

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    // DLL �ӳټ���
#ifdef WIN_64
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\PCI1020_64.DLL";
#else
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\PCI1020_32.DLL";
#endif

    auto _module = LoadLibraryA(_path.data());

    HANDLE hHandle = PCI1020_CreateDevice(deviceIndex);
    if (hHandle!=INVALID_HANDLE_VALUE)
    {
		PCIManager::Instance().AddDevice(deviceIndex, DeviceData(hHandle, g_DeviceInfo));

        LC.AxisNum = PCI1020_XAXIS;		// ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)
        LC.LV_DV = PCI1020_DV;			// ������ʽ PCI1020_DV:�������� PCI1020_LV:��������
        LC.PulseMode = PCI1020_CWCCW;	// ���巽ʽ PCI1020_CWCCW:CW/CCW��ʽ,PCI1020_CPDIR:CP/DIR��ʽ 
        LC.Line_Curve = PCI1020_LINE;	// �˶���ʽPCI1020_LINE:ֱ�߼�/����; PCI1020_CURVE:S���߼�/����)
        DL.Multiple = 1;					// ���� (1~500) 
        DL.Acceleration = 5000;			// ���ٶ�(125~1000,000)(ֱ�߼Ӽ��������м��ٶ�һֱ����)
        DL.Deceleration = 2500;			// ���ٶ�(125~1000,000)(ֱ�߼Ӽ��������м��ٶ�һֱ����)
    //	DL.AccIncRate = 8000;			// ���ٶȱ仯��(��S��������ʱ��Ч)
    //	DL.DecIncRate = 2000; 
        DL.StartSpeed = 100;			// ��ʼ�ٶ�(1~8000)
        DL.DriveSpeed = 8000;			// �����ٶ�	(1~8000)	
    //	LC.DecMode = PCI1020_AUTO;		// ����ģʽ PCI1020_AUTO :�Զ����� PCI1020_HAND:�ֶ�����
        LC.nPulseNum = 0;			// �������������(0~268435455)
        LC.Direction = PCI1020_PDIRECTION;// �˶����� PCI1020_PDIRECTION: ��ת  PCI1020_MDIRECTION:��ת		

        PCI1020_InitLVDV(				//	��ʼ������,������������
            hHandle,
            &DL,
            &LC);
        return 1;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    return PCI1020_ReleaseDevice(hHandle) ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    static PCI1020_PARA_DO Para[4];	// ���������

    const DeviceData* _devData = PCIManager::Instance().GetDeviceData(deviceIndex);
    if (!_devData) { return 0; }
    
    //CopyTo(_devData->DOStatus_Short(), _tempDOStatus,g_DeviceInfo.OutputCount);
    CopyTo(InDOStatus, _devData->DOStatus_Short(), g_DeviceInfo.OutputCount);

    
    for (int nAxisNum = 0; nAxisNum < 4; nAxisNum++)
    {
        // ����X��Y,Z,U������л�(PCI1020_STATUSOUT:״̬��� PCI1020_GENERALOUT:ͨ�����)
        PCI1020_OutSwitch(_devData->handle, nAxisNum, PCI1020_STATUSOUT);

        Para[nAxisNum].OUT0 = InDOStatus [nAxisNum * 4 + 0];
        Para[nAxisNum].OUT1 = InDOStatus[nAxisNum * 4 + 1];
        Para[nAxisNum].OUT2 = InDOStatus[nAxisNum * 4 + 2];
        Para[nAxisNum].OUT3 = InDOStatus[nAxisNum * 4 + 3];
        Para[nAxisNum].OUT4 = InDOStatus[nAxisNum * 4 + 4];
        Para[nAxisNum].OUT5 = InDOStatus[nAxisNum * 4 + 5];
        Para[nAxisNum].OUT6 = InDOStatus[nAxisNum * 4 + 6];
        Para[nAxisNum].OUT7 = InDOStatus[nAxisNum * 4 + 7];
        // ���ø��Ὺ�������
        PCI1020_SetDeviceDO(_devData->handle, nAxisNum, &Para[nAxisNum]);

        // �����źŴ���
        int _pulseChannel = 32 + nAxisNum * 8;
        if (InDOStatus[_pulseChannel]) {
            LC.AxisNum = nAxisNum;
            LC.nPulseNum = InDOStatus[_pulseChannel];

            DL.Multiple = InDOStatus[_pulseChannel + 1] ? InDOStatus[_pulseChannel + 1] : 1;
            DL.Acceleration = InDOStatus[_pulseChannel + 2] ? InDOStatus[_pulseChannel + 2] : 5000;
            DL.Deceleration = InDOStatus[_pulseChannel + 3] ? InDOStatus[_pulseChannel + 3] : 2500;
            DL.StartSpeed = InDOStatus[_pulseChannel + 4] ? InDOStatus[_pulseChannel + 2] : 100;
            DL.DriveSpeed = InDOStatus[_pulseChannel + 5] ? InDOStatus[_pulseChannel + 5] : 8000;

            PCI1020_InitLVDV(				//	��ʼ������,������������
                _devData->handle,
                &DL,
                &LC);
            PCI1020_StartLVDV(_devData->handle, nAxisNum);
        }

        // ��ͣ�źŴ���
        int _stopSignal = 64 + nAxisNum;
        if (InDOStatus[_stopSignal]) {
            PCI1020_InstStop(_devData->handle, nAxisNum);
        }
    }
    
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{	
	/*const DeviceData* _devData = PCIManager::Instance().GetDeviceData(deviceIndex);
	if (!_devData) { return 0; }
    int _retCode = PCI2312A_GetDeviceDO(_devData->handle, _devData->DOStatus()) ? 1 : 0;
	CopyTo(_devData->DOStatus(), OutDOStatus, g_DeviceInfo.OutputCount);*/
	return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    static PCI1020_PARA_RR3  ParaRR3;    // ���������� 
    static PCI1020_PARA_RR4  ParaRR4;    // ����������

    HANDLE hHandle = PCIManager::Instance().GetHandle(deviceIndex);
    ZeroMemory(OutDIStatus, g_DeviceInfo.InputCount);

    if (PCI1020_GetRR3Status(hHandle, &ParaRR3)) {
        OutDIStatus[0] = ParaRR3.XIN0;
        OutDIStatus[1] = ParaRR3.XIN1;
        OutDIStatus[2] = ParaRR3.XIN2;
        OutDIStatus[3] = ParaRR3.XIN3;
        OutDIStatus[4] = ParaRR3.XEXPP;
        OutDIStatus[5] = ParaRR3.XEXPM;
        OutDIStatus[6] = ParaRR3.XINPOS;
        OutDIStatus[7] = ParaRR3.XALARM;

        OutDIStatus[8] = ParaRR3.YIN0;
        OutDIStatus[9] = ParaRR3.YIN1;
        OutDIStatus[10] = ParaRR3.YIN2;
        OutDIStatus[11] = ParaRR3.YIN3;
        OutDIStatus[12] = ParaRR3.YEXPP;
        OutDIStatus[13] = ParaRR3.YEXPM;
        OutDIStatus[14] = ParaRR3.YINPOS;
        OutDIStatus[15] = ParaRR3.YALARM;
    }

    if (PCI1020_GetRR4Status(hHandle, &ParaRR4)) {
        OutDIStatus[16] = ParaRR4.ZIN0;
        OutDIStatus[17] = ParaRR4.ZIN1;
        OutDIStatus[18] = ParaRR4.ZIN2;
        OutDIStatus[19] = ParaRR4.ZIN3;
        OutDIStatus[20] = ParaRR4.ZEXPP;
        OutDIStatus[21] = ParaRR4.ZEXPM;
        OutDIStatus[22] = ParaRR4.ZINPOS;
        OutDIStatus[23] = ParaRR4.ZALARM;

        OutDIStatus[24] = ParaRR4.UIN0;
        OutDIStatus[25] = ParaRR4.UIN1;
        OutDIStatus[26] = ParaRR4.UIN2;
        OutDIStatus[27] = ParaRR4.UIN3;
        OutDIStatus[28] = ParaRR4.UEXPP;
        OutDIStatus[29] = ParaRR4.UEXPM;
        OutDIStatus[30] = ParaRR4.UINPOS;
        OutDIStatus[31] = ParaRR4.UALARM;
    }

    static PCI1020_PARA_RR0 ParaRR0;
    if (PCI1020_GetRR0Status(hHandle, &ParaRR0)) {
        OutDIStatus[32] = ParaRR0.XDRV;
        OutDIStatus[33] = ParaRR0.YDRV;
        OutDIStatus[34] = ParaRR0.ZDRV;
        OutDIStatus[35] = ParaRR0.UDRV;
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
