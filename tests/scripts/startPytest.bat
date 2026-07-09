@ECHO off
rem =========================================================
rem
rem startPyTest [<empty>=release|debug|fullrelease|fulldebug] 
rem			 [<empty>|marker]	
rem	 		 [<empty>|level]			: if not empty run test in parallel, level : limit of parallelism
rem			 [<empty>|tests path]	
rem		
rem ========================================================= 


rem Input parameters: 
rem ---------------------------------
set CONFIGURATION=%1
if "%CONFIGURATION%"=="" (
	set CONFIGURATION=release
)
echo Configuration: %CONFIGURATION%

set MARKER=%~2
echo marker: "%MARKER%"

set PARALLEL=%3
if "%PARALLEL%"=="" (
	set PARALLEL=0
)

set WORKSPACE=%~dp0
echo rootdir: %WORKSPACE%

set TESTDIR=%4
if "%TESTDIR%"=="" (
 	set TESTDIR=%WORKSPACE%..
)
echo Using test dir "%TESTDIR%"


rem ---------------------------------
set REPORT=%TESTDIR%\reports\CairnPytest-TNR.xml
rem !! Warning: current dir must be Cairn root
call GenericAppEnv.bat %CONFIGURATION%
call ..\GenericAppEnv.bat %CONFIGURATION%

cd /D %TESTDIR%

if "%MARKER%"=="" goto :no_marker
if not "%MARKER%"=="" goto :marker

:marker	
if "%PARALLEL%"=="0" (
	pytest -p no:faulthandler -m "%MARKER%" --junitxml %REPORT% "%TESTDIR%"
) else (
	echo Parallel: %PARALLEL%
	pytest -p no:faulthandler -n %PARALLEL% --dist=loadgroup -m "%MARKER%" --junitxml %REPORT% "%TESTDIR%"
)
goto:end

:no_marker
if "%PARALLEL%"=="0" (
	pytest -p no:faulthandler --junitxml "%REPORT%" "%TESTDIR%"
) else (
	echo Parallel: %PARALLEL%
	pytest -p no:faulthandler -n %PARALLEL% --dist=loadgroup --junitxml %REPORT% "%TESTDIR%"
)
goto:end

:end

rem convert to html
junit2html  %REPORT% %TESTDIR%\reports\CairnPytest-TNR.html


echo "ending"
