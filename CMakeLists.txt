cmake_minimum_required(VERSION 3.16)
project(CyberReconSuite VERSION 1.7.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Sql
    Network
    Charts
    PrintSupport
)

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(src)

# Collect all source files
set(SOURCES
    # Main application
    src/main.cpp
    src/MainWindow.cpp
    src/AttackGraph.cpp
    src/CollaborationChat.cpp
    src/MLCorrelation.cpp
    src/ReportGenerator.cpp
    
    # Authentication
    src/auth/LoginWindow.cpp
    src/auth/SplashScreen.cpp
    
    # User Interface
    src/ui/ModernMainWindow.cpp
    src/ui/ThemeSelector.cpp
    src/ui/SOCWallboard.cpp
    
    # Administration
    src/admin/AdminPanel.cpp
    
    # User Profile
    src/profile/UserProfile.cpp
    
    # Automation
    src/automation/SOARExecutor.cpp
    src/automation/SOARPlaybooks.cpp
    src/automation/SOARRules.cpp
    
    # Compliance
    src/compliance/AuditLogger.cpp
    src/compliance/ComplianceManager.cpp
    src/compliance/SOC2Compliance.cpp
    
    # Dashboards
    src/dashboards/SIEMDashboard.cpp
    src/dashboards/EDRMonitoring.cpp
    src/dashboards/IntelHub.cpp
    src/dashboards/ThreatHunting.cpp
    src/dashboards/Forensics.cpp
    src/dashboards/IncidentTimeline.cpp
    src/dashboards/CaseManagement.cpp
    src/dashboards/ComplianceDashboard.cpp
    
    # Integrations
    src/integrations/SplunkIntegration.cpp
    src/integrations/CrowdStrikeEDR.cpp
    src/integrations/ThreatIntelligenceFeeds.cpp
    
    # Modern Security
    src/modern/ThreatIntelligence.cpp
    src/modern/SecurityOrchestration.cpp
    
    # Monitoring
    src/monitoring/NetworkMonitor.cpp
    
    # Payment System
    src/payments/PaymentDialog.cpp
    src/payments/StripeManager.cpp
    
    # Security Tools
    src/tools/PentestingTools.cpp
    src/tools/WirelessPentesting.cpp
    
    # Updater
    src/updater/Updater.cpp
)

# Create executable
add_executable(CyberReconSuite ${SOURCES})

# Link Qt6 libraries
target_link_libraries(CyberReconSuite PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Charts
    Qt6::PrintSupport
)

# Set target properties
set_target_properties(CyberReconSuite PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install rules
install(TARGETS CyberReconSuite
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)