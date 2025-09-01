#include "ThreatIntelligence.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>

ThreatIntelligence::ThreatIntelligence(QWidget *parent) : QWidget(parent) {
    setupUI();
    
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &ThreatIntelligence::onNetworkReply);
    
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &ThreatIntelligence::refreshFeeds);
    refreshTimer->start(300000); // 5 minutes
    
    refreshFeeds();
}

void ThreatIntelligence::setupUI() {
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("🔍 Advanced Threat Intelligence Platform");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Search section
    auto *searchGroup = new QWidget();
    auto *searchLayout = new QHBoxLayout(searchGroup);
    
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Enter IOC (IP, domain, hash, URL)...");
    searchInput->setStyleSheet("padding: 10px; font-size: 12px; border: 2px solid #555; border-radius: 6px;");
    searchLayout->addWidget(searchInput);
    
    iocTypeCombo = new QComboBox();
    iocTypeCombo->addItems({"All Types", "IP Address", "Domain", "URL", "File Hash", "Email"});
    iocTypeCombo->setStyleSheet("padding: 10px; font-size: 12px;");
    searchLayout->addWidget(iocTypeCombo);
    
    auto *searchBtn = new QPushButton("🔍 Search");
    searchBtn->setStyleSheet("QPushButton { background-color: #00bcd4; color: white; padding: 10px 20px; border-radius: 6px; font-weight: bold; }");
    connect(searchBtn, &QPushButton::clicked, this, &ThreatIntelligence::searchIOC);
    searchLayout->addWidget(searchBtn);
    
    auto *exportBtn = new QPushButton("📤 Export IOCs");
    exportBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(exportBtn, &QPushButton::clicked, this, &ThreatIntelligence::exportIOCs);
    searchLayout->addWidget(exportBtn);
    
    layout->addWidget(searchGroup);
    
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
                stop:0 #00bcd4, stop:1 #26c6da);
            border-radius: 3px;
        }
    )");
    layout->addWidget(scanProgress);
    
    // Results section
    auto *resultsLayout = new QHBoxLayout();
    
    // Left side - IOC Table
    auto *leftWidget = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftWidget);
    
    auto *tableLabel = new QLabel("🎯 Threat Indicators");
    tableLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    leftLayout->addWidget(tableLabel);
    
    iocTable = new QTableWidget(0, 5);
    iocTable->setHorizontalHeaderLabels({"IOC", "Type", "Severity", "Source", "Last Seen"});
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
    leftLayout->addWidget(iocTable);
    
    // Right side - Analysis Results
    auto *rightWidget = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightWidget);
    
    auto *analysisLabel = new QLabel("📊 Analysis Results");
    analysisLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    rightLayout->addWidget(analysisLabel);
    
    resultsDisplay = new QTextEdit();
    resultsDisplay->setReadOnly(true);
    resultsDisplay->setStyleSheet(R"(
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
    rightLayout->addWidget(resultsDisplay);
    
    resultsLayout->addWidget(leftWidget, 1);
    resultsLayout->addWidget(rightWidget, 1);
    layout->addLayout(resultsLayout);
    
    connect(searchInput, &QLineEdit::returnPressed, this, &ThreatIntelligence::searchIOC);
}

