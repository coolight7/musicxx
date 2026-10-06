@echo off
rem 拟声下载页本地预览：双击本文件即可启动，优先用 node，其次用 python
rem 也可以带参数：preview.bat 9000        指定端口
rem             preview.bat 9000 mock   用 tool/mock 里的快照数据（断网时用）
setlocal
cd /d "%~dp0"
title mimicry music download preview

where node >nul 2>nul
if %errorlevel%==0 (
    node serve.js %1 %2
    goto end
)

where python >nul 2>nul
if %errorlevel%==0 (
    python serve.py %1 %2
    goto end
)

where py >nul 2>nul
if %errorlevel%==0 (
    py -3 serve.py %1 %2
    goto end
)

echo [ERROR] Node.js or Python is required. Install one of them first.
pause

:end
endlocal
