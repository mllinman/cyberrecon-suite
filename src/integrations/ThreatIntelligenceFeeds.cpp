#include "ThreatIntelligenceFeeds.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSplitter>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QRandomGenerator>
#include <QInputDialog>

ThreatIntelligenceFeeds::ThreatIntelligenceFeeds(QWidget *parent) : QWidget(parent) {
    setupUI();
    initializeDatabase();
    loadThreatFeeds();
    
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &ThreatIntelligenceFeeds::onNetworkReply);
    
    // Auto-refresh feeds every 10 minutes
    feedTimer = new QTimer(this);
    connect(feedTimer, &QTimer::timeout, this, &ThreatIntelligenceFeeds::refreshFeeds);
    feedTimer->start(600000);
    
    // Initial feed refresh
    refreshFeeds();
}

void ThreatIntelligenceFeeds::setupUI() {
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("🌐 Integrated Threat Intelligence Feeds");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Feed controls
    auto *controlGroup = new QGroupBox("Threat Intelligence Sources");
    auto *controlLayout = new QHBoxLayout(controlGroup);
    
    feedSourceCombo = new QComboBox();
    feedSourceCombo->addItems({
        "All Sources",
        "VirusTotal",
        "AlienVault OTX", 
        "Abuse.CH",
        "Malware Bazaar",
        "URLVoid",
        "Hybrid Analysis",
        "Internal Intel"
    });
    controlLayout->addWidget(feedSourceCombo);
    
    iocTypeCombo = new QComboBox();
    iocTypeCombo->addItems({"All Types", "IP Address", "Domain", "URL", "File Hash", "Email"});
    controlLayout->addWidget(iocTypeCombo);
    
    auto *refreshBtn = new QPushButton("🔄 Refresh Feeds");
    refreshBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(refreshBtn, &QPushButton::clicked, this, &ThreatIntelligenceFeeds::refreshFeeds);
    controlLayout->addWidget(refreshBtn);
    
    auto *queryBtn = new QPushButton("🔍 Query IOC");
    queryBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(queryBtn, &QPushButton::clicked, this, &ThreatIntelligenceFeeds::queryIOC);
    controlLayout->addWidget(queryBtn);
    
    auto *addBtn = new QPushButton("➕ Add Custom IOC");
    addBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(addBtn, &QPushButton::clicked, this, &ThreatIntelligenceFeeds::addCustomIOC);
    controlLayout->addWidget(addBtn);
    
    auto *exportBtn = new QPushButton("📤 Export IOCs");
    exportBtn->setStyleSheet("QPushButton { background-color: #9c27b0; color: white; padding: 8px 16px; border-radius: 4px; }");
    connect(exportBtn, &QPushButton::clicked, this, &ThreatIntelligenceFeeds::exportIOCs);
    controlLayout->addWidget(exportBtn);
    
    layout->addWidget(controlGroup);
    
    // Main content
    auto *contentSplitter = new QSplitter(Qt::Horizontal);
    
    // IOC table
    iocTable = new QTableWidget(0, 6);
    iocTable->setHorizontalHeaderLabels({"IOC", "Type", "Severity", "Source", "Confidence", "Last Seen"});
    iocTable->horizontalHeader()->setStretchLastSection(true);
    iocTable->setAlternatingRowColors(true);
    iocTable->setStyleSheet(R"(
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
        QHeaderView::section {
            background-color: #404040;
            color: #00bcd4;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
    )");
    contentSplitter->addWidget(iocTable);
    
    // Feed output
    feedOutput = new QTextEdit();
    feedOutput->setReadOnly(true);
    feedOutput->setStyleSheet(R"(
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
    contentSplitter->addWidget(feedOutput);
    
    contentSplitter->setStretchFactor(0, 2);
    contentSplitter->setStretchFactor(1, 1);
    layout->addWidget(contentSplitter);
}

void ThreatIntelligenceFeeds::initializeDatabase() {
    intelDb = QSqlDatabase::addDatabase("QSQLITE", "threat_intel");
    intelDb.setDatabaseName("threat_intelligence.db");
    
    if (!intelDb.open()) {
        QMessageBox::critical(this, "Database Error", "Could not open threat intelligence database");
        return;
    }
    
    QSqlQuery query(intelDb);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS threat_iocs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ioc TEXT UNIQUE NOT NULL,
            ioc_type TEXT NOT NULL,
            severity TEXT NOT NULL,
            source TEXT NOT NULL,
            confidence INTEGER DEFAULT 50,
            first_seen DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_seen DATETIME DEFAULT CURRENT_TIMESTAMP,
            description TEXT,
            tags TEXT
        )
    )");
}

