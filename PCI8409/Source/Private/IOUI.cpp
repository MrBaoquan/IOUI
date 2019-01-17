/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */

#include "IOUI.h"
#include <bitset>
#include "PCI8409.h"

#pragma comment(lib,"pci8409.lib")

uint8 GetGroupLength(uint8 groupIndex)
{
    uint8 group_length = 16;
    if (groupIndex == 5 || groupIndex == 6)
    {
        group_length = 8;
    }
    return group_length;
}

long SetDeviceDOByGroup(uint8 deviceIndex, uint8 groupIndex, BYTE* InDOStatus)
{
    size_t group_length = GetGroupLength(groupIndex);

    uint8 last_group_index = groupIndex - 1;
    size_t pos = 0;
    if (last_group_index == 5 || last_group_index == 6)
    {
        pos = 8;
    }
    else
    {
        pos = 16;
    }
    if (groupIndex == 1)
    {
        pos = 0;
    }

    static BYTE* lastPos = nullptr;
    if (lastPos == nullptr)
    {
        lastPos = InDOStatus;
    }

    BYTE* currentPos = lastPos + pos;
    lastPos = currentPos;

    if (groupIndex == 10)
    {
        lastPos = nullptr;
    }

    std::bitset<16> bits;

    for (uint8 index = 0;index < group_length;++index)
    {
        bits[index] = currentPos[index] ? true : false;
    }
    return ZT8409_DOAll(deviceIndex, groupIndex, bits.to_ulong()) == 0 ? 1 : 0;

}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    return ZT8409_OpenDevice(deviceIndex) == 0 ? 1 : 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    return ZT8409_CloseDevice(deviceIndex) == 0 ? 1 : 0;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, BYTE* InDOStatus)
{
    for (uint8 index = 1;index <= 10;++index)
    {
        if(!SetDeviceDOByGroup(deviceIndex, index, InDOStatus))
        {
            return 0;
        }
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, BYTE* OutDOStatus)
{
    uint8 accumulator = 0;
    for (uint8 group_index = 1;group_index <= 10;group_index++)
    {
        uint8 group_length = GetGroupLength(group_index);
        long doStatus = ZT8409_GetLastDO(deviceIndex, group_index);
        if (doStatus == -1) { return 0; }
        std::bitset<16> bits(doStatus);
        for (uint8 ch_index = 0;ch_index < group_length;++ch_index)
        {
            OutDOStatus[accumulator++] = bits[ch_index] ? 1 : 0;
        }
    }
    
    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}
