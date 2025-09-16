@echo off
REM CyberRecon Suite - Windows Deployment Verification Script
REM Checks if all components needed for Windows deployment are ready

echo 🛡️ CyberRecon Suite v1.7 - Windows Deployment Verification
echo ===========================================================
echo.

set "ERROR_COUNT=0"
set "SUCCESS_COUNT=0"

REM Check required files
echo [INFO] Checking deployment infrastructure files...

REM Build scripts
if exist "build.bat" (
    echo ✅ build.bat - Windows build script found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ build.bat - Missing Windows build script
    set /a ERROR_COUNT+=1
)

if exist "build-windows.sh" (
    echo ✅ build-windows.sh - Cross-compilation script found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ build-windows.sh - Missing cross-compilation script
    set /a ERROR_COUNT+=1
)

if exist "deploy-windows.bat" (
    echo ✅ deploy-windows.bat - Deployment packaging script found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ deploy-windows.bat - Missing deployment script
    set /a ERROR_COUNT+=1
)

REM Installer components
if exist "installer.nsi" (
    echo ✅ installer.nsi - NSIS installer script found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ installer.nsi - Missing NSIS installer script
    set /a ERROR_COUNT+=1
)

if exist "resources\app.rc" (
    echo ✅ resources\app.rc - Windows resource file found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ resources\app.rc - Missing Windows resource file
    set /a ERROR_COUNT+=1
)

REM CMake configuration
if exist "CMakeLists.txt" (
    echo ✅ CMakeLists.txt - Build configuration found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ CMakeLists.txt - Missing build configuration
    set /a ERROR_COUNT+=1
)

if exist "toolchain-windows.cmake" (
    echo ✅ toolchain-windows.cmake - Cross-compilation toolchain found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ toolchain-windows.cmake - Missing cross-compilation toolchain
    set /a ERROR_COUNT+=1
)

REM CI/CD
if exist ".github\workflows\windows-build.yml" (
    echo ✅ .github\workflows\windows-build.yml - GitHub Actions workflow found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ .github\workflows\windows-build.yml - Missing CI/CD workflow
    set /a ERROR_COUNT+=1
)

REM Documentation
if exist "docs\WINDOWS_DEPLOYMENT.md" (
    echo ✅ docs\WINDOWS_DEPLOYMENT.md - Deployment guide found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ docs\WINDOWS_DEPLOYMENT.md - Missing deployment documentation
    set /a ERROR_COUNT+=1
)

if exist "WINDOWS_QUICK_START.md" (
    echo ✅ WINDOWS_QUICK_START.md - Quick start guide found
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ WINDOWS_QUICK_START.md - Missing quick start guide
    set /a ERROR_COUNT+=1
)

echo.
echo [INFO] Checking build tools availability...

REM Check for Qt6
where qmake6 >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ Qt6 (qmake6) - Found in PATH
    set /a SUCCESS_COUNT+=1
) else (
    where qmake >nul 2>&1
    if %errorlevel% equ 0 (
        echo ✅ Qt (qmake) - Found in PATH
        set /a SUCCESS_COUNT+=1
    ) else (
        echo ❌ Qt6/Qt - Not found in PATH
        set /a ERROR_COUNT+=1
    )
)

REM Check for CMake
where cmake >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ CMake - Found in PATH
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ CMake - Not found in PATH
    set /a ERROR_COUNT+=1
)

REM Check for MinGW or MSVC
where gcc >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ GCC (MinGW) - Found in PATH
    set /a SUCCESS_COUNT+=1
) else (
    where cl >nul 2>&1
    if %errorlevel% equ 0 (
        echo ✅ MSVC (cl.exe) - Found in PATH
        set /a SUCCESS_COUNT+=1
    ) else (
        echo ❌ GCC/MinGW/MSVC - No compiler found in PATH
        set /a ERROR_COUNT+=1
    )
)

REM Check for NSIS (optional)
where makensis >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ NSIS (makensis) - Found in PATH (installer creation available)
    set /a SUCCESS_COUNT+=1
) else (
    echo ⚠️  NSIS (makensis) - Not found (installer creation will be skipped)
)

REM Check for windeployqt
where windeployqt >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ windeployqt - Found in PATH
    set /a SUCCESS_COUNT+=1
) else (
    echo ❌ windeployqt - Not found in PATH
    set /a ERROR_COUNT+=1
)

echo.
echo ===========================================================
echo 📊 Verification Results
echo ===========================================================
echo ✅ Success: %SUCCESS_COUNT%
echo ❌ Errors: %ERROR_COUNT%
echo.

if %ERROR_COUNT% equ 0 (
    echo 🎉 All checks passed! Your Windows deployment infrastructure is ready.
    echo.
    echo 🚀 Next Steps:
    echo    1. Run 'build.bat' to build the application
    echo    2. Run 'deploy-windows.bat' to create deployment package
    echo    3. Use 'makensis installer.nsi' to create installer
    echo.
) else (
    echo ⚠️  Some checks failed. Please review the errors above.
    echo.
    echo 🔧 Quick Fixes:
    echo    - Install Qt6 and add to PATH
    echo    - Install CMake and add to PATH  
    echo    - Install MinGW or Visual Studio
    echo    - Run 'deploy-windows.bat' to check windeployqt
    echo.
)

echo For detailed instructions, see:
echo    - WINDOWS_QUICK_START.md
echo    - docs\WINDOWS_DEPLOYMENT.md
echo.
pause