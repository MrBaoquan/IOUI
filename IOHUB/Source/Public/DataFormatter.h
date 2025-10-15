#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace IOHub {

// 数据格式枚举
enum class DataFormat {
    AUTO,       // 自动检测
    ASCII,      // ASCII字符串
    HEX,        // 16进制字节序列
    FRAME       // 自定义帧格式（用于串口）
};

// 数据格式化工具类
class DataFormatter {
public:
    // 解析配置字符串为字节序列
    static std::vector<uint8_t> parseFromConfig(const std::string& configStr, DataFormat format = DataFormat::AUTO);
    
    // 字节序列转显示字符串
    static std::string toDisplayString(const std::vector<uint8_t>& data, DataFormat format);
    
    // 检测数据格式
    static DataFormat detectFormat(const std::string& str);
    
    // ASCII字符串转字节
    static std::vector<uint8_t> asciiToBytes(const std::string& str);
    
    // HEX字符串转字节 (支持 "FE01AA" 或 "FE 01 AA" 或 "0xFE01AA")
    static std::vector<uint8_t> hexToBytes(const std::string& hexStr);
    
    // 字节转HEX字符串
    static std::string bytesToHex(const std::vector<uint8_t>& bytes, bool withPrefix = true, bool withSpaces = false);

private:
    static std::string removeSpaces(const std::string& str);
    static bool isHexString(const std::string& str);
};

} // namespace IOHub