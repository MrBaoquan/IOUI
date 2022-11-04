goto start

rem PLATFORM:					Win32 |	Win64
rem CONFIGURE:					Debug | Release

rem	IOUI_SRC_PATH				IOUI-WIN[36][24]-*.dll	ԴĿ¼
rem IOUI_DST_PATH				IOUI-WIN[36][24]-*.dll	Ŀ��Ŀ¼

rem IOUI_SRC_CORE_PATH			PCI*.dll			ԴĿ¼
rem IOUI_DST_CORE_PATH			PCI*.dll			Ŀ��Ŀ¼

rem start execute command
:start
@rem	Note: Write your command below this line

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-SERIALPORT.dll %IOUI_DST_PATH% /y

if %CONFIGURE% equ Debug (
	xcopy %ProjectDir%Config\SERIALPORT %IOUI_SRC_PATH%ExternalLibraries\Config\SERIALPORT\ /E /F /Y
) else (
	xcopy %ProjectDir%Config\SERIALPORT %IOUI_DST_PATH%Config\SERIALPORT\ /E /F /Y
)