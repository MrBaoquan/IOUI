#pragma once

#ifdef AHDATACOLLECT_EXPORTS
#define AHDCEXPORT _declspec(dllexport)
#else
#ifdef DONTIMPORT
#define AHDCEXPORT
#else
#define AHDCEXPORT _declspec(dllimport)
#endif
#endif

extern "C" AHDCEXPORT void __stdcall DataCollectInital();
extern "C" AHDCEXPORT void __stdcall DataCollectUpdate();
extern "C" AHDCEXPORT void __stdcall DataCollectRelese();
extern "C" AHDCEXPORT void __stdcall DataCollectBtnClicked(int _nBtnID);