#pragma once
#include <QObject>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QMutex>
#include <QQueue>
#include <QTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <functional>

Q_DECLARE_LOGGING_CATEGORY(asyncManager)

/**
 * @brief High-performance async task manager for long-running operations
 * 
 * This class provides:
 * - Thread pool management for CPU-intensive tasks
 * - Async operations with callbacks
 * - Task prioritization
 * - Progress monitoring
 * - Resource usage optimization
 * - Graceful cancellation
 */
class AsyncTaskManager : public QObject {
    Q_OBJECT

public:
    enum TaskPriority {
        LowPriority = 0,
        NormalPriority = 1,
        HighPriority = 2,
        CriticalPriority = 3
    };

    enum TaskType {
        DatabaseOperation,
        NetworkOperation,
        FileOperation,
        ComputationTask,
        UIUpdate
    };

    struct TaskInfo {
        QString taskId;
        TaskType type;
        TaskPriority priority;
        QString description;
        QDateTime startTime;
        QDateTime endTime;
        bool completed;
        bool cancelled;
        QString error;
        
        TaskInfo() : type(ComputationTask), priority(NormalPriority), 
                    completed(false), cancelled(false) {}
    };

    static AsyncTaskManager* instance();
    
    // Task scheduling
    QString scheduleTask(std::function<void()> task, TaskType type = ComputationTask, 
                        TaskPriority priority = NormalPriority, const QString &description = "");
    
    template<typename T>
    QString scheduleTaskWithResult(std::function<T()> task, std::function<void(T)> callback,
                                  TaskType type = ComputationTask, TaskPriority priority = NormalPriority,
                                  const QString &description = "");
    
    QString scheduleDelayedTask(std::function<void()> task, int delayMs, 
                               TaskType type = ComputationTask, const QString &description = "");
    
    // Task management
    bool cancelTask(const QString &taskId);
    bool isTaskRunning(const QString &taskId);
    TaskInfo getTaskInfo(const QString &taskId);
    QStringList getActiveTasks();
    
    // Performance optimization
    void setMaxThreadCount(int maxThreads);
    void setTaskTimeout(int timeoutMs);
    void optimizeForTaskType(TaskType type);
    
    // Resource monitoring
    int getActiveTaskCount();
    int getQueuedTaskCount();
    double getCpuUsagePercent();
    void generatePerformanceReport();

signals:
    void taskStarted(const QString &taskId, const QString &description);
    void taskCompleted(const QString &taskId);
    void taskFailed(const QString &taskId, const QString &error);
    void taskProgress(const QString &taskId, int percentage);
    void performanceAlert(const QString &message);

private slots:
    void processTaskQueue();
    void monitorTasks();
    void cleanupCompletedTasks();

private:
    explicit AsyncTaskManager(QObject *parent = nullptr);
    ~AsyncTaskManager();
    
    QString generateTaskId();
    void executeTask(const QString &taskId, std::function<void()> task);
    
    static AsyncTaskManager *m_instance;
    
    QThreadPool *m_threadPool;
    QMutex m_taskMutex;
    QHash<QString, TaskInfo> m_tasks;
    QQueue<QString> m_taskQueue;
    
    QTimer *m_queueProcessor;
    QTimer *m_taskMonitor;
    QTimer *m_cleanupTimer;
    
    int m_maxThreadCount;
    int m_taskTimeout;
    QDateTime m_startTime;
    
    // Performance metrics
    int m_totalTasksExecuted;
    qint64 m_totalExecutionTime;
    QHash<TaskType, qint64> m_taskTypeMetrics;
};

/**
 * @brief Runnable task wrapper for QThreadPool
 */
class AsyncTask : public QObject, public QRunnable {
    Q_OBJECT

public:
    AsyncTask(const QString &taskId, std::function<void()> task, AsyncTaskManager *manager);
    void run() override;

signals:
    void taskCompleted(const QString &taskId);
    void taskFailed(const QString &taskId, const QString &error);

private:
    QString m_taskId;
    std::function<void()> m_task;
    AsyncTaskManager *m_manager;
};

// Template implementation
template<typename T>
QString AsyncTaskManager::scheduleTaskWithResult(std::function<T()> task, std::function<void(T)> callback,
                                                TaskType type, TaskPriority priority, const QString &description) {
    QString taskId = generateTaskId();
    
    std::function<void()> wrappedTask = [task, callback, taskId, this]() {
        try {
            T result = task();
            // Schedule callback on main thread
            QTimer::singleShot(0, this, [callback, result]() {
                callback(result);
            });
        } catch (const std::exception &e) {
            emit taskFailed(taskId, QString::fromStdString(e.what()));
        }
    };
    
    return scheduleTask(wrappedTask, type, priority, description);
}

// Convenience macros
#define ASYNC_EXECUTE(task) AsyncTaskManager::instance()->scheduleTask(task)
#define ASYNC_EXECUTE_WITH_CALLBACK(task, callback) AsyncTaskManager::instance()->scheduleTaskWithResult(task, callback)
#define ASYNC_DELAYED(task, delay) AsyncTaskManager::instance()->scheduleDelayedTask(task, delay)