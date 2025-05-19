@echo off
setlocal
set build_type=%1
mkdir cmakebuilds
cd %~dp0/cmakebuilds
@REM call "D:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" MinSizeRel
@REM call "E:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17" ..
cmake --build . --config %build_type%
time /t
pause
endlocal