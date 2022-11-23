#ifndef _PCI2324_DEVICE_
#define _PCI2324_DEVICE_

//***********************************************************
// �жϺ���PCI2324_InitDeviceInt�е�INTSrcʹ�õ��ж�ѡ��ѡ��
const long PCI2324_INTSRC_DI0INT		= 0x00;	// ʹ��DI0�ж�(����������ͨ��0�����ز����ж�), =TRUEʹ��, =FALSE��ֹ
const long PCI2324_INTSRC_DI1INT		= 0x01;	// ʹ��DI1�ж�(����������ͨ��1�����ز����ж�), =TRUEʹ��, =FALSE��ֹ
const long PCI2324_INTSRC_DI01INT		= 0x02;	// ʹ��DI0��DI1�ж�(����������ͨ��0��1�����ز����ж�), =TRUEʹ��, =FALSE��ֹ

// �ж�״̬����
typedef struct _PCI2324_STATUS_INT	// �ж���Ϣ���Ʋ���
{
	LONG bDI0INT;						// DI0�����ش����ж�
	LONG bDI1INT;						// DI1�����ش����ж�
} PCI2324_STATUS_INT, *PPCI2324_STATUS_INT;
//######################## �������� #################################
// CreateFileObject���õ��ļ�������ʽ������(��ͨ����ָ��ʵ�ֶ��ַ�ʽ������)
#define PCI2324_modeRead          0x0000			// ֻ���ļ���ʽ
#define PCI2324_modeWrite         0x0001			// ֻд�ļ���ʽ
#define	PCI2324_modeReadWrite     0x0002			// �ȶ���д�ļ���ʽ
#define PCI2324_modeCreate        0x1000			// ����ļ�������Դ������ļ���������ڣ����ؽ����ļ�������0
#define PCI2324_typeText          0x4000			// ���ı���ʽ�����ļ�

//***********************************************************
// ����DA�ĸ�λ��ʽ��������PCI2324_InitDevProDA������ResetMode������
#define PCI2324_RESET_MODE_NEGATIVE        0x0000	// �����ȣ�-5V��-10V...��
#define PCI2324_RESET_MODE_ZERO			   0x0001	// ���(0V)

