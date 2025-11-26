# 📚 README.md 批量转换工具

## 📖 功能说明

将 `Config` 目录下所有子目录中的 `README.md` 文件批量转换为带侧边导航的 HTML 文档。

## ✨ 特性

- ✅ **自动扫描**: 自动查找 Config 目录下所有子目录的 README.md
- ✅ **侧边导航**: 生成的 HTML 包含可点击的目录导航
- ✅ **美观样式**: 深色导航栏 + 白色内容区，响应式设计
- ✅ **平滑滚动**: 点击目录项平滑滚动到对应章节
- ✅ **批量处理**: 一次转换所有文档

## 🚀 使用方法

### 方法 1: 默认转换（推荐）

在项目根目录执行：

```cmd
python convert_to_html.py
```

自动转换 `Config` 目录下的所有 README.md 文件。

### 方法 2: 指定 Config 目录

```cmd
python convert_to_html.py C:\自定义路径\Config
```

转换指定路径下的 README.md 文件。

## 📂 输出位置

生成的 HTML 文件与对应的 README.md 在同一目录：

```
Config/
├── MODBUS/
│   ├── README.md       # 源文件
│   └── README.html     # 生成的 HTML ✅
├── IOHUB/
│   ├── README.md       # 源文件
│   └── README.html     # 生成的 HTML ✅
└── ...
```

## 📊 转换示例

```
============================================================
📚 README.md 批量转换为 HTML 工具
============================================================

🔍 开始扫描 Config 目录...
📂 扫描路径: O:\DevModules\IODevice\IOUI\Config

✅ 找到 2 个 README.md 文件

🔄 正在转换: IOHUB/README.md
   ✅ 成功 → IOHUB/README.html (24,189 字节)

🔄 正在转换: MODBUS/README.md
   ✅ 成功 → MODBUS/README.html (38,263 字节)

============================================================
📊 转换完成统计:
   ✅ 成功: 2 个
   ❌ 失败: 0 个
   📁 总计: 2 个
============================================================
```

## 🎨 HTML 文档特性

### 侧边导航
- 固定在左侧，始终可见
- 自动提取所有 2-6 级标题
- 支持多级嵌套目录
- 悬停高亮效果

### 内容区域
- Markdown 语法自动转换
- 代码块深色主题
- 响应式布局
- 适配打印样式

### 交互功能
- 平滑滚动定位
- 当前章节高亮
- 移动端友好

## 📝 支持的 Markdown 语法

- ✅ 标题（H2-H6）
- ✅ 粗体 `**文本**`
- ✅ 行内代码 `` `代码` ``
- ✅ 代码块 ` ```语言 ` 
- ✅ 无序列表
- ✅ 段落

## 🔧 技术栈

- Python 3.x
- 纯正则表达式转换
- 无需第三方依赖

## 📄 文件说明

- `convert_to_html.py` - 批量转换主脚本（项目根目录）
- `Config/*/README.md` - 源 Markdown 文档
- `Config/*/README.html` - 生成的 HTML 文档

## 🌐 查看 HTML

生成后可以通过以下方式打开：

1. 直接双击 HTML 文件
2. 右键选择浏览器打开
3. 在 VS Code 中安装 Live Server 扩展预览

## ⚠️ 注意事项

- 确保 README.md 文件编码为 UTF-8
- 支持文件名大小写不敏感（README.md / readme.md / ReadMe.md）
- 如果转换失败，检查 Markdown 语法是否正确

## 📮 当前支持的驱动

根据 Config 目录结构，目前支持：

- ✅ MODBUS - Modbus RTU/TCP 驱动
- ✅ IOHUB - IO Hub 驱动
- 📝 其他驱动可添加 README.md 后转换

---

**更新时间**: 2025-10-29  
**版本**: 1.0
