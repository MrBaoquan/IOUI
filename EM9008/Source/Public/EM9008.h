// EM9008_Server.h: interface for the CEM9008 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EM9008_Server_H__D1413467_66A2_475B_8398_F579D95B3FB0__INCLUDED_)
#define AFX_EM9008_Server_H__D1413467_66A2_475B_8398_F579D95B3FB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZT_Type.h"

#define EM9008_TIMOUT_MS                            1000
#define EM9008_BASE_FREQ                            120000000.0//����ʱ�ӣ�120MHz
#define EM9008_MAXADCHCNT                           8 //AD���ͨ����
#define EM9008_MAXDICHCNT                           8 //DI���ͨ����
#define EM9008_MAXDOCHCNT                           8 //DO���ͨ����
#define EM9008_MAXCTCHCNT                           6 //���������ͨ����
#define EM9008_MAXECCHCNT                           1 //���������ͨ����
#define EM9008_MAXPWMCHCNT                          7 //PWM���ͨ����

//AD����Ƶ������
#define EM9008_AD_BASE_FREQ_1000K                          0
#define EM9008_AD_BASE_FREQ_571K                           1
#define EM9008_AD_BASE_FREQ_211K                           2
#define EM9008_AD_BASE_FREQ_114K                           3
#define EM9008_AD_BASE_FREQ_60K                            4

//AD�ɼ�Ƶ������
#define EM9008_AD_FREQ_7812                          0
#define EM9008_AD_FREQ_3906                          1
#define EM9008_AD_FREQ_1953                          2
#define EM9008_AD_FREQ_977                           3
#define EM9008_AD_FREQ_488                           4
#define EM9008_AD_FREQ_244                           5
#define EM9008_AD_FREQ_122                           6
#define EM9008_AD_FREQ_61                            7

//AD�ɼ���Χ
#define EM9008_AD_RANGE_N10_10V                      0L //��10V
#define	EM9008_AD_RANGE_N5_5V                        1L //��5V��Ӳ����ʱ��֧�֣�
#define	EM9008_AD_RANGE_0_10V                        2L //0��10V
#define	EM9008_AD_RANGE_0_5V                         3L //0��5V
#define	EM9008_AD_RANGE_0_20mA                       4L //0��20mA

//������ʽ
#define EM9008_CT_MODE_COUNT                         0 //����
#define EM9008_CT_MODE_LFREQ                         1 //��Ƶ��

//DI����
#define EM9008_DI_FUN_DI                            0 //DI
#define EM9008_DI_FUN_CT                            1 //����
#define EM9008_DI_FUN_ECA                           1 //������A
#define EM9008_DI_FUN_ECB                           1 //������B
#define EM9008_DI_FUN_ECZ                           3 //������Z

//DO����ܽŹ���
#define EM9008_DO_FUN_DO                             0 //��ʾDO����
#define EM9008_DO_FUN_PWM                            1 //��ʾ���PWM�ź�

#define EM9008_SFIFO_SIZE                            16 * 1024 * 1024// ��������СΪ16MB

