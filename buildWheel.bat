rem ~~~~~~~~~~~~~~~~~~~~~~~~~ Package Cairn API ~~~~~~~~~~~~~~~~~~~~~~
rem build and install Cairn wheel
rem %1 : CEA (all models), not CEA (private models not included)
rem %2 : installation ("DEFAULT", "NO", "", "directory"), 
rem                if "NO" no install, 
rem                   "DEFAULT" install in envCairn<version>
rem                   "" create install in 'virtualPy'
rem %3 : output path, if nothing the 'bin' path
rem %4 : CPLEX path
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set APPLI_PATH=%~dp0
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CLIENT=%1
if "%CLIENT%" == "" (
set CLIENT=CEA
)
if not "%CLIENT%"=="CEA" (	
	set PROJECT_OPTION1=-DWITH_PRIVATEMODELS:BOOL=OFF
)
if "%CLIENT%"=="CEA" (
	set PROJECT_OPTION1=-DWITH_PRIVATEMODELS:BOOL=ON
)
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set EXPORT_PATH=%3
if "%EXPORT_PATH%" == "" (
set EXPORT_PATH=%APPLI_PATH%\bin
)
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem cas CPLEX
set CPLEX_PATH=%4
if "%CPLEX_PATH%" == "" (
set CPLEX_PATH=-DCPLEX_ROOT:STRING="C:/Program Files/IBM/ILOG/CPLEX_Studio201/cplex"
)

set PYTHON_HOME=-DPYTHON_HOME:STRING="C:/PythonPegase/3_10_9/python"
set PYTHON_VENV=-DPYTHON_VENV:STRING="C:/PythonPegase/3_10_9/envPegase"
set pybind11_DIR=-Dpybind11_DIR:STRING="C:/PythonPegase/3_10_9/envPegase/Lib/site-packages/pybind11/share/cmake/pybind11"



rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem read project version
for /f "tokens=2 delims=) " %%G in ('find /I "MAJOR_VERSION" ^< "%APPLI_PATH%\cmake\CairnVersion.cmake"') do set MAJOR_VERSION=%%G
for /f "tokens=2 delims=) " %%G in ('find /I "MINOR_VERSION" ^< "%APPLI_PATH%\cmake\CairnVersion.cmake"') do set MINOR_VERSION=%%G
for /f "tokens=2 delims=) " %%G in ('find /I "PATCH_VERSION" ^< "%APPLI_PATH%\cmake\CairnVersion.cmake"') do set PATCH_VERSION=%%G

set PROJECT_VERSION=%MAJOR_VERSION%.%MINOR_VERSION%.%PATCH_VERSION%


rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem prepare setup.py
powershell -Command "(gc %APPLI_PATH%\cmake\setup.py.in) -replace '@PROJECT_VERSION@', '%PROJECT_VERSION%' | sc %APPLI_PATH%\setup.py"
powershell -Command "(gc %APPLI_PATH%\setup.py) -replace '@PROJECT_OPTION1@', '%PROJECT_OPTION1%' | sc %APPLI_PATH%\setup.py"
powershell -Command "(gc %APPLI_PATH%\setup.py) -replace '@PROJECT_OPTION2@', '%CPLEX_PATH%' | sc %APPLI_PATH%\setup.py"
powershell -Command "(gc %APPLI_PATH%\setup.py) -replace '@PROJECT_OPTION3@', '%PYTHON_HOME%' | sc %APPLI_PATH%\setup.py"
powershell -Command "(gc %APPLI_PATH%\setup.py) -replace '@PROJECT_OPTION4@', '%PYTHON_VENV%' | sc %APPLI_PATH%\setup.py"
powershell -Command "(gc %APPLI_PATH%\setup.py) -replace '@PROJECT_OPTION5@', '%pybind11_DIR%' | sc %APPLI_PATH%\setup.py"

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem production du wheel
if exist %APPLI_PATH%\_skbuild (
	rmdir /s /q %APPLI_PATH%\_skbuild
	)

pip wheel . -w %EXPORT_PATH%

rem del %EXPORT_PATH%\fullrelease\bin\Qt5Core.dll

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem installation in virtual environment 
set PYTHON_VENV=%2
set PYTHON_HOME=C:\PythonPegase\3_10_9\python\
if "%PYTHON_VENV%" == "NO" (
	goto stop
)
if "%PYTHON_VENV%" == "DEFAULT" (	
	set PYTHON_VENV=%PYTHON_HOME%\..\envCairn%PROJECT_VERSION%
)
if "%PYTHON_VENV%" == "" (
	set PYTHON_VENV=%APPLI_PATH%\virtualPy
)

call %~dp0\\\tests\\scripts\\installEnv.bat %PYTHON_VENV%

python.exe -m pip uninstall -y cairn

echo - CAIRNWHEEL_PATH is %EXPORT_PATH%

rem use the first file with extension *.whl
for /r  %EXPORT_PATH% %%F in (*.whl) do (
	echo %%~F
	python.exe -m pip install %%~F
	goto stop
)
:stop
