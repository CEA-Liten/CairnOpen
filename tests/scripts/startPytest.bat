set OPTION=%1
if "%OPTION%" == "" set OPTION=release
echo %OPTION%

set MARKER=%~2

set REPORT=%3
if "%REPORT%" == "" set REPORT=Cairn-TNR.xml
echo %REPORT%

set REPORT_UPDATE=Cairn-TNR_update.txt

set SCRIPT_DIR=%~dp0

set TESTED_DIR=%4
if "%TESTED_DIR%" == "" set TESTED_DIR=%SCRIPT_DIR%\..\
echo Using test dir "%TESTED_DIR%"

set HTML_ARG=%5
if "%HTML_ARG%" == "" set HTML_ARG="tests\reports\Cairn-TNR"

REM call with BUILD_STEP=CHECK to skip running all tests
set BUILD_STEP=%6

set HTML_REPORT=%7
if "%HTML_REPORT%" == "" set HTML_REPORT="YES"



set REPORT_OLD=%REPORT:~0,-4%_old.xml
REM on ne met pas a jour le rapport de test xml Junit dans le cas d'un CHECK (sans RUN)
if not "%BUILD_STEP%" == "CHECK" (
del /F %TESTED_DIR%\%REPORT_OLD%
copy %TESTED_DIR%\%REPORT% %TESTED_DIR%\%REPORT_OLD%
)

REM suppression de l'ancien fichier contenant des references a mettre a jour
del %TESTED_DIR%\*_update.txt

REM suppression de l'ancien html rapport de tests
del %TESTED_DIR%\%REPORT:~0,-4%.html

set PYTHONPATH=%SCRIPT_DIR%

REM Activation de l'environnement Python
call %~dp0\\pythonEnv.bat


cd /D %TESTED_DIR%
echo Executing pytest in "%CD%"

echo "%MARKER%"
if "%MARKER%" == "" goto :no_marker
if not "%MARKER%" == "" goto :marker


:marker	
if "%MARKER%" == "''" goto :no_marker
python -m pytest  -m "%MARKER%" --junitxml %REPORT% --ignore=%SCRIPT_DIR%\..\..\export\MIPModeler\external\HiGHS
echo "in marker"
echo "%HTML_REPORT%"
if %HTML_REPORT% == "YES" goto :html
goto:end

:no_marker

python -m pytest --junitxml %REPORT% --ignore=%SCRIPT_DIR%\..\..\export\MIPModeler\external\HiGHS
if %HTML_REPORT% == "YES" goto :html
goto:end

:html

echo "in html"
move %SCRIPT_DIR%\..\%REPORT_UPDATE% reports\
python -u %SCRIPT_DIR%\htmlReportLste.py %HTML_ARG%
	
:end
echo "ending"
