#include "ThreatHunting.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QRandomGenerator>
#include <QSqlQuery>

ThreatHunting::ThreatHunting(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Threat Hunting Console");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    queryInput = new QLineEdit();
    queryInput->setPlaceholderText("Enter hunt query (SQL-like)");
    queryInput->setStyleSheet("padding: 8px; font-size: 12pt;");
    layout->addWidget(queryInput);

    auto *buttonLayout = new QHBoxLayout();
    auto *runBtn = new QPushButton("Run Hunt");
    auto *clearBtn = new QPushButton("Clear Results");
    
    runBtn->setStyleSheet("QPushButton { background-color: #ff5722; color: white; padding: 10px; border-radius: 4px; }");
    clearBtn->setStyleSheet("QPushButton { background-color: #607d8b; color: white; padding: 10px; border-radius: 4px; }");
    
    buttonLayout->addWidget(runBtn);
    buttonLayout->addWidget(clearBtn);
    layout->addLayout(buttonLayout);

    results = new QTextEdit();
    results->setReadOnly(true);
    results->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace;");
    layout->addWidget(results);

    connect(runBtn, &QPushButton::clicked, this, &ThreatHunting::runHunt);
    connect(clearBtn, &QPushButton::clicked, [this]() { results->clear(); });
    connect(queryInput, &QLineEdit::returnPressed, this, &ThreatHunting::runHunt);

    // Auto-refresh timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ThreatHunting::refreshResults);
    timer->start(20000); // every 20 seconds

    refreshResults();
}

void ThreatHunting::runHunt() {
    QString q = queryInput->text();
    if (q.isEmpty()) {
        results->append("<font color='#ff9800'>[Warning] Please enter a hunt query</font>");
        return;
    }
    
    results->append(QString("<font color='#2196f3'>[Hunt] Executing: %1</font>").arg(q));
    
    // Simulate hunt results based on query
    QStringList mockResults;
    if (q.contains("login", Qt::CaseInsensitive)) {
        mockResults << "Found 3 failed login attempts from 192.168.1.100"
                   << "Detected brute force pattern on user 'admin'"
                   << "Suspicious login times: 02:30 AM, 03:15 AM";
    } else if (q.contains("process", Qt::CaseInsensitive)) {
        mockResults << "Rare process detected: unknown.exe"
                   << "Process spawned from unusual location: C:\\Temp\\"
                   << "Parent-child process anomaly detected";
    } else if (q.contains("network", Qt::CaseInsensitive)) {
        mockResults << "Unusual DNS requests to suspicious domains"
                   << "High volume data transfer detected"
                   << "Connection to known C2 infrastructure";
    } else {
        mockResults << "Generic hunt results: 5 anomalies detected"
                   << "Behavioral analysis complete"
                   << "Threat indicators found in logs";
    }
    
    for (const QString &result : mockResults) {
        results->append(QString("<font color='#4caf50'>[Result] %1</font>").arg(result));
    }
    
    // Log to database
    QSqlQuery query;
    query.prepare("INSERT INTO events (panel, severity, message) VALUES ('ThreatHunting', 'Info', :msg)");
    query.bindValue(":msg", QString("Hunt executed: %1").arg(q));
    query.exec();
}

void ThreatHunting::refreshResults() {
    QStringList hunts = {
        "Hunt Query: failed logins > 5 in 10 mins → 2 matches found",
        "Hunt Query: rare process names → detected unknown.exe in 3 hosts",
        "Hunt Query: external DNS requests → 7 anomalies identified",
        "Hunt Query: privilege escalation → 1 suspicious event detected",
        "Hunt Query: lateral movement → network scan pattern found"
    };

    QString res = hunts[qrand() % hunts.size()];
    QString res = hunts[QRandomGenerator::global()->bounded(hunts.size())];
    results->append(QString("<font color='#ff9800'>[Auto-Hunt] %1</font>").arg(res));
    
    // Log to database
    QSqlQuery query;
    query.prepare("INSERT INTO events (panel, severity, message) VALUES ('ThreatHunting', 'Warning', :msg)");
    query.bindValue(":msg", res);
    query.exec();
}