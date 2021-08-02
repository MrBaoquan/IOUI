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

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-PCI1020.dll %IOUI_DST_PATH% /y

if %PLATFORM% equ Win64 (
 copy %IOUI_SRC_CORE_PATH%PCI1020_64.DLL %IOUI_DST_CORE_PATH% /y
 copy %IOUI_SRC_CORE_PATH%PCI1020_low64.dll %IOUI_DST_CORE_PATH% /y
) else (
 copy %IOUI_SRC_CORE_PATH%PCI1020_32.DLL %IOUI_DST_CORE_PATH%	/y
 copy %IOUI_SRC_CORE_PATH%PCI1020_low32.dll %IOUI_DST_CORE_PATH% /y
)

if %CONFIGURE% equ Debug (
	xcopy %IOUI_SRC_PATH%Config\PCI1020 %IOUI_SRC_PATH%ExternalLibraries\Config\PCI1020\ /E /F /Y
) else (
	xcopy %IOUI_SRC_PATH%Config\PCI1020 %IOUI_DST_PATH%Config\PCI1020\ /E /F /Y
)