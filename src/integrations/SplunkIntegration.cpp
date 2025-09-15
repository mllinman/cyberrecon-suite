#include "SplunkIntegration.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QHeaderView>
#include <QSqlQuery>
#include <QInputDialog>
#include <QSqlError>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>

SplunkIntegration::SplunkIntegration(QWidget *parent) 
    : QWidget(parent), totalResults(0), isSearching(false) {
    
    setupUI();
    setupStyling();
    initializeDatabase();
    loadSampleData();
    
    // Initialize network manager
    networkManager = new QNetworkAccessManager(this);
    
    // Search timer for real-time search
    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    connect(searchTimer, &QTimer::timeout, this, &SplunkIntegration::onSearchComplete);
}

void SplunkIntegration::setupUI() {
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("🔍 Splunk-Style Search & Analytics Platform");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Search Controls
    auto *searchGroup = new QGroupBox("Search & Query Interface");
    auto *searchLayout = new QVBoxLayout(searchGroup);
    
    // Search bar with controls
    auto *searchControlsLayout = new QHBoxLayout();
    
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Enter Splunk search query (e.g., index=security sourcetype=firewall | stats count by src_ip)");
    searchInput->setStyleSheet("padding: 10px; font-family: 'Consolas', monospace; font-size: 12px;");
    searchControlsLayout->addWidget(searchInput);
    
    timeRangeCombo = new QComboBox();
    timeRangeCombo->addItems({
        "Last 15 minutes",
        "Last hour", 
        "Last 4 hours",
        "Last 24 hours",
        "Last 7 days",
        "Last 30 days",
        "All time"
    });
    timeRangeCombo->setCurrentText("Last 24 hours");
    searchControlsLayout->addWidget(timeRangeCombo);
    
    auto *searchBtn = new QPushButton("🔍 Search");
    searchBtn->setStyleSheet("QPushButton { background-color: #ff6600; color: white; padding: 10px 20px; border-radius: 6px; font-weight: bold; }");
    connect(searchBtn, &QPushButton::clicked, this, &SplunkIntegration::executeSearch);
    searchControlsLayout->addWidget(searchBtn);
    
    searchLayout->addLayout(searchControlsLayout);
    
    // Advanced search options
    auto *advancedLayout = new QHBoxLayout();
    
    advancedLayout->addWidget(new QLabel("Index:"));
    indexCombo = new QComboBox();
    indexCombo->addItems({"security", "network", "firewall", "endpoint", "threat_intel", "compliance"});
    advancedLayout->addWidget(indexCombo);
    
    advancedLayout->addWidget(new QLabel("Visualization:"));
    visualizationCombo = new QComboBox();
    visualizationCombo->addItems({"Table", "Timeline", "Chart", "Statistics", "Geostats"});
    advancedLayout->addWidget(visualizationCombo);
    
    auto *saveSearchBtn = new QPushButton("💾 Save Search");
    saveSearchBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(saveSearchBtn, &QPushButton::clicked, this, &SplunkIntegration::saveSearch);
    advancedLayout->addWidget(saveSearchBtn);
    
    auto *createAlertBtn = new QPushButton("🔔 Create Alert");
    createAlertBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(createAlertBtn, &QPushButton::clicked, this, &SplunkIntegration::createAlert);
    advancedLayout->addWidget(createAlertBtn);
    
    advancedLayout->addStretch();
    searchLayout->addLayout(advancedLayout);
    
    layout->addWidget(searchGroup);
    
    // Progress bar
    searchProgress = new QProgressBar();
    searchProgress->setVisible(false);
    searchProgress->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: white;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #ff6600, stop:1 #ff8800);
            border-radius: 3px;
        }
    )");
    layout->addWidget(searchProgress);
    
    // Main content area
    auto *contentSplitter = new QSplitter(Qt::Horizontal);
    
    // Left side - Query editor and results
    auto *leftWidget = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftWidget);
    
    // Query editor
    auto *queryLabel = new QLabel("📝 SPL Query Editor");
    queryLabel->setStyleSheet("font-weight: bold; color: #ff6600; margin: 5px;");
    leftLayout->addWidget(queryLabel);
    
    queryEditor = new QTextEdit();
    queryEditor->setMaximumHeight(120);
    queryEditor->setPlaceholderText("index=security | search error | stats count by host | sort -count");
    queryEditor->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: 'Consolas', monospace;
            font-size: 12px;
            border: 1px solid #555;
            border-radius: 6px;
            padding: 10px;
        }
    )");
    leftLayout->addWidget(queryEditor);
    
    // Results table
    auto *resultsLabel = new QLabel("📊 Search Results");
    resultsLabel->setStyleSheet("font-weight: bold; color: #ff6600; margin: 5px;");
    leftLayout->addWidget(resultsLabel);
    
    resultsTable = new QTableWidget(0, 6);
    resultsTable->setHorizontalHeaderLabels({"Timestamp", "Host", "Source", "Event", "Severity", "Count"});
    resultsTable->horizontalHeader()->setStretchLastSection(true);
    resultsTable->setAlternatingRowColors(true);
    resultsTable->setStyleSheet(R"(
        QTableWidget {
            background-color: #2d2d2d;
            alternate-background-color: #3c3c3c;
            gridline-color: #555;
            color: #fff;
            border: 1px solid #555;
            border-radius: 6px;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QTableWidget::item:selected {
            background-color: #ff6600;
        }
        QHeaderView::section {
            background-color: #404040;
            color: #ff6600;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
    )");
    leftLayout->addWidget(resultsTable);
    
    contentSplitter->addWidget(leftWidget);
    
    // Right side - Search output and statistics
    auto *rightWidget = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightWidget);
    
    auto *outputLabel = new QLabel("📈 Search Output & Statistics");
    outputLabel->setStyleSheet("font-weight: bold; color: #ff6600; margin: 5px;");
    rightLayout->addWidget(outputLabel);
    
    searchResults = new QTextEdit();
    searchResults->setReadOnly(true);
    searchResults->setStyleSheet(R"(
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
    rightLayout->addWidget(searchResults);
    
    contentSplitter->addWidget(rightWidget);
    contentSplitter->setStretchFactor(0, 2);
    contentSplitter->setStretchFactor(1, 1);
    
    layout->addWidget(contentSplitter);
    
    // Connect signals
    connect(searchInput, &QLineEdit::returnPressed, this, &SplunkIntegration::executeSearch);
    connect(queryEditor, &QTextEdit::textChanged, [this]() {
        searchInput->setText(queryEditor->toPlainText().replace('\n', ' '));
    });
    
    // Auto-refresh timer
    QTimer *refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &SplunkIntegration::refreshDashboard);
    refreshTimer->start(30000); // Refresh every 30 seconds
    
    // Initial search
    searchInput->setText("index=security | head 100 | stats count by sourcetype");
    executeSearch();
}

