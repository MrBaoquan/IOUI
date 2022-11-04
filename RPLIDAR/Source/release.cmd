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

@rem copy %src_binaries_path%IOUI-%platform%-IOUI.dll %dst_binaries_path% /y

copy %IOUI_SRC_PATH%IOUI-%PLATFORM%-RPLIDAR.dll %IOUI_DST_PATH% /y

copy %IOUI_SRC_CORE_PATH%rplidar_driver.dll %IOUI_DST_CORE_PATH% /y
copy %IOUI_SRC_CORE_PATH%opencv_world440.dll %IOUI_DST_CORE_PATH% /y

if %CONFIGURE% equ Debug (
	xcopy %ProjectDir%Config\RPLIDAR %IOUI_SRC_PATH%ExternalLibraries\Config\RPLIDAR\ /E /F /Y
) else (
	xcopy %ProjectDir%Config\RPLIDAR %IOUI_DST_PATH%Config\RPLIDAR\ /E /F /Y
)
