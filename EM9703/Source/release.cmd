@echo off

set param1=%1%

if "%param1%" neq "Win64" (
	if "%param1%" neq "Win32" exit
)

set src_root_path=E:\VisualStudioProjects\2015\IOUI\
set dst_root_path=G:\DevelopHelper\IOToolkit\

set src_binaries_path=%src_root_path%Binaries\%param1%\Release\

set dst_binaries_path=%dst_root_path%binaries\%param1%\
set dst_config_path=%dst_binaries_path%ExternalLibraries\EM9703\

if not exist %dst_binaries_path% md %dst_binaries_path%
copy %src_binaries_path%IOUI-%param1%-EM9703.dll %dst_binaries_path%ExternalLibraries\ /y
copy %src_binaries_path%Libraries\EM9703.dll %dst_binaries_path% /y

if not exist %dst_config_path% md %dst_config_path%
copy %src_binaries_path%EM9703\config.ini %dst_config_path%