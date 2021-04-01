/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-506 10:44
 *  Email: mrma617@gmail.com
 */

#pragma once

#ifdef IOUI_EXPORTS
#define IOUI_API __declspec(dllexport)
#else
#define IOUI_API __declspec(dllimport)
#endif

typedef unsigned __int8 uint8;
typedef unsigned   char BYTE;

struct IOUI_API DeviceInfo
{
    /** 输入通道数量 */
    BYTE InputCount = 16;
    /** 输出通道数量 */
    BYTE OutputCount = 16;
    /** 模拟量通道数量 */
    BYTE AxisCount = 0;
};

extern "C"
{
    IOUI_API DeviceInfo* __stdcall Initialize();

    /**
     * 打开 External 设备
     * @param deviceIndex : 设备索引
     * @return 成功返回1 否则返回 0
     */
    IOUI_API int __stdcall OpenDevice(uint8 deviceIndex);
    
    /**
    * 关闭 External 设备
    * @param deviceIndex : 设备索引
    * @return 成功返回1 否则返回 0
    */
    IOUI_API int __stdcall CloseDevice(uint8 deviceIndex);

    /**
    * 设置 External 设备输出状态
    * @param deviceIndex : 设备索引
    * @param InDOStatus : 输入开关量状态 BYTE[32]
    * @return 成功返回1 否则返回 0
    */
    IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus);
    
    /**
    * 获取 External 设备输出状态
    * @param deviceIndex : 设备索引
    * @param InDOStatus : 输出开关量状态 BYTE[32]
    * @return 成功返回1 否则返回 0
    */
    IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus);

    /**
    * 获取 External 设备输入状态
    * @param deviceIndex : 设备索引
    * @param InDOStatus : 输入开关量状态 BYTE[32]
    * @return 成功返回1 否则返回 0
    */
    IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus);

    /**
    * 设置 External 设备输出状态
    * @param deviceIndex : 设备索引
    * @param OutADStatus : 输出模拟量状态 short[32] 取值范围限制在[-1000,1000]
    * @return 成功返回1 否则返回 0
    */
    IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus);
};