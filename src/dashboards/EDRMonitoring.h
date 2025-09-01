#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>

using namespace QtCharts;

class EDRMonitoring : public QWidget {
    Q_OBJECT
public:
    explicit EDRMonitoring(QWidget *parent = nullptr);

private slots:
    void simulateAlert();
    void refreshAlerts();

private:
    void createChart();
    void updateChart();
    
    QTextEdit *output;
    QBarSet *edrSet;
    QChartView *chartView;
};