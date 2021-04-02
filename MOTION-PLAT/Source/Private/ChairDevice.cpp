#include "ChairDevice.h"
#include "TISocket.hpp"

ChairDevice::ChairDevice()
{

}

ChairDevice::~ChairDevice()
{

}

const unsigned char* ChairDevice::GetActionCommand()
{
	float fPitch = this->Euler.Pitch;
	float fRoll = this->Euler.Roll;

	ZeroMemory(buffer, sizeof(unsigned char)*32);
	//帧头1、2
	buffer[0] = 0xFB; buffer[1] = 0xFD;
	//一号电缸伸长量，单位位电缸总行程/65535，最大值位65535.
	buffer[2] = 0x00; buffer[3] = 0x00; buffer[4] = 0x00; buffer[5] = 0x00;
	//二号电缸伸长量，单位位电缸总行程/65535，最大值位65535.
	buffer[6] = 0x00; buffer[7] = 0x00; buffer[8] = 0x00; buffer[9] = 0x00;
	//三号电缸伸长量，单位位电缸总行程/65535，最大值位65535.
	buffer[10] = 0x00; buffer[11] = 0x00; buffer[12] = 0x00; buffer[13] = 0x00;
	//四号电缸伸长量，单位位电缸总行程/65535，最大值位65535. Pitch
	buffer[14] = (int)((unsigned char *)(&fPitch))[0]; buffer[15] = (int)((unsigned char*)(&fPitch))[1]; buffer[16] = (int)((unsigned char *)(&fPitch))[2]; buffer[17] = (int)((unsigned char *)(&fPitch))[3];
	//五号电缸伸长量，单位位电缸总行程/65535，最大值位65535. Row
	buffer[18] = ((unsigned char *)(&fRoll))[0]; buffer[19] = ((unsigned char *)(&fRoll))[1]; buffer[20] = ((unsigned char *)(&fRoll))[2]; buffer[21] = ((unsigned char *)(&fRoll))[3];
	//六号电缸伸长量，单位位电缸总行程/65535，最大值位65535.
	buffer[22] = 0x00; buffer[23] = 0x00; buffer[24] = 0x00; buffer[25] = 0x00;
	//保留 速度0x00-0x20
	buffer[26] = bSpeed;
	//软件版本
	buffer[27] = 0x00; buffer[28] = 0x00; buffer[29] = 0x00; buffer[30] = 0x00;
	for (int i = 1; i <= 30; i++)
	{
		//从第二个字节到倒数第二个字节累加和的低 8 位
		buffer[31] += buffer[i];
	}
	return buffer;
}

