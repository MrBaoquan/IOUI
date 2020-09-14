#pragma once

#include <windows.h>

//�첽��д����
class Comm
{
public:
	Comm(void);
	~Comm(void);

public:
	void                 Init(int nComID/*���ں�*/, DWORD BaudRate = 9600/*������*/, BYTE ByteSize = 8/*У��λ*/,
		                       BYTE Parity = NOPARITY/*����λ*/ , BYTE StopBits = ONESTOPBIT/*ֹͣλ*/);
	BOOL                 WriteCom(char * lpOutBuffer, DWORD dwBytesWrite/*�����ֽڳ���*/);
	BOOL                 ReadCom(char * buf, DWORD len, LPDWORD outCount);

private:
	HANDLE               m_hCom;//���ھ��
	int                  m_nComID;//���ں�

private:
	//ʮ������תASCII��
	int                  Char2ToChar(char * str, char * result, int len/*Ҫת���ֽڵĳ��ȣ�һ��ΪĿ���ֽڵ�2��*/);
	char                 ConvertHexChar(char ch) ;
	//ASCII��תʮ������
	void                 ASCIIToChar(char * Str, char * Dst);
};