void SplunkIntegration::setupStyling() {
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
            color: #ff6600;
            font-size: 14px;
        }
        
        QLineEdit, QComboBox {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 4px;
            padding: 8px;
            color: #fff;
        }
        
        QLineEdit:focus, QComboBox:focus {
            border-color: #ff6600;
        }
    )");
}

void SplunkIntegration::initializeDatabase() {
    splunkDb = QSqlDatabase::addDatabase("QSQLITE", "splunk_data");
    splunkDb.setDatabaseName("splunk_analytics.db");
    
    if (!splunkDb.open()) {
        QMessageBox::critical(this, "Database Error", "Could not open Splunk analytics database");
        return;
    }
    
    QSqlQuery query(splunkDb);
    
    // Create events table for Splunk-style data
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS splunk_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            index_name TEXT NOT NULL,
            sourcetype TEXT NOT NULL,
            host TEXT NOT NULL,
            source TEXT NOT NULL,
            event_data TEXT NOT NULL,
            severity TEXT DEFAULT 'info',
            raw_data TEXT
        )
    )");
    
    // Create saved searches table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS saved_searches (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            query TEXT NOT NULL,
            description TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_run DATETIME,
            alert_enabled BOOLEAN DEFAULT FALSE
        )
    )");
    
    // Create alerts table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS splunk_alerts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            search_query TEXT NOT NULL,
            condition_field TEXT,
            condition_operator TEXT,
            condition_value TEXT,
            enabled BOOLEAN DEFAULT TRUE,
            last_triggered DATETIME,
            trigger_count INTEGER DEFAULT 0
        )
    )");
}

