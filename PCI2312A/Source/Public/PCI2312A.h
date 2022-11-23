/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-30 10:03
 *  Email: mrma617@gmail.com
 */

#ifndef _PCI2312A_DEVICE_
#define _PCI2312A_DEVICE_

//***********************************************************
// ����INTӲ������
typedef struct _PCI2312A_PARA_INT
{
	LONG lINTDrection;      // �жϷ���:0:�½��ش����ж� 1:�����ش����ж�
	LONG lIntEnable[16];	// ����16��ͨ���ж�ʹ��, TRUE��ʹ��;FALSE:��ֹ
	ULONG lINTTarCount[16];	// �ж�Ŀ��������lINTEndCount>0ʱ ͨ���жϴﵽĿ�������ֹ��ͨ���ж�
} PCI2312A_PARA_INT, *PPCI2312A_PARA_INT;

typedef struct _PCI2312A_INT_STATE
{
	SHORT INTTime[8];		// �ж�ʱ��
	WORD INTSrc;			// �ж�ʱ��ͨ���ж�״̬
	WORD INTDIVal;			// �ж�ʱ��ͨ��״̬
	LONG INTCount[16];		// �ж�ʱ��ͨ���ж���
} PCI2312A_INT_STATE, *PPCI2312A_INT_STATE;


//######################## �������� #################################
// CreateFileObject���õ��ļ�������ʽ������(��ͨ����ָ��ʵ�ֶ��ַ�ʽ������)
#define PCI2312A_modeRead				0x0000		// ֻ���ļ���ʽ
#define PCI2312A_modeWrite				0x0001		// ֻд�ļ���ʽ
#define	PCI2312A_modeReadWrite			0x0002		// �ȶ���д�ļ���ʽ
#define PCI2312A_modeCreate				0x1000		// ����ļ�������Դ������ļ���������ڣ����ؽ����ļ�������0
#define PCI2312A_typeText				0x4000		// ���ı���ʽ�����ļ�

//***********************************************************

