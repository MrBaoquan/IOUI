@echo off

set param1=%1%

if "%param1%" neq "Win64" (
	if "%param1%" neq "Win32" exit
)

cd ..
set src_root_path=%cd%\
set dst_root_path=%src_root_path%IOUIDebug\

set src_binaries_path=%src_root_path%Binaries\%param1%\Debug\
set dst_binaries_path=%dst_root_path%Binaries\%param1%\Debug\

if not exist %dst_binaries_path%ExternalLibraries\ md %dst_binaries_path%ExternalLibraries\
copy %src_binaries_path%IOUI-%param1%-PCI2312A.dll %dst_binaries_path%ExternalLibraries\ /y

if not exist %dst_binaries_path%ExternalLibraries\Core\ md %dst_binaries_path%ExternalLibraries\Core\

if "%param1%"=="Win64" (
 copy %src_binaries_path%Libraries\PCI2312A_64.dll %dst_binaries_path%ExternalLibraries\Core\
) else (
 copy %src_binaries_path%Libraries\PCI2312A_32.dll %dst_binaries_path%ExternalLibraries\Core\
)
