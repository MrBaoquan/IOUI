#if !defined(EM9703_H)
#define EM9703_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZT_Type.h"


#define EM9703_USB_MAX_COUNT                        16 //����֧��USBģ�����
#define EM9703_BASE_FREQ                            50000000.0//����ʱ�ӣ�50M
#define EM9703_PWM_BASE_FREQ                        10000000.0//PWM����ʱ�ӣ�10M
#define EM9703_MAXIOCHCNT                           64 //IO���ͨ����
#define EM9703_MAXCTCHCNT                           16 //���������ͨ����
#define EM9703_MAXECCHCNT                           4 //���������ͨ����
#define EM9703_MAXPWMCHCNT                          8 //PWM���ͨ����

//������ʽ
#define EM9703_STARTMode_ONCE                       0 //���Σ�EM9703��ÿ100ms����һ��ת�����û����Զ������һ�ε�ת�����
#define EM9703_STARTMode_ICLK                       1 //��ʱ�������ɼ�
#define EM9703_STARTMode_ECLK                       2 //��ʱ�������ɼ�

//������������ʽ
#define EM9703_CT_MODE_COUNT                         0 //����
#define EM9703_CT_MODE_HFREQ                         1 //���Ƶ
#define EM9703_CT_MODE_LFREQ                         3 //���Ƶ

//IO����
#define EM9703_IO_IN                                 0 //����
#define EM9703_IO_OUT                                1 //���

extern "C"
{
	//����һ���豸
	HANDLE _stdcall EM9703_CreateDevice();
	//�ر��豸��ͬʱ�ͷ��豸ռ�õ��ڴ���Դ
	void _stdcall EM9703_CloseDevice(HANDLE pDev);
	//�õ������
	I32 _stdcall EM9703_GetErrorCode(HANDLE pDev);
	//��������˿�
	I32 _stdcall EM9703_CmdConnect( HANDLE pDev, char* strIP, int port );
	//�ر�����˿�
	void _stdcall EM9703_CmdClose( HANDLE pDev );
	//ʹ��ͨѶ���Ź�,����λ����overTimeS����û�н��յ��κ��������ΪͨѶ�жϣ���ʱ��λ����Ҫ���µ���EM9703_CmdConnect������
	I32 _stdcall EM9703_CmdEnableWTD( HANDLE pDev, U16 overTimeS );
	//�������ݶ˿�
	I32 _stdcall EM9703_DataConnect( HANDLE pDev, int port );
	//�ر����ݶ˿�
	void _stdcall EM9703_DataClose( HANDLE pDev );
	
	//�趨��ʱ�ɼ�Ƶ��
	I32 _stdcall EM9703_HCSetFreq( HANDLE pDev,  F64 groupFreq, F64* realFreq );
	//�����ɼ�����ʱ���ⴥ����
	I32 _stdcall EM9703_HCStart( HANDLE pDev, I32 startMode );
	//ֹͣ�ɼ�����ʱ���ⴥ����
	I32 _stdcall EM9703_HCStop( HANDLE pDev );
	//�����ݶ˿ڶ�ȡ��λ���Ķ�ʱ�ɼ�����
	I32 _stdcall EM9703_HCReadCode( HANDLE pDev, I32 dataCount, U8* dataBuffer, I32 outtime = 500 );
	
	//���ÿ���������ע��˺�����EM9703N��Ч
	I32 _stdcall EM9703_IoSetDir( HANDLE pDev, I8 dir[2] );
	//�õ����п�������״̬��ע��iStatus[0]~iStatus[31]��ӦDI1~DI32,iStatus[32]~iStatus[63]��ӦDO1~DO32,
	I32 _stdcall EM9703_IoGetAll( HANDLE pDev, I8 iStatus[EM9703_MAXIOCHCNT] );
	//�������п�������״̬��ע��oStatus[0]~oStatus[31]��ӦDI1~DI32,oStatus[32]~oStatus[63]��ӦDO1~DO32,
	I32 _stdcall EM9703_IoSetAll( HANDLE pDev, I8 oStatus[EM9703_MAXIOCHCNT] );
	//������λ���ϵ�ʱ��ֵ��ע��oStatus[0]~oStatus[31]��ӦDI1~DI32,oStatus[32]~oStatus[63]��ӦDO1~DO32
	I32 _stdcall EM9703_IoSetInitStatus( HANDLE pDev, I8 initStatus[EM9703_MAXIOCHCNT] );
	//�õ���λ���ϵ�ʱ��ֵ��ע��oStatus[0]~oStatus[31]��ӦDI1~DI32,oStatus[32]~oStatus[63]��ӦDO1~DO32
	I32 _stdcall EM9703_IoGetInitStatus( HANDLE pDev, I8 initStatus[EM9703_MAXIOCHCNT] );
	
	//������������ʽ
	I32 _stdcall EM9703_CtSetModeAll( HANDLE pDev, I8 ctMode[EM9703_MAXCTCHCNT] );
	//���������
	I32 _stdcall EM9703_CtClear( HANDLE pDev, I32 chNo );
	//�������м�����
	I32 _stdcall EM9703_CtClearAll( HANDLE pDev );
	//���ò�Ƶ��׼����msΪ��λ��ֻ�е������������ڲ�Ƶ��ʽ��ʱ������
	I32 _stdcall EM9703_CtSetFreqBase( HANDLE pDev, I32 chNo, F64 freqBase_ms, F64* real_ms );
	//��������ͨ���ļ�������ǰֵ
	I32 _stdcall EM9703_CtReadCodeAll( HANDLE pDev, U32 ctCode[EM9703_MAXCTCHCNT] );
	
	//��������ͨ���ı�����ֵ
	I32 _stdcall EM9703_EcReadAll( HANDLE pDev, I32 encoderAB[EM9703_MAXECCHCNT], I32 encoderZ[EM9703_MAXECCHCNT] );
	//���������㣬isClear[0]~isClear[3]��Ӧ������1~������4,0��ʾ�����㣬1��ʾ����
	I32 _stdcall EM9703_EcClearAll( HANDLE pDev, I8 isClear[EM9703_MAXECCHCNT] );
	
	//����ָ��ͨ����PWM����������������������Ϊ0�����ʾ����PWM��һֱ�����
	I32 _stdcall EM9703_PwmSetCount( HANDLE pDev, I32 chNo, U32 setCount );
	//PWM�����Ƿ������ָ������
	I32 _stdcall EM9703_PwmIsOver( HANDLE pDev, I8 isOver[EM9703_MAXPWMCHCNT] );
	//����PWM��������ں�ռ�ձ�
	I32 _stdcall EM9703_PwmSetPulse( HANDLE pDev, I32 chNo, double freq, double dutyCycle, double* realFreq, double* realDutyCycle );
	//��������ֹͣPWM�����is90��ʾ��Ӧ��ͨ���Ƿ���ʱ90��
	I32 _stdcall EM9703_PwmStartAll( HANDLE pDev, I8 startOrStop[EM9703_MAXPWMCHCNT], I8 is90[EM9703_MAXPWMCHCNT] );
	
	//�õ���λ��USB��������������
	I32 _stdcall EM9703_GetUSBStatus( char usbName[][256], unsigned char ip[][4], long ipCount );
	//�õ���λ��USB��������������
	I32 _stdcall EM9703_SetUSBHostIp( long devInx, char* ip );
	//������λ��USB����������IP��ַ
	I32 _stdcall EM9703_SetClientIP( HANDLE pDev, const char* strIP );
};

#endif // !defined(EM9703_H)
