#pragma once

#ifdef NETPACKET_EXPORTS
#define DATCOLLECT_API _declspec(dllexport)
#else
#ifdef DONTIMPORT
#define DATCOLLECT_API
#else
#define DATCOLLECT_API _declspec(dllimport)
#endif
#endif

//运行情况
enum ERunCondition
{
	ERC_START = 1,	// 软件开始
	ERC_CLOSE,		// 软件关闭
	ERC_ALARM,		// 软件异常
};

class DATCOLLECT_API DataCollect
{
public:
	// 软件互动
	static void Interaction();

	// 异常报警
	static void Alarm();

	// 软件运行情况 开始、关闭、异常
	static void Software(int _eRunCondition);
	static void Software(ERunCondition _eRunCondition);
};

// 一下给C#使用
extern "C" DATCOLLECT_API void __stdcall DC_Interaction();
extern "C" DATCOLLECT_API void __stdcall DC_Alarm();
//ERC_START = 1,	// 软件开始
//ERC_CLOSE,		// 软件关闭
//ERC_ALARM,		// 软件异常
extern "C" DATCOLLECT_API void __stdcall DC_Software(int _eRunCondition);
