#include "CrowdStrikeEDR.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QRandomGenerator>
#include <QApplication>

CrowdStrikeEDR::CrowdStrikeEDR(QWidget *parent) 
    : QWidget(parent), totalEndpoints(0), totalThreats(0), quarantinedEndpoints(0) {
    
    setupUI();
    setupStyling();
    initializeDatabase();
    loadEndpointData();
    
    // Auto-refresh timer
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &CrowdStrikeEDR::refreshEndpoints);
    refreshTimer->start(15000); // Refresh every 15 seconds
}

void CrowdStrikeEDR::setupUI() {
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("🦅 CrowdStrike-Style Endpoint Detection & Response");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Statistics Dashboard
    auto *statsGroup = new QGroupBox("Endpoint Protection Overview");
    auto *statsLayout = new QGridLayout(statsGroup);
    
    endpointCount = new QLabel("0");
    endpointCount->setStyleSheet("font-size: 24px; font-weight: bold; color: #4caf50;");
    statsLayout->addWidget(new QLabel("Protected Endpoints:"), 0, 0);
    statsLayout->addWidget(endpointCount, 0, 1);
    
    threatCount = new QLabel("0");
    threatCount->setStyleSheet("font-size: 24px; font-weight: bold; color: #f44336;");
    statsLayout->addWidget(new QLabel("Active Threats:"), 0, 2);
    statsLayout->addWidget(threatCount, 0, 3);
    
    quarantineCount = new QLabel("0");
    quarantineCount->setStyleSheet("font-size: 24px; font-weight: bold; color: #ff9800;");
    statsLayout->addWidget(new QLabel("Quarantined:"), 1, 0);
    statsLayout->addWidget(quarantineCount, 1, 1);
    
    auto *lastUpdateLabel = new QLabel(QString("Last Update: %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    lastUpdateLabel->setStyleSheet("color: #888;");
    statsLayout->addWidget(lastUpdateLabel, 1, 2, 1, 2);
    
    layout->addWidget(statsGroup);
    
    // Control Panel
    auto *controlGroup = new QGroupBox("Endpoint Management Controls");
    auto *controlLayout = new QHBoxLayout(controlGroup);
    
    auto *scanBtn = new QPushButton("🔍 Scan All Endpoints");
    scanBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 10px 20px; border-radius: 6px; font-weight: bold; }");
    connect(scanBtn, &QPushButton::clicked, this, &CrowdStrikeEDR::scanEndpoints);
    controlLayout->addWidget(scanBtn);
    
    auto *quarantineBtn = new QPushButton("🔒 Quarantine Selected");
    quarantineBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(quarantineBtn, &QPushButton::clicked, this, &CrowdStrikeEDR::quarantineEndpoint);
    controlLayout->addWidget(quarantineBtn);
    
    auto *releaseBtn = new QPushButton("🔓 Release Quarantine");
    releaseBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(releaseBtn, &QPushButton::clicked, this, &CrowdStrikeEDR::releaseQuarantine);
    controlLayout->addWidget(releaseBtn);
    
    policyCombo = new QComboBox();
    policyCombo->addItems({"Standard Protection", "High Security", "Prevent Mode", "Detection Only", "Custom Policy"});
    controlLayout->addWidget(policyCombo);
    
    auto *deployBtn = new QPushButton("📋 Deploy Policy");
    deployBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(deployBtn, &QPushButton::clicked, this, &CrowdStrikeEDR::deployPolicy);
    controlLayout->addWidget(deployBtn);
    
    layout->addWidget(controlGroup);
    
    // Progress bar
    scanProgress = new QProgressBar();
    scanProgress->setVisible(false);
    scanProgress->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: white;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #2196f3, stop:1 #64b5f6);
            border-radius: 3px;
        }
    )");
    layout->addWidget(scanProgress);
    
    // Main content area
    auto *mainSplitter = new QSplitter(Qt::Horizontal);
    
    // Left side - Endpoints and Threats
    auto *leftWidget = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftWidget);
    
    // Endpoints table
    auto *endpointsLabel = new QLabel("🖥️ Protected Endpoints");
    endpointsLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    leftLayout->addWidget(endpointsLabel);
    
    endpointsTable = new QTableWidget(0, 6);
    endpointsTable->setHorizontalHeaderLabels({"Hostname", "IP Address", "OS", "Status", "Last Seen", "Threats"});
    endpointsTable->horizontalHeader()->setStretchLastSection(true);
    endpointsTable->setAlternatingRowColors(true);
    endpointsTable->setMaximumHeight(250);
    leftLayout->addWidget(endpointsTable);
    
    // Threats table
    auto *threatsLabel = new QLabel("⚠️ Active Threats");
    threatsLabel->setStyleSheet("font-weight: bold; color: #f44336; margin: 5px;");
    leftLayout->addWidget(threatsLabel);
    
    threatsTable = new QTableWidget(0, 5);
    threatsTable->setHorizontalHeaderLabels({"Endpoint", "Threat Type", "Severity", "Status", "Detected"});
    threatsTable->horizontalHeader()->setStretchLastSection(true);
    threatsTable->setAlternatingRowColors(true);
    leftLayout->addWidget(threatsTable);
    
    mainSplitter->addWidget(leftWidget);
    
    // Right side - Activity Log and Analysis
    auto *rightWidget = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightWidget);
    
    // Threat Hunting
    auto *huntGroup = new QGroupBox("🎯 Threat Hunting");
    auto *huntLayout = new QVBoxLayout(huntGroup);
    
    auto *huntControlsLayout = new QHBoxLayout();
    huntQuery = new QLineEdit();
    huntQuery->setPlaceholderText("Enter threat hunting query (e.g., process_name:powershell.exe AND command_line:*base64*)");
    huntControlsLayout->addWidget(huntQuery);
    
    auto *huntBtn = new QPushButton("🔍 Hunt");
    huntBtn->setStyleSheet("QPushButton { background-color: #9c27b0; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(huntBtn, &QPushButton::clicked, this, &CrowdStrikeEDR::huntThreats);
    huntControlsLayout->addWidget(huntBtn);
    
    huntLayout->addLayout(huntControlsLayout);
    
    // Hash blocking
    auto *hashLayout = new QHBoxLayout();
    hashInput = new QLineEdit();
    hashInput->setPlaceholderText("Enter file hash to block (SHA256)");
    hashLayout->addWidget(hashInput);
    
    auto *blockBtn = new QPushButton("🚫 Block Hash");
    blockBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(blockBtn, &QPushButton::clicked, this, &CrowdStrikeEDR::blockHash);
    hashLayout->addWidget(blockBtn);
    
    huntLayout->addLayout(hashLayout);
    rightLayout->addWidget(huntGroup);
    
    // Activity log
    auto *activityLabel = new QLabel("📝 EDR Activity Log");
    activityLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    rightLayout->addWidget(activityLabel);
    
    activityLog = new QTextEdit();
    activityLog->setReadOnly(true);
    activityLog->setMaximumHeight(200);
    activityLog->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: 'Consolas', monospace;
            font-size: 11px;
            border: 1px solid #555;
            border-radius: 6px;
            padding: 10px;
        }
    )");
    rightLayout->addWidget(activityLog);
    
    // Process analysis
    auto *processLabel = new QLabel("🔬 Process Analysis");
    processLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    rightLayout->addWidget(processLabel);
    
    processAnalysis = new QTextEdit();
    processAnalysis->setReadOnly(true);
    processAnalysis->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: 'Consolas', monospace;
            font-size: 11px;
            border: 1px solid #555;
            border-radius: 6px;
            padding: 10px;
        }
    )");
    rightLayout->addWidget(processAnalysis);
    
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setStretchFactor(0, 2);
    mainSplitter->setStretchFactor(1, 1);
    
    layout->addWidget(mainSplitter);
    
    // Connect table selection
    connect(endpointsTable, &QTableWidget::itemSelectionChanged, this, &CrowdStrikeEDR::viewEndpointDetails);
    connect(huntQuery, &QLineEdit::returnPressed, this, &CrowdStrikeEDR::huntThreats);
    
    // Initial log messages
    activityLog->append("<font color='#00bcd4'>[EDR] CrowdStrike-style EDR platform initialized</font>");
    activityLog->append("<font color='#4caf50'>[EDR] Endpoint protection active</font>");
}

