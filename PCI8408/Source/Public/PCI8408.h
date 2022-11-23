/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-30 9:51
 *  Email: mrma617@gmail.com
 */

#ifndef PCI8408_H
#define PCI8408_H

#ifdef PCI8KPLX_EXPORTS
#define ZT_API __declspec(dllexport)
#else
#define ZT_API __declspec(dllimport)

//��������
#define ZT_SUCCESS 0  //�޴���
#define ERR_PARAMETER1 1 //����1��
#define ERR_PARAMETER2 2 //����2��
#define ERR_PARAMETER3 3 //����3��
#define ERR_PARAMETER4 4 //����4��
#define ERR_PARAMETER5 5 //����5��
#define ERR_PARAMETER6 6 //����6��
#define ERR_PARAMETER7 7 //����7��
#define ERR_PARAMETER8 8 //����8��
#define ERR_PARAMETER9 9 //����9��
#define ERR_PARAMETER10 10 //����10��
#define ERR_PARAMETER11 11 //����11��
#define ERR_PARAMETER12 12 //����12��
#define ERR_PARAMETER13 13 //����13��
#define ERR_PARAMETER14 14 //����14��
#define ERR_PARAMETER15 15 //����15��
#define ERR_PARAMETER16 16 //����16��
#define ERR_PARAMETER17 17 //����17��
#define ERR_PARAMETER18 18 //����18��
#define ERR_PARAMETER19 19 //����19��
#define ERR_PARAMETER20 20 //����20��
#define ERR_PARAMETER_BASEADDR 21 //���ISA�����忨����ַ������Χ������ַӦ��Ϊ0x100��0x3F0֮��δ��ϵͳռ�õĵ�ַ
#define ERR_PARAMETER_CARDNO 22 //���PCI�����忨�����ų�����Χ����������а忨������Ĭ�ϴ�1��ʼ
#define ERR_PARAMETER_CHMODE 23 //ͨ����ʽ��������ͨ����ʽ������Χ��һ��ֻ��0--3��4�ַ�ʽ
#define ERR_PARAMETER_CH 24 //ͨ���Ų�������ͨ���ų�����Χ
#define ERR_PARAMETER_CHIP 25 //оƬ�Ų�������оƬ�ų�����Χ
#define ERR_PARAMETER_PORT 26 //�ںŲ������󣬿ںų�����Χ
#define ERR_PARAMETER_AI_RANGE 27 //AD���̲��������˿���֧������AD��������
#define ERR_PARAMETER_AI_AMP 28 //AD���������
#define ERR_PARAMETER_AI_STARTMODE 29 //AD������ʽ������
#define ERR_PARAMETER_AO_RANGE 30 //DA���̲��������˿���֧������DA�������
#define ERR_PARAMETER_ADFREQ 31 //AD�ɼ�Ƶ��(��AD��Ƶϵ��)������
#define ERR_PARAMETER_IRQ 32 //���ж���ز������������Ǵ˿���֧�������жϷ�ʽ���жϺ�
#define ERR_PARAMETER_NULL_POINTER 33 //��ָ�����ԭ�򣺻������׵�ַΪ�ջ����û�����Ĳ����������õ���ָ��
#define ERR_AD_OVERTIME 34 //AD��ʱ������ISA��������ԭ������ǣ�δ�忨��IO��ַ��忨�����ò�ƥ�䡣���ö�ʱ����AD���ⴥ������ADʱ��һ�㲻Ӧ���AD�Ƿ�ʱ
#define ERR_OPEN_DEVICE 35 //���豸ʧ�ܣ�����PCI��������ԭ������ǣ�δ�忨��δװ��������dll��sys�汾��һ��
#define ERR_CLOSE_DEVICE 36 //�ر��豸ʧ��
#define ERR_TIMING 37 //��ʱδ�����ⴥ������δ��
#define ERR_IOADDR_DA_OVERTIME 38 //�����DAд���̳�ʱ
#define ERR_OPEN_IRQ 39 //���жϳ���
#define ERR_FUNC_CANNT_RUN 40 //�˺�����������̨�����������
#define ERR_ASYNC_FUNC_FAILED 41 //�첽��������ʧ��
#define ERR_FUNC_OPERATE 42 //�ڵ�ǰ�Ŀ�������״̬�£���Ӧ�õ��ô˺���
#define ERR_EXCHANGE_DATA 43 //��ײ�����֮�佻�����ݳ���

#endif

