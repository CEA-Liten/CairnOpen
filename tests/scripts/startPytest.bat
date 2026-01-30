set OPTION=%1
if "%OPTION%" == "" set OPTION=release
echo %OPTION%

set MARKER=%~2

set XDIST=%3

set REPORT=%4
if "%REPORT%" == "" set REPORT=Cairn-TNR.xml
echo %REPORT%

set REPORT_UPDATE=Cairn-TNR_update.txt

set SCRIPT_DIR=%~dp0

set TESTED_DIR=%5
if "%TESTED_DIR%" == "" set TESTED_DIR=%SCRIPT_DIR%\..\
echo Using test dir "%TESTED_DIR%"

set HTML_ARG=%6
if "%HTML_ARG%" == "" set HTML_ARG="tests\reports\Cairn-TNR"

set HTML_REPORT=%7
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

if "%XDIST%" == "''" set XDIST=0

echo "%MARKER%"
if "%MARKER%" == "" goto :no_marker
if not "%MARKER%" == "" goto :marker

:marker	
if "%MARKER%" == "''" goto :no_marker

if %XDIST% == 0 (
pytest -m "%MARKER%" --junitxml %REPORT% --ignore=%SCRIPT_DIR%\..\..\export\MIPModeler\external\HiGHS
) else (
pytest -n %XDIST% -m "%MARKER%" --junitxml %REPORT% --ignore=%SCRIPT_DIR%\..\..\export\MIPModeler\external\HiGHS
)
echo "in marker"
echo "%HTML_REPORT%"
if %HTML_REPORT% == "YES" goto :html
goto:end

:no_marker

if %XDIST% == 0 (
pytest --junitxml %REPORT% --ignore=%SCRIPT_DIR%\..\..\export\MIPModeler\external\HiGHS
) else (
pytest -n %XDIST% --junitxml %REPORT% --ignore=%SCRIPT_DIR%\..\..\export\MIPModeler\external\HiGHS
)
if %HTML_REPORT% == "YES" goto :html
goto:end

:html

echo "in html"
move %SCRIPT_DIR%\..\%REPORT_UPDATE% reports\
python -u %SCRIPT_DIR%\htmlReportLste.py %HTML_ARG%
	
:end
echo "ending"
