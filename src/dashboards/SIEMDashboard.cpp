#include "SIEMDashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QTimer>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QPainter>

using namespace QtCharts;

SIEMDashboard::SIEMDashboard(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("SIEM Dashboard - Security Event Monitoring");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // Create chart
    createChart();
    layout->addWidget(chartView);

    logBox = new QTextEdit();
    logBox->setReadOnly(true);
    logBox->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace;");
    layout->addWidget(logBox);

    auto *buttonLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("Simulate SIEM Event");
    QPushButton *refreshBtn = new QPushButton("Refresh Events");
    
    addBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px; border-radius: 4px; }");
    refreshBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 8px; border-radius: 4px; }");
    
    buttonLayout->addWidget(addBtn);
    buttonLayout->addWidget(refreshBtn);
    layout->addLayout(buttonLayout);

    connect(addBtn, &QPushButton::clicked, this, &SIEMDashboard::simulateEvent);
    connect(refreshBtn, &QPushButton::clicked, this, &SIEMDashboard::refreshEvents);

    // Auto-refresh timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SIEMDashboard::refreshEvents);
    timer->start(10000); // every 10 seconds

    // Initial data
    refreshEvents();
}

void SIEMDashboard::createChart() {
    series = new QPieSeries();
    series->append("Info", 0);
    series->append("Warning", 0);
    series->append("Critical", 0);

    // Style the slices
    series->slices().at(0)->setBrush(QColor("#4caf50"));
    series->slices().at(1)->setBrush(QColor("#ff9800"));
    series->slices().at(2)->setBrush(QColor("#f44336"));

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("SIEM Event Severity Distribution");
    chart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    chart->setTitleBrush(QBrush(QColor("#ffffff")));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMaximumHeight(300);
}

void SIEMDashboard::simulateEvent() {
    QString alert = "Suspicious login attempt detected from 192.168.1.100";
    logBox->append(QString("[SIEM Critical] %1").arg(alert));

    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SIEM', 'Critical', :msg)");
    q.bindValue(":msg", alert);
    q.exec();

    updateChart();
}

void SIEMDashboard::refreshEvents() {
    static int counter = 0;
    struct Event { QString text; QString sev; };
    QVector<Event> mockEvents = {
        {"Login success: user=analyst_john ip=192.168.1.10", "Info"},
        {"Login failure: user=unknown ip=203.0.113.55", "Warning"},
        {"Suspicious command execution: whoami from host=10.0.0.5", "Critical"},
        {"Privilege escalation detected: service_account", "Critical"},
        {"Firewall rule triggered: blocked 192.168.1.200", "Warning"}
    };

    Event e = mockEvents[counter % mockEvents.size()];
    counter++;

    QString color = (e.sev == "Critical") ? "#f44336" : (e.sev == "Warning") ? "#ff9800" : "#4caf50";
    QString formatted = QString("<font color='%1'>[SIEM %2] %3</font>")
        .arg(color).arg(e.sev).arg(e.text);

    logBox->append(formatted);

    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SIEM', :sev, :msg)");
    q.bindValue(":sev", e.sev);
    q.bindValue(":msg", e.text);
    q.exec();

    updateChart();
}

void SIEMDashboard::updateChart() {
    // Query database for current counts
    QSqlQuery q("SELECT severity, COUNT(*) FROM events WHERE panel='SIEM' GROUP BY severity");
    
    int info = 0, warning = 0, critical = 0;
    while (q.next()) {
        QString sev = q.value(0).toString();
        int count = q.value(1).toInt();
        
        if (sev == "Info") info = count;
        else if (sev == "Warning") warning = count;
        else if (sev == "Critical") critical = count;
    }

    series->slices().at(0)->setValue(info);
    series->slices().at(1)->setValue(warning);
    series->slices().at(2)->setValue(critical);
}