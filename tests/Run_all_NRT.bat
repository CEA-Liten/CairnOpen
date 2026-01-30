set PATH_init=%PATH%
set test_dir=%~dp0 
set OPTION=fullrelease

call ..\GenericAppEnv.bat %OPTION% ..\virtualPy
cd /D .\scripts

set XDIST=%1
if "%XDIST%" == "" set XDIST=''

set TestCase=
set /p TestCase=repertoire a refaire (vide=tous):

startPytest.bat %OPTION% Cairn %XDIST% Pegase-TNR_Persee.xml %~dp0\%TestCase%

set PATH="%PATH_init%"

exit /B
