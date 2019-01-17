@echo off

set param1=%1%

if "%param1%" neq "Win64" (
	if "%param1%" neq "Win32" exit
)

cd ..
set src_root_path=%cd%\
set dst_root_path=%src_root_path%IOUIDebug\

set src_binaries_path=%src_root_path%Binaries\%param1%\Debug\
set dst_binaries_path=%dst_root_path%Binaries\%param1%\Debug\ExternalLibraries\

if not exist %dst_binaries_path% md %dst_binaries_path%
copy %src_binaries_path%IOUI-%param1%-IOUI.dll %dst_binaries_path% /y
