#ifndef PCI8409_H
#define PCI8409_H

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
#define ERR_PARAMETER_BASEADDR 21 //���ISA�����忨����ַ������Χ��Ӧ����0x100��0x3F0֮��δ��ϵͳռ�õĵ�ַ
#define ERR_PARAMETER_CARDNO 22 //���PCI�����忨�����ų�����Χ����������а忨�����Ŵ�1��ʼ
#define ERR_PARAMETER_CHMODE 23 //ͨ����ʽ��������ͨ����ʽ������Χ��һ��ֻ��0--3��4�ַ�ʽ
#define ERR_PARAMETER_CH 24 //ͨ���Ų�������ͨ���ų�����Χ
#define ERR_PARAMETER_CHIP_OR_GROUP 25 //оƬ�Ż���Ų�������оƬ�Ż���ų�����Χ
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
#define ERR_EEPROM_ID 44L          //EEPROM��ʶ����ȷ�����ܵ�ԭ��1. EEPROMδ��ʼ�� 2. EEPROM�����ڣ�3. EEPROM��
#define ERR_EEPROM_VERSION 45L     //δ֪EEPROM���ݸ�ʽ�汾
#define ERR_EEPROM_REC_FORMAT 46L //���ݸ�ʽ����ȷ
#define ERR_EEPROM_CRC 47L         //CRC�����
#define ERR_EEPROM_REC_DATA 48L        //��¼�а��������ݴ�����EEPROM�а����Ƿ�����
#define ERR_EEPROM_REC_NOTFOUND 49L  //δ�ҵ�ָ����¼
#define ERR_EEPROM_REC_ADDR 50L    //δ�ҵ����ݵ�ַ��
#define ERR_EEPROM_READ 51L        //EEPROM������ʧ��
#define ERR_EEPROM_WRITE 52L        //EEPROMд����ʧ��
#define ERR_SW_MODE_COLLISION 53L  //��������������ǰ���õĿ�������ʽ��ͻ

#endif