void CrowdStrikeEDR::setupStyling() {
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 15px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px 0 8px;
            color: #00bcd4;
            font-size: 14px;
        }
        
        QTableWidget {
            background-color: #2d2d2d;
            alternate-background-color: #3c3c3c;
            gridline-color: #555;
            color: #fff;
            border: 1px solid #555;
            border-radius: 6px;
        }
        
        QTableWidget::item {
            padding: 6px;
        }
        
        QTableWidget::item:selected {
            background-color: #00bcd4;
        }
        
        QHeaderView::section {
            background-color: #404040;
            color: #00bcd4;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        QLineEdit, QComboBox {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 4px;
            padding: 6px;
            color: #fff;
        }
        
        QLineEdit:focus, QComboBox:focus {
            border-color: #00bcd4;
        }
    )");
}

void CrowdStrikeEDR::initializeDatabase() {
    edrDb = QSqlDatabase::addDatabase("QSQLITE", "crowdstrike_edr");
    edrDb.setDatabaseName("crowdstrike_edr.db");
    
    if (!edrDb.open()) {
        QMessageBox::critical(this, "Database Error", "Could not open CrowdStrike EDR database");
        return;
    }
    
    QSqlQuery query(edrDb);
    
    // Create endpoints table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS endpoints (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            hostname TEXT UNIQUE NOT NULL,
            ip_address TEXT NOT NULL,
            operating_system TEXT NOT NULL,
            status TEXT DEFAULT 'Online',
            last_seen DATETIME DEFAULT CURRENT_TIMESTAMP,
            agent_version TEXT DEFAULT '7.15.0',
            policy_name TEXT DEFAULT 'Standard Protection',
            threat_count INTEGER DEFAULT 0,
            quarantined BOOLEAN DEFAULT FALSE
        )
    )");
    
    // Create threats table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS threats (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            endpoint_hostname TEXT NOT NULL,
            threat_type TEXT NOT NULL,
            threat_name TEXT NOT NULL,
            severity TEXT NOT NULL,
            status TEXT DEFAULT 'Active',
            detected_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            file_path TEXT,
            process_name TEXT,
            command_line TEXT,
            hash_sha256 TEXT,
            remediation_action TEXT
        )
    )");
    
    // Create blocked hashes table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS blocked_hashes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            hash_sha256 TEXT UNIQUE NOT NULL,
            hash_type TEXT DEFAULT 'SHA256',
            threat_name TEXT,
            blocked_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            block_reason TEXT,
            global_block BOOLEAN DEFAULT TRUE
        )
    )");
    
    // Create policies table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS edr_policies (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            policy_name TEXT UNIQUE NOT NULL,
            prevention_enabled BOOLEAN DEFAULT TRUE,
            detection_enabled BOOLEAN DEFAULT TRUE,
            machine_learning BOOLEAN DEFAULT TRUE,
            behavioral_analysis BOOLEAN DEFAULT TRUE,
            script_protection BOOLEAN DEFAULT TRUE,
            memory_protection BOOLEAN DEFAULT TRUE,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");
}

