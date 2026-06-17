
set NUM_VERSION=%1

rem check version input
for /F "tokens=1,2,3 delims=." %%a in ("%NUM_VERSION%") do (
    set "major=%%a"
    set "minor=%%b" 
    set "patch=%%c"
)
echo %major%| findstr /r "^[1-9][0-9]*$">nul
echo %minor%| findstr /r "^[1-9][0-9]*$">nul
echo %patch%| findstr /r "^[1-9][0-9]*$">nul

if %errorlevel% equ 0 (
    echo "Valid version number"
) else (   
    echo "error invalid version number"
    exit /b 1
)

rem adding version
echo %NUM_VERSION% >> %~dp0/officialVersions.txt

rem update version
set CMAKE_VER_FILE=%~dp0/../cmake/CairnVersion.cmake
echo # --- set current version --- > %CMAKE_VER_FILE%
echo set(MAJOR_VERSION %major%) >> %CMAKE_VER_FILE%
echo set(MINOR_VERSION %minor%) >> %CMAKE_VER_FILE%
echo set(PATCH_VERSION %patch%) >> %CMAKE_VER_FILE%
