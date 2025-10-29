// fixed16_16 单元测试验证代码
// 用于验证 parseFixed16_16 函数的正确性

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>

// 复制实现的函数
float parseFixed16_16(const uint16_t* regs, const std::string& byteOrder) {
    uint32_t rawValue;
    if (byteOrder == "BA") {
        rawValue = (static_cast<uint32_t>(regs[1]) << 16) | regs[0];
    } else {  // AB (默认大端序)
        rawValue = (static_cast<uint32_t>(regs[0]) << 16) | regs[1];
    }
    
    bool isNegative = (rawValue & 0x80000000) != 0;
    
    if (isNegative) {
        rawValue = rawValue & 0x7FFFFFFF;
    }
    
    float result = static_cast<float>(rawValue) / 65536.0f;
    
    return isNegative ? -result : result;
}

struct TestCase {
    uint16_t reg0;
    uint16_t reg1;
    std::string byteOrder;
    float expected;
    std::string description;
};

int main() {
    TestCase tests[] = {
        // 正数测试
        {0x0007, 0xADA7, "AB", 7.678f, "正数: 7.678 (大端序)"},
        {0x0001, 0x0000, "AB", 1.0f, "正数: 1.0 整数"},
        {0x0000, 0x8000, "AB", 0.5f, "正数: 0.5 纯小数"},
        {0x0000, 0x0000, "AB", 0.0f, "零值"},
        
        // 负数测试
        {0x8007, 0xADA7, "AB", -7.678f, "负数: -7.678 (大端序)"},
        {0x8001, 0x0000, "AB", -1.0f, "负数: -1.0 整数"},
        {0x8000, 0x8000, "AB", -0.5f, "负数: -0.5 纯小数"},
        
        // 小端序测试
        {0xADA7, 0x0007, "BA", 7.678f, "正数: 7.678 (小端序)"},
        {0xADA7, 0x8007, "BA", -7.678f, "负数: -7.678 (小端序)"},
        
        // 边界测试
        {0x7FFF, 0xFFFF, "AB", 32767.999f, "最大正数"},
        {0xFFFF, 0xFFFF, "AB", -32767.999f, "最大负数"},
    };
    
    std::cout << "=== fixed16_16 测试验证 ===" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    
    int passed = 0;
    int total = sizeof(tests) / sizeof(TestCase);
    
    for (int i = 0; i < total; i++) {
        uint16_t regs[2] = {tests[i].reg0, tests[i].reg1};
        float result = parseFixed16_16(regs, tests[i].byteOrder);
        float diff = std::abs(result - tests[i].expected);
        bool success = diff < 0.001f;  // 允许0.001的误差
        
        std::cout << "[" << (success ? "PASS" : "FAIL") << "] ";
        std::cout << tests[i].description << std::endl;
        std::cout << "  寄存器: 0x" << std::hex << std::uppercase 
                  << tests[i].reg0 << " 0x" << tests[i].reg1 << std::dec;
        std::cout << " (" << tests[i].byteOrder << ")" << std::endl;
        std::cout << "  期望值: " << tests[i].expected << std::endl;
        std::cout << "  实际值: " << result << std::endl;
        std::cout << "  误差: " << diff << std::endl;
        std::cout << std::endl;
        
        if (success) passed++;
    }
    
    std::cout << "=== 测试结果 ===" << std::endl;
    std::cout << "通过: " << passed << "/" << total << std::endl;
    std::cout << "成功率: " << (passed * 100.0 / total) << "%" << std::endl;
    
    return (passed == total) ? 0 : 1;
}
