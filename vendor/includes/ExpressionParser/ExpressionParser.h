/**
 * Expression Parser - 预编译表达式系统
 * 用于高性能的AI通道数据转换
 * 
 * 共享组件 - 供 SNAP7、MODBUS 等模块使用
 */

#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <string>
#include <map>
#include <memory>

// 前向声明 TinyExpr 类型
struct te_expr;

// 预编译表达式对象
class CompiledExpression {
public:
    CompiledExpression();
    ~CompiledExpression();
    
    // 禁用拷贝
    CompiledExpression(const CompiledExpression&) = delete;
    CompiledExpression& operator=(const CompiledExpression&) = delete;
    
    // 编译表达式
    bool compile(const std::string& expression);
    
    // 计算表达式值（x 为输入变量）
    double eval(double x);
    
    // 检查表达式是否已编译
    bool isCompiled() const { return m_compiled; }
    
    // 获取错误信息
    const std::string& getError() const { return m_errorMsg; }
    
private:
    te_expr* m_expr;
    double m_xValue;  // 变量 x 的存储
    bool m_compiled;
    std::string m_errorMsg;
};

// 表达式管理器
class ExpressionParser {
public:
    ExpressionParser();
    ~ExpressionParser();
    
    // 加载 [expressions] 配置区段
    void loadExpressions(const std::map<std::string, std::string>& expressionsSection);
    
    // 解析并编译表达式（支持预定义名称、内联表达式）
    std::shared_ptr<CompiledExpression> parse(const std::string& exprStr);
    
    // 清空所有缓存的表达式
    void clear();
    
private:
    // 预定义表达式库 (name -> formula)
    std::map<std::string, std::string> m_namedExpressions;
    
    // 编译缓存 (formula -> compiled expression)
    std::map<std::string, std::shared_ptr<CompiledExpression>> m_compiledCache;
    
    // 辅助函数：提取内联表达式 {x*360/1024} -> x*360/1024
    bool extractInlineExpr(const std::string& str, std::string& outFormula);
};

#endif // EXPRESSION_PARSER_H
