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
HHOOK g_hHook = NULL;

// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

std::set<int> keysPressed; // 存储已经按下的键

// 定义消息结构
struct KeyMessage {
    std::string keyName; // 按键名称
    bool isPressed; // 按下或弹起
};

// 消息队列映射
std::map<uint8, std::queue<KeyMessage>> messageQueues;
std::mutex queueMutex; // 互斥锁保护消息队列
std::mutex keysMutex;  // 互斥锁保护已按下的键集合

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        int vkCode = pKeyboard->vkCode;

        char keyName[256];
        if (!GetKeyNameTextA((MapVirtualKey(vkCode, MAPVK_VK_TO_VSC) << 16) | (0x0000 & 0xFFFF), keyName, sizeof(keyName))) {
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        std::string keyNameStr = std::string(keyName);

        // 处理按键按下事件
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std::lock_guard<std::mutex> lock(keysMutex);
            if (keysPressed.find(vkCode) == keysPressed.end()) {
                keysPressed.insert(vkCode);

                // 加锁并将按下事件加入所有设备的消息队列
                std::lock_guard<std::mutex> lockQueue(queueMutex);
                for (auto& pair : messageQueues) {
                    pair.second.push(KeyMessage{ keyNameStr, true });
                }
            }
        }
        // 处理按键弹起事件
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            std::lock_guard<std::mutex> lock(keysMutex);
            if (keysPressed.find(vkCode) != keysPressed.end()) {
                keysPressed.erase(vkCode);

                // 加锁并将弹起事件加入所有设备的消息队列
                std::lock_guard<std::mutex> lockQueue(queueMutex);
                for (auto& pair : messageQueues) {
                    pair.second.push(KeyMessage{ keyNameStr, false });
                }
            }
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

BOOL InstallHook() {
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, DLL_INSTANCE, 0);
    if (g_hHook == NULL) {
        return FALSE;
    }
    return TRUE;
}

BOOL UninstallHook() {
    return UnhookWindowsHookEx(g_hHook);
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

    return InstallHook();
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueues.erase(deviceIndex);
    }

    if (!messageQueues.empty()) return 1;

    delete g_iniFIle;
    delete g_iniStructure;
    return UninstallHook();
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
