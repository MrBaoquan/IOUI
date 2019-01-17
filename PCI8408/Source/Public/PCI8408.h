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

//定义错误号
#define ZT_SUCCESS 0  //无错误
#define ERR_PARAMETER1 1 //参数1错
#define ERR_PARAMETER2 2 //参数2错
#define ERR_PARAMETER3 3 //参数3错
#define ERR_PARAMETER4 4 //参数4错
#define ERR_PARAMETER5 5 //参数5错
#define ERR_PARAMETER6 6 //参数6错
#define ERR_PARAMETER7 7 //参数7错
#define ERR_PARAMETER8 8 //参数8错
#define ERR_PARAMETER9 9 //参数9错
#define ERR_PARAMETER10 10 //参数10错
#define ERR_PARAMETER11 11 //参数11错
#define ERR_PARAMETER12 12 //参数12错
#define ERR_PARAMETER13 13 //参数13错
#define ERR_PARAMETER14 14 //参数14错
#define ERR_PARAMETER15 15 //参数15错
#define ERR_PARAMETER16 16 //参数16错
#define ERR_PARAMETER17 17 //参数17错
#define ERR_PARAMETER18 18 //参数18错
#define ERR_PARAMETER19 19 //参数19错
#define ERR_PARAMETER20 20 //参数20错
#define ERR_PARAMETER_BASEADDR 21 //针对ISA卡，板卡基地址超出范围，基地址应该为0x100至0x3F0之间未被系统占用的地址
#define ERR_PARAMETER_CARDNO 22 //针对PCI卡，板卡索引号超出范围，这版驱动中板卡索引号默认从1开始
#define ERR_PARAMETER_CHMODE 23 //通道方式参数错误，通道方式超出范围，一般只有0--3共4种方式
#define ERR_PARAMETER_CH 24 //通道号参数错误，通道号超出范围
#define ERR_PARAMETER_CHIP 25 //芯片号参数错误，芯片号超出范围
#define ERR_PARAMETER_PORT 26 //口号参数错误，口号超出范围
#define ERR_PARAMETER_AI_RANGE 27 //AD量程参数错，此卡不支持这种AD输入量程
#define ERR_PARAMETER_AI_AMP 28 //AD增益参数错
#define ERR_PARAMETER_AI_STARTMODE 29 //AD启动方式参数错
#define ERR_PARAMETER_AO_RANGE 30 //DA量程参数错，此卡不支持这种DA输出量程
#define ERR_PARAMETER_ADFREQ 31 //AD采集频率(或AD分频系数)参数错
#define ERR_PARAMETER_IRQ 32 //与中断相关参数错，可能是此卡不支持这种中断方式或中断号
#define ERR_PARAMETER_NULL_POINTER 33 //空指针错。原因：缓冲区首地址为空或者用户传入的参数导致引用到空指针
#define ERR_AD_OVERTIME 34 //AD超时。对于ISA卡，出错原因可能是：未插卡或IO地址与板卡上设置不匹配。当用定时启动AD或外触发启动AD时，一般不应检查AD是否超时
#define ERR_OPEN_DEVICE 35 //打开设备失败，对于PCI卡，出错原因可能是：未插卡或未装驱动，或dll与sys版本不一致
#define ERR_CLOSE_DEVICE 36 //关闭设备失败
#define ERR_TIMING 37 //定时未到或外触发脉冲未到
#define ERR_IOADDR_DA_OVERTIME 38 //带光隔DA写过程超时
#define ERR_OPEN_IRQ 39 //打开中断出错
#define ERR_FUNC_CANNT_RUN 40 //此函数不能在这台计算机上运行
#define ERR_ASYNC_FUNC_FAILED 41 //异步函数调用失败
#define ERR_FUNC_OPERATE 42 //在当前的卡的设置状态下，不应该调用此函数
#define ERR_EXCHANGE_DATA 43 //与底层驱动之间交换数据出错

#endif

