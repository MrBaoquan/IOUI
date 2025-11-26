@echo off
REM 编译 MODBUS 项目 - 预编译表达式系统
echo ========================================
echo  MODBUS 项目编译脚本
echo ========================================
echo.

REM 设置 Visual Studio 环境
set VSWHERE="C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
  set VS_PATH=%%i
)

if not defined VS_PATH (
    echo 错误: 未找到 Visual Studio 安装路径
    pause
    exit /b 1
)

echo Visual Studio 路径: %VS_PATH%
echo.

REM 调用 MSBuild
set MSBUILD="%VS_PATH%\MSBuild\Current\Bin\MSBuild.exe"

if not exist %MSBUILD% (
    echo 错误: 未找到 MSBuild.exe
    pause
    exit /b 1
)

echo 开始编译 Debug x64...
%MSBUILD% "MODBUS.vcxproj" /p:Configuration=Debug /p:Platform=x64 /v:minimal /m

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo  编译失败！错误代码: %ERRORLEVEL%
    echo ========================================
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo  编译成功！
echo ========================================
echo.
echo 输出文件: ..\Binaries\Win64\Debug\MODBUS.dll
echo.
pause
