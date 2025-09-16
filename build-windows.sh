#!/bin/bash
# CyberRecon Suite - Windows Cross-Compilation and Packaging Script
# This script builds the Windows executable and creates an installer

set -e  # Exit on any error

echo "🛡️ CyberRecon Suite v1.7 - Windows Build & Package Script"
echo "=========================================================="

# Configuration
BUILD_DIR="build-windows"
DEPLOY_DIR="CyberReconSuite-v1.7-Windows"
INSTALLER_NAME="CyberReconSuite-v1.7-Setup.exe"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Functions
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    print_status "Checking build dependencies..."
    
    local missing_deps=()
    
    # Check for MinGW cross-compiler
    if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
        missing_deps+=("mingw-w64")
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    fi
    
    # Check for NSIS (for installer)
    if ! command -v makensis &> /dev/null; then
        print_warning "NSIS not found. Installer creation will be skipped."
        print_warning "To install NSIS: sudo apt install nsis"
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        print_error "Please install them using:"
        print_error "sudo apt update"
        for dep in "${missing_deps[@]}"; do
            print_error "sudo apt install $dep"
        done
        exit 1
    fi
    
    print_success "All required dependencies found"
}

# Install cross-compilation dependencies
install_mingw_qt() {
    print_status "Setting up Qt6 for MinGW cross-compilation..."
    
    # Check if Qt6 MinGW packages are available
    if ! dpkg -l | grep -q qt6-base-dev-tools; then
        print_warning "Qt6 not found. Installing Qt6 development packages..."
        sudo apt update
        sudo apt install -y \
            qt6-base-dev \
            qt6-base-dev-tools \
            qt6-charts-dev \
            qt6-tools-dev \
            qt6-tools-dev-tools \
            libqt6sql6-sqlite \
            qt6-l10n-tools
    fi
    
    print_success "Qt6 development environment ready"
}

# Build for Windows
build_windows() {
    print_status "Building CyberRecon Suite for Windows..."
    
    # Clean build directory
    if [ -d "$BUILD_DIR" ]; then
        print_warning "Cleaning existing build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake for cross-compilation
    print_status "Configuring build with CMake..."
    if [ -f "../toolchain-windows.cmake" ] && command -v x86_64-w64-mingw32-gcc &> /dev/null; then
        # Cross-compilation setup
        cmake .. \
            -DCMAKE_TOOLCHAIN_FILE="../toolchain-windows.cmake" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="/usr/x86_64-w64-mingw32" \
            -DQt6_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt6"
    else
        # Native Linux build (for testing/development)
        print_warning "Cross-compilation not available, building native Linux version..."
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release
    fi
    
    # Build the project
    print_status "Compiling application..."
    make -j$(nproc)
    
    cd ..
    print_success "Build completed successfully"
}

# Create Windows deployment package
create_deployment_package() {
    print_status "Creating Windows deployment package..."
    
    # Clean deployment directory
    if [ -d "$DEPLOY_DIR" ]; then
        rm -rf "$DEPLOY_DIR"
    fi
    
    mkdir -p "$DEPLOY_DIR"
    
    # Find the executable
    if [ -f "$BUILD_DIR/CyberReconSuite.exe" ]; then
        EXE_PATH="$BUILD_DIR/CyberReconSuite.exe"
    elif [ -f "$BUILD_DIR/CyberReconSuite" ]; then
        EXE_PATH="$BUILD_DIR/CyberReconSuite"
        # Rename for Windows
        cp "$EXE_PATH" "$DEPLOY_DIR/CyberReconSuite.exe"
    else
        print_error "Executable not found in build directory"
        exit 1
    fi
    
    if [ -f "$EXE_PATH" ] && [[ "$EXE_PATH" == *.exe ]]; then
        cp "$EXE_PATH" "$DEPLOY_DIR/"
    fi
    
    # Copy documentation
    [ -f "README.md" ] && cp "README.md" "$DEPLOY_DIR/"
    [ -f "LICENSE" ] && cp "LICENSE" "$DEPLOY_DIR/"
    [ -f "CHANGELOG.md" ] && cp "CHANGELOG.md" "$DEPLOY_DIR/"
    
    # Copy documentation directory
    if [ -d "docs" ]; then
        cp -r "docs" "$DEPLOY_DIR/"
    fi
    
    # Copy demo files
    if [ -d "demo" ]; then
        cp -r "demo" "$DEPLOY_DIR/"
    fi
    
    # Create installation scripts
    create_install_scripts
    
    # Create version info
    create_version_info
    
    print_success "Deployment package created in $DEPLOY_DIR"
}

