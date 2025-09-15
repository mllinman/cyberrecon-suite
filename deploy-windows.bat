@echo off
REM CyberRecon Suite Windows Deployment Script v1.7
REM This script packages the application with Qt dependencies for Windows distribution

echo 🚀 CyberRecon Suite v1.7 - Windows Deployment Package Creator...
echo.

REM Check if executable exists
if not exist "build\Release\CyberReconSuite.exe" (
    if not exist "build\CyberReconSuite.exe" (
        echo [ERROR] CyberReconSuite.exe not found in build directory.
        echo Please run build.bat first to compile the application.
        exit /b 1
    )
    set "EXECUTABLE_PATH=build\CyberReconSuite.exe"
) else (
    set "EXECUTABLE_PATH=build\Release\CyberReconSuite.exe"
)

echo [INFO] Found executable: %EXECUTABLE_PATH%

REM Create deployment directory
set "DEPLOY_DIR=CyberReconSuite-v1.7-Windows"
echo [INFO] Creating deployment directory: %DEPLOY_DIR%
if exist "%DEPLOY_DIR%" (
    echo [WARNING] Deployment directory exists. Cleaning...
    rmdir /s /q "%DEPLOY_DIR%"
)
mkdir "%DEPLOY_DIR%"

REM Copy executable
echo [INFO] Copying executable...
copy "%EXECUTABLE_PATH%" "%DEPLOY_DIR%\CyberReconSuite.exe"

REM Find Qt installation
echo [INFO] Locating Qt installation...
for /f "tokens=*" %%i in ('qmake6 -query QT_INSTALL_PREFIX 2^>nul') do set QT_PREFIX=%%i
if "%QT_PREFIX%"=="" (
    for /f "tokens=*" %%i in ('qmake -query QT_INSTALL_PREFIX 2^>nul') do set QT_PREFIX=%%i
)

if "%QT_PREFIX%"=="" (
    echo [ERROR] Could not find Qt installation. Please ensure Qt is installed and in PATH.
    exit /b 1
)

echo [SUCCESS] Found Qt installation: %QT_PREFIX%

REM Check if windeployqt is available
where windeployqt >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] windeployqt not found. Please ensure Qt bin directory is in PATH.
    echo Expected location: %QT_PREFIX%\bin\windeployqt.exe
    exit /b 1
)

REM Deploy Qt dependencies
echo [INFO] Deploying Qt dependencies...
windeployqt "%DEPLOY_DIR%\CyberReconSuite.exe" --release --no-translations --no-system-d3d-compiler --no-opengl-sw

if %errorlevel% neq 0 (
    echo [ERROR] Failed to deploy Qt dependencies!
    exit /b 1
)

REM Copy additional runtime files
echo [INFO] Copying additional runtime files...

REM Create necessary directories
if not exist "%DEPLOY_DIR%\data" mkdir "%DEPLOY_DIR%\data"
if not exist "%DEPLOY_DIR%\docs" mkdir "%DEPLOY_DIR%\docs"

REM Copy documentation
if exist "README.md" copy "README.md" "%DEPLOY_DIR%\"
if exist "LICENSE" copy "LICENSE" "%DEPLOY_DIR%\"
if exist "CHANGELOG.md" copy "CHANGELOG.md" "%DEPLOY_DIR%\"

REM Copy documentation directory
if exist "docs" (
    echo [INFO] Copying documentation...
    xcopy "docs" "%DEPLOY_DIR%\docs" /E /I /Y
)

REM Copy demo files if they exist
if exist "demo" (
    echo [INFO] Copying demo files...
    xcopy "demo" "%DEPLOY_DIR%\demo" /E /I /Y
)

REM Create installation script
echo [INFO] Creating installation script...
(
echo @echo off
echo echo Installing CyberRecon Suite v1.7...
echo echo.
echo if not exist "C:\Program Files\CyberRecon" mkdir "C:\Program Files\CyberRecon"
echo xcopy "." "C:\Program Files\CyberRecon\" /E /I /Y
echo echo.
echo echo [SUCCESS] CyberRecon Suite installed to C:\Program Files\CyberRecon\
echo echo You can now run the application from: C:\Program Files\CyberRecon\CyberReconSuite.exe
echo echo.
echo echo Creating desktop shortcut...
echo set "SHORTCUT_PATH=%USERPROFILE%\Desktop\CyberRecon Suite.lnk"
echo set "TARGET_PATH=C:\Program Files\CyberRecon\CyberReconSuite.exe"
echo powershell "$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%SHORTCUT_PATH%'); $Shortcut.TargetPath = '%TARGET_PATH%'; $Shortcut.WorkingDirectory = 'C:\Program Files\CyberRecon'; $Shortcut.Description = 'CyberRecon Suite v1.7 - Cybersecurity Operations Platform'; $Shortcut.Save()"
echo echo [SUCCESS] Desktop shortcut created.
echo pause
) > "%DEPLOY_DIR%\install.bat"