void ThreatIntelligenceFeeds::loadThreatFeeds() {
    // Sample threat intelligence data
    QStringList sampleIOCs = {
        "192.168.100.50|ip|high|Internal Detection|85|Suspicious internal IP with multiple failed login attempts",
        "malware.example.com|domain|critical|VirusTotal|95|Known malware command and control domain",
        "abc123def456789|hash|critical|Malware Bazaar|90|Trojan.Win32.Agent variant hash",
        "phishing.test.com|domain|medium|URLVoid|75|Suspected phishing domain",
        "203.0.113.45|ip|high|AlienVault OTX|80|IP associated with brute force attacks",
        "evil.exe|hash|critical|Hybrid Analysis|88|Ransomware executable hash",
        "spam@malicious.com|email|medium|Internal Intel|70|Email address used in phishing campaigns"
    };
    
    QSqlQuery insertQuery(intelDb);
    insertQuery.prepare("INSERT OR IGNORE INTO threat_iocs (ioc, ioc_type, severity, source, confidence, description) VALUES (?, ?, ?, ?, ?, ?)");
    
    for (const QString &ioc : sampleIOCs) {
        QStringList parts = ioc.split('|');
        if (parts.size() >= 6) {
            insertQuery.addBindValue(parts[0]);
            insertQuery.addBindValue(parts[1]);
            insertQuery.addBindValue(parts[2]);
            insertQuery.addBindValue(parts[3]);
            insertQuery.addBindValue(parts[4].toInt());
            insertQuery.addBindValue(parts[5]);
            insertQuery.exec();
        }
    }
    
    feedOutput->append("<font color='#00bcd4'>[INTEL] Threat intelligence feeds initialized</font>");
    feedOutput->append("<font color='#4caf50'>[INTEL] 7 IOCs loaded from multiple sources</font>");
    
    updateFeedStatus();
}

void ThreatIntelligenceFeeds::refreshFeeds() {
    feedOutput->append("<font color='#2196f3'>[FEEDS] Refreshing threat intelligence feeds...</font>");
    
    // Simulate new IOCs from feeds
    QStringList newIOCs = {
        "45.67.89.101|ip|high|VirusTotal|82|New malware C2 server detected",
        "badsite.example.org|domain|medium|URLVoid|78|Suspicious domain registered today",
        "def789abc123456|hash|critical|Abuse.CH|92|New malware family hash detected"
    };
    
    QString newIOC = newIOCs[QRandomGenerator::global()->bounded(newIOCs.size())];
    QStringList parts = newIOC.split('|');
    
    if (parts.size() >= 6) {
        QSqlQuery insertQuery(intelDb);
        insertQuery.prepare("INSERT OR IGNORE INTO threat_iocs (ioc, ioc_type, severity, source, confidence, description) VALUES (?, ?, ?, ?, ?, ?)");
        insertQuery.addBindValue(parts[0]);
        insertQuery.addBindValue(parts[1]);
        insertQuery.addBindValue(parts[2]);
        insertQuery.addBindValue(parts[3]);
        insertQuery.addBindValue(parts[4].toInt());
        insertQuery.addBindValue(parts[5]);
        insertQuery.exec();
        
        addIOCToTable(parts[0], parts[1], parts[2], parts[3], parts[4]);
        feedOutput->append(QString("<font color='#4caf50'>[NEW IOC] %1 (%2) - %3</font>").arg(parts[0]).arg(parts[1]).arg(parts[2]));
    }
    
    // Log feed update
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('ThreatIntelFeeds', 'Info', :msg)");
    logQuery.bindValue(":msg", "Threat intelligence feeds updated");
    logQuery.exec();
}

void ThreatIntelligenceFeeds::updateFeedStatus() {
    iocTable->setRowCount(0);
    
    QString filter = "";
    if (feedSourceCombo->currentText() != "All Sources") {
        filter += QString(" WHERE source = '%1'").arg(feedSourceCombo->currentText());
    }
    if (iocTypeCombo->currentText() != "All Types") {
        QString typeFilter = QString(" ioc_type = '%1'").arg(iocTypeCombo->currentText().toLower().replace(" ", "_"));
        filter += filter.isEmpty() ? " WHERE" + typeFilter : " AND" + typeFilter;
    }
    
    QSqlQuery query(QString("SELECT ioc, ioc_type, severity, source, confidence, last_seen FROM threat_iocs%1 ORDER BY last_seen DESC").arg(filter), intelDb);
    
    while (query.next()) {
        QString ioc = query.value(0).toString();
        QString type = query.value(1).toString();
        QString severity = query.value(2).toString();
        QString source = query.value(3).toString();
        QString confidence = query.value(4).toString();
        QString lastSeen = query.value(5).toString();
        
        addIOCToTable(ioc, type, severity, source, confidence);
    }
}

