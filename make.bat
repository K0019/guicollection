@echo off
SETLOCAL EnableDelayedExpansion

:: Define the build directory name
set BUILD_DIR=build

:: 1. Create the build directory if it doesn't exist
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
    echo Created directory: %BUILD_DIR%
)

:: 2. Navigate to the build directory
cd %BUILD_DIR%

:: 3. Run CMake configuration
echo [INFO] Configuring the project...
cmake ..

pause