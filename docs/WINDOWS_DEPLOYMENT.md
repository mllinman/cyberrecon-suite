# Windows Deployment Guide - CyberRecon Suite v1.7

This guide covers building, packaging, and distributing CyberRecon Suite for Windows platforms.

## 🚀 Quick Start

### Automated Build (Recommended)
```bash
# On Linux/WSL with cross-compilation support
./build-windows.sh

# On Windows with Qt6 and MinGW
build.bat
deploy-windows.bat
```

### Manual Build Process
1. Install dependencies (Qt6, CMake, MinGW)
2. Configure and build with CMake
3. Deploy Qt dependencies with windeployqt
4. Package for distribution
5. Create installer (optional)

## 📋 Prerequisites

### Windows Build Environment

#### Required Software
- **Qt6** (6.2+) with MinGW or MSVC support
- **CMake** 3.16 or higher  
- **MinGW-w64** or **Visual Studio 2019+**
- **Git** for version control

#### Optional Tools
- **NSIS** for creating Windows installers
- **7-Zip** for creating distribution archives
- **ImageMagick** for icon conversion

### Cross-Compilation (Linux → Windows)

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y \
    mingw-w64 \
    cmake \
    git \
    nsis \
    zip
```

#### Qt6 for Cross-Compilation
```bash
# Install Qt6 development packages
sudo apt install -y \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-charts-dev \
    libqt6sql6-sqlite

# Or download Qt6 for MinGW from qt.io
```

## 🔧 Build Process

### Method 1: Native Windows Build

#### Step 1: Setup Environment
1. Install Qt6 from https://www.qt.io/download
2. Add Qt6\bin to your PATH
3. Install CMake and MinGW

#### Step 2: Build Application
```batch
REM Clone repository
git clone https://github.com/mllinman/cyberrecon-suite.git
cd cyberrecon-suite

REM Build using provided script
build.bat

REM Or manually
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

#### Step 3: Deploy Dependencies
```batch
REM Use the deployment script
deploy-windows.bat

REM Or manually deploy Qt libraries
windeployqt build/CyberReconSuite.exe --release --no-translations
```

### Method 2: Cross-Compilation (Linux)

#### Step 1: Prepare Cross-Compilation Environment
```bash
# Install cross-compilation tools
sudo apt install mingw-w64

# Setup Qt6 for MinGW (if available)
export Qt6_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt6"
```

#### Step 2: Build with Cross-Compilation
```bash
# Use automated script
./build-windows.sh

# Or manually
mkdir build-windows
cd build-windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-windows.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 📦 Packaging

### Deployment Package Structure
```
CyberReconSuite-v1.7-Windows/
├── CyberReconSuite.exe          # Main application
├── *.dll                       # Qt and runtime libraries
├── platforms/                  # Qt platform plugins
├── styles/                     # Qt style plugins
├── imageformats/               # Image format plugins
├── docs/                       # Documentation
├── demo/                       # Demo files
├── README.txt                  # Package information
├── LICENSE                     # License file
├── VERSION.txt                 # Version information
├── install.bat                 # Installation script
└── uninstall.bat              # Uninstallation script
```

### Automated Packaging
```bash
# Linux cross-compilation
./build-windows.sh

# Windows native
deploy-windows.bat
```

### Manual Packaging
```bash
# Create deployment directory
mkdir CyberReconSuite-v1.7-Windows

# Copy executable
cp build/CyberReconSuite.exe CyberReconSuite-v1.7-Windows/

# Deploy Qt dependencies
windeployqt CyberReconSuite-v1.7-Windows/CyberReconSuite.exe --release

# Copy documentation
cp README.md LICENSE docs/ CyberReconSuite-v1.7-Windows/

# Create ZIP archive
zip -r CyberReconSuite-v1.7-Windows.zip CyberReconSuite-v1.7-Windows/
```

## 💿 Installer Creation

### NSIS Installer

#### Prerequisites
```bash
# Linux
sudo apt install nsis

# Windows
# Download NSIS from https://nsis.sourceforge.io/
```

#### Build Installer
```bash
# Ensure deployment package exists
# then compile NSIS script
makensis installer.nsi

# Output: CyberReconSuite-v1.7-Setup.exe
```

#### Installer Features
- System-wide installation to Program Files
- Desktop and Start Menu shortcuts
- Uninstaller with registry cleanup
- Version information and branding
- Component selection (core, docs, demo)

### MSI Installer (Advanced)
For enterprise deployment, consider WiX Toolset:
```xml
<!-- Example WiX configuration -->
<Product Id="*" Name="CyberRecon Suite" Version="1.7.0">
  <!-- MSI installer definition -->
