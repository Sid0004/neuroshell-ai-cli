@echo off
echo ========================================
echo NeuroShell AI CLI - Build Script
echo ========================================
echo.

REM Check if build directory exists
if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)

REM Create build directory
echo Creating build directory...
mkdir build
cd build

REM Run CMake
echo.
echo Running CMake...
cmake ..

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo Make sure CMake is installed and in your PATH.
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building NeuroShell...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: build\bin\Release\neuroshell.exe
echo.
echo To run:
echo   cd build\bin\Release
echo   neuroshell.exe
echo.
pause
