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

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-ENCODER-BH38M.dll %IOUI_DST_PATH% /y

if %CONFIGURE% equ Debug (
	xcopy %ProjectDir%Config\ENCODER-BH38M %IOUI_SRC_PATH%ExternalLibraries\Config\ENCODER-BH38M\ /E /F /Y
) else (
	xcopy %ProjectDir%Config\ENCODER-BH38M %IOUI_DST_PATH%Config\ENCODER-BH38M\ /E /F /Y
)