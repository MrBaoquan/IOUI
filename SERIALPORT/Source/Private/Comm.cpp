#include <string>
#include "Comm.h"


Comm::Comm(void)
{
	m_hCom = INVALID_HANDLE_VALUE;
}

Comm::~Comm(void)
{
	CloseHandle(m_hCom);
}
void Comm::Init(int nComID, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
	wchar_t strCom[10];

	m_nComID = nComID;

	if ( m_nComID <10 )
		swprintf_s( strCom,L"COM%d",m_nComID);
	else
		swprintf_s(strCom,L"\\\\.\\COM%d",m_nComID);
	
	m_hCom = CreateFile(strCom,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,/*NULL*/FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if ( m_hCom == (HANDLE)-1)
	{
		MessageBoxW(NULL,L"打开串口失败！",L"警告！",0);
	}

	SetupComm(m_hCom,1024,1024);
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout=0/*1000*/;
	TimeOuts.ReadTotalTimeoutMultiplier=0/*500*/;
	TimeOuts.ReadTotalTimeoutConstant=0/*5000*/;
	TimeOuts.WriteTotalTimeoutMultiplier=0/*500*/;
	TimeOuts.WriteTotalTimeoutConstant=0/*2000*/;
	SetCommTimeouts(m_hCom,&TimeOuts);

	DCB dcb;
	GetCommState(m_hCom,&dcb);
	dcb.BaudRate=BaudRate;
	dcb.ByteSize=ByteSize;
	dcb.Parity=Parity;
	dcb.StopBits=StopBits;
	SetCommState(m_hCom,&dcb);

	PurgeComm(m_hCom,PURGE_TXABORT|PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXCLEAR);
	
}
BOOL Comm::WriteCom(char * lpOutBuffer, DWORD dwBytesWrite)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD nCount;
	OVERLAPPED osWrite;
	memset( &osWrite, 0, sizeof(OVERLAPPED) );
	osWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	bWriteStat = WriteFile(m_hCom,lpOutBuffer,dwBytesWrite,&nCount,&osWrite);
	if(!bWriteStat)
	{
		if (GetLastError()==ERROR_IO_PENDING)
		{
			WaitForSingleObject(osWrite.hEvent,1000);
			return true;
		}
		return false;
	}
	ClearCommError(m_hCom,&dwErrorFlags,&ComStat);
	PurgeComm(m_hCom,PURGE_TXABORT|PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXCLEAR);
	return bWriteStat;
}
BOOL Comm::ReadCom(char * buf, DWORD len, LPDWORD outCount)
{

	BOOL bReadStat;
	OVERLAPPED osRead;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	memset( &osRead, 0, sizeof(OVERLAPPED) );
	osRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	ClearCommError(m_hCom,&dwErrorFlags,&ComStat);
	len = min(len, (DWORD)ComStat.cbInQue);
	if (!len)
		return FALSE;
	bReadStat = ReadFile(m_hCom,buf,len,outCount,&osRead);
	if(!bReadStat)
	{
		if (GetLastError()==ERROR_IO_PENDING)
		{
			WaitForSingleObject(osRead.hEvent,1000);
			PurgeComm(m_hCom,PURGE_TXABORT|PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXCLEAR);
			//GetOverlappedResult(m_hCom,&osRead,outCount,TRUE);
			return TRUE;
		}
		return bReadStat;
	}

	PurgeComm(m_hCom,PURGE_TXABORT|PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXCLEAR);
	return bReadStat;
}
void Comm::ASCIIToChar(char * Str, char * Dst)
{
	int nLen = static_cast<int>(strlen(Str));
	BYTE bt[1024];
	std::string strch;
	for (int i=0;i<nLen;i++)
	{
		char tt[4];
		bt[i]=Str[i];
		sprintf_s(tt,"%x",bt[i]);
		strch += tt;
	}
	char strresult[1024];
	memset(strresult,0,sizeof(strresult));
	for (unsigned int i=0;i<strch.size();i++)
	{
		strresult[i]=strch[i];
	}
}
int Comm::Char2ToChar(char * str, char * result, int len)
{
	int hexdata,lowhexdata;
	int hexdatalen=0;
	for(int i=0;i<len;)
	{
		char lstr,hstr=str[i];
		if(hstr==' ')
		{
			i++;
			continue;
		}
		i++;
		if(i>=len)
			break;
		lstr=str[i];
		hexdata=ConvertHexChar(hstr);
		lowhexdata=ConvertHexChar(lstr);
		if((hexdata==16)||(lowhexdata==16))
			break;
		else 
			hexdata=hexdata*16+lowhexdata;
		i++;
		/*senddata[hexdatalen]=(char)hexdata;*/
		result[hexdatalen]=(char)hexdata;
		hexdatalen++;
	}
	/*senddata.SetSize(hexdatalen);*/
	return hexdatalen;
}
char Comm::ConvertHexChar(char ch) 
{
	if((ch>='0')&&(ch<='9'))
		return ch-0x30;
	else if((ch>='A')&&(ch<='F'))
		return ch-'A'+10;
	else if((ch>='a')&&(ch<='f'))
		return ch-'a'+10;
	else return (-1);
}