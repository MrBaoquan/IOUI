#ifndef PCI8407_H
#define PCI8407_H

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
ZT_API unsigned long _stdcall ZT8407_GetLastErr();
//�������ƣ�ZT8407_GetLastErr
//�������ܣ��õ���ǰ����š���������Ϊ0��ʾ�޴���
//����ֵ��������룬������뺬���뿴�궨��
ZT_API void _stdcall ZT8407_ClearLastErr();
//�������ƣ�ZT8407_ClearLastErr
//�������ܣ��������š�
//          ע�⣺һ����������Ϊ��ʹ������������ִ�У�������������
//����ֵ����
ZT_API long _stdcall ZT8407_GetCardCount();
//�������ƣ�ZT8407_GetCardCount
//�������ܣ��õ�ϵͳ�������ͺŵİ忨��������δ��װ�����İ忨����
//          
//��ڲ�������
//����ֵ�� >=0  ��ʾϵͳ����ʶ��������ͺŵİ忨��������
ZT_API long _stdcall ZT8407_GetOpenedCardCount();
//�������ƣ�ZT8407_GetOpenedCardCount
//�������ܣ��õ���ǰ�������Ѿ��򿪵������ͺŵİ忨��������
//          
//��ڲ�������
//����ֵ�� >=0  ��ʾ��ǰ�������Ѿ��򿪵������ͺŵİ忨������
ZT_API long _stdcall ZT8407_CloseAllDevice();
//�������ƣ�ZT8407_CloseAllDevice
//�������ܣ��رյ�ǰ�����д򿪵�������ͬ�ͺŵ��豸
//��ڲ�����
//          ��
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �жϳ���ԭ��
ZT_API void _stdcall ZT8407_SetBaseNO(unsigned long baseNO);
//�������ƣ�ZT8407_SetBaseNO
//�������ܣ����ð忨������(cardNO)��оƬ��(chipNO)���ں�(portNO)��ͨ����(chNO)��
//          ��֮����NO����׺�Ĳ����Ǵ�0��ʼ���Ǵ�1��ʼ��Ĭ��Ϊ��1��ʼ
//          ���鲻Ҫ�����������
//��ڲ�����
//          baseNO��ֻ����Ϊ0��1
//����ֵ����
ZT_API long _stdcall ZT8407_GetBaseNO();
//�������ƣ�ZT8407_GetBaseNO
//�������ܣ����ص�ǰ�忨������(cardNO)��оƬ��(chipNO)���ں�(portNO)��ͨ����(chNO)��
//          ��֮����NO����׺�Ĳ����Ǵ�0��ʼ���Ǵ�1��ʼ��Ĭ��Ϊ��1��ʼ
//����ֵ������0  ����0��ʼ
//        ����1  ����1��ʼ
ZT_API long _stdcall ZT8407_OpenDevice(unsigned long cardNO);
//�������ƣ�ZT8407_OpenDevice
//�������ܣ����豸�������ڵ�����������֮ǰ���á�
//          ���ڳ����ʼ��ʱ���ñȽ�ǡ����ֻ����һ�μ���
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ
//                    ���ϵͳ��ֻ����һ�������ͺŵİ忨�����������Ϊ1������
//                    ���ϵͳ�����˶�������ͺŵİ忨������ַ��С�İ忨������Ϊ1������ַ��С�İ忨������Ϊ2���Դ�����
//                    ע�⣺�û�һ�㲻�ù��İ忨����ַ�ľ���ֵ
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8407_CloseDevice(unsigned long cardNO);
//�������ƣ�ZT8407_CloseDevice
//�������ܣ��ر�ָ�����豸
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ
//                    ���ϵͳ��ֻ����һ�������ͺŵİ忨�����������Ϊ1������
//                    ���ϵͳ�����˶�������ͺŵİ忨������ַ��С�İ忨������Ϊ1������ַ��С�İ忨������Ϊ2���Դ�����
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8407_GetBaseAddr(unsigned long cardNO);
//�������ƣ�ZT8407_GetBaseAddr
//�������ܣ��õ�ָ���忨�Ļ���ַ��
//          ��2K��XP���û������õõ��Ļ���ֱַ�ӷ��ʶ˿�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8407_OpenDevice ����
//
//����ֵ��  ����0  ��ʾ���ذ忨�Ļ���ַ
//            -1   ��ʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8407_GetCardStatus(unsigned long cardNO);
//�������ƣ�ZT8407_GetCardStatus
//�������ܣ�����ָ������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8407_OpenDevice ����
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8407_WriteD(unsigned long cardNO, 
                                   unsigned long nOffset,
                                   unsigned long dataDoubleWord);
//�������ƣ�ZT8407_WriteD
//�������ܣ���IO��ʽ���԰忨�Ĵ�������32λд
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8407_OpenDevice ����
//         nOffset��ƫ�Ƶ�ַ����Ӳ��˵�����Ͽ��Բ鵽
//  dataDoubleWord��Ҫд��Ĵ��ֵ
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8407_DOBit(unsigned long cardNO, unsigned long chNO, unsigned long nState);
//�������ƣ�ZT8407_DOBit
//�������ܣ�ָ��ĳͨ���Ŀ��������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8407_OpenDevice ����
//            chNO��ͨ���ţ�1--32
//          nState��ָ��ĳͨ���Ŀ��������״̬��
//                    nState =    0    ��ָ������͵�ƽ
//                    nState = 1(���0)��ָ������ߵ�ƽ
//����ֵ��  
//           0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8407_DOAll(unsigned long cardNO, unsigned long nStateAll);
//�������ƣ�ZT8407_DOAll
//�������ܣ�ָ������ͨ���Ŀ��������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8407_OpenDevice ����
//       nStateAll��ָ������ͨ���Ŀ��������״̬��
//                    nStateAll =      0    ��ָ������ͨ������͵�ƽ
//                    nStateAll = 0xFFFFFFFF��ָ������ͨ������ߵ�ƽ
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8407_GetLastErr �������ҳ���ԭ��
ZT_API unsigned long _stdcall ZT8407_GetLastDO(unsigned long cardNO);
//�������ƣ�ZT8407_GetLastDO
//�������ܣ��õ����п��������(DO)ͨ�������һ�ο����������ֵ��
//          ע�⣺1. ���ϵͳ������δ�����κο������������֮ǰ�����ô˺����᷵��0
//                2. �˺����������κο����������������ֻ�ǲ�ѯһ���ϴε�ֵ
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8407_OpenDevice ����
//����ֵ��
//             0     ��ʾ�������һ��ָ������ͨ��DO����͵�ƽ
//        0xFFFFFFFF ��ʾ�������һ��ָ������ͨ��DO����ߵ�ƽ
//            -1   ��ʾ���ó�����Ӧ�ý�һ������ ZT8407_GetLastErr �����жϳ���ԭ��
}
#endif
