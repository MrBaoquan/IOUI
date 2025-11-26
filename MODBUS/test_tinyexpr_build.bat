@echo off
REM 编译测试 TinyExpr

set CL_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\bin\Hostx64\x64\cl.exe"
set INCLUDE_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\include;C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt"
set LIB_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.42.34433\lib\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\ucrt\x64"

echo 编译 test_tinyexpr.c ...
%CL_PATH% /nologo /W3 /TC /EHsc ^
    /I"Source/Public/TinyExpr" ^
    /DWIN32_LEAN_AND_MEAN ^
    test_tinyexpr.c ^
    Source/Public/TinyExpr/tinyexpr.c ^
    /Fe:test_tinyexpr.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo 编译成功，运行测试...
    echo.
    test_tinyexpr.exe
) else (
    echo 编译失败！
)

pause
