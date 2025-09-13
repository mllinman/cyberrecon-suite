#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>
#include <QQueue>
#include <QTimer>
#include <QString>
#include <QHash>
#include <QThread>

/**
 * @brief Centralized database manager with connection pooling and optimization
 * 
 * This class provides:
 * - Connection pooling for improved performance
 * - Automatic connection health monitoring
 * - Transaction management
 * - Query optimization and caching
 * - Thread-safe database operations
 */
class DatabaseManager : public QObject {
    Q_OBJECT

public:
    enum DatabaseType {
        MainDatabase,
        AuthDatabase,
        AuditDatabase,
        ComplianceDatabase,
        SOC2Database
    };

    static DatabaseManager* instance();
    
    // Connection management
    QSqlDatabase getConnection(DatabaseType type = MainDatabase);
    void releaseConnection(const QString &connectionName);
    bool initializeDatabase(DatabaseType type, const QString &databasePath = "");
    
    // Optimized query execution
    QSqlQuery executeQuery(const QString &query, DatabaseType type = MainDatabase);
    QSqlQuery executePreparedQuery(const QString &query, const QVariantList &params, DatabaseType type = MainDatabase);
    
    // Transaction management
    bool beginTransaction(DatabaseType type = MainDatabase);
    bool commitTransaction(DatabaseType type = MainDatabase);
    bool rollbackTransaction(DatabaseType type = MainDatabase);
    
    // Performance optimization
    void enableQueryCaching(bool enabled = true);
    void optimizeDatabase(DatabaseType type = MainDatabase);
    void analyzePerformance();
    
    // Health monitoring
    bool isConnectionHealthy(DatabaseType type = MainDatabase);
    void refreshConnection(DatabaseType type = MainDatabase);
    int getActiveConnectionCount();
    
signals:
    void connectionError(DatabaseType type, const QString &error);
    void performanceReport(const QString &report);

private slots:
    void checkConnectionHealth();
    void cleanupIdleConnections();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    QString getDatabasePath(DatabaseType type);
    QString getConnectionName(DatabaseType type);
    void setupConnectionPool(DatabaseType type, const QString &databasePath);
    QSqlQuery executeQueryInternal(const QString &query, const QVariantList &params, DatabaseType type);
    
    static DatabaseManager *m_instance;
    
    // Connection pooling
    QHash<DatabaseType, QQueue<QString>> m_availableConnections;
    QHash<DatabaseType, QString> m_databasePaths;
    QHash<QString, QDateTime> m_connectionLastUsed;
    QMutex m_connectionMutex;
    
    // Configuration
    int m_maxConnectionsPerType;
    int m_connectionTimeoutSeconds;
    bool m_queryCachingEnabled;
    
    // Monitoring
    QTimer *m_healthCheckTimer;
    QTimer *m_cleanupTimer;
    QHash<DatabaseType, qint64> m_queryExecutionTimes;
    QHash<QString, QVariant> m_queryCache;
    
    // Performance metrics
    int m_totalQueries;
    qint64 m_totalExecutionTime;
    QDateTime m_startTime;
};

// Convenience macros for common database operations
#define DB_EXECUTE(query, type) DatabaseManager::instance()->executeQuery(query, type)
#define DB_PREPARE(query, params, type) DatabaseManager::instance()->executePreparedQuery(query, params, type)
#define DB_CONNECTION(type) DatabaseManager::instance()->getConnection(type)

// RAII transaction helper
class DatabaseTransaction {
public:
    explicit DatabaseTransaction(DatabaseManager::DatabaseType type = DatabaseManager::MainDatabase);
    ~DatabaseTransaction();
    
    bool commit();
    bool rollback();
    
private:
    DatabaseManager::DatabaseType m_type;
    bool m_committed;
    bool m_rolledBack;
};