//***********************************************************
// �û������ӿ�
#ifndef _PCI2324_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## ����ͨ�ú��� #################################
	// �����ڱ��豸�����������
	HANDLE DEVAPI FAR PASCAL PCI2324_CreateDevice(int DeviceID = 0);		// �����豸����
	int DEVAPI FAR PASCAL PCI2324_GetDeviceCount(HANDLE hDevice);			// ȡ���豸��̨��
	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceCurrentID(HANDLE hDevice, PLONG DeviceLgcID, PLONG DevicePhysID);
	BOOL DEVAPI FAR PASCAL PCI2324_ListDeviceDlg(HANDLE hDevice);			// �б�ϵͳ���е����еĸ�PCI�豸
    BOOL DEVAPI FAR PASCAL PCI2324_ReleaseDevice(HANDLE hDevice);			// �ر��豸,��ֹ����,���ͷ���Դ

	//####################### ����I/O����������� #################################
	// �û�����ʹ��WriteRegisterULong��ReadRegisterULong�Ⱥ���ֱ�ӿ��ƼĴ�������I/O
	// �����������ʹ����������������ʡ�£�������Ҫ�����ļĴ��������λ�����ȣ���ֻ
	// ����VB�����Ե����Բ�����ô�򵥵�ʵ�ָ�������ͨ���Ŀ��ơ�
	BOOL DEVAPI FAR PASCAL PCI2324_SetDeviceDO(             // �������������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDOSts[32]);	// ����״̬	

	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceDI(             // ȡ������������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDISts[2]);	// ����״̬	

	//####################### �жϺ��� #################################
	// ����Ӳ���źŵ�״̬�仯����CPU�����ж��¼�hEventInt��
	BOOL DEVAPI FAR PASCAL PCI2324_InitDeviceInt(			// ��ʼ���ж�
										HANDLE hDevice,     // �豸���	
										HANDLE hEventInt,	// �ж��¼����,����CreateSystemEvent����
										LONG INTSrc);		// �ж�Դѡ��
	LONG DEVAPI FAR PASCAL PCI2324_GetDeviceIntCount(HANDLE hDevice);  // ���жϳ�ʼ��������ȡ���жϷ����������Ĵ���
	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceIntSrc(HANDLE hDevice, PPCI2324_STATUS_INT pINTSts);  // ���жϳ�ʼ��������ȡ���ж�Դ
	BOOL DEVAPI FAR PASCAL PCI2324_ReleaseDeviceInt(HANDLE hDevice); // �ͷ��ж���Դ

	//################# �ڴ�ӳ��Ĵ���ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	BOOL DEVAPI FAR PASCAL PCI2324_GetDeviceBar(					// ȡ��ָ����ָ���豸�Ĵ�����BAR��ַ
										HANDLE hDevice,			// �豸������,����CreateDevice��������
										__int64 pbPCIBar[6]);	// ����PCI BAR���е�ַ,����PCI BAR���ж��ٿ��õ�ַ�뿴Ӳ��˵����

	BOOL DEVAPI FAR PASCAL PCI2324_GetDevVersion(					// ��ȡ�豸�̼�������汾
										HANDLE hDevice,			// �豸������,����CreateDevice��������
										PULONG pulFmwVersion,	// �̼��汾
										PULONG pulDriverVersion);// �����汾

	BOOL DEVAPI FAR PASCAL PCI2324_WriteRegisterByte(			// ��ָ���Ĵ����ռ�λ��д�뵥��������
										HANDLE hDevice,			// �豸������,����CreateDevice��������
										__int64 pbLinearAddr,	// ָ���Ĵ��������Ի���ַ,������GetDeviceAddr�е�pbLinearAddr��������ֵ
										ULONG OffsetBytes,		// ��������Ի���ַ����ַ��ƫ��λ��(�ֽ�)
										BYTE Value);			// ��ָ����ַд�뵥�ֽ����ݣ����ַ�����Ի���ַ��ƫ��λ�þ�����

	BOOL DEVAPI FAR PASCAL PCI2324_WriteRegisterWord(			// д˫�ֽ����ݣ�����ͬ�ϣ�
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes,  
										WORD Value);

	BOOL DEVAPI FAR PASCAL PCI2324_WriteRegisterULong(			// д�Ľ������ݣ�����ͬ�ϣ�
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes,  
										ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI2324_ReadRegisterByte(			// ���뵥�ֽ����ݣ�����ͬ�ϣ�
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes);

	WORD DEVAPI FAR PASCAL PCI2324_ReadRegisterWord(			// ����˫�ֽ����ݣ�����ͬ�ϣ�
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes);

	ULONG DEVAPI FAR PASCAL PCI2324_ReadRegisterULong(			// �������ֽ����ݣ�����ͬ�ϣ�
										HANDLE hDevice, 
										__int64 pbLinearAddr, 
										ULONG OffsetBytes);

	//################# I/O�˿�ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	// ����Щ������Ҫ�����ڴ�ͳ�豸����ISA���ߡ����ڡ����ڵ��豸���������ڱ�PCI�豸
	BOOL DEVAPI FAR PASCAL PCI2324_WritePortByte(HANDLE hDevice, __int64 pbPort, BYTE Value);
	BOOL DEVAPI FAR PASCAL PCI2324_WritePortWord(HANDLE hDevice, __int64 pbPort, WORD Value);
	BOOL DEVAPI FAR PASCAL PCI2324_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI2324_ReadPortByte(HANDLE hDevice, __int64 pbPort);
	WORD DEVAPI FAR PASCAL PCI2324_ReadPortWord(HANDLE hDevice, __int64 pbPort);
	ULONG DEVAPI FAR PASCAL PCI2324_ReadPortULong(HANDLE hDevice, __int64 pbPort);


	//########################### �̲߳������� ######################################
	HANDLE DEVAPI FAR PASCAL PCI2324_CreateSystemEvent(void);			// �����ں��¼����󣬹�InitDeviceInt��VB���̵߳Ⱥ���ʹ��
	BOOL DEVAPI FAR PASCAL PCI2324_ReleaseSystemEvent(HANDLE hEvent);	// �ͷ��ں��¼�����	

#ifdef __cplusplus
}
#endif

// �Զ������������������
#ifndef _PCI2324_DRIVER_
#ifndef _WIN64
#pragma comment(lib, "PCI2324_32.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with PCI2324_32.dll...")
#pragma message("======== Successfully linked with PCI2324_32.dll")
#else
#pragma comment(lib, "PCI2324_64.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with PCI2324_64.dll...")
#pragma message("======== Successfully linked with PCI2324_64.dll")
#endif

#endif

#endif // _PCI2324_DEVICE_
