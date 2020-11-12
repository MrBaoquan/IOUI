#ifndef _PCI2310_DEVICE_
#define _PCI2310_DEVICE_

//######################## �������� #################################
// CreateFileObject���õ��ļ�������ʽ������(��ͨ����ָ��ʵ�ֶ��ַ�ʽ������)
#define PCI2310_modeRead				0x0000		// ֻ���ļ���ʽ
#define PCI2310_modeWrite				0x0001		// ֻд�ļ���ʽ
#define	PCI2310_modeReadWrite			0x0002		// �ȶ���д�ļ���ʽ
#define PCI2310_modeCreate				0x1000		// ����ļ�������Դ������ļ���������ڣ����ؽ����ļ�������0
#define PCI2310_typeText				0x4000		// ���ı���ʽ�����ļ�

//***********************************************************

//***********************************************************
// �û������ӿ�
#ifndef _PCI2310_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## ����ͨ�ú��� #################################
	// �����ڱ��豸�����������
	HANDLE DEVAPI FAR PASCAL PCI2310_CreateDevice(int DeviceID = 0);		// �����豸����
	int DEVAPI FAR PASCAL PCI2310_GetDeviceCount(HANDLE hDevice);			// ȡ���豸��̨��
	int DEVAPI FAR PASCAL PCI2310_GetDeviceCurrentID(HANDLE hDevice);
	BOOL DEVAPI FAR PASCAL PCI2310_ListDeviceDlg(HANDLE hDevice);			// �б�ϵͳ���е����еĸ�PCI�豸
    BOOL DEVAPI FAR PASCAL PCI2310_ReleaseDevice(HANDLE hDevice);			// �ر��豸,��ֹ����,���ͷ���Դ

	//####################### ����I/O����������� #################################
	// �û�����ʹ��WriteRegisterULong��ReadRegisterULong�Ⱥ���ֱ�ӿ��ƼĴ�������I/O
	// �����������ʹ����������������ʡ�£�������Ҫ�����ļĴ��������λ�����ȣ���ֻ
	// ����VB�����Ե����Բ�����ô�򵥵�ʵ�ָ�������ͨ���Ŀ��ơ�
	BOOL DEVAPI FAR PASCAL PCI2310_SetDeviceDO(             // �������������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDOSts[32]);	// ����״̬

	BOOL DEVAPI FAR PASCAL PCI2310_GetDeviceDI(             // ȡ������������״̬     
										HANDLE hDevice,     // �豸���								        
										BYTE bDISts[32]);	// ����״̬	

	//####################### �жϺ��� #################################
	// ����Ӳ���źŵ�״̬�仯����CPU�����ж��¼�hEventInt��
	BOOL DEVAPI FAR PASCAL PCI2310_InitDeviceInt(			// ��ʼ���ж�
										HANDLE hDevice,     // �豸���	
										HANDLE hEventInt);	// �ж��¼����,����CreateSystemEvent����
	LONG DEVAPI FAR PASCAL PCI2310_GetDeviceIntCount(HANDLE hDevice);  // ���жϳ�ʼ��������ȡ���жϷ����������Ĵ���
	BOOL DEVAPI FAR PASCAL PCI2310_ReleaseDeviceInt(HANDLE hDevice); // �ͷ��ж���Դ

	//################# �ڴ�ӳ��Ĵ���ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	BOOL DEVAPI FAR PASCAL PCI2310_GetDeviceAddr(			// ȡ��ָ����ָ���豸�Ĵ���������Ի���ַ�������ַ
									HANDLE hDevice,			// �豸������,����CreateDevice��������
									PUCHAR* LinearAddr,		// ����ָ���Ĵ���������Ե�ַ
									PUCHAR* PhysAddr,		// ����ָ���Ĵ�����������ַ
									int RegisterID = 0);	// �豸�Ĵ������ID�ţ�0-5��

    BOOL DEVAPI FAR PASCAL PCI2310_WritePortByte(HANDLE hDevice, PUCHAR pbPort, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI2310_WritePortWord(HANDLE hDevice, PUCHAR pbPort, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI2310_WritePortULong(HANDLE hDevice, PUCHAR pbPort, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI2310_ReadPortByte(HANDLE hDevice, PUCHAR pbPort);
    WORD DEVAPI FAR PASCAL PCI2310_ReadPortWord(HANDLE hDevice, PUCHAR pbPort);
    ULONG DEVAPI FAR PASCAL PCI2310_ReadPortULong(HANDLE hDevice, PUCHAR pbPort);

	//########################### �̲߳������� ######################################
	HANDLE DEVAPI FAR PASCAL PCI2310_CreateSystemEvent(void);			// �����ں��¼����󣬹�InitDeviceInt��VB���̵߳Ⱥ���ʹ��
	BOOL DEVAPI FAR PASCAL PCI2310_ReleaseSystemEvent(HANDLE hEvent);	// �ͷ��ں��¼�����


#ifdef __cplusplus
}
#endif

// �Զ������������������
#ifndef _PCI2310_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI2310_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2310_32.dll...")
		#pragma message("======== Successfully linked with PCI2310_32.dll")
	#else
		#pragma comment(lib, "PCI2310_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI2310_64.dll...")
		#pragma message("======== Successfully linked with PCI2310_64.dll")
	#endif

#endif

#endif // _PCI2310_DEVICE_
