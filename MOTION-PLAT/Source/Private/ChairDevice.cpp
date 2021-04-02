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
	//֡ͷ1��2
	buffer[0] = 0xFB; buffer[1] = 0xFD;
	//һ�ŵ���쳤������λλ������г�/65535�����ֵλ65535.
	buffer[2] = 0x00; buffer[3] = 0x00; buffer[4] = 0x00; buffer[5] = 0x00;
	//���ŵ���쳤������λλ������г�/65535�����ֵλ65535.
	buffer[6] = 0x00; buffer[7] = 0x00; buffer[8] = 0x00; buffer[9] = 0x00;
	//���ŵ���쳤������λλ������г�/65535�����ֵλ65535.
	buffer[10] = 0x00; buffer[11] = 0x00; buffer[12] = 0x00; buffer[13] = 0x00;
	//�ĺŵ���쳤������λλ������г�/65535�����ֵλ65535. Pitch
	buffer[14] = (int)((unsigned char *)(&fPitch))[0]; buffer[15] = (int)((unsigned char*)(&fPitch))[1]; buffer[16] = (int)((unsigned char *)(&fPitch))[2]; buffer[17] = (int)((unsigned char *)(&fPitch))[3];
	//��ŵ���쳤������λλ������г�/65535�����ֵλ65535. Row
	buffer[18] = ((unsigned char *)(&fRoll))[0]; buffer[19] = ((unsigned char *)(&fRoll))[1]; buffer[20] = ((unsigned char *)(&fRoll))[2]; buffer[21] = ((unsigned char *)(&fRoll))[3];
	//���ŵ���쳤������λλ������г�/65535�����ֵλ65535.
	buffer[22] = 0x00; buffer[23] = 0x00; buffer[24] = 0x00; buffer[25] = 0x00;
	//���� �ٶ�0x00-0x20
	buffer[26] = bSpeed;
	//����汾
	buffer[27] = 0x00; buffer[28] = 0x00; buffer[29] = 0x00; buffer[30] = 0x00;
	for (int i = 1; i <= 30; i++)
	{
		//�ӵڶ����ֽڵ������ڶ����ֽ��ۼӺ͵ĵ� 8 λ
		buffer[31] += buffer[i];
	}
	return buffer;
}

