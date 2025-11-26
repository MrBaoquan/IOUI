#include <stdlib.h>
#include <algorithm>
#include "IOUI.h"
#include "Paths.hpp"
#include <utility>
#include <limits.h>
#include "windows.h"
#include "mIni/mini/ini.h"
#include <map>
#include <set>
#include <queue>
#include <mutex>

extern HINSTANCE DLL_INSTANCE;

// Raw Input 相关变量
HWND g_targetWindow = NULL;
WNDPROC g_originalWndProc = NULL;
bool g_rawInputRegistered = false;

// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

std::set<int> keysPressed; // 存储已经按下的键（虚拟键码）

// 定义消息结构
struct KeyMessage {
    std::string keyName; // 按键名称
    bool isPressed; // 按下或弹起
};

// 消息队列映射
std::map<uint8, std::queue<KeyMessage>> messageQueues;
std::mutex queueMutex; // 互斥锁保护消息队列
std::mutex keysMutex;  // 互斥锁保护已按下的键集合

// 获取按键名称（从虚拟键码和扫描码）
std::string GetKeyNameFromRawInput(USHORT vKey, USHORT scanCode, USHORT flags)
{
    // 构建 lParam 用于 GetKeyNameText
    LONG lParam = (scanCode << 16);
    
    // 检查是否是扩展键
    if (flags & RI_KEY_E0) {
        lParam |= 0x01000000;  // 设置扩展键标志位
    }
    
    char keyName[256] = { 0 };
    if (GetKeyNameTextA(lParam, keyName, sizeof(keyName)) == 0) {
        // 若失败，尝试简单转换vKey
        return "VK_" + std::to_string(vKey);
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
                    
                    std::string keyName = GetKeyNameFromRawInput(vKey, scanCode, flags);
                    
                    // 处理按键按下事件
                    if (!(flags & RI_KEY_BREAK))
                    {
                        std::lock_guard<std::mutex> lock(keysMutex);
                        if (keysPressed.find(vKey) == keysPressed.end()) {
                            keysPressed.insert(vKey);
                            
                            // 将按下事件加入所有设备的消息队列
                            std::lock_guard<std::mutex> lockQueue(queueMutex);
                            for (auto& pair : messageQueues) {
                                pair.second.push(KeyMessage{ keyName, true });
                            }
                        }
                    }
                    // 处理按键弹起事件
                    else
                    {
                        std::lock_guard<std::mutex> lock(keysMutex);
                        if (keysPressed.find(vKey) != keysPressed.end()) {
                            keysPressed.erase(vKey);
                            
                            // 将弹起事件加入所有设备的消息队列
                            std::lock_guard<std::mutex> lockQueue(queueMutex);
                            for (auto& pair : messageQueues) {
                                pair.second.push(KeyMessage{ keyName, false });
                            }
                        }
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
        return FALSE;
    }
    
    // 子类化窗口，拦截消息
    g_originalWndProc = (WNDPROC)SetWindowLongPtr(g_targetWindow, GWLP_WNDPROC, (LONG_PTR)SubclassWndProc);
    if (!g_originalWndProc) {
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
    return TRUE;
}

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize() {
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

    auto& ini = *g_iniStructure;
    auto& file = *g_iniFIle;
    for (int _idx = 0; _idx < devInfo.InputCount; ++_idx) {
        std::string _key = "k" + std::to_string(_idx);
        if (!ini["KeyMap"].has(_key)) {
            ini["KeyMap"][_key] = comboKey;
            g_comboKeysMap.insert(std::pair<std::string, int>(comboKey, _idx));
            file.write(ini);
            return _idx;
        }
    }
    return -1;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex) {
    static bool _created = false;

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (messageQueues.find(deviceIndex) == messageQueues.end()) {
            messageQueues[deviceIndex] = std::queue<KeyMessage>();
        }
    }

    if (_created) return 1;

    _created = true;

    std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
    std::string config_file_path = path + "Config\\KEYBOARD\\config.ini";
    g_iniFIle = new mINI::INIFile(config_file_path);
    g_iniStructure = new mINI::INIStructure();
    g_iniFIle->read(*g_iniStructure);
    auto& ini = *g_iniStructure;

    for (int _idx = 0; _idx < devInfo.InputCount; ++_idx) {
        std::string _key = "k" + std::to_string(_idx);
        std::string& _val = ini["KeyMap"][_key];
        if (_val == "") {
            ini["KeyMap"].remove(_key);
            continue;
        }
        g_comboKeysMap.insert(std::pair<std::string, int>(_val, _idx));
    }

    return InstallRawInput();
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueues.erase(deviceIndex);
    }

    if (!messageQueues.empty()) return 1;

    delete g_iniFIle;
    delete g_iniStructure;
    return UninstallRawInput();
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus) {
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus) {
    return 0;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus) {
    std::lock_guard<std::mutex> lock(queueMutex);
    auto& queue = messageQueues[deviceIndex];
    if (!queue.empty()) {
        KeyMessage msg = queue.front();
        queue.pop();

        if (g_comboKeysMap.count(msg.keyName) <= 0) AppendComboKeys(msg.keyName);
        int channel = g_comboKeysMap[msg.keyName];
        OutDIStatus[channel] = msg.isPressed ? 1 : 0;
    }
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus) {
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
    return 0;
}
