#ifndef PCI8409_H
#define PCI8409_H

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
#define ERR_PARAMETER_BASEADDR 21 //针对ISA卡，板卡基地址超出范围，应该在0x100至0x3F0之间未被系统占用的地址
#define ERR_PARAMETER_CARDNO 22 //针对PCI卡，板卡索引号超出范围，这版驱动中板卡索引号从1开始
#define ERR_PARAMETER_CHMODE 23 //通道方式参数错误，通道方式超出范围，一般只有0--3共4种方式
#define ERR_PARAMETER_CH 24 //通道号参数错误，通道号超出范围
#define ERR_PARAMETER_CHIP_OR_GROUP 25 //芯片号或组号参数错误，芯片号或组号超出范围
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
#define ERR_EEPROM_ID 44L          //EEPROM标识不正确。可能的原因：1. EEPROM未初始化 2. EEPROM不存在；3. EEPROM损坏
#define ERR_EEPROM_VERSION 45L     //未知EEPROM数据格式版本
#define ERR_EEPROM_REC_FORMAT 46L //数据格式不正确
#define ERR_EEPROM_CRC 47L         //CRC检验错
#define ERR_EEPROM_REC_DATA 48L        //记录中包含的数据错，从EEPROM中包含非法数据
#define ERR_EEPROM_REC_NOTFOUND 49L  //未找到指定记录
#define ERR_EEPROM_REC_ADDR 50L    //未找到数据地址错
#define ERR_EEPROM_READ 51L        //EEPROM读操作失败
#define ERR_EEPROM_WRITE 52L        //EEPROM写操作失败
#define ERR_SW_MODE_COLLISION 53L  //开关量操作与先前设置的开关量方式冲突

#endif

extern "C" 
{
ZT_API unsigned long _stdcall ZT8409_GetLastErr();
//函数名称：ZT8409_GetLastErr
//函数功能：得到当前错误号。如果错误号为0表示无错误
//返回值：错误代码，错误代码含义请看宏定义
ZT_API void _stdcall ZT8409_ClearLastErr();
//函数名称：ZT8409_ClearLastErr
//函数功能：清除错误号。
//          注意：一旦产生错误，为了使函数重新正常执行，必须清除错误号
//返回值：无
ZT_API long _stdcall ZT8409_OpenDevice(unsigned long cardNO);
//函数名称：ZT8409_OpenDevice
//函数功能：打开设备。必须在调用其他函数之前调用。
//          放在程序初始化时调用比较恰当，只调用一次即可
//入口参数：
//          cardNO：板卡号默认从 1 开始
//                    如果系统中只用了一块这种型号的板卡，令这个参数为1就行了
//                    如果系统中用了多块这种型号的板卡，基地址最小的板卡索引号为1，基地址次小的板卡索引号为2，以此类推
//                    注意：用户一般不用关心板卡基地址的具体值
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8409_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8409_CloseDevice(unsigned long cardNO);
//函数名称：ZT8409_CloseDevice
//函数功能：关闭指定的设备
//
//入口参数：ZT8409_CloseDevice
//          cardNO：板卡号默认从 1 开始
//                    如果系统中只用了一块这种型号的板卡，令这个参数为1就行了
//                    如果系统中用了多块这种型号的板卡，基地址最小的板卡索引号为1，基地址次小的板卡索引号为2，以此类推
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8409_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8409_GetBaseAddr(unsigned long cardNO);
//函数名称：ZT8409_GetBaseAddr
//函数功能：得到指定板卡的基地址。
//          在2K或XP下用户不能用得到的基地址直接访问端口
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//
//返回值：  大于0  表示返回板卡的基地址
//            -1   表失败，应该进一步调用 ZT8409_GetLastErr 判断出错原因

///////////////////////////////////// 开关量函数 /////////////////////////////////////////

ZT_API long _stdcall ZT8409_SetIOMode(unsigned long cardNO, 
                                      unsigned long groupNO, 
                                      unsigned long nIOMode);
//函数名称：ZT8409_SetIOMode
//函数功能：设置某一组开关量的输入输出方向
//          注意：开关量使用前必须先设置输入输出方向。
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//         nIOMode：设置IO方式，0 = 输入方式，1 = 输出方式
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_GetIOMode(unsigned long cardNO, 
                                      unsigned long groupNO);
//函数名称：ZT8409_GetIOMode
//函数功能：得到某一组开关量的输入输出方向设置
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//返回值：   0  表示这一组当前为输入方式
//           1  表示这一组当前为输出方式
//          -1      表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_SetIOModeAll(unsigned long cardNO, unsigned long nIOMode);
//函数名称：ZT8409_SetIOModeAll
//函数功能：设置所有组开关量的输入输出方向。
//            提示：开关量分为10组
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         nIOMode：设置IO方式，每一组占1个bit
//                    例如：0x0 = 所有组(共10组)都设为输入方式，
//                          0x1 = 第1组设为输出方式，其他组设为输入方式
//                          0x2 = 第2组设为输出方式，其他组设为输入方式
//                          0x3FF = 所有组(共10组)都设为输出方式
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_GetIOModeAll(unsigned long cardNO);
//函数名称：ZT8409_GetIOModeAll
//函数功能：返回所有组开关量的工作方式。
//            提示：开关量分为10组
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//返回值： 大于等于0   所有组开关量的工作方式
//             -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_DIBit(unsigned long cardNO, 
                                  unsigned long groupNO, 
                                  unsigned long chNO);
