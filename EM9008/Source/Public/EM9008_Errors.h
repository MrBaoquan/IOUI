#ifndef EM9008_ERRORS_H
#define EM9008_ERRORS_H

//定义错误号
#define EM9008_SUCCESS                           0 //成功
#define EM9008_FAILURE                          -1 //失败
#define EM9008_TIMEOUT_ERROR                    -2 //超时错误
#define EM9008_CONNECT_ERROR                    -3 //网络连接错误，发生此错误后应该重新建立连接
#define EM9008_SEND_ERROR                       -4 //发送错误
#define EM9008_CMDVERIFY_ERROR                  -5 //发送命令校验错误
#define EM9008_RECVVERIFY_ERROR                 -6 //接收数据校验错误
#define EM9008_DATANOTREADY                     -7 //数据没有准备好
#define EM9008_PARA_ERROR                       -8 //参数错误
#define EM9008_FREQBASE_ERROR                   -9 //参数错误
#define EM9008_CH_ERROR                        -10 //通道设置错误，一般是由于没有设置使能通道或者全部通道不使能导致。
#define EM9008_DATA_CONNECT_ERROR              -11 //数据连接错误，发生此错误后应该重新建立连接
#define EM9008_INITNET_ERROR                    -100 //初始化网络错误
#define EM9008_SOCKET_ERROR                     -101 //打开网络套接字错误
#define EM9008_FILEPATH_ERROR                   -201 //文件目录错误
#define EM9008_FILEWRITE_ERROR                  -202 //写文件错误
#define EM9008_FILEREAD_ERROR                   -203 //读文件错误
#define EM9008_FILESEEK_ERROR                   -204 //读文件错误
#define EM9008_FILECLOSE_ERROR                  -205 //读文件错误

#endif
