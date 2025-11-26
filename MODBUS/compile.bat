@echo off
echo 正在编译 MODBUS 项目（Debug x64）...
echo.

cd /d "%~dp0"

"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "MODBUS.vcxproj" /p:Configuration=Debug /p:Platform=x64 /t:Build /v:minimal /nologo

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   编译成功！
    echo ========================================
    echo.
) else (
    echo.
    echo ========================================
    echo   编译失败，错误代码: %ERRORLEVEL%
    echo ========================================
    echo.
)

pause