//函数名称：ZT8409_DIBit
//函数功能：得到某组中某个通道的开关量输入状态
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//            chNO：通道号：当 groupNO = 1,2,3,4,7,8,9,10时，chNO取值范围在 1--16
//                          当 groupNO = 5,6时，chNO取值范围在 1--8
//返回值：
//          0 表示低电平
//          1 表示高电平
//         -1 表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_DIAll(unsigned long cardNO, unsigned long groupNO);
//函数名称：ZT8409_DIAll
//函数功能：得到某组中所有通道的开关量输入状态。
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//返回值：
//         大于等于0  表示这一组所有通道的开关量输入状态
//            -1      表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_DOBit(unsigned long cardNO, 
                                  unsigned long groupNO, 
                                  unsigned long chNO, 
                                  unsigned long nState);
//函数名称：ZT8409_DOBit
//函数功能：指定某组中某个通道的开关量输出状态
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//            chNO：通道号：当 groupNO = 1,2,3,4,7,8,9,10时，chNO取值范围在 1--16
//                          当 groupNO = 5,6时，chNO取值范围在 1--8
//          nState：指定某通道的开关量输出状态：
//                    nState =    0    ，指定输出低电平
//                    nState = 1(或非0)，指定输出高电平
//返回值：  
//           0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_DOAll(unsigned long cardNO, 
                                  unsigned long groupNO, 
                                  unsigned long nStateAll);
//函数名称：ZT8409_DOAll
//函数功能：设定某组的所有通道的开关量输出状态。
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//       nStateAll：指定这组中所有通道的开关量输出状态
//
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_GetLastDO(unsigned long cardNO, unsigned long groupNO);
//函数名称：ZT8409_GetLastDO
//函数功能：得到某组中所有通道最后一次开关量输出的值。
//
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         groupNO：组号(1--10)共10组
//返回值：
//         大于等于0  表示这一组所有通道的开关量输入状态
//            -1      表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_GetGroupChNO(unsigned long logicChNO, 
                                         unsigned long &groupNO, 
                                         unsigned long &chNO);
//函数名称：ZT8409_GetGroupChNO
//函数功能：算出大排序通道号(1--144)属于哪个组的哪个通道
//
//入口参数：
//       logicChNO：通道号：1--144。对所有通道按组进行大排序
//         groupNO：用户传入一个变量，用来接收算出的组号
//            chNO：用户传入一个变量，用来接收算出的通道号
//
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_DIBitEx(unsigned long cardNO, 
                                    unsigned long logicChNO, 
                                    unsigned long byMode);
//函数名称：ZT8409_DIBitEx
//函数功能：为了方便用户操作，按照通道号大排序得到某个通道的开关量输入状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//       logicChNO：通道号：1--144。对所有通道按组进行大排序
//          byMode：是否先判断通道的工作方式，再返回值，0 = 不判断，1 = 判断
//                    说明：当 byMode 设为 1 时，如果用户指定的通道的工作方式为输出时，返回0，
//                                    因为对于设为输出工作方式的通道，去读其输入状态是矛盾的，
//                          当 byMode 设为 0 时，函数把从寄存器读到的值直接返回，不做判断
//返回值：
//          0 表示低电平
//          1 表示高电平
//         -1 表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_DOBitEx(unsigned long cardNO, 
                                    unsigned long logicChNO, 
                                    unsigned long nState, 
                                    unsigned long byMode);
