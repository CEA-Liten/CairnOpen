rem echo off
set CMAKEPATH=C:/PROGRAM FILES/MICROSOFT VISUAL STUDIO/2022/COMMUNITY/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/
if exist "cmakepath.bat" (	
	call cmakepath.bat
)
echo %CMAKEPATH%

set OPTION=%1
if "%OPTION%"=="" (
 	set OPTION=release
)
set TESTDIR=%2
if "%TESTDIR%"=="" (
 	set TESTDIR=out/%OPTION%
)

set WORKSPACE=%~dp0

call GenericAppEnv.bat %OPTION%

rem "%CMAKEPATH%/ctest.exe" -I 7,7 -C release --test-dir out/release -V
set BUILD_TESTING=on

echo %BUILD_TESTING%

"%CMAKEPATH%/ctest.exe" --preset %OPTION% --test-dir %TESTDIR% --output-junit %WORKSPACE%\tests\reports\CairnCtest-TNR.xml

rem convert to html
pip install junit2html
junit2html  %WORKSPACE%\tests\reports\CairnCtest-TNR.xml %WORKSPACE%\tests\reports\CairnCtest-TNR.html

rem force script to return code 0
exit /B 0
