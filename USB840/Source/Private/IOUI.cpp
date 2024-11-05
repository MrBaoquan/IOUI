/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include "IOUI.h"
#include "lusb0_usb.h"
#include "Paths.hpp"
#include "Debug.hpp"

#pragma comment(lib,"libusb.lib")

std::vector<usb_dev_handle*> udev_vec;
std::vector<bool> deviceOpend;
static bool isCreateDevice = false;
struct usb_bus* busses;
struct usb_bus* bus;
struct usb_device* dev;
int bus_num;
int device_num = 0;

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 16;
	devInfo.OutputCount = 16;
	devInfo.AxisCount = 0;
    return &devInfo;
}

void CreateDevice()
{
	if (isCreateDevice) return;

	usb_init();
	bus_num = usb_find_busses();
	device_num = usb_find_devices();
	busses = usb_get_busses(); 

	udev_vec.resize(device_num);
	deviceOpend.resize(device_num, false);

	int deviceID = 0;
	for (bus = busses; bus; bus = bus->next)
	{
		//struct usb_device *dev;    
		for (dev = bus->devices; dev; dev = dev->next)
		{
			struct usb_device_descriptor* desc = &(dev->descriptor);

			if ((desc->idVendor == 0x0471) && (desc->idProduct == 0x2378))
			{
				udev_vec[deviceID] = usb_open(dev);
				deviceID++;
			}
		}
	}

	if (udev_vec.size() == 0)
		isCreateDevice = false;
	else
		isCreateDevice = true;

}

usb_dev_handle* udev;
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
#ifdef WIN_64
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\libusb0.dll";
#else
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\libusb0_x86.dll";
#endif
	auto _module = LoadLibraryA(_path.data());
	int _err = GetLastError();

	CreateDevice();
	
	if (udev_vec.size() == 0 || deviceIndex >= udev_vec.size())
    {
        return 0;
    }

	EnableConsoleDebug();
	if (deviceOpend[deviceIndex]) return 1;


    if (udev_vec[deviceIndex] != NULL)
    {
        usb_set_configuration(udev_vec[deviceIndex], 1);
        usb_claim_interface(udev_vec[deviceIndex], 0);
		deviceOpend[deviceIndex] = true;
        return 1;
    }
    return 0;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	if(!deviceOpend[deviceIndex]) return 0;
	DisableConsoleDebug();
	char write_buff_out[4];

	write_buff_out[0] = 0x10;
    write_buff_out[1] = 0x4;
	
	write_buff_out[2] = 1;
	write_buff_out[3] = 0x00;
	usb_bulk_write(udev_vec[deviceIndex], 1, write_buff_out, 4, 500);
	Sleep(10);

	write_buff_out[2] = 2;
	write_buff_out[3] = 0x00;
	usb_bulk_write(udev_vec[deviceIndex], 1, write_buff_out, 4, 500);
	Sleep(10);

	usb_release_interface(udev_vec[deviceIndex], 0);
	usb_close(udev_vec[deviceIndex]);
	deviceOpend[deviceIndex] = false;

    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	if(!deviceOpend[deviceIndex]) return 0;
  	char write_buff_out[4];
	write_buff_out[0] = 0x10;
    write_buff_out[1] = 0x4;


	unsigned char do0 = 0x00;
	for (int i = 0; i < 8; i++)
	{
		short _bitVal = InDOStatus[i];
		if (_bitVal > 0){
			do0 |= (1 << i);
		}
	}
	write_buff_out[2] = 1;
	write_buff_out[3] = do0;
	usb_bulk_write(udev_vec[deviceIndex], 1, write_buff_out, 4, 500);
	Sleep(10);

	unsigned char do1 = 0x00;
	for(int i=8; i<16; i++){
		short _bitVal = InDOStatus[i];
		if(_bitVal>0){
			do1 |= (1 << (i-8));
		}
	}
	write_buff_out[2] = 2;
	write_buff_out[3] = do1;
	usb_bulk_write(udev_vec[deviceIndex], 1, write_buff_out, 4, 500);
	Sleep(10);

    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	if(!deviceOpend[deviceIndex]) return 0;

	char write_buff_in[2];
	char read_buff[4];

	write_buff_in[0] = 0x2;
	write_buff_in[1] = 0x2;

	usb_bulk_write(udev_vec[deviceIndex], 1, write_buff_in, 2, 500);
	Sleep(10);
	usb_bulk_read(udev_vec[deviceIndex], 0x81, read_buff, 4, 500);
	Sleep(10);

	unsigned char din0 = unsigned char(read_buff[2]);
	unsigned char din1 = unsigned char(read_buff[3]);
	unsigned short data = 0;

	data |= din1 & 0x00ff;
	data = data << 8;
	data |= din0 & 0x00ff;

	for (int i = 0; i < 16; i++)
	{
		OutDIStatus[i] = data >> i & 1;
	}
	ClearConsole();
	PrintDIData(OutDIStatus, 16);
	return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	return 0;
}
