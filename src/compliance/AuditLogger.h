#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QMutex>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    enum AuditLevel {
        Info,
        Warning,
        Critical,
        Security,
        Compliance
    };

    static AuditLogger* instance();
    
    void logUserAction(const QString &username, const QString &action, const QString &resource = "");
    void logSystemEvent(const QString &event, AuditLevel level, const QString &details = "");
    void logSecurityEvent(const QString &event, const QString &sourceIP, const QString &details = "");
    void logDataAccess(const QString &username, const QString &dataType, const QString &operation);
    void logComplianceEvent(const QString &framework, const QString &control, const QString &status);
    void logAuthenticationEvent(const QString &username, const QString &result, const QString &sourceIP);
    
    // SOC 2 specific logging
    void logSOC2Event(const QString &trustCriteria, const QString &control, const QString &status, const QString &evidence = "");
    
    QString generateAuditReport(const QDateTime &startDate, const QDateTime &endDate);
    void exportAuditLog(const QString &filePath, const QDateTime &startDate = QDateTime(), const QDateTime &endDate = QDateTime());

private:
    explicit AuditLogger(QObject *parent = nullptr);
    void initializeDatabase();
    QString formatAuditEntry(const QString &category, const QString &event, const QString &details, AuditLevel level);
    
    static AuditLogger *m_instance;
    QSqlDatabase auditDb;
    QMutex logMutex;
};

// Convenience macros for audit logging
#define AUDIT_USER_ACTION(username, action, resource) \
    AuditLogger::instance()->logUserAction(username, action, resource)

#define AUDIT_SYSTEM_EVENT(event, level, details) \
    AuditLogger::instance()->logSystemEvent(event, AuditLogger::level, details)

#define AUDIT_SECURITY_EVENT(event, sourceIP, details) \
    AuditLogger::instance()->logSecurityEvent(event, sourceIP, details)

#define AUDIT_SOC2_EVENT(criteria, control, status, evidence) \
    AuditLogger::instance()->logSOC2Event(criteria, control, status, evidence)
