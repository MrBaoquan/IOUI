/**************************************************************************
// Ӳ�����ò���
**************************************************************************/
#include "windows.h"

// ���ò��� 
#ifndef _PCI1020_PAPA_DataList
typedef struct _PCI1020_PAPA_DataList
{	 
	LONG Multiple;				// ���� (1~500)
	LONG StartSpeed;			// ��ʼ�ٶ�(1~8000)
	LONG DriveSpeed;			// �����ٶ�(1~8000)
	LONG Acceleration;			// ���ٶ�(125~1000000)
	LONG Deceleration;			// ���ٶ�(125~1000000)
	LONG AccIncRate;			// ���ٶȱ仯��(954~62500000)
	LONG DecIncRate;			// ���ٶȱ仯��(954~62500000)
} PCI1020_PARA_DataList, *PPCI1020_PARA_DataList;
#endif

// ֱ�ߺ�S���߲���
#ifndef _PCI1020_PAPA_LCData
typedef struct _PCI1020_PAPA_LCData
{
	LONG AxisNum;				// ��� (X�� | Y�� | X��Y��)
	LONG LV_DV;					// ������ʽ  (���� | ���� )
	LONG DecMode;				// ���ٷ�ʽ  (�Զ����� | �ֶ�����)	
	LONG PulseMode;				// ���巽ʽ (CW/CCW��ʽ | CP/DIR��ʽ)
	LONG Line_Curve;			// �˶���ʽ	(ֱ�� | ����)
	LONG Direction;				// �˶����� (������ | ������)
	LONG nPulseNum;		    	// �������������(0~268435455)
} PCI1020_PARA_LCData, *PPCI1020_PARA_LCData;
#endif

// �岹��
#ifndef _PCI1020_PAPA_InterpolationAxis
typedef struct _PCI1020_PAPA_InterpolationAxis
{	
	LONG Axis1;					// ����
	LONG Axis2;					// �ڶ���
	LONG Axis3;					// ������
} PCI1020_PARA_InterpolationAxis, *PPCI1020_PARA_InterpolationAxis;
#endif

// ֱ�߲岹�͹̶����ٶ�ֱ�߲岹����
#ifndef _PCI1020_PAPA_LineData
typedef struct _PCI1020_PAPA_LineData	
{	
	LONG Line_Curve;			// �˶���ʽ	(ֱ�� | ����)
	LONG ConstantSpeed;			// �̶����ٶ� (���̶����ٶ� | �̶����ٶ�)
	LONG n1AxisPulseNum;		// �����յ������� (-8388608~8388607)
	LONG n2AxisPulseNum;		// �ڶ������յ������� (-8388608~8388607)
	LONG n3AxisPulseNum;		// ���������յ������� (-8388608~8388607)		
} PCI1020_PARA_LineData, *PPCI1020_PARA_LineData;
#endif

// ��������Բ���岹����
#ifndef _PCI1020_PAPA_CircleData
typedef struct _PCI1020_PAPA_CircleData	
{
	LONG ConstantSpeed;			// �̶����ٶ� (���̶����ٶ� | �̶����ٶ�)
	LONG Direction;				// �˶����� (������ | ������)
	LONG Center1;				// ����Բ������(������-8388608~8388607)
    LONG Center2;				// �ڶ�����Բ������(������-8388608~8388607)
	LONG Pulse1;				// �����յ�����(������-8388608~8388607)	
	LONG Pulse2;				// �ڶ������յ�����(������-8388608~8388607)
} PCI1020_PARA_CircleData, *PPCI1020_PARA_CircleData;
#endif

/***************************************************************/
// ���
#define		PCI1020_XAXIS			0X0				// X��
#define		PCI1020_YAXIS			0x1				// Y��
#define		PCI1020_ZAXIS			0x2				// Z��
#define		PCI1020_UAXIS			0x3				// U��
#define		PCI1020_ALLAXIS			0xF				// ������

/***************************************************************/
// ������ʽ
#define		PCI1020_DV				0x0				// ��������
#define		PCI1020_LV				0x1				// ��������

/***************************************************************/
// ���ٷ�ʽ
#define		PCI1020_AUTO			0x0				// �Զ�����
#define		PCI1020_HAND			0x1				// �ֶ�����

/***************************************************************/
// ���������ʽ
#define 	PCI1020_CWCCW			0X0				// CW/CCW��ʽ 
#define 	PCI1020_CPDIR 			0X1				// CP/DIR��ʽ

/***************************************************************/
// �������뷽ʽ
#define 	PCI1020_A_B			    0X0				// A/B�෽ʽ
#define 	PCI1020_U_D 			0X1				// ��/���������뷽ʽ

/***************************************************************/
// �˶���ʽ
#define		PCI1020_LINE			0X0				// ֱ���˶�
#define		PCI1020_CURVE			0X1				// S�����˶�

/***************************************************************/
// �˶�����
#define		PCI1020_MDIRECTION		0x0				// ������
#define		PCI1020_PDIRECTION		0x1				// ������

/***************************************************************/
//�̶����ٶ�
#define		PCI1020_NOCONSTAND		0X0				// ���̶����ٶ�
#define		PCI1020_CONSTAND		0X1				// �̶����ٶ�

/***************************************************************/
// ������λ���߼�ʵλ������ѡ��������ⲿԽ���źŵ�ֹͣ��ʽ�������ⲿֹͣ�źŵ�ֹͣ��ѡ��
/***************************************************************/
// ���������
#define		PCI1020_LOGIC			0x0				// �߼�λ������
#define		PCI1020_FACT			0x1				// ʵλ������

/***************************************************************/
// �ⲿֹͣ�ź�
#define 	PCI1020_IN0				0X0				// ֹͣ�ź�0
#define 	PCI1020_IN1				0X1				// ֹͣ�ź�1
#define 	PCI1020_IN2				0X2				// ֹͣ�ź�2
#define 	PCI1020_IN3				0X3				// ֹͣ�ź�3

/***************************************************************/
// ֹͣ��ʽ
#define		PCI1020_SUDDENSTOP		0x0				// ����ֹͣ
#define		PCI1020_DECSTOP			0X1				// ����ֹͣ

/********************************************************************/
// ����л�
#define		PCI1020_GENERALOUT		0x0				// ͨ�����
#define		PCI1020_STATUSOUT		0X1				// ״̬���

