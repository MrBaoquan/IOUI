#pragma once

#include <windows.h>

//异步读写串口
class Comm
{
public:
	Comm(void);
	~Comm(void);

public:
	void                 Init(int nComID/*串口号*/, DWORD BaudRate = 9600/*波特率*/, BYTE ByteSize = 8/*校验位*/,
		                       BYTE Parity = NOPARITY/*数据位*/ , BYTE StopBits = ONESTOPBIT/*停止位*/);
	BOOL                 WriteCom(char * lpOutBuffer, DWORD dwBytesWrite/*发送字节长度*/);
	BOOL                 ReadCom(char * buf, DWORD len, LPDWORD outCount);

private:
	HANDLE               m_hCom;//串口句柄
	int                  m_nComID;//串口号

private:
	//十六进制转ASCII码
	int                  Char2ToChar(char * str, char * result, int len/*要转换字节的长度，一般为目标字节的2倍*/);
	char                 ConvertHexChar(char ch) ;
	//ASCII码转十六进制
	void                 ASCIIToChar(char * Str, char * Dst);
};
