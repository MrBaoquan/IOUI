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

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-PULSE_1020.dll %IOUI_DST_PATH% /y

if %PLATFORM% equ Win64 (
 copy %IOUI_SRC_CORE_PATH%PCI1020_64.DLL %IOUI_DST_CORE_PATH% /y
 copy %IOUI_SRC_CORE_PATH%PCI1020_low64.dll %IOUI_DST_CORE_PATH% /y
) else (
 copy %IOUI_SRC_CORE_PATH%PCI1020_32.DLL %IOUI_DST_CORE_PATH%	/y
 copy %IOUI_SRC_CORE_PATH%PCI1020_low32.dll %IOUI_DST_CORE_PATH% /y
)

rem if %CONFIGURE% equ Debug (
rem 	xcopy %ProjectDir%Config\PULSE_1020 %IOUI_SRC_PATH%ExternalLibraries\Config\PULSE_1020\ /E /F /Y
rem ) else (
rem 	xcopy %ProjectDir%Config\PULSE_1020 %IOUI_DST_PATH%Config\PULSE_1020\ /E /F /Y
rem )