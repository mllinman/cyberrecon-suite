#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>

class SOARExecutor : public QWidget {
    Q_OBJECT
public:
    explicit SOARExecutor(QWidget *parent = nullptr);

private slots:
    void executeWorkflow();
    void pauseExecution();
    void stopExecution();
    void updateProgress();

private:
    void loadWorkflows();
    void simulateStepExecution();
    
    QComboBox *workflowCombo;
    QTextEdit *executionLog;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QTimer *executionTimer;
    QPushButton *executeBtn;
    QPushButton *pauseBtn;
    QPushButton *stopBtn;
    
    bool isExecuting;
    int currentStep;
    int totalSteps;
    QStringList currentWorkflow;
};