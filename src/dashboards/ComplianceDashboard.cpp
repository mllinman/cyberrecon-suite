#include "ComplianceDashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QTimer>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QPainter>

using namespace QtCharts;

ComplianceDashboard::ComplianceDashboard(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Compliance Dashboard");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // Create chart
    createChart();
    layout->addWidget(chartView);

    // Sliders for compliance scores
    auto *slidersLayout = new QVBoxLayout();
    
    auto *nistLayout = new QHBoxLayout();
    nistLayout->addWidget(new QLabel("NIST Compliance:"));
    nistSlider = new QSlider(Qt::Horizontal);
    nistSlider->setRange(0, 100);
    nistSlider->setValue(75);
    nistLayout->addWidget(nistSlider);
    nistLabel = new QLabel("75%");
    nistLayout->addWidget(nistLabel);
    slidersLayout->addLayout(nistLayout);

    auto *gdprLayout = new QHBoxLayout();
    gdprLayout->addWidget(new QLabel("GDPR Compliance:"));
    gdprSlider = new QSlider(Qt::Horizontal);
    gdprSlider->setRange(0, 100);
    gdprSlider->setValue(85);
    gdprLayout->addWidget(gdprSlider);
    gdprLabel = new QLabel("85%");
    gdprLayout->addWidget(gdprLabel);
    slidersLayout->addLayout(gdprLayout);

    auto *hipaaLayout = new QHBoxLayout();
    hipaaLayout->addWidget(new QLabel("HIPAA Compliance:"));
    hipaaSlider = new QSlider(Qt::Horizontal);
    hipaaSlider->setRange(0, 100);
    hipaaSlider->setValue(90);
    hipaaLayout->addWidget(hipaaSlider);
    hipaaLabel = new QLabel("90%");
    hipaaLayout->addWidget(hipaaLabel);
    slidersLayout->addLayout(hipaaLayout);

    layout->addLayout(slidersLayout);

    QPushButton *saveBtn = new QPushButton("Save Compliance Scores");
    saveBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px; border-radius: 4px; }");
    layout->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &ComplianceDashboard::saveScores);
    connect(nistSlider, &QSlider::valueChanged, this, &ComplianceDashboard::updateLabels);
    connect(gdprSlider, &QSlider::valueChanged, this, &ComplianceDashboard::updateLabels);
    connect(hipaaSlider, &QSlider::valueChanged, this, &ComplianceDashboard::updateLabels);

    // Auto-refresh timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ComplianceDashboard::refreshCompliance);
    timer->start(30000); // every 30 seconds

    updateChart();
}

void ComplianceDashboard::createChart() {
    nistSet = new QBarSet("NIST");
    gdprSet = new QBarSet("GDPR");
    hipaaSet = new QBarSet("HIPAA");

    *nistSet << 75;
    *gdprSet << 85;
    *hipaaSet << 90;

    QBarSeries *series = new QBarSeries();
    series->append(nistSet);
    series->append(gdprSet);
    series->append(hipaaSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Compliance Scores");
    chart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    chart->setTitleBrush(QBrush(QColor("#ffffff")));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMaximumHeight(300);
}

void ComplianceDashboard::saveScores() {
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Compliance', 'Info', :msg)");
    q.bindValue(":msg", QString("NIST=%1%, GDPR=%2%, HIPAA=%3%")
                          .arg(nistSlider->value())
                          .arg(gdprSlider->value())
                          .arg(hipaaSlider->value()));
    q.exec();

    updateChart();
}

void ComplianceDashboard::updateLabels() {
    nistLabel->setText(QString("%1%").arg(nistSlider->value()));
    gdprLabel->setText(QString("%1%").arg(gdprSlider->value()));
    hipaaLabel->setText(QString("%1%").arg(hipaaSlider->value()));
    updateChart();
}

void ComplianceDashboard::updateChart() {
    nistSet->replace(0, nistSlider->value());
    gdprSet->replace(0, gdprSlider->value());
    hipaaSet->replace(0, hipaaSlider->value());
}

void ComplianceDashboard::refreshCompliance() {
    int nist = 70 + QRandomGenerator::global()->bounded(30);
    int gdpr = 75 + QRandomGenerator::global()->bounded(25);
    int hipaa = 80 + QRandomGenerator::global()->bounded(20);

    nistSlider->setValue(nist);
    gdprSlider->setValue(gdpr);
    hipaaSlider->setValue(hipaa);

    QString msg = QString("Compliance auto-update → NIST=%1% GDPR=%2% HIPAA=%3%")
                    .arg(nist).arg(gdpr).arg(hipaa);

    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Compliance', 'Info', :msg)");
    q.bindValue(":msg", msg);
    q.exec();
}