@ECHO off
rem =========================================================
rem
rem startCTest [<empty>=release|debug|fullrelease|fulldebug] 
rem			 [<empty>|tests path]	
rem	 		 [<empty>=level]			: if not empty run test in parallel, level : limit of parallelism
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

set TESTDIR=%2
if "%TESTDIR%"=="" (
 	set TESTDIR=out/%CONFIGURATION%
)

set PARALLEL=%3

rem ---------------------------------

set WORKSPACE=%~dp0
call GenericAppEnv.bat %CONFIGURATION%

rem "%CMAKEPATH%/ctest.exe" -I 7,7 -C release --test-dir out/release -V

if "%PARALLEL%"=="" (
	"%CMAKEPATH%/ctest.exe" --preset %CONFIGURATION% --test-dir %TESTDIR% --output-junit %WORKSPACE%\tests\reports\CairnCtest-TNR.xml
) else (
	echo Parallel: %PARALLEL%
	"%CMAKEPATH%/ctest.exe" --preset %CONFIGURATION% -j %PARALLEL% --test-dir %TESTDIR% --output-junit %WORKSPACE%\tests\reports\CairnCtest-TNR.xml
)

rem convert to html
pip install junit2html
junit2html  %WORKSPACE%\tests\reports\CairnCtest-TNR.xml %WORKSPACE%\tests\reports\CairnCtest-TNR.html

rem temp
copy %WORKSPACE%\tests\reports\CairnCtest-TNR.html %WORKSPACE%\tests\reports\CairnCtest-TNR-log.html

rem force script to return code 0
rem exit /B 0
