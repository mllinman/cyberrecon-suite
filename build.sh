#!/bin/bash

# CyberRecon Suite Build Script v1.7

echo "🛡️  Building CyberRecon Suite v1.7 - Modern Cybersecurity Platform..."

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1";
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

# Check operating system
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    IS_WINDOWS=true
else
    IS_WINDOWS=false
fi

# Check if Qt6 is available
print_status "Checking Qt6 installation..."
if ! which qmake6 > /dev/null 2>&1 && ! which qmake > /dev/null 2>&1; then
    print_error "Qt6 not found. Please install Qt6 development packages."
    echo ""
    echo "Installation commands:"
    echo "Ubuntu/Debian: sudo apt install qt6-base-dev qt6-charts-dev qt6-multimedia-dev qt6-tools-dev cmake build-essential"
    echo "Fedora: sudo dnf install qt6-qtbase-devel qt6-qtcharts-devel qt6-qttools-devel cmake gcc-c++"
    echo "macOS: brew install qt6 cmake"
    echo "Windows: Download from https://www.qt.io/download"
    exit 1
fi

# Check Qt6 version
if which qmake6 > /dev/null 2>&1; then
    QT_VERSION=$(qmake6 -query QT_VERSION 2>/dev/null)
elif which qmake > /dev/null 2>&1; then
    QT_VERSION=$(qmake -query QT_VERSION 2>/dev/null)
fi

if [ -n "$QT_VERSION" ]; then
    print_success "Found Qt version: $QT_VERSION"
else
    print_warning "Could not determine Qt version"
fi

# Check if CMake is available
print_status "Checking CMake installation..."
if ! which cmake > /dev/null 2>&1; then
    print_error "CMake not found. Please install CMake 3.16 or higher."
    echo ""
    echo "Installation commands:"
    echo "Ubuntu/Debian: sudo apt install cmake"
    echo "Fedora: sudo dnf install cmake"
    echo "macOS: brew install cmake"
    echo "Windows: Download from https://cmake.org/download/"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
print_success "Found CMake version: $CMAKE_VERSION"

# Check for required system libraries
print_status "Checking system dependencies..."

# Check for OpenGL (required for Qt6 Charts) - Linux only
if [ "$IS_WINDOWS" = false ]; then
    if which ldconfig > /dev/null 2>&1; then
        if ! ldconfig -p 2>/dev/null | grep -q libGL; then
            print_warning "OpenGL libraries may not be installed. Charts may not work properly."
            echo "Install with: sudo apt install libgl1-mesa-dev (Ubuntu/Debian)"
        fi
    fi
fi

# Create build directory
print_status "Creating build directory..."
if [ -d "build" ]; then
    print_warning "Build directory exists. Cleaning..."
    rm -rf build
fi
mkdir -p build
cd build

# Configure with CMake
print_status "Configuring with CMake..."

# Get Qt installation prefix
if which qmake6 > /dev/null 2>&1; then
    QT_PREFIX=$(qmake6 -query QT_INSTALL_PREFIX 2>/dev/null)
elif which qmake > /dev/null 2>&1; then
    QT_PREFIX=$(qmake -query QT_INSTALL_PREFIX 2>/dev/null)
fi

if [ -n "$QT_PREFIX" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$QT_PREFIX"
else
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed!"
    echo ""
    echo "Common solutions:"
    echo "1. Ensure Qt6 development packages are installed"
    echo "2. Set Qt6_DIR environment variable if Qt6 is in a custom location"
    echo "3. Install missing dependencies listed above"
    exit 1
fi

# Detect number of CPU cores
if which nproc > /dev/null 2>&1; then
    CORES=$(nproc)
elif which sysctl > /dev/null 2>&1; then
    CORES=$(sysctl -n hw.ncpu)
elif [ "$IS_WINDOWS" = true ]; then
    CORES=${NUMBER_OF_PROCESSORS:-4}
else
    CORES=4
fi

# Build the project
print_status "Building project using $CORES cores..."

if [ "$IS_WINDOWS" = true ]; then
    cmake --build . --config Release --parallel $CORES
else
    make -j$CORES
fi

if [ $? -ne 0 ]; then
    print_error "Build failed!"
    echo ""
    echo "Check the error messages above for specific issues."
    echo "Common solutions:"
    echo "1. Ensure all Qt6 modules are installed (Charts, Multimedia, etc.)"
    echo "2. Check that C++17 compiler is available"
    echo "3. Verify all source files are present"
    exit 1
fi

print_success "Build completed successfully!"
echo ""
echo "🚀 CyberRecon Suite v1.7 is ready!"