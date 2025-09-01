#pragma once
#include <QWidget>
#include <QComboBox>
#include <QTextEdit>

class ReportGenerator : public QWidget {
    Q_OBJECT
public:
    explicit ReportGenerator(QWidget *parent = nullptr);

private slots:
    void generateReport();
    void exportTxt();
    void exportCsv();

private:
    QString generateIncidentSummary();
    QString generateThreatIntelReport();
    QString generateComplianceReport();
    QString generateForensicsReport();
    QString generateExecutiveReport();
    
    QComboBox *reportTypeCombo;
    QTextEdit *reportPreview;
};