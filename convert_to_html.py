#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
批量将 Config 目录下的 README.md 转换为带侧边导航的 HTML 文档
"""

import re
import os
import sys
from pathlib import Path

def parse_markdown_headers(md_content):
    """解析 Markdown 标题，生成目录结构"""
    headers = []
    lines = md_content.split('\n')
    
    for i, line in enumerate(lines):
        # 匹配标题：## 标题 或 ### 标题
        match = re.match(r'^(#{2,6})\s+(.+?)(?:\s+[🎯✨📋🔧⚙️🛡️💡📊🔍📝❓🚀💻⚠️✅🔴🆕📈🎉]+)?$', line)
        if match:
            level = len(match.group(1))  # 标题级别
            title = match.group(2).strip()
            # 生成锚点 ID
            anchor_id = re.sub(r'[^\w\u4e00-\u9fff]+', '-', title.lower()).strip('-')
            headers.append({
                'level': level,
                'title': title,
                'id': anchor_id,
                'line': i
            })
    
    return headers

def generate_toc_html(headers):
    """生成目录 HTML"""
    if not headers:
        return ""
    
    toc_html = ['<nav class="toc">']
    toc_html.append('<h2>📑 目录</h2>')
    toc_html.append('<ul>')
    
    current_level = 2
    for header in headers:
        level = header['level']
        
        # 处理层级变化
        if level > current_level:
            for _ in range(level - current_level):
                toc_html.append('<ul>')
        elif level < current_level:
            for _ in range(current_level - level):
                toc_html.append('</ul>')
        
        current_level = level
        
        # 添加目录项
        toc_html.append(f'<li><a href="#{header["id"]}">{header["title"]}</a></li>')
    
    # 关闭所有未关闭的列表
    for _ in range(current_level - 2):
        toc_html.append('</ul>')
    
    toc_html.append('</ul>')
    toc_html.append('</nav>')
    
    return '\n'.join(toc_html)

def convert_markdown_to_html(md_content, headers):
    """将 Markdown 转换为 HTML"""
    html_content = md_content
    
    # 为标题添加锚点
    for header in headers:
        pattern = r'^(#{' + str(header['level']) + r'})\s+' + re.escape(header['title']) + r'(?:\s+[🎯✨📋🔧⚙️🛡️💡📊🔍📝❓🚀💻⚠️✅🔴🆕📈🎉]+)?$'
        replacement = f'<h{header["level"]} id="{header["id"]}">{header["title"]}</h{header["level"]}>'
        html_content = re.sub(pattern, replacement, html_content, flags=re.MULTILINE)
    
    # 转换其他 Markdown 语法
    # 粗体
    html_content = re.sub(r'\*\*(.+?)\*\*', r'<strong>\1</strong>', html_content)
    
    # 代码块
    html_content = re.sub(r'```(\w+)?\n(.*?)\n```', lambda m: f'<pre><code class="language-{m.group(1) or ""}">{m.group(2)}</code></pre>', html_content, flags=re.DOTALL)
    
    # 行内代码
    html_content = re.sub(r'`(.+?)`', r'<code>\1</code>', html_content)
    
    # 列表项
    html_content = re.sub(r'^\s*[-*]\s+(.+)$', r'<li>\1</li>', html_content, flags=re.MULTILINE)
    
    # 包裹列表
    html_content = re.sub(r'(<li>.*?</li>\n?)+', r'<ul>\n\g<0></ul>\n', html_content, flags=re.MULTILINE)
    
    # 段落
    html_content = re.sub(r'\n\n', '</p>\n<p>', html_content)
    html_content = '<p>' + html_content + '</p>'
    
    # 清理多余的段落标签
    html_content = re.sub(r'<p>\s*</p>', '', html_content)
    html_content = re.sub(r'<p>(<h\d)', r'\1', html_content)
    html_content = re.sub(r'(</h\d>)</p>', r'\1', html_content)
    html_content = re.sub(r'<p>(<ul>)', r'\1', html_content)
    html_content = re.sub(r'(</ul>)</p>', r'\1', html_content)
    html_content = re.sub(r'<p>(<pre>)', r'\1', html_content)
    html_content = re.sub(r'(</pre>)</p>', r'\1', html_content)
    
    return html_content

def generate_html(md_file_path, output_file_path, doc_title=None):
    """生成完整的 HTML 文件"""
    
    try:
        # 读取 Markdown 文件
        with open(md_file_path, 'r', encoding='utf-8') as f:
            md_content = f.read()
        
        # 解析标题
        headers = parse_markdown_headers(md_content)
        
        # 生成目录
        toc_html = generate_toc_html(headers)
        
        # 转换内容
        content_html = convert_markdown_to_html(md_content, headers)
        
        # 如果没有指定标题，使用文件所在目录名
        if not doc_title:
            parent_dir = os.path.basename(os.path.dirname(md_file_path))
            doc_title = f"{parent_dir} 配置文档"
        
        # 生成完整的 HTML
        html_template = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{doc_title}</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        body {{
            font-family: "Microsoft YaHei", "Segoe UI", Arial, sans-serif;
            line-height: 1.6;
            color: #333;
            background: #f5f5f5;
        }}
        
        .container {{
            display: flex;
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            box-shadow: 0 0 20px rgba(0,0,0,0.1);
        }}
        
        /* 侧边导航栏 */
        .toc {{
            width: 280px;
            background: #2c3e50;
            color: white;
            padding: 20px;
            position: sticky;
            top: 0;
            height: 100vh;
            overflow-y: auto;
            flex-shrink: 0;
        }}
        
        .toc h2 {{
            color: #3498db;
            margin-bottom: 15px;
            font-size: 1.3em;
            border-bottom: 2px solid #3498db;
            padding-bottom: 10px;
        }}
        
        .toc ul {{
            list-style: none;
        }}
        
        .toc > ul {{
            margin-left: 0;
        }}
        
        .toc ul ul {{
            margin-left: 15px;
            margin-top: 5px;
        }}
        
        .toc li {{
            margin: 5px 0;
        }}
        
        .toc a {{
            color: #ecf0f1;
            text-decoration: none;
            display: block;
            padding: 5px 10px;
            border-radius: 4px;
            transition: all 0.3s;
        }}
        
        .toc a:hover {{
            background: #34495e;
            color: #3498db;
            transform: translateX(5px);
        }}
        
        /* 滚动条样式 */
        .toc::-webkit-scrollbar {{
            width: 6px;
        }}
        
        .toc::-webkit-scrollbar-track {{
            background: #34495e;
        }}
        
        .toc::-webkit-scrollbar-thumb {{
            background: #3498db;
            border-radius: 3px;
        }}
        
        /* 主内容区 */
        .content {{
            flex: 1;
            padding: 40px;
            overflow-y: auto;
        }}
        
        h1 {{
            color: #2c3e50;
            border-bottom: 3px solid #3498db;
            padding-bottom: 10px;
            margin-bottom: 30px;
            font-size: 2.5em;
        }}
        
        h2 {{
            color: #2c3e50;
            margin-top: 40px;
            margin-bottom: 20px;
            padding-bottom: 10px;
            border-bottom: 2px solid #ecf0f1;
            font-size: 1.8em;
        }}
        
        h3 {{
            color: #34495e;
            margin-top: 30px;
            margin-bottom: 15px;
            font-size: 1.4em;
        }}
        
        h4 {{
            color: #7f8c8d;
            margin-top: 20px;
            margin-bottom: 10px;
            font-size: 1.2em;
        }}
        
        p {{
            margin-bottom: 15px;
            text-align: justify;
        }}
        
        code {{
            background: #f8f9fa;
            padding: 2px 6px;
            border-radius: 3px;
            font-family: "Consolas", "Monaco", monospace;
            color: #e74c3c;
            font-size: 0.9em;
        }}
        
        pre {{
            background: #2c3e50;
            color: #ecf0f1;
            padding: 15px;
            border-radius: 5px;
            overflow-x: auto;
            margin: 20px 0;
            line-height: 1.5;
        }}
        
        pre code {{
            background: none;
            color: inherit;
            padding: 0;
        }}
        
        ul {{
            margin: 15px 0;
            padding-left: 30px;
        }}
        
        li {{
            margin: 8px 0;
        }}
        
        strong {{
            color: #2c3e50;
            font-weight: 600;
        }}
        
        /* 响应式设计 */
        @media (max-width: 768px) {{
            .container {{
                flex-direction: column;
            }}
            
            .toc {{
                width: 100%;
                height: auto;
                position: relative;
            }}
            
            .content {{
                padding: 20px;
            }}
        }}
        
        /* 打印样式 */
        @media print {{
            .toc {{
                display: none;
            }}
            
            .content {{
                padding: 0;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        {toc_html}
        <div class="content">
            {content_html}
        </div>
    </div>
    
    <script>
        // 平滑滚动
        document.querySelectorAll('.toc a').forEach(anchor => {{
            anchor.addEventListener('click', function (e) {{
                e.preventDefault();
                const target = document.querySelector(this.getAttribute('href'));
                if (target) {{
                    target.scrollIntoView({{
                        behavior: 'smooth',
                        block: 'start'
                    }});
                }}
            }});
        }});
        
        // 高亮当前章节
        const observer = new IntersectionObserver((entries) => {{
            entries.forEach(entry => {{
                if (entry.isIntersecting) {{
                    const id = entry.target.getAttribute('id');
                    document.querySelectorAll('.toc a').forEach(link => {{
                        link.style.background = '';
                        link.style.color = '#ecf0f1';
                    }});
                    const activeLink = document.querySelector(`.toc a[href="#${{id}}"]`);
                    if (activeLink) {{
                        activeLink.style.background = '#34495e';
                        activeLink.style.color = '#3498db';
                    }}
                }}
            }});
        }}, {{ threshold: 0.5 }});
        
        document.querySelectorAll('h2, h3, h4').forEach(heading => {{
            observer.observe(heading);
        }});
    </script>
</body>
</html>
"""
        
        # 写入 HTML 文件
        with open(output_file_path, 'w', encoding='utf-8') as f:
            f.write(html_template)
        
        return True
        
    except Exception as e:
        print(f"❌ 转换失败 [{md_file_path}]: {e}")
        return False

