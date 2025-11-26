/**
 * Expression Parser - 预编译表达式系统实现
 * 
 * 共享组件 - 供 SNAP7、MODBUS 等模块使用
 */

#include "ExpressionParser.h"
#include "TinyExpr/tinyexpr.h"
#include <cmath>
#include <sstream>
#include <cctype>

// ============ CompiledExpression ============

CompiledExpression::CompiledExpression()
    : m_expr(nullptr)
    , m_xValue(0.0)
    , m_compiled(false)
{
}

CompiledExpression::~CompiledExpression()
{
    if (m_expr) {
        te_free(m_expr);
        m_expr = nullptr;
    }
}

bool CompiledExpression::compile(const std::string& expression)
{
    // 释放旧表达式
    if (m_expr) {
        te_free(m_expr);
        m_expr = nullptr;
    }
    
    m_compiled = false;
    m_errorMsg.clear();
    
    // 定义变量 x
    te_variable vars[] = {
        {"x", &m_xValue, TE_VARIABLE, 0}  // 使用 0 而不是 nullptr 以兼容 C
    };
    
    // 编译表达式
    int error = 0;
    m_expr = te_compile(expression.c_str(), vars, 1, &error);
    
    if (!m_expr) {
        std::ostringstream oss;
        if (error == -1) {
            oss << "Expression compile error (NULL result): " << expression;
        } else {
            oss << "Expression compile error at position " << error << ": " << expression;
        }
        m_errorMsg = oss.str();
        return false;
    }
    
    m_compiled = true;
    return true;
}

double CompiledExpression::eval(double x)
{
    if (!m_compiled || !m_expr) {
        return std::nan("");
    }
    
    m_xValue = x;
    return te_eval(m_expr);
}

// ============ ExpressionParser ============

ExpressionParser::ExpressionParser()
{
}

ExpressionParser::~ExpressionParser()
{
    clear();
}

void ExpressionParser::loadExpressions(const std::map<std::string, std::string>& expressionsSection)
{
    m_namedExpressions.clear();
    
    for (const auto& kv : expressionsSection) {
        // 去除前后空格
        std::string formula = kv.second;
        formula.erase(0, formula.find_first_not_of(" \t\r\n"));
        formula.erase(formula.find_last_not_of(" \t\r\n") + 1);
        
        m_namedExpressions[kv.first] = formula;
    }
}

bool ExpressionParser::extractInlineExpr(const std::string& str, std::string& outFormula)
{
    if (str.size() >= 2 && str.front() == '{' && str.back() == '}') {
        outFormula = str.substr(1, str.size() - 2);
        return true;
    }
    return false;
}

std::shared_ptr<CompiledExpression> ExpressionParser::parse(const std::string& scaleStr)
{
    // 去除前后空格
    std::string trimmed = scaleStr;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    
    if (trimmed.empty()) {
        // 空字符串，返回默认表达式 x (不缩放)
        auto expr = std::make_shared<CompiledExpression>();
        expr->compile("x");
        return expr;
    }
    
    std::string formula;
    
    // 1. 检查是否是内联表达式 {x*360/1024}
    if (extractInlineExpr(trimmed, formula)) {
        // 检查缓存
        if (m_compiledCache.count(formula)) {
            return m_compiledCache[formula];
        }
        
        auto expr = std::make_shared<CompiledExpression>();
        if (expr->compile(formula)) {
            m_compiledCache[formula] = expr;
            return expr;
        }
        return nullptr;
    }
    
    // 2. 检查是否是预定义表达式名称
    if (m_namedExpressions.count(trimmed)) {
        formula = m_namedExpressions[trimmed];
        
        // 检查缓存
        if (m_compiledCache.count(formula)) {
            return m_compiledCache[formula];
        }
        
        auto expr = std::make_shared<CompiledExpression>();
        if (expr->compile(formula)) {
            m_compiledCache[formula] = expr;
            return expr;
        }
        return nullptr;
    }
    
    // 3. 作为普通表达式尝试编译
    formula = trimmed;
    
    // 检查缓存
    if (m_compiledCache.count(formula)) {
        return m_compiledCache[formula];
    }
    
    auto expr = std::make_shared<CompiledExpression>();
    if (expr->compile(formula)) {
        m_compiledCache[formula] = expr;
        return expr;
    }
    
    return nullptr;
}

void ExpressionParser::clear()
{
    m_namedExpressions.clear();
    m_compiledCache.clear();
}
