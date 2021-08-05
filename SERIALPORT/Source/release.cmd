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

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-SERIALPORT.dll %IOUI_DST_PATH% /y

if %CONFIGURE% equ Debug (
	xcopy %IOUI_SRC_PATH%Config\SERIALPORT %IOUI_SRC_PATH%ExternalLibraries\Config\SERIALPORT\ /E /F /Y
) else (
	xcopy %IOUI_SRC_PATH%Config\SERIALPORT %IOUI_DST_PATH%Config\SERIALPORT\ /E /F /Y
)