void CrowdStrikeEDR::loadEndpointData() {
    // Check if endpoints already exist
    QSqlQuery checkQuery("SELECT COUNT(*) FROM endpoints", edrDb);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        refreshEndpoints();
        return;
    }
    
    // Sample endpoint data
    QStringList sampleEndpoints = {
        "DESKTOP-001|192.168.1.10|Windows 11 Pro|Online",
        "LAPTOP-042|192.168.1.25|Windows 10 Enterprise|Online", 
        "SERVER-DC01|192.168.1.5|Windows Server 2022|Online",
        "WORKSTATION-15|192.168.1.30|Windows 11 Pro|Offline",
        "MACBOOK-PRO|192.168.1.45|macOS 13.0|Online",
        "UBUNTU-DEV|192.168.1.50|Ubuntu 22.04 LTS|Online",
        "LAPTOP-HR01|192.168.1.35|Windows 10 Pro|Online",
        "SERVER-WEB01|192.168.1.8|Ubuntu Server 20.04|Online"
    };
    
    QSqlQuery insertQuery(edrDb);
    insertQuery.prepare("INSERT INTO endpoints (hostname, ip_address, operating_system, status) VALUES (?, ?, ?, ?)");
    
    for (const QString &endpoint : sampleEndpoints) {
        QStringList parts = endpoint.split('|');
        if (parts.size() >= 4) {
            insertQuery.addBindValue(parts[0]);
            insertQuery.addBindValue(parts[1]);
            insertQuery.addBindValue(parts[2]);
            insertQuery.addBindValue(parts[3]);
            insertQuery.exec();
        }
    }
    
    // Add sample threats
    QStringList sampleThreats = {
        "DESKTOP-001|Malware|Trojan.Win32.Agent|Critical|C:\\Users\\user\\Downloads\\malware.exe|malware.exe",
        "LAPTOP-042|Suspicious Activity|PowerShell Injection|High|powershell.exe|-EncodedCommand base64data",
        "SERVER-WEB01|Vulnerability|CVE-2023-1234|Medium|/var/www/html/vulnerable.php|apache2"
    };
    
    QSqlQuery threatQuery(edrDb);
    threatQuery.prepare("INSERT INTO threats (endpoint_hostname, threat_type, threat_name, severity, file_path, process_name) VALUES (?, ?, ?, ?, ?, ?)");
    
    for (const QString &threat : sampleThreats) {
        QStringList parts = threat.split('|');
        if (parts.size() >= 6) {
            threatQuery.addBindValue(parts[0]);
            threatQuery.addBindValue(parts[1]);
            threatQuery.addBindValue(parts[2]);
            threatQuery.addBindValue(parts[3]);
            threatQuery.addBindValue(parts[4]);
            threatQuery.addBindValue(parts[5]);
            threatQuery.exec();
        }
    }
    
    activityLog->append("<font color='#4caf50'>[EDR] Sample endpoint data loaded</font>");
    activityLog->append("<font color='#2196f3'>[EDR] 8 endpoints registered</font>");
    activityLog->append("<font color='#f44336'>[EDR] 3 active threats detected</font>");
    
    refreshEndpoints();
}

