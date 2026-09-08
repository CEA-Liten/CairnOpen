@ECHO off
rem =========================================================
rem
rem updateCTest jsonTestFile
rem 	[<empty>=release|debug|fullrelease|fulldebug] 
rem		
rem ========================================================= 

rem Input parameters: 
rem ---------------------------------
set FILETEST=%1
if "%FILETEST%"=="" (
	echo No test defined!
	exit /B 0
)
if NOT EXIST %FILETEST% (
	echo test Not found!
	exit /B 0
)


set CONFIGURATION=%2
if "%CONFIGURATION%"=="" (
	set CONFIGURATION=release
)
echo Configuration: %CONFIGURATION%


rem ---------------------------------
rem init CAIRN_BIN and CAIRN_APP
if EXIST GenericAppEnv.bat (
	call GenericAppEnv.bat %CONFIGURATION%
) else (
	call ..\GenericAppEnv.bat %CONFIGURATION%
)

rem ---------------------------------
call %CAIRN_APP%\out\%CONFIGURATION%\bin\GenericTests.exe "--case:%FILETEST%" --update
