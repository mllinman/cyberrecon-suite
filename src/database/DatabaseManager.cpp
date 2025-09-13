#include "DatabaseManager.h"
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QLoggingCategory>
#include <QMutexLocker>
#include <QDateTime>
#include <QCoreApplication>

Q_LOGGING_CATEGORY(dbManager, "cyberrecon.database")

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager* DatabaseManager::instance() {
    static QMutex instanceMutex;
    QMutexLocker locker(&instanceMutex);
    
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_maxConnectionsPerType(5)
    , m_connectionTimeoutSeconds(300) // 5 minutes
    , m_queryCachingEnabled(true)
    , m_totalQueries(0)
    , m_totalExecutionTime(0)
    , m_startTime(QDateTime::currentDateTime())
{
    // Initialize health check timer
    m_healthCheckTimer = new QTimer(this);
    connect(m_healthCheckTimer, &QTimer::timeout, this, &DatabaseManager::checkConnectionHealth);
    m_healthCheckTimer->start(30000); // Check every 30 seconds
    
    // Initialize cleanup timer
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &DatabaseManager::cleanupIdleConnections);
    m_cleanupTimer->start(60000); // Cleanup every minute
    
    qCInfo(dbManager) << "DatabaseManager initialized with connection pooling";
}

DatabaseManager::~DatabaseManager() {
    // Close all connections
    QMutexLocker locker(&m_connectionMutex);
    
    for (auto it = m_availableConnections.begin(); it != m_availableConnections.end(); ++it) {
        while (!it.value().isEmpty()) {
            QString connectionName = it.value().dequeue();
            QSqlDatabase::removeDatabase(connectionName);
        }
    }
    
    qCInfo(dbManager) << "DatabaseManager destroyed, all connections closed";
}

bool DatabaseManager::initializeDatabase(DatabaseType type, const QString &databasePath) {
    QMutexLocker locker(&m_connectionMutex);
    
    QString actualPath = databasePath.isEmpty() ? getDatabasePath(type) : databasePath;
    m_databasePaths[type] = actualPath;
    
    // Ensure directory exists
    QFileInfo fileInfo(actualPath);
    QDir().mkpath(fileInfo.absolutePath());
    
    setupConnectionPool(type, actualPath);
    
    // Test initial connection
    QSqlDatabase testDb = getConnection(type);
    if (!testDb.isOpen()) {
        qCCritical(dbManager) << "Failed to open database:" << actualPath << testDb.lastError().text();
        return false;
    }
    
    // Optimize database settings
    QSqlQuery query(testDb);
    
    // Enable WAL mode for better concurrency and performance
    query.exec("PRAGMA journal_mode = WAL");
    query.exec("PRAGMA synchronous = NORMAL"); // Balance between safety and speed
    query.exec("PRAGMA cache_size = 10000"); // 10MB cache
    query.exec("PRAGMA temp_store = MEMORY"); // Use memory for temporary tables
    query.exec("PRAGMA mmap_size = 268435456"); // 256MB memory map
    
    releaseConnection(testDb.connectionName());
    
    qCInfo(dbManager) << "Database initialized successfully:" << actualPath;
    return true;
}

QSqlDatabase DatabaseManager::getConnection(DatabaseType type) {
    QMutexLocker locker(&m_connectionMutex);
    
    // Check if we have available connections
    if (m_availableConnections.contains(type) && !m_availableConnections[type].isEmpty()) {
        QString connectionName = m_availableConnections[type].dequeue();
        m_connectionLastUsed[connectionName] = QDateTime::currentDateTime();
        
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if (db.isOpen()) {
            return db;
        } else {
            // Connection is stale, remove it
            QSqlDatabase::removeDatabase(connectionName);
            m_connectionLastUsed.remove(connectionName);
        }
    }
    
    // Create new connection if pool is not full
    QString connectionName = QString("%1_conn_%2_%3")
                           .arg(getConnectionName(type))
                           .arg(QThread::currentThreadId())
                           .arg(QDateTime::currentMSecsSinceEpoch());
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(m_databasePaths[type]);
    
    if (!db.open()) {
        qCCritical(dbManager) << "Failed to open database connection:" << db.lastError().text();
        QSqlDatabase::removeDatabase(connectionName);
        emit connectionError(type, db.lastError().text());
        return QSqlDatabase(); // Invalid database
    }
    
    m_connectionLastUsed[connectionName] = QDateTime::currentDateTime();
    return db;
}

