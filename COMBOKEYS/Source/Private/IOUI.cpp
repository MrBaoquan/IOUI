/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <stdlib.h>
#include <algorithm>
#include "IOUI.h"
#include "Paths.hpp"
#include <utility>
#include <limits.h>
#include "windows.h"
#include "mIni/mini/ini.h"
#include <map>
#include <string>
#include <sstream>

extern HINSTANCE DLL_INSTANCE;
HHOOK g_hHook = NULL;
// 当前组合键记录
std::string g_currentComboKey = "";
std::string g_terminalKey;
// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

// 辅助函数：获取按键名称，基于KBDLLHOOKSTRUCT的scanCode和vkCode
std::string GetKeyNameFromLParam(LPARAM lParam, WPARAM wParam)
{
    // 低级键盘钩子结构指针
    KBDLLHOOKSTRUCT* kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (!kbStruct) return "";

    // 获取扫描码
    UINT scanCode = kbStruct->scanCode;
    // 部分键异常处理，参考MSDN建议：
    // 扩展键设置第24位
    if ((kbStruct->flags & LLKHF_EXTENDED) != 0) {
        scanCode |= 0x01000000;
    }

    // lParam对应GetKeyNameText要求的格式
    // bits 16-23 = scan code
    // bit 24 = extended key flag
    LONG lParamKeyName = (scanCode << 16);

    char keyName[128] = { 0 };
    if (GetKeyNameTextA(lParamKeyName, keyName, sizeof(keyName)) == 0) {
        // 若失败，尝试简单转换vkCode
        std::ostringstream oss;
        oss << "VK_" << kbStruct->vkCode;
        return oss.str();
    }
    return std::string(keyName);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0 || code == HC_NOREMOVE) {
        // 继续调用下一个钩子
        return CallNextHookEx(g_hHook, code, wParam, lParam);
    }

    // 我们只在键盘“按键释放”事件处理（注意：处理低级钩子消息WM_KEYUP）
    if (wParam != WM_KEYUP && wParam != WM_SYSKEYUP) {
        return CallNextHookEx(g_hHook, code, wParam, lParam);
    }

    std::string keyName = GetKeyNameFromLParam(lParam, wParam);

#ifdef _DEBUG
    std::ostringstream dbgStream;
    dbgStream << "Key Released: " << keyName << " (lParam: " << lParam << ")\n";
    OutputDebugStringA(dbgStream.str().c_str());
#endif

    if (g_terminalKey == keyName) {
        if (!g_currentComboKey.empty()) {
            g_cachedComboKeys.push_back(g_currentComboKey);
            g_currentComboKey.clear();
        }
    }
    else {
        if (!g_currentComboKey.empty())
            g_currentComboKey += "|" + keyName;
        else
            g_currentComboKey = keyName;
    }

    return CallNextHookEx(g_hHook, code, wParam, lParam);
}

BOOL InstallHook() {
    // 安装低级键盘钩子，DLL_INSTANCE必须为有效模块句柄
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, DLL_INSTANCE, 0);
    if (g_hHook == NULL) {
        DWORD err = GetLastError();
        std::ostringstream oss;
        oss << "SetWindowsHookEx failed, error code: " << err << "\n";
        OutputDebugStringA(oss.str().c_str());
        return FALSE;
    }
    return TRUE;
}

BOOL UninstallHook() {
    if (g_hHook) {
        BOOL ret = UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
        return ret;
    }
    return FALSE;
}

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
    devInfo.InputCount = 255;
    devInfo.OutputCount = 0;
    devInfo.AxisCount = 0;
    return &devInfo;
}

std::map<std::string, int> g_comboKeysMap;
mINI::INIFile* g_iniFIle = nullptr;
mINI::INIStructure* g_iniStructure = nullptr;

int AppendComboKeys(const std::string& comboKey) {
    if (g_comboKeysMap.count(comboKey) > 0) return -1;

    if (!g_iniFIle || !g_iniStructure)
        return -1;

    auto& ini = *g_iniStructure;
    auto& file = *g_iniFIle;

    try {
        //if (!ini.has("ComboKeys")) {
        //    ini["ComboKeys"] = mINI::Section();
        //}

        for (int idx = 0; idx < devInfo.InputCount; ++idx) {
            std::string key = "k" + std::to_string(idx);
            if (!ini["ComboKeys"].has(key)) {
                ini["ComboKeys"][key] = comboKey;
                g_comboKeysMap[comboKey] = idx;
                file.write(ini);
                return idx;
            }
        }
    }
    catch (...) {
        return -1;
    }

    return -1;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
    // 禁止创建多个设备, 没有意义
    static bool _created = false;
    if (_created) return 0;
    _created = true;

    std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\COMBOKEYS\\config.ini";
    g_iniFIle = new mINI::INIFile(config_file_path);
    g_iniStructure = new mINI::INIStructure();
    g_iniFIle->read(*g_iniStructure);
    auto& ini = *g_iniStructure;

    g_terminalKey = "Enter"; // 默认终端键
    // 优先从default节读取terminal_key，默认值为Enter
    if (ini.has("default") && ini["default"].has("terminal_key")) {
        g_terminalKey = ini["default"]["terminal_key"];
    }

    for (int _idx = 0; _idx < devInfo.InputCount; ++_idx)
    {
        std::string _key = "k" + std::to_string(_idx);
        std::string& _val = ini["ComboKeys"][_key];
        if (_val == "") {
            ini["ComboKeys"].remove(_key);
            continue;
        }
        g_comboKeysMap.insert(std::pair<std::string, int>(_val, _idx));
    }

    return InstallHook();
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    delete g_iniFIle; g_iniFIle = nullptr;
    delete g_iniStructure; g_iniStructure = nullptr;
    return UninstallHook();
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    ZeroMemory(OutDIStatus, sizeof(BYTE) * devInfo.InputCount);
    for (const std::string& _comboKey : g_cachedComboKeys)
    {
        if (g_comboKeysMap.count(_comboKey) <= 0) AppendComboKeys(_comboKey);
        if (g_comboKeysMap.count(_comboKey) <= 0) continue;
        OutDIStatus[g_comboKeysMap[_comboKey]] = 1;
    }
    g_cachedComboKeys.clear();
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 0;
}
