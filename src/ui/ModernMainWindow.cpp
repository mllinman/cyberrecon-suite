#include "ModernMainWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QTimer>

// Import all dashboard modules
#include "dashboards/SIEMDashboard.h"
#include "dashboards/EDRMonitoring.h"
#include "dashboards/ComplianceDashboard.h"
#include "dashboards/Forensics.h"
#include "dashboards/ThreatHunting.h"
#include "dashboards/IncidentTimeline.h"
#include "dashboards/CaseManagement.h"
#include "dashboards/IntelHub.h"

// Modern modules
#include "modern/ThreatIntelligence.h"
#include "modern/SecurityOrchestration.h"

// Other modules
#include "automation/SOARPlaybooks.h"
#include "automation/SOARRules.h"
#include "admin/AdminPanel.h"
#include "ui/ThemeSelector.h"
#include "ui/SOCWallboard.h"
#include "CollaborationChat.h"
#include "AttackGraph.h"
#include "ReportGenerator.h"
#include "MLCorrelation.h"
#include "profile/UserProfile.h"
#include "monitoring/NetworkMonitor.h"
#include "automation/SOARExecutor.h"
#include "tools/WirelessPentesting.h"
#include "tools/PentestingTools.h"
#include "compliance/SOC2Compliance.h"
#include "integrations/SplunkIntegration.h"

ModernMainWindow::ModernMainWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent), currentUser(username) {
    
    setupModernUI();
    applyModernTheme();
    
    // Center window
    if (QScreen *screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - 1400) / 2;
        int y = (screenGeometry.height() - 900) / 2;
        move(x, y);
    }
    
    setWindowTitle("CyberRecon Suite v1.7 - Advanced Cybersecurity Operations");
    resize(1400, 900);
    setMinimumSize(1200, 800);
    show();
}

void ModernMainWindow::setupModernUI() {
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    createSidebar();
    
    // Right side content area
    auto *rightArea = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    createTopBar();
    createModules();
    
    rightLayout->addWidget(topBar);
    rightLayout->addWidget(moduleStack, 1);
    
    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(rightArea, 1);
}

void ModernMainWindow::createSidebar() {
    sidebar = new QFrame();
    sidebar->setObjectName("modernSidebar");
    sidebar->setFixedWidth(280);
    
    sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);
    sidebarLayout->setSpacing(5);
    
    // Logo section
    auto *logoFrame = new QFrame();
    logoFrame->setObjectName("logoFrame");
    auto *logoLayout = new QVBoxLayout(logoFrame);
    
    auto *logoLabel = new QLabel("🛡️ CyberRecon");
    logoLabel->setObjectName("logoLabel");
    logoLabel->setAlignment(Qt::AlignCenter);
    
    auto *versionLabel = new QLabel("Suite v1.7");
    versionLabel->setObjectName("versionLabel");
    versionLabel->setAlignment(Qt::AlignCenter);
    
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(versionLabel);
    sidebarLayout->addWidget(logoFrame);
    
    addSidebarSeparator();
    
    // Navigation buttons
    sidebarButtons = new QButtonGroup(this);
    connect(sidebarButtons, QOverload<int>::of(&QButtonGroup::idClicked), 
            this, &ModernMainWindow::switchToModule);
    
    // Dashboard Group
    auto *dashLabel = new QLabel("DASHBOARDS");
    dashLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(dashLabel);
    
    addSidebarButton("📊", "SIEM Dashboard", "Security Information & Event Management", 0);
    addSidebarButton("🖥️", "EDR Monitoring", "Endpoint Detection & Response", 1);
    addSidebarButton("📋", "Compliance", "Regulatory Compliance Tracking", 2);
    addSidebarButton("🔍", "Forensics", "Digital Investigation Tools", 3);
    addSidebarButton("📈", "SOC Wallboard", "Security Operations Center Overview", 4);
    addSidebarButton("🌐", "Network Monitor", "Packet Capture & Network Scanning", 5);
    
    addSidebarSeparator();
    
    // Intelligence Group
    auto *intelLabel = new QLabel("INTELLIGENCE");
    intelLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(intelLabel);
    
    addSidebarButton("🎯", "Threat Intel", "Advanced Threat Intelligence", 6);
    addSidebarButton("🔎", "Threat Hunting", "Proactive Threat Detection", 7);
    addSidebarButton("🌐", "Intel Hub", "Threat Intelligence Feeds", 8);
    addSidebarButton("🔍", "Splunk Analytics", "Search & Analytics Platform", 24);
    addSidebarButton("🧠", "ML Correlation", "Machine Learning Analysis", 9);
    
    addSidebarSeparator();
    
    // Automation Group
    auto *autoLabel = new QLabel("AUTOMATION");
    autoLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(autoLabel);
    
    addSidebarButton("🔄", "SOAR Advanced", "Security Orchestration Platform", 10);
    addSidebarButton("📝", "Playbooks", "Automated Response Workflows", 11);
    addSidebarButton("⚙️", "SOAR Rules", "Automation Rule Engine", 12);
    addSidebarButton("⚡", "SOAR Executor", "Workflow Execution Engine", 13);
    addSidebarButton("🕸️", "Attack Graph", "Threat Visualization", 14);
    
    addSidebarSeparator();
    
    // Tools Group
    auto *toolsLabel = new QLabel("PENETRATION TESTING");
    toolsLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(toolsLabel);
    
    addSidebarButton("📡", "Wireless Pentest", "WiFi & Bluetooth Testing", 15);
    addSidebarButton("🛠️", "Pentest Tools", "Web App & Password Tools", 16);
    
    addSidebarSeparator();
    
    // Operations Group
    auto *opsLabel = new QLabel("OPERATIONS");
    opsLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(opsLabel);
    
    addSidebarButton("📋", "Incidents", "Incident Timeline", 17);
    addSidebarButton("💼", "Cases", "Case Management", 18);
    addSidebarButton("💬", "Team Chat", "Collaboration Platform", 19);
    addSidebarButton("📊", "Reports", "Security Reports", 20);
    
    addSidebarSeparator();
    
    // Compliance Group
    auto *complianceLabel = new QLabel("COMPLIANCE");
    complianceLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(complianceLabel);
    
    addSidebarButton("🛡️", "SOC 2", "SOC 2 Trust Service Criteria", 23);
    
    addSidebarSeparator();
    
    // Admin Group
    auto *adminLabel = new QLabel("ADMINISTRATION");
    adminLabel->setObjectName("sectionLabel");
    sidebarLayout->addWidget(adminLabel);
    
    addSidebarButton("👥", "Admin Panel", "User & System Management", 21);
    addSidebarButton("🎨", "Themes", "Interface Customization", 22);
    
    sidebarLayout->addStretch();
    
    // Set first button as active
    if (!sidebarButtons->buttons().isEmpty()) {
        sidebarButtons->button(0)->setChecked(true);
    }
}

