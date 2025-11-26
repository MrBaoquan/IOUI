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
#include <chrono>

extern HINSTANCE DLL_INSTANCE;

// Raw Input 相关变量
HWND g_targetWindow = NULL;  // 宿主程序的窗口句柄
WNDPROC g_originalWndProc = NULL;  // 原始窗口过程
bool g_rawInputRegistered = false;

// 当前组合键记录
std::string g_currentComboKey = "";
std::string g_terminalKey;
// 输入保持时间（毫秒）
int g_inputHoldTime = 100;
// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

// 输入通道激活时间记录
struct InputChannelState {
    bool isActive;
    std::chrono::steady_clock::time_point activatedTime;
};
std::map<int, InputChannelState> g_inputStates;

// 临界区保护共享数据
CRITICAL_SECTION g_dataCS;

// 获取按键名称（从虚拟键码和扫描码）
std::string GetKeyNameFromRawInput(USHORT vKey, USHORT scanCode, USHORT flags)
{
    // 构建 lParam 用于 GetKeyNameText
    LONG lParam = (scanCode << 16);
    
    // 检查是否是扩展键
    if (flags & RI_KEY_E0) {
        lParam |= 0x01000000;  // 设置扩展键标志位
    }
    
    char keyName[128] = { 0 };
    if (GetKeyNameTextA(lParam, keyName, sizeof(keyName)) == 0) {
        // 若失败，尝试简单转换vKey
        std::ostringstream oss;
        oss << "VK_" << vKey;
        return oss.str();
    }
    return std::string(keyName);
}

// 子类化的窗口过程，拦截 WM_INPUT 消息
LRESULT CALLBACK SubclassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INPUT)
    {
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        
        if (dwSize > 0)
        {
            LPBYTE lpb = new BYTE[dwSize];
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize)
            {
                RAWINPUT* raw = (RAWINPUT*)lpb;
                
                if (raw->header.dwType == RIM_TYPEKEYBOARD)
                {
                    USHORT vKey = raw->data.keyboard.VKey;
                    USHORT scanCode = raw->data.keyboard.MakeCode;
                    USHORT flags = raw->data.keyboard.Flags;
                    
                    // 只处理按键释放事件
                    if (flags & RI_KEY_BREAK)
                    {
                        std::string keyName = GetKeyNameFromRawInput(vKey, scanCode, flags);
                        
#ifdef _DEBUG
                        std::ostringstream dbgStream;
                        dbgStream << "Key Released: " << keyName << " (vKey: " << vKey << ")\n";
                        OutputDebugStringA(dbgStream.str().c_str());
#endif
                        
                        EnterCriticalSection(&g_dataCS);
                        
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
                        
                        LeaveCriticalSection(&g_dataCS);
                    }
                }
            }
            delete[] lpb;
        }
    }
    
    // 调用原始窗口过程
    if (g_originalWndProc) {
        return CallWindowProc(g_originalWndProc, hwnd, msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 枚举窗口的回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    
    // 检查是否是当前进程的窗口
    if (processId == GetCurrentProcessId())
    {
        // 检查是否是可见的主窗口（通常是我们需要的）
        if (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL)
        {
            HWND* pResult = (HWND*)lParam;
            *pResult = hwnd;
            return FALSE;  // 找到后停止枚举
        }
    }
    return TRUE;  // 继续枚举
}

BOOL InstallRawInput()
{
    InitializeCriticalSection(&g_dataCS);
    
    // 枚举当前进程的窗口，优先查找可见的主窗口
    HWND foundWindow = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&foundWindow);
    
    if (foundWindow) {
        g_targetWindow = foundWindow;
    }
    
    // 如果没找到可见窗口，查找任意属于当前进程的窗口
    if (!g_targetWindow) {
        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            GetWindowThreadProcessId(hwnd, &processId);
            
            if (processId == GetCurrentProcessId()) {
                HWND* pResult = (HWND*)lParam;
                *pResult = hwnd;
                return FALSE;  // 找到任意窗口就停止
            }
            return TRUE;
        }, (LPARAM)&g_targetWindow);
    }
    
    if (!g_targetWindow) {
        DeleteCriticalSection(&g_dataCS);
        return FALSE;
    }
    
    // 子类化窗口，拦截消息
    g_originalWndProc = (WNDPROC)SetWindowLongPtr(g_targetWindow, GWLP_WNDPROC, (LONG_PTR)SubclassWndProc);
    if (!g_originalWndProc) {
        DeleteCriticalSection(&g_dataCS);
        return FALSE;
    }
    
    // 注册 Raw Input 设备
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;  // HID_USAGE_PAGE_GENERIC
    rid.usUsage = 0x06;      // HID_USAGE_GENERIC_KEYBOARD
    rid.dwFlags = RIDEV_INPUTSINK;  // 即使窗口无焦点也接收输入
    rid.hwndTarget = g_targetWindow;
    
    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        // 恢复原始窗口过程
        SetWindowLongPtr(g_targetWindow, GWLP_WNDPROC, (LONG_PTR)g_originalWndProc);
        g_originalWndProc = NULL;
        DeleteCriticalSection(&g_dataCS);
        return FALSE;
    }
    
    g_rawInputRegistered = true;
    return TRUE;
}

