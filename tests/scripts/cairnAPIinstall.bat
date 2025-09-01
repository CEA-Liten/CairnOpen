
set CAIRNWHEEL_PATH=%1
if "%CAIRNWHEEL_PATH%"=="" (
	set CAIRNWHEEL_PATH=..\..\bin
)

set APPLI_PATH=%~dp0\..\..

rem read project version
for /f "tokens=2 delims=) " %%G in ('find /I "MAJOR_VERSION" ^< "%APPLI_PATH%\cmake\CairnVersion.cmake"') do set MAJOR_VERSION=%%G
for /f "tokens=2 delims=) " %%G in ('find /I "MINOR_VERSION" ^< "%APPLI_PATH%\cmake\CairnVersion.cmake"') do set MINOR_VERSION=%%G
for /f "tokens=2 delims=) " %%G in ('find /I "PATCH_VERSION" ^< "%APPLI_PATH%\cmake\CairnVersion.cmake"') do set PATCH_VERSION=%%G

set PROJECT_VERSION=%MAJOR_VERSION%.%MINOR_VERSION%.%PATCH_VERSION%


rem Activate Python environment
set PYTHON_HOME=C:\PythonPegase\3_10_9\python\
set PYTHON_VENV=%PYTHON_HOME%\..\envCairn%PROJECT_VERSION%

call %~dp0\\installEnv.bat %PYTHON_VENV%

python.exe -m pip uninstall -y cairn

echo - CAIRNWHEEL_PATH is %CAIRNWHEEL_PATH%

rem use the first file with extension *.whl
for /r  %CAIRNWHEEL_PATH% %%F in (*.whl) do (
	echo %%~F
	python.exe -m pip install %%~F
	goto stop
)
:stop

