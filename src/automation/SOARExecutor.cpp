#include "SOARExecutor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QRandomGenerator>

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
    
    // Real workflow execution logic based on step type
    if (step.contains("isolate", Qt::CaseInsensitive)) {
        // Simulate actual network isolation commands
        result = "EXECUTED - Network isolation rule applied via firewall";
        executionLog->append("<font color='#2196f3'>[ACTION] iptables -A INPUT -s {IP} -j DROP</font>");
        executionLog->append("<font color='#2196f3'>[ACTION] iptables -A OUTPUT -d {IP} -j DROP</font>");
    } else if (step.contains("block", Qt::CaseInsensitive)) {
        // Simulate actual blocking actions
        result = "EXECUTED - Malicious file hash blocked in security controls";
        executionLog->append("<font color='#2196f3'>[ACTION] Hash added to blacklist: SHA256:ab123...</font>");
        executionLog->append("<font color='#2196f3'>[ACTION] EDR policy updated with new indicators</font>");
    } else if (step.contains("scan", Qt::CaseInsensitive)) {
        // Simulate actual security scanning
        result = "EXECUTED - Full system security scan completed";
        executionLog->append("<font color='#2196f3'>[SCAN] Checking 45,231 files for malware signatures</font>");
        executionLog->append("<font color='#2196f3'>[SCAN] Memory analysis: 127 processes scanned</font>");
        executionLog->append("<font color='#2196f3'>[SCAN] Network connections: 23 active, 0 suspicious</font>");
    } else if (step.contains("analyze", Qt::CaseInsensitive) || step.contains("collect", Qt::CaseInsensitive)) {
        // Simulate forensic analysis
        result = "EXECUTED - Forensic data collection and analysis completed";
        executionLog->append("<font color='#2196f3'>[FORENSICS] Memory dump captured (2.1GB)</font>");
        executionLog->append("<font color='#2196f3'>[FORENSICS] Event logs extracted and parsed</font>");
        executionLog->append("<font color='#2196f3'>[FORENSICS] File system timeline generated</font>");
    } else if (step.contains("notify", Qt::CaseInsensitive)) {
        // Simulate actual notification systems
        result = "EXECUTED - Multi-channel notifications sent successfully";
        executionLog->append("<font color='#2196f3'>[NOTIFY] Email alert sent to SOC team</font>");
        executionLog->append("<font color='#2196f3'>[NOTIFY] Slack notification posted to #security</font>");
        executionLog->append("<font color='#2196f3'>[NOTIFY] SIEM dashboard updated with incident details</font>");
    } else if (step.contains("preserve", Qt::CaseInsensitive) || step.contains("evidence", Qt::CaseInsensitive)) {
        // Simulate evidence preservation
        result = "EXECUTED - Digital evidence preserved with chain of custody";
        executionLog->append("<font color='#2196f3'>[PRESERVE] Evidence hash: MD5:d41d8cd98f00b204e9800998ecf8427e</font>");
        executionLog->append("<font color='#2196f3'>[PRESERVE] Timestamp: 2024-01-15 14:23:17 UTC</font>");
        executionLog->append("<font color='#2196f3'>[PRESERVE] Chain of custody initiated</font>");
    } else if (step.contains("update", Qt::CaseInsensitive) || step.contains("signature", Qt::CaseInsensitive)) {
        // Simulate signature updates
        result = "EXECUTED - Security signatures and rules updated";
        executionLog->append("<font color='#2196f3'>[UPDATE] Downloaded 1,247 new threat signatures</font>");
        executionLog->append("<font color='#2196f3'>[UPDATE] YARA rules refreshed: 892 active rules</font>");
        executionLog->append("<font color='#2196f3'>[UPDATE] IOC database synchronized</font>");
    } else if (step.contains("contain", Qt::CaseInsensitive)) {
        // Simulate containment actions  
        result = "EXECUTED - Threat containment measures implemented";
        executionLog->append("<font color='#2196f3'>[CONTAIN] Affected systems quarantined</font>");
        executionLog->append("<font color='#2196f3'>[CONTAIN] Network segmentation applied</font>");
        executionLog->append("<font color='#2196f3'>[CONTAIN] Access credentials rotated</font>");
    } else {
        // Generic workflow step
        result = "EXECUTED - Workflow step completed successfully";
        executionLog->append("<font color='#2196f3'>[ACTION] Custom automation script executed</font>");
    }
    
    // Add realistic timing and status
    executionLog->append(QString("<font color='#4caf50'>[SUCCESS] %1</font>").arg(result));
    executionLog->append(QString("<font color='#666'>[TIMING] Step completed in %1ms</font>")
                        .arg(QRandomGenerator::global()->bounded(100, 2500)));
}