@echo off
echo Starting NeuroShell...
echo.

cd build\bin\Release
neuroshell.exe

echo.
echo.
echo Neuroshell exited with code: %ERRORLEVEL%
echo.
pause
