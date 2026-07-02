::@echo off
set OLD_PATH=%PATH%

set TNR_DIR=%1
set TNR_XML=%2
set RUN_DIR=%3

if exist %TNR_DIR%\%TNR_XML% (
    copy /Y %TNR_DIR%\%TNR_XML% .
) else (
   echo "ERROR Could not find %TNR_DIR%\%TNR_XML%
)

set "RUNFILE=%~dp0"

cd "%RUN_DIR%"
set APP_HOME=%RUN_DIR%
set PATH=%APP_HOME%lib;%PATH%
call %RUNFILE%\..\..\GenericAppEnv.bat fullrelease
echo exist %RUNFILE%\..\..\GenericAppEnv.bat
if "%USE_FBSF_FULL_BATCH%" == "true" (
    echo "Using FBSF Full batch "
    if exist %PEGASE_OUT%\FbsfBatch.exe (        
        REM new Fbsf version with full batch mode
         %PEGASE_OUT%\FbsfBatch.exe %TNR_XML%
    ) else (
        echo "[FATAL] FbsfBatch.exe not found"
    )
) else if exist %PEGASE_OUT%\FbsfFramework.exe (
    REM new Fbsf version
    echo %PEGASE_OUT%\FbsfFramework.exe %TNR_XML% -no-gui
    %PEGASE_OUT%\FbsfFramework.exe %TNR_XML% -no-gui
) else (
	echo " ***** ERROR : No FbsfFramework.exe found ***** "
        echo " folder pegase
	exit /B -1
)

set PATH=%OLD_PATH%