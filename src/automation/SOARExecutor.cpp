#include "SOARExecutor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

SOARExecutor::SOARExecutor(QWidget *parent) 
    : QWidget(parent), isExecuting(false), currentStep(0), totalSteps(0) {
    
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("SOAR Workflow Executor");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // Workflow selection
    auto *selectionLayout = new QHBoxLayout();
    selectionLayout->addWidget(new QLabel("Workflow:"));
    
    workflowCombo = new QComboBox();
    workflowCombo->setStyleSheet("padding: 8px; font-size: 12pt;");
    selectionLayout->addWidget(workflowCombo);
    selectionLayout->addStretch();
    layout->addLayout(selectionLayout);

    // Control buttons
    auto *buttonLayout = new QHBoxLayout();
    executeBtn = new QPushButton("Execute Workflow");
    pauseBtn = new QPushButton("Pause");
    stopBtn = new QPushButton("Stop");

    executeBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px; border-radius: 4px; }");
    pauseBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px; border-radius: 4px; }");
    stopBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px; border-radius: 4px; }");

    pauseBtn->setEnabled(false);
    stopBtn->setEnabled(false);

    buttonLayout->addWidget(executeBtn);
    buttonLayout->addWidget(pauseBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    // Status and progress
    statusLabel = new QLabel("Status: Ready");
    statusLabel->setStyleSheet("font-weight: bold; color: #4caf50; margin: 5px;");
    layout->addWidget(statusLabel);

    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #555; border-radius: 3px; } QProgressBar::chunk { background-color: #2196f3; }");
    layout->addWidget(progressBar);

    // Execution log
    executionLog = new QTextEdit();
    executionLog->setReadOnly(true);
    executionLog->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace;");
    layout->addWidget(executionLog);

    // Connect signals
    connect(executeBtn, &QPushButton::clicked, this, &SOARExecutor::executeWorkflow);
    connect(pauseBtn, &QPushButton::clicked, this, &SOARExecutor::pauseExecution);
    connect(stopBtn, &QPushButton::clicked, this, &SOARExecutor::stopExecution);

    // Execution timer
    executionTimer = new QTimer(this);
    connect(executionTimer, &QTimer::timeout, this, &SOARExecutor::updateProgress);

    loadWorkflows();
}

void SOARExecutor::loadWorkflows() {
    workflowCombo->addItem("Incident Response - Malware Detection");
    workflowCombo->addItem("Incident Response - Data Breach");
    workflowCombo->addItem("Network Security - DDoS Mitigation");
    workflowCombo->addItem("Email Security - Phishing Response");
    workflowCombo->addItem("Endpoint Security - Ransomware");
}

void SOARExecutor::executeWorkflow() {
    if (isExecuting) return;

    QString workflow = workflowCombo->currentText();
    executionLog->append(QString("<font color='#2196f3'>[EXECUTOR] Starting workflow: %1</font>").arg(workflow));

    // Define workflow steps
    if (workflow.contains("Malware")) {
        currentWorkflow = {
            "Isolate affected endpoint",
            "Collect memory dump",
            "Block malicious file hash",
            "Scan for lateral movement",
            "Update signatures",
            "Notify security team"
        };
    } else if (workflow.contains("Breach")) {
        currentWorkflow = {
            "Identify data exposure scope",
            "Contain the breach",
            "Preserve evidence",
            "Assess legal requirements",
            "Notify stakeholders",
            "Generate report"
        };
    } else {
        currentWorkflow = {
            "Initialize response",
            "Gather intelligence",
            "Execute containment",
            "Perform analysis",
            "Implement remediation",
            "Document findings"
        };
    }

    isExecuting = true;
    currentStep = 0;
    totalSteps = currentWorkflow.size();

    executeBtn->setEnabled(false);
    pauseBtn->setEnabled(true);
    stopBtn->setEnabled(true);

    statusLabel->setText("Status: Executing");
    statusLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");

    progressBar->setVisible(true);
    progressBar->setRange(0, totalSteps);
    progressBar->setValue(0);

    executionTimer->start(2000); // Execute step every 2 seconds

    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOARExecutor', 'Info', :msg)");
    q.bindValue(":msg", QString("Workflow execution started: %1").arg(workflow));
    q.exec();
}

void SOARExecutor::pauseExecution() {
    if (!isExecuting) return;

    executionTimer->stop();
    statusLabel->setText("Status: Paused");
    statusLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    executionLog->append("<font color='#ff9800'>[EXECUTOR] Workflow execution paused</font>");
}

void SOARExecutor::stopExecution() {
    if (!isExecuting) return;

    isExecuting = false;
    executionTimer->stop();
    currentStep = 0;

    executeBtn->setEnabled(true);
    pauseBtn->setEnabled(false);
    stopBtn->setEnabled(false);

    statusLabel->setText("Status: Stopped");
    statusLabel->setStyleSheet("font-weight: bold; color: #f44336; margin: 5px;");
    progressBar->setVisible(false);

    executionLog->append("<font color='#f44336'>[EXECUTOR] Workflow execution stopped</font>");
}

void SOARExecutor::updateProgress() {
    if (!isExecuting || currentStep >= totalSteps) {
        // Workflow complete
        isExecuting = false;
        executionTimer->stop();

        executeBtn->setEnabled(true);
        pauseBtn->setEnabled(false);
        stopBtn->setEnabled(false);

        statusLabel->setText("Status: Completed");
        statusLabel->setStyleSheet("font-weight: bold; color: #4caf50; margin: 5px;");
        progressBar->setValue(totalSteps);

        executionLog->append("<font color='#4caf50'>[EXECUTOR] Workflow execution completed successfully</font>");

        // Log completion
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOARExecutor', 'Info', :msg)");
        q.bindValue(":msg", "Workflow execution completed");
        q.exec();

        return;
    }

    // Execute current step
    QString step = currentWorkflow[currentStep];
    executionLog->append(QString("<font color='#2196f3'>[STEP %1] %2</font>").arg(currentStep + 1).arg(step));

    simulateStepExecution();

    currentStep++;
    progressBar->setValue(currentStep);
}

void SOARExecutor::simulateStepExecution() {
    QString step = currentWorkflow[currentStep];
    QString result;

    if (step.contains("isolate", Qt::CaseInsensitive) || step.contains("block", Qt::CaseInsensitive)) {
        result = "SUCCESS - Network action completed";
    } else if (step.contains("scan", Qt::CaseInsensitive) || step.contains("analyze", Qt::CaseInsensitive)) {
        result = "SUCCESS - Analysis completed, 0 threats found";
    } else if (step.contains("notify", Qt::CaseInsensitive)) {
        result = "SUCCESS - Notification sent to security team";
    } else {
        result = "SUCCESS - Action completed";
    }

    executionLog->append(QString("<font color='#4caf50'>[RESULT] %1</font>").arg(result));
}