extern "C" 
{
ZT_API unsigned long _stdcall ZT8409_GetLastErr();
//�������ƣ�ZT8409_GetLastErr
//�������ܣ��õ���ǰ����š���������Ϊ0��ʾ�޴���
//����ֵ��������룬������뺬���뿴�궨��
ZT_API void _stdcall ZT8409_ClearLastErr();
//�������ƣ�ZT8409_ClearLastErr
//�������ܣ��������š�
//          ע�⣺һ����������Ϊ��ʹ������������ִ�У�������������
//����ֵ����
ZT_API long _stdcall ZT8409_OpenDevice(unsigned long cardNO);
//�������ƣ�ZT8409_OpenDevice
//�������ܣ����豸�������ڵ�����������֮ǰ���á�
//          ���ڳ����ʼ��ʱ���ñȽ�ǡ����ֻ����һ�μ���
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ
//                    ���ϵͳ��ֻ����һ�������ͺŵİ忨�����������Ϊ1������
//                    ���ϵͳ�����˶�������ͺŵİ忨������ַ��С�İ忨������Ϊ1������ַ��С�İ忨������Ϊ2���Դ�����
//                    ע�⣺�û�һ�㲻�ù��İ忨����ַ�ľ���ֵ
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8409_CloseDevice(unsigned long cardNO);
//�������ƣ�ZT8409_CloseDevice
//�������ܣ��ر�ָ�����豸
//
//��ڲ�����ZT8409_CloseDevice
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ
//                    ���ϵͳ��ֻ����һ�������ͺŵİ忨�����������Ϊ1������
//                    ���ϵͳ�����˶�������ͺŵİ忨������ַ��С�İ忨������Ϊ1������ַ��С�İ忨������Ϊ2���Դ�����
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8409_GetBaseAddr(unsigned long cardNO);
//�������ƣ�ZT8409_GetBaseAddr
//�������ܣ��õ�ָ���忨�Ļ���ַ��
//          ��2K��XP���û������õõ��Ļ���ֱַ�ӷ��ʶ˿�
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//
//����ֵ��  ����0  ��ʾ���ذ忨�Ļ���ַ
//            -1   ��ʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �жϳ���ԭ��

///////////////////////////////////// ���������� /////////////////////////////////////////

ZT_API long _stdcall ZT8409_SetIOMode(unsigned long cardNO, 
                                      unsigned long groupNO, 
                                      unsigned long nIOMode);
//�������ƣ�ZT8409_SetIOMode
//�������ܣ�����ĳһ�鿪�����������������
//          ע�⣺������ʹ��ǰ���������������������
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//         nIOMode������IO��ʽ��0 = ���뷽ʽ��1 = �����ʽ
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_GetIOMode(unsigned long cardNO, 
                                      unsigned long groupNO);
//�������ƣ�ZT8409_GetIOMode
//�������ܣ��õ�ĳһ�鿪���������������������
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//����ֵ��   0  ��ʾ��һ�鵱ǰΪ���뷽ʽ
//           1  ��ʾ��һ�鵱ǰΪ�����ʽ
//          -1      ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_SetIOModeAll(unsigned long cardNO, unsigned long nIOMode);
//�������ƣ�ZT8409_SetIOModeAll
//�������ܣ����������鿪�����������������
//            ��ʾ����������Ϊ10��
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         nIOMode������IO��ʽ��ÿһ��ռ1��bit
//                    ���磺0x0 = ������(��10��)����Ϊ���뷽ʽ��
//                          0x1 = ��1����Ϊ�����ʽ����������Ϊ���뷽ʽ
//                          0x2 = ��2����Ϊ�����ʽ����������Ϊ���뷽ʽ
//                          0x3FF = ������(��10��)����Ϊ�����ʽ
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_GetIOModeAll(unsigned long cardNO);
//�������ƣ�ZT8409_GetIOModeAll
//�������ܣ����������鿪�����Ĺ�����ʽ��
//            ��ʾ����������Ϊ10��
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//����ֵ�� ���ڵ���0   �����鿪�����Ĺ�����ʽ
//             -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_DIBit(unsigned long cardNO, 
                                  unsigned long groupNO, 
                                  unsigned long chNO);
//�������ƣ�ZT8409_DIBit
//�������ܣ��õ�ĳ����ĳ��ͨ���Ŀ���������״̬
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//            chNO��ͨ���ţ��� groupNO = 1,2,3,4,7,8,9,10ʱ��chNOȡֵ��Χ�� 1--16
//                          �� groupNO = 5,6ʱ��chNOȡֵ��Χ�� 1--8
//����ֵ��
//          0 ��ʾ�͵�ƽ
//          1 ��ʾ�ߵ�ƽ
//         -1 ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_DIAll(unsigned long cardNO, unsigned long groupNO);
//�������ƣ�ZT8409_DIAll
//�������ܣ��õ�ĳ��������ͨ���Ŀ���������״̬��
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//����ֵ��
//         ���ڵ���0  ��ʾ��һ������ͨ���Ŀ���������״̬
//            -1      ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_DOBit(unsigned long cardNO, 
                                  unsigned long groupNO, 
                                  unsigned long chNO, 
                                  unsigned long nState);
//�������ƣ�ZT8409_DOBit
//�������ܣ�ָ��ĳ����ĳ��ͨ���Ŀ��������״̬
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//            chNO��ͨ���ţ��� groupNO = 1,2,3,4,7,8,9,10ʱ��chNOȡֵ��Χ�� 1--16
//                          �� groupNO = 5,6ʱ��chNOȡֵ��Χ�� 1--8
//          nState��ָ��ĳͨ���Ŀ��������״̬��
//                    nState =    0    ��ָ������͵�ƽ
//                    nState = 1(���0)��ָ������ߵ�ƽ
//����ֵ��  
//           0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_DOAll(unsigned long cardNO, 
                                  unsigned long groupNO, 
                                  unsigned long nStateAll);
