#ifndef _PCI8735_DEVICE_
#define _PCI8735_DEVICE_

#include "windows.h"

//#################### ADӲ������PCI8735_PARA_AD���� #####################
// ����AD������ʵ��Ӳ������
typedef struct _PCI8735_PARA_AD     
{
	LONG FirstChannel;      // ��ͨ��[0, 31]
	LONG LastChannel;       // ĩͨ��[0, 31],Ҫ��ĩͨ��������ڻ������ͨ��
	LONG InputRange;		// ģ�����������̷�Χ,��ȡֵ���������س�������
	LONG GroundingMode;		// �ӵط�ʽ(���˻�˫��ѡ��),��ȡֵ���������س�������
	LONG Gains;
} PCI8735_PARA_AD, *PPCI8735_PARA_AD;

//***********************************************************
// ADӲ������PCI8735_PARA_AD�е�InputRangeģ�������뷶Χ��ʹ�õ�ѡ��
const long PCI8735_INPUT_N10000_P10000mV= 0x00; // ��10000mV
const long PCI8735_INPUT_N5000_P5000mV	= 0x01; // ��5000mV
const long PCI8735_INPUT_N2500_P2500mV	= 0x02; // ��2500mV
const long PCI8735_INPUT_0_P10000mV		= 0x03; // 0��10000mV

//***********************************************************
// AD����PCI8735_PARA_AD�е�Gainsʹ�õ�Ӳ������ѡ��
const long PCI8735_GAINS_1MULT			= 0x00; // 1������
const long PCI8735_GAINS_2MULT			= 0x01; // 2������
const long PCI8735_GAINS_4MULT			= 0x02; // 4������
const long PCI8735_GAINS_8MULT			= 0x03; // 8������

//***********************************************************
// AD����PCI8735_PARA_AD�е�GroundingModeʹ�õ�ģ���źŽӵط�ʽѡ��
const long PCI8735_GNDMODE_SE			= 0x00;	// ���˷�ʽ(SE:Single end)
const long PCI8735_GNDMODE_DI			= 0x01;	// ˫�˷�ʽ(DI:Differential)

//***********************************************************
// �û������ӿ�
#ifndef _PCI8735_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## �豸��������� ##############################
	HANDLE DEVAPI FAR PASCAL PCI8735_CreateDevice(int DeviceLgcID = 0);     // ���߼��Ŵ����豸����
	HANDLE DEVAPI FAR PASCAL PCI8735_CreateDeviceEx(int DevicePhysID = 0);  // ������Ŵ����豸����
	int DEVAPI FAR PASCAL PCI8735_GetDeviceCount(HANDLE hDevice); // ȡ���豸��̨��
	BOOL DEVAPI FAR PASCAL PCI8735_GetDeviceCurrentID( // ��ȡ�߼�ID�ź�����ID��
									HANDLE hDevice,	   // �豸������,����CreateDevice��������	
									PLONG DeviceLgcID, // �߼�ID��
									PLONG DevicePhysID); // ��������ID����Ч
	BOOL DEVAPI FAR PASCAL PCI8735_ListDeviceDlg(HANDLE hDevice); // �ԶԻ����巽ʽ�б�ϵͳ���е����еĸ�PCI�豸
    BOOL DEVAPI FAR PASCAL PCI8735_ReleaseDevice(HANDLE hDevice); // ���ͷ��豸����

	//####################### AD���ݶ�ȡ���� #################################
	// ���ڴ������ͨ�û�����Щ�ӿ���򵥡����ݡ���ɿ������û�����֪���豸
	// �Ͳ㸴�ӵ�Ӳ������Э��ͷ����������Ʊ�̣���������ĳ�ʼ���豸�Ͷ�ȡ
	// AD�������������������ɸ�Ч��ʵ�ָ��١����������ݲɼ�

	BOOL DEVAPI FAR PASCAL PCI8735_InitDeviceAD(			// ��ʼ���豸��������TRUE��,�豸��׼������.
									HANDLE hDevice,			// �豸����,����CreateDevice��������
									PPCI8735_PARA_AD pADPara); // Ӳ������, �����ڴ˺����о���Ӳ��״̬

	BOOL DEVAPI FAR PASCAL PCI8735_ReadDeviceAD(			// ��ȡ�豸�ϵ�AD����
									HANDLE hDevice,			// �豸���,����CreateDevice��������
									USHORT ADBuffer[],		// ����ԭʼAD���ݵ��û�������
									LONG nReadSizeWords,	// �����ƫλ����������ݳ���(��)
									PLONG nRetSizeWords = NULL);	// ����ʵ�ʶ�ȡ�ĳ���(��)

	BOOL DEVAPI FAR PASCAL PCI8735_ReleaseDeviceAD(HANDLE hDevice); // ֹͣ�ͷ�AD����
									
	//##################### AD��Ӳ�������������� ###########################
	BOOL DEVAPI FAR PASCAL PCI8735_SaveParaAD(HANDLE hDevice, PPCI8735_PARA_AD pADPara); // ����ǰ��AD��������������ϵͳ��
    BOOL DEVAPI FAR PASCAL PCI8735_LoadParaAD(HANDLE hDevice, PPCI8735_PARA_AD pADPara); // ��AD����������ϵͳ�ж���
    BOOL DEVAPI FAR PASCAL PCI8735_ResetParaAD(HANDLE hDevice, PPCI8735_PARA_AD pADPara); // ��AD���������ָ�������Ĭ��ֵ

	//####################### ����I/O����������� #################################
	// �û�����ʹ��WriteRegisterULong��ReadRegisterULong�Ⱥ���ֱ�ӿ��ƼĴ�������I/O
	// �����������ʹ����������������ʡ�£�������Ҫ�����ļĴ��������λ�����ȣ���ֻ
	// ����VB�����Ե����Բ�����ô�򵥵�ʵ�ָ�������ͨ���Ŀ��ơ�
	BOOL DEVAPI FAR PASCAL PCI8735_GetDeviceDI(						// ȡ�ÿ�����״̬     
											HANDLE hDevice,			// �豸������,����CreateDevice��������								        
											BYTE bDISts[16]);		// ��������״̬(ע��: ���붨��Ϊ16���ֽ�Ԫ�ص�����)

    BOOL DEVAPI FAR PASCAL PCI8735_SetDeviceDO(						// ���������״̬
											HANDLE hDevice,			// �豸������,����CreateDevice��������        
											BYTE bDOSts[16]);		// �������״̬(ע��: ���붨��Ϊ16���ֽ�Ԫ�ص�����)

    BOOL DEVAPI FAR PASCAL PCI8735_RetDeviceDO(						// �ض����������״̬
											HANDLE hDevice,			// �豸������,����CreateDevice��������        
											BYTE bDOSts[16]);		// ��ÿ������״̬(ע��: ���붨��Ϊ16���ֽ�Ԫ�ص�����)

