#ifndef PCI8407_H
#define PCI8407_H

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
ZT_API unsigned long _stdcall ZT8407_GetLastErr();
//函数名称：ZT8407_GetLastErr
//函数功能：得到当前错误号。如果错误号为0表示无错误
//返回值：错误代码，错误代码含义请看宏定义
ZT_API void _stdcall ZT8407_ClearLastErr();
//函数名称：ZT8407_ClearLastErr
//函数功能：清除错误号。
//          注意：一旦产生错误，为了使函数重新正常执行，必须清除错误号
//返回值：无
ZT_API long _stdcall ZT8407_GetCardCount();
//函数名称：ZT8407_GetCardCount
//函数功能：得到系统中这种型号的板卡的数量。未安装驱动的板卡除外
//          
//入口参数：无
//返回值： >=0  表示系统中已识别的这种型号的板卡的数量。
ZT_API long _stdcall ZT8407_GetOpenedCardCount();
//函数名称：ZT8407_GetOpenedCardCount
//函数功能：得到当前进程中已经打开的这种型号的板卡的数量。
//          
//入口参数：无
//返回值： >=0  表示当前进程中已经打开的这种型号的板卡的数量
ZT_API long _stdcall ZT8407_CloseAllDevice();
//函数名称：ZT8407_CloseAllDevice
//函数功能：关闭当前进程中打开的所有相同型号的设备
//入口参数：
//          无
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8407_GetLastErr 判断出错原因
ZT_API void _stdcall ZT8407_SetBaseNO(unsigned long baseNO);
//函数名称：ZT8407_SetBaseNO
//函数功能：设置板卡索引号(cardNO)，芯片号(chipNO)，口号(portNO)和通道号(chNO)，
//          总之带“NO”后缀的参数是从0开始还是从1开始。默认为从1开始
//          建议不要调用这个函数
//入口参数：
//          baseNO：只能设为0或1
//返回值：无
ZT_API long _stdcall ZT8407_GetBaseNO();
//函数名称：ZT8407_GetBaseNO
//函数功能：返回当前板卡索引号(cardNO)，芯片号(chipNO)，口号(portNO)和通道号(chNO)，
//          总之带“NO”后缀的参数是从0开始还是从1开始。默认为从1开始
//返回值：返回0  表从0开始
//        返回1  表从1开始
ZT_API long _stdcall ZT8407_OpenDevice(unsigned long cardNO);
//函数名称：ZT8407_OpenDevice
//函数功能：打开设备。必须在调用其他函数之前调用。
//          放在程序初始化时调用比较恰当，只调用一次即可
//入口参数：
//          cardNO：板卡号默认从 1 开始
//                    如果系统中只用了一块这种型号的板卡，令这个参数为1就行了
//                    如果系统中用了多块这种型号的板卡，基地址最小的板卡索引号为1，基地址次小的板卡索引号为2，以此类推
//                    注意：用户一般不用关心板卡基地址的具体值
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8407_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8407_CloseDevice(unsigned long cardNO);
//函数名称：ZT8407_CloseDevice
//函数功能：关闭指定的设备
//
//入口参数：
//          cardNO：板卡号默认从 1 开始
//                    如果系统中只用了一块这种型号的板卡，令这个参数为1就行了
//                    如果系统中用了多块这种型号的板卡，基地址最小的板卡索引号为1，基地址次小的板卡索引号为2，以此类推
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8407_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8407_GetBaseAddr(unsigned long cardNO);
//函数名称：ZT8407_GetBaseAddr
//函数功能：得到指定板卡的基地址。
//          在2K或XP下用户不能用得到的基地址直接访问端口
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8407_OpenDevice 函数
//
//返回值：  大于0  表示返回板卡的基地址
//            -1   表失败，应该进一步调用 ZT8407_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8407_GetCardStatus(unsigned long cardNO);
//函数名称：ZT8407_GetCardStatus
//函数功能：返回指定卡的状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8407_OpenDevice 函数
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8407_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8407_WriteD(unsigned long cardNO, 
                                   unsigned long nOffset,
                                   unsigned long dataDoubleWord);
//函数名称：ZT8407_WriteD
//函数功能：以IO方式，对板卡寄存器进行32位写
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8407_OpenDevice 函数
//         nOffset：偏移地址，在硬件说明书上可以查到
//  dataDoubleWord：要写入寄存的值
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8407_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8407_DOBit(unsigned long cardNO, unsigned long chNO, unsigned long nState);
//函数名称：ZT8407_DOBit
//函数功能：指定某通道的开关量输出状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8407_OpenDevice 函数
//            chNO：通道号：1--32
//          nState：指定某通道的开关量输出状态：
//                    nState =    0    ，指定输出低电平
//                    nState = 1(或非0)，指定输出高电平
//返回值：  
//           0      表示成功
//          -1      表示失败，应该进一步调用 ZT8407_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8407_DOAll(unsigned long cardNO, unsigned long nStateAll);
//函数名称：ZT8407_DOAll
//函数功能：指定所有通道的开关量输出状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8407_OpenDevice 函数
//       nStateAll：指定所有通道的开关量输出状态：
//                    nStateAll =      0    ，指定所有通道输出低电平
//                    nStateAll = 0xFFFFFFFF，指定所有通道输出高电平
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用 ZT8407_GetLastErr 函数查找出错原因
ZT_API unsigned long _stdcall ZT8407_GetLastDO(unsigned long cardNO);
//函数名称：ZT8407_GetLastDO
//函数功能：得到所有开关量输出(DO)通道的最后一次开关量输出的值。
//          注意：1. 如果系统重启后，未调用任何开关量输出函数之前，调用此函数会返回0
//                2. 此函数不进行任何开关量输出操作，而只是查询一下上次的值
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8407_OpenDevice 函数
//返回值：
//             0     表示程序最后一次指定所有通道DO输出低电平
//        0xFFFFFFFF 表示程序最后一次指定所有通道DO输出高电平
//            -1   表示调用出错，应该进一步调用 ZT8407_GetLastErr 函数判断出错原因
}
#endif
