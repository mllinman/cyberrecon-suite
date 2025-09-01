#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>

class SecurityOrchestration : public QWidget {
    Q_OBJECT

public:
    explicit SecurityOrchestration(QWidget *parent = nullptr);

private slots:
    void executeWorkflow();
    void pauseWorkflow();
    void stopWorkflow();
    void createCustomWorkflow();
    void updateWorkflowStatus();

private:
    void setupUI();
    void loadPredefinedWorkflows();
    void simulateWorkflowExecution();
    
    QComboBox *workflowCombo;
    QTreeWidget *workflowSteps;
    QTextEdit *executionLog;
    QProgressBar *executionProgress;
    QLabel *statusLabel;
    QTimer *executionTimer;
    
    bool isExecuting;
    int currentStep;
    QStringList currentWorkflowSteps;
};