extern "C" 
{
ZT_API unsigned long _stdcall ZT8408_GetLastErr();
//函数名称：ZT8408_GetLastErr
//函数功能：得到当前错误号。如果错误号为0表示无错误
//返回值：错误代码，错误代码含义请看宏定义
ZT_API void _stdcall ZT8408_ClearLastErr();
//函数名称：ZT8408_ClearLastErr
//函数功能：清除错误号。
//          注意：一旦产生错误，为了使函数重新正常执行，必须清除错误号
//返回值：无
ZT_API long _stdcall ZT8408_OpenDevice(unsigned long cardNO);
//函数名称：ZT8408_OpenDevice
//函数功能：打开设备。必须在调用其他函数之前调用。
//          放在程序初始化时调用比较恰当，只调用一次即可
//入口参数：
//          cardNO：板卡号默认从 1 开始
//                    如果系统中只用了一块这种型号的板卡，令这个参数为1就行了
//                    如果系统中用了多块这种型号的板卡，基地址最小的板卡索引号为1，基地址次小的板卡索引号为2，以此类推
//                    注意：用户一般不用关心板卡基地址的具体值
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_CloseDevice(unsigned long cardNO);
//函数名称：ZT8408_CloseDevice
//函数功能：关闭指定的设备
//
//入口参数：
//          cardNO：板卡号默认从 1 开始
//                    如果系统中只用了一块这种型号的板卡，令这个参数为1就行了
//                    如果系统中用了多块这种型号的板卡，基地址最小的板卡索引号为1，基地址次小的板卡索引号为2，以此类推
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_GetBaseAddr(unsigned long cardNO);
//函数名称：ZT8408_GetBaseAddr
//函数功能：得到指定板卡的基地址。
//          在2K或XP下用户不能用得到的基地址直接访问端口
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//
//返回值：  大于0  表示返回板卡的基地址
//            -1   表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API unsigned short _stdcall ZT8408_ReadW(unsigned long cardNO,
                                            unsigned long nOffset);
//函数名称：ZT8408_ReadW
//函数功能：以IO方式，对板卡寄存器进行16位读
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//         nOffset：偏移地址，在硬件说明书上可以查到
//返回值：  返回大于等于0的数，表读出的具体值
//          -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_WriteW(unsigned long cardNO,
                                   unsigned long nOffset,
                                   unsigned long dataWord);
//函数名称：
//函数功能：以IO方式，对板卡寄存器进行16位写
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//         nOffset：偏移地址，在硬件说明书上可以查到
//        dataWord：要写入寄存的值
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_ClearIRQ(unsigned long cardNO, unsigned long clearIrqStatusMask);
//函数名称：ZT8408_ClearIRQ
//函数功能：清前8路开关量输入中断状态标志。
//            对于变化频率小于 0.1Hz 的DI通道必须清中断状态标志；
//            对于变化频率大于 0.1Hz 的DI通道不要清中断状态标志。
//入口参数：
//            cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//clearIrqStatusMask：相应的bit置1表示清该通道中断状态标志
//                      对于变化频率小于 0.1Hz 的DI通道必须清中断；
//                      对于变化频率大于 0.1Hz 的DI通道不要清中断。
//                      例如：
//                            0x01 = 表清第1路DI中断状态标志;
//                            0x02 = 表清第2路DI中断状态标志;
//                            0x80 = 表清第8路DI中断状态标志;
//                            0xFF = 表清全部8路DI中断状态标志;
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_OpenIRQ(unsigned long cardNO, long hEventDI);
//函数名称：ZT8408_OpenIRQ
//函数功能：打开8408开关量输入中断。
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//        hEventDI：是“开关量输入中断”事件句柄
//                    由用户建立一个事件，然后把事件句柄传入驱动，当开关量输入中断产生后，
//                    驱动会将这个事件设置为有信号状态，从而唤醒用户建立的用来等待的线程。
//                    如果用户不需要用到多线程，请保持这个参数为0
//返回值：  0  表示成功
//          -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_CloseIRQ(unsigned long cardNO);
//函数名称：ZT8408_CloseIRQ
//函数功能：关闭8408开关量输入中断
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_ReadIRQstatus(unsigned long cardNO,
                                          unsigned long* DIirqStatusMask,
                                          unsigned long* DIvalinIrq);
