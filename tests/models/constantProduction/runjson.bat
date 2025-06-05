rem @echo off
color 3F
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Ce script permet de démarrer QtCreator avec les bonnes variables
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd %~dp0

::set extension=_debug
set OPTION=debug

set PERSEE_ROOT=..\..

set tnrdir=%1
set ARCH_FILE=%2
set RAD_FILE=%~n2
set EXT_FILE=%~x2
set DATASERIES_FILE=%3
set OUTPUT_FILE=%4

set EXT2=""

cd %~dp0

if "%EXT_FILE%" == ".json" set EXT2=".json"
if "%EXT_FILE%" == ".xml" set RAD_FILE=%RAD_FILE:_desc=%

if "%2" == "" set ARCH_FILE=constant_production.json
if "%3" == "" set DATASERIES_FILE=constant_production_dataseries.csv
if "%4" == "" set OUTPUT_FILE=%RAD_FILE%

call %PERSEE_ROOT%\DepsEnv.bat

echo "ARCH_FILE=%ARCH_FILE%"
echo "DATASERIES_FILE=%DATASERIES_FILE%"
echo "OUTPUT_FILE=%OUTPUT_FILE%"

cd %~dp0

if exist %OUTPUT_FILE%_Results.csv del %OUTPUT_FILE%_Results.csv
del TNR\%OUTPUT_FILE%_Sortie.csv%EXT%

REM Run json file
echo running %ARCH_FILE%

%PERSEE_ROOT%\bin\stdalone\%OPTION%\PerseeApp.exe %~dp0\%ARCH_FILE% %~dp0\%DATASERIES_FILE%

copy %RAD_FILE%_Results.csv TNR\%OUTPUT_FILE%_Sortie.csv%EXT2%
copy %RAD_FILE%_Settings.ini %OUTPUT_FILE%_Settings.ini

echo End of %ARCH_FILE%
:: not possible as Pegase does not have same csv output format as std alone computation 
:: check that .json yield exact same result as _desc.xml
::if exist TNR\%OUTPUT_FILE%_StdAlone-Reference.csv FC TNR\%OUTPUT_FILE%_Sortie.csv%EXT2% TNR\%OUTPUT_FILE%_StdAlone-Reference.csv

