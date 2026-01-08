@ECHO off
SET STARTTIME=%TIME%
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

rem Input parameter: debug, release
set CONFIGURATION=%1
if "%CONFIGURATION%"=="" (
	set CONFIGURATION=release
)
echo %CONFIGURATION%

rem Input parameter: all, open
set OPTION=%2
set OPTION_PRIVATE=
if "%OPTION%"=="" (
	set OPTION=all
)
if "%OPTION%"=="all" ( 
	set OPTION_PRIVATE=-DWITH_PRIVATEMODELS=ON
) else (
	set OPTION_PRIVATE=-DWITH_PRIVATEMODELS=OFF -DWITH_LICENCE=OFF -DCAIRN_DEFAULTSOLVER:STRING=Highs
)
echo %OPTION_PRIVATE%

rem Input parameter: wheel
set WHEEL=%3
set OPTION_WHEEL=-DBUILD_WHEEL=OFF -DINSTALL_WHEEL=OFF
if "%WHEEL%"=="wheel" (
	set OPTION_WHEEL=-DBUILD_WHEEL=ON -DINSTALL_WHEEL=ON
)
echo %OPTION_WHEEL%

rem Input parameter: deps
set INSTALLDEPS=%4
set OPTION_DEPS=
if "%INSTALLDEPS%"=="deps" (
	set OPTION_DEPS=-DDEPS_INSTALL=ON -DDEPS_ROOT:STRING=D:/Tools/DepsCairn
)
echo %OPTION_DEPS%


rem remove build directory
set BUILD_PATH=out\%CONFIGURATION%
if exist %BUILD_PATH% (
    rmdir /s /q "%BUILD_PATH%"
)
mkdir "%BUILD_PATH%"

rem Generate config
cmake -G "Ninja" --preset=%CONFIGURATION% %OPTION_DEPS% %OPTION_WHEEL% %OPTION_PRIVATE% -S . 

rem build 
cmake --build --preset %CONFIGURATION%  

rem remove previous install directory
set BIN_PATH=bin\%CONFIGURATION%
if exist %BIN_PATH% (
	rmdir /s /q "%BIN_PATH%"
)
	
rem Install
cmake --install %BUILD_PATH% --prefix %BIN_PATH%

