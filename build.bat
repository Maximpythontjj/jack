@echo off
echo Building SCARRY EXECUTOR...

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo Executable location: build\bin\Release\SCARRY_EXECUTOR.exe
echo DLL location: build\bin\Release\ScarryInternal.dll

pause