#ifndef _PCI8201_DEVICE_
#define _PCI8201_DEVICE_

//***********************************************************
// DA�������WriteDeviceDA��ģ���������Χ����OutputRange��ʹ�õ�ѡ��
const long PCI8201_OUTPUT_0_P5000mV			= 0x00;		// 0��5000mV
const long PCI8201_OUTPUT_0_P10000mV		= 0x01;		// 0��10000mV
const long PCI8201_OUTPUT_0_P10800mV		= 0x02;		// 0��10800mV
const long PCI8201_OUTPUT_N5000_P5000mV		= 0x03;		// ��5000mV
const long PCI8201_OUTPUT_N10000_P10000mV	= 0x04;		// ��10000mV
const long PCI8201_OUTPUT_N10800_P10800mV	= 0x05;		// ��10800mV
const long PCI8201_OUTPUT_0_P10mA			= 0x06;		// 0��10mA
const long PCI8201_OUTPUT_4_P20mA			= 0x07;		// 4��20mA

//***********************************************************
// �û������ӿ�
#ifndef _PCI8201_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## ����ͨ�ú��� #################################
	// �����ڱ��豸�����������
	HANDLE DEVAPI FAR PASCAL PCI8201_CreateDevice(int DeviceID = 0);		// �����豸����
	HANDLE DEVAPI FAR PASCAL PCI8201_CreateDeviceEx(LONG DevicePhysID = 0);  // �������Ŵ����豸����
	int DEVAPI FAR PASCAL PCI8201_GetDeviceCount(HANDLE hDevice);			// ȡ���豸��̨��
	BOOL DEVAPI FAR PASCAL PCI8201_GetDeviceCurrentID(HANDLE hDevice, PLONG DeviceLgcID, PLONG DevicePhysID);
	BOOL DEVAPI FAR PASCAL PCI8201_ListDeviceDlg(HANDLE hDevice);			// �б�ϵͳ���е����еĸ�PCI�豸
    BOOL DEVAPI FAR PASCAL PCI8201_ReleaseDevice(HANDLE hDevice);			// �ر��豸,��ֹ����,���ͷ���Դ

	//####################### DA����������� #################################
	// ���ڴ������ͨ�û�����Щ�ӿ���򵥡����ݡ���ɿ������û�����֪���豸
	// �Ͳ㸴�ӵ�Ӳ������Э��ͷ�����������Ʊ�̣���������һ������������
	// ��ʵ�ָ��١�������DA�������
	BOOL DEVAPI FAR PASCAL PCI8201_WriteDeviceDA(							// дDA����
									HANDLE hDevice,							// �豸������,����CreateDevice��������
									LONG OutputRange,						// ������̣����嶨����ο�����ĳ������岿��
									SHORT nDAData,							// �����DAԭʼ����[0, 4095]
									int nDAChannel);						// DA���ͨ��[0-7]

	//################# �ڴ�ӳ��Ĵ���ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	BOOL DEVAPI FAR PASCAL PCI8201_GetDeviceBar(					// ȡ��ָ����ָ���豸�Ĵ�����BAR��ַ
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											__int64 pbPCIBar[6]);	// ����PCI BAR���е�ַ,����PCI BAR���ж��ٿ��õ�ַ�뿴Ӳ��˵����
	BOOL DEVAPI FAR PASCAL PCI8201_GetDevVersion(					// ��ȡ�豸�̼�������汾
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											PULONG pulFmwVersion,	// �̼��汾
											PULONG pulDriverVersion);// �����汾

	BOOL DEVAPI FAR PASCAL PCI8201_WriteRegisterByte(		// ��ָ���Ĵ����ռ�λ��д�뵥��������
											HANDLE hDevice,			// �豸������,����CreateDevice��������
											__int64 LinearAddr,		// ָ���Ĵ��������Ի���ַ,������GetDeviceAddr�е�LinearAddr��������ֵ
											__int64 OffsetBytes,		// ��������Ի���ַ����ַ��ƫ��λ��(�ֽ�)
											BYTE Value);			// ��ָ����ַд�뵥�ֽ����ݣ����ַ�����Ի���ַ��ƫ��λ�þ�����

	BOOL DEVAPI FAR PASCAL PCI8201_WriteRegisterWord(		// д˫�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 LinearAddr, 
											__int64 OffsetBytes,  
											WORD Value);

	BOOL DEVAPI FAR PASCAL PCI8201_WriteRegisterULong(		// д�Ľ������ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 LinearAddr, 
											__int64 OffsetBytes,  
											ULONG Value);

	BYTE DEVAPI FAR PASCAL PCI8201_ReadRegisterByte(		// ���뵥�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 LinearAddr, 
											ULONG OffsetBytes);

	WORD DEVAPI FAR PASCAL PCI8201_ReadRegisterWord(		// ����˫�ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 LinearAddr, 
											ULONG OffsetBytes);

	ULONG DEVAPI FAR PASCAL PCI8201_ReadRegisterULong(		// �������ֽ����ݣ�����ͬ�ϣ�
											HANDLE hDevice, 
											__int64 LinearAddr, 
											ULONG OffsetBytes);

	//################# I/O�˿�ֱ�Ӳ�������д���� ########################
	// �������û��Ա��豸��ֱ�ӡ������⡢���Ͳ㡢�����ӵĿ��ơ�������������
	// ���ƶ�����Ҫ����Ŀ������̺Ϳ���Ч��ʱ�����û�����ʹ����Щ�ӿ�����ʵ�֡�
	// ����Щ������Ҫ�����ڴ�ͳ�豸����ISA���ߡ����ڡ����ڵ��豸���������ڱ�PCI�豸

	BOOL DEVAPI FAR PASCAL PCI8201_WritePortByte(HANDLE hDevice, __int64 pbPort, BYTE Value);
    BOOL DEVAPI FAR PASCAL PCI8201_WritePortWord(HANDLE hDevice, __int64 pbPort, WORD Value);
    BOOL DEVAPI FAR PASCAL PCI8201_WritePortULong(HANDLE hDevice, __int64 pbPort, ULONG Value);

    BYTE DEVAPI FAR PASCAL PCI8201_ReadPortByte(HANDLE hDevice, __int64 pbPort);
    WORD DEVAPI FAR PASCAL PCI8201_ReadPortWord(HANDLE hDevice, __int64 pbPort);
    ULONG DEVAPI FAR PASCAL PCI8201_ReadPortULong(HANDLE hDevice, __int64 pbPort);

	//########################### �̲߳������� ######################################
	HANDLE DEVAPI FAR PASCAL PCI8201_CreateSystemEvent(void);			// �����ں��¼����󣬹�InitDeviceInt��VB���̵߳Ⱥ���ʹ��
	BOOL DEVAPI FAR PASCAL PCI8201_ReleaseSystemEvent(HANDLE hEvent);	// �ͷ��ں��¼�����


#ifdef __cplusplus
}
#endif

// �Զ������������������
#ifndef _PCI8201_DRIVER_
	#ifndef _WIN64
		#pragma comment(lib, "PCI8201_32.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8201_32.dll...")
		#pragma message("======== Successfully linked with PCI8201_32.dll")
	#else
		#pragma comment(lib, "PCI8201_64.lib")
		#pragma message("======== Welcome to use our art company's products!")
		#pragma message("======== Automatically linking with PCI8201_64.dll...")
		#pragma message("======== Successfully linked with PCI8201_64.dll")
	#endif

#endif

#endif // _PCI8201_DEVICE_