//�������ƣ�ZT8409_DOAll
//�������ܣ��趨ĳ�������ͨ���Ŀ��������״̬��
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//       nStateAll��ָ������������ͨ���Ŀ��������״̬
//
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_GetLastDO(unsigned long cardNO, unsigned long groupNO);
//�������ƣ�ZT8409_GetLastDO
//�������ܣ��õ�ĳ��������ͨ�����һ�ο����������ֵ��
//
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         groupNO�����(1--10)��10��
//����ֵ��
//         ���ڵ���0  ��ʾ��һ������ͨ���Ŀ���������״̬
//            -1      ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_GetGroupChNO(unsigned long logicChNO, 
                                         unsigned long &groupNO, 
                                         unsigned long &chNO);
//�������ƣ�ZT8409_GetGroupChNO
//�������ܣ����������ͨ����(1--144)�����ĸ�����ĸ�ͨ��
//
//��ڲ�����
//       logicChNO��ͨ���ţ�1--144��������ͨ��������д�����
//         groupNO���û�����һ������������������������
//            chNO���û�����һ���������������������ͨ����
//
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_DIBitEx(unsigned long cardNO, 
                                    unsigned long logicChNO, 
                                    unsigned long byMode);
//�������ƣ�ZT8409_DIBitEx
//�������ܣ�Ϊ�˷����û�����������ͨ���Ŵ�����õ�ĳ��ͨ���Ŀ���������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//       logicChNO��ͨ���ţ�1--144��������ͨ��������д�����
//          byMode���Ƿ����ж�ͨ���Ĺ�����ʽ���ٷ���ֵ��0 = ���жϣ�1 = �ж�
//                    ˵������ byMode ��Ϊ 1 ʱ������û�ָ����ͨ���Ĺ�����ʽΪ���ʱ������0��
//                                    ��Ϊ������Ϊ���������ʽ��ͨ����ȥ��������״̬��ì�ܵģ�
//                          �� byMode ��Ϊ 0 ʱ�������ѴӼĴ���������ֱֵ�ӷ��أ������ж�
//����ֵ��
//          0 ��ʾ�͵�ƽ
//          1 ��ʾ�ߵ�ƽ
//         -1 ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_DOBitEx(unsigned long cardNO, 
                                    unsigned long logicChNO, 
                                    unsigned long nState, 
                                    unsigned long byMode);
//�������ƣ�ZT8409_DOBitEx
//�������ܣ�Ϊ�˷����û�����������ͨ���Ŵ���������ĳ��ͨ���Ŀ��������״̬
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//       logicChNO��ͨ���ţ�1--144��������ͨ��������д�����
//          nState��ָ��ĳͨ���Ŀ��������״̬��
//                    nState =    0    ��ָ������͵�ƽ
//                    nState = 1(���0)��ָ������ߵ�ƽ
//          byMode���Ƿ����ж�ͨ���Ĺ�����ʽ���ٷ���ֵ��0 = ���жϣ�1 = �ж�
//                    ˵������ byMode ��Ϊ 1 ʱ������û�ָ����ͨ���Ĺ�����ʽΪ����ʱ����������
//                                    ��Ϊ������Ϊ���빤����ʽ��ͨ����ȥ���������״̬��ì�ܵģ�
//                          �� byMode ��Ϊ 0 ʱ������ֱ�����üĴ�����ֵ�������ж�
//����ֵ��  
//           0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API long _stdcall ZT8409_GetChIOMode(unsigned long cardNO, unsigned long logicChNO);
//�������ƣ�ZT8409_GetChIOMode
//�������ܣ�Ϊ�˷����û�����������ͨ���Ŵ�����õ�ĳ��ͨ���Ĺ�����ʽ
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//       logicChNO��ͨ���ţ�1--144��������ͨ��������д�����
//����ֵ��
//          0 ��ʾ���뷽ʽ
//          1 ��ʾ�����ʽ
//         -1 ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API unsigned long _stdcall ZT8409_GetUInt32Bits(unsigned long value, 
                                                   unsigned long mask,
                                                   unsigned long bit);