/********************************************************************/
#define		PCI1020_ERROR			0XFF			// ����

/****************************************************************/
// �����ж�λʹ��
#ifndef _PCI1020_PARA_Interrupt
typedef struct _PCI1020_PARA_Interrupt      
{
	UINT PULSE;			// 1���ж�ʹ�ܣ��ж��ź��ɸ���������������ش��� 0����ֹ�ж�
	UINT PBCM;			// 1���ж�ʹ�ܣ����߼�/ʵ��λ�ü�������ֵ���ڵ���COMP-�Ĵ�����ֵʱ���ж��ź� 0����ֹ�ж�
	UINT PSCM;			// 1���ж�ʹ�ܣ����߼�/ʵ��λ�ü�������ֵС��COMP-�Ĵ�����ֵʱ���ж��ź� 0����ֹ�ж�
	UINT PSCP;			// 1���ж�ʹ�ܣ����߼�/ʵ��λ�ü�������ֵС��COMP+�Ĵ�����ֵʱ���ж��ź� 0����ֹ�ж�
	UINT PBCP;			// 1���ж�ʹ�ܣ����߼�/ʵ��λ�ü�������ֵ���ڵ���COMP+�Ĵ�����ֵ���ж��ź� 0����ֹ�ж�
	UINT CDEC;			// 1���ж�ʹ�ܣ��ڼ�/���������У�����ʼ����ʱ���ж��ź� 0����ֹ�ж�
	UINT CSTA;			// 1���ж�ʹ�ܣ��ڼ�/���������У�����ʼ����ʱ���ж��ź� 0����ֹ�ж�
	UINT DEND;			// 1���ж�ʹ�ܣ�����������ʱ���ж��ź� 0����ֹ�ж�
	UINT CIINT;			// 1���ж�ʹ�ܣ�������д����һ���ڵ�����ʱ�����ж� 0����ֹ�ж�
	UINT BPINT;			// 1���ж�ʹ�ܣ���λ�岹��ջ��������ֵ��2��Ϊ1ʱ�����ж� 0����ֹ�ж�

} PCI1020_PARA_Interrupt,*PPCI1020_PARA_Interrupt;
#endif

// ����ͬ������(����)
#ifndef _PCI1020_PARA_SynchronActionOwnAxis
typedef struct _PCI1020_PARA_SynchronActionOwnAxis    
{
	UINT PBCP;			// 1�����߼�/ʵλ��������ֵ���ڵ���COMP+�Ĵ���ʱ������ͬ������ 0����Ч
	UINT PSCP;			// 1�����߼�/ʵλ��������ֵС��COMP+�Ĵ���ʱ������ͬ������ 0����Ч
	UINT PSCM;			// 1�����߼�/ʵλ��������ֵС��COMP-�Ĵ���ʱ������ͬ������ 0����Ч
	UINT PBCM;			// 1�����߼�/ʵλ��������ֵ���ڵ���COMP-�Ĵ���ʱ������ͬ������ 0����Ч
	UINT DSTA;			// 1����������ʼʱ������ͬ������ 0����Ч
	UINT DEND;			// 1������������ʱ������ͬ������ 0����Ч
	UINT IN3LH;			// 1����IN3����������ʱ������ͬ������ 0����Ч
	UINT IN3HL;			// 1����IN3�����½���ʱ������ͬ������ 0����Ч
	UINT LPRD;			// 1�������߼�λ�ü�����ʱ������ͬ������ 0����Ч
	UINT CMD;			// 1����д��ͬ����������ʱ������ͬ�����ͬ������ 0����Ч
	UINT AXIS1;			// 1��ָ�����Լ���ͬ������  0��û��ָ��
	UINT AXIS2;			// 1��ָ�����Լ���ͬ������  0��û��ָ��
	UINT AXIS3;			// 1��ָ�����Լ���ͬ������  0��û��ָ��
						// ��ǰ��	AXIS3		AXIS2		AXIS1  
						// X��		 U��		 Z��		 Y��
						// Y��		 X��		 U��		 Z��
						// Z��		 Y��		 X��		 U��
						// U��		 Z��		 Y��		 X��
} PCI1020_PARA_SynchronActionOwnAxis,*PPCI1020_PARA_SynchronActionOwnAxis;
#endif

// ����ͬ������(������)
#ifndef _PCI1020_PARA_SynchronActionOtherAxis
typedef struct _PCI1020_PARA_SynchronActionOtherAxis    
{
	UINT FDRVP;			// 1�����������򶨳����� 0����Ч
	UINT FDRVM;			// 1�����������򶨳����� 0����Ч
	UINT CDRVP;			// 1�������������������� 0����Ч
	UINT CDRVM;			// 1�������������������� 0����Ч
	UINT SSTOP;			// 1������ֹͣ 0����Ч
	UINT ISTOP;			// 1������ֹͣ 0����Ч
	UINT LPSAV;			// 1���ѵ�ǰ�߼��Ĵ���LPֵ���浽ͬ������Ĵ���BR 0����Ч
	UINT EPSAV;			// 1���ѵ�ǰʵλ�Ĵ���EPֵ���浽ͬ������Ĵ���BR 0����Ч
	UINT LPSET;			// 1����WR6��WR7��ֵ�趨���߼��Ĵ���LP�� 0����Ч
	UINT EPSET;			// 1����WR6��WR7��ֵ�趨���߼��Ĵ���EP�� 0����Ч 
	UINT OPSET;			// 1����WR6��WR7��ֵ�趨���߼��Ĵ���LP�� 0����Ч
	UINT VLSET;			// 1����WR6��ֵ�趨Ϊ�����ٶ�V 0����Ч
	UINT OUTN;			// 1����nDCC�������ͬ������  0��nDCC���ͬ��������Ч������
	UINT INTN;			// 1�������ж�  0���������ж�
} PCI1020_PARA_SynchronActionOtherAxis,*PPCI1020_PARA_SynchronActionOtherAxis;
#endif