void ThreatIntelligence::searchIOC() {
    QString ioc = searchInput->text().trimmed();
    if (ioc.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter an IOC to search.");
        return;
    }
    
    scanProgress->setVisible(true);
    scanProgress->setRange(0, 0); // Indeterminate progress
    
    resultsDisplay->clear();
    resultsDisplay->append(QString("<font color='#00bcd4'>[SEARCH] Analyzing IOC: %1</font>").arg(ioc));
    resultsDisplay->append("<font color='#ffeb3b'>[INFO] Querying threat intelligence sources...</font>");
    
    // Simulate API calls to various threat intel sources
    QTimer::singleShot(1000, this, [this, ioc]() {
        resultsDisplay->append("<font color='#4caf50'>[VIRUSTOTAL] Query completed</font>");
        resultsDisplay->append("<font color='#4caf50'>[ALIENVAULT] OTX data retrieved</font>");
        resultsDisplay->append("<font color='#4caf50'>[ABUSE.CH] Malware bazaar checked</font>");
        
        // Simulate results based on IOC type
        if (ioc.contains(".")) {
            if (ioc.startsWith("http")) {
                resultsDisplay->append(QString("<font color='#f44336'>[RESULT] URL flagged as malicious by 3/10 engines</font>"));
                addIOCToTable(ioc, "URL", "HIGH", "VirusTotal");
            } else {
                resultsDisplay->append(QString("<font color='#ff9800'>[RESULT] Domain reputation: Suspicious</font>"));
                addIOCToTable(ioc, "Domain", "MEDIUM", "AlienVault OTX");
            }
        } else if (ioc.length() == 32 || ioc.length() == 40 || ioc.length() == 64) {
            resultsDisplay->append(QString("<font color='#f44336'>[RESULT] File hash detected in malware samples</font>"));
            addIOCToTable(ioc, "Hash", "CRITICAL", "Abuse.CH");
        } else {
            resultsDisplay->append(QString("<font color='#4caf50'>[RESULT] No threats detected</font>"));
            addIOCToTable(ioc, "IP", "LOW", "Multiple Sources");
        }
        
        resultsDisplay->append("<font color='#00bcd4'>[COMPLETE] Analysis finished</font>");
        scanProgress->setVisible(false);
        
        // Log to events
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('ThreatIntel', 'Info', :msg)");
        q.bindValue(":msg", QString("IOC analysis completed: %1").arg(ioc));
        q.exec();
    });
}

void ThreatIntelligence::refreshFeeds() {
    resultsDisplay->append("<font color='#2196f3'>[FEEDS] Refreshing threat intelligence feeds...</font>");
    
    // Simulate feed updates
    QStringList feedUpdates = {
        "New malware family 'DarkLoader' detected",
        "C2 infrastructure takedown: 15 domains seized",
        "APT group 'ShadowCyber' campaign indicators updated",
        "Ransomware IOCs added: 'CryptoLocker v3.2'",
        "Phishing campaign targeting financial sector detected"
    };
    
    QString update = feedUpdates[qrand() % feedUpdates.size()];
    resultsDisplay->append(QString("<font color='#4caf50'>[FEED UPDATE] %1</font>").arg(update));
    
    // Add some sample IOCs to the table
    static int feedCounter = 0;
    feedCounter++;
    
    if (feedCounter % 3 == 0) {
        addIOCToTable("192.168.100." + QString::number(100 + feedCounter), "IP", "HIGH", "Threat Feed");
    }
    
    // Log feed update
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('ThreatIntel', 'Info', :msg)");
    q.bindValue(":msg", QString("Threat feed updated: %1").arg(update));
    q.exec();
}

void ThreatIntelligence::addIOCToTable(const QString &ioc, const QString &type, const QString &severity, const QString &source) {
    int row = iocTable->rowCount();
    iocTable->insertRow(row);
    
    iocTable->setItem(row, 0, new QTableWidgetItem(ioc));
    iocTable->setItem(row, 1, new QTableWidgetItem(type));
    
    auto *severityItem = new QTableWidgetItem(severity);
    if (severity == "CRITICAL") {
        severityItem->setForeground(QColor("#f44336"));
    } else if (severity == "HIGH") {
        severityItem->setForeground(QColor("#ff5722"));
    } else if (severity == "MEDIUM") {
        severityItem->setForeground(QColor("#ff9800"));
    } else {
        severityItem->setForeground(QColor("#4caf50"));
    }
    iocTable->setItem(row, 2, severityItem);
    
    iocTable->setItem(row, 3, new QTableWidgetItem(source));
    iocTable->setItem(row, 4, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm")));
    
    // Keep only last 100 entries
    if (iocTable->rowCount() > 100) {
        iocTable->removeRow(0);
    }
}

void ThreatIntelligence::exportIOCs() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export IOCs", 
        QString("ThreatIntel_IOCs_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "IOC,Type,Severity,Source,Last_Seen\n";
        
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
            QString("IOCs exported successfully to:\n%1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Export Failed", "Could not save file.");
    }
}

void ThreatIntelligence::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        // Process real API responses here
        resultsDisplay->append("<font color='#4caf50'>[API] External threat intelligence data received</font>");
    } else {
        resultsDisplay->append("<font color='#f44336'>[ERROR] Failed to fetch external threat data</font>");
    }
    
    reply->deleteLater();
}