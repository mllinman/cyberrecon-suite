# CyberRecon Suite - Development Guide

## Development Environment Setup

### Prerequisites

#### Required Software
- **C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **Qt6**: Complete development installation (6.0+)
- **CMake**: Version 3.16 or higher
- **Git**: Latest version for version control

#### Optional Tools
- **Qt Creator**: Recommended IDE for Qt development
- **Visual Studio Code**: With C++ and CMake extensions
- **Ninja**: Fast build system (alternative to Make)
- **ccache**: Compiler cache for faster rebuilds

### Platform-Specific Setup

#### Linux (Ubuntu/Debian)
```bash
# Install development tools
sudo apt update
sudo apt install -y build-essential git cmake ninja-build ccache

# Install Qt6 development packages
sudo apt install -y qt6-base-dev qt6-charts-dev qt6-tools-dev

# Optional: Install additional Qt6 modules
sudo apt install -y qt6-multimedia-dev qt6-networkauth-dev
```

#### Windows
```powershell
# Install Visual Studio 2019+ with C++ support
# Download and install Qt6 from https://www.qt.io/download

# Install CMake from https://cmake.org/download/
# Install Git from https://git-scm.com/download/win

# Add Qt6 and CMake to system PATH
# Set Qt6_DIR environment variable to Qt installation path
```

#### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install development dependencies
brew install cmake ninja qt6 git
```

### Project Setup

#### Clone and Initialize
```bash
# Clone the repository
git clone https://github.com/mllinman/cyberrecon-suite.git
cd cyberrecon-suite

# Create development build
mkdir build-dev && cd build-dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -GNinja
ninja
```

#### IDE Configuration

**Qt Creator Setup:**
1. Open Qt Creator
2. File → Open File or Project → Select CMakeLists.txt
3. Configure build settings for Debug and Release
4. Set up code style to match project conventions

**VS Code Setup:**
```json
// .vscode/settings.json
{
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "cmake.buildDirectory": "${workspaceFolder}/build-dev",
    "cmake.generator": "Ninja"
}
```

## Project Architecture

### Directory Structure
```
cyberrecon-suite/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── auth/              # Authentication system
│   ├── monitoring/        # Network monitoring
│   ├── dashboards/        # Security dashboards
│   ├── automation/        # SOAR automation
│   └── ...
├── test/                  # Test suite
├── docs/                  # Documentation
├── assets/                # Visual assets
├── CMakeLists.txt         # Main CMake file
└── README.md             # Project README
```

### Module Organization
Each major feature is organized into its own module:
- **Header files**: Interface definitions and public API
- **Source files**: Implementation details
- **CMakeLists.txt**: Module-specific build configuration
- **README.md**: Module documentation and usage

### Coding Standards

#### C++ Guidelines
```cpp
// Use modern C++17 features
#include <memory>
#include <string_view>
#include <optional>

// Class naming: PascalCase
class SecurityDashboard {
public:
    // Method naming: camelCase
    void updateEventStream();
    
    // Member variables: m_ prefix with camelCase
    std::unique_ptr<EventProcessor> m_processor;
    
private:
    // Constants: UPPER_CASE
    static constexpr int MAX_EVENTS = 10000;
};
```

#### Qt-Specific Conventions
```cpp
// Use Qt object hierarchy for memory management
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    
private slots:
    void onEventReceived();
    
private:
    // Qt objects as children for automatic cleanup
    QTimer *m_updateTimer;
    QNetworkAccessManager *m_networkManager;
};
```

### Building and Testing

#### Development Builds
```bash
# Debug build with all debugging symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Run with debugging
gdb ./CyberReconSuite
# or
lldb ./CyberReconSuite
```

#### Performance Builds
```bash
# Optimized release build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG"
make -j$(nproc)

# Profile performance
perf record ./CyberReconSuite
perf report
```

### Testing Framework

#### Unit Tests
```cpp
#include <QtTest/QtTest>

class SecurityTest : public QObject {
    Q_OBJECT
    
private slots:
    void testEventProcessing();
    void testThreatDetection();
    
private:
    // Test data and helpers
};

// Test implementation
void SecurityTest::testEventProcessing() {
    EventProcessor processor;
    SecurityEvent event("test_event");
    
    QVERIFY(processor.processEvent(event));
    QCOMPARE(processor.getEventCount(), 1);
}

QTEST_MAIN(SecurityTest)
#include "SecurityTest.moc"
```

#### Integration Tests
- Test complete workflows end-to-end
- Validate database operations
- Test network communication
- Verify UI behavior

### Debugging and Profiling

#### Debugging Tools
```bash
# Memory debugging with Valgrind
valgrind --tool=memcheck --leak-check=full ./CyberReconSuite

# Thread debugging
valgrind --tool=helgrind ./CyberReconSuite

# Qt-specific debugging
export QT_LOGGING_RULES="*.debug=true"
./CyberReconSuite
```

#### Performance Profiling
```bash
# CPU profiling
perf record -g ./CyberReconSuite
perf report

# Memory profiling
valgrind --tool=massif ./CyberReconSuite
ms_print massif.out.* | head -20
```

### Contributing Workflow

#### Branch Strategy
```bash
# Create feature branch
git checkout -b feature/your-feature-name

# Make changes and commit frequently
git add .
git commit -m "Add feature component X"

# Push and create pull request
git push origin feature/your-feature-name
```

#### Code Review Process
1. **Automated Checks**: CI/CD pipeline runs tests and linting
2. **Peer Review**: Other developers review code quality and design
3. **Security Review**: Security-focused code analysis
4. **Documentation**: Ensure documentation is updated
5. **Testing**: Verify comprehensive test coverage

### Release Process

#### Version Management
- Follow semantic versioning (MAJOR.MINOR.PATCH)
- Update version numbers in CMakeLists.txt and documentation
- Tag releases with git tags

#### Build Pipeline
```bash
# Create release build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run full test suite
ctest --parallel $(nproc)

# Package for distribution
make package
```

### Documentation

#### Code Documentation
```cpp
/**
 * @brief Processes security events and generates alerts
 * 
 * This class handles the core event processing logic for the SIEM engine.
 * It correlates events, applies rules, and generates appropriate alerts.
 * 
 * @see SecurityEvent, AlertManager
 * @since 1.7.0
 */
class EventProcessor {
public:
    /**
     * @brief Process a single security event
     * @param event The security event to process
     * @return true if processing was successful, false otherwise
     * 
     * @note This method is thread-safe and can be called concurrently
     * @warning Large events may cause performance impact
     */
    bool processEvent(const SecurityEvent& event);
};
```

#### API Documentation
- Use Doxygen for generating API documentation
- Include usage examples in documentation
- Maintain up-to-date developer guides

### Getting Help

#### Developer Resources
- **GitHub Discussions**: Technical questions and discussions
- **Developer Email**: dev@bulletdropstudios.com
- **Code Reviews**: Submit pull requests for feedback
- **Documentation**: Comprehensive guides in `/docs` directory

#### Best Practices
- Write self-documenting code with clear variable names
- Use const correctness and RAII principles
- Prefer standard library and Qt containers over raw pointers
- Follow the project's error handling conventions
- Write tests for all new functionality

---

*This development guide is continuously updated. Please check for the latest version when contributing to the project.*