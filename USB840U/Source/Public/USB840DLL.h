// USB840DLL.h : main header file for the USB840 DLL
//

//打开USB设备
bool  DeviceOpen(unsigned int mindex);
//关闭USB设备
void  DeviceClose(unsigned int mindex);
//读取8个通道开关量输入状态
unsigned char  USB840ReadDI(unsigned int mindex,unsigned char addr);
//设置8个通道开关量输出状态
bool  USB840SetDO(unsigned int mindex,unsigned char addr,unsigned char do_data);
//设置单通道开关量输出状态
bool USB840SetBitDO(unsigned int mindex,unsigned char chnum,bool do_data);
//设定块写，块读，中断写，中断读的超时设定值
bool DeviceSetTimeOut(unsigned int mindex, unsigned int iWriteTimeout,unsigned int iReadTimeout,unsigned int iAuxTimeout,unsigned int iInterTimeout );
//设定设备独占使用方式
bool DeviceSetExclusive(unsigned int mindex,unsigned int lexclusiv);