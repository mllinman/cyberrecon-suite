#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>

using namespace QtCharts;

class SIEMDashboard : public QWidget {
    Q_OBJECT
public:
    explicit SIEMDashboard(QWidget *parent = nullptr);

private slots:
    void simulateEvent();
    void refreshEvents();

private:
    void createChart();
    void updateChart();
    
    QTextEdit *logBox;
    QPieSeries *series;
    QChartView *chartView;
};