@echo off
REM CyberRecon Suite Build Script v1.7 - Windows

echo 🛡️  Building CyberRecon Suite v1.7 - Modern Cybersecurity Platform...

REM Check if Qt6 is available
echo [INFO] Checking Qt6 installation...
where qmake6 >nul 2>&1
if %errorlevel% neq 0 (
    where qmake >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] Qt6 not found. Please install Qt6 development packages.
        echo.
        echo Installation:
        echo Windows: Download from https://www.qt.io/download
        echo Make sure Qt6 bin directory is in your PATH environment variable
        exit /b 1
    )
)

REM Get Qt version
for /f "tokens=*" %%i in ('qmake6 -query QT_VERSION 2^>nul') do set QT_VERSION=%%i
if "%QT_VERSION%"=="" (
    for /f "tokens=*" %%i in ('qmake -query QT_VERSION 2^>nul') do set QT_VERSION=%%i
)

if not "%QT_VERSION%"=="" (
    echo [SUCCESS] Found Qt version: %QT_VERSION%
) else (
    echo [WARNING] Could not determine Qt version
)

REM Check if CMake is available
echo [INFO] Checking CMake installation...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found. Please install CMake 3.16 or higher.
    echo.
    echo Installation:
    echo Windows: Download from https://cmake.org/download/
    exit /b 1
)

for /f "tokens=3" %%i in ('cmake --version ^| findstr /R "cmake version"') do set CMAKE_VERSION=%%i
echo [SUCCESS] Found CMake version: %CMAKE_VERSION%

REM Create build directory
echo [INFO] Creating build directory...
if exist build (
    echo [WARNING] Build directory exists. Cleaning...
    rmdir /s /q build
)
mkdir build
cd build

REM Configure with CMake
echo [INFO] Configuring with CMake...

REM Get Qt installation prefix
for /f "tokens=*" %%i in ('qmake6 -query QT_INSTALL_PREFIX 2^>nul') do set QT_PREFIX=%%i
if "%QT_PREFIX%"=="" (
    for /f "tokens=*" %%i in ('qmake -query QT_INSTALL_PREFIX 2^>nul') do set QT_PREFIX=%%i
)

if not "%QT_PREFIX%"=="" (
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_PREFIX%"
) else (
    cmake .. -DCMAKE_BUILD_TYPE=Release
)

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    echo.
    echo Common solutions:
    echo 1. Ensure Qt6 development packages are installed
    echo 2. Set Qt6_DIR environment variable if Qt6 is in a custom location
    echo 3. Make sure Visual Studio or MinGW is properly installed
    exit /b 1
)

REM Detect number of CPU cores
if not defined NUMBER_OF_PROCESSORS set NUMBER_OF_PROCESSORS=4

REM Build the project
echo [INFO] Building project using %NUMBER_OF_PROCESSORS% cores...
cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    echo.
    echo Check the error messages above for specific issues.
    echo Common solutions:
    echo 1. Ensure all Qt6 modules are installed (Charts, Multimedia, etc.)
    echo 2. Check that Visual Studio or MinGW is properly installed
    echo 3. Verify all source files are present
    exit /b 1
)

echo [SUCCESS] Build completed successfully!
echo.
echo 🚀 CyberRecon Suite v1.7 is ready!