BOOL UninstallRawInput()
{
    if (g_rawInputRegistered) {
        // 取消注册 Raw Input
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x06;
        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = NULL;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
        
        g_rawInputRegistered = false;
    }
    
    // 恢复原始窗口过程
    if (g_targetWindow && g_originalWndProc) {
        SetWindowLongPtr(g_targetWindow, GWLP_WNDPROC, (LONG_PTR)g_originalWndProc);
        g_originalWndProc = NULL;
    }
    
    g_targetWindow = NULL;
    DeleteCriticalSection(&g_dataCS);
    return TRUE;
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

    // 读取输入保持时间配置，默认为100ms
    g_inputHoldTime = 100;
    if (ini.has("default") && ini["default"].has("input_hold_ms")) {
        try {
            g_inputHoldTime = std::stoi(ini["default"]["input_hold_ms"]);
            if (g_inputHoldTime < 0) g_inputHoldTime = 0;
        }
        catch (...) {
            g_inputHoldTime = 100;
        }
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

    return InstallRawInput();
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    g_inputStates.clear();
    delete g_iniFIle; g_iniFIle = nullptr;
    delete g_iniStructure; g_iniStructure = nullptr;
    return UninstallRawInput();
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
    
    auto currentTime = std::chrono::steady_clock::now();
    
    EnterCriticalSection(&g_dataCS);
    
    // 处理新触发的组合键
    for (const std::string& _comboKey : g_cachedComboKeys)
    {
        if (g_comboKeysMap.count(_comboKey) <= 0) AppendComboKeys(_comboKey);
        if (g_comboKeysMap.count(_comboKey) <= 0) continue;
        
        int channelIndex = g_comboKeysMap[_comboKey];
        // 激活通道并记录时间
        g_inputStates[channelIndex].isActive = true;
        g_inputStates[channelIndex].activatedTime = currentTime;
    }
    g_cachedComboKeys.clear();
    
    LeaveCriticalSection(&g_dataCS);
    
    // 检查所有激活的通道，根据保持时间决定是否保持或重置
    std::vector<int> channelsToReset;
    for (auto& pair : g_inputStates)
    {
        int channelIndex = pair.first;
        InputChannelState& state = pair.second;
        
        if (state.isActive)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - state.activatedTime
            ).count();
            
            if (elapsed < g_inputHoldTime)
            {
                // 保持时间未到，保持激活状态
                OutDIStatus[channelIndex] = 1;
            }
            else
            {
                // 保持时间已到，标记为待重置
                channelsToReset.push_back(channelIndex);
            }
        }
    }
    
    // 重置超时的通道
    for (int channelIndex : channelsToReset)
    {
        g_inputStates[channelIndex].isActive = false;
    }
    
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
