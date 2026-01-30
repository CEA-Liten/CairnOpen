@ECHO off
SET STARTTIME=%TIME%

set CMAKEPATH=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin
if exist "cmakepath.bat" (	
	call cmakepath.bat
)
echo %CMAKEPATH%

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CLIENT=%1
if "%CLIENT%" == "" (
set CLIENT=CEA
)
if not "%CLIENT%"=="CEA" (	
	set PROJECT_OPTION1=-DWITH_PRIVATEMODELS:BOOL=OFF
)
if "%CLIENT%"=="CEA" (
	set PROJECT_OPTION1=-DWITH_PRIVATEMODELS:BOOL=ON
)

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CONFIGURATION=Release

rem remove build directory
set BUILD_PATH=out\wheel
if exist %BUILD_PATH% (
    rmdir /s /q "%BUILD_PATH%"
)
mkdir "%BUILD_PATH%"

rem Generate config
"%CMAKEPATH%\cmake.exe" --preset=wheel  %PROJECT_OPTION1% -S . 
	
rem Install
set BIN_PATH=bin\%CONFIGURATION%
"%CMAKEPATH%\cmake.exe" --install %BUILD_PATH% --prefix %BIN_PATH%

