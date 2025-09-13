#include "AsyncTaskManager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QRandomGenerator>
#include <QProcess>

Q_LOGGING_CATEGORY(asyncManager, "cyberrecon.async")

AsyncTaskManager* AsyncTaskManager::m_instance = nullptr;

AsyncTaskManager* AsyncTaskManager::instance() {
    static QMutex instanceMutex;
    QMutexLocker locker(&instanceMutex);
    
    if (!m_instance) {
        m_instance = new AsyncTaskManager();
    }
    return m_instance;
}

AsyncTaskManager::AsyncTaskManager(QObject *parent)
    : QObject(parent)
    , m_threadPool(QThreadPool::globalInstance())
    , m_maxThreadCount(QThread::idealThreadCount())
    , m_taskTimeout(30000) // 30 seconds default
    , m_totalTasksExecuted(0)
    , m_totalExecutionTime(0)
    , m_startTime(QDateTime::currentDateTime())
{
    // Configure thread pool for optimal performance
    m_threadPool->setMaxThreadCount(m_maxThreadCount);
    
    qCInfo(asyncManager) << "AsyncTaskManager initialized with" << m_maxThreadCount << "threads";
    
    // Initialize timers
    m_queueProcessor = new QTimer(this);
    connect(m_queueProcessor, &QTimer::timeout, this, &AsyncTaskManager::processTaskQueue);
    m_queueProcessor->start(100); // Process queue every 100ms
    
    m_taskMonitor = new QTimer(this);
    connect(m_taskMonitor, &QTimer::timeout, this, &AsyncTaskManager::monitorTasks);
    m_taskMonitor->start(1000); // Monitor every second
    
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &AsyncTaskManager::cleanupCompletedTasks);
    m_cleanupTimer->start(60000); // Cleanup every minute
}

AsyncTaskManager::~AsyncTaskManager() {
    m_threadPool->waitForDone(5000); // Wait up to 5 seconds for tasks to complete
    qCInfo(asyncManager) << "AsyncTaskManager destroyed";
}

QString AsyncTaskManager::scheduleTask(std::function<void()> task, TaskType type, 
                                      TaskPriority priority, const QString &description) {
    QString taskId = generateTaskId();
    
    TaskInfo taskInfo;
    taskInfo.taskId = taskId;
    taskInfo.type = type;
    taskInfo.priority = priority;
    taskInfo.description = description.isEmpty() ? QString("Task %1").arg(taskId) : description;
    
    {
        QMutexLocker locker(&m_taskMutex);
        m_tasks[taskId] = taskInfo;
        m_taskQueue.enqueue(taskId);
    }
    
    // Create and schedule async task
    AsyncTask *asyncTask = new AsyncTask(taskId, task, this);
    asyncTask->setAutoDelete(true);
    
    // Set priority for thread pool
    switch (priority) {
        case CriticalPriority:
            asyncTask->setPriority(QThread::TimeCriticalPriority);
            break;
        case HighPriority:
            asyncTask->setPriority(QThread::HighPriority);
            break;
        case NormalPriority:
            asyncTask->setPriority(QThread::NormalPriority);
            break;
        case LowPriority:
            asyncTask->setPriority(QThread::LowPriority);
            break;
    }
    
    m_threadPool->start(asyncTask);
    
    qCDebug(asyncManager) << "Scheduled task" << taskId << ":" << description;
    return taskId;
}

QString AsyncTaskManager::scheduleDelayedTask(std::function<void()> task, int delayMs, 
                                             TaskType type, const QString &description) {
    QString taskId = generateTaskId();
    
    QTimer::singleShot(delayMs, this, [this, task, type, taskId, description]() {
        scheduleTask(task, type, NormalPriority, 
                    QString("Delayed: %1").arg(description.isEmpty() ? taskId : description));
    });
    
    qCDebug(asyncManager) << "Scheduled delayed task" << taskId << "with" << delayMs << "ms delay";
    return taskId;
}

bool AsyncTaskManager::cancelTask(const QString &taskId) {
    QMutexLocker locker(&m_taskMutex);
    
    if (!m_tasks.contains(taskId)) {
        return false;
    }
    
    TaskInfo &info = m_tasks[taskId];
    if (info.completed) {
        return false; // Already completed
    }
    
    info.cancelled = true;
    info.endTime = QDateTime::currentDateTime();
    
    qCDebug(asyncManager) << "Cancelled task" << taskId;
    return true;
}