</Product>
```

## 🚀 Distribution

### Distribution Methods

#### Method 1: ZIP Archive
- **Pros**: Simple, portable, no installation required
- **Cons**: Manual setup, no system integration
- **Use case**: Portable/testing installations

#### Method 2: NSIS Installer
- **Pros**: Professional installation, system integration
- **Cons**: Requires admin privileges
- **Use case**: End-user installations

#### Method 3: MSI Package
- **Pros**: Enterprise-friendly, Group Policy deployment
- **Cons**: Complex creation process
- **Use case**: Corporate environments

### GitHub Releases
```bash
# Tag and push for automated release
git tag -a v1.7.0 -m "CyberRecon Suite v1.7.0"
git push origin v1.7.0

# GitHub Actions will automatically:
# 1. Build Windows executable
# 2. Create deployment package  
# 3. Generate NSIS installer
# 4. Upload to GitHub Releases
```

## 📊 CI/CD Pipeline

### GitHub Actions Workflow

The `.github/workflows/windows-build.yml` automates:
1. **Environment Setup**: Qt6, MinGW, CMake
2. **Build Process**: Configure, compile, test
3. **Deployment**: windeployqt, packaging
4. **Installer Creation**: NSIS compilation
5. **Artifact Upload**: ZIP and EXE files
6. **Release**: Automatic GitHub release on tags

### Triggered On
- Push to main branch
- Pull requests
- Tagged releases (v*)
- Manual workflow dispatch

## 🔍 Testing & Validation

### Build Testing
```bash
# Verify executable runs
./CyberReconSuite.exe --version

# Check dependencies
ldd CyberReconSuite.exe  # Linux cross-compile
dependency_walker CyberReconSuite.exe  # Windows

# Test installation
install.bat
```

### System Requirements Testing
- Windows 10/11 (32-bit and 64-bit)
- Different Qt versions (6.2, 6.5, 6.7)
- Various hardware configurations
- Antivirus compatibility

## 🔐 Code Signing (Production)

### Certificate Setup
```batch
REM Sign executable with certificate
signtool sign /f "certificate.pfx" /p "password" /t "http://timestamp.comodoca.com" CyberReconSuite.exe

REM Sign installer
signtool sign /f "certificate.pfx" /p "password" /t "http://timestamp.comodoca.com" CyberReconSuite-v1.7-Setup.exe
```

### Benefits
- Windows SmartScreen compatibility
- User trust and security
- Enterprise deployment readiness

## 🛠️ Troubleshooting

### Common Build Issues

#### Qt6 Not Found
```bash
# Set Qt6 path
export Qt6_DIR="/path/to/qt6/lib/cmake/Qt6"

# Or use CMAKE_PREFIX_PATH
cmake -DCMAKE_PREFIX_PATH="/path/to/qt6" ..
```

#### Missing DLLs
```bash
# Use windeployqt to find missing dependencies
windeployqt --debug --verbose CyberReconSuite.exe

# Or use dependency walker on Windows
```

#### Cross-Compilation Failures
```bash
# Check toolchain file
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-windows.cmake ..

# Verify MinGW installation
x86_64-w64-mingw32-gcc --version
```

### Runtime Issues

#### Application Won't Start
1. Check missing DLLs with Dependency Walker
2. Verify Qt platform plugins (platforms/)
3. Check Windows version compatibility

#### Performance Issues
1. Build in Release mode
2. Enable compiler optimizations
3. Profile with Qt Creator or Visual Studio

## 📞 Support

### Build Support
- GitHub Issues: Report build problems
- Documentation: Check docs/ directory
- Community: Discussions and Q&A

### Windows-Specific Issues
- Qt Documentation: Qt6 Windows deployment
- Microsoft Docs: Windows development
- Stack Overflow: Community solutions

## 🏆 Best Practices

### Security
- Always build from clean source
- Verify dependencies and checksums
- Use code signing for production
- Scan for vulnerabilities

### Distribution
- Test on multiple Windows versions
- Provide both portable and installer options
- Include comprehensive documentation
- Maintain version compatibility matrix

### Maintenance
- Automate build and deployment
- Keep dependencies updated
- Monitor for security updates
- Maintain backward compatibility

---

## 📝 File Reference

| File | Purpose |
|------|---------|
| `build.bat` | Windows native build script |
| `build-windows.sh` | Cross-compilation build script |
| `deploy-windows.bat` | Windows deployment packaging |
| `installer.nsi` | NSIS installer script |
| `toolchain-windows.cmake` | Cross-compilation toolchain |
| `resources/app.rc` | Windows resource file |
| `.github/workflows/windows-build.yml` | CI/CD pipeline |

---

**Ready to deploy CyberRecon Suite on Windows! 🚀**