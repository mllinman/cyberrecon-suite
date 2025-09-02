#include "AuditLogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMutexLocker>
#include <QTextStream>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

AuditLogger* AuditLogger::m_instance = nullptr;

AuditLogger* AuditLogger::instance() {
    if (!m_instance) {
        m_instance = new AuditLogger();
    }
    return m_instance;
}

AuditLogger::AuditLogger(QObject *parent) : QObject(parent) {
    initializeDatabase();
}

void AuditLogger::initializeDatabase() {
    auditDb = QSqlDatabase::addDatabase("QSQLITE", "audit_log");
    auditDb.setDatabaseName("cyberrecon_audit.db");
    
    if (!auditDb.open()) {
        qCritical() << "Failed to open audit database:" << auditDb.lastError().text();
        return;
    }
    
    QSqlQuery query(auditDb);
    
    // Create comprehensive audit log table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS audit_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            category TEXT NOT NULL,
            event_type TEXT NOT NULL,
            username TEXT,
            source_ip TEXT,
            resource TEXT,
            action TEXT,
            result TEXT,
            details TEXT,
            risk_level TEXT,
            compliance_framework TEXT,
            session_id TEXT,
            user_agent TEXT
        )
    )");
    
    // Create SOC 2 specific audit table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS soc2_audit (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            trust_criteria TEXT NOT NULL,
            control_id TEXT NOT NULL,
            status TEXT NOT NULL,
            evidence TEXT,
            auditor TEXT,
            remediation_required BOOLEAN DEFAULT FALSE,
            next_review_date DATETIME
        )
    )");
    
    // Create data access log for privacy compliance
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS data_access_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            username TEXT NOT NULL,
            data_type TEXT NOT NULL,
            operation TEXT NOT NULL,
            record_count INTEGER DEFAULT 1,
            purpose TEXT,
            retention_period TEXT,
            encryption_status TEXT
        )
    )");
    
    // Log audit system initialization
    logSystemEvent("Audit logging system initialized", Info, "SOC 2 compliance audit trail active");
}

void AuditLogger::logUserAction(const QString &username, const QString &action, const QString &resource) {
    QMutexLocker locker(&logMutex);
    
    QSqlQuery query(auditDb);
    query.prepare(R"(
        INSERT INTO audit_log (category, event_type, username, action, resource, risk_level) 
        VALUES ('User Action', 'User Activity', ?, ?, ?, 'Low')
    )");
    query.addBindValue(username);
    query.addBindValue(action);
    query.addBindValue(resource);
    query.exec();
}

void AuditLogger::logSystemEvent(const QString &event, AuditLevel level, const QString &details) {
    QMutexLocker locker(&logMutex);
    
    QString levelStr;
    QString riskLevel;
    
    switch (level) {
        case Info:
            levelStr = "Info";
            riskLevel = "Low";
            break;
        case Warning:
            levelStr = "Warning";
            riskLevel = "Medium";
            break;
        case Critical:
            levelStr = "Critical";
            riskLevel = "High";
            break;
        case Security:
            levelStr = "Security";
            riskLevel = "High";
            break;
        case Compliance:
            levelStr = "Compliance";
            riskLevel = "Medium";
            break;
    }
    
    QSqlQuery query(auditDb);
    query.prepare(R"(
        INSERT INTO audit_log (category, event_type, action, details, risk_level) 
        VALUES ('System Event', ?, ?, ?, ?)
    )");
    query.addBindValue(levelStr);
    query.addBindValue(event);
    query.addBindValue(details);
    query.addBindValue(riskLevel);
    query.exec();
}

void AuditLogger::logSecurityEvent(const QString &event, const QString &sourceIP, const QString &details) {
    QMutexLocker locker(&logMutex);
    
    QSqlQuery query(auditDb);
    query.prepare(R"(
        INSERT INTO audit_log (category, event_type, source_ip, action, details, risk_level, compliance_framework) 
        VALUES ('Security Event', 'Security Incident', ?, ?, ?, 'High', 'SOC2')
    )");
    query.addBindValue(sourceIP);
    query.addBindValue(event);
    query.addBindValue(details);
    query.exec();
}

void AuditLogger::logDataAccess(const QString &username, const QString &dataType, const QString &operation) {
    QMutexLocker locker(&logMutex);
    
    // Log to main audit table
    QSqlQuery auditQuery(auditDb);
    auditQuery.prepare(R"(
        INSERT INTO audit_log (category, event_type, username, action, resource, risk_level, compliance_framework) 
        VALUES ('Data Access', 'Data Operation', ?, ?, ?, 'Medium', 'SOC2,GDPR')
    )");
    auditQuery.addBindValue(username);
    auditQuery.addBindValue(operation);
    auditQuery.addBindValue(dataType);
    auditQuery.exec();
    
    // Log to specific data access table
    QSqlQuery dataQuery(auditDb);
    dataQuery.prepare(R"(
        INSERT INTO data_access_log (username, data_type, operation, purpose, encryption_status) 
        VALUES (?, ?, ?, 'Business Operations', 'AES-256 Encrypted')
    )");
    dataQuery.addBindValue(username);
    dataQuery.addBindValue(dataType);
    dataQuery.addBindValue(operation);
    dataQuery.exec();
}

