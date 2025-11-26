# TinyExpr 共享库说明

## 位置

TinyExpr 库现在位于项目根目录的 vendor 中：

```
vendor/includes/TinyExpr/
├── tinyexpr.h
└── tinyexpr.c
```

## 使用项目

以下项目共享此 TinyExpr 库：

1. **MODBUS** - 用于表达式解析和数据转换
2. **SNAP7** - 用于 AI 通道表达式支持

## 包含方式

### 在代码中

两个项目统一使用相对路径包含：

```cpp
extern "C" {
#include "TinyExpr/tinyexpr.h"
}
```

### 在项目文件中

**编译源文件**:
```xml
<ClCompile Include="..\vendor\includes\TinyExpr\tinyexpr.c">
  <CompileAs>CompileAsC</CompileAs>
  <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)</PreprocessorDefinitions>
  <WarningLevel>Level1</WarningLevel>
</ClCompile>
```

**包含路径**: 
项目需要在包含路径中添加 `vendor\includes`（已在 Props 配置中设置）

## 版本信息

- **来源**: MODBUS 项目的 TinyExpr 实现
- **版本**: TinyExpr 原始版本 + 修复
- **许可证**: Zlib License
- **特性**: 
  - 支持变量绑定
  - 支持 20+ 数学函数
  - 表达式预编译优化

## 修改记录

### 枚举值调整

为避免 `switch-case` 冲突，调整了枚举值：

```c
enum {
    TE_VARIABLE = 0,
    
    TE_FUNCTION0 = 8, TE_FUNCTION1, TE_FUNCTION2, TE_FUNCTION3,
    TE_FUNCTION4, TE_FUNCTION5, TE_FUNCTION6, TE_FUNCTION7,
    
    TE_CLOSURE0 = 16, TE_CLOSURE1, TE_CLOSURE2, TE_CLOSURE3,
    TE_CLOSURE4, TE_CLOSURE5, TE_CLOSURE6, TE_CLOSURE7,
    
    TE_CONSTANT = 24,
    
    TE_FLAG_PURE = 32
};
```

### Switch 语句优化

将可能冲突的 `switch-case` 改为 `if-else` 结构，确保编译器正确处理。

## 维护指南

### 更新 TinyExpr

如果需要更新 TinyExpr 库：

1. 修改 `vendor/includes/TinyExpr/` 中的文件
2. 确保枚举值不冲突
3. 测试 MODBUS 和 SNAP7 两个项目的编译
4. 运行测试验证功能正常

### 添加新使用项目

如果新项目需要使用 TinyExpr：

1. **项目文件** 添加编译项：
   ```xml
   <ClCompile Include="..\vendor\includes\TinyExpr\tinyexpr.c">
     <CompileAs>CompileAsC</CompileAs>
   </ClCompile>
   ```

2. **代码中** 包含头文件：
   ```cpp
   extern "C" {
   #include "TinyExpr/tinyexpr.h"
   }
   ```

3. **确保包含路径** 包含 `vendor\includes`

## 支持的功能

### 运算符
- 算术: `+`, `-`, `*`, `/`, `^`, `%`
- 括号: `(`, `)`

### 函数
- 基础: `abs()`, `sqrt()`, `pow()`
- 三角: `sin()`, `cos()`, `tan()`, `asin()`, `acos()`, `atan()`, `atan2()`
- 双曲: `sinh()`, `cosh()`, `tanh()`
- 对数: `ln()`, `log()`, `log10()`, `exp()`
- 取整: `ceil()`, `floor()`
- 常量: `pi()`, `e()`
- 其他: `fac()` (阶乘), `ncr()`, `npr()`

### 使用示例

```cpp
// 简单求值
double result = te_interp("2 + 3 * 4", nullptr);

// 变量绑定
double x = 5.0;
te_variable vars[] = {{"x", &x}};
te_expr* expr = te_compile("x * 2 + 10", vars, 1, nullptr);
if (expr) {
    double result = te_eval(expr);  // result = 20
    te_free(expr);
}
```

## 相关文档

- [SNAP7/EXPRESSION_GUIDE.md](../SNAP7/EXPRESSION_GUIDE.md) - SNAP7 表达式使用指南
- [SNAP7/EXPRESSION_QUICK_START.md](../SNAP7/EXPRESSION_QUICK_START.md) - 快速开始
- [MODBUS/Config/README.md](../MODBUS/Config/README.md) - MODBUS 配置说明

---

**更新日期**: 2025-11-25  
**维护者**: 项目团队
