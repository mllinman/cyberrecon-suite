# CyberRecon Suite v1.7 - Dependencies Guide

## Required Dependencies

### Core Qt6 Modules
- **Qt6::Core** - Essential Qt functionality
- **Qt6::Widgets** - UI components and widgets
- **Qt6::Sql** - Database connectivity (SQLite)
- **Qt6::Network** - Network operations and HTTP requests
- **Qt6::Charts** - Data visualization and charts
- **Qt6::PrintSupport** - PDF export and printing
- **Qt6::Gui** - Graphics and UI rendering

### System Dependencies

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y \
    qt6-base-dev \
    qt6-charts-dev \
    qt6-tools-dev \
    libqt6sql6-sqlite \
    libgl1-mesa-dev \
    cmake \
    build-essential \
    pkg-config
```

#### Linux (Fedora/RHEL)
```bash
sudo dnf install -y \
    qt6-qtbase-devel \
    qt6-qtcharts-devel \
    qt6-qttools-devel \
    qt6-qtbase-mysql \
    mesa-libGL-devel \
    cmake \
    gcc-c++ \
    make
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install qt6 cmake
```

#### Windows
1. Download Qt6 from https://www.qt.io/download
2. Install Visual Studio 2019/2022 with C++ support
3. Install CMake from https://cmake.org/download/

## Module Dependencies Matrix

| Module | Qt6 Core | Widgets | Sql | Network | Charts | PrintSupport | Gui |
|--------|----------|---------|-----|---------|--------|--------------|-----|
| Authentication | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ✅ |
| User Profile | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ✅ |
| SIEM Dashboard | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ✅ |
| EDR Monitoring | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ✅ |
| Threat Intelligence | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ✅ |
| SOAR Automation | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ✅ |
| Compliance Dashboard | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ✅ |
| Forensics | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ✅ |
| Network Monitor | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ✅ |
| Wireless Pentest | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ✅ |
| Pentest Tools | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ✅ |
| Report Generator | ✅ | ✅ | ✅ | ❌ | ❌ | ✅ | ✅ |
| SOC Wallboard | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ✅ |
| Theme System | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ✅ |
| Updater | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ |

## Build Requirements

### Minimum Versions
- **CMake**: 3.16 or higher
- **Qt6**: 6.2 or higher
- **C++ Compiler**: C++17 support required
  - GCC 7+ (Linux)
  - Clang 5+ (macOS)
  - MSVC 2019+ (Windows)

### Recommended System Specs
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 2GB free space for build
- **CPU**: Multi-core processor for faster compilation

## Database Dependencies

### SQLite (Included with Qt6)
- **cyberrecon_main.db** - Events and timeline data
- **cyberrecon_auth.db** - User authentication and profiles
- **cyberrecon_cases.db** - Case management and evidence
- **soar_rules.db** - SOAR automation rules

No external database server required - SQLite is embedded.

## Network Dependencies (Optional)

### External APIs (Simulated in Demo)
- **VirusTotal API** - Threat intelligence
- **AlienVault OTX** - IOC feeds
- **Abuse.CH** - Malware intelligence
- **GitHub API** - Update checking

### Firewall Considerations
- Outbound HTTPS (443) for update checks
- Outbound HTTP/HTTPS for threat intelligence feeds (if enabled)

## Troubleshooting Dependencies

### Common Issues

#### Qt6 Not Found
```bash
# Set Qt6 path manually
export Qt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6
# or
export CMAKE_PREFIX_PATH=/usr/lib/qt6
```

#### Missing Charts Module
```bash
# Ubuntu/Debian
sudo apt install qt6-charts-dev

# Fedora
sudo dnf install qt6-qtcharts-devel
```

#### OpenGL Issues
```bash
# Ubuntu/Debian
sudo apt install libgl1-mesa-dev

# Fedora
sudo dnf install mesa-libGL-devel
```

#### Build Errors
1. Ensure all Qt6 modules are installed
2. Check CMake finds Qt6 correctly
3. Verify C++17 compiler support
4. Clean build directory and retry

## Verification Script

```bash
#!/bin/bash
echo "Checking CyberRecon Suite dependencies..."

# Check Qt6
if command -v qmake6 &> /dev/null; then
    echo "✅ Qt6 found: $(qmake6 -query QT_VERSION)"
else
    echo "❌ Qt6 not found"
fi

# Check CMake
if command -v cmake &> /dev/null; then
    echo "✅ CMake found: $(cmake --version | head -n1 | cut -d' ' -f3)"
else
    echo "❌ CMake not found"
fi

# Check compiler
if command -v g++ &> /dev/null; then
    echo "✅ GCC found: $(g++ --version | head -n1)"
elif command -v clang++ &> /dev/null; then
    echo "✅ Clang found: $(clang++ --version | head -n1)"
else
    echo "❌ No C++ compiler found"
fi

echo "Dependency check complete."
```

Run this script to verify all dependencies are properly installed before building.