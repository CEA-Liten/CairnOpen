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

cd "%RUN_DIR%"
call .\AppEnv.bat
if "%USE_FBSF_FULL_BATCH%" == "true" (
    echo "Using FBSF Full batch "
    if exist %FBSF_BIN%\FbsfBatch.exe (        
        REM new Fbsf version with full batch mode
         %FBSF_BIN%\FbsfBatch.exe %TNR_XML%
    ) else (
        echo "[FATAL] FbsfBatch.exe not found"
    )
) else if exist %FBSF_BIN%\FbsfFramework.exe (
    REM new Fbsf version
    FbsfFramework.exe %TNR_XML% -no-gui
) else (
	echo " ***** ERROR : No FbsfFramework.exe found ***** "
	exit /B -1
)

set PATH=%OLD_PATH%