// ������������
#ifndef _PCI1020_PARA_ExpMode
typedef struct _PCI1020_PARA_ExpMode
{
	UINT EPCLR;			// 1����IN2������Чʱ���ʵλ������ 0����Ч
	UINT FE0;			// 1���ⲿ�����ź�EMGN��nLMTP��nLMTM��nIN0��nIN1�˲�����Ч 0����Ч
	UINT FE1;			// 1���ⲿ�����ź�nIN2�˲�����Ч 0����Ч
	UINT FE2;			// 1���ⲿ�����ź�nALARM��nINPOS�˲�����Ч 0����Ч
	UINT FE3;			// 1���ⲿ�����ź�nEXPP��nEXPM��EXPLS�˲�����Ч 0����Ч
	UINT FE4;			// 1���ⲿ�����ź�nIN3�˲�����Ч 0����Ч
	UINT FL0;			// �˲�����ʱ�䳣�� 
						//	FL2 FL1 FL0	 �˲���ʱ�䳣��	 �ź��ӳ�
	UINT FL1;			//		0��			1.75��S			2��S
	UINT FL2;			//		1��			224��S			256��S
						//		2��			448��S			512��S
						//		3��			896��S			1.024m��S
						//		4��			1.792mS			2.048mS
						//		5��			3.584mS			4.096mS
						//		6��			7.168mS			8.012mS
						//		7��			14.336mS		16.384mS
} PCI1020_PARA_ExpMode,*PPCI1020_PARA_ExpMode;
#endif


// ƫ��������������
#ifndef _PCI1020_PARA_DCC
typedef struct _PCI1020_PARA_DCC
{
	UINT DCCE;			// 1��ʹ��ƫ������������� 0����Ч
	UINT DCCL;			// 1��ƫ����������������߼���ƽΪ�͵�ƽ  0��ƫ����������������߼���ƽΪ�ߵ�ƽ
	UINT DCCW0;			// ����ָ��ƫ����������������������
	UINT DCCW1;			//  DCCW2 DCCW1 DCCW0 ������������(��S)
	UINT DCCW2;			// 	  0		0	  0		  10         	  1	 0  0		1000
						// 	  0		0	  1		  20			  1	 0  1		2000
						// 	  0		1	  0		  100			  1	 1  0		10000
						// 	  0		1	  1		  200			  1	 1  1		20000
} PCI1020_PARA_DCC,*PPCI1020_PARA_DCC;
#endif

// �Զ�ԭ����Ѱ����
#ifndef _PCI1020_PARA_AutoHomeSearch
typedef struct _PCI1020_PARA_AutoHomeSearch
{
	UINT ST1E;			// 1����һ��ʹ�� 0����Ч
	UINT ST1D;			// ��һ������Ѱ��ת���� 0��������  1��������
	UINT ST2E;			// 1���ڶ���ʹ�� 0����Ч
	UINT ST2D;			// �ڶ�������Ѱ��ת���� 0��������  1��������
	UINT ST3E;			// 1��������ʹ�� 0����Ч
	UINT ST3D;			// ����������Ѱ��ת���� 0��������  1��������
	UINT ST4E;			// 1�����Ĳ�ʹ�� 0����Ч
	UINT ST4D;			// ���Ĳ�����Ѱ��ת���� 0��������  1��������
	UINT PCLR;			// 1�����Ĳ�����ʱ����߼���������ʵλ������ 0����Ч
	UINT SAND;			// 1��ԭ���źź�Z���ź���Чʱֹͣ���������� 0����Ч 
	UINT LIMIT;			// 1������Ӳ����λ�ź�(nLMTP��nLMPM)����ԭ����Ѱ 0����Ч
	UINT HMINT;			// 1�����Զ�ԭ����������ʱ�����ж� 0����Ч
} PCI1020_PARA_AutoHomeSearch,*PPCI1020_PARA_AutoHomeSearch;
#endif

// IO���
#ifndef _PCI1020_PARA_DO
typedef struct _PCI1020_PARA_DO      
{
	UINT OUT0;			// ���0
	UINT OUT1;			// ���1
	UINT OUT2;			// ���2
	UINT OUT3;			// ���3
	UINT OUT4;			// ���4
	UINT OUT5;			// ���5
	UINT OUT6;			// ���6
	UINT OUT7;			// ���7
} PCI1020_PARA_DO,*PPCI1020_PARA_DO;
#endif

// ״̬�Ĵ���RR0
#ifndef _PCI1020_PARA_RR0
typedef struct _PCI1020_PARA_RR0      
{
	UINT XDRV;			// X�������״̬  1������������� 0��ֹͣ����
	UINT YDRV;			// Y�������״̬  1������������� 0��ֹͣ����
	UINT ZDRV;			// Z�������״̬  1������������� 0��ֹͣ����
	UINT UDRV;			// U�������״̬  1������������� 0��ֹͣ����
	UINT XERROR;		// X��ĳ���״̬  X���RR2�Ĵ������κ�һλΪ1����λ��Ϊ1
	UINT YERROR;		// Y��ĳ���״̬  Y���RR2�Ĵ������κ�һλΪ1����λ��Ϊ1
	UINT ZERROR;		// Z��ĳ���״̬  Z���RR2�Ĵ������κ�һλΪ1����λ��Ϊ1
	UINT UERROR;		// U��ĳ���״̬  U���RR2�Ĵ������κ�һλΪ1����λ��Ϊ1
	UINT IDRV;			// �岹����״̬   1�������ڲ岹ģʽ  0��δ���ڲ岹ģʽ
	UINT CNEXT;			// ��ʾ����д�������岹����һ������  1������д�� 0��������д��
	                    // �����������岹�ж�ʹ�ܺ�CNEXTΪ1��ʾ�������жϣ����жϳ���д����һ���岹����󣬸�λ���㲢���ж��źŻص��ߵ�ƽ
	UINT ZONE0;			// ZONE2��ZONE1��ZONE0��ʾ��Բ���岹���������ڵ�����
	UINT ZONE1;			// 000 ����0����   001����1����  010����2����  011����3����
	UINT ZONE2;			// 100 ����4����   101����5����	 110����6����  111����7����
	UINT BPSC0;			// BPSC1��BPSC0��ʾ��λ�岹�����ж�ջ������(SC)����ֵ
	UINT BPSC1;			// 00�� 0   01��1   10�� 2   11��3
						// ����λ�岹�ж�ʹ�ܺ󣬵�SC��ֵ��2��Ϊ1ʱ�������жϣ�
	                    // ����λ�岹��ջд���µ����ݻ����PCI1020_ClearInterruptStatus���жϽ����
} PCI1020_PARA_RR0,*PPCI1020_PARA_RR0;
#endif

