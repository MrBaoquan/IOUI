goto start

rem PLATFORM:					Win32 |	Win64
rem CONFIGURE:					Debug | Release

rem	IOUI_SRC_PATH				IOUI-WIN[36][24]-*.dll	源目录
rem IOUI_DST_PATH				IOUI-WIN[36][24]-*.dll	目标目录

rem IOUI_SRC_CORE_PATH			PCI*.dll			源目录
rem IOUI_DST_CORE_PATH			PCI*.dll			目标目录

rem start execute command
:start
@rem	Note: Write your command below this line

@rem copy %src_binaries_path%IOUI-%platform%-IOUI.dll %dst_binaries_path% /y

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-RPLIDAR.dll %IOUI_DST_PATH% /y