extern "C"
{
HANDLE _stdcall EM9008_DeviceCreate();
//�������ܣ������豸
//��ڲ�����
//      ��
//����ֵ��0�������豸ʧ��
//        ����ֵΪ�豸��������Ժ���豸��ز����о�Ҫʹ���䷵��ֵ

void _stdcall EM9008_DeviceClose( HANDLE hDevice );
//�������ܣ��ر��豸��ͬʱ�ͷ��豸��ռ����Դ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//����ֵ����

I32 _stdcall EM9008_CmdConnect( HANDLE hDevice, char* strIP, I32 ipBC, I32 cmdPort, I32 dataPort, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���������˿�
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//             strIP���豸IP
//              ipBC��strIP�ĳ��ȣ��˲���Ϊ0ʱ��ʾstrIP����0Ϊ�������ַ�����
//           cmdPort������˿ںš�
//          dataPort�����ݶ˿ںš�
//         timeOutMS����ʱ��������
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

void _stdcall EM9008_CmdClose( HANDLE hDevice );
//�������ܣ��ر�����˿�
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//����ֵ����

I32 _stdcall EM9008_DataConnect( HANDLE hDevice, int port, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��������ݶ˿�
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              port�����ݶ˿ں�
//����ֵ��>=0��ʾ�ɹ���<0��ʾʧ��

void _stdcall EM9008_DataClose( HANDLE hDevice, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��ر����ݶ˿�
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//����ֵ����

/////////////////AD��غ���//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_AdSetRange( HANDLE hDevice, I32 chNo, I32 rangeInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�����AD�ɼ���Χ��ע��Ҫ���豸����һ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��ͨ����1��8
//          rangeInx���ɼ���Χ
//                    0����10V
//                    1����5V��Ӳ����ʱ��֧�֣�
//                    2��0~10V
//                    3��0~5V
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_AdSetBeginEndCh( HANDLE hDevice, I32 beginCh, I32 endCh, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�����ADͨ�������Ƿ������ٻ�������ֻ��ָ����ͨ�����ܹ����뻺����
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//           beginCh����ʼͨ����1��endCh
//             endCh����ֹͨ����beginCh��8
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_AdCodeToValue( HANDLE hDevice, I32 chNo, U16* adCode, I32 codeCount, F64* adValue );
//�������ܣ���ADԭ��ֵת��������ֵ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��ͨ���ţ�1~8
//            adCode��ԭ��ֵ
//         codeCount��pCode����������Ԫ�ظ���
//���ڲ�����
//           adValue��AD����ֵת���������ѹֵ����Ҫ�û�����ռ䣬��Ԫ�ظ���ΪcodeCount��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_AdReadAllOnce( HANDLE hDevice, U16 adCode[EM9008_MAXADCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���ȡ����һ��ԭ��ֵ���˺����������һ��AD�Ĳɼ����
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//            adCode��32λ�������飬8Ԫ�أ���ӦAD1��AD8
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

/////////////////Ӳ��������غ���//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_HcSetTranBytesCount( HANDLE hDevice, I32 bytesCount, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�������λ��ÿ�δ�����λ���������ֽڸ������˺���һ��������������
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//        bytesCount���ֽڸ�����4��65535��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_HcStart( HANDLE hDevice, I32 isHcStart, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�����Ӳ�����Ʋɼ�
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//         isHcStart���Ƿ�������0��ʾֹͣ�ɼ���1��ʾ�����ɼ�
//���ڲ�����
//                ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_HcSetBaseFreq( HANDLE hDevice,  I32 freqInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�����AD�����ɼ�Ƶ��
//    ע�⣺�豸����ʵ�ʵĲɼ�Ƶ�ʺ�Ӳ���˲�����������˲������й�ϵ������ز���������Ϻ󣬿��Ե���EM9008_HcGetChFreq�õ�ÿ��ͨ���Ĳɼ�Ƶ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//            freqInx���ɼ�Ƶ���������庬����ο���AD����Ƶ�����á��궨��
//���ڲ�����
//                ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_HcSetFreq( HANDLE hDevice,  I32 freqInx, F64* realFreq, I32 timeOutMS  = EM9008_TIMOUT_MS );
//�������ܣ�����AD�ɼ�Ƶ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//            freqInx��ֻ������0��4���ɼ�Ƶ���������庬����ο���AD�ɼ�Ƶ�����á��궨��
//���ڲ�����
//           realFreq�����������Ĳɼ�Ƶ�ʡ�
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_AdSetHardFilterInx( HANDLE hDevice,  I32 hardFilterInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�����Ӳ���˲�ϵ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//     hardFilterInx��0�����˲�
//                    1��2��
//                    2��4��
//                    3��8��
//                    4��16��
//                    5��32��
//                    6��64��
//���ڲ�����
//                ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_AdSetSoftFilter( HANDLE hDevice,  I32 softFilterTimes, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���������˲�ϵ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//   softFilterTimes��ֻ�����ó�1��255��
//���ڲ�����
//                ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_HcGetChFreq( HANDLE hDevice,  F64* chFreq, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���ȡÿͨ�����ݲɼ�Ƶ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//            chFreq��ÿͨ���ɼ�Ƶ�ʣ�����
//                    EM9008_HcSetBaseFreq��EM9008_AdSetHardFilterInx��EM9008_AdSetSoftFilter��EM9008_AdSetBeginEndCh
//                    �������������ü���õ�����˱����ȵ������漸���������ŵ��ô˺������ܹ��õ���ȷ��Ƶ��ֵ��
//                ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_HcGetGroupBc( HANDLE hDevice );
//�������ܣ��õ�ÿ���ֽ�����Ҳ���ǰ�����ÿ�ɼ�һ�����ݣ����뻺�����������ֽ�����
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//����ֵ��>0��ʾ�ֽ���

I32 _stdcall EM9008_HcSFifoCanReadCount( HANDLE hDevice, I32* canReadCount );
//�������ܣ��õ���λ���������ɶ����ݸ���
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//      canReadCount����ǰ�ɶ����ݸ���
//����ֵ��>0��ʾ�ֽ���

I32 _stdcall EM9008_HcSFifoRead( HANDLE hDevice, I32 dataCount, U16* dataBuffer, I32* realReadCount );
//�������ܣ�������λ��������������
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//         dataCount��Ҫ��ȡ�����ݸ�����������Ϊͨ�����������������ݴ���
//���ڲ�����
//        dataBuffer��ԭ���������飬���СΪdataCount
//     realReadCount��ʵ�ʶ�ȡ���ݸ���
//����ֵ��>0��ʾ�ֽ���

/////////////////��������غ���//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_IoReadAllOnce( HANDLE hDevice, I8 iStatus[EM9008_MAXDICHCNT], I8 oStatus[EM9008_MAXDOCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��õ����п������������һ������
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//           iStatus�����飬iStatus[0]~iStatus[7]��ӦDI1~DI8
//           oStatus�����飬oStatus[0]~oStatus[7]��ӦDO1~DO8
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_DoWriteAllOnce( HANDLE hDevice, I8 oStatus[EM9008_MAXDOCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��������п��������
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//           oStatus�����飬oStatus[0]~oStatus[7]��ӦDO1~DO8
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_DoWriteInitStatus( HANDLE hDevice, I8 intiStatus[EM9008_MAXDOCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��������п���������ϵ��ʼ״̬
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//        intiStatus�����飬oStatus[0]~oStatus[7]��ӦDO1~DO8
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_DoSetMode( HANDLE hDevice, I32 chNo, I32 modeInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ÿ������������
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��1��7
//           modeInx��0��ʾΪ��������1��ʾΪPWM
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_DiSetMode( HANDLE hDevice, I32 chNo, I32 modeInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ÿ��������빦��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��ͨ���ţ�1��8
//           modeInx��0��ʾ������������ֵ��ֵͬʱ�京���ͨ����ء�
//                   chNo=1ʱ��modeInx=1��ʾ������A
//                   chNo=2ʱ��modeInx=1��ʾ������B
//                   chNo=3ʱ��modeInx=1��ʾ������1������modeInx=2��ʾ������1��Ƶ��modeInx=3��ʾ�����������ź�
//                   chNo=4ʱ��modeInx=1��ʾ������2������modeInx=2��ʾ������2��Ƶ��modeInx=3��ʾPWMʧЧ�ź�1
//                   chNo=5ʱ��modeInx=1��ʾ������3������modeInx=2��ʾ������3��Ƶ��modeInx=3��ʾPWMʧЧ�ź�2
//                   chNo=6ʱ��modeInx=1��ʾ������4������modeInx=2��ʾ������4��Ƶ��modeInx=3��ʾPWMʧЧ�ź�3
//                   chNo=7ʱ��modeInx=1��ʾ������5������modeInx=2��ʾ������5��Ƶ
//                   chNo=8ʱ��modeInx=1��ʾ������6������modeInx=2��ʾ������6��Ƶ
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

/////////////////��������غ���//////////////////////////////////////////////////////////////////////////////////////////////////
//ע��˴���������Ҫ������һ·DI��������Ϊ��������ſ�����ȷִ��
I32 _stdcall EM9008_CtSetFreqBase( HANDLE hDevice, F64 freqBase_ms, F64* real_ms, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���������ͨ���ļ�������Ƶ��׼�������ڣ��Ժ���Ϊ��λ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//       freqBase_ms����Ƶ��׼���ڣ��Ժ���Ϊ��λ
//���ڲ�����
//           real_ms��ʵ�ʲ�Ƶ��׼������������freqBase_msһ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_CtReadAllOnce( HANDLE hDevice, U32 ctCode[EM9008_MAXCTCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���ȡ����·��������ԭ��ֵ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//            ctCode��������ԭ��ֵ������Ǽ�����ʽ���Ǽ���ֵ������ǲ�Ƶ����Ҫ�������ת����Ƶ��ֵ
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_CtCodeToValue( HANDLE hDevice, I32 chNo, U32* ctCode, I32 codeCount, F64* ctValue );
//�������ܣ���ԭ��ֵת���ɶ�Ӧ������ֵ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��ͨ����
//            ctCode��������ԭ��ֵ������Ǽ�����ʽ���Ǽ���ֵ������ǲ�Ƶ����Ҫ�������ת����Ƶ��ֵ
//         codeCount��ԭ��ֵ������ctCode��ctValue�Ĵ�С����˲����й�
//���ڲ�����
//           ctValue������������ֵ������Ǽ�����ʽ���Ǽ���ֵ������ǲ�Ƶ���ǲ�Ƶֵ
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_CtClearAll( HANDLE hDevice, I8 clearBit[EM9008_MAXCTCHCNT] , I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�ָ��ͨ�����������㣬������ֻ���ڼ�����ʽ�²������壬���ִ�к�Ὣ��Ӧͨ������Ϊ����������ʽ	
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//           isClear��6Ԫ�����飬ÿһ��Ԫ�ض�Ӧһ·��������1��ʾ��Ӧͨ������,0��ʾ��Ӧͨ��������
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_CtSetMode( HANDLE hDevice, I32 chNo, I8 ctMode, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�ָ��ͨ��������������ʽ	
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��ͨ����
//            ctMode��������ʽ��0��������1����Ƶ
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

/////////////////��������غ���//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_EcReadAllOnce( HANDLE hDevice, I32* abCode, F64* freq, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ���ȡ����������ֵ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//            abCode��������ABֵ��32λ�з���ָ�룬��Ҫ�û�����ռ䡣
//             freq��������Ƶ�ʣ�64λ������ָ�룬��Ҫ�û�����ռ䣬���ָ��ֵΪ0�����ʾ������Ƶ�ʡ�
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_EcSetMaxPos( HANDLE hDevice, U32 maxPosition, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ñ��������λ��
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//       maxPosition�����������λ�ã�������������ֵ������ֵ�󽫴�0��ʼ����
//���ڲ�����
//           ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_EcSetVelocityInx( HANDLE hDevice, I32 velocityInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ñ��������ٲ���
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//       velocityInx�����ٲ�����
//���ڲ�����
//       ��
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_EcSetMode( HANDLE hDevice, I32 ecEn, I32 ecMode, I32 ecFilter, I32 ecZeroInxEnable, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ñ���������ģʽ
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              ecEn��ʹ��λ������Ϊ1
//            ecMode������ģʽ��0��ʾ�����ź����룬1��ʾʱ�ӷ����ź�����
//          ecFilter���˲�ϵ����0��15������Խ���˲�Ч��Խ�á�
//   ecZeroInxEnable�������ź�ʹ�ܣ�0��ֹ�����źţ�1ʹ�ܹ����źš�ʹ�ܹ����ź�ʱ����⵽�����źű���������������
//���ڲ�����
//       maxPosition�����������λ�ã�������������ֵ������ֵ�󽫴�0��ʼ����
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_EcClear( HANDLE hDevice, I32 chNo, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ�����������ֵ����
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//              chNo��ָ��ͨ���ŵı��������㣬���豸ֻ��һ��ͨ�������ֻ��Ϊ1
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_SetWtdStatus( HANDLE hDevice, I8 enableWtd, U16 s, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ÿ��Ź�
//��ڲ�����
//           hDevice���豸�����EM9008_DeviceCreate��������ֵ
//         enableWtd��1��ʹ�ܿ��Ź���0����ֹ���Ź�
//                 s����ʱ������1��30�������ָ��ʱ����û������λ�������κ�ָ�����λ�����Ḵλ
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

/////////////////���������غ���//////////////////////////////////////////////////////////////////////////////////////////////////
//ע��˴���������Ҫ������һ·DO��������ΪPWM��ſ�����ȷִ��
I32 _stdcall EM9008_PwmSetBaseClkDiv( HANDLE hDevice, I8 baseClkDiv, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��趨PWM�����׼ʱ��
//��ڲ�����
//                hDevice���豸�����EM9008_DeviceCreate��������ֵ
//             baseClkDiv����׼ʱ�ӷ�Ƶϵ����0��7
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_PwmGetBaseClkDiv( HANDLE hDevice, I8* baseClkDiv, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��õ�PWM�����׼ʱ��
//��ڲ�����
//                hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//             baseClkDiv����׼ʱ�ӷ�Ƶϵ����0��7
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_PwmGetFreqRange( HANDLE hDevice, F64* minHz, F64* maxHz, I8* baseClkDiv, I32* sysClkFreq, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��õ�PWM���Ƶ�ʷ�Χ��Ƶ�ʷ�Χ��EM9008_PwmSetBaseClkDiv���õĻ�׼ʱ��ϵ���й�ϵ
//��ڲ�����
//                hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//                  minHz����С������Ƶ��
//                  maxHz����������Ƶ�ʣ�ע��Ƶ��Խ�ӽ���������Ƶ�ʣ�ռ�ձȿɵ��ڷ�ΧԽС��
//                         ����������Ƶ��ʱ��ռ�ձ�ֻ��Ϊ50%��
//                         ������Ӳ�����ޣ�ʵ������޷��ﵽ��������Ƶ�ʣ�������ο�Ӳ���ֲᡣ
//             baseClkDiv����׼ʱ�ӷ�Ƶϵ��
//             sysClkFreq��ϵͳʱ��Ƶ�ʣ�һ��Ϊ120,000,000Hz
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_PwmSetPulse( HANDLE hDevice, I32 chNo, F64 freq, F64 duty, F64* realFreq, F64* realDuty, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��趨ָ��ͨ����PWM����������
//          ע�⣺7·PWM��Ϊ���飬��1��2����3��4����5��6����7����ÿ��ֻ��������ͬ�����Ƶ�ʣ����ͨ��2��4��6Ƶ��������Ч��ֻ������ռ�ձ�
//��ڲ�����
//                hDevice���豸�����EM9008_DeviceCreate��������ֵ
//                   chNo��ͨ���ţ�1��7
//                   freq�����Ƶ�ʡ��䷶Χ���ڵ���EM9008_PwmSetBaseClkDiv�����EM9008_PwmGetFreqRange�������
//              dutyCycle��ռ�ձȡ�0~1��������ڵ�λȡ�������Ƶ�ʣ����Ƶ��Խ�ͣ��ɵ��ڵĵ�λԽ�ࡣ
//���ڲ�����
//               realFreq����ʵƵ��ֵ���˲�����Ч�������EM9008_PwmGetPulse���
//          realDutyCycle����ʵռ�ձ�ֵ���˲�����Ч�������EM9008_PwmGetPulse���
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_PwmGetPulse( HANDLE hDevice, I32 chNo, F64 freq, F64 duty, F64* realFreq, F64* realDuty, I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ����ݸ���Ƶ�ʺ�ռ�ձȼ����ʵ��Ƶ�ʺ�ռ�ձȡ�
//��ڲ�����
//                hDevice���豸�����EM9008_DeviceCreate��������ֵ
//                   chNo��ͨ���ţ�1��7
//                   freq�����Ƶ�ʡ�
//              dutyCycle��ռ�ձȡ�0~1��������ڵ�λȡ�������Ƶ�ʣ����Ƶ��Խ�ͣ��ɵ��ڵĵ�λԽ�ࡣ
//���ڲ�����
//               realFreq����ʵƵ��ֵ�����������������趨Ƶ�ʺ���ʵƵ��֮�����Щ���
//          realDutyCycle����ʵռ�ձ�ֵ�����������������趨ռ�ձȺ���ʵռ�ձ�֮�����Щ���
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_PwmStartAll( HANDLE hDevice, I8 startOrStop[EM9008_MAXPWMCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//�������ܣ��趨����ͨ����PWM�����ʼ����ֹͣ
//��ڲ�����
//                hDevice���豸�����EM9008_DeviceCreate��������ֵ
//            startOrStop�������������飬����Ԫ��0~7��ӦPWM1~PWM7����PWM1Ϊ����
//                         startOrStop[0]=0��ֹͣ���
//                         startOrStop[0]=1���������
//����ֵ��0��ʾ�ɹ���<0��ʾʧ��

I32 _stdcall EM9008_SysSetBaseClkDiv( HANDLE hDevice, I8 baseClkDiv, I32 timeOutMS = EM9008_TIMOUT_MS );
//�˺�����Ҫ�����ڲ��������û�����ʹ��
I32 _stdcall EM9008_SysGetSysClkFreq( HANDLE hDevice, I32* sysClkFreq, I32 timeOutMS = EM9008_TIMOUT_MS );
//�˺�����Ҫ�����ڲ��������û�����ʹ��

/////////////////////���к���Ϊ��ʷ�����ļ���ز���////////////////////////////////////////////////////////
I32 _stdcall EM9008_DFWOpen( HANDLE hDevice, char* dirPath, I32 pathLen );
//�������ܣ���ʼ����ʷ����д�����������
//��ڲ�����
//        hDevice���豸�����EM9008_DeviceCreate��������ֵ
//        dirPath�������ʷ�ļ���·��������󳤶Ȳ��ܳ���256
//        pathLen��·�������ȣ����Ϊ0��ʾdisPath����0��β���ַ�����
//                ���򽫻���ָ��Ŀ¼���Զ�������yy-mm-dd-hh-MM-ss-1.dat���ļ���������������ʱ����,-1��ʾ��һ���ļ���ÿ���ļ�512MB���ҡ�����512MB���ļ�����˳������-2��-3����
//���ڲ�����
//                ��
//����ֵ��0��ʾû�д���

I32 _stdcall EM9008_DFWClose( HANDLE hDevice );
//�������ܣ��ر���ʷ�����ļ�
//��ڲ�����
//          hDevice���豸�����EM9008_DeviceCreate��������ֵ
//���ڲ�����
//                ��
//����ֵ��0��ʾû�д���

I32 _stdcall EM9008_DFROpen( HANDLE hDevice, char* filePathName, I32 pathLen );
//�������ܣ���ʼ����ʷ���ݶ�ȡ��������
//��ڲ�����
//        filePathName������·��������ʷ�ļ�����ע���䳤�Ȳ��ܳ���256���ַ�
//             pathLen��ǰһ���ַ����ĳ��ȡ����Ϊ0����Ĭ��filePathName����0Ϊ��β��C�����ַ�����
//���ڲ�����
//                ��
//����ֵ����0ֵ��ʾ�ļ����
//        0��ʾ���ļ�ʧ�ܡ�
//        ע�����ʷ����ʱ���ܹ�ͬʱ�����豸���������ȵ�����EM9008_CmdConnect���ҳɹ����أ���˺����᷵��ʧ��

I32 _stdcall EM9008_DFRClose( HANDLE hDevice );
//�������ܣ��ر���ʷ���ļ�
//��ڲ�����
//           hDevice����ʷ�ļ����
//���ڲ�����
//                ��
//����ֵ����

I32 _stdcall EM9008_DFRRead( HANDLE hDevice, F64 beginGroupInx, I32 groupCount, I32* realReadCount );
//�������ܣ���ȡ��ʷ�ļ����Ҹ��ݶ�ȡ���������ݰ���ͨ���ŵ���������
//��ڲ�����
//              hDevice����ʷ�ļ����
//        groupCount��Ҫ��ȡ��������ע������һ��������ÿ��ʹ��ͨ��������һ�Ρ�
//     beginGroupInx����ȡ��ʼ��λ�á����Ϊ-1�����ʾ����һ�ζ�ȡ�Ľ�����ʼ��ȡ��
//���ڲ�����
//     realReadCount��ʵ�ʶ�ȡ��ÿͨ�����ݸ�����
//����ֵ��0ֵ�ɹ�
//        <0��ʾʧ�ܣ�������ο�EM9008_Error.H��

I32 _stdcall EM9008_DFRGetFreq( HANDLE hDevice, F64* daqFreq );
//�������ܣ��õ���ʷ�ļ��Ĳɼ�Ƶ��
//��ڲ�����
//              hDevice����ʷ�ļ����
//���ڲ�����
//      daqFreq���ɼ�Ƶ��
//����ֵ��0ֵ�ɹ�
//        <0��ʾʧ�ܣ�������ο�EM9008_Error.H��

I32 _stdcall EM9008_DFRGetGroupTotal( HANDLE hDevice, F64* groupTotal, F64* timeTotal );
//�������ܣ��õ���ʷ�ļ�������������������������ʱ��
//��ڲ�����
//              hDevice����ʷ�ļ����
//���ڲ�����
//        groupTotal��������
//        timeTotal����ʱ��
//����ֵ��0ֵ�ɹ�
//        <0��ʾʧ�ܣ�������ο�EM9008_Error.H��

I32 _stdcall EM9008_DFRGetAdChRange( HANDLE hDevice, I32* adBeginCh, I32* adEndCh );
//�������ܣ��õ���ʷ�ļ���������ʹ��ADͨ��
//��ڲ�����
//              hDevice����ʷ�ļ����
//���ڲ�����
//            adBeginCh����ʼͨ��
//              adEndCh����ֹͨ��
//����ֵ��0ֵ�ɹ�
//        <0��ʾʧ�ܣ�������ο�EM9008_Error.H��

I32 _stdcall EM9008_DFRGetRange( HANDLE hDevice, I8 rangeInx[EM9008_MAXADCHCNT] );
//�������ܣ�����ʷ�����ļ��еõ�ÿ��ͨ���Ĳɼ���Χ
//��ڲ�����
//              hDevice����ʷ�ļ����
//���ڲ�����
//           typeInx��8Ԫ�����飬ÿ��ͨ���ķ�Χ
//����ֵ��=0,��ʾ�ɹ�

/////////////�����û�һ���ò����ĺ���
I32 _stdcall EM9008_AdWriteZero( HANDLE hDevice, I32 chNo, U16 zeroCode, I32 timeOutMS = EM9008_TIMOUT_MS );
//У׼��غ���
I32 _stdcall EM9008_AdWriteFull( HANDLE hDevice, I32 chNo, U16 fullCode, I32 timeOutMS = EM9008_TIMOUT_MS );
//У׼��غ���
I32 _stdcall EM9008_AdReadZeroFull( HANDLE hDevice, U16* zeroCode, U16* fullCode, I32 timeOutMS = EM9008_TIMOUT_MS );
//У׼��غ���
I32 _stdcall EM9008_HcHFifoStatus( HANDLE hDevice, I32* sFifoStatus, I32 timeOutMS = EM9008_TIMOUT_MS );
//��λ�����������״̬
I32 _stdcall EM9008_ReadCoreTemp( HANDLE hDevice, F64* coreTemperature, I32 timeOutMS = EM9008_TIMOUT_MS );
//��ȡ��λ�������¶�


}

#endif // !defined(AFX_EM9008_Server_H__D1413467_66A2_475B_8398_F579D95B3FB0__INCLUDED_)
