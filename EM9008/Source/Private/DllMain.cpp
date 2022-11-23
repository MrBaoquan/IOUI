/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:38
 *  Email: mrma617@gmail.com
 */

#include <windows.h>
#include "Paths.hpp"

namespace dh = DevelopHelper;
BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: 
        {
		    dh::Paths::Instance().SetModule(hinstDLL);
		    std::string dllPath = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\";
		    std::wstring _wdllPath(dllPath.begin(), dllPath.end());
		    SetDllDirectoryA(dllPath.data());
		    AddDllDirectory(_wdllPath.data());
        }
        OutputDebugStringA("============== Attched external dll EM9008 for IODevice.dll ... ================ \n");
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugStringA("============== Detached external dll EM9008 for IODevice.dll ... ================ \n");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}