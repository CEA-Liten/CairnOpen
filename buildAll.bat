@ECHO off
SET STARTTIME=%TIME%

set CMAKEPATH=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin
if exist "cmakepath.bat" (	
	call cmakepath.bat
)
echo %CMAKEPATH%

rem Input parameter: debug, release
set CONFIGURATION=%1
if "%CONFIGURATION%"=="" (
	set CONFIGURATION=release
)
echo %CONFIGURATION%

rem Input parameter: all, open
set OPTION=%2
if "%OPTION%"=="" (
	set OPTION=all
)
echo %OPTION%

rem remove build directory
set BUILD_PATH=out\%CONFIGURATION%
if exist %BUILD_PATH% (
    rmdir /s /q "%BUILD_PATH%"
)
mkdir "%BUILD_PATH%"


rem Generate config
if "%OPTION%"=="all" ( 
	"%CMAKEPATH%\cmake.exe" --preset=%CONFIGURATION% -S . 
) else (
    "%CMAKEPATH%\cmake.exe" --preset=%CONFIGURATION% -DWITH_PRIVATEMODELS=OFF -DWITH_LICENCE=OFF -DCAIRN_DEFAULTSOLVER:STRING=Highs -S . 
)

rem build  -j %NUMBER_OF_PROCESSORS%
"%CMAKEPATH%\cmake.exe" --build --preset %CONFIGURATION%  


rem remove previous install directory
set BIN_PATH=bin\%CONFIGURATION%
if exist %BIN_PATH% (
	rmdir /s /q "%BIN_PATH%"
)
	
rem Install
"%CMAKEPATH%\cmake.exe" --install %BUILD_PATH% --prefix %BIN_PATH%


del %~dp0\bin\%CONFIGURATION%\bin\Qt5Core.dll

