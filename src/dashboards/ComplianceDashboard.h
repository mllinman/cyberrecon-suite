#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>

using namespace QtCharts;

class ComplianceDashboard : public QWidget {
    Q_OBJECT
public:
    explicit ComplianceDashboard(QWidget *parent = nullptr);

private slots:
    void saveScores();
    void updateLabels();
    void refreshCompliance();

private:
    void createChart();
    void updateChart();
    
    QSlider *nistSlider;
    QSlider *gdprSlider;
    QSlider *hipaaSlider;
    QLabel *nistLabel;
    QLabel *gdprLabel;
    QLabel *hipaaLabel;
    QBarSet *nistSet;
    QBarSet *gdprSet;
    QBarSet *hipaaSet;
    QChartView *chartView;
};