REM Create uninstall script
echo [INFO] Creating uninstall script...
(
echo @echo off
echo echo Uninstalling CyberRecon Suite v1.7...
echo echo.
echo if exist "C:\Program Files\CyberRecon" (
echo     rmdir /s /q "C:\Program Files\CyberRecon"
echo     echo [SUCCESS] CyberRecon Suite removed from C:\Program Files\CyberRecon\
echo ^) else (
echo     echo [WARNING] CyberRecon Suite installation not found.
echo ^)
echo echo.
echo echo Removing desktop shortcut...
echo if exist "%USERPROFILE%\Desktop\CyberRecon Suite.lnk" (
echo     del "%USERPROFILE%\Desktop\CyberRecon Suite.lnk"
echo     echo [SUCCESS] Desktop shortcut removed.
echo ^) else (
echo     echo [WARNING] Desktop shortcut not found.
echo ^)
echo echo.
echo echo [SUCCESS] Uninstallation complete.
echo pause
) > "%DEPLOY_DIR%\uninstall.bat"

REM Create README for the package
echo [INFO] Creating package README...
(
echo # CyberRecon Suite v1.7 - Windows Distribution Package
echo.
echo ## Installation
echo.
echo 1. Run `install.bat` as Administrator to install the application system-wide
echo 2. Or simply run `CyberReconSuite.exe` directly from this folder
echo.
echo ## System Requirements
echo.
echo - Windows 10/11 ^(64-bit^)
echo - 4GB RAM minimum, 8GB recommended
echo - 500MB free disk space
echo - OpenGL compatible graphics card
echo.
echo ## Features
echo.
echo - Modern Cybersecurity Operations Platform
echo - SIEM Dashboard with real-time monitoring
echo - Threat Intelligence and Analysis
echo - Security Orchestration ^(SOAR^)
echo - Compliance Management ^(SOC 2, NIST, GDPR^)
echo - Digital Forensics Tools
echo - Network and Wireless Security Testing
echo.
echo ## Support
echo.
echo For technical support and documentation, visit:
echo https://github.com/mllinman/cyberrecon-suite
echo.
echo ## License
echo.
echo See LICENSE file for licensing information.
) > "%DEPLOY_DIR%\README.txt"

REM Create version info file
echo [INFO] Creating version info...
(
echo CyberRecon Suite v1.7
echo Build Date: %DATE% %TIME%
echo Platform: Windows x64
echo Qt Version: Unknown
echo.
echo Build Information:
echo - Compiler: MSVC/MinGW
echo - Configuration: Release
echo - Target: Windows Desktop
) > "%DEPLOY_DIR%\VERSION.txt"

REM Get package size
echo [INFO] Calculating package size...
for /f %%A in ('dir "%DEPLOY_DIR%" /s /-c ^| find "bytes"') do set "PACKAGE_SIZE=%%A"

echo.
echo ============================================
echo 🎉 Windows Deployment Package Created! 🎉
echo ============================================
echo Package: %DEPLOY_DIR%
echo Size: %PACKAGE_SIZE% bytes
echo.
echo Contents:
echo - CyberReconSuite.exe (Main Application)
echo - Qt Runtime Libraries
echo - Documentation and License
echo - Installation Scripts
echo - Uninstall Scripts
echo.
echo To distribute:
echo 1. Zip the entire %DEPLOY_DIR% folder
echo 2. Users can extract and run install.bat as Administrator
echo 3. Or run CyberReconSuite.exe directly
echo.
echo Creating distribution archive...

REM Create ZIP archive if 7zip or PowerShell available
where 7z >nul 2>&1
if %errorlevel% equ 0 (
    echo [INFO] Creating ZIP archive with 7-Zip...
    7z a -tzip "CyberReconSuite-v1.7-Windows.zip" "%DEPLOY_DIR%"
    echo [SUCCESS] Created: CyberReconSuite-v1.7-Windows.zip
) else (
    echo [INFO] Creating ZIP archive with PowerShell...
    powershell "Compress-Archive -Path '%DEPLOY_DIR%' -DestinationPath 'CyberReconSuite-v1.7-Windows.zip' -Force"
    if exist "CyberReconSuite-v1.7-Windows.zip" (
        echo [SUCCESS] Created: CyberReconSuite-v1.7-Windows.zip
    ) else (
        echo [WARNING] Could not create ZIP archive automatically.
        echo Please manually zip the %DEPLOY_DIR% folder for distribution.
    )
)

echo.
echo 🚀 Deployment complete! Ready for distribution.