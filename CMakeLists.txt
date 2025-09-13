cmake_minimum_required(VERSION 3.16)
project(CyberReconSuite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components - All required dependencies
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Sql 
    Network 
    Charts
    PrintSupport
    Gui
)

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(src)
include_directories(src/dashboards)
include_directories(src/automation)
include_directories(src/admin)
include_directories(src/ui)
include_directories(src/updater)
include_directories(src/auth)
include_directories(src/profile)
include_directories(src/modern)
include_directories(src/payments)
include_directories(src/monitoring)
include_directories(src/tools)
include_directories(src/compliance)
include_directories(src/integrations)
include_directories(src/database)
include_directories(src/performance)

# Define source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ui/ModernMainWindow.cpp
    src/auth/LoginWindow.cpp
    src/auth/SplashScreen.cpp
    src/database/DatabaseManager.cpp
    src/performance/AsyncTaskManager.cpp
    src/modern/ErrorHandling.cpp
    src/modern/SecureNetworkManager.cpp
    src/profile/UserProfile.cpp
    src/modern/ThreatIntelligence.cpp
    src/modern/SecurityOrchestration.cpp
    src/dashboards/SIEMDashboard.cpp
    src/dashboards/EDRMonitoring.cpp
    src/dashboards/ComplianceDashboard.cpp
    src/dashboards/Forensics.cpp
    src/dashboards/ThreatHunting.cpp
    src/dashboards/IncidentTimeline.cpp
    src/dashboards/CaseManagement.cpp
    src/dashboards/IntelHub.cpp
    src/automation/SOARPlaybooks.cpp
    src/automation/SOARRules.cpp
    src/automation/SOARExecutor.cpp
    src/admin/AdminPanel.cpp
    src/ui/ThemeSelector.cpp
    src/ui/SOCWallboard.cpp
    src/updater/Updater.cpp
    src/CollaborationChat.cpp
    src/AttackGraph.cpp
    src/ReportGenerator.cpp
    src/MLCorrelation.cpp
    src/payments/StripeManager.cpp
    src/payments/PaymentDialog.cpp
    src/monitoring/NetworkMonitor.cpp
    src/tools/WirelessPentesting.cpp
    src/tools/PentestingTools.cpp
    src/compliance/SOC2Compliance.cpp
    src/compliance/ComplianceManager.cpp
    src/compliance/AuditLogger.cpp
    src/integrations/SplunkIntegration.cpp
    src/integrations/ThreatIntelligenceFeeds.cpp
)

# Define header files
set(HEADERS
    src/MainWindow.h
    src/ui/ModernMainWindow.h
    src/auth/LoginWindow.h
    src/auth/SplashScreen.h
    src/profile/UserProfile.h
    src/modern/ThreatIntelligence.h
    src/modern/SecurityOrchestration.h
    src/dashboards/SIEMDashboard.h
    src/dashboards/EDRMonitoring.h
    src/dashboards/ComplianceDashboard.h
    src/dashboards/Forensics.h
    src/dashboards/ThreatHunting.h
    src/dashboards/IncidentTimeline.h
    src/dashboards/CaseManagement.h
    src/dashboards/IntelHub.h
    src/automation/SOARPlaybooks.h
    src/automation/SOARRules.h
    src/automation/SOARExecutor.h
    src/admin/AdminPanel.h
    src/ui/ThemeSelector.h
    src/ui/SOCWallboard.h
    src/updater/Updater.h
    src/CollaborationChat.h
    src/AttackGraph.h
    src/ReportGenerator.h
    src/MLCorrelation.h
    src/payments/StripeManager.h
    src/payments/PaymentDialog.h
    src/monitoring/NetworkMonitor.h
    src/tools/WirelessPentesting.h
    src/tools/PentestingTools.h
    src/compliance/SOC2Compliance.h
    src/compliance/ComplianceManager.h
    src/compliance/AuditLogger.h
    src/integrations/SplunkIntegration.h
    src/integrations/ThreatIntelligenceFeeds.h
    src/database/DatabaseManager.h
    src/performance/AsyncTaskManager.h
    src/modern/ErrorHandling.h
    src/modern/SecureNetworkManager.h
)

# Create executable
add_executable(CyberReconSuite ${SOURCES} ${HEADERS})

# Link Qt6 libraries - All required dependencies
target_link_libraries(CyberReconSuite PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Charts
    Qt6::PrintSupport
    Qt6::Gui
)

# Compiler-specific options
if(MSVC)
    target_compile_options(CyberReconSuite PRIVATE /W4)
else()
    target_compile_options(CyberReconSuite PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set target properties
set_target_properties(CyberReconSuite PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install rules
install(TARGETS CyberReconSuite
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)