// ״̬�Ĵ���RR1��ÿһ���ᶼ��RR1�Ĵ��������ĸ�Ҫָ�����
#ifndef _PCI1020_PARA_RR1
typedef struct _PCI1020_PARA_RR1    
{
	UINT CMPP;			// ��ʾ�߼�/ʵλ��������COMP+�Ĵ����Ĵ�С��ϵ 1���߼�/ʵλ��������COMP+ 0���߼�/ʵλ��������COMP+
	UINT CMPM;			// ��ʾ�߼�/ʵλ��������COMP-�Ĵ����Ĵ�С��ϵ 1���߼�/ʵλ��������COMP- 0���߼�/ʵλ��������COMP-
	UINT ASND;			// �ڼ�/���������м���ʱ��Ϊ1
	UINT CNST;			// �ڼ�/���������ж���ʱ��Ϊ1
	UINT DSND;			// �ڼ�/���������м���ʱ��Ϊ1
	UINT AASND;			// ��S���߼�/���������У����ٶ�/���ٶ�����ʱ��Ϊ1 
	UINT ACNST;			// ��S���߼�/���������У����ٶ�/���ٶȲ���ʱ��Ϊ1 
	UINT ADSND;			// ��S���߼�/���������У����ٶ�/���ٶȼ���ʱ��Ϊ1 
	UINT IN0;			// �ⲿֹͣ�ź�IN0��Чʹ����ֹͣʱ��Ϊ1
	UINT IN1;			// �ⲿֹͣ�ź�IN1��Чʹ����ֹͣʱ��Ϊ1
	UINT IN2;			// �ⲿֹͣ�ź�IN2��Чʹ����ֹͣʱ��Ϊ1
	UINT IN3;			// �ⲿֹͣ�ź�IN3��Чʹ����ֹͣʱ��Ϊ1
	UINT LMTP;			// �ⲿ�����������ź�(nLMTP)��Чʹ����ֹͣʱ��Ϊ1
	UINT LMTM;			// �ⲿ�����������ź�(nLMTM)��Чʹ����ֹͣʱ��Ϊ1
	UINT ALARM;			// �ⲿ�ŷ����ﱨ���ź�(nALARM)��Чʹ����ֹͣʱ��Ϊ1
	UINT EMG;			// �ⲿ����ֹͣ�ź�(EMGN)ʹ����ֹͣʱ��Ϊ1
} PCI1020_PARA_RR1,*PPCI1020_PARA_RR1;
#endif

// ״̬�Ĵ���RR2��ÿһ���ᶼ��RR2�Ĵ��������ĸ�Ҫָ�����
#ifndef _PCI1020_PARA_RR2
typedef struct _PCI1020_PARA_RR2     
{
	UINT SLMTP;			// ����������������λ���������������У��߼�/ʵλ����������COMP+�Ĵ���ֵʱ��Ϊ1
	UINT SLMTM;			// ���÷�����������λ���ڷ����������У��߼�/ʵλ������С��COMP-�Ĵ���ֵʱ��Ϊ1
	UINT HLMTP;			// �ⲿ�����������ź�(nLMTP)������Ч��ƽʱ��Ϊ1
	UINT HLMTM;			// �ⲿ�����������ź�(nLMTM)������Ч��ƽʱ��Ϊ1
	UINT ALARM;			// �ⲿ�ŷ����ﱨ���ź�(nALARM)����Ϊ��Ч��������Ч״̬ʱ��Ϊ1
	UINT EMG;			// �ⲿ����ֹͣ�źŴ��ڵ͵�ƽʱ��Ϊ1
	UINT HOME;			// ��Z������ź����Զ���Ѱԭ�����ʱΪ1
	UINT HMST0;			// HMST0-4(HMST4-0)��ʾ�Զ�ԭ����Ѱ��ִ�еĲ���
	UINT HMST1;			//	 0���ȴ��Զ�ԭ����Ѱ����
	UINT HMST2;			//	 3���ȴ�IN0�ź���ָ����������Ч	
	UINT HMST3;			//	 8��12��15���ȴ�IN1�ź���ָ����������Ч
	UINT HMST4;			//	 20��IN2�ź���ָ����������Ч
						//	 25�����Ĳ�
} PCI1020_PARA_RR2,*PPCI1020_PARA_RR2;
#endif

// ״̬�Ĵ���RR3
#ifndef _PCI1020_PARA_RR3
typedef struct _PCI1020_PARA_RR3      
{
	UINT XIN0;			// �ⲿֹͣ�ź�XIN0�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT XIN1;			// �ⲿֹͣ�ź�XIN1�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT XIN2;			// �ⲿֹͣ�ź�XIN2�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT XIN3;			// �ⲿֹͣ�ź�XIN3�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT XEXPP;			// �ⲿ������㶯�����ź�XEXPP�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT XEXPM;			// �ⲿ������㶯�����ź�XEXPM�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT XINPOS;		// �ⲿ�ŷ������λ�ź�XINPOS�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT XALARM;		// �ⲿ�ŷ����ﱨ���ź�XALARM�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT YIN0;			// �ⲿ�����ź�YIN0�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT YIN1;			// �ⲿ�����ź�YIN1�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT YIN2;			// �ⲿ�����ź�YIN2�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT YIN3;			// �ⲿ�����ź�YIN3�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT YEXPP;			// �ⲿ������㶯�����ź�YEXPP�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT YEXPM;			// �ⲿ������㶯�����ź�YEXPM�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT YINPOS;		// �ⲿ�ŷ������λ�ź�YINPOS�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT YALARM;		// �ⲿ�ŷ����ﱨ���ź�YALARM�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
} PCI1020_PARA_RR3,*PPCI1020_PARA_RR3;
#endif

