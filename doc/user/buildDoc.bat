cd /d "%~dp0"

:: Generation de la documentation Cairn 
cd %~dp0

set OPTION=release
set CLIENT=%1
if "%CLIENT%"=="" (
	set CLIENT=CEA
)
set CEA_CONTENT=False
if %CLIENT%==CEA (
	set CEA_CONTENT=True
)
set OPEN=FALSE

echo " - Configuring environment... "

call ..\..\GenericAppEnv.bat

echo " ... done"

REM Manage python environment
set PYTHON_HOME=C:/PythonPegase/3_10_9/python/

set PYTHON_ENV=%2

if "%PYTHON_ENV%" == "" (
set /p PYTHON_ENV=enter the path of the python environment in which Cairn Python API is installed:
)

echo " - Extracting RST parts from Cairn source code... "

call python %CAIRN_APP%\scripts\doc\userDocGenRST.py %OPEN%

if not "%CLIENT%"=="CEA" (	
	rd /S /Q %CAIRN_APP%\doc\user\privateDoc
)

echo "RST generation done"

call python %CAIRN_APP%\doc\user\editConfig.py "CEA=%CEA_CONTENT%,OPEN=%OPEN%"

if "%PYTHON_ENV%" == "" goto :build

set SPHINX_HOME=%PYTHON_ENV%/Scripts/

%SPHINX_HOME%python.exe -m pip install -r requirements.txt

:build
set result=htmlSphinx
call %SPHINX_HOME%sphinx-build.exe -E -b html . ../%result%