bool AsyncTaskManager::isTaskRunning(const QString &taskId) {
    QMutexLocker locker(&m_taskMutex);
    
    if (!m_tasks.contains(taskId)) {
        return false;
    }
    
    const TaskInfo &info = m_tasks[taskId];
    return !info.completed && !info.cancelled;
}

AsyncTaskManager::TaskInfo AsyncTaskManager::getTaskInfo(const QString &taskId) {
    QMutexLocker locker(&m_taskMutex);
    return m_tasks.value(taskId);
}

QStringList AsyncTaskManager::getActiveTasks() {
    QMutexLocker locker(&m_taskMutex);
    
    QStringList activeTasks;
    for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
        if (!it.value().completed && !it.value().cancelled) {
            activeTasks.append(it.key());
        }
    }
    
    return activeTasks;
}

void AsyncTaskManager::setMaxThreadCount(int maxThreads) {
    m_maxThreadCount = qMax(1, maxThreads);
    m_threadPool->setMaxThreadCount(m_maxThreadCount);
    qCInfo(asyncManager) << "Thread count set to" << m_maxThreadCount;
}

void AsyncTaskManager::setTaskTimeout(int timeoutMs) {
    m_taskTimeout = qMax(1000, timeoutMs); // Minimum 1 second
    qCInfo(asyncManager) << "Task timeout set to" << m_taskTimeout << "ms";
}

void AsyncTaskManager::optimizeForTaskType(TaskType type) {
    switch (type) {
        case DatabaseOperation:
            // Fewer threads for I/O bound operations
            setMaxThreadCount(qMin(4, QThread::idealThreadCount()));
            break;
        case NetworkOperation:
            // Moderate thread count for network operations
            setMaxThreadCount(QThread::idealThreadCount());
            break;
        case FileOperation:
            // Limited threads to avoid I/O contention
            setMaxThreadCount(qMin(2, QThread::idealThreadCount()));
            break;
        case ComputationTask:
            // Use all available cores for CPU-intensive tasks
            setMaxThreadCount(QThread::idealThreadCount() * 2);
            break;
        case UIUpdate:
            // Single thread for UI updates
            setMaxThreadCount(1);
            break;
    }
    
    qCInfo(asyncManager) << "Optimized for task type" << type << "with" << m_maxThreadCount << "threads";
}

int AsyncTaskManager::getActiveTaskCount() {
    return m_threadPool->activeThreadCount();
}

int AsyncTaskManager::getQueuedTaskCount() {
    QMutexLocker locker(&m_taskMutex);
    return m_taskQueue.size();
}

double AsyncTaskManager::getCpuUsagePercent() {
    // Simple CPU usage estimation based on active threads
    return (static_cast<double>(getActiveTaskCount()) / m_maxThreadCount) * 100.0;
}

void AsyncTaskManager::generatePerformanceReport() {
    QMutexLocker locker(&m_taskMutex);
    
    int completedTasks = 0;
    int failedTasks = 0;
    int cancelledTasks = 0;
    
    for (const TaskInfo &info : m_tasks.values()) {
        if (info.completed) completedTasks++;
        else if (info.cancelled) cancelledTasks++;
        else if (!info.error.isEmpty()) failedTasks++;
    }
    
    double avgExecutionTime = m_totalTasksExecuted > 0 ? 
                             static_cast<double>(m_totalExecutionTime) / m_totalTasksExecuted : 0.0;
    
    QString report = QString("Async Task Manager Performance Report:\n"
                           "- Active threads: %1/%2\n"
                           "- Queued tasks: %3\n"
                           "- Total tasks executed: %4\n"
                           "- Completed: %5, Failed: %6, Cancelled: %7\n"
                           "- Average execution time: %8ms\n"
                           "- CPU usage: %9%\n"
                           "- Uptime: %10 minutes")
                    .arg(getActiveTaskCount())
                    .arg(m_maxThreadCount)
                    .arg(getQueuedTaskCount())
                    .arg(m_totalTasksExecuted)
                    .arg(completedTasks)
                    .arg(failedTasks)
                    .arg(cancelledTasks)
                    .arg(avgExecutionTime, 0, 'f', 2)
                    .arg(getCpuUsagePercent(), 0, 'f', 1)
                    .arg(m_startTime.secsTo(QDateTime::currentDateTime()) / 60);
    
    qCInfo(asyncManager) << report;
}

