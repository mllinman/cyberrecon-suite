#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QSqlDatabase>
#include <QDateTime>

class SOC2Compliance : public QWidget {
    Q_OBJECT

public:
    explicit SOC2Compliance(QWidget *parent = nullptr);

private slots:
    void runComplianceAudit();
    void generateSOC2Report();
    void exportAuditLog();
    void refreshCompliance();
    void viewControlDetails();

private:
    void setupUI();
    void initializeDatabase();
    void loadSOC2Controls();
    void updateComplianceStatus();
    void logComplianceEvent(const QString &control, const QString &status, const QString &details);
    QString assessControlCompliance(const QString &controlId);
    void generateControlEvidence(const QString &controlId);
    
    QTableWidget *controlsTable;
    QTextEdit *auditLog;
    QTextEdit *reportPreview;
    QComboBox *auditScopeCombo;
    QProgressBar *complianceProgress;
    QLabel *overallScoreLabel;
    QLabel *lastAuditLabel;
    QPushButton *auditBtn;
    QPushButton *reportBtn;
    QPushButton *exportBtn;
    QTimer *refreshTimer;
    
    QSqlDatabase complianceDb;
    double overallComplianceScore;
};