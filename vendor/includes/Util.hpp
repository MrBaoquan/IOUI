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