//################# �ڴ�ӳ��Ĵ���ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	BOOL DEVAPI FAR PASCAL PCI8735_GetDeviceBar(					// ȡ��ָ����ָ���豸�Ĵ�����BAR��ַ
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											__int64 pbPCIBar[6]);	// ����PCI BAR���е�ַ,����PCI BAR���ж��ٿ��õ�ַ�뿴Ӳ��˵����
    BOOL DEVAPI FAR PASCAL PCI8735_GetDevVersion(					// ��ȡ�豸�̼�������汾
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											PULONG pulFmwVersion,	// �̼��汾
											PULONG pulDriverVersion);// �����汾
	BOOL DEVAPI FAR PASCAL PCI8735_WriteRegisterByte(				// ���豸��ӳ��Ĵ����ռ�ָ���˿�д�뵥��������
											HANDLE hDevice,			// �豸����
											__int64 pbLinearAddr,	// ָ��ӳ��Ĵ��������Ի���ַ
											ULONG OffsetBytes,		// ����ڻ���ַ��ƫ��λ��
											BYTE Value);			// ��ָ����ַд�뵥�ֽ����ݣ����ַ�����Ի���ַ��ƫ��λ�þ�����
	BOOL DEVAPI FAR PASCAL PCI8735_WriteRegisterWord(				// д˫�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// ָ��ӳ��Ĵ��������Ի���ַ
											ULONG OffsetBytes,  
											WORD Value);
	BOOL DEVAPI FAR PASCAL PCI8735_WriteRegisterULong(				// д�Ľ������ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// ָ��ӳ��Ĵ��������Ի���ַ
											ULONG OffsetBytes,  
											ULONG Value);
	BYTE DEVAPI FAR PASCAL PCI8735_ReadRegisterByte(				// ���뵥�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// ָ��ӳ��Ĵ��������Ի���ַ
											ULONG OffsetBytes);
	WORD DEVAPI FAR PASCAL PCI8735_ReadRegisterWord(				// ����˫�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// ָ��ӳ��Ĵ��������Ի���ַ
											ULONG OffsetBytes);
	ULONG DEVAPI FAR PASCAL PCI8735_ReadRegisterULong(				// �������ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 pbLinearAddr,	// ָ��ӳ��Ĵ��������Ի���ַ
											ULONG OffsetBytes);

	//################# I/O�˿�ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	// ����Щ������Ҫ�����ڴ�ͳ�豸����ISA���ߡ����ڡ����ڵ��豸���������ڱ�PCI�豸
    BOOL DEVAPI FAR PASCAL PCI8735_WritePortByte(HANDLE hDevice, __int64 pbPort, ULONG offserBytes, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI8735_WritePortWord(HANDLE hDevice, __int64 pbPort, ULONG offserBytes, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI8735_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG offserBytes, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI8735_ReadPortByte(HANDLE hDevice, __int64 pbPort, ULONG offserBytes);
    WORD DEVAPI FAR PASCAL PCI8735_ReadPortWord(HANDLE hDevice, __int64 pbPort, ULONG offserBytes);
    ULONG DEVAPI FAR PASCAL PCI8735_ReadPortULong(HANDLE hDevice, __int64 pbPort, ULONG offserBytes);

	//########################### ���Ӳ������� ######################################
	HANDLE DEVAPI FAR PASCAL PCI8735_CreateSystemEvent(void);			// �����ں��¼����󣬹�InitDeviceInt��VB���̵߳Ⱥ���ʹ��
	BOOL DEVAPI FAR PASCAL PCI8735_ReleaseSystemEvent(HANDLE hEvent);	// �ͷ��ں��¼�����

#ifdef __cplusplus
}
#endif

// �Զ������������������
#ifndef _PCI8735_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI8735_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8735_32.dll...")
		#pragma message("======== Successfully linked with PCI8735_32.dll")
	#else
		#pragma comment(lib, "PCI8735_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8735_64.dll...")
		#pragma message("======== Successfully linked with PCI8735_64.dll")
	#endif
	
#endif

#endif // _PCI8735_DEVICE_