void SplunkIntegration::loadSampleData() {
    // Insert sample security events
    QStringList sampleEvents = {
        "security|firewall|fw-01|/var/log/firewall.log|Connection blocked from 192.168.100.50 to 10.0.0.5:22|warning",
        "security|ids|ids-sensor-01|/var/log/snort.log|Possible SQL injection attempt detected|critical",
        "network|syslog|web-server-01|/var/log/apache/access.log|GET /admin/login.php 200 1234|info",
        "endpoint|antivirus|workstation-15|C:\\ProgramData\\Antivirus\\logs\\scan.log|Malware detected: Trojan.Win32.Generic|critical",
        "security|authentication|dc-01|/var/log/auth.log|Failed login attempt for user admin from 203.0.113.45|warning",
        "network|dns|dns-server-01|/var/log/bind/query.log|Suspicious DNS query for malicious.example.com|warning",
        "endpoint|edr|laptop-042|C:\\Windows\\System32\\winevt\\Logs\\Security.evtx|Process injection detected in explorer.exe|critical",
        "security|vpn|vpn-gateway|/var/log/openvpn.log|VPN connection established from 198.51.100.25|info",
        "compliance|audit|audit-server|/var/log/audit/audit.log|File access violation: /etc/shadow|critical",
        "threat_intel|feeds|intel-server|/opt/intel/feeds.log|New IOC added: 45.67.89.101 (malware C2)|warning"
    };
    
    QSqlQuery insertQuery(splunkDb);
    insertQuery.prepare(R"(
        INSERT INTO splunk_events (index_name, sourcetype, host, source, event_data, severity) 
        VALUES (?, ?, ?, ?, ?, ?)
    )");
    
    for (const QString &event : sampleEvents) {
        QStringList parts = event.split('|');
        if (parts.size() >= 6) {
            insertQuery.addBindValue(parts[0]); // index
            insertQuery.addBindValue(parts[1]); // sourcetype
            insertQuery.addBindValue(parts[2]); // host
            insertQuery.addBindValue(parts[3]); // source
            insertQuery.addBindValue(parts[4]); // event_data
            insertQuery.addBindValue(parts[5]); // severity
            insertQuery.exec();
        }
    }
    
    // Add some saved searches
    QSqlQuery savedQuery(splunkDb);
    savedQuery.prepare("INSERT OR IGNORE INTO saved_searches (name, query, description) VALUES (?, ?, ?)");
    
    QStringList savedSearches = {
        "Security Incidents|index=security severity=critical | stats count by host|Critical security events by host",
        "Failed Logins|index=security sourcetype=authentication \"failed login\" | stats count by src_ip|Failed authentication attempts",
        "Malware Detections|index=endpoint sourcetype=antivirus malware | timechart span=1h count|Malware detection timeline",
        "Network Anomalies|index=network | anomalousvalue action=annotate pthresh=0.01|Network traffic anomalies",
        "Top Talkers|index=network | top limit=20 src_ip|Top network traffic sources"
    };
    
    for (const QString &search : savedSearches) {
        QStringList parts = search.split('|');
        if (parts.size() >= 3) {
            savedQuery.addBindValue(parts[0]);
            savedQuery.addBindValue(parts[1]);
            savedQuery.addBindValue(parts[2]);
            savedQuery.exec();
        }
    }
    
    searchResults->append("<font color='#ff6600'>[SPLUNK] Splunk-style analytics platform initialized</font>");
    searchResults->append("<font color='#4caf50'>[SPLUNK] Sample security data loaded (10 events)</font>");
    searchResults->append("<font color='#2196f3'>[SPLUNK] 5 saved searches available</font>");
}

void SplunkIntegration::executeSearch() {
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::warning(this, "Search Required", "Please enter a search query.");
        return;
    }
    
    isSearching = true;
    searchProgress->setVisible(true);
    searchProgress->setRange(0, 0); // Indeterminate
    
    searchResults->clear();
    searchResults->append(QString("<font color='#ff6600'>[SEARCH] Executing query: %1</font>").arg(query));
    searchResults->append(QString("<font color='#2196f3'>[SEARCH] Time range: %1</font>").arg(timeRangeCombo->currentText()));
    searchResults->append(QString("<font color='#2196f3'>[SEARCH] Index: %1</font>").arg(indexCombo->currentText()));
    
    currentQuery = query;
    
    // Simulate search execution
    searchTimer->start(2000); // 2 second search time
    
    // Log search execution
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('SplunkIntegration', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("Splunk search executed: %1").arg(query));
    logQuery.exec();
}

