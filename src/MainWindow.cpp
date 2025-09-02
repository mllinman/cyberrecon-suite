#include "MainWindow.h"
#include <QTabWidget>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <QScreen>
#include <QStyleFactory>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>

// Dashboards
#include "dashboards/SIEMDashboard.h"
#include "dashboards/EDRMonitoring.h"
#include "dashboards/ComplianceDashboard.h"
#include "dashboards/Forensics.h"
#include "dashboards/ThreatHunting.h"
#include "dashboards/IntelHub.h"
#include "dashboards/IncidentTimeline.h"
#include "dashboards/CaseManagement.h"

// Automation / SOAR
#include "automation/SOARPlaybooks.h"
#include "automation/SOARRules.h"

// Admin
#include "admin/AdminPanel.h"

// UI
#include "ui/ThemeSelector.h"
#include "ui/SOCWallboard.h"

// Additional components
#include "CollaborationChat.h"
#include "AttackGraph.h"
#include "ReportGenerator.h"
#include "MLCorrelation.h"

// Modern modules
#include "modern/ThreatIntelligence.h"
#include "modern/SecurityOrchestration.h"
#include "integrations/SplunkIntegration.h"
#include "integrations/ThreatIntelligenceFeeds.h"
#include "monitoring/NetworkMonitor.h"
#include "automation/SOARExecutor.h"
#include "tools/WirelessPentesting.h"
#include "tools/PentestingTools.h"
#include "compliance/SOC2Compliance.h"
#include "compliance/ComplianceManager.h"
#include "compliance/AuditLogger.h"

// Profile
#include "profile/UserProfile.h"

// Updater
#include "updater/Updater.h"

MainWindow::MainWindow(const QString &username, QWidget *parent) 
    : QMainWindow(parent), currentUser(username) {
    // Initialize database first
    initializeDatabase();
    
    setupMenuBar();
    setupStatusBar();
    
    tabs = new QTabWidget(this);
    setCentralWidget(tabs);
    
    // Apply default theme
    applyTheme("resources/themes/qdarkstyle.qss");
    
    // Core SOC dashboards
    tabs->addTab(new SIEMDashboard(), "SIEM Dashboard");
    tabs->addTab(new EDRMonitoring(), "EDR Monitoring");
    tabs->addTab(new ThreatIntelligence(), "🔍 Threat Intel");
    tabs->addTab(new ComplianceDashboard(), "Compliance");
    tabs->addTab(new Forensics(), "Forensics");
    tabs->addTab(new ThreatHunting(), "Threat Hunting");
    tabs->addTab(new IntelHub(), "Intel Hub");
    tabs->addTab(new IncidentTimeline(), "Incident Timeline");
    tabs->addTab(new CaseManagement(), "Case Management");
    tabs->addTab(new NetworkMonitor(), "🌐 Network Monitor");
    tabs->addTab(new SOC2Compliance(), "🛡️ SOC 2 Compliance");

    // Integrated Security Tools
    tabs->addTab(new SplunkIntegration(), "🔍 Splunk Analytics");
    tabs->addTab(new ThreatIntelligenceFeeds(), "🌐 Intel Feeds");

    // Penetration Testing Tools
    tabs->addTab(new WirelessPentesting(), "📡 Wireless Pentest");
    tabs->addTab(new PentestingTools(), "🛠️ Pentest Tools");

    // Automation
    tabs->addTab(new SOARPlaybooks(), "SOAR Playbooks");
    tabs->addTab(new SOARRules(), "SOAR Rules");
    tabs->addTab(new SecurityOrchestration(), "🔄 SOAR Advanced");
    tabs->addTab(new SOARExecutor(), "⚡ SOAR Executor");

    // Admin
    tabs->addTab(new AdminPanel(), "Admin Panel");

    // UI / Settings
    tabs->addTab(new ThemeSelector(), "Themes / Settings");
    tabs->addTab(new SOCWallboard(), "SOC Wallboard");
    
    // Additional tools
    tabs->addTab(new CollaborationChat(), "Team Chat");
    tabs->addTab(new AttackGraph(), "Attack Graph");
    tabs->addTab(new ReportGenerator(), "Reports");
    tabs->addTab(new MLCorrelation(), "ML Correlation");
    
    // User Profile
    tabs->addTab(new UserProfile(username), "👤 Profile");

    // Initialize compliance manager
    ComplianceManager *complianceManager = new ComplianceManager(this);
    complianceManager->enableSOC2Compliance(true);
    complianceManager->startComplianceMonitoring();
    
    // Updater runs in background
    new Updater(this);
    
    setupWindow();
    
    // Update status bar with user info
    statusBar()->showMessage(QString("Welcome, %1 | CyberRecon Suite v1.7").arg(username));
}

