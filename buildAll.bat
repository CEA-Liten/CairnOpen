@ECHO off
SET STARTTIME=%TIME%

set CMAKEPATH=C:/PROGRAM FILES/MICROSOFT VISUAL STUDIO/2022/COMMUNITY/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/
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
set BUILD_PATH=out/%CONFIGURATION%
if exist %BUILD_PATH% (
    rmdir /s /q "%BUILD_PATH%"
    )
mkdir "%BUILD_PATH%"


rem Generate config
REM if "%OPTION%"=="all" ( 
	REM "%CMAKEPATH%/cmake.exe" --preset=%CONFIGURATION% -S . 
REM ) else (
    REM "%CMAKEPATH%/cmake.exe" --preset=%CONFIGURATION% -DWITH_PRIVATEMODELS=OFF -DWITH_LICENCE=OFF -DCAIRN_DEFAULTSOLVER:STRING=Highs -S . 
REM )

rem build  -j %NUMBER_OF_PROCESSORS%
REM "%CMAKEPATH%/cmake.exe" --build --preset %CONFIGURATION%  


rem remove previous install directory
set BIN_PATH=bin\%CONFIGURATION%
REM if exist %BIN_PATH% (
	REM rmdir /s /q "%BIN_PATH%"
	REM )
	
rem Install
REM "%CMAKEPATH%/cmake.exe" --install %BUILD_PATH% --prefix %BIN_PATH%


del %BIN_PATH%\bin\Qt5Core.dll