void CrowdStrikeEDR::refreshEndpoints() {
    // Clear tables
    endpointsTable->setRowCount(0);
    threatsTable->setRowCount(0);
    
    // Load endpoints
    QSqlQuery endpointQuery("SELECT hostname, ip_address, operating_system, status, last_seen, threat_count FROM endpoints ORDER BY hostname", edrDb);
    
    totalEndpoints = 0;
    quarantinedEndpoints = 0;
    
    while (endpointQuery.next()) {
        QString hostname = endpointQuery.value(0).toString();
        QString ip = endpointQuery.value(1).toString();
        QString os = endpointQuery.value(2).toString();
        QString status = endpointQuery.value(3).toString();
        QString lastSeen = endpointQuery.value(4).toString();
        int threats = endpointQuery.value(5).toInt();
        
        addEndpointToTable(hostname, ip, os, status, lastSeen, QString::number(threats));
        
        totalEndpoints++;
        if (status == "Quarantined") {
            quarantinedEndpoints++;
        }
    }
    
    // Load threats
    QSqlQuery threatQuery("SELECT endpoint_hostname, threat_type, threat_name, severity, detected_at FROM threats WHERE status = 'Active' ORDER BY detected_at DESC", edrDb);
    
    totalThreats = 0;
    while (threatQuery.next()) {
        QString endpoint = threatQuery.value(0).toString();
        QString type = threatQuery.value(1).toString();
        QString name = threatQuery.value(2).toString();
        QString severity = threatQuery.value(3).toString();
        QString detected = threatQuery.value(4).toString();
        
        addThreatDetection(endpoint, QString("%1: %2").arg(type).arg(name), severity);
        totalThreats++;
    }
    
    // Update statistics
    endpointCount->setText(QString::number(totalEndpoints));
    threatCount->setText(QString::number(totalThreats));
    quarantineCount->setText(QString::number(quarantinedEndpoints));
}