QString AsyncTaskManager::generateTaskId() {
    return QString("task_%1_%2")
           .arg(QDateTime::currentMSecsSinceEpoch())
           .arg(QRandomGenerator::global()->bounded(1000, 9999));
}

void AsyncTaskManager::executeTask(const QString &taskId, std::function<void()> task) {
    QDateTime startTime = QDateTime::currentDateTime();
    
    {
        QMutexLocker locker(&m_taskMutex);
        if (m_tasks.contains(taskId)) {
            m_tasks[taskId].startTime = startTime;
        }
    }
    
    emit taskStarted(taskId, m_tasks[taskId].description);
    
    try {
        task();
        
        QDateTime endTime = QDateTime::currentDateTime();
        qint64 executionTime = startTime.msecsTo(endTime);
        
        {
            QMutexLocker locker(&m_taskMutex);
            if (m_tasks.contains(taskId)) {
                m_tasks[taskId].completed = true;
                m_tasks[taskId].endTime = endTime;
                
                m_totalTasksExecuted++;
                m_totalExecutionTime += executionTime;
                m_taskTypeMetrics[m_tasks[taskId].type] += executionTime;
            }
        }
        
        qCDebug(asyncManager) << "Task" << taskId << "completed in" << executionTime << "ms";
        emit taskCompleted(taskId);
        
    } catch (const std::exception &e) {
        QString error = QString::fromStdString(e.what());
        
        {
            QMutexLocker locker(&m_taskMutex);
            if (m_tasks.contains(taskId)) {
                m_tasks[taskId].error = error;
                m_tasks[taskId].endTime = QDateTime::currentDateTime();
            }
        }
        
        qCWarning(asyncManager) << "Task" << taskId << "failed:" << error;
        emit taskFailed(taskId, error);
    }
}

void AsyncTaskManager::processTaskQueue() {
    // This method is intentionally simple since QThreadPool handles most of the work
    // We could add additional queue management logic here if needed
}

void AsyncTaskManager::monitorTasks() {
    QMutexLocker locker(&m_taskMutex);
    
    QDateTime now = QDateTime::currentDateTime();
    
    for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
        TaskInfo &info = it.value();
        
        // Check for timeouts
        if (!info.completed && !info.cancelled && !info.startTime.isNull()) {
            qint64 runningTime = info.startTime.msecsTo(now);
            if (runningTime > m_taskTimeout) {
                info.cancelled = true;
                info.error = "Task timed out";
                info.endTime = now;
                
                qCWarning(asyncManager) << "Task" << info.taskId << "timed out after" << runningTime << "ms";
                emit taskFailed(info.taskId, "Task timed out");
            }
        }
    }
    
    // Check for performance issues
    double cpuUsage = getCpuUsagePercent();
    if (cpuUsage > 90.0) {
        emit performanceAlert(QString("High CPU usage: %1%").arg(cpuUsage, 0, 'f', 1));
    }
    
    if (getQueuedTaskCount() > 50) {
        emit performanceAlert(QString("High task queue: %1 tasks queued").arg(getQueuedTaskCount()));
    }
}

void AsyncTaskManager::cleanupCompletedTasks() {
    QMutexLocker locker(&m_taskMutex);
    
    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-300); // Keep completed tasks for 5 minutes
    QStringList tasksToRemove;
    
    for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
        const TaskInfo &info = it.value();
        if ((info.completed || info.cancelled) && info.endTime < cutoff) {
            tasksToRemove.append(it.key());
        }
    }
    
    for (const QString &taskId : tasksToRemove) {
        m_tasks.remove(taskId);
    }
    
    if (!tasksToRemove.isEmpty()) {
        qCDebug(asyncManager) << "Cleaned up" << tasksToRemove.size() << "completed tasks";
    }
}

// AsyncTask implementation
AsyncTask::AsyncTask(const QString &taskId, std::function<void()> task, AsyncTaskManager *manager)
    : m_taskId(taskId), m_task(task), m_manager(manager) {
}

void AsyncTask::run() {
    if (m_manager) {
        m_manager->executeTask(m_taskId, m_task);
    }
}