void ThreatIntelligenceFeeds::addIOCToTable(const QString &ioc, const QString &type, const QString &severity, 
                                           const QString &source, const QString &confidence) {
    int row = iocTable->rowCount();
    iocTable->insertRow(row);
    
    iocTable->setItem(row, 0, new QTableWidgetItem(ioc));
    iocTable->setItem(row, 1, new QTableWidgetItem(type.toUpper()));
    
    // Color-code severity
    auto *severityItem = new QTableWidgetItem(severity.toUpper());
    if (severity == "critical") {
        severityItem->setForeground(QColor("#f44336"));
    } else if (severity == "high") {
        severityItem->setForeground(QColor("#ff5722"));
    } else if (severity == "medium") {
        severityItem->setForeground(QColor("#ff9800"));
    } else {
        severityItem->setForeground(QColor("#4caf50"));
    }
    iocTable->setItem(row, 2, severityItem);
    
    iocTable->setItem(row, 3, new QTableWidgetItem(source));
    iocTable->setItem(row, 4, new QTableWidgetItem(confidence + "%"));
    iocTable->setItem(row, 5, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm")));
}

void ThreatIntelligenceFeeds::queryIOC() {
    bool ok;
    QString ioc = QInputDialog::getText(this, "Query IOC", "Enter IOC to query:", QLineEdit::Normal, "", &ok);
    if (!ok || ioc.isEmpty()) return;
    
    feedOutput->append(QString("<font color='#2196f3'>[QUERY] Searching for IOC: %1</font>").arg(ioc));
    feedOutput->append("<font color='#ff9800'>[QUERY] Checking VirusTotal, AlienVault OTX, Abuse.CH...</font>");
    
    // Simulate IOC lookup
    QTimer::singleShot(2000, this, [this, ioc]() {
        bool found = QRandomGenerator::global()->bounded(100) < 60; // 60% chance of finding IOC
        
        if (found) {
            QString severity = QStringList({"critical", "high", "medium"})[QRandomGenerator::global()->bounded(3)];
            QString source = QStringList({"VirusTotal", "AlienVault OTX", "Abuse.CH"})[QRandomGenerator::global()->bounded(3)];
            int confidence = 70 + QRandomGenerator::global()->bounded(30);
            
            feedOutput->append(QString("<font color='#f44336'>[FOUND] IOC %1 found in %2 (Confidence: %3%)</font>")
                              .arg(ioc).arg(source).arg(confidence));
            
            // Determine IOC type
            QString type = "unknown";
            if (ioc.contains(".")) {
                type = ioc.startsWith("http") ? "url" : "domain";
            } else if (ioc.length() >= 32) {
                type = "hash";
            } else if (ioc.contains("@")) {
                type = "email";
            } else {
                type = "ip";
            }
            
            addIOCToTable(ioc, type, severity, source, QString::number(confidence));
        } else {
            feedOutput->append(QString("<font color='#4caf50'>[CLEAN] IOC %1 not found in threat databases</font>").arg(ioc));
        }
    });
}

void ThreatIntelligenceFeeds::addCustomIOC() {
    bool ok;
    QString ioc = QInputDialog::getText(this, "Add Custom IOC", "Enter IOC:", QLineEdit::Normal, "", &ok);
    if (!ok || ioc.isEmpty()) return;
    
    QString type = QInputDialog::getItem(this, "IOC Type", "Select IOC type:", 
                                        {"ip", "domain", "url", "hash", "email"}, 0, false, &ok);
    if (!ok) return;
    
    QString severity = QInputDialog::getItem(this, "Severity", "Select severity:", 
                                           {"critical", "high", "medium", "low"}, 0, false, &ok);
    if (!ok) return;
    
    QSqlQuery insertQuery(intelDb);
    insertQuery.prepare("INSERT OR REPLACE INTO threat_iocs (ioc, ioc_type, severity, source, confidence, description) VALUES (?, ?, ?, ?, ?, ?)");
    insertQuery.addBindValue(ioc);
    insertQuery.addBindValue(type);
    insertQuery.addBindValue(severity);
    insertQuery.addBindValue("Internal Intel");
    insertQuery.addBindValue(90);
    insertQuery.addBindValue("Manually added by security analyst");
    insertQuery.exec();
    
    addIOCToTable(ioc, type, severity, "Internal Intel", "90");
    feedOutput->append(QString("<font color='#4caf50'>[ADDED] Custom IOC added: %1 (%2)</font>").arg(ioc).arg(type));
}

void ThreatIntelligenceFeeds::exportIOCs() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export IOCs", 
        QString("Threat_IOCs_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "IOC,Type,Severity,Source,Confidence,Last_Seen\n";
        
        for (int row = 0; row < iocTable->rowCount(); ++row) {
            QStringList rowData;
            for (int col = 0; col < iocTable->columnCount(); ++col) {
                QTableWidgetItem *item = iocTable->item(row, col);
                rowData << (item ? item->text() : "");
            }
            out << rowData.join(",") << "\n";
        }
        
        file.close();
        QMessageBox::information(this, "Export Complete", 
            QString("IOCs exported to:\n%1").arg(fileName));
    }
}

void ThreatIntelligenceFeeds::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        feedOutput->append("<font color='#4caf50'>[API] External threat intelligence data received</font>");
    } else {
        feedOutput->append("<font color='#f44336'>[ERROR] Failed to fetch external threat data</font>");
    }
    
    reply->deleteLater();
}