void CrowdStrikeEDR::addEndpointToTable(const QString &hostname, const QString &ip, const QString &os, 
                                       const QString &status, const QString &lastSeen, const QString &threats) {
    int row = endpointsTable->rowCount();
    endpointsTable->insertRow(row);
    
    endpointsTable->setItem(row, 0, new QTableWidgetItem(hostname));
    endpointsTable->setItem(row, 1, new QTableWidgetItem(ip));
    endpointsTable->setItem(row, 2, new QTableWidgetItem(os));
    
    // Color-code status
    auto *statusItem = new QTableWidgetItem(status);
    if (status == "Online") {
        statusItem->setForeground(QColor("#4caf50"));
    } else if (status == "Quarantined") {
        statusItem->setForeground(QColor("#f44336"));
    } else {
        statusItem->setForeground(QColor("#ff9800"));
    }
    endpointsTable->setItem(row, 3, statusItem);
    
    endpointsTable->setItem(row, 4, new QTableWidgetItem(lastSeen));
    
    // Color-code threat count
    auto *threatItem = new QTableWidgetItem(threats);
    int threatCount = threats.toInt();
    if (threatCount > 0) {
        threatItem->setForeground(QColor("#f44336"));
    } else {
        threatItem->setForeground(QColor("#4caf50"));
    }
    endpointsTable->setItem(row, 5, threatItem);
}

void CrowdStrikeEDR::addThreatDetection(const QString &endpoint, const QString &threat, const QString &severity) {
    int row = threatsTable->rowCount();
    threatsTable->insertRow(row);
    
    threatsTable->setItem(row, 0, new QTableWidgetItem(endpoint));
    threatsTable->setItem(row, 1, new QTableWidgetItem(threat));
    
    // Color-code severity
    auto *severityItem = new QTableWidgetItem(severity);
    if (severity == "Critical") {
        severityItem->setForeground(QColor("#f44336"));
    } else if (severity == "High") {
        severityItem->setForeground(QColor("#ff5722"));
    } else if (severity == "Medium") {
        severityItem->setForeground(QColor("#ff9800"));
    } else {
        severityItem->setForeground(QColor("#4caf50"));
    }
    threatsTable->setItem(row, 2, severityItem);
    
    threatsTable->setItem(row, 3, new QTableWidgetItem("Active"));
    threatsTable->setItem(row, 4, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm")));
}

void CrowdStrikeEDR::scanEndpoints() {
    activityLog->append("<font color='#2196f3'>[EDR] Starting comprehensive endpoint scan...</font>");
    
    scanProgress->setVisible(true);
    scanProgress->setRange(0, totalEndpoints);
    scanProgress->setValue(0);
    
    // Simulate scanning each endpoint
    for (int i = 0; i < totalEndpoints; ++i) {
        QApplication::processEvents();
        scanProgress->setValue(i + 1);
        
        // Simulate finding threats
        if (QRandomGenerator::global()->bounded(100) < 20) { // 20% chance of finding threat
            QString hostname = QString("ENDPOINT-%1").arg(i + 1, 3, 10, QChar('0'));
            QString threatType = QStringList({"Malware", "Suspicious Activity", "Vulnerability", "Behavioral Anomaly"})[QRandomGenerator::global()->bounded(4)];
            QString severity = QStringList({"Critical", "High", "Medium", "Low"})[QRandomGenerator::global()->bounded(4)];
            
            activityLog->append(QString("<font color='#f44336'>[THREAT] %1 detected on %2</font>").arg(threatType).arg(hostname));
        }
        
        QThread::msleep(100); // Simulate scan time
    }
    
    scanProgress->setVisible(false);
    activityLog->append("<font color='#4caf50'>[EDR] Endpoint scan completed</font>");
    
    // Log scan completion
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('CrowdStrikeEDR', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("EDR endpoint scan completed - %1 endpoints scanned").arg(totalEndpoints));
    logQuery.exec();
    
    refreshEndpoints();
}

void CrowdStrikeEDR::quarantineEndpoint() {
    int currentRow = endpointsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select an endpoint to quarantine.");
        return;
    }
    
    QString hostname = endpointsTable->item(currentRow, 0)->text();
    
    QSqlQuery updateQuery(edrDb);
    updateQuery.prepare("UPDATE endpoints SET status = 'Quarantined', quarantined = TRUE WHERE hostname = ?");
    updateQuery.addBindValue(hostname);
    updateQuery.exec();
    
    activityLog->append(QString("<font color='#f44336'>[QUARANTINE] Endpoint %1 has been quarantined</font>").arg(hostname));
    activityLog->append("<font color='#ff9800'>[QUARANTINE] Network access restricted, investigating threats</font>");
    
    // Log quarantine action
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('CrowdStrikeEDR', 'Warning', :msg)");
    logQuery.bindValue(":msg", QString("Endpoint quarantined: %1").arg(hostname));
    logQuery.exec();
    
    refreshEndpoints();
}

