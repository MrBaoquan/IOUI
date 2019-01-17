@echo off
set VERSION=v1.0.0
set PLATFORM=%1%
set CONFIGURE=%2%

if "%PLATFORM%" neq "Win64" (
	if "%PLATFORM%" neq "Win32" exit
)
cd ..

set SOLUTION_ROOT_PATH=%cd%\

if %CONFIGURE% equ Debug (
	set dst_root_path=%SOLUTION_ROOT_PATH%IOUIDebug\Binaries\%PLATFORM%\Debug\
) else (
	set dst_root_path=%SOLUTION_ROOT_PATH%IOUIDebug\%VERSION%\IOToolkit\
)
if not exist %dst_root_path% md %dst_root_path%

set IOUI_SRC_PATH=%SOLUTION_ROOT_PATH%Binaries\%PLATFORM%\Release\
set IOUI_SRC_CORE_PATH=%IOUI_SRC_PATH%Libraries\

if %CONFIGURE% equ Debug (
	set IOUI_DST_ROOT_PATH=%dst_root_path%\
) else (
	set IOUI_DST_ROOT_PATH=%dst_root_path%binaries\%PLATFORM%\
)
if not exist %IOUI_DST_ROOT_PATH% md %IOUI_DST_ROOT_PATH%


set IOUI_DST_PATH=%IOUI_DST_ROOT_PATH%ExternalLibraries\
if not exist %IOUI_DST_PATH% md %IOUI_DST_PATH%


set IOUI_DST_CORE_PATH=%IOUI_DST_PATH%Core\
if not exist %IOUI_DST_CORE_PATH% md %IOUI_DST_CORE_PATH%

call %3%Source\release.cmd