//***********************************************************
// �û������ӿ�
#ifndef _PCI2312A_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## ����ͨ�ú��� #################################
	// �����ڱ��豸�����������
	HANDLE DEVAPI FAR PASCAL PCI2312A_CreateDevice(int DeviceID = 0);		// �����豸����
	int DEVAPI FAR PASCAL PCI2312A_GetDeviceCount(HANDLE hDevice);			// ȡ���豸��̨��
	int DEVAPI FAR PASCAL PCI2312A_GetDeviceCurrentID(HANDLE hDevice);
	BOOL DEVAPI FAR PASCAL PCI2312A_ListDeviceDlg(HANDLE hDevice);			// �б�ϵͳ���е����еĸ�PCI�豸
    BOOL DEVAPI FAR PASCAL PCI2312A_ReleaseDevice(HANDLE hDevice);			// �ر��豸,��ֹ����,���ͷ���Դ

	//####################### ����I/O����������� #################################
	// �û�����ʹ��WriteRegisterULong��ReadRegisterULong�Ⱥ���ֱ�ӿ��ƼĴ�������I/O
	// �����������ʹ����������������ʡ�£�������Ҫ�����ļĴ��������λ�����ȣ���ֻ
	// ����VB�����Ե����Բ�����ô�򵥵�ʵ�ָ�������ͨ���Ŀ��ơ�
	BOOL DEVAPI FAR PASCAL PCI2312A_SetDeviceDO(            // �������������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDOSts[16]);	// ����״̬

	BOOL DEVAPI FAR PASCAL PCI2312A_GetDeviceDO(            // ȡ�����������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDISts[16]);	// ����״̬	


	BOOL DEVAPI FAR PASCAL PCI2312A_GetDeviceDI(            // ȡ������������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDISts[16]);	// ����״̬	

	//####################### �жϺ��� #################################
	// ����Ӳ���źŵ�״̬�仯����CPU�����ж��¼�hEventInt��
	BOOL DEVAPI FAR PASCAL PCI2312A_InitDeviceInt(			// ��ʼ���ж�
										HANDLE hDevice,     // �豸���	
										HANDLE hEventInt,	// �ж��¼�
										PPCI2312A_PARA_INT pINTPara);	//  Ӳ������, �����ڴ˺����о���Ӳ��״̬

	ULONG DEVAPI FAR PASCAL PCI2312A_GetDeviceIntCount(HANDLE hDevice,	// ���жϳ�ʼ����ȡ��ָ��ͨ�������Ĵ���
										LONG lChannel = 0);				// ͨ����[0~15]

	BOOL DEVAPI FAR PASCAL PCI2312A_ClearIntCount(HANDLE hDevice,		// ��ָ������������ͨ�����жϴ������㣬��ʹ�ܸ�ͨ���ж�
										LONG lChannel = 0);				// ͨ����[0~15]

	BOOL DEVAPI FAR PASCAL PCI2312A_GetIntSrc(HANDLE hDevice,			// ȡ�ø�ͨ���ж�״̬
										LONG lChannelInt[16]);			// ��ͨ���ж�״̬

	ULONG DEVAPI FAR PASCAL PCI2312A_GetIntStateCount(HANDLE hDevice);	// ȡ�õ�ǰ������ʣ���ж�״̬�������2048����������


	LONG DEVAPI FAR PASCAL PCI2312A_ReadINTState(						// ��ȡ�ж�״̬
										HANDLE hDevice,					// �豸���	
										PCI2312A_INT_STATE pINTState[],	// �����ж�״̬
										ULONG lReadCount);				// ��ȡ�ж�״̬��

	BOOL DEVAPI FAR PASCAL PCI2312A_ReleaseDeviceInt(HANDLE hDevice); // �ͷ��ж���Դ

	//################# �ڴ�ӳ��Ĵ���ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	BOOL DEVAPI FAR PASCAL PCI2312A_GetDeviceBar(					// ȡ��ָ����ָ���豸�Ĵ�����BAR��ַ
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											__int64 pbPCIBar[6]);	// ����PCI BAR���е�ַ,����PCI BAR���ж��ٿ��õ�ַ�뿴Ӳ��˵����

	BOOL DEVAPI FAR PASCAL PCI2312A_GetDevVersion(					// ��ȡ�豸�̼�������汾
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											PULONG pulFmwVersion,	// �̼��汾
											PULONG pulDriverVersion);// �����汾

	BOOL DEVAPI FAR PASCAL PCI2312A_WriteRegisterByte(			// ��ָ���Ĵ����ռ�λ��д�뵥��������
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											__int64 pbLinearAddr,	// ָ���Ĵ��������Ի���ַ,������GetDeviceAddr�е�pbLinearAddr��������ֵ
											ULONG OffsetBytes,		// ��������Ի���ַ����ַ��ƫ��λ��(�ֽ�)
											BYTE Value);			// ��ָ����ַд�뵥�ֽ����ݣ����ַ�����Ի���ַ��ƫ��λ�þ�����

	BOOL DEVAPI FAR PASCAL PCI2312A_WriteRegisterWord(			// д˫�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes,  
											WORD Value);

	BOOL DEVAPI FAR PASCAL PCI2312A_WriteRegisterULong(			// д�Ľ������ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes,  
											ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI2312A_ReadRegisterByte(			// ���뵥�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes);

	WORD DEVAPI FAR PASCAL PCI2312A_ReadRegisterWord(			// ����˫�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes);

	ULONG DEVAPI FAR PASCAL PCI2312A_ReadRegisterULong(			// �������ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr, 
											ULONG OffsetBytes);

	//################# I/O�˿�ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	// ����Щ������Ҫ�����ڴ�ͳ�豸����ISA���ߡ����ڡ����ڵ��豸���������ڱ�PCI�豸
	BOOL DEVAPI FAR PASCAL PCI2312A_WritePortByte(HANDLE hDevice, __int64 pbPort, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI2312A_WritePortWord(HANDLE hDevice, __int64 pbPort, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI2312A_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI2312A_ReadPortByte(HANDLE hDevice, __int64 pbPort);
    WORD DEVAPI FAR PASCAL PCI2312A_ReadPortWord(HANDLE hDevice, __int64 pbPort);
    ULONG DEVAPI FAR PASCAL PCI2312A_ReadPortULong(HANDLE hDevice, __int64 pbPort);

	//########################### �̲߳������� ######################################
	HANDLE DEVAPI FAR PASCAL PCI2312A_CreateSystemEvent(void);			// �����ں��¼����󣬹�InitDeviceInt��VB���̵߳Ⱥ���ʹ��
	BOOL DEVAPI FAR PASCAL PCI2312A_ReleaseSystemEvent(HANDLE hEvent);	// �ͷ��ں��¼�����


#ifdef __cplusplus
}
#endif

// �Զ������������������
#ifndef _PCI2312A_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI2312A_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2312A_32.dll...")
		#pragma message("======== Successfully linked with PCI2312A_32.dll")
	#else
		#pragma comment(lib, "PCI2312A_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2312A_64.dll...")
		#pragma message("======== Successfully linked with PCI2312A_64.dll")
	#endif

#endif

#endif // _PCI2312A_DEVICE_