extern "C" 
{
ZT_API unsigned long _stdcall ZT8408_GetLastErr();
//�������ƣ�ZT8408_GetLastErr
//�������ܣ��õ���ǰ����š���������Ϊ0��ʾ�޴���
//����ֵ��������룬������뺬���뿴�궨��
ZT_API void _stdcall ZT8408_ClearLastErr();
//�������ƣ�ZT8408_ClearLastErr
//�������ܣ��������š�
//          ע�⣺һ����������Ϊ��ʹ������������ִ�У�������������
//����ֵ����
ZT_API long _stdcall ZT8408_OpenDevice(unsigned long cardNO);
//�������ƣ�ZT8408_OpenDevice
//�������ܣ����豸�������ڵ�����������֮ǰ���á�
//          ���ڳ����ʼ��ʱ���ñȽ�ǡ����ֻ����һ�μ���
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ
//                    ���ϵͳ��ֻ����һ�������ͺŵİ忨�����������Ϊ1������
//                    ���ϵͳ�����˶�������ͺŵİ忨������ַ��С�İ忨������Ϊ1������ַ��С�İ忨������Ϊ2���Դ�����
//                    ע�⣺�û�һ�㲻�ù��İ忨����ַ�ľ���ֵ
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_CloseDevice(unsigned long cardNO);
//�������ƣ�ZT8408_CloseDevice
//�������ܣ��ر�ָ�����豸
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ
//                    ���ϵͳ��ֻ����һ�������ͺŵİ忨�����������Ϊ1������
//                    ���ϵͳ�����˶�������ͺŵİ忨������ַ��С�İ忨������Ϊ1������ַ��С�İ忨������Ϊ2���Դ�����
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_GetBaseAddr(unsigned long cardNO);
//�������ƣ�ZT8408_GetBaseAddr
//�������ܣ��õ�ָ���忨�Ļ���ַ��
//          ��2K��XP���û������õõ��Ļ���ֱַ�ӷ��ʶ˿�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//
//����ֵ��  ����0  ��ʾ���ذ忨�Ļ���ַ
//            -1   ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API unsigned short _stdcall ZT8408_ReadW(unsigned long cardNO,
                                            unsigned long nOffset);
//�������ƣ�ZT8408_ReadW
//�������ܣ���IO��ʽ���԰忨�Ĵ�������16λ��
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//         nOffset��ƫ�Ƶ�ַ����Ӳ��˵�����Ͽ��Բ鵽
//����ֵ��  ���ش��ڵ���0�������������ľ���ֵ
//          -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_WriteW(unsigned long cardNO,
                                   unsigned long nOffset,
                                   unsigned long dataWord);
//�������ƣ�
//�������ܣ���IO��ʽ���԰忨�Ĵ�������16λд
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//         nOffset��ƫ�Ƶ�ַ����Ӳ��˵�����Ͽ��Բ鵽
//        dataWord��Ҫд��Ĵ��ֵ
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_ClearIRQ(unsigned long cardNO, unsigned long clearIrqStatusMask);
//�������ƣ�ZT8408_ClearIRQ
//�������ܣ���ǰ8·�����������ж�״̬��־��
//            ���ڱ仯Ƶ��С�� 0.1Hz ��DIͨ���������ж�״̬��־��
//            ���ڱ仯Ƶ�ʴ��� 0.1Hz ��DIͨ����Ҫ���ж�״̬��־��
//��ڲ�����
//            cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//clearIrqStatusMask����Ӧ��bit��1��ʾ���ͨ���ж�״̬��־
//                      ���ڱ仯Ƶ��С�� 0.1Hz ��DIͨ���������жϣ�
//                      ���ڱ仯Ƶ�ʴ��� 0.1Hz ��DIͨ����Ҫ���жϡ�
//                      ���磺
//                            0x01 = �����1·DI�ж�״̬��־;
//                            0x02 = �����2·DI�ж�״̬��־;
//                            0x80 = �����8·DI�ж�״̬��־;
//                            0xFF = ����ȫ��8·DI�ж�״̬��־;
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_OpenIRQ(unsigned long cardNO, long hEventDI);
//�������ƣ�ZT8408_OpenIRQ
//�������ܣ���8408�����������жϡ�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//        hEventDI���ǡ������������жϡ��¼����
//                    ���û�����һ���¼���Ȼ����¼���������������������������жϲ�����
//                    �����Ὣ����¼�����Ϊ���ź�״̬���Ӷ������û������������ȴ����̡߳�
//                    ����û�����Ҫ�õ����̣߳��뱣���������Ϊ0
//����ֵ��  0  ��ʾ�ɹ�
//          -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_CloseIRQ(unsigned long cardNO);
//�������ƣ�ZT8408_CloseIRQ
//�������ܣ��ر�8408�����������ж�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_ReadIRQstatus(unsigned long cardNO,
                                          unsigned long* DIirqStatusMask,
                                          unsigned long* DIvalinIrq);