void ModernMainWindow::createTopBar() {
    topBar = new QFrame();
    topBar->setObjectName("modernTopBar");
    topBar->setFixedHeight(70);
    
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(30, 10, 30, 10);
    
    // Left side - current module info
    auto *moduleInfo = new QLabel("Security Information & Event Management");
    moduleInfo->setObjectName("moduleTitle");
    topLayout->addWidget(moduleInfo);
    
    topLayout->addStretch();
    
    // Right side - user profile area
    auto *userArea = new QFrame();
    userArea->setObjectName("userArea");
    auto *userLayout = new QHBoxLayout(userArea);
    userLayout->setContentsMargins(15, 5, 15, 5);
    userLayout->setSpacing(15);
    
    // User avatar
    userAvatar = new QLabel();
    userAvatar->setObjectName("userAvatar");
    userAvatar->setFixedSize(40, 40);
    userAvatar->setText("👤");
    userAvatar->setAlignment(Qt::AlignCenter);
    userLayout->addWidget(userAvatar);
    
    // User info
    auto *userInfoLayout = new QVBoxLayout();
    userInfoLayout->setSpacing(2);
    
    userName = new QLabel(currentUser);
    userName->setObjectName("userName");
    
    auto *userRole = new QLabel("Security Analyst");
    userRole->setObjectName("userRole");
    
    userInfoLayout->addWidget(userName);
    userInfoLayout->addWidget(userRole);
    userLayout->addLayout(userInfoLayout);
    
    // Action buttons
    profileBtn = new QPushButton("👤");
    profileBtn->setObjectName("topBarButton");
    profileBtn->setToolTip("User Profile");
    profileBtn->setFixedSize(35, 35);
    
    settingsBtn = new QPushButton("⚙️");
    settingsBtn->setObjectName("topBarButton");
    settingsBtn->setToolTip("Settings");
    settingsBtn->setFixedSize(35, 35);
    
    logoutBtn = new QPushButton("🚪");
    logoutBtn->setObjectName("topBarButton");
    logoutBtn->setToolTip("Logout");
    logoutBtn->setFixedSize(35, 35);
    
    userLayout->addWidget(profileBtn);
    userLayout->addWidget(settingsBtn);
    userLayout->addWidget(logoutBtn);
    
    topLayout->addWidget(userArea);
    
    // Connect signals
    connect(profileBtn, &QPushButton::clicked, this, &ModernMainWindow::showUserProfile);
    connect(settingsBtn, &QPushButton::clicked, this, &ModernMainWindow::showSettings);
    connect(logoutBtn, &QPushButton::clicked, this, &ModernMainWindow::logout);
}

