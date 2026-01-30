rem Install all packages from reqs_tests.txt inside the virtual environment latest
rem Installation d'un python env au niveau du serveur ?
call %~dp0\\pythonEnv.bat %1

python.exe -m venv %PYTHON_VENV%
call %PYTHON_VENV%\scripts\activate.bat
rem la ligne est effectuée dans pythonEnv
python.exe -m pip install -r %~dp0\reqs_tests.txt

