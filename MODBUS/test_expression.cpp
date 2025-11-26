/**
 * 测试表达式解析器
 */

#include <iostream>
#include <iomanip>
#include "../Source/Public/ExpressionParser.h"

int main() {
    std::cout << "=== 表达式解析器测试 ===" << std::endl << std::endl;
    
    ExpressionParser parser;
    
    // 加载预定义表达式
    std::map<std::string, std::string> expressions = {
        {"encoder_angle_10bit", "x * 360 / 1024"},
        {"encoder_angle_12bit", "x * 360 / 4096"},
        {"temp_pt100", "(x - 1000) * 0.1"},
        {"pressure_4_20ma", "(x - 4000) / 16000 * 100"}
    };
    parser.loadExpressions(expressions);
    
    std::cout << "已加载预定义表达式:" << std::endl;
    for (const auto& kv : expressions) {
        std::cout << "  " << kv.first << " = " << kv.second << std::endl;
    }
    std::cout << std::endl;
    
    // 测试用例
    struct TestCase {
        std::string scaleStr;
        double inputValue;
        std::string description;
    };
    
    TestCase tests[] = {
        // 数值缩放（向下兼容）
        {"0.1", 1000.0, "数值缩放 0.1"},
        {"0.3515625", 95803.0, "数值缩放 0.3515625 (编码器)"},
        
        // 预定义表达式
        {"encoder_angle_10bit", 95803.0, "预定义: encoder_angle_10bit"},
        {"encoder_angle_12bit", 383212.0, "预定义: encoder_angle_12bit"},
        {"temp_pt100", 1250.0, "预定义: temp_pt100"},
        {"pressure_4_20ma", 12000.0, "预定义: pressure_4_20ma"},
        
        // 内联表达式
        {"{x*360/1024}", 95803.0, "内联: {x*360/1024}"},
        {"{(x-1000)*0.1}", 1250.0, "内联: {(x-1000)*0.1}"},
        {"{x*0.01}", 5000.0, "内联: {x*0.01}"},
        
        // 复杂表达式
        {"{x*x*0.001 + x*0.5 + 10}", 100.0, "内联: 二次方程"},
        {"{sqrt(x)}", 16.0, "内联: sqrt(x)"},
        {"{sin(x * 3.14159 / 180)}", 30.0, "内联: sin(30度)"}
    };
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "测试结果:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& test : tests) {
        auto expr = parser.parse(test.scaleStr);
        
        if (expr && expr->isCompiled()) {
            double result = expr->eval(test.inputValue);
            std::cout << std::setw(40) << std::left << test.description << ": "
                      << std::setw(12) << test.inputValue << " -> "
                      << std::setw(15) << result << " ✓" << std::endl;
        } else {
            std::cout << std::setw(40) << std::left << test.description << ": "
                      << "编译失败 ✗";
            if (expr) {
                std::cout << " (" << expr->getError() << ")";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << std::string(80, '-') << std::endl;
    std::cout << std::endl;
    
    // 性能测试
    std::cout << "=== 性能测试 ===" << std::endl;
    auto perfExpr = parser.parse("encoder_angle_10bit");
    
    if (perfExpr && perfExpr->isCompiled()) {
        const int iterations = 1000000;
        auto start = std::chrono::high_resolution_clock::now();
        
        double sum = 0;
        for (int i = 0; i < iterations; i++) {
            sum += perfExpr->eval(95803.0);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        std::cout << "执行 " << iterations << " 次计算:" << std::endl;
        std::cout << "  总耗时: " << duration.count() / 1000000.0 << " ms" << std::endl;
        std::cout << "  平均耗时: " << duration.count() / iterations << " ns/次" << std::endl;
        std::cout << "  (防止优化: sum=" << sum << ")" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "测试完成!" << std::endl;
    
    return 0;
}
