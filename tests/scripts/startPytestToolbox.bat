set OPTION=%1
if "%OPTION%" == "" set OPTION=release
echo %OPTION%

set MARKER=%~2

set XDIST=%3

set REPORT=%4
if "%REPORT%" == "" set REPORT=Cairn-TNR_toolbox.xml
echo %REPORT%

set REPORT_UPDATE=Cairn-TNR_update.txt

set SCRIPT_DIR=%~dp0

set TESTED_DIR=%5
if "%TESTED_DIR%" == "" set TESTED_DIR=%SCRIPT_DIR%\..\
echo Using test dir "%TESTED_DIR%"

set SCRIPT_REPORT=%6 
if "%SCRIPT_REPORT%" == "" set SCRIPT_REPORT=htmlReportLste.py
echo %SCRIPT_REPORT%

set HTML_ARG=%7
if "%HTML_ARG%" == "" set HTML_ARG="tests\reports\Cairn-TNR_toolbox"

set HTML_REPORT=%8
if "%HTML_REPORT%" == "" set HTML_REPORT="YES"

set REPORT_OLD=%REPORT:~0,-4%_old.xml

REM suppression de l'ancien fichier contenant des references a mettre a jour
del %TESTED_DIR%\*_update.txt

REM suppression de l'ancien html rapport de tests
del %TESTED_DIR%\%REPORT:~0,-4%.html

set PYTHONPATH=%SCRIPT_DIR%

REM Activation de l'environnement Python

call %~dp0\\pythonEnv.bat %PYTHON_VENV%

REM Activation d'uranie

powershell D:\Uranie_Binaries\833-47843-venv\uranie\bin\thisroot.ps1

cd /D %TESTED_DIR%
echo Executing pytest in "%CD%"
echo Using %PYTHON_VENV%

if "%XDIST%" == "" set XDIST=0

echo "%MARKER%"
if "%MARKER%" == "" goto :no_marker
if not "%MARKER%" == "" goto :marker

:marker	
if "%MARKER%" == "" goto :no_marker

if %XDIST% == 0 (
pytest -p no:faulthandler -m "%MARKER%" --junitxml %REPORT% 
) else (
pytest -p no:faulthandler -n %XDIST% --dist=loadgroup -m "%MARKER%" --junitxml %REPORT% 
)

echo "in marker"
echo "%HTML_REPORT%"
if %HTML_REPORT% == "YES" goto :html
goto:end

:no_marker

if %XDIST% == 0 (
pytest -p no:faulthandler --junitxml %REPORT% 
) else (
pytest -p no:faulthandler -n %XDIST% --dist=loadgroup --junitxml %REPORT% 
)
if %HTML_REPORT% == "YES" goto :html
goto:end

:html

echo "move"
move %SCRIPT_DIR%\..\%REPORT_UPDATE% reports\
echo "in html"
python -u %SCRIPT_DIR%\%SCRIPT_REPORT% %HTML_ARG%

echo "junit2html"
pip install junit2html
junit2html %REPORT% %SCRIPT_DIR%\..\reports\Cairn-TNR_toolbox-log.html

:end
echo "ending"