void ModernMainWindow::createModules() {
    moduleStack = new QStackedWidget();
    
    // Add all modules to stack
    moduleStack->addWidget(new SIEMDashboard());           // 0
    moduleStack->addWidget(new EDRMonitoring());           // 1
    moduleStack->addWidget(new ComplianceDashboard());     // 2
    moduleStack->addWidget(new Forensics());              // 3
    moduleStack->addWidget(new SOCWallboard());            // 4
    moduleStack->addWidget(new NetworkMonitor());          // 5
    moduleStack->addWidget(new ThreatIntelligence());      // 6
    moduleStack->addWidget(new ThreatHunting());           // 7
    moduleStack->addWidget(new IntelHub());                // 8
    moduleStack->addWidget(new MLCorrelation());           // 9
    moduleStack->addWidget(new SecurityOrchestration());   // 10
    moduleStack->addWidget(new SOARPlaybooks());           // 11
    moduleStack->addWidget(new SOARRules());               // 12
    moduleStack->addWidget(new SOARExecutor());            // 13
    moduleStack->addWidget(new AttackGraph());             // 14
    moduleStack->addWidget(new WirelessPentesting());      // 15
    moduleStack->addWidget(new PentestingTools());         // 16
    moduleStack->addWidget(new IncidentTimeline());        // 17
    moduleStack->addWidget(new CaseManagement());          // 18
    moduleStack->addWidget(new CollaborationChat());       // 19
    moduleStack->addWidget(new ReportGenerator());         // 20
    moduleStack->addWidget(new AdminPanel());              // 21
    moduleStack->addWidget(new ThemeSelector());           // 22
    moduleStack->addWidget(new SOC2Compliance());          // 23
    moduleStack->addWidget(new SplunkIntegration());        // 24
}

void ModernMainWindow::addSidebarButton(const QString &icon, const QString &text, const QString &tooltip, int index) {
    auto *button = new QPushButton();
    button->setObjectName("sidebarButton");
    button->setCheckable(true);
    button->setToolTip(tooltip);
    button->setFixedHeight(50);
    
    auto *buttonLayout = new QHBoxLayout(button);
    buttonLayout->setContentsMargins(20, 0, 20, 0);
    buttonLayout->setSpacing(15);
    
    auto *iconLabel = new QLabel(icon);
    iconLabel->setObjectName("buttonIcon");
    iconLabel->setFixedSize(24, 24);
    iconLabel->setAlignment(Qt::AlignCenter);
    
    auto *textLabel = new QLabel(text);
    textLabel->setObjectName("buttonText");
    
    buttonLayout->addWidget(iconLabel);
    buttonLayout->addWidget(textLabel);
    buttonLayout->addStretch();
    
    sidebarButtons->addButton(button, index);
    sidebarLayout->addWidget(button);
}

void ModernMainWindow::addSidebarSeparator() {
    auto *separator = new QFrame();
    separator->setObjectName("sidebarSeparator");
    separator->setFixedHeight(1);
    sidebarLayout->addWidget(separator);
    sidebarLayout->addSpacing(10);
}

void ModernMainWindow::switchToModule(int moduleIndex) {
    if (moduleIndex >= 0 && moduleIndex < moduleStack->count()) {
        moduleStack->setCurrentIndex(moduleIndex);
        
        // Update top bar title based on module
        QStringList moduleTitles = {
            "Security Information & Event Management",
            "Endpoint Detection & Response",
            "Regulatory Compliance Dashboard",
            "Digital Forensics & Investigation",
            "Security Operations Center Overview",
            "Network Monitoring & Analysis Suite",
            "Advanced Threat Intelligence Platform",
            "Proactive Threat Hunting Console",
            "Threat Intelligence Hub",
            "Machine Learning Event Correlation",
            "Security Orchestration & Automated Response",
            "SOAR Playbook Management",
            "Automation Rule Engine",
            "SOAR Workflow Execution Engine",
            "Attack Graph Visualization",
            "Wireless Penetration Testing Suite",
            "Penetration Testing Toolkit",
            "Incident Timeline & Management",
            "Case Management System",
            "Team Collaboration Platform",
            "Security Report Generator",
            "System Administration Panel",
            "Theme & Interface Customization",
            "SOC 2 Compliance Management",
            "Splunk-Style Search & Analytics Platform",
            "CrowdStrike-Style Endpoint Detection & Response"
        };
        
        if (moduleIndex < moduleTitles.size()) {
            auto *moduleTitle = topBar->findChild<QLabel*>("moduleTitle");
            if (moduleTitle) {
                moduleTitle->setText(moduleTitles[moduleIndex]);
            }
        }
    }
}