void CrowdStrikeEDR::releaseQuarantine() {
    int currentRow = endpointsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select an endpoint to release from quarantine.");
        return;
    }
    
    QString hostname = endpointsTable->item(currentRow, 0)->text();
    
    QSqlQuery updateQuery(edrDb);
    updateQuery.prepare("UPDATE endpoints SET status = 'Online', quarantined = FALSE WHERE hostname = ?");
    updateQuery.addBindValue(hostname);
    updateQuery.exec();
    
    activityLog->append(QString("<font color='#4caf50'>[RELEASE] Endpoint %1 released from quarantine</font>").arg(hostname));
    activityLog->append("<font color='#4caf50'>[RELEASE] Network access restored</font>");
    
    // Log release action
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('CrowdStrikeEDR', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("Endpoint released from quarantine: %1").arg(hostname));
    logQuery.exec();
    
    refreshEndpoints();
}

void CrowdStrikeEDR::deployPolicy() {
    QString policy = policyCombo->currentText();
    
    activityLog->append(QString("<font color='#2196f3'>[POLICY] Deploying policy: %1</font>").arg(policy));
    activityLog->append("<font color='#ff9800'>[POLICY] Updating agent configurations...</font>");
    
    // Simulate policy deployment
    QSqlQuery updateQuery(edrDb);
    updateQuery.prepare("UPDATE endpoints SET policy_name = ? WHERE status != 'Offline'");
    updateQuery.addBindValue(policy);
    updateQuery.exec();
    
    activityLog->append(QString("<font color='#4caf50'>[POLICY] Policy %1 deployed to %2 endpoints</font>").arg(policy).arg(totalEndpoints));
    
    // Log policy deployment
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('CrowdStrikeEDR', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("EDR policy deployed: %1").arg(policy));
    logQuery.exec();
}

void CrowdStrikeEDR::huntThreats() {
    QString query = huntQuery->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::warning(this, "Hunt Query Required", "Please enter a threat hunting query.");
        return;
    }
    
    activityLog->append(QString("<font color='#9c27b0'>[HUNT] Executing threat hunt: %1</font>").arg(query));
    activityLog->append("<font color='#2196f3'>[HUNT] Searching across all endpoints...</font>");
    
    QString huntResult = simulateThreatHunt();
    activityLog->append(QString("<font color='%1'>[HUNT] %2</font>")
                      .arg(huntResult.contains("found") ? "#f44336" : "#4caf50")
                      .arg(huntResult));
    
    // Log threat hunt
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('CrowdStrikeEDR', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("Threat hunt executed: %1").arg(query));
    logQuery.exec();
}

void CrowdStrikeEDR::blockHash() {
    QString hash = hashInput->text().trimmed();
    if (hash.isEmpty()) {
        QMessageBox::warning(this, "Hash Required", "Please enter a file hash to block.");
        return;
    }
    
    if (hash.length() != 64) {
        QMessageBox::warning(this, "Invalid Hash", "Please enter a valid SHA256 hash (64 characters).");
        return;
    }
    
    QSqlQuery blockQuery(edrDb);
    blockQuery.prepare("INSERT OR REPLACE INTO blocked_hashes (hash_sha256, threat_name, block_reason) VALUES (?, ?, ?)");
    blockQuery.addBindValue(hash);
    blockQuery.addBindValue("User Blocked Hash");
    blockQuery.addBindValue("Manually blocked by security analyst");
    blockQuery.exec();
    
    activityLog->append(QString("<font color='#f44336'>[BLOCK] Hash blocked globally: %1</font>").arg(hash.left(16) + "..."));
    activityLog->append("<font color='#ff9800'>[BLOCK] All endpoints will prevent execution of this file</font>");
    
    hashInput->clear();
    
    // Log hash blocking
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('CrowdStrikeEDR', 'Warning', :msg)");
    logQuery.bindValue(":msg", QString("File hash blocked: %1").arg(hash.left(16) + "..."));
    logQuery.exec();
}

