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
#define EM9008_BASE_FREQ                            120000000.0//基础时钟，120MHz
#define EM9008_MAXADCHCNT                           8 //AD最大通道数
#define EM9008_MAXDICHCNT                           8 //DI最大通道数
#define EM9008_MAXDOCHCNT                           8 //DO最大通道数
#define EM9008_MAXCTCHCNT                           6 //计数器最大通道数
#define EM9008_MAXECCHCNT                           1 //编码器最大通道数
#define EM9008_MAXPWMCHCNT                          7 //PWM最大通道数

//AD基础频率设置
#define EM9008_AD_BASE_FREQ_1000K                          0
#define EM9008_AD_BASE_FREQ_571K                           1
#define EM9008_AD_BASE_FREQ_211K                           2
#define EM9008_AD_BASE_FREQ_114K                           3
#define EM9008_AD_BASE_FREQ_60K                            4

//AD采集频率设置
#define EM9008_AD_FREQ_7812                          0
#define EM9008_AD_FREQ_3906                          1
#define EM9008_AD_FREQ_1953                          2
#define EM9008_AD_FREQ_977                           3
#define EM9008_AD_FREQ_488                           4
#define EM9008_AD_FREQ_244                           5
#define EM9008_AD_FREQ_122                           6
#define EM9008_AD_FREQ_61                            7

//AD采集范围
#define EM9008_AD_RANGE_N10_10V                      0L //±10V
#define	EM9008_AD_RANGE_N5_5V                        1L //±5V（硬件暂时不支持）
#define	EM9008_AD_RANGE_0_10V                        2L //0～10V
#define	EM9008_AD_RANGE_0_5V                         3L //0～5V
#define	EM9008_AD_RANGE_0_20mA                       4L //0～20mA

//计数方式
#define EM9008_CT_MODE_COUNT                         0 //计数
#define EM9008_CT_MODE_LFREQ                         1 //测频率

//DI功能
#define EM9008_DI_FUN_DI                            0 //DI
#define EM9008_DI_FUN_CT                            1 //计数
#define EM9008_DI_FUN_ECA                           1 //编码器A
#define EM9008_DI_FUN_ECB                           1 //编码器B
#define EM9008_DI_FUN_ECZ                           3 //编码器Z

//DO输出管脚功能
#define EM9008_DO_FUN_DO                             0 //表示DO功能
#define EM9008_DO_FUN_PWM                            1 //表示输出PWM信号

#define EM9008_SFIFO_SIZE                            16 * 1024 * 1024// 缓冲区大小为16MB

