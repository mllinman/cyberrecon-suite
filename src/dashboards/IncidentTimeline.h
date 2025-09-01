#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QSqlDatabase>

class IncidentTimeline : public QWidget {
    Q_OBJECT
public:
    explicit IncidentTimeline(QWidget *parent = nullptr);

private slots:
    void exportCSV();
    void refreshTimeline();

private:
    QTextEdit *timelineBox;
    QSqlDatabase db;
};