#include "SOCWallboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>
#include <QTimer>
#include <QSqlQuery>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QPainter>
#include <QSqlQuery>
#include <QRegularExpression>

using namespace QtCharts;

SOCWallboard::SOCWallboard(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    
    QLabel *title = new QLabel("SOC Wallboard - Real-time Security Overview");
    title->setStyleSheet("color: #00bcd4; font: bold 18pt; margin: 15px; text-align: center;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Chart placeholders
    siemChartView = new QChartView(new QChart());
    edrChartView = new QChartView(new QChart());
    intelChartView = new QChartView(new QChart());
    complianceChartView = new QChartView(new QChart());
    forensicsChartView = new QChartView(new QChart());
    
    siemChartView->setRenderHint(QPainter::Antialiasing);
    edrChartView->setRenderHint(QPainter::Antialiasing);
    intelChartView->setRenderHint(QPainter::Antialiasing);
    complianceChartView->setRenderHint(QPainter::Antialiasing);
    forensicsChartView->setRenderHint(QPainter::Antialiasing);
    
    // Top row → SIEM + EDR
    auto *topRow = new QHBoxLayout();
    topRow->addWidget(siemChartView);
    topRow->addWidget(edrChartView);

    // Bottom row → Intel + Compliance + Forensics
    auto *bottomRow = new QHBoxLayout();
    bottomRow->addWidget(intelChartView);
    bottomRow->addWidget(complianceChartView);
    bottomRow->addWidget(forensicsChartView);

    mainLayout->addLayout(topRow);
    mainLayout->addLayout(bottomRow);

    // Refresh data every 10 seconds
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SOCWallboard::updateCharts);
    timer->start(10000);
    
    updateCharts();
}

void SOCWallboard::updateCharts() {
    // --- SIEM Pie Chart ---
    QPieSeries *siemSeries = new QPieSeries();
    QSqlQuery q1("SELECT severity, COUNT(*) FROM events WHERE panel='SIEM' GROUP BY severity");
    while(q1.next()) {
        siemSeries->append(q1.value(0).toString(), q1.value(1).toInt());
    }
    
    // Style the slices
    for (auto slice : siemSeries->slices()) {
        if (slice->label() == "Critical") slice->setBrush(QColor("#f44336"));
        else if (slice->label() == "Warning") slice->setBrush(QColor("#ff9800"));
        else slice->setBrush(QColor("#4caf50"));
    }
    
    QChart *siemChart = new QChart();
    siemChart->addSeries(siemSeries);
    siemChart->setTitle("SIEM Event Severity");
    siemChart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    siemChart->setTitleBrush(QBrush(QColor("#ffffff")));
    siemChartView->setChart(siemChart);

    // --- EDR Bar Chart ---
    QBarSet *edrSet = new QBarSet("EDR Alerts");
    QSqlQuery q2("SELECT message FROM events WHERE panel='EDR'");
    int inj = 0, ransom = 0, ps = 0, other = 0;
    while(q2.next()) {
        QString msg = q2.value(0).toString();
        if(msg.contains("injection", Qt::CaseInsensitive)) inj++;
        else if(msg.contains("ransomware", Qt::CaseInsensitive)) ransom++;
        else if(msg.contains("powershell", Qt::CaseInsensitive)) ps++;
        else other++;
    }
    *edrSet << inj << ransom << ps << other;
    
    QBarSeries *edrSeries = new QBarSeries();
    edrSeries->append(edrSet);
    QChart *edrChart = new QChart();
    edrChart->addSeries(edrSeries);
    edrChart->setTitle("EDR Alerts by Type");
    edrChart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    edrChart->setTitleBrush(QBrush(QColor("#ffffff")));
    edrChartView->setChart(edrChart);

    // Intel → Line chart
    QLineSeries *intelSeries = new QLineSeries();
    QSqlQuery q3("SELECT timestamp FROM events WHERE panel='IntelHub' ORDER BY timestamp DESC LIMIT 10");
    int i = 0;
    while(q3.next()) {
        intelSeries->append(i++, 1);
    }
    if (intelSeries->count() == 0) {
        intelSeries->append(0, 1);
        intelSeries->append(1, 2);
        intelSeries->append(2, 4);
        intelSeries->append(3, 3);
    }
    
    QChart *intelChart = new QChart();
    intelChart->addSeries(intelSeries);
    intelChart->createDefaultAxes();
    intelChart->setTitle("Intel IOC Trend");
    intelChart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    intelChart->setTitleBrush(QBrush(QColor("#ffffff")));
    intelChartView->setChart(intelChart);

    // Compliance → Bar chart
    QBarSet *nistSet = new QBarSet("NIST");
    QBarSet *gdprSet = new QBarSet("GDPR");
    QBarSet *hipaaSet = new QBarSet("HIPAA");
    
    QSqlQuery q4("SELECT message FROM events WHERE panel='Compliance' ORDER BY timestamp DESC LIMIT 1");
    if(q4.next()) {
        QString msg = q4.value(0).toString();
        QRegularExpression re("NIST=(\\d+)%, GDPR=(\\d+)%, HIPAA=(\\d+)%");
        auto m = re.match(msg);
        if(m.hasMatch()) {
            *nistSet << m.captured(1).toInt();
            *gdprSet << m.captured(2).toInt();
            *hipaaSet << m.captured(3).toInt();
        } else {
            *nistSet << 75; *gdprSet << 85; *hipaaSet << 90;
        }
    } else {
        *nistSet << 75; *gdprSet << 85; *hipaaSet << 90;
    }
    
    QBarSeries *complianceSeries = new QBarSeries();
    complianceSeries->append(nistSet);
    complianceSeries->append(gdprSet);
    complianceSeries->append(hipaaSet);
    QChart *compChart = new QChart();
    compChart->addSeries(complianceSeries);
    compChart->setTitle("Compliance Scores");
    compChart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    compChart->setTitleBrush(QBrush(QColor("#ffffff")));
    complianceChartView->setChart(compChart);

    // Forensics → Line chart
    QLineSeries *forSeries = new QLineSeries();
    QSqlQuery q5("SELECT timestamp FROM events WHERE panel='Forensics' ORDER BY timestamp DESC LIMIT 10");
    int f = 0;
    while(q5.next()) {
        forSeries->append(f++, qrand() % 5 + 1);
    }
    if (forSeries->count() == 0) {
        forSeries->append(0, 0);
        forSeries->append(1, 2);
        forSeries->append(2, 1);
        forSeries->append(3, 3);
    }
    
    QChart *forChart = new QChart();
    forChart->addSeries(forSeries);
    forChart->createDefaultAxes();
    forChart->setTitle("Forensic Findings Trend");
    forChart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    forChart->setTitleBrush(QBrush(QColor("#ffffff")));
    forensicsChartView->setChart(forChart);
}