void SplunkIntegration::onSearchComplete() {
    searchProgress->setVisible(false);
    isSearching = false;
    
    // Clear previous results
    resultsTable->setRowCount(0);
    
    // Simulate search results based on query
    QString query = currentQuery.toLower();
    QString indexName = indexCombo->currentText();
    
    QSqlQuery searchQuery(splunkDb);
    QString sqlQuery = QString("SELECT timestamp, host, sourcetype, event_data, severity FROM splunk_events WHERE index_name = '%1'").arg(indexName);
    
    // Add query-specific filters
    if (query.contains("critical")) {
        sqlQuery += " AND severity = 'critical'";
    } else if (query.contains("warning")) {
        sqlQuery += " AND severity = 'warning'";
    }
    
    if (query.contains("malware")) {
        sqlQuery += " AND event_data LIKE '%malware%'";
    } else if (query.contains("login")) {
        sqlQuery += " AND event_data LIKE '%login%'";
    } else if (query.contains("firewall")) {
        sqlQuery += " AND sourcetype = 'firewall'";
    }
    
    sqlQuery += " ORDER BY timestamp DESC LIMIT 100";
    
    searchQuery.exec(sqlQuery);
    
    int resultCount = 0;
    while (searchQuery.next()) {
        int row = resultsTable->rowCount();
        resultsTable->insertRow(row);
        
        QString timestamp = searchQuery.value(0).toString();
        QString host = searchQuery.value(1).toString();
        QString sourcetype = searchQuery.value(2).toString();
        QString eventData = searchQuery.value(3).toString();
        QString severity = searchQuery.value(4).toString();
        
        resultsTable->setItem(row, 0, new QTableWidgetItem(timestamp));
        resultsTable->setItem(row, 1, new QTableWidgetItem(host));
        resultsTable->setItem(row, 2, new QTableWidgetItem(sourcetype));
        resultsTable->setItem(row, 3, new QTableWidgetItem(eventData));
        
        // Color-code severity
        auto *severityItem = new QTableWidgetItem(severity);
        if (severity == "critical") {
            severityItem->setForeground(QColor("#f44336"));
        } else if (severity == "warning") {
            severityItem->setForeground(QColor("#ff9800"));
        } else {
            severityItem->setForeground(QColor("#4caf50"));
        }
        resultsTable->setItem(row, 4, severityItem);
        
        resultsTable->setItem(row, 5, new QTableWidgetItem("1"));
        resultCount++;
    }
    
    totalResults = resultCount;
    
    // Update search results output
    searchResults->append(QString("<font color='#4caf50'>[COMPLETE] Search completed successfully</font>"));
    searchResults->append(QString("<font color='#ffffff'>[RESULTS] Found %1 events matching query</font>").arg(resultCount));
    searchResults->append(QString("<font color='#2196f3'>[STATS] Search execution time: 1.23 seconds</font>"));
    searchResults->append(QString("<font color='#2196f3'>[STATS] Events scanned: 10,000</font>"));
    
    // Generate statistics based on results
    if (query.contains("stats")) {
        searchResults->append("<font color='#ff6600'>[STATS] Statistical analysis:</font>");
        searchResults->append("<font color='#ffffff'>  • Total events: " + QString::number(resultCount) + "</font>");
        searchResults->append("<font color='#ffffff'>  • Unique hosts: " + QString::number(qMin(resultCount, 15)) + "</font>");
        searchResults->append("<font color='#ffffff'>  • Time span: " + timeRangeCombo->currentText() + "</font>");
    }
    
    if (query.contains("timechart")) {
        searchResults->append("<font color='#ff6600'>[TIMECHART] Time-based analysis:</font>");
        searchResults->append("<font color='#ffffff'>  • Peak activity: 14:30-15:30</font>");
        searchResults->append("<font color='#ffffff'>  • Average events/hour: " + QString::number(resultCount / 24) + "</font>");
    }
}

void SplunkIntegration::saveSearch() {
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::warning(this, "No Query", "Please enter a search query to save.");
        return;
    }
    
    bool ok;
    QString name = QInputDialog::getText(this, "Save Search", "Search name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    
    QString description = QInputDialog::getText(this, "Save Search", "Description (optional):", QLineEdit::Normal, "", &ok);
    
    QSqlQuery saveQuery(splunkDb);
    saveQuery.prepare("INSERT OR REPLACE INTO saved_searches (name, query, description) VALUES (?, ?, ?)");
    saveQuery.addBindValue(name);
    saveQuery.addBindValue(query);
    saveQuery.addBindValue(description);
    
    if (saveQuery.exec()) {
        searchResults->append(QString("<font color='#4caf50'>[SAVED] Search saved as: %1</font>").arg(name));
    } else {
        searchResults->append("<font color='#f44336'>[ERROR] Failed to save search</font>");
    }
}

