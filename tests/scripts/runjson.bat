rem @echo off
color 3F
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
rem This script allows to call Persee StdAlone
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

set OPTION=release

set ARCH_FILE=%1
set DATASERIES_FILE=%2
::set MYSTEPFILE_FILE=%3

if "%1" == "" set /p ARCH_FILE=
if "%2" == "" set /p DATASERIES_FILE=

echo "ARCH_FILE=%ARCH_FILE%"
echo "DATASERIES_FILE=%DATASERIES_FILE%"
::echo "MYSTEPFILE_FILE=%MYSTEPFILE_FILE%"

::In case of several input time series files (needed for run sensitivity when launched from GUI)
for /f "tokens=2,* delims= " %%a in ("%*") do set REMAINING_ARGS=%%b
echo "Remaining arguments=%REMAINING_ARGS%"

if exist AppEnv.bat (
	call .\AppEnv.bat
) else (
	call ..\..\GenericAppEnv.bat
)

echo running %ARCH_FILE%

::if "%MYSTEPFILE_FILE%"=="" (
::	%CAIRN_BIN%\PerseeCmd.exe %ARCH_FILE% %DATASERIES_FILE% 
::)else (
::	%CAIRN_BIN%\PerseeCmd.exe %ARCH_FILE% %DATASERIES_FILE% "-TimeStepFile %MYSTEPFILE_FILE%"
::)

::Remaining arguments can be: Timeseries, TimeStepFile, or TypicalPeriodsFile 
::TimeStepFile.csv should be preceded by -TimeStepFile, or simply named as ${something}TimeStepFile.csv
::Similarly for TypicalPeriodsFile
%CAIRN_BIN%\CairnCmd.exe %ARCH_FILE% %DATASERIES_FILE% %REMAINING_ARGS%

echo End of %ARCH_FILE%

