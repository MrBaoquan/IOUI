#ifndef _USB5538_DEVICE_
#define _USB5538_DEVICE_

#include<windows.h>

//***********************************************************
// ���������ӿ�
#ifndef _USB5538_DRIVER_
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//######################## ����ͨ�ú��� #################################
	HANDLE DEVAPI FAR PASCAL USB5538_CreateDevice(int DeviceLgcID = 0); // �����豸����(�ú���ʹ��ϵͳ���߼��豸ID��
	HANDLE DEVAPI FAR PASCAL USB5538_CreateDeviceEx(int DevicePhysID = 0); // �����豸����(�ú���ʹ�ð忨�����豸ID��
	int DEVAPI FAR PASCAL USB5538_GetDeviceCount(HANDLE hDevice);      // ȡ��USB5538��ϵͳ�е��豸����
	BOOL DEVAPI FAR PASCAL USB5538_GetDeviceCurrentID(HANDLE hDevice, PLONG DeviceLgcID, PLONG DevicePhysID); // ȡ�õ�ǰ�豸���߼�ID�ź�����ID��
	BOOL DEVAPI FAR PASCAL USB5538_ListDeviceDlg(void); // �öԻ����б�ϵͳ���е�����USB5538�豸
	BOOL DEVAPI FAR PASCAL USB5538_ResetDevice(HANDLE hDevice);		 // ��λ����USB�豸
    BOOL DEVAPI FAR PASCAL USB5538_ReleaseDevice(HANDLE hDevice);    // �豸���

	//####################### ����I/O����������� #################################
	BOOL DEVAPI FAR PASCAL USB5538_GetDeviceDI(					// ȡ�ÿ�����״̬     
									HANDLE hDevice,				// �豸���,��Ӧ��CreateDevice��������								        
									BYTE bDISts[16]);			// ��������״̬(ע��: ���붨��Ϊ8���ֽ�Ԫ�ص�����)

    BOOL DEVAPI FAR PASCAL USB5538_SetDeviceDO(					// ���������״̬
									HANDLE hDevice,				// �豸���,��Ӧ��CreateDevice��������								        
									BYTE bDOSts[16]);			// �������״̬(ע��: ���붨��Ϊ8���ֽ�Ԫ�ص�����)

	BOOL DEVAPI FAR PASCAL USB5538_RetDeviceDO(					// �ض����������״̬
									HANDLE hDevice,				// �豸���,��Ӧ��CreateDevice��������								        
									BYTE bDOSts[16]);			// �������״̬(ע��: ���붨��Ϊ8���ֽ�Ԫ�ص�����)

#ifdef __cplusplus
}
#endif

// �Զ������������������
#ifndef _USB5538_DRIVER_
#ifndef _WIN64
#pragma comment(lib, "USB5538_32.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with USB5538_32.dll...")
#pragma message("======== Successfully linked with USB5538_32.dll")
#else
#pragma comment(lib, "USB5538_64.lib")
#pragma message("======== Welcome to use our art company's products!")
#pragma message("======== Automatically linking with USB5538_64.dll...")
#pragma message("======== Successfully linked with USB5538_64.dll")
#endif

#endif

#endif; // _USB5538_DEVICE_