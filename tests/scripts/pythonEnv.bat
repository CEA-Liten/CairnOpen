rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for Python
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set PYTHON_HOME=C:\PythonPegase\3_10_9\python\
set PYTHON_LIB=python310.lib

set PYTHON_VENV=%1
if "%PYTHON_VENV%"=="" (
	set PYTHON_VENV=%PYTHON_HOME%\..\envPegase
)

echo python environment is %1

call %PYTHON_VENV%\scripts\deactivate.bat

set PATH=%PYTHON_HOME%;%PATH%

rem bien mettre cette ligne après le 'set PATH' (sinon problème de définition du PATH)
call %PYTHON_VENV%\scripts\activate.bat

echo - PYTHON_HOME is %PYTHON_HOME%
echo - PYTHON_VENV is %PYTHON_VENV%
echo - PATH is %PATH%
