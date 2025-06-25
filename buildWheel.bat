rem ~~~~~~~~~~~~~~~~~~~~~~~~~ Package Cairn API ~~~~~~~~~~~~~~~~~~~~~~
rem build and install Cairn wheel
rem %1 : CEA (all models), not CEA (private models not included)
rem %2 : output path, if nothing the 'bin' path
rem %3 : CPLEX path
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set APPLI_PATH=%~dp0
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CLIENT=%1
if "%CLIENT%" == "" (
set /p CLIENT=enter project name:
)
if not "%CLIENT%"=="CEA" (	
	set PROJECT_OPTION1=-DWITH_PRIVATEMODELS:BOOL=OFF
)
if "%CLIENT%"=="CEA" (
	set PROJECT_OPTION1=-DWITH_PRIVATEMODELS:BOOL=ON
)
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set EXPORT_PATH=%2
if "%EXPORT_PATH%" == "" (
set EXPORT_PATH=%APPLI_PATH%\bin
)
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem cas CPLEX
set CPLEX_PATH=%3
if "%CPLEX_PATH%" == "" (
set CPLEX_PATH=-DCPLEX_ROOT:STRING="C:/Program Files/IBM/ILOG/CPLEX_Studio201/cplex"
)

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

rem production du wheel
if exist %APPLI_PATH%\_skbuild (
	rmdir /s /q %APPLI_PATH%\_skbuild
	)

pip wheel . -w %EXPORT_PATH%
