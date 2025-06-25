rem Install all packages from reqs_core.txt and reqs_TNR inside the virtual environment latest
rem Installation d'un python env au niveau du serveur ?
call pythonEnv.bat

python.exe -m venv %PYTHON_VENV%
call %PYTHON_VENV%\scripts\activate.bat
rem la ligne est effectuée dans pythonEnv

python.exe -m pip install -r %~dp0\reqs_core.txt 
python.exe -m pip install -r %~dp0\reqs_NRT.txt
python.exe -m pip install -r %~dp0\reqs_NRT_API.txt

rem TODO récupérer les wheels
rem %~dp0\latest%extension%\python.exe -m pip install --no-index --find-links=%~dp0\wheels -r %~dp0\reqs_core.txt
rem %~dp0\latest%extension%\python.exe -m pip install --no-index --find-links=%~dp0\wheels -r %~dp0\reqs_TNR.txt

