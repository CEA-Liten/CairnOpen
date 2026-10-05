@ECHO off
rem =========================================================
rem
rem startCTest [<empty>=release|debug|fullrelease|fulldebug] 
rem	 		 [<empty>=level]			: if not empty run test in parallel, level : limit of parallelism
rem			 [<empty>=all tests|jsonTestFile]		 
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

rem ---------------------------------
set WORKSPACE=%~dp0
set REPORT=%WORKSPACE%\reports\CairnCtest-TNR
rem !! Warning: current dir must be Cairn root
rem init CAIRN_BIN and CAIRN_APP
if EXIST GenericAppEnv.bat (
	call GenericAppEnv.bat %CONFIGURATION%
) else (
	call ..\GenericAppEnv.bat %CONFIGURATION%
)
set TESTDIR=out/%CONFIGURATION%

set PARALLEL=%2

set FILETEST=%3
if "%FILETEST%"=="" (
	goto allTests
)
if NOT EXIST %FILETEST% (
	echo test Not found!
	exit /B 0
)

call %CAIRN_APP%\out\%CONFIGURATION%\bin\GenericTests.exe "--case:%FILETEST%"

goto endTests


:allTests

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

:endTests
