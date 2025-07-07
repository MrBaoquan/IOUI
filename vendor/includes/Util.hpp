#pragma once
#include <windows.h>
#include <vector>
#include <string>

bool CopyTo(BYTE* Src, short* Dst, size_t Count) 
{
	ZeroMemory(Dst, sizeof(short) * Count);
	for (size_t _index = 0; _index < Count; ++_index)
	{
		Dst[_index] = static_cast<short>(Src[_index]);
	}
	return true;
}

bool CopyTo(std::vector<BYTE> Src, short* Dst) 
{
	ZeroMemory(Dst, sizeof(short)*Src.size());
	for (size_t _index = 0; _index < Src.size(); ++_index)
	{
		Dst[_index] = Src[_index];
	}
	return true;
}

bool CopyTo(short* Src, BYTE* Dst, size_t Count)
{
	ZeroMemory(Dst, sizeof(BYTE)*Count);
	for (size_t _index = 0; _index < Count; ++_index)
	{
		Dst[_index] = static_cast<BYTE>(Src[_index] > 0 ? 1 : 0);
	}
	return true;
}

bool CopyTo(short* Src, short* Dst, size_t Count)
{
	ZeroMemory(Dst, sizeof(short) * Count);
	for (size_t _index = 0; _index < Count; ++_index)
	{
		Dst[_index] = Src[_index];
	}
	return true;
}

bool CopyTo(short* Src, const std::vector<BYTE>& Dst, size_t Count)
{
	BYTE* _DstPtr = const_cast<BYTE*>(Dst.data());
	ZeroMemory(_DstPtr, sizeof(BYTE)*Count);
	for (size_t _index = 0; _index < Count; ++_index)
	{
		_DstPtr[_index] = static_cast<BYTE>(Src[_index]);
	}
	return true;
}

bool CopyTo(LONG* Src, short* Dst, size_t Count)
{
	ZeroMemory(Dst, sizeof(short) * Count);
	for (size_t _index = 0; _index < Count; ++_index)
	{
		Dst[_index] = static_cast<short>(Src[_index]);
	}
	return true;
}

std::string ToString(short* Arr, size_t Count) {
	std::string _str = "";
	for (size_t _index = 0; _index < Count; ++_index)
	{
		std::string _valFormat = std::to_string(_index).append(": ") + std::to_string(Arr[_index]).append(" |");
		_str.append(_valFormat);
	}
	return _str.append("\n");
}

std::string ToString(BYTE* Arr, size_t Count) 
{
	std::string _str = "";
	for (size_t _index = 0; _index < Count; ++_index)
	{
		std::string _valFormat = std::to_string(_index).append(": ") + std::to_string(Arr[_index]).append(" |");
		_str.append(_valFormat);
	}
	return _str.append("\n");
}

std::string BuildDeviceAttribute(const char* Attribute, uint8 DeviceIndex) {
	return std::string(Attribute) +"_" + std::to_string(DeviceIndex);
}

void Int2Bytes(std::vector<BYTE>& Bytes, int Val) {
	Bytes.push_back((Val >> 24) & 0xFF);
	Bytes.push_back((Val >> 16) & 0xFF);
	Bytes.push_back((Val >> 8) & 0xFF);
	Bytes.push_back((Val) & 0xFF);
}

void Long2Bytes(std::vector<BYTE>& Bytes, int Val) {
	Bytes.push_back((Val >> 24) & 0xFF);
	Bytes.push_back((Val >> 16) & 0xFF);
	Bytes.push_back((Val >> 8) & 0xFF);
	Bytes.push_back((Val) & 0xFF);
}

void Float2Bytes(std::vector<BYTE>& Bytes, int Val) {
	Bytes.push_back((Val >> 24) & 0xFF);
	Bytes.push_back((Val >> 16) & 0xFF);
	Bytes.push_back((Val >> 8) & 0xFF);
	Bytes.push_back((Val) & 0xFF);
}

// 将配置的串口名或设备索引转为合法 Windows 串口名格式
std::string NormalizePortName(const std::string& portNameInput, int deviceIndex = 0) {
	// 如果传入的串口名非空，先尝试处理
	if (!portNameInput.empty()) {
		std::string portName = portNameInput;
		// 转为大写统一处理（可选）
		for (auto& ch : portName) {
			ch = std::toupper(static_cast<unsigned char>(ch));
		}

		if (portName.size() > 3 && portName.substr(0, 3) == "COM") {
			try {
				int portNum = std::stoi(portName.substr(3));
				if (portNum > 9) {
					return "\\\\.\\" + portName;
				}
				else {
					return portName;
				}
			}
			catch (...) {
				// 解析失败，继续用 deviceIndex 处理
			}
		}
		else {
			// 不是以 COM 开头的，直接返回输入
			return portNameInput;
		}
	}

	// portNameInput 为空或者无效，使用 deviceIndex 生成
	if (deviceIndex <= 9 && deviceIndex > 0) {
		return "COM" + std::to_string(deviceIndex);
	}
	else if (deviceIndex > 9) {
		return "\\\\.\\COM" + std::to_string(deviceIndex);
	}
	else {
		// deviceIndex 非法（如负数或0），返回空串表示无效
		return "";
	}
}