//�������ƣ�ZT8409_GetUInt32Bits
//�������ܣ���һ�� UInt32 ���͵ı����õ�ĳ��λ��ĳЩλ��ֵ��
//
//��ڲ�����
//           value��һ�� UInt32 ���͵ı���
//            mask������
//             bit������λ��ʼ0--31
//����ֵ��
//         ���ڵ���0  ��ʾ��һ������ͨ���Ŀ���������״̬
//            -1      ��ʾ���ó�����Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��
ZT_API unsigned long _stdcall ZT8409_SetUInt32Bits(unsigned long value, 
                                                   unsigned long mask,
                                                   unsigned long bit,
                                                   unsigned long state);
//�������ƣ�ZT8409_SetUInt32Bits
//�������ܣ�����һ�� UInt32 ���͵ı�����ĳ��λ��ĳЩλ��ֵ��
//
//��ڲ�����
//           value��һ�� UInt32 ���͵ı���
//            mask������
//             bit������λ��ʼ0--31
//           state��״̬
//����ֵ��   0      ��ʾ�ɹ�
//          -1      ��ʾʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �������ҳ���ԭ��

///////////////////////////////////// �ر��ײ㺯�� /////////////////////////////////////////

ZT_API void _stdcall ZT8409_SetBaseNO(unsigned long baseNO);
//�������ƣ�ZT8409_SetBaseNO
//�������ܣ����ð忨������(cardNO)��оƬ��(chipNO)���ں�(portNO)��ͨ����(chNO)��
//          ��֮����NO����׺�Ĳ����Ǵ�0��ʼ���Ǵ�1��ʼ��Ĭ��Ϊ��1��ʼ
//          ���鲻Ҫ�����������
//��ڲ�����
//          baseNO��ֻ����Ϊ0��1
//����ֵ����
ZT_API long _stdcall ZT8409_GetBaseNO();
//�������ƣ�ZT8409_GetBaseNO
//�������ܣ����ص�ǰ�忨������(cardNO)��оƬ��(chipNO)���ں�(portNO)��ͨ����(chNO)��
//          ��֮����NO����׺�Ĳ����Ǵ�0��ʼ���Ǵ�1��ʼ��Ĭ��Ϊ��1��ʼ
//����ֵ������0  ����0��ʼ
//        ����1  ����1��ʼ
ZT_API long _stdcall ZT8409_ReadW(unsigned long cardNO,
                                unsigned long nOffset);
//�������ƣ�ZT8409_ReadW
//�������ܣ���IO��ʽ���԰忨�Ĵ�������16λ��
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         nOffset��ƫ�Ƶ�ַ����Ӳ��˵�����Ͽ��Բ鵽
//����ֵ��  ���ش��ڵ���0�������������ľ���ֵ
//          -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �жϳ���ԭ��
ZT_API long _stdcall ZT8409_WriteW(unsigned long cardNO,
                                   unsigned long nOffset,
                                   unsigned long dataWord);
//�������ƣ�ZT8409_WriteW
//�������ܣ���IO��ʽ���԰忨�Ĵ�������16λд
//��ڲ�����
//          cardNO���忨��Ĭ�ϴ� 1 ��ʼ���й������������ϸ˵������ο� ZT8409_OpenDevice ����
//         nOffset��ƫ�Ƶ�ַ����Ӳ��˵�����Ͽ��Բ鵽
//        dataWord��Ҫд��Ĵ��ֵ
//����ֵ��  0  ���ɹ�
//         -1  ��ʧ�ܣ�Ӧ�ý�һ������ ZT8409_GetLastErr �жϳ���ԭ��
}
#endif