void DatabaseManager::releaseConnection(const QString &connectionName) {
    QMutexLocker locker(&m_connectionMutex);
    
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isValid()) {
        return;
    }
    
    // Determine database type from connection name
    DatabaseType type = MainDatabase; // Default
    if (connectionName.contains("auth")) type = AuthDatabase;
    else if (connectionName.contains("audit")) type = AuditDatabase;
    else if (connectionName.contains("compliance")) type = ComplianceDatabase;
    else if (connectionName.contains("soc2")) type = SOC2Database;
    
    // Return connection to pool if not full
    if (m_availableConnections[type].size() < m_maxConnectionsPerType) {
        m_availableConnections[type].enqueue(connectionName);
        m_connectionLastUsed[connectionName] = QDateTime::currentDateTime();
    } else {
        // Pool is full, close the connection
        QSqlDatabase::removeDatabase(connectionName);
        m_connectionLastUsed.remove(connectionName);
    }
}

QSqlQuery DatabaseManager::executeQuery(const QString &query, DatabaseType type) {
    return executeQueryInternal(query, QVariantList(), type);
}

QSqlQuery DatabaseManager::executePreparedQuery(const QString &query, const QVariantList &params, DatabaseType type) {
    return executeQueryInternal(query, params, type);
}

QSqlQuery DatabaseManager::executeQueryInternal(const QString &query, const QVariantList &params, DatabaseType type) {
    QDateTime startTime = QDateTime::currentDateTime();
    
    // Check cache for SELECT queries if enabled
    if (m_queryCachingEnabled && query.trimmed().startsWith("SELECT", Qt::CaseInsensitive)) {
        QString cacheKey = QString("%1_%2").arg(static_cast<int>(type)).arg(query);
        if (m_queryCache.contains(cacheKey)) {
            qCDebug(dbManager) << "Query served from cache:" << query.left(50);
            // Note: For a full implementation, we'd need to return cached results
            // This is a simplified version focusing on the architecture
        }
    }
    
    QSqlDatabase db = getConnection(type);
    QSqlQuery sqlQuery(db);
    
    bool success;
    if (params.isEmpty()) {
        success = sqlQuery.exec(query);
    } else {
        sqlQuery.prepare(query);
        for (const QVariant &param : params) {
            sqlQuery.addBindValue(param);
        }
        success = sqlQuery.exec();
    }
    
    // Performance monitoring
    qint64 executionTime = startTime.msecsTo(QDateTime::currentDateTime());
    m_totalQueries++;
    m_totalExecutionTime += executionTime;
    m_queryExecutionTimes[type] = m_queryExecutionTimes.value(type, 0) + executionTime;
    
    if (!success) {
        qCWarning(dbManager) << "Query failed:" << query << sqlQuery.lastError().text();
    } else {
        qCDebug(dbManager) << "Query executed in" << executionTime << "ms:" << query.left(50);
    }
    
    releaseConnection(db.connectionName());
    return sqlQuery;
}

bool DatabaseManager::beginTransaction(DatabaseType type) {
    QSqlDatabase db = getConnection(type);
    bool success = db.transaction();
    if (!success) {
        qCWarning(dbManager) << "Failed to begin transaction:" << db.lastError().text();
        releaseConnection(db.connectionName());
    }
    return success;
}

bool DatabaseManager::commitTransaction(DatabaseType type) {
    QSqlDatabase db = getConnection(type);
    bool success = db.commit();
    if (!success) {
        qCWarning(dbManager) << "Failed to commit transaction:" << db.lastError().text();
    }
    releaseConnection(db.connectionName());
    return success;
}

bool DatabaseManager::rollbackTransaction(DatabaseType type) {
    QSqlDatabase db = getConnection(type);
    bool success = db.rollback();
    if (!success) {
        qCWarning(dbManager) << "Failed to rollback transaction:" << db.lastError().text();
    }
    releaseConnection(db.connectionName());
    return success;
}

void DatabaseManager::optimizeDatabase(DatabaseType type) {
    QSqlDatabase db = getConnection(type);
    QSqlQuery query(db);
    
    qCInfo(dbManager) << "Optimizing database type:" << type;
    
    // Analyze tables for better query planning
    query.exec("ANALYZE");
    
    // Vacuum to reclaim space and optimize
    query.exec("VACUUM");
    
    // Update statistics
    query.exec("PRAGMA optimize");
    
    releaseConnection(db.connectionName());
}

