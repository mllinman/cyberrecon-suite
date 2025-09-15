# Windows Distribution Quick Start - CyberRecon Suite v1.7

This document provides quick instructions for building and distributing Windows .exe files and installers.

## 🚀 Quick Build Options

### Option 1: Windows Native Build (Recommended for Windows users)
```batch
REM Prerequisites: Qt6, CMake, MinGW/Visual Studio
build.bat
deploy-windows.bat
```

**Output:**
- `CyberReconSuite-v1.7-Windows/` - Deployment folder
- `CyberReconSuite-v1.7-Windows.zip` - Portable package

### Option 2: Cross-Compilation (Linux → Windows)
```bash
# Prerequisites: mingw-w64, cmake
./build-windows.sh
```

**Output:**
- `CyberReconSuite-v1.7-Windows/` - Deployment folder
- `CyberReconSuite-v1.7-Windows.zip` - Portable package
- `CyberReconSuite-v1.7-Setup.exe` - NSIS installer (if available)

### Option 3: GitHub Actions (Automated)
```bash
# Tag and push for automated release
git tag -a v1.7.0 -m "Release v1.7.0"  
git push origin v1.7.0
```

**Output on GitHub Releases:**
- `CyberReconSuite-Windows.zip` - Portable package
- `CyberReconSuite-v1.7-Setup.exe` - Professional installer

## 📦 Distribution Packages

### Portable Package (.zip)
- Extract and run `CyberReconSuite.exe`
- No installation required
- Includes all dependencies
- Perfect for testing or portable usage

### Professional Installer (.exe)
- System-wide installation to Program Files
- Desktop and Start Menu shortcuts
- Uninstaller with registry cleanup
- Professional appearance for end users

## 🔧 Prerequisites

### Windows Development
- **Qt6** (6.2+) - Download from qt.io
- **CMake** 3.16+ - Download from cmake.org  
- **MinGW** or **Visual Studio 2019+**

### Linux Cross-Compilation
```bash
sudo apt update
sudo apt install -y mingw-w64 cmake nsis qt6-base-dev
```

## 📋 Usage Examples

### For End Users
1. Download `CyberReconSuite-v1.7-Setup.exe` from GitHub Releases
2. Run as Administrator
3. Follow installation wizard
4. Launch from desktop shortcut

### For Portable Usage
1. Download `CyberReconSuite-Windows.zip`
2. Extract to desired location
3. Run `CyberReconSuite.exe` directly

### For Developers
1. Clone repository
2. Install prerequisites
3. Run `build.bat` (Windows) or `./build-windows.sh` (Linux)
4. Use `deploy-windows.bat` for packaging

## 🔍 Verification

### Test Your Build
```batch
REM Check executable info
CyberReconSuite.exe --version

REM Test installation
install.bat

REM Verify dependencies
windeployqt --help
```

### System Requirements
- **OS:** Windows 10/11 (64-bit recommended)
- **RAM:** 4GB minimum, 8GB recommended
- **Storage:** 500MB free space
- **Graphics:** OpenGL compatible

## 🆘 Troubleshooting

### Build Issues
- **Qt6 not found:** Add Qt6\bin to PATH
- **CMake errors:** Install CMake 3.16+
- **Compiler errors:** Install MinGW or Visual Studio

### Runtime Issues
- **Missing DLLs:** Run `windeployqt CyberReconSuite.exe`
- **Won't start:** Check platforms/ folder exists
- **Performance:** Build in Release mode

## 📞 Support

- **Documentation:** [docs/WINDOWS_DEPLOYMENT.md](docs/WINDOWS_DEPLOYMENT.md)
- **Issues:** https://github.com/mllinman/cyberrecon-suite/issues
- **Releases:** https://github.com/mllinman/cyberrecon-suite/releases

---

## 🎯 Ready to Deploy!

Your CyberRecon Suite is now ready for Windows distribution with:
✅ Professional installer  
✅ Portable package  
✅ Automated CI/CD  
✅ Complete documentation  

**Deploy with confidence! 🚀**