void ModernMainWindow::showUserProfile() {
    // Create and show user profile dialog
    auto *profileDialog = new UserProfile(currentUser, this);
    profileDialog->setWindowModality(Qt::ApplicationModal);
    profileDialog->show();
}

void ModernMainWindow::showSettings() {
    // Switch to theme selector module
    switchToModule(22);
    sidebarButtons->button(22)->setChecked(true);
}

void ModernMainWindow::logout() {
    int ret = QMessageBox::question(this, "Logout", 
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void ModernMainWindow::applyModernTheme() {
    setStyleSheet(R"(
        /* Main Window */
        QMainWindow {
            background-color: #0f0f0f;
            color: #ffffff;
        }
        
        /* Modern Sidebar */
        QFrame#modernSidebar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #1a1a1a, stop:1 #2d2d2d);
            border-right: 2px solid #333333;
        }
        
        /* Logo Section */
        QFrame#logoFrame {
            background-color: rgba(0, 188, 212, 0.1);
            border-radius: 10px;
            margin: 10px;
            padding: 15px;
        }
        
        QLabel#logoLabel {
            color: #00bcd4;
            font-size: 24px;
            font-weight: bold;
            margin: 5px;
        }
        
        QLabel#versionLabel {
            color: #888888;
            font-size: 12px;
            margin: 2px;
        }
        
        /* Section Labels */
        QLabel#sectionLabel {
            color: #00bcd4;
            font-size: 11px;
            font-weight: bold;
            margin: 15px 20px 5px 20px;
            letter-spacing: 1px;
        }
        
        /* Sidebar Buttons */
        QPushButton#sidebarButton {
            background-color: transparent;
            border: none;
            border-radius: 8px;
            margin: 2px 10px;
            text-align: left;
            padding: 0px;
        }
        
        QPushButton#sidebarButton:hover {
            background-color: rgba(0, 188, 212, 0.1);
        }
        
        QPushButton#sidebarButton:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(0, 188, 212, 0.3), stop:1 rgba(0, 188, 212, 0.1));
            border-left: 4px solid #00bcd4;
        }
        
        QLabel#buttonIcon {
            font-size: 18px;
            color: #ffffff;
        }
        
        QLabel#buttonText {
            color: #ffffff;
            font-size: 13px;
            font-weight: 500;
        }
        
        QPushButton#sidebarButton:checked QLabel#buttonIcon {
            color: #00bcd4;
        }
        
        QPushButton#sidebarButton:checked QLabel#buttonText {
            color: #00bcd4;
            font-weight: bold;
        }
        
        /* Sidebar Separator */
        QFrame#sidebarSeparator {
            background-color: #333333;
            margin: 0px 20px;
        }
        
        /* Modern Top Bar */
        QFrame#modernTopBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2d2d2d, stop:1 #1a1a1a);
            border-bottom: 2px solid #333333;
        }
        
        QLabel#moduleTitle {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
        }
        
        /* User Area */
        QFrame#userArea {
            background-color: rgba(0, 188, 212, 0.1);
            border: 1px solid rgba(0, 188, 212, 0.3);
            border-radius: 25px;
        }
        
        QLabel#userAvatar {
            background-color: #00bcd4;
            border-radius: 20px;
            font-size: 20px;
            color: #000000;
        }
        
        QLabel#userName {
            color: #ffffff;
            font-size: 14px;
            font-weight: bold;
        }
        
        QLabel#userRole {
            color: #888888;
            font-size: 11px;
        }
        
        /* Top Bar Buttons */
        QPushButton#topBarButton {
            background-color: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 17px;
            font-size: 16px;
            color: #ffffff;
        }
        
        QPushButton#topBarButton:hover {
            background-color: rgba(0, 188, 212, 0.3);
            border-color: #00bcd4;
        }
        
        QPushButton#topBarButton:pressed {
            background-color: rgba(0, 188, 212, 0.5);
        }
        
        /* Module Content Area */
        QStackedWidget {
            background-color: #0f0f0f;
            border: none;
        }
    )");
}