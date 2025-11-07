@echo off
REM Quick run script for NeuroShell

if not exist build\bin\Release\neuroshell.exe (
    echo ERROR: neuroshell.exe not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

cd build\bin\Release
start neuroshell.exe
