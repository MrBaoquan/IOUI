// kpci800.h : main header file for the KPCI800 DLL
//
#include <windows.h>
typedef unsigned long ULONG;

BOOL WINAPI OpenDevice(ULONG mindex);
BOOL WINAPI OpenKPCI820(ULONG mindex);
BOOL WINAPI OpenmDevice(ULONG mindex,ULONG boardtype);
void  WINAPI CloseDevice(ULONG mindex);
void  WINAPI WriteByte(ULONG mindex,UCHAR doch,UCHAR dout);
UCHAR WINAPI ReadByte(ULONG mindex,UCHAR dich);
WORD WINAPI ReadWord(ULONG mindex,UCHAR dich);
void WINAPI WriteWord(ULONG mindex,UCHAR doch,WORD dout);
void WINAPI WriteDword(ULONG mindex,UCHAR doch,ULONG dout);
ULONG WINAPI ReadDword(ULONG mindex,UCHAR dich);
WORD  WINAPI PCI812_ReadAd(ULONG mindex,UCHAR adch,ULONG delay);
WORD  WINAPI PCI815_ReadAd(ULONG mindex,UCHAR adch,ULONG delay);
void WINAPI DAOut823(ULONG mindex,UCHAR dach,UCHAR aodata);
void WINAPI TimeDelay(ULONG timedelay);
void WINAPI DoutBit(ULONG mindex,UCHAR doch,BOOL dodata);
void WINAPI DAOut820(ULONG mindex,UCHAR dach,USHORT aodata);
void WINAPI DAOut825(ULONG mindex,UCHAR dach,USHORT aodata);