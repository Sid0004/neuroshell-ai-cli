@echo off
setlocal enabledelayedexpansion

cls
echo ========================================
echo   NeuroShell Build Script
echo ========================================
echo.

REM Check dependencies
echo [1/4] Checking dependencies...
echo.

set DEPS_OK=1

if not exist "thirdparty\imgui\imgui.cpp" (
    echo ERROR: ImGui not found
    set DEPS_OK=0
) else (
    echo [OK] ImGui
)

if not exist "thirdparty\glfw\CMakeLists.txt" (
    echo ERROR: GLFW not found
    set DEPS_OK=0
) else (
    echo [OK] GLFW
)

if not exist "thirdparty\glad\src\glad.c" (
    echo ERROR: GLAD not found
    set DEPS_OK=0
) else (
    echo [OK] GLAD
)

if not exist "thirdparty\json\include" (
    echo ERROR: nlohmann/json not found
    set DEPS_OK=0
) else (
    echo [OK] nlohmann/json
)

if "%DEPS_OK%"=="0" (
    echo.
    echo ========================================
    echo Missing dependencies!
    echo ========================================
    echo.
    pause
    exit /b 1
)

echo.
echo All dependencies found!
echo.

REM Clean build directory
echo [2/4] Preparing build directory...
echo.

if exist build (
    rd /s /q build 2>nul
)

mkdir build
cd build

REM Configure with CMake
echo [3/4] Configuring with CMake...
echo.

cmake -G "Visual Studio 17 2022" -A x64 ..

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo CMake configuration FAILED
    echo ========================================
    pause
    exit /b 1
)

REM Build
echo.
echo [4/4] Building NeuroShell...
echo.

cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo Build FAILED
    echo ========================================
    pause
    exit /b 1
)

REM Success
echo.
echo ========================================
echo   Build Successful!
echo ========================================
echo.
echo Executable: %CD%\bin\Release\neuroshell.exe
echo.
pause