// ״̬�Ĵ���RR4
#ifndef _PCI1020_PARA_RR4
typedef struct _PCI1020_PARA_RR4     
{
	UINT ZIN0;			// �ⲿֹͣ�ź�YIN0�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT ZIN1;			// �ⲿֹͣ�ź�YIN1�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT ZIN2;			// �ⲿֹͣ�ź�YIN2�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT ZIN3;			// �ⲿֹͣ�ź�YIN3�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT ZEXPP;			// �ⲿ������㶯�����ź�ZEXPP�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT ZEXPM;			// �ⲿ������㶯�����ź�ZEXPM�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT ZINPOS;		// �ⲿ�ŷ������λ�ź�ZINPOS�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT ZALARM;		// �ⲿ�ŷ����ﱨ���ź�ZALARM�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT UIN0;			// �ⲿֹͣ�ź�UIN0�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT UIN1;			// �ⲿֹͣ�ź�UIN1�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT UIN2;			// �ⲿֹͣ�ź�UIN2�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT UIN3;			// �ⲿֹͣ�ź�UIN3�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT UEXPP;			// �ⲿ������㶯�����ź�UEXPP�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT UEXPM;			// �ⲿ������㶯�����ź�UEXPM�ĵ�ƽ״̬ 1���ߵ�ƽ 0���͵�ƽ
	UINT UINPOS;		// �ⲿ�ŷ������λ�ź�UINPOS�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
	UINT UALARM;		// �ⲿ�ŷ����ﱨ���ź�UALARM�ĵ�ƽ״̬  1���ߵ�ƽ 0���͵�ƽ
} PCI1020_PARA_RR4,*PPCI1020_PARA_RR4;
#endif

// ״̬�Ĵ���RR5  �����жϲ���ʱ����Ӧ���жϱ�־Ϊ1���ж�����ź�Ϊ�͵�ƽ
// ����CPU����RR5�Ĵ������жϱ�־��RR5�ı�־��Ϊ0���ж��źŻָ����ߵ�ƽ
#ifndef _PCI1020_PARA_RR5
typedef struct _PCI1020_PARA_RR5     
{
	UINT PULSE;			// ����һ����������ʱΪ1
	UINT PBCM;			// �߼�/ʵ��λ�ü�������ֵ���ڵ���COMP-�Ĵ�����ֵʱΪ1
	UINT PSCM;			// �߼�/ʵ��λ�ü�������ֵС��COMP-�Ĵ�����ֵʱΪ1
	UINT PSCP;			// �߼�/ʵ��λ�ü�������ֵС��COMP+�Ĵ�����ֵʱΪ1
	UINT PBCP;			// �߼�/ʵ��λ�ü�������ֵ���ڵ���COMP+�Ĵ�����ֵΪ1
	UINT CDEC;			// �ڼ�/����ʱ�����忪ʼ����ʱΪ1
	UINT CSTA;			// �ڼ�/����ʱ����ʼ����ʱΪ1
	UINT DEND;			// ��������ʱΪ1
	UINT HMEND;			// �Զ�ԭ����������ʱΪ1
	UINT SYNC;			// ͬ���������ж�
} PCI1020_PARA_RR5,*PPCI1020_PARA_RR5;
#endif

