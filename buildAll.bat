@ECHO off
rem =========================================================
rem
rem buildAll [<empty>=release|debug|fullrelease|fulldebug] 
rem	 		 [<empty>=all|open]					: all=with private models, open=without
rem			 [<empty>|wheel|wheel-noinstall]	: wheel=build and install wheel, wheel-noinstall=build but no install
rem			 [<empty>|deps]			: deps=use dependencies installed in the directory D:/Tools/DepsCairn
rem			 [<empty>|envCairn]		: envCairn=use env python enCairn<Number> else use defaultoption
rem			 [<empty>|buildDoc]		: buildDoc=buil cairn documentation
rem		
rem ========================================================= 
SET STARTTIME=%TIME%
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

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
if "%WHEEL%"=="wheel-noinstall" (
	set OPTION_WHEEL=-DBUILD_WHEEL=ON -DINSTALL_WHEEL=OFF
)
echo %OPTION_WHEEL%

rem Input parameter: deps
set INSTALLDEPS=%4
set OPTION_DEPS=
if "%INSTALLDEPS%"=="deps" (
	set OPTION_DEPS=-DDEPS_INSTALL=ON -DDEPS_ROOT:STRING=D:/Tools/DepsCairn
)
echo %OPTION_DEPS%

rem Input parameter: envCairn
set USE_ENVCAIRN=%5
set OPTION_ENVCAIRN=
if "%USE_ENVCAIRN%"=="envCairn" (
	set OPTION_ENVCAIRN=-DUSE_ENVCAIRN=ON
)
echo %OPTION_ENVCAIRN%

rem ========================================================= 

rem remove build directory
set BUILD_PATH=out\%CONFIGURATION%
if exist %BUILD_PATH% (
    rmdir /s /q "%BUILD_PATH%"
)
mkdir "%BUILD_PATH%"

rem ========================================================= 

rem Generate config
"%CMAKEPATH%\cmake.exe" -G "Ninja" --preset=%CONFIGURATION% %OPTION_DEPS% %OPTION_WHEEL% %OPTION_PRIVATE% %OPTION_ENVCAIRN% -S . 

rem build 
"%CMAKEPATH%\cmake.exe" --build --preset %CONFIGURATION%  

rem remove previous install directory
set BIN_PATH=bin\%CONFIGURATION%
if exist %BIN_PATH% (
	rmdir /s /q "%BIN_PATH%"
)
	
rem Install
"%CMAKEPATH%\cmake.exe" --install %BUILD_PATH% --prefix %BIN_PATH%

rem ========================================================= 
rem Input parameter: buildDoc
set BUILD_DOCCAIRN=%6
if "%BUILD_DOCCAIRN%"=="buildDoc" (	
	"%CMAKEPATH%\cmake.exe" -G "Ninja" --preset=buildDoc %OPTION_PRIVATE% -S . 
)



