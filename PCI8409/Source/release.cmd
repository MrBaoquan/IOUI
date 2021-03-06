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



copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-PCI8409.dll %IOUI_DST_PATH% /y
copy %IOUI_SRC_CORE_PATH%pci8409.dll %IOUI_DST_CORE_PATH% /y

echo %IOUI_SRC_PATH%Config\PCI8409 
if %CONFIGURE% equ Debug (
	xcopy %IOUI_SRC_PATH%Config\PCI8409 %IOUI_SRC_PATH%ExternalLibraries\Config\ /E /F /Y
) else (
	xcopy %IOUI_SRC_PATH%Config\PCI8409 %IOUI_DST_PATH%Config\PCI8409\ /E /F /Y
)