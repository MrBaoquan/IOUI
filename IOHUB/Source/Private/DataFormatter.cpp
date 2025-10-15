#include "DataFormatter.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace IOHub {

std::string DataFormatter::removeSpaces(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char ch : str) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            result.push_back(ch);
        }
    }
    return result;
}

bool DataFormatter::isHexString(const std::string& str) {
    if (str.empty()) return false;
    
    std::string clean = removeSpaces(str);
    
    // 移除0x前缀
    if (clean.size() >= 2 && clean.substr(0, 2) == "0x") {
        clean = clean.substr(2);
    }
    
    if (clean.empty()) return false;
    
    // 检查是否全是16进制字符
    return std::all_of(clean.begin(), clean.end(), 
        [](char c) { return std::isxdigit(static_cast<unsigned char>(c)); });
}

DataFormat DataFormatter::detectFormat(const std::string& str) {
    std::string trimmed = removeSpaces(str);
    
    if (trimmed.empty()) {
        return DataFormat::ASCII;
    }
    
    // 如果以0x开头，肯定是HEX
    if (trimmed.size() >= 2 && trimmed.substr(0, 2) == "0x") {
        return DataFormat::HEX;
    }
    
    // 如果全是16进制字符且长度是偶数，很可能是HEX
    if (isHexString(trimmed) && trimmed.size() % 2 == 0) {
        return DataFormat::HEX;
    }
    
    return DataFormat::ASCII;
}

std::vector<uint8_t> DataFormatter::parseFromConfig(const std::string& configStr, DataFormat format) {
    if (format == DataFormat::AUTO) {
        format = detectFormat(configStr);
    }
    
    if (format == DataFormat::HEX) {
        return hexToBytes(configStr);
    } else {
        return asciiToBytes(configStr);
    }
}

std::vector<uint8_t> DataFormatter::asciiToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::vector<uint8_t> DataFormatter::hexToBytes(const std::string& hexStr) {
    std::string clean = removeSpaces(hexStr);
    
    // 移除0x前缀
    if (clean.size() >= 2 && clean.substr(0, 2) == "0x") {
        clean = clean.substr(2);
    }
    
    // 确保长度为偶数
    if (clean.size() % 2 != 0) {
        clean = "0" + clean;
    }
    
    std::vector<uint8_t> result;
    result.reserve(clean.size() / 2);
    
    for (size_t i = 0; i < clean.size(); i += 2) {
        try {
            uint8_t byte = static_cast<uint8_t>(
                std::stoi(clean.substr(i, 2), nullptr, 16));
            result.push_back(byte);
        } catch (...) {
            // 解析失败，跳过
        }
    }
    
    return result;
}

std::string DataFormatter::bytesToHex(const std::vector<uint8_t>& bytes, bool withPrefix, bool withSpaces) {
    std::ostringstream oss;
    
    if (withPrefix) {
        oss << "0x";
    }
    
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (withSpaces && i > 0) {
            oss << " ";
        }
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(bytes[i]);
    }
    
    return oss.str();
}

std::string DataFormatter::toDisplayString(const std::vector<uint8_t>& data, DataFormat format) {
    switch (format) {
        case DataFormat::HEX:
        case DataFormat::FRAME:
            return bytesToHex(data, true, true);
            
        case DataFormat::ASCII:
        case DataFormat::AUTO:
        default:
            // 尝试作为ASCII显示，不可打印字符用十六进制
            std::ostringstream oss;
            bool hasNonPrintable = false;
            
            for (uint8_t byte : data) {
                if (std::isprint(byte)) {
                    oss << static_cast<char>(byte);
                } else {
                    hasNonPrintable = true;
                    break;
                }
            }
            
            if (hasNonPrintable) {
                return bytesToHex(data, true, true);
            } else {
                return oss.str();
            }
    }
}

} // namespace IOHub