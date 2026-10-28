# -*- coding: utf-8 -*-
"""Render the blog draft to docs/index.html so it can be served by GitHub Pages."""
import os
import re

import markdown

HERE = os.path.dirname(os.path.abspath(__file__))
DOCS = os.path.abspath(os.path.join(HERE, '..'))
MD = os.path.join(DOCS, '博客-STM32F103-uCOSII-AHT20.md')
OUT = os.path.join(DOCS, 'index.html')

CSS = """
:root { --blue:#2b6cb0; --ink:#1f2933; --muted:#5b6b7c; }
* { box-sizing: border-box; }
body { margin:0; background:#eef2f7; color:var(--ink);
       font-family:"Microsoft YaHei","PingFang SC",system-ui,sans-serif; line-height:1.85; }
header { background:linear-gradient(135deg,#1a365d,#2b6cb0); color:#fff; padding:56px 20px 44px; }
header .wrap { max-width:900px; margin:0 auto; }
header .kicker { opacity:.8; letter-spacing:2px; font-size:13px; }
header h1 { margin:10px 0 12px; font-size:30px; line-height:1.4; }
header .meta { opacity:.85; font-size:14px; }
main { max-width:900px; margin:-26px auto 40px; background:#fff; border-radius:12px;
       padding:38px 46px 50px; box-shadow:0 10px 30px rgba(16,42,67,.12); }
h2 { color:var(--blue); font-size:22px; margin:34px 0 12px; padding-left:12px;
     border-left:5px solid var(--blue); }
h3 { color:#2c5282; font-size:18px; margin:26px 0 10px; }
p { margin:12px 0; }
a { color:var(--blue); }
img { max-width:100%; height:auto; display:block; margin:20px auto; border:1px solid #dbe3ec;
      border-radius:8px; }
code { background:#f2f5f8; color:#b03060; padding:2px 6px; border-radius:4px;
       font-family:Consolas,monospace; font-size:.92em; }
pre { background:#0f172a; color:#e2e8f0; padding:16px 18px; border-radius:8px;
      overflow-x:auto; line-height:1.55; font-size:13.5px; }
pre code { background:none; color:inherit; padding:0; }
table { width:100%; border-collapse:collapse; margin:18px 0; font-size:14px; }
th, td { border:1px solid #cdd8e5; padding:8px 10px; text-align:left; vertical-align:top; }
th { background:#eef3fa; }
blockquote { margin:16px 0; padding:10px 16px; background:#f7fafc;
             border-left:4px solid #cbd5e0; color:#3d4852; }
footer { max-width:900px; margin:0 auto 50px; color:var(--muted); font-size:13px;
         text-align:center; }
@media (max-width:640px){ main{padding:24px 18px; margin:-18px 10px 30px;} header h1{font-size:23px;} }
"""


def main():
    src = open(MD, encoding='utf-8').read()
    # the page already shows the title in <header>; drop the duplicate H1
    src = re.sub(r'^#\s+.*?\n', '', src, count=1, flags=re.M)
    body = markdown.markdown(
        src,
        extensions=['tables', 'fenced_code', 'sane_lists', 'nl2br'],
        output_format='html5',
    )
    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>STM32F103C8T6 移植 uC/OS-II 并接入 AHT20</title>
<style>{CSS}</style>
</head>
<body>
<header><div class="wrap">
  <div class="kicker">STM32 · RTOS · I²C</div>
  <h1>STM32F103C8T6 移植 uC/OS-II 并接入 AHT20：从 PendSV 到 I²C 的完整踩坑记录</h1>
  <div class="meta">蒋俊熙 · 2026-10-28 · 嵌入式实时操作系统课程作业</div>
</div></header>
<main>
{body}
</main>
<footer>
  工程源码：<a href="https://github.com/jkahwbdu/stm32-rtos-aht20-task">github.com/jkahwbdu/stm32-rtos-aht20-task</a>
</footer>
</body>
</html>
"""
    open(OUT, 'w', encoding='utf-8', newline='\n').write(html)
    print('written', OUT)


if __name__ == '__main__':
    main()