#ifndef DEFINING
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
	#endif

	//######################## �豸����������� #################################
	// �����ڱ��豸�����������	
	HANDLE DEVAPI FAR PASCAL PCI1020_CreateDevice(       // �������
								int DeviceID);           // �豸ID��

	int DEVAPI FAR PASCAL PCI1020_GetDeviceCount(        // ����豸����
								HANDLE hDevice);		 // �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_ReleaseDevice(		 // �ͷ��豸
								HANDLE hDevice);		 // �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_Reset(				 // ������λ
								HANDLE hDevice);		 // �豸���
	//*******************************************************************
	// ���õ�����߼���������ʵ��λ�ü����������ټ�����ƫ��

	BOOL DEVAPI FAR PASCAL PCI1020_PulseOutMode(         // �����������ģʽ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  
								LONG Mode);				 // ģʽ

	BOOL DEVAPI FAR PASCAL PCI1020_PulseInputMode(       // ������������ģʽ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  
								LONG Mode);				 // ģʽ

	BOOL DEVAPI FAR PASCAL PCI1020_SetR(				 // ���ñ���(1-500)	
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  
								LONG Data);				 // ����ֵ(1-500)

	BOOL DEVAPI FAR PASCAL PCI1020_SetA(				 // ���ü��ٶ�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)     
								LONG Data);				 // ���ٶ� (125-1000000)

	BOOL DEVAPI FAR PASCAL PCI1020_SetDec(				 // ���ü��ٶ�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // ���ٶ�ֵ(125-1000,000)

	BOOL DEVAPI FAR PASCAL PCI1020_SetAccIncRate(		 // ���ٶȱ仯��  
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)   
								LONG Data);				 // ���� (954-62500000) 

	BOOL DEVAPI FAR PASCAL PCI1020_SetDecIncRate(		 // ���ٶȱ仯��  
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // ���� (954-62500000)

	BOOL DEVAPI FAR PASCAL PCI1020_SetSV(				 // ���ó�ʼ�ٶ�(1-8000)
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)   
								LONG Data);				 // �ٶ�ֵ(1-8000)

	BOOL DEVAPI FAR PASCAL PCI1020_SetV(				 // ���������ٶ�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)     
								LONG Data);				 // �����ٶ�ֵ(1-8000)

	BOOL DEVAPI FAR PASCAL PCI1020_SetHV(				 // ����ԭ����Ѱ�ٶ�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)	
								LONG Data);				 // ԭ����Ѱ�ٶ�ֵ(1-8000)

	BOOL DEVAPI FAR PASCAL PCI1020_SetP(				 // ���ö���������
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);			     // ����������(0-268435455)

	BOOL DEVAPI FAR PASCAL PCI1020_SetIP(				 // ���ò岹�յ�������(-8388608-+8388607)
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // �岹�յ�������(-8388608-+8388607)

	BOOL DEVAPI FAR PASCAL PCI1020_SetC(                 // ����Բ������(������)  
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // Բ��������������Χ(-2147483648-+2147483647)

	BOOL DEVAPI FAR PASCAL PCI1020_SetLP(				 // �����߼�λ�ü�����
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // �߼�λ�ü�����ֵ(-2147483648-+2147483647)

	BOOL DEVAPI FAR PASCAL PCI1020_SetEP(				 // ����ʵλ������ 
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // ʵλ������ֵ(-2147483648-+2147483647)

	BOOL DEVAPI FAR PASCAL PCI1020_SetAccofst(			 // ���ü��ټ�����ƫ��
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // ƫ�Ʒ�Χ(0-65535)

	BOOL DEVAPI FAR PASCAL PCI1020_SelectLPEP(			 // ѡ���߼���������ʵλ������
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG LogicFact);		 // ѡ���߼�λ�ü�������ʵλ������ PCI1020_LOGIC���߼�λ�ü����� PCI1020_FACT��ʵλ������	

	BOOL DEVAPI FAR PASCAL PCI1020_SetCOMPP(			 // ����COMP+�Ĵ���
								HANDLE hDevice,			 // �豸��
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  
								USHORT LogicFact,		 // ѡ���߼�λ�ü�������ʵλ������ PCI1020_LOGIC���߼�λ�ü����� PCI1020_FACT��ʵλ������	
								LONG Data);

	BOOL DEVAPI FAR PASCAL PCI1020_SetCOMPM(			 // ����COMP-�Ĵ���
								HANDLE hDevice,			 // �豸��
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								USHORT LogicFact,		 // ѡ���߼�λ�ü�������ʵλ������ PCI1020_LOGIC���߼�λ�ü����� PCI1020_FACT��ʵλ������	
								LONG Data);
	//*******************************************************************
	// ����ͬ��λ
	BOOL DEVAPI FAR PASCAL PCI1020_SetSynchronAction(	 // ����ͬ��λ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_SynchronActionOwnAxis pPara1,// �Լ���Ĳ�������
								PPCI1020_PARA_SynchronActionOtherAxis pPara2);// ������Ĳ�������

	BOOL DEVAPI FAR PASCAL PCI1020_SynchronActionDisable(// ����ͬ��λ��Ч
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)
								PPCI1020_PARA_SynchronActionOwnAxis pPara1,// �Լ���Ĳ�������
								PPCI1020_PARA_SynchronActionOtherAxis pPara2);// ������Ĳ�������

	BOOL DEVAPI FAR PASCAL PCI1020_WriteSynchronActionCom(// дͬ����������
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	//*******************************************************************
	//  ����DCC������ģʽ
	BOOL DEVAPI FAR PASCAL PCI1020_SetDCC(				 // ��������ź�nDCC�������ƽ�͵�ƽ����
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_DCC pPara);// DCC�źŲ����ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_StartDCC(			   // ����ƫ�����������������
								HANDLE hDevice,			   // �豸���
								LONG AxisNum);			   // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	BOOL DEVAPI FAR PASCAL PCI1020_ExtMode(				   // ��������ģʽ
								HANDLE hDevice,			   // �豸���
								LONG AxisNum,			   // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_ExpMode pPara);// ���������ṹ��ָ��
	//*******************************************************************
	// �����Զ�ԭ����Ѱ
	BOOL DEVAPI FAR PASCAL PCI1020_SetInEnable(			// �����Զ�ԭ����Ѱ��һ���ڶ����������ⲿ�����ź�IN0-2����Ч��ƽ
								HANDLE hDevice,			// �豸��
								LONG AxisNum,			// ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)	
								LONG InNum,				// ֹͣ��
								LONG LogLever);			// ��Ч��ƽ

	BOOL DEVAPI FAR PASCAL PCI1020_SetAutoHomeSearch(   // �����Զ�ԭ����Ѱ����
								HANDLE hDevice,			// �豸���
								LONG AxisNum,			// ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)
								PPCI1020_PARA_AutoHomeSearch pPara);// �Զ���Ѱԭ������ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_StartAutoHomeSearch( // �����Զ�ԭ����Ѱ
								HANDLE hDevice,			// �豸���		
								LONG AxisNum);			// ���(1:X��; 2:Y��)	

	//*******************************************************************
	// ���ñ����������ź�����
	BOOL DEVAPI FAR PASCAL PCI1020_SetEncoderSignalType(// ���ñ����������ź�����
								HANDLE hDevice,			// �豸���
								LONG AxisNum,			// ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)	
								LONG Type);				// �����ź����� 0��2���������� 1����/����������

	//*******************************************************************
	// ֱ��S���߳�ʼ������������
	BOOL DEVAPI FAR PASCAL PCI1020_InitLVDV(				// ��ʼ������,������������
								HANDLE hDevice,				// �豸���
								PPCI1020_PARA_DataList pDL, // ���������ṹ��ָ��
								PPCI1020_PARA_LCData pLC);	// ֱ��S���߲����ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_StartLVDV(				// ��������,������������
								HANDLE hDevice,				// �豸���
								LONG AxisNum);				// ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	BOOL DEVAPI FAR PASCAL	PCI1020_Start4D(HANDLE hDevice);// 4��ͬʱ����						           
	//*******************************************************************
	// ����2��ֱ�߲岹��ʼ������������
	BOOL DEVAPI FAR PASCAL PCI1020_InitLineInterpolation_2D(// ��ʼ������2��ֱ�߲岹�˶� 
								HANDLE hDevice,				// �豸���
								PPCI1020_PARA_DataList pDL, // ���������ṹ��ָ��
								PPCI1020_PARA_InterpolationAxis pIA,// �岹��ṹ��ָ��
								PPCI1020_PARA_LineData pLD);// ֱ�߲岹�ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_StartLineInterpolation_2D(// ��������2��ֱ�߲岹�˶� 
								HANDLE hDevice);			 // �豸���
								
	//*******************************************************************
	// ����3��ֱ�߲岹��ʼ������������
	BOOL DEVAPI FAR PASCAL PCI1020_InitLineInterpolation_3D(// ��ʼ������3��ֱ�߲岹�˶�	
								HANDLE hDevice,				// �豸���
								PPCI1020_PARA_DataList pDL, // ���������ṹ��ָ��
								PPCI1020_PARA_InterpolationAxis pIA,// �岹��ṹ��ָ��
								PPCI1020_PARA_LineData pLD);// ֱ�߲岹�ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_StartLineInterpolation_3D(// ��������3��ֱ�߲岹�˶� 				
								HANDLE hDevice);			 // �豸���
		
	//*******************************************************************
	// ����2����������Բ���岹��ʼ������������
	BOOL DEVAPI FAR PASCAL PCI1020_InitCWInterpolation_2D(	// ��ʼ������2����������Բ���岹�˶� 
								HANDLE hDevice,				// �豸���
								PPCI1020_PARA_DataList pDL, // ���������ṹ��ָ��
								PPCI1020_PARA_InterpolationAxis pIA,// �岹��ṹ��ָ��
								PPCI1020_PARA_CircleData pCD);// Բ���岹�ṹ��ָ��
	                         
	BOOL DEVAPI FAR PASCAL PCI1020_StartCWInterpolation_2D( // ��������2������������Բ���岹�˶� 
								HANDLE hDevice,				// �豸���
								LONG Direction);			// ���� ��ת��PCI1020_PDIRECTION ��ת��PCI1020_MDIRECTION                     
	//*************************************************************************
	// λ�岹��غ���
	BOOL DEVAPI FAR PASCAL PCI1020_InitBitInterpolation_2D(	// ��ʼ������2��λ�岹����
								HANDLE hDevice,				// �豸���
								PPCI1020_PARA_InterpolationAxis pIA,// �岹��ṹ��ָ��
								PPCI1020_PARA_DataList pDL);// ���������ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_InitBitInterpolation_3D(// ��ʼ������2��λ�岹����
								HANDLE hDevice,			   // �豸���
								PPCI1020_PARA_InterpolationAxis pIA,// �岹��ṹ��ָ��
								PPCI1020_PARA_DataList pDL);// ���������ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_AutoBitInterpolation_2D( // ��������2��λ�岹���߳�
								HANDLE hDevice,				// �豸���
								PUSHORT pBuffer,				// λ�岹����ָ��	
								UINT nCount);				// ��������

	BOOL DEVAPI FAR PASCAL PCI1020_AutoBitInterpolation_3D( // ��������3��λ�岹���߳�
								HANDLE hDevice,				// �豸���
								PSHORT pBuffer,				// λ�岹����ָ��	
								UINT nCount);				// ��������

	BOOL DEVAPI FAR PASCAL PCI1020_ReleaseBitInterpolation(	// �ͷ�BP�Ĵ���
								HANDLE hDevice);			// �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_SetBP_2D(                // ��������2��λ�岹����
								HANDLE hDevice,				// �豸��� 
								LONG BP1PData,				// 1����������������
								LONG BP1MData,				// 1�ᷴ������������
								LONG BP2PData,				// 2����������������
								LONG BP2MData);				// 2�ᷴ������������

	BOOL DEVAPI FAR PASCAL PCI1020_SetBP_3D(				// ��������3��λ�岹����	
								HANDLE hDevice,				// �豸���
								USHORT BP1PData,			// 1����������������
								USHORT BP1MData,			// 1�ᷴ������������
								USHORT BP2PData,			// 2����������������
								USHORT BP2MData,			// 2�ᷴ������������
								USHORT BP3PData,			// 3����������������
								USHORT BP3MData);			// 3�ᷴ������������

	LONG DEVAPI FAR PASCAL PCI1020_BPRegisterStack(			// BPλ���ݶ�ջ����ֵ
								HANDLE hDevice);			// �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_StartBitInterpolation_2D(// ��������2��λ�岹
								HANDLE hDevice);			// �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_StartBitInterpolation_3D(// ��������3��λ�岹
								HANDLE hDevice);			// �豸���

	BOOL DEVAPI FAR PASCAL  PCI1020_BPWait(					// �ȴ�λ�岹����һ�������趨
								HANDLE hDevice);			// �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_ClearBPData(				// ���BP�Ĵ�������
								HANDLE hDevice);			// �豸���
	//*******************************************************************
	// �����岹��غ���
								
	BOOL DEVAPI FAR PASCAL  PCI1020_NextWait(				// �ȴ������岹��һ���ڵ������趨
								HANDLE hDevice);			// �豸���

	//*******************************************************************
	// �����岹����
	BOOL DEVAPI FAR PASCAL PCI1020_SingleStepInterpolationCom(// ����������Ƶ����岹�˶�
								HANDLE hDevice);			// �豸���	

	BOOL DEVAPI FAR PASCAL PCI1020_StartSingleStepInterpolation(// ����������
								HANDLE hDevice);

	BOOL DEVAPI FAR PASCAL PCI1020_SingleStepInterpolationExt(// �����ⲿ���Ƶ����岹�˶�
								HANDLE hDevice);			// �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_ClearSingleStepInterpolation(// ��������岹����
								HANDLE hDevice);			// �豸���
	//*******************************************************************
	// �ж�λ���á��岹�ж�״̬���
	BOOL DEVAPI FAR PASCAL PCI1020_SetInterruptBit(			// �����ж�λ
								HANDLE hDevice,				// �豸���
								LONG AxisNum,				// ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_Interrupt pPara);// �ж�λ�ṹ��ָ��

	BOOL DEVAPI FAR PASCAL PCI1020_ClearInterruptStatus(	// ����岹�ж�״̬ 
								HANDLE hDevice);			// �豸���

	//*******************************************************************
	// �ⲿ�ź��������������������������

	BOOL DEVAPI FAR PASCAL PCI1020_SetOutEnableDV(		 // �����ⲿʹ�ܶ�������(�½�����Ч)
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
			                    
	BOOL DEVAPI FAR PASCAL PCI1020_SetOutEnableLV(		 // �����ⲿʹ����������(���ֵ͵�ƽ��Ч)
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	//*******************************************************************
	// ����������λ��Ч����Ч
	BOOL DEVAPI FAR PASCAL PCI1020_SetPDirSoftwareLimit( // ����������������λ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG LogicFact,			 // �߼�/ʵλ������ѡ�� PCI1020_LOGIC���߼�λ�ü����� PCI1020_FACT��ʵλ������	
								LONG Data);				 // ������λ����

	BOOL DEVAPI FAR PASCAL PCI1020_SetMDirSoftwareLimit( // ���÷�����������λ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG LogicFact,			 // �߼�/ʵλ������ѡ�� PCI1020_LOGIC���߼�λ�ü����� PCI1020_FACT��ʵλ������	
								LONG Data);				 

	BOOL DEVAPI FAR PASCAL PCI1020_ClearSoftwareLimit(	 // ���������λ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)

	//******************************************************************* 
	// �����ⲿ�����źŵ���Ч����Ч		
	BOOL DEVAPI FAR PASCAL PCI1020_SetLMTEnable(		 // �����ⲿԽ���źŵ���Ч��ֹͣ��ʽ	
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG StopMode);          // PCI1020_DECSTOP������ֹͣ��PCI1020_SUDDENSTOP������ֹͣ

	BOOL DEVAPI FAR PASCAL PCI1020_SetStopEnable(		 // �����ⲿֹͣ�ź���Ч
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG StopNum);			 // ֹͣ��

	BOOL DEVAPI FAR PASCAL PCI1020_SetStopDisable(		 // �����ⲿֹͣ�ź���Ч
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)
								LONG StopNum);			 // ֹͣ��
												
	BOOL DEVAPI FAR PASCAL PCI1020_SetALARMEnable(       // �����ŷ������ź���Ч 
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  

	BOOL DEVAPI FAR PASCAL PCI1020_SetALARMDisable(      // �����ŷ������ź���Ч  
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  

	BOOL DEVAPI FAR PASCAL PCI1020_SetINPOSEnable(		 // �����ŷ����ﶨλ��������ź���Ч 
								HANDLE hDevice,			 // �豸���	
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	BOOL DEVAPI FAR PASCAL PCI1020_SetINPOSDisable(		 // �����ŷ����ﶨλ��������ź���Ч
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	//*******************************************************************
	// ���ٺ�������

	BOOL DEVAPI FAR PASCAL PCI1020_DecValid(			 // ������Ч
								HANDLE hDevice);		 // �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_DecInvalid(			 // ������Ч
								HANDLE hDevice);		 // �豸���

	BOOL DEVAPI FAR PASCAL PCI1020_DecStop(				 // ����ֹͣ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)  

	BOOL DEVAPI FAR PASCAL PCI1020_InstStop(			 // ����ֹͣ
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	BOOL DEVAPI FAR PASCAL PCI1020_AutoDec(				 // �Զ�����
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	BOOL DEVAPI FAR PASCAL PCI1020_HanDec(				 // �ֶ����ٵ��趨
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Data);				 // �ֶ����ٵ����ݣ���Χ(0 - 4294967295)

	//*************************************************************************
	// �����״̬���߼���������ʵ��λ�ü���������ǰ�ٶȡ���/���ٶ�
	LONG DEVAPI FAR PASCAL PCI1020_ReadLP(				 // ���߼�������
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	LONG DEVAPI FAR PASCAL PCI1020_ReadEP(				 // ��ʵλ������
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	LONG DEVAPI FAR PASCAL PCI1020_ReadBR(				 // ��ͬ������Ĵ���
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��)

	LONG DEVAPI FAR PASCAL PCI1020_ReadCV(				 // ����ǰ�ٶ�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	LONG DEVAPI FAR PASCAL PCI1020_ReadCA(				 // ����ǰ���ٶ�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum);			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 

	//*******************************************************************
	// ��������л���ͨ�����
	BOOL DEVAPI FAR PASCAL PCI1020_OutSwitch(			 // ��������л�
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG StatusGeneralOut);	 // ״̬�����ͨ�����ѡ�� PCI1020_STATUS:״̬��� PCI1020_GENERAL:ͨ�����

	BOOL DEVAPI FAR PASCAL PCI1020_SetDeviceDO(
								 HANDLE hDevice,	 	 // �豸��
								 LONG AxisNum,			 // ���
								 PPCI1020_PARA_DO pPara);		
	//*******************************************************************
	// ��״̬�Ĵ�����λ״̬
	LONG DEVAPI FAR PASCAL PCI1020_ReadRR(				 // ��RR�Ĵ���
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								LONG Num);				 // �Ĵ�����

	BOOL DEVAPI FAR PASCAL PCI1020_GetRR0Status(		 // �����״̬�Ĵ���RR0��λ״̬
								HANDLE hDevice,			 // �豸���
								PPCI1020_PARA_RR0 pPara);// RR0�Ĵ���״̬

	BOOL DEVAPI FAR PASCAL PCI1020_GetRR1Status(		 // ���״̬�Ĵ���RR1��λ״̬
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_RR1 pPara);// RR1�Ĵ���״̬			

	BOOL DEVAPI FAR PASCAL PCI1020_GetRR2Status(		 // ���״̬�Ĵ���RR2��λ״̬
								HANDLE hDevice,			 // �豸���
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_RR2 pPara);// RR2�Ĵ���״̬			

	BOOL DEVAPI FAR PASCAL PCI1020_GetRR3Status(		 // ���״̬�Ĵ���RR3��λ״̬
								HANDLE hDevice,			 // �豸���
								PPCI1020_PARA_RR3 pPara);// RR3�Ĵ���״̬			

	BOOL DEVAPI FAR PASCAL PCI1020_GetRR4Status(		 // ���״̬�Ĵ���RR4��λ״̬
								HANDLE hDevice,			 // �豸���
								PPCI1020_PARA_RR4 pPara);// RR4�Ĵ���״̬

	BOOL DEVAPI FAR PASCAL PCI1020_GetRR5Status(
								HANDLE hDevice,			 // �豸��
								LONG AxisNum,			 // ���(PCI1020_XAXIS:X��,PCI1020_YAXIS:Y��, PCI1020_ZAXIS:Z��,PCI1020_UAXIS:U��) 
								PPCI1020_PARA_RR5 pPara);// RR5�Ĵ���״̬

	BOOL DEVAPI FAR PASCAL PCI1020_Reset(HANDLE hDevice);// ������λ
	//####################### �жϺ��� #################################
	// ����Ӳ���źŵ�״̬�仯����CPU�����ж��¼�hEventInt��
	BOOL DEVAPI FAR PASCAL PCI1020_InitDeviceInt(HANDLE hDevice, HANDLE hEventInt); // ��ʼ���ж�

	BOOL DEVAPI FAR PASCAL PCI1020_ReleaseDeviceInt(HANDLE hDevice); // �ͷ��ж���Դ
	BOOL DEVAPI FAR PASCAL PCI1020_GetDeviceIntSrc(HANDLE hDevice, BYTE IntSrc[16]); // ��ȡ�豸�ж�λ
	BOOL DEVAPI FAR PASCAL PCI1020_ResetDeviceIntSrc(HANDLE hDevice, BYTE IntSrcID); // �ͷ��ж�λ
	//*******************************************************************
#ifdef __cplusplus
}
#endif
// �Զ������������������
#ifndef DEFINING
	#ifndef _WIN64
		#pragma comment(lib, "PCI1020_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI1020_32.dll...")
		#pragma message("======== Successfully linked with PCI1020_32.dll")
	#else
		#pragma comment(lib, "PCI1020_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI1020_64.dll...")
		#pragma message("======== Successfully linked with PCI1020_64.dll")
	#endif
#endif