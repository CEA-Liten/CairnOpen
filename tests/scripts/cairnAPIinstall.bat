
set CAIRNWHEEL_PATH=%1
if "%CAIRNWHEEL_PATH%"=="" (
	set CAIRNWHEEL_PATH=..\..\bin
)

rem Activate Python environment
call %~dp0\\pythonEnv.bat

python.exe -m pip uninstall -y cairn

echo - CAIRNWHEEL_PATH is %CAIRNWHEEL_PATH%

rem use the first file with extension *.whl
for /r  %CAIRNWHEEL_PATH% %%F in (*.whl) do (
	echo %%~F
	python.exe -m pip install %%~F
	goto stop
)
:stop