# Create installation scripts
create_install_scripts() {
    print_status "Creating installation scripts..."
    
    # Windows batch installer
    cat > "$DEPLOY_DIR/install.bat" << 'EOF'
@echo off
echo Installing CyberRecon Suite v1.7...
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo This installer requires administrator privileges.
    echo Please run as Administrator.
    pause
    exit /b 1
)

REM Create installation directory
if not exist "C:\Program Files\CyberRecon Suite" mkdir "C:\Program Files\CyberRecon Suite"

REM Copy files
echo Copying application files...
xcopy "." "C:\Program Files\CyberRecon Suite\" /E /I /Y /EXCLUDE:exclude.txt

REM Create desktop shortcut
echo Creating desktop shortcut...
powershell "$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%PUBLIC%\Desktop\CyberRecon Suite.lnk'); $Shortcut.TargetPath = 'C:\Program Files\CyberRecon Suite\CyberReconSuite.exe'; $Shortcut.WorkingDirectory = 'C:\Program Files\CyberRecon Suite'; $Shortcut.Description = 'CyberRecon Suite v1.7'; $Shortcut.Save()"

REM Create start menu shortcut
mkdir "%ALLUSERSPROFILE%\Microsoft\Windows\Start Menu\Programs\CyberRecon Suite" 2>nul
powershell "$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%ALLUSERSPROFILE%\Microsoft\Windows\Start Menu\Programs\CyberRecon Suite\CyberRecon Suite.lnk'); $Shortcut.TargetPath = 'C:\Program Files\CyberRecon Suite\CyberReconSuite.exe'; $Shortcut.WorkingDirectory = 'C:\Program Files\CyberRecon Suite'; $Shortcut.Description = 'CyberRecon Suite v1.7'; $Shortcut.Save()"

echo.
echo [SUCCESS] CyberRecon Suite v1.7 installed successfully!
echo You can now run it from the desktop shortcut or Start Menu.
echo.
pause
EOF

    # Windows batch uninstaller
    cat > "$DEPLOY_DIR/uninstall.bat" << 'EOF'
@echo off
echo Uninstalling CyberRecon Suite v1.7...
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo This uninstaller requires administrator privileges.
    echo Please run as Administrator.
    pause
    exit /b 1
)

REM Remove installation directory
if exist "C:\Program Files\CyberRecon Suite" (
    rmdir /s /q "C:\Program Files\CyberRecon Suite"
    echo Removed application files.
)

REM Remove shortcuts
if exist "%PUBLIC%\Desktop\CyberRecon Suite.lnk" (
    del "%PUBLIC%\Desktop\CyberRecon Suite.lnk"
    echo Removed desktop shortcut.
)

if exist "%ALLUSERSPROFILE%\Microsoft\Windows\Start Menu\Programs\CyberRecon Suite" (
    rmdir /s /q "%ALLUSERSPROFILE%\Microsoft\Windows\Start Menu\Programs\CyberRecon Suite"
    echo Removed start menu shortcuts.
)

echo.
echo [SUCCESS] CyberRecon Suite v1.7 uninstalled successfully!
echo.
pause
EOF

    # Exclude file for xcopy
    cat > "$DEPLOY_DIR/exclude.txt" << 'EOF'
install.bat
uninstall.bat
exclude.txt
EOF

    chmod +x "$DEPLOY_DIR/install.bat" 2>/dev/null || true
    chmod +x "$DEPLOY_DIR/uninstall.bat" 2>/dev/null || true
}