def find_readme_files(config_dir):
    """查找 Config 目录下所有子目录中的 README.md 文件"""
    readme_files = []
    config_path = Path(config_dir)
    
    if not config_path.exists():
        print(f"❌ Config 目录不存在: {config_dir}")
        return readme_files
    
    # 遍历 Config 目录下的所有子目录
    for subdir in config_path.iterdir():
        if subdir.is_dir():
            # 查找 README.md 文件（不区分大小写）
            for readme in subdir.glob('[Rr][Ee][Aa][Dd][Mm][Ee].[Mm][Dd]'):
                readme_files.append(readme)
    
    return readme_files

def batch_convert(config_dir):
    """批量转换 Config 目录下的所有 README.md"""
    print("🔍 开始扫描 Config 目录...")
    print(f"📂 扫描路径: {config_dir}\n")
    
    readme_files = find_readme_files(config_dir)
    
    if not readme_files:
        print("⚠️  未找到任何 README.md 文件")
        return
    
    print(f"✅ 找到 {len(readme_files)} 个 README.md 文件\n")
    
    success_count = 0
    fail_count = 0
    
    for readme_path in readme_files:
        # 生成 HTML 文件路径（与 README.md 在同一目录）
        html_path = readme_path.with_suffix('.html')
        
        # 获取驱动名称
        driver_name = readme_path.parent.name
        
        print(f"🔄 正在转换: {driver_name}/README.md")
        
        if generate_html(str(readme_path), str(html_path), f"{driver_name} 驱动配置文档"):
            file_size = os.path.getsize(html_path)
            print(f"   ✅ 成功 → {driver_name}/README.html ({file_size:,} 字节)\n")
            success_count += 1
        else:
            print(f"   ❌ 失败\n")
            fail_count += 1
    
    print("=" * 60)
    print(f"📊 转换完成统计:")
    print(f"   ✅ 成功: {success_count} 个")
    print(f"   ❌ 失败: {fail_count} 个")
    print(f"   📁 总计: {len(readme_files)} 个")
    print("=" * 60)

if __name__ == '__main__':
    # 获取项目根目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    config_dir = os.path.join(script_dir, 'Config')
    
    print("=" * 60)
    print("📚 README.md 批量转换为 HTML 工具")
    print("=" * 60)
    print()
    
    if len(sys.argv) > 1:
        # 如果提供了参数，使用参数作为 Config 目录路径
        config_dir = sys.argv[1]
    
    batch_convert(config_dir)
