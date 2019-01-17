/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-30 10:55
 *  Email: mrma617@gmail.com
 */

#pragma once
#pragma once
#include <map>
#include <windows.h>

typedef void *HANDLE;
typedef unsigned __int8 uint8;

class PCIManager
{
public:
    static PCIManager& Instance()
    {
        static PCIManager Instance;
        return Instance;
    }

    HANDLE GetHandle(uint8 deviceIndex)
    {
        if (Devices.count(deviceIndex))
        {
            return Devices[deviceIndex];
        }
        return INVALID_HANDLE_VALUE;
    }
    
    void AddHandle(uint8 deviceIndex, HANDLE deviceHandle)
    {
        Devices.try_emplace(deviceIndex, deviceHandle);
    }

private:
    PCIManager() = default;
    ~PCIManager() = default;

private:
    std::map<uint8, HANDLE> Devices;
};