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

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-DAM3000M.dll %IOUI_DST_PATH% /y

if %CONFIGURE% equ Debug (
	xcopy %ProjectDir%Config\DAM3000M %IOUI_SRC_PATH%ExternalLibraries\Config\DAM3000M\ /E /F /Y
) else (
	xcopy %ProjectDir%Config\DAM3000M %IOUI_DST_PATH%Config\DAM3000M\ /E /F /Y
)

if %PLATFORM% equ Win64 (
 copy %IOUI_SRC_CORE_PATH%DAM3000M_64.dll %IOUI_DST_CORE_PATH% /y
) else (
 copy %IOUI_SRC_CORE_PATH%DAM3000M_32.dll %IOUI_DST_CORE_PATH%	/y
)