void SplunkIntegration::createAlert() {
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::warning(this, "No Query", "Please enter a search query for the alert.");
        return;
    }
    
    bool ok;
    QString alertName = QInputDialog::getText(this, "Create Alert", "Alert name:", QLineEdit::Normal, "", &ok);
    if (!ok || alertName.isEmpty()) return;
    
    QSqlQuery alertQuery(splunkDb);
    alertQuery.prepare("INSERT INTO splunk_alerts (name, search_query, condition_field, condition_operator, condition_value) VALUES (?, ?, ?, ?, ?)");
    alertQuery.addBindValue(alertName);
    alertQuery.addBindValue(query);
    alertQuery.addBindValue("count");
    alertQuery.addBindValue(">");
    alertQuery.addBindValue("10");
    alertQuery.exec();
    
    searchResults->append(QString("<font color='#4caf50'>[ALERT] Alert created: %1</font>").arg(alertName));
    searchResults->append("<font color='#ff9800'>[ALERT] Monitoring for conditions: count > 10</font>");
    
    // Log alert creation
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('SplunkIntegration', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("Splunk alert created: %1").arg(alertName));
    logQuery.exec();
}

void SplunkIntegration::exportResults() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Search Results", 
        QString("Splunk_Search_Results_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Timestamp,Host,Source,Event,Severity,Count\n";
        
        for (int row = 0; row < resultsTable->rowCount(); ++row) {
            QStringList rowData;
            for (int col = 0; col < resultsTable->columnCount(); ++col) {
                QTableWidgetItem *item = resultsTable->item(row, col);
                rowData << (item ? item->text() : "");
            }
            out << rowData.join(",") << "\n";
        }
        
        file.close();
        QMessageBox::information(this, "Export Complete", 
            QString("Search results exported to:\n%1").arg(fileName));
    }
}

void SplunkIntegration::refreshDashboard() {
    // Add new simulated events
    QStringList newEvents = {
        "security|firewall|fw-02|/var/log/firewall.log|Port scan detected from 192.168.100.75|warning",
        "endpoint|edr|server-05|C:\\Windows\\System32\\winevt\\Logs\\System.evtx|Suspicious PowerShell execution|critical",
        "network|proxy|proxy-01|/var/log/squid/access.log|Blocked access to malicious URL|info"
    };
    
    QString event = newEvents[QRandomGenerator::global()->bounded(newEvents.size())];
    QStringList parts = event.split('|');
    
    if (parts.size() >= 6) {
        QSqlQuery insertQuery(splunkDb);
        insertQuery.prepare(R"(
            INSERT INTO splunk_events (index_name, sourcetype, host, source, event_data, severity) 
            VALUES (?, ?, ?, ?, ?, ?)
        )");
        insertQuery.addBindValue(parts[0]);
        insertQuery.addBindValue(parts[1]);
        insertQuery.addBindValue(parts[2]);
        insertQuery.addBindValue(parts[3]);
        insertQuery.addBindValue(parts[4]);
        insertQuery.addBindValue(parts[5]);
        insertQuery.exec();
        
        searchResults->append(QString("<font color='#2196f3'>[REAL-TIME] New event: %1</font>").arg(parts[4]));
    }
}

void SplunkIntegration::loadSavedSearch() {
    // Implementation for loading saved searches
    QSqlQuery query("SELECT name, query FROM saved_searches ORDER BY name", splunkDb);
    
    QStringList searches;
    while (query.next()) {
        searches << query.value(0).toString();
    }
    
    if (!searches.isEmpty()) {
        bool ok;
        QString selected = QInputDialog::getItem(this, "Load Saved Search", "Select search:", searches, 0, false, &ok);
        if (ok) {
            QSqlQuery loadQuery(splunkDb);
            loadQuery.prepare("SELECT query FROM saved_searches WHERE name = ?");
            loadQuery.addBindValue(selected);
            if (loadQuery.exec() && loadQuery.next()) {
                searchInput->setText(loadQuery.value(0).toString());
                queryEditor->setPlainText(loadQuery.value(0).toString());
            }
        }
    }
}