void CrowdStrikeEDR::viewEndpointDetails() {
    int currentRow = endpointsTable->currentRow();
    if (currentRow < 0) return;
    
    QString hostname = endpointsTable->item(currentRow, 0)->text();
    QString ip = endpointsTable->item(currentRow, 1)->text();
    QString os = endpointsTable->item(currentRow, 2)->text();
    
    processAnalysis->clear();
    processAnalysis->append(QString("<font color='#00bcd4'>[ENDPOINT] %1 (%2)</font>").arg(hostname).arg(ip));
    processAnalysis->append(QString("<font color='#ffffff'>Operating System: %1</font>").arg(os));
    processAnalysis->append(generateProcessAnalysis());
    
    // Query threats for this endpoint
    QSqlQuery threatQuery(edrDb);
    threatQuery.prepare("SELECT threat_type, threat_name, severity, file_path FROM threats WHERE endpoint_hostname = ? AND status = 'Active'");
    threatQuery.addBindValue(hostname);
    
    if (threatQuery.exec()) {
        processAnalysis->append("<font color='#f44336'>[THREATS] Active threats on this endpoint:</font>");
        while (threatQuery.next()) {
            QString type = threatQuery.value(0).toString();
            QString name = threatQuery.value(1).toString();
            QString severity = threatQuery.value(2).toString();
            QString path = threatQuery.value(3).toString();
            
            processAnalysis->append(QString("<font color='#ff5722'>  • %1: %2 (%3) - %4</font>")
                                  .arg(type).arg(name).arg(severity).arg(path));
        }
    }
}

QString CrowdStrikeEDR::generateProcessAnalysis() {
    QStringList processes = {
        "explorer.exe (PID: 1234) - Normal behavior",
        "chrome.exe (PID: 5678) - Web browsing activity",
        "powershell.exe (PID: 9012) - Administrative script execution",
        "svchost.exe (PID: 3456) - Windows service host",
        "winlogon.exe (PID: 7890) - Windows logon process"
    };
    
    QString analysis = "\n<font color='#ff9800'>[PROCESSES] Running processes analysis:</font>\n";
    for (const QString &process : processes) {
        analysis += QString("<font color='#ffffff'>  • %1</font>\n").arg(process);
    }
    
    analysis += "\n<font color='#2196f3'>[BEHAVIOR] Behavioral analysis:</font>\n";
    analysis += "<font color='#ffffff'>  • No suspicious process injection detected</font>\n";
    analysis += "<font color='#ffffff'>  • Memory protection active</font>\n";
    analysis += "<font color='#ffffff'>  • Script execution monitoring enabled</font>\n";
    
    return analysis;
}

QString CrowdStrikeEDR::simulateThreatHunt() {
    QStringList huntResults = {
        "Hunt completed - 0 matches found across all endpoints",
        "Hunt found 2 suspicious PowerShell executions on LAPTOP-042",
        "Hunt detected 1 potential lateral movement attempt",
        "Hunt identified 3 processes with suspicious network connections",
        "Hunt found 0 indicators of compromise"
    };
    
    return huntResults[QRandomGenerator::global()->bounded(huntResults.size())];
}

void CrowdStrikeEDR::analyzeProcess() {
    processAnalysis->append(generateProcessAnalysis());
}

void CrowdStrikeEDR::exportThreatData() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Threat Data", 
        QString("CrowdStrike_Threats_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Endpoint,Threat_Type,Threat_Name,Severity,Detected_At\n";
        
        for (int row = 0; row < threatsTable->rowCount(); ++row) {
            QStringList rowData;
            for (int col = 0; col < threatsTable->columnCount(); ++col) {
                QTableWidgetItem *item = threatsTable->item(row, col);
                rowData << (item ? item->text() : "");
            }
            out << rowData.join(",") << "\n";
        }
        
        file.close();
        QMessageBox::information(this, "Export Complete", 
            QString("Threat data exported to:\n%1").arg(fileName));
    }
}