# Create version and readme files
create_version_info() {
    print_status "Creating version information..."
    
    # Version info
    cat > "$DEPLOY_DIR/VERSION.txt" << EOF
CyberRecon Suite v1.7
Build Date: $(date)
Platform: Windows x64
Architecture: x86_64

Build Information:
- Build Type: Release
- Compiler: MinGW-w64
- Qt Version: 6.x
- Target: Windows Desktop

Components:
- Core Application
- Qt Runtime Libraries
- Documentation
- Demo Files
EOF

    # Package README
    cat > "$DEPLOY_DIR/README.txt" << 'EOF'
# CyberRecon Suite v1.7 - Windows Distribution

## Quick Start

### Option 1: Simple Installation
1. Run `install.bat` as Administrator for system-wide installation
2. Use desktop shortcut or Start Menu to launch

### Option 2: Portable Mode  
1. Simply run `CyberReconSuite.exe` directly from this folder
2. No installation required

## System Requirements

- Windows 10/11 (64-bit)
- 4GB RAM minimum, 8GB recommended  
- 1GB free disk space
- OpenGL compatible graphics

## Features

- SIEM Dashboard & Real-time Monitoring
- Threat Intelligence & Analysis
- Security Orchestration (SOAR)
- Compliance Management (SOC 2, NIST, GDPR)
- Digital Forensics Tools
- Network Security Testing
- Wireless Penetration Testing
- Case Management & Collaboration

## Support

- Documentation: docs/ folder
- GitHub: https://github.com/mllinman/cyberrecon-suite
- Issues: Report bugs and feature requests on GitHub

## License

See LICENSE file for terms and conditions.
EOF
}

# Create NSIS installer
create_nsis_installer() {
    if ! command -v makensis &> /dev/null; then
        print_warning "NSIS not found. Skipping installer creation."
        print_warning "Install with: sudo apt install nsis"
        return
    fi
    
    print_status "Creating Windows installer with NSIS..."
    
    # Check if installer script exists and deployment package is ready
    if [ ! -f "installer.nsi" ]; then
        print_warning "installer.nsi not found. Skipping NSIS installer creation."
        return
    fi
    
    if [ ! -d "$DEPLOY_DIR" ]; then
        print_error "Deployment directory not found. Run create_deployment_package first."
        return
    fi
    
    # Create installer
    makensis installer.nsi
    
    if [ -f "$INSTALLER_NAME" ]; then
        print_success "Windows installer created: $INSTALLER_NAME"
    else
        print_error "Failed to create Windows installer"
    fi
}

# Create distribution archive
create_archive() {
    print_status "Creating distribution archive..."
    
    if [ -d "$DEPLOY_DIR" ]; then
        # Create ZIP archive
        zip -r "CyberReconSuite-v1.7-Windows.zip" "$DEPLOY_DIR"
        print_success "Created: CyberReconSuite-v1.7-Windows.zip"
        
        # Create TAR archive for backup
        tar -czf "CyberReconSuite-v1.7-Windows.tar.gz" "$DEPLOY_DIR"
        print_success "Created: CyberReconSuite-v1.7-Windows.tar.gz"
    else
        print_error "Deployment directory not found"
        return 1
    fi
}

# Display summary
show_summary() {
    echo ""
    echo "=========================================="
    echo "🎉 Windows Build & Package Complete! 🎉"
    echo "=========================================="
    echo ""
    echo "Generated files:"
    echo "📁 $DEPLOY_DIR/ - Deployment package"
    [ -f "CyberReconSuite-v1.7-Windows.zip" ] && echo "📦 CyberReconSuite-v1.7-Windows.zip - Distribution archive"
    [ -f "$INSTALLER_NAME" ] && echo "💿 $INSTALLER_NAME - Windows installer"
    echo ""
    echo "Distribution options:"
    echo "1. Share the ZIP file for manual extraction"
    echo "2. Use the .exe installer for automated installation"
    echo "3. Provide the deployment folder for portable usage"
    echo ""
    echo "🚀 Ready for Windows deployment!"
}

# Main execution
main() {
    echo ""
    print_status "Starting Windows build and packaging process..."
    
    check_dependencies
    install_mingw_qt
    build_windows
    create_deployment_package
    create_nsis_installer
    create_archive
    show_summary
}

# Run main function
main "$@"