//函数名称：ZT8409_DOBitEx
//函数功能：为了方便用户操作，按照通道号大排序设置某个通道的开关量输出状态
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//       logicChNO：通道号：1--144。对所有通道按组进行大排序
//          nState：指定某通道的开关量输出状态：
//                    nState =    0    ，指定输出低电平
//                    nState = 1(或非0)，指定输出高电平
//          byMode：是否先判断通道的工作方式，再返回值，0 = 不判断，1 = 判断
//                    说明：当 byMode 设为 1 时，如果用户指定的通道的工作方式为输入时，不操作，
//                                    因为对于设为输入工作方式的通道，去设置其输出状态是矛盾的，
//                          当 byMode 设为 0 时，函数直接设置寄存器的值，不做判断
//返回值：  
//           0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API long _stdcall ZT8409_GetChIOMode(unsigned long cardNO, unsigned long logicChNO);
//函数名称：ZT8409_GetChIOMode
//函数功能：为了方便用户操作，按照通道号大排序得到某个通道的工作方式
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//       logicChNO：通道号：1--144。对所有通道按组进行大排序
//返回值：
//          0 表示输入方式
//          1 表示输出方式
//         -1 表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API unsigned long _stdcall ZT8409_GetUInt32Bits(unsigned long value, 
                                                   unsigned long mask,
                                                   unsigned long bit);
//函数名称：ZT8409_GetUInt32Bits
//函数功能：从一个 UInt32 类型的变量得到某个位或某些位的值。
//
//入口参数：
//           value：一个 UInt32 类型的变量
//            mask：掩码
//             bit：从哪位开始0--31
//返回值：
//         大于等于0  表示这一组所有通道的开关量输入状态
//            -1      表示调用出错，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因
ZT_API unsigned long _stdcall ZT8409_SetUInt32Bits(unsigned long value, 
                                                   unsigned long mask,
                                                   unsigned long bit,
                                                   unsigned long state);
//函数名称：ZT8409_SetUInt32Bits
//函数功能：设置一个 UInt32 类型的变量的某个位或某些位的值。
//
//入口参数：
//           value：一个 UInt32 类型的变量
//            mask：掩码
//             bit：从哪位开始0--31
//           state：状态
//返回值：   0      表示成功
//          -1      表示失败，应该进一步调用 ZT8409_GetLastErr 函数查找出错原因

///////////////////////////////////// 必备底层函数 /////////////////////////////////////////

ZT_API void _stdcall ZT8409_SetBaseNO(unsigned long baseNO);
//函数名称：ZT8409_SetBaseNO
//函数功能：设置板卡索引号(cardNO)，芯片号(chipNO)，口号(portNO)和通道号(chNO)，
//          总之带“NO”后缀的参数是从0开始还是从1开始。默认为从1开始
//          建议不要调用这个函数
//入口参数：
//          baseNO：只能设为0或1
//返回值：无
ZT_API long _stdcall ZT8409_GetBaseNO();
//函数名称：ZT8409_GetBaseNO
//函数功能：返回当前板卡索引号(cardNO)，芯片号(chipNO)，口号(portNO)和通道号(chNO)，
//          总之带“NO”后缀的参数是从0开始还是从1开始。默认为从1开始
//返回值：返回0  表从0开始
//        返回1  表从1开始
ZT_API long _stdcall ZT8409_ReadW(unsigned long cardNO,
                                unsigned long nOffset);
//函数名称：ZT8409_ReadW
//函数功能：以IO方式，对板卡寄存器进行16位读
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         nOffset：偏移地址，在硬件说明书上可以查到
//返回值：  返回大于等于0的数，表读出的具体值
//          -1  表失败，应该进一步调用 ZT8409_GetLastErr 判断出错原因
ZT_API long _stdcall ZT8409_WriteW(unsigned long cardNO,
                                   unsigned long nOffset,
                                   unsigned long dataWord);
//函数名称：ZT8409_WriteW
//函数功能：以IO方式，对板卡寄存器进行16位写
//入口参数：
//          cardNO：板卡号默认从 1 开始，有关这个参数的详细说明，请参考 ZT8409_OpenDevice 函数
//         nOffset：偏移地址，在硬件说明书上可以查到
//        dataWord：要写入寄存的值
//返回值：  0  表成功
//         -1  表失败，应该进一步调用 ZT8409_GetLastErr 判断出错原因
}
#endif
