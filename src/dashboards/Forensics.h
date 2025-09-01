#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>

using namespace QtCharts;

class Forensics : public QWidget {
    Q_OBJECT
public:
    explicit Forensics(QWidget *parent = nullptr);

private slots:
    void refreshForensics();

private:
    void createChart();
    
    QTextEdit *output;
    QLineSeries *forSeries;
    QChartView *chartView;
    int cycle;
};