//函数名称：ZT8408_ReadIRQstatus
//函数功能：读中断时开关量输入(DI)的状态及前8路中断状态标志
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
// DIirqStatusMask：进入中断后，前8路开关量输入通道的中断状态标志，每2个bit表示一个通道的中断状态标志
//                    例如：
//                          0x0001 = 表第1路DI产生中断;
//                          0x0004 = 表第2路DI产生中断;
//                          0x0010 = 表第3路DI产生中断;
//                          0x0040 = 表第4路DI产生中断;
//                          0x0100 = 表第5路DI产生中断;
//                          0x0400 = 表第6路DI产生中断;
//                          0x1000 = 表第7路DI产生中断;
//                          0x4000 = 表第8路DI产生中断;
//      DIvalinIrq：产生中断时刻，所有开关量输入的状态
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_ReadDIirqCnt(unsigned long cardNO);
//函数名称：ZT8408_ReadDIirqCnt
//函数功能：读开关量输入引发的中断次数。
//            注意：只有前8路开关量输入能够产生中断
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//返回值：  大于等于0  表DI中断次数
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_ClearDIirqCnt(unsigned long cardNO);
//函数名称：ZT8408_ClearDIirqCnt
//函数功能：将开关量输入引发的中断次数清零。
//            注意：只有前8路开关量输入能够产生中断
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8408_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8408_DIBit(unsigned long cardNO, unsigned long chNO);
//函数名称：ZT8408_DIBit
//函数功能：得到指定通道的开关量输入状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//            chNO：通道号：1--16
//返回值：
//          0 表示低电平
//          1 表示高电平
//         -1 表示调用出错，应该进一步调用ZT8408_GetLastErr函数查找出错原因
ZT_API long _stdcall ZT8408_DIAll(unsigned long cardNO);
//函数名称：ZT8408_DIAll
//函数功能：得到所有通道的开关量输入状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//返回值：
//             0   表示所有通道的开关量输入状态为低电平
//          0xFFFF 表示所有通道的开关量输入状态为高电平
//            -1   表示调用出错，应该进一步调用ZT8408_GetLastErr函数查找出错原因
ZT_API long _stdcall ZT8408_DOBit(unsigned long cardNO, unsigned long chNO, unsigned long nState);
//函数名称：ZT8408_DOBit
//函数功能：指定某通道的开关量输出状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//            chNO：通道号：1--16
//          nState：指定某通道的开关量输出状态：
//                    nState =    0    ，指定输出低电平
//                    nState = 1(或非0)，指定输出高电平
//返回值：  
//           0      表示成功
//          -1      表示失败，应该进一步调用ZT8408_GetLastErr函数查找出错原因
ZT_API long _stdcall ZT8408_DOAll(unsigned long cardNO, unsigned long nStateAll);
//函数名称：ZT8408_DOAll
//函数功能：指定所有通道的开关量输出状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//       nStateAll：指定所有通道的开关量输出状态：
//                    nStateAll =   0   ，指定所有通道输出低电平
//                    nStateAll = 0xFFFF，指定所有通道输出高电平
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用ZT8408_GetLastErr函数查找出错原因
ZT_API long _stdcall ZT8408_GetLastDO(unsigned long cardNO);
//函数名称：ZT8408_GetLastDO
//函数功能：得到所有开关量输出(DO)通道的最后一次开关量输出的值。
//          注意：1. 如果系统重启后，未调用任何开关量输出函数之前，调用此函数会返回0
//                2. 此函数不进行任何开关量输出操作，而只是查询一下上次的值
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8408_OpenDevice 函数
//返回值：
//           0   表示程序最后一次指定所有通道DO输出低电平
//        0xFFFF 表示所有通道的开关量输入状态为高电平
//                 注意：对于有符号32位整数，0xFFFFFFFF = -1，当返回这个值时，
//                       应该进一步调用 ZT8408_GetLastErr 函数判断是否产生了一个错误
}
#endif
