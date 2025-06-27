cd /d "%~dp0"

REM Manage python environment
set PYTHON_HOME=C:/PythonPegase/3_10_9/python/

set PYTHON_ENV=%1

if "%CLIENT%" == "" (
set /p PYTHON_ENV=enter the path of the python environment in which Cairn Python API is installed:
)
if "%CLIENT%" == "" goto :build

set SPHINX_HOME=%PYTHON_ENV%/Scripts/

%SPHINX_HOME%python.exe -m pip install -r requirements.txt

:build
REM rd /s /q ..\..\%result%
set result=htmlSphinx
call %SPHINX_HOME%sphinx-build.exe -E -b html . ../%result%