void DatabaseManager::analyzePerformance() {
    double avgExecutionTime = m_totalQueries > 0 ? 
                             static_cast<double>(m_totalExecutionTime) / m_totalQueries : 0.0;
    
    QString report = QString("Database Performance Report:\n"
                           "- Total queries: %1\n"
                           "- Average execution time: %2ms\n"
                           "- Total execution time: %3ms\n"
                           "- Uptime: %4 minutes\n"
                           "- Active connections: %5")
                    .arg(m_totalQueries)
                    .arg(avgExecutionTime, 0, 'f', 2)
                    .arg(m_totalExecutionTime)
                    .arg(m_startTime.secsTo(QDateTime::currentDateTime()) / 60)
                    .arg(getActiveConnectionCount());
    
    qCInfo(dbManager) << report;
    emit performanceReport(report);
}

QString DatabaseManager::getDatabasePath(DatabaseType type) {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    switch (type) {
    case MainDatabase:
        return QDir(dataDir).filePath("cyberrecon_main.db");
    case AuthDatabase:
        return QDir(dataDir).filePath("cyberrecon_auth.db");
    case AuditDatabase:
        return QDir(dataDir).filePath("cyberrecon_audit.db");
    case ComplianceDatabase:
        return QDir(dataDir).filePath("cyberrecon_compliance.db");
    case SOC2Database:
        return QDir(dataDir).filePath("cyberrecon_soc2.db");
    }
    
    return QDir(dataDir).filePath("cyberrecon_main.db");
}

QString DatabaseManager::getConnectionName(DatabaseType type) {
    switch (type) {
    case MainDatabase: return "main";
    case AuthDatabase: return "auth";
    case AuditDatabase: return "audit";
    case ComplianceDatabase: return "compliance";
    case SOC2Database: return "soc2";
    }
    return "main";
}

void DatabaseManager::setupConnectionPool(DatabaseType type, const QString &databasePath) {
    if (!m_availableConnections.contains(type)) {
        m_availableConnections[type] = QQueue<QString>();
    }
}

bool DatabaseManager::isConnectionHealthy(DatabaseType type) {
    QSqlDatabase db = getConnection(type);
    QSqlQuery query("SELECT 1", db);
    bool healthy = query.exec();
    releaseConnection(db.connectionName());
    return healthy;
}

int DatabaseManager::getActiveConnectionCount() {
    QMutexLocker locker(&m_connectionMutex);
    return m_connectionLastUsed.size();
}

void DatabaseManager::checkConnectionHealth() {
    for (auto type : {MainDatabase, AuthDatabase, AuditDatabase, ComplianceDatabase, SOC2Database}) {
        if (m_databasePaths.contains(type) && !isConnectionHealthy(type)) {
            qCWarning(dbManager) << "Unhealthy connection detected for database type:" << type;
            emit connectionError(type, "Connection health check failed");
        }
    }
}

void DatabaseManager::cleanupIdleConnections() {
    QMutexLocker locker(&m_connectionMutex);
    
    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-m_connectionTimeoutSeconds);
    QStringList connectionsToRemove;
    
    for (auto it = m_connectionLastUsed.begin(); it != m_connectionLastUsed.end(); ++it) {
        if (it.value() < cutoff) {
            connectionsToRemove.append(it.key());
        }
    }
    
    for (const QString &connectionName : connectionsToRemove) {
        QSqlDatabase::removeDatabase(connectionName);
        m_connectionLastUsed.remove(connectionName);
        qCDebug(dbManager) << "Removed idle connection:" << connectionName;
    }
    
    if (!connectionsToRemove.isEmpty()) {
        qCInfo(dbManager) << "Cleaned up" << connectionsToRemove.size() << "idle connections";
    }
}

// DatabaseTransaction implementation
DatabaseTransaction::DatabaseTransaction(DatabaseManager::DatabaseType type)
    : m_type(type), m_committed(false), m_rolledBack(false) {
    DatabaseManager::instance()->beginTransaction(m_type);
}

DatabaseTransaction::~DatabaseTransaction() {
    if (!m_committed && !m_rolledBack) {
        rollback();
    }
}

bool DatabaseTransaction::commit() {
    if (m_committed || m_rolledBack) {
        return false;
    }
    
    m_committed = DatabaseManager::instance()->commitTransaction(m_type);
    return m_committed;
}

bool DatabaseTransaction::rollback() {
    if (m_committed || m_rolledBack) {
        return false;
    }
    
    m_rolledBack = DatabaseManager::instance()->rollbackTransaction(m_type);
    return m_rolledBack;
}