extern "C"
{
HANDLE _stdcall EM9008_DeviceCreate();
//函数功能：创建设备
//入口参数：
//      无
//返回值：0，创建设备失败
//        其它值为设备句柄，在以后的设备相关操作中均要使用其返回值

void _stdcall EM9008_DeviceClose( HANDLE hDevice );
//函数功能：关闭设备，同时释放设备所占用资源
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//返回值：无

I32 _stdcall EM9008_CmdConnect( HANDLE hDevice, char* strIP, I32 ipBC, I32 cmdPort, I32 dataPort, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：连接命令端口
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//             strIP：设备IP
//              ipBC：strIP的长度，此参数为0时表示strIP是以0为结束的字符串。
//           cmdPort：命令端口号。
//          dataPort：数据端口号。
//         timeOutMS：超时毫秒数。
//返回值：0表示成功，<0表示失败

void _stdcall EM9008_CmdClose( HANDLE hDevice );
//函数功能：关闭命令端口
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//返回值：无

I32 _stdcall EM9008_DataConnect( HANDLE hDevice, int port, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：连接数据端口
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              port：数据端口号
//返回值：>=0表示成功，<0表示失败

void _stdcall EM9008_DataClose( HANDLE hDevice, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：关闭数据端口
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//返回值：无

/////////////////AD相关函数//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_AdSetRange( HANDLE hDevice, I32 chNo, I32 rangeInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置AD采集范围，注意要与设备跳线一致
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：通道号1～8
//          rangeInx：采集范围
//                    0，±10V
//                    1，±5V（硬件暂时不支持）
//                    2，0~10V
//                    3，0~5V
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_AdSetBeginEndCh( HANDLE hDevice, I32 beginCh, I32 endCh, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置AD通道数据是否进入高速缓冲区，只有指定的通道才能够进入缓冲区
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//           beginCh：起始通道，1～endCh
//             endCh：终止通道，beginCh～8
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_AdCodeToValue( HANDLE hDevice, I32 chNo, U16* adCode, I32 codeCount, F64* adValue );
//函数功能：将AD原码值转换成物理值
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：通道号，1~8
//            adCode：原码值
//         codeCount：pCode数组所包含元素个数
//出口参数：
//           adValue：AD物理值转换结果，电压值，需要用户分配空间，其元素个数为codeCount个
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_AdReadAllOnce( HANDLE hDevice, U16 adCode[EM9008_MAXADCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：读取所有一次原码值，此函数返回最近一次AD的采集结果
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//            adCode：32位整型数组，8元素，对应AD1～AD8
//返回值：0表示成功，<0表示失败

/////////////////硬件控制相关函数//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_HcSetTranBytesCount( HANDLE hDevice, I32 bytesCount, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置下位机每次传给上位机的数据字节个数，此函数一般情况下无需调用
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//        bytesCount：字节个数，4～65535。
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_HcStart( HANDLE hDevice, I32 isHcStart, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：启动硬件控制采集
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//         isHcStart：是否启动，0表示停止采集，1表示启动采集
//出口参数：
//                无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_HcSetBaseFreq( HANDLE hDevice,  I32 freqInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置AD基础采集频率
//    注意：设备最终实际的采集频率和硬件滤波次数、软件滤波次数有关系，在相关参数设置完毕后，可以调用EM9008_HcGetChFreq得到每个通道的采集频率
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//            freqInx：采集频率索引具体含义请参考“AD基础频率设置”宏定义
//出口参数：
//                无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_HcSetFreq( HANDLE hDevice,  I32 freqInx, F64* realFreq, I32 timeOutMS  = EM9008_TIMOUT_MS );
//函数功能：设置AD采集频率
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//            freqInx：只能设置0～4，采集频率索引具体含义请参考“AD采集频率设置”宏定义
//出口参数：
//           realFreq：返回真正的采集频率。
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_AdSetHardFilterInx( HANDLE hDevice,  I32 hardFilterInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置硬件滤波系数
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//     hardFilterInx：0：不滤波
//                    1：2次
//                    2：4次
//                    3：8次
//                    4：16次
//                    5：32次
//                    6：64次
//出口参数：
//                无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_AdSetSoftFilter( HANDLE hDevice,  I32 softFilterTimes, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置软件滤波系数
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//   softFilterTimes：只能设置成1～255。
//出口参数：
//                无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_HcGetChFreq( HANDLE hDevice,  F64* chFreq, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：获取每通道数据采集频率
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//            chFreq：每通道采集频率，根据
//                    EM9008_HcSetBaseFreq，EM9008_AdSetHardFilterInx，EM9008_AdSetSoftFilter，EM9008_AdSetBeginEndCh
//                    几个函数的设置计算得到，因此必须先调用上面几个函数，才调用此函数才能够得到正确的频率值。
//                无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_HcGetGroupBc( HANDLE hDevice );
//函数功能：得到每组字节数，也就是包括了每采集一次数据，进入缓冲区的所有字节数。
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//返回值：>0表示字节数

I32 _stdcall EM9008_HcSFifoCanReadCount( HANDLE hDevice, I32* canReadCount );
//函数功能：得到上位机缓冲区可读数据个数
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//      canReadCount：当前可读数据个数
//返回值：>0表示字节数

I32 _stdcall EM9008_HcSFifoRead( HANDLE hDevice, I32 dataCount, U16* dataBuffer, I32* realReadCount );
//函数功能：读出上位机缓冲区中数据
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//         dataCount：要读取的数据个数，请设置为通道整数倍，便于数据处理
//出口参数：
//        dataBuffer：原码数据数组，其大小为dataCount
//     realReadCount：实际读取数据个数
//返回值：>0表示字节数

/////////////////开关量相关函数//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_IoReadAllOnce( HANDLE hDevice, I8 iStatus[EM9008_MAXDICHCNT], I8 oStatus[EM9008_MAXDOCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：得到所有开关量输入最近一次输入
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//           iStatus：数组，iStatus[0]~iStatus[7]对应DI1~DI8
//           oStatus：数组，oStatus[0]~oStatus[7]对应DO1~DO8
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_DoWriteAllOnce( HANDLE hDevice, I8 oStatus[EM9008_MAXDOCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置所有开关量输出
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//           oStatus：数组，oStatus[0]~oStatus[7]对应DO1~DO8
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_DoWriteInitStatus( HANDLE hDevice, I8 intiStatus[EM9008_MAXDOCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置所有开关量输出上电初始状态
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//        intiStatus：数组，oStatus[0]~oStatus[7]对应DO1~DO8
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_DoSetMode( HANDLE hDevice, I32 chNo, I32 modeInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置开关量输出功能
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：1～7
//           modeInx：0表示为开关量，1表示为PWM
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_DiSetMode( HANDLE hDevice, I32 chNo, I32 modeInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置开关量输入功能
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：通道号，1～8
//           modeInx：0表示开关量，其他值不同值时其含义和通道相关。
//                   chNo=1时，modeInx=1表示编码器A
//                   chNo=2时，modeInx=1表示编码器B
//                   chNo=3时，modeInx=1表示计数器1计数，modeInx=2表示计数器1测频，modeInx=3表示编码器过零信号
//                   chNo=4时，modeInx=1表示计数器2计数，modeInx=2表示计数器2测频，modeInx=3表示PWM失效信号1
//                   chNo=5时，modeInx=1表示计数器3计数，modeInx=2表示计数器3测频，modeInx=3表示PWM失效信号2
//                   chNo=6时，modeInx=1表示计数器4计数，modeInx=2表示计数器4测频，modeInx=3表示PWM失效信号3
//                   chNo=7时，modeInx=1表示计数器5计数，modeInx=2表示计数器5测频
//                   chNo=8时，modeInx=1表示计数器6计数，modeInx=2表示计数器6测频
//返回值：0表示成功，<0表示失败

/////////////////计数器相关函数//////////////////////////////////////////////////////////////////////////////////////////////////
//注意此处函数均需要将至少一路DI功能设置为计数器后才可以正确执行
I32 _stdcall EM9008_CtSetFreqBase( HANDLE hDevice, F64 freqBase_ms, F64* real_ms, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置所有通道的计数器测频基准脉冲周期，以毫秒为单位
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//       freqBase_ms：测频基准周期，以毫秒为单位
//出口参数：
//           real_ms：实际测频基准，大多数情况和freqBase_ms一致
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_CtReadAllOnce( HANDLE hDevice, U32 ctCode[EM9008_MAXCTCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：读取所有路计数器的原码值
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//            ctCode：计数器原码值，如果是计数方式就是计数值，如果是测频则需要计算才能转化成频率值
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_CtCodeToValue( HANDLE hDevice, I32 chNo, U32* ctCode, I32 codeCount, F64* ctValue );
//函数功能：将原码值转换成对应的物理值
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：通道号
//            ctCode：计数器原码值，如果是计数方式就是计数值，如果是测频则需要计算才能转化成频率值
//         codeCount：原码值个数，ctCode和ctValue的大小均与此参数有关
//出口参数：
//           ctValue：计数器物理值，如果是计数方式就是计数值，如果是测频则是测频值
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_CtClearAll( HANDLE hDevice, I8 clearBit[EM9008_MAXCTCHCNT] , I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：指定通道计数器清零，本函数只有在计数方式下才有意义，因此执行后会将相应通道设置为计数工作方式	
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//           isClear：6元素数组，每一个元素对应一路计数器，1表示对应通道清零,0表示对应通道不清零
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_CtSetMode( HANDLE hDevice, I32 chNo, I8 ctMode, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：指定通道计数器工作方式	
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：通道号
//            ctMode：计数方式，0，计数，1，测频
//返回值：0表示成功，<0表示失败

/////////////////编码器相关函数//////////////////////////////////////////////////////////////////////////////////////////////////
I32 _stdcall EM9008_EcReadAllOnce( HANDLE hDevice, I32* abCode, F64* freq, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：读取编码器返回值
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//            abCode：编码器AB值，32位有符号指针，需要用户分配空间。
//             freq：编码器频率，64位浮点型指针，需要用户分配空间，如果指针值为0，则表示不返回频率。
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_EcSetMaxPos( HANDLE hDevice, U32 maxPosition, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置编码器最大位置
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//       maxPosition：编码器最大位置，当编码器计数值超过此值后将从0开始计数
//出口参数：
//           无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_EcSetVelocityInx( HANDLE hDevice, I32 velocityInx, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置编码器测速参数
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//       velocityInx：测速参数。
//出口参数：
//       无
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_EcSetMode( HANDLE hDevice, I32 ecEn, I32 ecMode, I32 ecFilter, I32 ecZeroInxEnable, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置编码器工作模式
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              ecEn：使能位，请设为1
//            ecMode：工作模式，0表示正交信号输入，1表示时钟方向信号输入
//          ecFilter：滤波系数，0～15，数字越大，滤波效果越好。
//   ecZeroInxEnable：过零信号使能，0禁止过零信号，1使能过零信号。使能过零信号时，检测到过零信号编码器计数将清零
//出口参数：
//       maxPosition：编码器最大位置，当编码器计数值超过此值后将从0开始计数
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_EcClear( HANDLE hDevice, I32 chNo, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：编码器计数值清零
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//              chNo：指定通道号的编码器清零，本设备只有一个通道，因此只能为1
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_SetWtdStatus( HANDLE hDevice, I8 enableWtd, U16 s, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设置看门狗
//入口参数：
//           hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//         enableWtd：1，使能看门狗；0，禁止看门狗
//                 s：超时秒数，1～30，如果在指定时间内没有向下位机发送任何指令，则下位机将会复位
//返回值：0表示成功，<0表示失败

/////////////////脉宽调制相关函数//////////////////////////////////////////////////////////////////////////////////////////////////
//注意此处函数均需要将至少一路DO功能设置为PWM后才可以正确执行
I32 _stdcall EM9008_PwmSetBaseClkDiv( HANDLE hDevice, I8 baseClkDiv, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设定PWM输出基准时钟
//入口参数：
//                hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//             baseClkDiv：基准时钟分频系数，0～7
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_PwmGetBaseClkDiv( HANDLE hDevice, I8* baseClkDiv, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：得到PWM输出基准时钟
//入口参数：
//                hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//             baseClkDiv：基准时钟分频系数，0～7
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_PwmGetFreqRange( HANDLE hDevice, F64* minHz, F64* maxHz, I8* baseClkDiv, I32* sysClkFreq, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：得到PWM输出频率范围，频率范围和EM9008_PwmSetBaseClkDiv设置的基准时钟系数有关系
//入口参数：
//                hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//                  minHz：最小可设置频率
//                  maxHz：最大可设置频率，注意频率越接近最大可设置频率，占空比可调节范围越小，
//                         在最大可设置频率时，占空比只能为50%，
//                         另外受硬件所限，实际输出无法达到最大可设置频率，具体请参考硬件手册。
//             baseClkDiv：基准时钟分频系数
//             sysClkFreq：系统时钟频率，一般为120,000,000Hz
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_PwmSetPulse( HANDLE hDevice, I32 chNo, F64 freq, F64 duty, F64* realFreq, F64* realDuty, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设定指定通道的PWM输出各项参数
//          注意：7路PWM分为四组，（1，2）（3，4）（5，6）（7），每组只能设置相同的输出频率，因此通道2，4，6频率设置无效，只能设置占空比
//入口参数：
//                hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//                   chNo：通道号，1～7
//                   freq：输出频率。其范围请在调用EM9008_PwmSetBaseClkDiv后调用EM9008_PwmGetFreqRange函数获得
//              dutyCycle：占空比。0~1，具体调节档位取决于输出频率，输出频率越低，可调节的档位越多。
//出口参数：
//               realFreq：真实频率值，此参数无效，请调用EM9008_PwmGetPulse获得
//          realDutyCycle：真实占空比值，此参数无效，请调用EM9008_PwmGetPulse获得
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_PwmGetPulse( HANDLE hDevice, I32 chNo, F64 freq, F64 duty, F64* realFreq, F64* realDuty, I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：根据给定频率和占空比计算出实际频率和占空比。
//入口参数：
//                hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//                   chNo：通道号，1～7
//                   freq：输出频率。
//              dutyCycle：占空比。0~1，具体调节档位取决于输出频率，输出频率越低，可调节的档位越多。
//出口参数：
//               realFreq：真实频率值，由于数字量化误差，设定频率和真实频率之间会有些误差
//          realDutyCycle：真实占空比值，由于数字量化误差，设定占空比和真实占空比之间会有些误差
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_PwmStartAll( HANDLE hDevice, I8 startOrStop[EM9008_MAXPWMCHCNT], I32 timeOutMS = EM9008_TIMOUT_MS );
//函数功能：设定所有通道的PWM输出开始或者停止
//入口参数：
//                hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//            startOrStop：启动设置数组，数组元素0~7对应PWM1~PWM7，以PWM1为例：
//                         startOrStop[0]=0，停止输出
//                         startOrStop[0]=1，启动输出
//返回值：0表示成功，<0表示失败

I32 _stdcall EM9008_SysSetBaseClkDiv( HANDLE hDevice, I8 baseClkDiv, I32 timeOutMS = EM9008_TIMOUT_MS );
//此函数主要用于内部生产，用户无需使用
I32 _stdcall EM9008_SysGetSysClkFreq( HANDLE hDevice, I32* sysClkFreq, I32 timeOutMS = EM9008_TIMOUT_MS );
//此函数主要用于内部生产，用户无需使用

/////////////////////下列函数为历史数据文件相关操作////////////////////////////////////////////////////////
I32 _stdcall EM9008_DFWOpen( HANDLE hDevice, char* dirPath, I32 pathLen );
//函数功能：初始化历史数据写入操作函数。
//入口参数：
//        hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//        dirPath：存放历史文件的路径，其最大长度不能超过256
//        pathLen：路径名长度，如果为0表示disPath是以0结尾的字符串。
//                程序将会在指定目录下自动建立“yy-mm-dd-hh-MM-ss-1.dat”文件，依次是年月日时分秒,-1表示第一个文件，每个文件512MB左右。超过512MB的文件将会顺次增加-2，-3……
//出口参数：
//                无
//返回值：0表示没有错误

I32 _stdcall EM9008_DFWClose( HANDLE hDevice );
//函数功能：关闭历史数据文件
//入口参数：
//          hDevice：设备句柄，EM9008_DeviceCreate函数返回值
//出口参数：
//                无
//返回值：0表示没有错误

I32 _stdcall EM9008_DFROpen( HANDLE hDevice, char* filePathName, I32 pathLen );
//函数功能：初始化历史数据读取操作函数
//入口参数：
//        filePathName：包含路径名的历史文件名，注意其长度不能超过256个字符
//             pathLen：前一个字符串的长度。如果为0，则默认filePathName是以0为结尾的C类型字符串。
//出口参数：
//                无
//返回值：非0值表示文件句柄
//        0表示打开文件失败。
//        注意读历史数据时不能够同时连接设备，因此如果先调用了EM9008_CmdConnect并且成功返回，则此函数会返回失败

I32 _stdcall EM9008_DFRClose( HANDLE hDevice );
//函数功能：关闭历史读文件
//入口参数：
//           hDevice：历史文件句柄
//出口参数：
//                无
//返回值：无

I32 _stdcall EM9008_DFRRead( HANDLE hDevice, F64 beginGroupInx, I32 groupCount, I32* realReadCount );
//函数功能：读取历史文件并且根据读取组数将数据按照通道放到缓冲区中
//入口参数：
//              hDevice：历史文件句柄
//        groupCount：要读取的组数，注意这里一组数据是每个使能通道都读回一次。
//     beginGroupInx：读取开始的位置。如果为-1，则表示从上一次读取的结束开始读取。
//出口参数：
//     realReadCount：实际读取的每通道数据个数。
//返回值：0值成功
//        <0表示失败，具体请参考EM9008_Error.H。

I32 _stdcall EM9008_DFRGetFreq( HANDLE hDevice, F64* daqFreq );
//函数功能：得到历史文件的采集频率
//入口参数：
//              hDevice：历史文件句柄
//出口参数：
//      daqFreq：采集频率
//返回值：0值成功
//        <0表示失败，具体请参考EM9008_Error.H。

I32 _stdcall EM9008_DFRGetGroupTotal( HANDLE hDevice, F64* groupTotal, F64* timeTotal );
//函数功能：得到历史文件所包含的数据总组数，和总时长
//入口参数：
//              hDevice：历史文件句柄
//出口参数：
//        groupTotal：总组数
//        timeTotal：总时长
//返回值：0值成功
//        <0表示失败，具体请参考EM9008_Error.H。

I32 _stdcall EM9008_DFRGetAdChRange( HANDLE hDevice, I32* adBeginCh, I32* adEndCh );
//函数功能：得到历史文件所包含的使能AD通道
//入口参数：
//              hDevice：历史文件句柄
//出口参数：
//            adBeginCh：起始通道
//              adEndCh：终止通道
//返回值：0值成功
//        <0表示失败，具体请参考EM9008_Error.H。

I32 _stdcall EM9008_DFRGetRange( HANDLE hDevice, I8 rangeInx[EM9008_MAXADCHCNT] );
//函数功能：从历史数据文件中得到每个通道的采集范围
//入口参数：
//              hDevice：历史文件句柄
//出口参数：
//           typeInx：8元素数组，每个通道的范围
//返回值：=0,表示成功

/////////////其它用户一般用不到的函数
I32 _stdcall EM9008_AdWriteZero( HANDLE hDevice, I32 chNo, U16 zeroCode, I32 timeOutMS = EM9008_TIMOUT_MS );
//校准相关函数
I32 _stdcall EM9008_AdWriteFull( HANDLE hDevice, I32 chNo, U16 fullCode, I32 timeOutMS = EM9008_TIMOUT_MS );
//校准相关函数
I32 _stdcall EM9008_AdReadZeroFull( HANDLE hDevice, U16* zeroCode, U16* fullCode, I32 timeOutMS = EM9008_TIMOUT_MS );
//校准相关函数
I32 _stdcall EM9008_HcHFifoStatus( HANDLE hDevice, I32* sFifoStatus, I32 timeOutMS = EM9008_TIMOUT_MS );
//下位机软件缓冲区状态
I32 _stdcall EM9008_ReadCoreTemp( HANDLE hDevice, F64* coreTemperature, I32 timeOutMS = EM9008_TIMOUT_MS );
//读取下位机核心温度


}

#endif // !defined(AFX_EM9008_Server_H__D1413467_66A2_475B_8398_F579D95B3FB0__INCLUDED_)
