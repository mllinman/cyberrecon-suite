#include "EDRMonitoring.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QTimer>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QPainter>

using namespace QtCharts;

EDRMonitoring::EDRMonitoring(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("EDR Monitoring - Endpoint Detection & Response");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // Create chart
    createChart();
    layout->addWidget(chartView);

    output = new QTextEdit();
    output->setReadOnly(true);
    output->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace;");
    layout->addWidget(output);

    auto *buttonLayout = new QHBoxLayout();
    QPushButton *btn = new QPushButton("Simulate EDR Alert");
    QPushButton *refreshBtn = new QPushButton("Refresh Alerts");
    
    btn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    refreshBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 8px; border-radius: 4px; }");
    
    buttonLayout->addWidget(btn);
    buttonLayout->addWidget(refreshBtn);
    layout->addLayout(buttonLayout);

    connect(btn, &QPushButton::clicked, this, &EDRMonitoring::simulateAlert);
    connect(refreshBtn, &QPushButton::clicked, this, &EDRMonitoring::refreshAlerts);

    // Auto-refresh timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EDRMonitoring::refreshAlerts);
    timer->start(8000); // every 8 seconds

    refreshAlerts();
}

void EDRMonitoring::createChart() {
    edrSet = new QBarSet("EDR Alerts");
    *edrSet << 0 << 0 << 0 << 0; // injection, ransomware, powershell, other

    QBarSeries *series = new QBarSeries();
    series->append(edrSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("EDR Alert Categories");
    chart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    chart->setTitleBrush(QBrush(QColor("#ffffff")));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMaximumHeight(300);
}

void EDRMonitoring::simulateAlert() {
    QString alert = "Process injection detected in explorer.exe";
    output->append(QString("[EDR Critical] %1").arg(alert));

    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('EDR', 'Critical', :msg)");
    q.bindValue(":msg", alert);
    q.exec();

    updateChart();
}

void EDRMonitoring::refreshAlerts() {
    static int counter = 0;
    struct Alert { QString text; QString sev; };
    QVector<Alert> mockAlerts = {
        {"Process injection detected: explorer.exe → lsass.exe", "Critical"},
        {"Ransomware behavior flagged: high file rename rate", "Critical"},
        {"Suspicious PowerShell execution: base64 script", "Warning"},
        {"Unusual connection: chrome.exe → 198.51.100.44", "Info"},
        {"Memory scan: shellcode detected in notepad.exe", "Critical"}
    };

    Alert a = mockAlerts[counter % mockAlerts.size()];
    counter++;

    QString color = (a.sev == "Critical") ? "#f44336" : (a.sev == "Warning") ? "#ff9800" : "#4caf50";
    QString formatted = QString("<font color='%1'>[EDR %2] %3</font>")
        .arg(color).arg(a.sev).arg(a.text);

    output->append(formatted);

    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('EDR', :sev, :msg)");
    q.bindValue(":sev", a.sev);
    q.bindValue(":msg", a.text);
    q.exec();

    updateChart();
}

void EDRMonitoring::updateChart() {
    // Query database for current counts
    QSqlQuery q("SELECT message FROM events WHERE panel='EDR'");
    int injection = 0, ransomware = 0, powershell = 0, other = 0;
    
    while (q.next()) {
        QString msg = q.value(0).toString();
        if (msg.contains("injection", Qt::CaseInsensitive)) injection++;
        else if (msg.contains("ransomware", Qt::CaseInsensitive)) ransomware++;
        else if (msg.contains("powershell", Qt::CaseInsensitive)) powershell++;
        else other++;
    }

    edrSet->replace(0, injection);
    edrSet->replace(1, ransomware);
    edrSet->replace(2, powershell);
    edrSet->replace(3, other);
}