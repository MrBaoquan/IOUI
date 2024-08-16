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

extern HINSTANCE DLL_INSTANCE;
HHOOK g_hHook = NULL;
// 当前组合键记录
std::string g_currentComboKey="";
std::string g_terminalKey;
// 当前缓存的待处理的组合键记录
std::vector<std::string> g_cachedComboKeys;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code < 0 || code == HC_NOREMOVE) {
		// 如果代码小于零，则挂钩过程必须将消息传递给CallNextHookEx函数，而无需进一步处理，并且应返回CallNextHookEx返回的值。此参数可以是下列值之一。(来自官网手册)
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}
	if (lParam & 0x40000000) {
		// 【第30位的含义】键状态。如果在发送消息之前按下了键，则值为1。如果键被释放，则为0。(来自官网手册)
		// 我们只考虑被按下后松开的状态
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}
	char szKeyName[MAX_PATH];
	// 【参数1】LPARAM类型，代表键状态
	// 【参数2】缓冲区
	// 【参数3】缓冲区大小
	GetKeyNameTextA(lParam, szKeyName, MAX_PATH);
	
	if (g_terminalKey == szKeyName) {
		g_cachedComboKeys.push_back(g_currentComboKey);
		g_currentComboKey = "";
	}
	else {
		g_currentComboKey += g_currentComboKey != "" ? (std::string("|") + szKeyName) : szKeyName;
	}

	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

BOOL InstallHook() {
	// 【参数1】钩子的类型，这里代表键盘钩子
	// 【参数2】钩子处理的函数
	// 【参数3】获取模块,PROJECT_NAME为DLL的项目名称
	// 【参数4】线程的ID，如果是全局钩子的话，这里要填0，如果是某个线程的钩子，那就需要写线程的ID
	g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, DLL_INSTANCE, 0);
	if (g_hHook == NULL) {
		// 钩子安装失败
		//MessageBox(NULL, L"全局钩子注册失败", L"信息", MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL UninstallHook() {
	return UnhookWindowsHookEx(g_hHook);
}

DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 255;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 0;
    return &devInfo;
}

std::map< std::string, int> g_comboKeysMap;
mINI::INIFile* g_iniFIle = nullptr;
mINI::INIStructure* g_iniStructure = nullptr;

int AppendComboKeys(const std::string& comboKey) {
	if (g_comboKeysMap.count(comboKey) > 0) return -1;

	auto& ini = *g_iniStructure;
	auto& file = *g_iniFIle;
	for (int _idx=0;_idx<devInfo.InputCount;++_idx)
	{
		std::string _key = "k" + std::to_string(_idx);
		if (!ini["ComboKeys"].has(_key)) {
			ini["ComboKeys"][_key] = comboKey;
			g_comboKeysMap.insert(std::pair<std::string, int>(comboKey, _idx));
			file.write(ini);
			return _idx;
		}
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
	g_iniFIle =new  mINI::INIFile(config_file_path);
	g_iniStructure =new  mINI::INIStructure();
	g_iniFIle->read(*g_iniStructure);
	auto& ini = *g_iniStructure;

	g_terminalKey = ini["ComboKeys"]["TerminalKey"];
	
	for (int _idx=0;_idx<devInfo.InputCount;++_idx)
	{
		std::string _key = "k" + std::to_string(_idx);
		std::string& _val = ini["ComboKeys"][_key];
		if (_val == "") {
			ini["ComboKeys"].remove (_key);
			continue;
		}
		g_comboKeysMap.insert(std::pair<std::string, int>(_val, _idx));
	}
	
	return InstallHook();
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	delete g_iniFIle;
	delete g_iniStructure;
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
	for (const std::string& _comboKey :g_cachedComboKeys)
	{
		if (g_comboKeysMap.count(_comboKey) <= 0)AppendComboKeys(_comboKey);
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