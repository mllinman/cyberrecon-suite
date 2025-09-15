#include "IntelHub.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QRandomGenerator>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QTimer>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QPainter>
#include <QLabel>


IntelHub::IntelHub(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Threat Intelligence Hub");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    entry = new QLineEdit();
    entry->setPlaceholderText("Enter IP/Domain/Hash...");
    entry->setStyleSheet("padding: 8px; font-size: 12pt;");
    layout->addWidget(entry);

    auto *btn = new QPushButton("Enrich IOC");
    btn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 10px; border-radius: 4px; }");
    layout->addWidget(btn);

    output = new QTextEdit();
    output->setReadOnly(true);
    output->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace;");
    layout->addWidget(output);

    connect(btn, &QPushButton::clicked, this, &IntelHub::lookupIOC);
    connect(entry, &QLineEdit::returnPressed, this, &IntelHub::lookupIOC);
    
    net = new QNetworkAccessManager(this);
    connect(net, &QNetworkAccessManager::finished, this, &IntelHub::onReply);
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &IntelHub::refreshIntel);
    timer->start(15000); // every 15 seconds
    
    showTrend();
    refreshIntel();
}

void IntelHub::lookupIOC() {
    QString ioc = entry->text();
    if (ioc.isEmpty()) return;

    output->append(QString("[Intel Lookup] Analyzing IOC: %1").arg(ioc));
    
    // Simulate IOC analysis (in real implementation, would call VirusTotal, etc.)
    output->append("Reputation: Checking external sources...");
    output->append("Result: Clean (simulated response)");
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('IntelHub', 'Info', :msg)");
    q.bindValue(":msg", QString("IOC lookup performed: %1").arg(ioc));
    q.exec();
}

void IntelHub::showTrend() {
    QLineSeries *series = new QLineSeries();
    series->append(0, 1);
    series->append(1, 3);
    series->append(2, 7);
    series->append(3, 5);
    series->append(4, 8);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("IOC Detection Trend");
    chart->setBackgroundBrush(QBrush(QColor("#2b2b2b")));
    chart->setTitleBrush(QBrush(QColor("#ffffff")));

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMaximumHeight(200);

    layout()->addWidget(view);
}

void IntelHub::refreshIntel() {
    struct Intel { QString text; QString sev; };
    QVector<Intel> feeds = {
        {"IOC Update: IP 185.220.101.1 (Tor Exit Node) → Blacklist", "Warning"},
        {"IOC Update: SHA256=abc123... flagged by VirusTotal (10/70)", "Critical"},
        {"IOC Update: Domain badsite.example added to threat feed", "Info"},
        {"IOC Update: New malware family detected in the wild", "Critical"},
        {"IOC Update: C2 infrastructure taken down", "Info"}
    };

    Intel i = feeds[QRandomGenerator::global()->bounded(feeds.size())];

    QString color = (i.sev == "Critical") ? "#f44336" : (i.sev == "Warning") ? "#ff9800" : "#4caf50";
    QString formatted = QString("<font color='%1'>[Intel %2] %3</font>")
        .arg(color).arg(i.sev).arg(i.text);

    output->append(formatted);

    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('IntelHub', :sev, :msg)");
    q.bindValue(":sev", i.sev);
    q.bindValue(":msg", i.text);
    q.exec();
}

void IntelHub::onReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        output->append("Network error occurred during intel lookup");
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    output->append("Intel feed updated successfully");
    
    reply->deleteLater();
}