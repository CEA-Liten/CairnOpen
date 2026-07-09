@ECHO off
rem =========================================================
rem
rem startCTest [<empty>=release|debug|fullrelease|fulldebug] 
rem	 		 [<empty>=level]			: if not empty run test in parallel, level : limit of parallelism
rem			 [<empty>|tests path]		 
rem		
rem ========================================================= 

set CMAKEPATH=C:/PROGRAM FILES/MICROSOFT VISUAL STUDIO/2022/COMMUNITY/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/
if exist "cmakepath.bat" (	
	call cmakepath.bat
)

echo CMake: %CMAKEPATH%

rem Input parameters: 
rem ---------------------------------
set CONFIGURATION=%1
if "%CONFIGURATION%"=="" (
	set CONFIGURATION=release
)
echo Configuration: %CONFIGURATION%

set PARALLEL=%2

set TESTDIR=%3
if "%TESTDIR%"=="" (
 	set TESTDIR=out/%CONFIGURATION%
)

rem ---------------------------------
set WORKSPACE=%~dp0
set REPORT=%WORKSPACE%\reports\CairnCtest-TNR
rem !! Warning: current dir must be Cairn root
call GenericAppEnv.bat %CONFIGURATION%

rem "%CMAKEPATH%/ctest.exe" -I 7,7 -C release --test-dir out/release -V

if "%PARALLEL%"=="" (
	"%CMAKEPATH%/ctest.exe" --preset %CONFIGURATION% --test-dir %TESTDIR% --output-junit %REPORT%.xml
) else (
	echo Parallel: %PARALLEL%
	"%CMAKEPATH%/ctest.exe" --preset %CONFIGURATION% -j %PARALLEL% --test-dir %TESTDIR% --output-junit %REPORT%.xml
)

rem convert to html
junit2html %REPORT%.xml %REPORT%.html

rem temp
copy %REPORT%.html %REPORT%-log.html

rem force script to return code 0
rem exit /B 0
