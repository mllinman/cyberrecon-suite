#include "Forensics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QPainter>

using namespace QtCharts;

Forensics::Forensics(QWidget *parent) : QWidget(parent), cycle(0) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Digital Forensics Tools");
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
    QPushButton *memBtn = new QPushButton("Analyze Memory Dump");
    QPushButton *regBtn = new QPushButton("Dump Registry Keys");
    QPushButton *netBtn = new QPushButton("Capture Network Packets");

    memBtn->setStyleSheet("QPushButton { background-color: #9c27b0; color: white; padding: 8px; border-radius: 4px; }");
    regBtn->setStyleSheet("QPushButton { background-color: #673ab7; color: white; padding: 8px; border-radius: 4px; }");
    netBtn->setStyleSheet("QPushButton { background-color: #3f51b5; color: white; padding: 8px; border-radius: 4px; }");

    buttonLayout->addWidget(memBtn);
    buttonLayout->addWidget(regBtn);
    buttonLayout->addWidget(netBtn);
    layout->addLayout(buttonLayout);

    connect(memBtn, &QPushButton::clicked, [this]() { 
        output->append("[Forensics] Memory dump analysis started...");
        output->append("[Forensics] Found 3 suspicious processes in memory");
    });
    connect(regBtn, &QPushButton::clicked, [this]() { 
        output->append("[Forensics] Registry dump completed");
        output->append("[Forensics] Persistence keys found in HKCU\\Run");
    });
    connect(netBtn, &QPushButton::clicked, [this]() { 
        output->append("[Forensics] Network packet capture initiated");
        output->append("[Forensics] Capturing traffic on all interfaces");
    });

    // Auto-refresh timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Forensics::refreshForensics);
    timer->start(12000); // every 12 seconds

    refreshForensics();
}

void Forensics::createChart() {
    forSeries = new QLineSeries();
    
    QChart *chart = new QChart();
    chart->addSeries(forSeries);
    chart->createDefaultAxes();
    chart->setTitle("Forensic Findings Over Time");
    chart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    chart->setTitleBrush(QBrush(QColor("#ffffff")));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMaximumHeight(250);
}

void Forensics::refreshForensics() {
    QStringList mockFindings = {
        "Memory scan: found injected thread in notepad.exe",
        "Browser history: suspicious URL → hxxp://malicious.test",
        "Registry: persistence key detected at HKCU\\Run\\evil.exe",
        "Packet capture: outbound connection to 45.67.89.101",
        "File system: hidden executable found in temp directory",
        "Process analysis: hollowed process detected"
    };

    QString finding = mockFindings[qrand() % mockFindings.size()];
    QString finding = mockFindings[QRandomGenerator::global()->bounded(mockFindings.size())];
    QString color = finding.contains("suspicious") || finding.contains("evil") || finding.contains("malicious") ? "#f44336" : "#4caf50";
    
    output->append(QString("<font color='%1'>[Forensics] %2</font>").arg(color).arg(finding));
    
    // Update chart
    forSeries->append(cycle++, QRandomGenerator::global()->bounded(10));
    if (forSeries->count() > 20) {
        forSeries->remove(0);
    }

    // Log to database
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Forensics', 'Info', :msg)");
    q.bindValue(":msg", finding);
    q.exec();
}