@echo off 
setlocal enabledelayedexpansion

cd /d "%~dp0"

:: Generation de la documentation Cairn 
set "OPTION=release"
set "CLIENT=%~1"
set "CAIRN_DOC_ENV=%~2"

if "%CLIENT%"=="" (
	set "CLIENT=CEA"
)

set "CEA_CONTENT=False"

if /I "%CLIENT%"=="CEA" ( 
	set "CEA_CONTENT=True" 
)

set "OPEN=FALSE"

echo " - Configuring environment... "
call ..\..\GenericAppEnv.bat
echo " ... done"

if "%PYTHON_VENV%"=="" (
	set /p PYTHON_VENV=enter the path of the python environment in which Cairn Python API is installed:
)

echo " - Extracting RST parts from Cairn source code... "
call python "%CAIRN_APP%\scripts\doc\userDocGenRST.py" "%OPEN%"

if /I not "%CLIENT%"=="CEA" ( 
	rd /S /Q "%CAIRN_APP%\doc\user\privateDoc" 
)

echo "RST generation done"

call python "%CAIRN_APP%\doc\user\editConfig.py" "CEA=%CEA_CONTENT%,OPEN=%OPEN%"

:: Normalize variables by removing embedded quotes 
set "PYTHON_VENV=%PYTHON_VENV:"=%" 
set "CAIRN_DOC_ENV=%CAIRN_DOC_ENV:"=%"

echo PYTHON_VENV=%PYTHON_VENV%

:: Determine SPHINX_HOME
if exist "%PYTHON_VENV%" (
    set "SPHINX_HOME=%PYTHON_VENV%\Scripts\"
) else (
    echo %PYTHON_VENV% does not exist!
    if exist "%CAIRN_DOC_ENV%" (
        set "SPHINX_HOME=%CAIRN_DOC_ENV%\Scripts\"
    ) else (
        echo ERROR: No valid Python environment found.
        goto :end
    )
)

echo SPHINX_HOME=%SPHINX_HOME%

:: Install requirements
"%SPHINX_HOME%python.exe" -m pip install -r requirements.txt

:: Build htmlSphinx
set "result=htmlSphinx"
call "%SPHINX_HOME%sphinx-build.exe" -E -b html . "../%result%"

:end
endlocal