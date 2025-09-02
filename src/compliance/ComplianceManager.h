#pragma once
#include <QObject>
#include <QTimer>
#include <QSqlDatabase>
#include <QDateTime>

class ComplianceManager : public QObject {
    Q_OBJECT

public:
    explicit ComplianceManager(QObject *parent = nullptr);
    
    // Compliance framework management
    void enableSOC2Compliance(bool enabled = true);
    void enableGDPRCompliance(bool enabled = true);
    void enableHIPAACompliance(bool enabled = true);
    void enableNISTCompliance(bool enabled = true);
    
    // Real-time compliance monitoring
    void startComplianceMonitoring();
    void stopComplianceMonitoring();
    
    // Compliance assessments
    double getOverallComplianceScore();
    double getFrameworkScore(const QString &framework);
    QStringList getComplianceGaps();
    
    // Automated compliance checks
    void performAutomatedChecks();
    void scheduleComplianceAudit(const QDateTime &scheduledTime);

signals:
    void complianceViolationDetected(const QString &framework, const QString &violation);
    void complianceScoreUpdated(const QString &framework, double score);
    void auditRequired(const QString &framework, const QString &reason);

private slots:
    void performPeriodicChecks();
    void checkDataRetentionCompliance();
    void checkAccessControlCompliance();
    void checkEncryptionCompliance();
    void checkBackupCompliance();

private:
    void initializeComplianceDatabase();
    void loadComplianceFrameworks();
    void assessControlImplementation(const QString &framework, const QString &controlId);
    void generateComplianceEvidence();
    
    QTimer *monitoringTimer;
    QSqlDatabase complianceDb;
    
    bool soc2Enabled;
    bool gdprEnabled;
    bool hipaaEnabled;
    bool nistEnabled;
    
    QDateTime lastAuditDate;
    double currentComplianceScore;
};