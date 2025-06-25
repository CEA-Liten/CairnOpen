set PATH_init=%PATH%
set test_dir=%~dp0 
call ../../GenericAppEnv.bat
cd /D ./scripts
set OPTION=fullrelease
set TestCase=
set /p TestCase=repertoire a refaire (vide=tous):


rm */*Sortie.csv
rm */*/*Sortie.csv
rm */*/*/*Sortie.csv

rm */*model.lp
rm */*/*model.lp
rm */*/*/*model.lp

startPytest.bat %OPTION% Cairn Pegase-TNR_Persee.xml %~dp0\%TestCase%

set PATH="%PATH_init%"

exit /B
