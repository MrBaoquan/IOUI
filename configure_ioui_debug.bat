rem 为 IOUI_Debug  提供最新 IODevice

set platform=Win64

set env=Debug
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.dll .\Binaries\%platform%\%env%\ /Y
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.lib .\Binaries\%platform%\%env%\ /Y

set env=Release
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.dll .\Binaries\%platform%\%env%\ /Y
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.lib .\Binaries\%platform%\%env%\ /Y



set platform=Win32

set env=Debug
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.dll .\Binaries\%platform%\%env%\ /Y
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.lib .\Binaries\%platform%\%env%\ /Y

set env=Release
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.dll .\Binaries\%platform%\%env%\ /Y
copy .\..\IODevice\Binaries\%platform%\%env%\IODevice.lib .\Binaries\%platform%\%env%\ /Y
pause