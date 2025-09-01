#pragma once
#include <QWidget>
#include <QtCharts/QChartView>

using namespace QtCharts;

class SOCWallboard : public QWidget {
    Q_OBJECT
public:
    explicit SOCWallboard(QWidget *parent = nullptr);

private slots:
    void updateCharts();

private:
    QChartView *siemChartView;
    QChartView *edrChartView;
    QChartView *intelChartView;
    QChartView *complianceChartView;
    QChartView *forensicsChartView;
};