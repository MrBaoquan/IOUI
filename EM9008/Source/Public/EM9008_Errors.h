#ifndef EM9008_ERRORS_H
#define EM9008_ERRORS_H

//��������
#define EM9008_SUCCESS                           0 //�ɹ�
#define EM9008_FAILURE                          -1 //ʧ��
#define EM9008_TIMEOUT_ERROR                    -2 //��ʱ����
#define EM9008_CONNECT_ERROR                    -3 //�������Ӵ��󣬷����˴����Ӧ�����½�������
#define EM9008_SEND_ERROR                       -4 //���ʹ���
#define EM9008_CMDVERIFY_ERROR                  -5 //��������У�����
#define EM9008_RECVVERIFY_ERROR                 -6 //��������У�����
#define EM9008_DATANOTREADY                     -7 //����û��׼����
#define EM9008_PARA_ERROR                       -8 //��������
#define EM9008_FREQBASE_ERROR                   -9 //��������
#define EM9008_CH_ERROR                        -10 //ͨ�����ô���һ��������û������ʹ��ͨ������ȫ��ͨ����ʹ�ܵ��¡�
#define EM9008_DATA_CONNECT_ERROR              -11 //�������Ӵ��󣬷����˴����Ӧ�����½�������
#define EM9008_INITNET_ERROR                    -100 //��ʼ���������
#define EM9008_SOCKET_ERROR                     -101 //�������׽��ִ���
#define EM9008_FILEPATH_ERROR                   -201 //�ļ�Ŀ¼����
#define EM9008_FILEWRITE_ERROR                  -202 //д�ļ�����
#define EM9008_FILEREAD_ERROR                   -203 //���ļ�����
#define EM9008_FILESEEK_ERROR                   -204 //���ļ�����
#define EM9008_FILECLOSE_ERROR                  -205 //���ļ�����

#endif
