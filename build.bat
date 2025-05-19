@echo off
setlocal
set build_type=%1
mkdir cmakebuilds
cd %~dp0/cmakebuilds
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17" ..
cmake --build . --config %build_type%
time /t
pause
endlocal
