@echo off
echo =============================================
echo        SCARRY EXECUTOR BUILD SCRIPT
echo =============================================
echo.

if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

echo Configuring with CMake...
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" ..

if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building project...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo =============================================
echo Build completed successfully!
echo.
echo Output files:
echo - ScarryExecutor.exe (in build/bin/Release/)
echo - ScarryHook.dll (in build/bin/Release/)
echo.
echo Copy these files to your Roblox executor directory.
echo =============================================

pause