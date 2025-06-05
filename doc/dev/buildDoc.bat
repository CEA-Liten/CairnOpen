echo off
set CMAKEPATH=C:/PROGRAM FILES/MICROSOFT VISUAL STUDIO/2022/COMMUNITY/COMMON7/IDE/COMMONEXTENSIONS/MICROSOFT/CMAKE/CMake/bin/
if exist "cmakepath.bat" (	
	call cmakepath.bat
)
echo %CMAKEPATH%

rem Generate config
"%CMAKEPATH%/cmake.exe" -DCMAKE_BUILD_TYPE=release -S . -B out/build/release/
rem build
"%CMAKEPATH%/cmake.exe" --build out/build/release/