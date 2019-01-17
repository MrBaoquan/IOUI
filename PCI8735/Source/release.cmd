@echo off

set param1=%1%

if "%param1%" neq "Win64" (
	if "%param1%" neq "Win32" exit
)

cd ..
set src_root_path=%cd%\
set dst_root_path=%src_root_path%IOUIDebug\IOToolkit\

set src_binaries_path=%src_root_path%Binaries\%param1%\Release\
set dst_binaries_path=%dst_root_path%binaries\%param1%\

set core_path=%dst_binaries_path%ExternalLibraries\Core\

if not exist %core_path% md %core_path%

copy %src_binaries_path%IOUI-%param1%-PCI8735.dll %dst_binaries_path%ExternalLibraries\ /y
if "%param1%"=="Win64" (
 copy %src_binaries_path%Libraries\PCI8735_64.dll %dst_binaries_path%ExternalLibraries\Core\
) else (
 copy %src_binaries_path%Libraries\PCI8735_32.dll %dst_binaries_path%ExternalLibraries\Core\
)