void AuditLogger::logComplianceEvent(const QString &framework, const QString &control, const QString &status) {
    QMutexLocker locker(&logMutex);
    
    QSqlQuery query(auditDb);
    query.prepare(R"(
        INSERT INTO audit_log (category, event_type, action, resource, result, compliance_framework, risk_level) 
        VALUES ('Compliance', 'Control Assessment', ?, ?, ?, ?, 'Medium')
    )");
    query.addBindValue(control);
    query.addBindValue(framework);
    query.addBindValue(status);
    query.addBindValue(framework);
    query.exec();
}

void AuditLogger::logAuthenticationEvent(const QString &username, const QString &result, const QString &sourceIP) {
    QMutexLocker locker(&logMutex);
    
    QString riskLevel = (result == "Success") ? "Low" : "High";
    
    QSqlQuery query(auditDb);
    query.prepare(R"(
        INSERT INTO audit_log (category, event_type, username, source_ip, result, risk_level, compliance_framework) 
        VALUES ('Authentication', 'Login Attempt', ?, ?, ?, ?, 'SOC2')
    )");
    query.addBindValue(username);
    query.addBindValue(sourceIP);
    query.addBindValue(result);
    query.addBindValue(riskLevel);
    query.exec();
}

void AuditLogger::logSOC2Event(const QString &trustCriteria, const QString &control, const QString &status, const QString &evidence) {
    QMutexLocker locker(&logMutex);
    
    QSqlQuery query(auditDb);
    query.prepare(R"(
        INSERT INTO soc2_audit (trust_criteria, control_id, status, evidence, auditor, next_review_date) 
        VALUES (?, ?, ?, ?, 'System Auditor', ?)
    )");
    query.addBindValue(trustCriteria);
    query.addBindValue(control);
    query.addBindValue(status);
    query.addBindValue(evidence);
    query.addBindValue(QDateTime::currentDateTime().addDays(90)); // Next review in 90 days
    query.exec();
}

QString AuditLogger::generateAuditReport(const QDateTime &startDate, const QDateTime &endDate) {
    QString report;
    QTextStream stream(&report);
    
    stream << "=====================================\n";
    stream << "SOC 2 AUDIT REPORT\n";
    stream << "BulletDrop Studios - CyberRecon Suite\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    stream << "Period: " << startDate.toString("yyyy-MM-dd") << " to " << endDate.toString("yyyy-MM-dd") << "\n";
    stream << "=====================================\n\n";
    
    // Summary statistics
    QSqlQuery statsQuery(auditDb);
    statsQuery.prepare("SELECT category, COUNT(*) FROM audit_log WHERE timestamp BETWEEN ? AND ? GROUP BY category");
    statsQuery.addBindValue(startDate);
    statsQuery.addBindValue(endDate);
    
    stream << "AUDIT SUMMARY\n";
    stream << "-------------\n";
    
    if (statsQuery.exec()) {
        while (statsQuery.next()) {
            stream << statsQuery.value(0).toString() << ": " << statsQuery.value(1).toString() << " events\n";
        }
    }
    
    stream << "\nDETAILED AUDIT LOG\n";
    stream << "------------------\n";
    
    // Detailed audit entries
    QSqlQuery detailQuery(auditDb);
    detailQuery.prepare("SELECT timestamp, category, event_type, username, action, resource, result, risk_level FROM audit_log WHERE timestamp BETWEEN ? AND ? ORDER BY timestamp DESC");
    detailQuery.addBindValue(startDate);
    detailQuery.addBindValue(endDate);
    
    if (detailQuery.exec()) {
        while (detailQuery.next()) {
            stream << "[" << detailQuery.value(0).toString() << "] "
                   << detailQuery.value(1).toString() << " - "
                   << detailQuery.value(2).toString() << " - "
                   << detailQuery.value(3).toString() << " - "
                   << detailQuery.value(4).toString() << " - "
                   << detailQuery.value(5).toString() << " - "
                   << detailQuery.value(6).toString() << " ("
                   << detailQuery.value(7).toString() << ")\n";
        }
    }
    
    return report;
}

void AuditLogger::exportAuditLog(const QString &filePath, const QDateTime &startDate, const QDateTime &endDate) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        
        QDateTime start = startDate.isValid() ? startDate : QDateTime::currentDateTime().addDays(-30);
        QDateTime end = endDate.isValid() ? endDate : QDateTime::currentDateTime();
        
        QString report = generateAuditReport(start, end);
        out << report;
        
        file.close();
    }
}