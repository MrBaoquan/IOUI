 
rem PLATFORM:					Win32 |	Win64
rem CONFIGURE:					Debug | Release

rem	IOUI_SRC_PATH				IOUI-WIN[36][24]-*.dll
;rem IOUI_DST_PATH				IOUI-WIN[36][24]-*.dll

;rem IOUI_SRC_CORE_PATH			PCI*.dll			
;rem IOUI_DST_CORE_PATH			PCI*.dll			

;rem start execute command

@rem	Note: Write your command below this line

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-EM9008.dll %IOUI_DST_PATH% /y
copy %IOUI_SRC_CORE_PATH%EM9008.dll %IOUI_DST_CORE_PATH% /y
copy %IOUI_SRC_CORE_PATH%msvcp120.dll %IOUI_DST_CORE_PATH% /y
copy %IOUI_SRC_CORE_PATH%msvcr120.dll %IOUI_DST_CORE_PATH% /y
copy %IOUI_SRC_CORE_PATH%ZTLC_TCP.dll %IOUI_DST_CORE_PATH% /y

if %CONFIGURE% equ Debug (
	xcopy %ProjectDir%Config\EM9008 %IOUI_SRC_PATH%ExternalLibraries\Config\EM9008\ /E /F /Y
) else (
	xcopy %ProjectDir%Config\EM9008 %IOUI_DST_PATH%Config\EM9008\ /E /F /Y
)