//�������ƣ�ZT8408_ReadIRQstatus
//�������ܣ����ж�ʱ����������(DI)��״̬��ǰ8·�ж�״̬��־
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
// DIirqStatusMask�������жϺ�ǰ8·����������ͨ�����ж�״̬��־��ÿ2��bit��ʾһ��ͨ�����ж�״̬��־
//                    ���磺
//                          0x0001 = ����1·DI�����ж�;
//                          0x0004 = ����2·DI�����ж�;
//                          0x0010 = ����3·DI�����ж�;
//                          0x0040 = ����4·DI�����ж�;
//                          0x0100 = ����5·DI�����ж�;
//                          0x0400 = ����6·DI�����ж�;
//                          0x1000 = ����7·DI�����ж�;
//                          0x4000 = ����8·DI�����ж�;
//      DIvalinIrq�������ж�ʱ�̣����п����������״̬
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_ReadDIirqCnt(unsigned long cardNO);
//�������ƣ�ZT8408_ReadDIirqCnt
//�������ܣ��������������������жϴ�����
//            ע�⣺ֻ��ǰ8·�����������ܹ������ж�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//����ֵ��  ���ڵ���0  ��DI�жϴ���
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_ClearDIirqCnt(unsigned long cardNO);
//�������ƣ�ZT8408_ClearDIirqCnt
//�������ܣ��������������������жϴ������㡣
//            ע�⣺ֻ��ǰ8·�����������ܹ������ж�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8408_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8408_DIBit(unsigned long cardNO, unsigned long chNO);
//�������ƣ�ZT8408_DIBit
//�������ܣ��õ�ָ��ͨ���Ŀ���������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//            chNO��ͨ���ţ�1--16
//����ֵ��
//          0 ��ʾ�͵�ƽ
//          1 ��ʾ�ߵ�ƽ
//         -1 ��ʾ���ó�����Ӧ�ý�һ������ZT8408_GetLastErr�������ҳ���ԭ��
ZT_API long _stdcall ZT8408_DIAll(unsigned long cardNO);
//�������ƣ�ZT8408_DIAll
//�������ܣ��õ�����ͨ���Ŀ���������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//����ֵ��
//             0   ��ʾ����ͨ���Ŀ���������״̬Ϊ�͵�ƽ
//          0xFFFF ��ʾ����ͨ���Ŀ���������״̬Ϊ�ߵ�ƽ
//            -1   ��ʾ���ó�����Ӧ�ý�һ������ZT8408_GetLastErr�������ҳ���ԭ��
ZT_API long _stdcall ZT8408_DOBit(unsigned long cardNO, unsigned long chNO, unsigned long nState);
//�������ƣ�ZT8408_DOBit
//�������ܣ�ָ��ĳͨ���Ŀ��������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//            chNO��ͨ���ţ�1--16
//          nState��ָ��ĳͨ���Ŀ��������״̬��
//                    nState =    0    ��ָ������͵�ƽ
//                    nState = 1(���0)��ָ������ߵ�ƽ
//����ֵ��  
//           0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ZT8408_GetLastErr�������ҳ���ԭ��
ZT_API long _stdcall ZT8408_DOAll(unsigned long cardNO, unsigned long nStateAll);
//�������ƣ�ZT8408_DOAll
//�������ܣ�ָ������ͨ���Ŀ��������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//       nStateAll��ָ������ͨ���Ŀ��������״̬��
//                    nStateAll =   0   ��ָ������ͨ������͵�ƽ
//                    nStateAll = 0xFFFF��ָ������ͨ������ߵ�ƽ
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ZT8408_GetLastErr�������ҳ���ԭ��
ZT_API long _stdcall ZT8408_GetLastDO(unsigned long cardNO);
//�������ƣ�ZT8408_GetLastDO
//�������ܣ��õ����п��������(DO)ͨ�������һ�ο����������ֵ��
//          ע�⣺1. ���ϵͳ������δ�����κο������������֮ǰ�����ô˺����᷵��0
//                2. �˺����������κο����������������ֻ�ǲ�ѯһ���ϴε�ֵ
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8408_OpenDevice ����
//����ֵ��
//           0   ��ʾ�������һ��ָ������ͨ��DO����͵�ƽ
//        0xFFFF ��ʾ����ͨ���Ŀ���������״̬Ϊ�ߵ�ƽ
//                 ע�⣺�����з���32λ������0xFFFFFFFF = -1�����������ֵʱ��
//                       Ӧ�ý�һ������ ZT8408_GetLastErr �����ж��Ƿ������һ������
}
#endif