MainWindow::~MainWindow() {
    // Cleanup handled by Qt parent-child relationship
}

void MainWindow::setupMenuBar() {
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    QAction *newCaseAction = new QAction("&New Case", this);
    newCaseAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newCaseAction);
    
    QAction *exportAction = new QAction("&Export Report", this);
    exportAction->setShortcut(QKeySequence("Ctrl+E"));
    fileMenu->addAction(exportAction);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    
    QAction *fullscreenAction = new QAction("&Fullscreen", this);
    fullscreenAction->setShortcut(QKeySequence::FullScreen);
    fullscreenAction->setCheckable(true);
    connect(fullscreenAction, &QAction::triggered, this, [this](bool checked) {
        if (checked) {
            showFullScreen();
        } else {
            showNormal();
        }
    });
    viewMenu->addAction(fullscreenAction);
    
    // Tools menu
    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    
    QAction *refreshAction = new QAction("&Refresh All", this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    toolsMenu->addAction(refreshAction);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About CyberRecon Suite",
            "<h3>CyberRecon Suite v1.7</h3>"
            "<p>Advanced Cybersecurity Operations Platform</p>"
            "<p>Built with Qt6 and modern C++ technologies</p>"
            "<p>© 2024 CyberRecon Technologies</p>");
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupStatusBar() {
    statusBar()->setStyleSheet(R"(
        QStatusBar {
            background-color: #2d2d2d;
            color: #00bcd4;
            border-top: 1px solid #555;
        }
    )");
}

void MainWindow::initializeDatabase() {
    // Create main database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("cyberrecon_main.db");
    
    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", 
                             "Failed to open database: " + db.lastError().text());
        return;
    }
    
    // Create events table for timeline
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS events ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "panel TEXT NOT NULL, "
               "severity TEXT NOT NULL, "
               "message TEXT NOT NULL)");
    
    // Initialize audit logging
    AuditLogger::instance()->logSystemEvent("CyberRecon Suite started", AuditLogger::Info, 
                                           QString("Application started by user: %1").arg(currentUser));
}

void MainWindow::setupWindow() {
    setWindowTitle("CyberRecon Suite v1.7");
    resize(1400, 900);
    setMinimumSize(1000, 700);
    
    // Center window on screen
    centerWindow();
    
    // Style the tab bar
    setStyleSheet("QTabBar::tab { height: 35px; width: 140px; font-weight: bold; }");
    
    setAttribute(Qt::WA_DeleteOnClose);
    show();
}

void MainWindow::applyTheme(const QString &themeFile) {
    QFile file(themeFile);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
    } else {
        // Fallback to built-in dark style
        qApp->setStyleSheet(
            "QWidget { background-color: #2b2b2b; color: #ffffff; }"
            "QTabWidget::pane { border: 1px solid #555555; }"
            "QTabBar::tab { background-color: #3c3c3c; padding: 8px; margin: 2px; }"
            "QTabBar::tab:selected { background-color: #0078d4; }"
            "QPushButton { background-color: #0078d4; border: none; padding: 8px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #106ebe; }"
            "QTextEdit, QLineEdit { background-color: #1e1e1e; border: 1px solid #555555; padding: 4px; }"
        );
    }
}

void MainWindow::centerWindow() {
    if (QScreen *screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}