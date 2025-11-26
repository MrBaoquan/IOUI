/**
 * 简单测试 TinyExpr 编译功能
 */
#include "Source/Public/TinyExpr/tinyexpr.h"
#include <stdio.h>

int main() {
    printf("=== TinyExpr 测试 ===\n\n");
    
    // 测试表达式
    const char* expressions[] = {
        "x",
        "x*2",
        "x*360",
        "x/1024",
        "x*360/1024",
        "x * 360 / 1024",  // 带空格
        "(x*360)/1024",     // 带括号
        NULL
    };
    
    double xValue = 95803.0;
    
    for (int i = 0; expressions[i] != NULL; i++) {
        printf("测试表达式: %s\n", expressions[i]);
        
        // 定义变量 x
        te_variable vars[] = {
            {"x", &xValue, TE_VARIABLE, NULL}
        };
        
        // 编译表达式
        int error = 0;
        te_expr* expr = te_compile(expressions[i], vars, 1, &error);
        
        if (!expr) {
            printf("  [失败] 编译失败, error=%d\n", error);
        } else {
            // 计算结果
            double result = te_eval(expr);
            printf("  [成功] x=%.0f 结果=%.6f\n", xValue, result);
            te_free(expr);
        }
        printf("\n");
    }
    
    return 0;
}
