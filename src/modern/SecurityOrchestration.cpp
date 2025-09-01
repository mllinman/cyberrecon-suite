#include "SecurityOrchestration.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>

SecurityOrchestration::SecurityOrchestration(QWidget *parent) 
    : QWidget(parent), isExecuting(false), currentStep(0) {
    setupUI();
    loadPredefinedWorkflows();
    
    executionTimer = new QTimer(this);
    connect(executionTimer, &QTimer::timeout, this, &SecurityOrchestration::updateWorkflowStatus);
}

void SecurityOrchestration::setupUI() {
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("🔄 Security Orchestration & Automated Response");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Workflow selection
    auto *selectionLayout = new QHBoxLayout();
    selectionLayout->addWidget(new QLabel("Workflow:"));
    
    workflowCombo = new QComboBox();
    workflowCombo->setStyleSheet("padding: 8px; font-size: 12px; min-width: 200px;");
    selectionLayout->addWidget(workflowCombo);
    
    auto *executeBtn = new QPushButton("▶️ Execute");
    executeBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px 20px; border-radius: 6px; font-weight: bold; }");
    connect(executeBtn, &QPushButton::clicked, this, &SecurityOrchestration::executeWorkflow);
    selectionLayout->addWidget(executeBtn);
    
    auto *pauseBtn = new QPushButton("⏸️ Pause");
    pauseBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(pauseBtn, &QPushButton::clicked, this, &SecurityOrchestration::pauseWorkflow);
    selectionLayout->addWidget(pauseBtn);
    
    auto *stopBtn = new QPushButton("⏹️ Stop");
    stopBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(stopBtn, &QPushButton::clicked, this, &SecurityOrchestration::stopWorkflow);
    selectionLayout->addWidget(stopBtn);
    
    auto *createBtn = new QPushButton("➕ Create Workflow");
    createBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 10px 20px; border-radius: 6px; }");
    connect(createBtn, &QPushButton::clicked, this, &SecurityOrchestration::createCustomWorkflow);
    selectionLayout->addWidget(createBtn);
    
    selectionLayout->addStretch();
    layout->addLayout(selectionLayout);
    
    // Status and progress
    statusLabel = new QLabel("Status: Ready");
    statusLabel->setStyleSheet("font-weight: bold; color: #4caf50; margin: 5px;");
    layout->addWidget(statusLabel);
    
    executionProgress = new QProgressBar();
    executionProgress->setVisible(false);
    executionProgress->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: white;
            height: 25px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4caf50, stop:1 #8bc34a);
            border-radius: 3px;
        }
    )");
    layout->addWidget(executionProgress);
    
    // Main content area
    auto *contentLayout = new QHBoxLayout();
    
    // Left side - Workflow steps
    auto *leftWidget = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftWidget);
    
    auto *stepsLabel = new QLabel("📋 Workflow Steps");
    stepsLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    leftLayout->addWidget(stepsLabel);
    
    workflowSteps = new QTreeWidget();
    workflowSteps->setHeaderLabels({"Step", "Status", "Duration"});
    workflowSteps->setStyleSheet(R"(
        QTreeWidget {
            background-color: #2d2d2d;
            color: #fff;
            border: 1px solid #555;
            border-radius: 6px;
        }
        QTreeWidget::item {
            padding: 8px;
            border-bottom: 1px solid #444;
        }
        QTreeWidget::item:selected {
            background-color: #00bcd4;
        }
        QHeaderView::section {
            background-color: #404040;
            color: #00bcd4;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
    )");
    leftLayout->addWidget(workflowSteps);
    
    // Right side - Execution log
    auto *rightWidget = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightWidget);
    
    auto *logLabel = new QLabel("📝 Execution Log");
    logLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    rightLayout->addWidget(logLabel);
    
    executionLog = new QTextEdit();
    executionLog->setReadOnly(true);
    executionLog->setStyleSheet(R"(
        QTextEdit {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: 'Consolas', monospace;
            font-size: 11px;
            border: 1px solid #555;
            border-radius: 6px;
            padding: 10px;
        }
    )");
    rightLayout->addWidget(executionLog);
    
    contentLayout->addWidget(leftWidget, 1);
    contentLayout->addWidget(rightWidget, 1);
    layout->addLayout(contentLayout);
    
    // Initial log message
    executionLog->append("<font color='#00bcd4'>[SOAR] Security Orchestration system initialized</font>");
    executionLog->append("<font color='#4caf50'>[SOAR] Ready to execute automated response workflows</font>");
}

void SecurityOrchestration::loadPredefinedWorkflows() {
    workflowCombo->addItem("🚨 Incident Response - Malware Detection");
    workflowCombo->addItem("🔒 Incident Response - Data Breach");
    workflowCombo->addItem("🎣 Incident Response - Phishing Attack");
    workflowCombo->addItem("👤 Incident Response - Insider Threat");
    workflowCombo->addItem("🌐 Network Security - DDoS Mitigation");
    workflowCombo->addItem("🔐 Access Control - Suspicious Login");
    workflowCombo->addItem("📧 Email Security - Spam Campaign");
    workflowCombo->addItem("🛡️ Endpoint Security - Ransomware");
    
    connect(workflowCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this](int index) {
        workflowSteps->clear();
        
        QStringList steps;
        switch(index) {
            case 0: // Malware Detection
                steps = {
                    "Isolate affected endpoint",
                    "Collect memory dump",
                    "Block malicious file hash",
                    "Scan network for lateral movement",
                    "Update antivirus signatures",
                    "Notify security team",
                    "Generate incident report"
                };
                break;
            case 1: // Data Breach
                steps = {
                    "Identify data exposure scope",
                    "Contain the breach",
                    "Preserve forensic evidence",
                    "Assess legal requirements",
                    "Notify stakeholders",
                    "Implement additional controls",
                    "Conduct post-incident review"
                };
                break;
            case 2: // Phishing Attack
                steps = {
                    "Quarantine malicious emails",
                    "Extract IOCs from attachments",
                    "Check for user interactions",
                    "Reset affected credentials",
                    "Block sender domains",
                    "Send security awareness alert",
                    "Update email filters"
                };
                break;
            default:
                steps = {
                    "Initialize response",
                    "Gather intelligence",
                    "Execute containment",
                    "Perform analysis",
                    "Implement remediation",
                    "Document findings",
                    "Close incident"
                };
        }
        
        for (const QString &step : steps) {
            auto *item = new QTreeWidgetItem(workflowSteps);
            item->setText(0, step);
            item->setText(1, "⏳ Pending");
            item->setText(2, "0s");
        }
        
        currentWorkflowSteps = steps;
    });
    
    // Load first workflow by default
    workflowCombo->setCurrentIndex(0);
}

void SecurityOrchestration::executeWorkflow() {
    if (isExecuting) {
        QMessageBox::information(this, "Workflow Running", "A workflow is already executing.");
        return;
    }
    
    QString workflowName = workflowCombo->currentText();
    executionLog->append(QString("<font color='#4caf50'>[EXECUTE] Starting workflow: %1</font>").arg(workflowName));
    
    isExecuting = true;
    currentStep = 0;
    
    statusLabel->setText("Status: Executing");
    statusLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    
    executionProgress->setVisible(true);
    executionProgress->setRange(0, currentWorkflowSteps.size());
    executionProgress->setValue(0);
    
    executionTimer->start(2000); // Execute step every 2 seconds
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
    q.bindValue(":msg", QString("Automated workflow started: %1").arg(workflowName));
    q.exec();
}

void SecurityOrchestration::pauseWorkflow() {
    if (!isExecuting) return;
    
    executionTimer->stop();
    statusLabel->setText("Status: Paused");
    statusLabel->setStyleSheet("font-weight: bold; color: #ff9800; margin: 5px;");
    executionLog->append("<font color='#ff9800'>[PAUSE] Workflow execution paused</font>");
}

void SecurityOrchestration::stopWorkflow() {
    if (!isExecuting) return;
    
    executionTimer->stop();
    isExecuting = false;
    currentStep = 0;
    
    statusLabel->setText("Status: Stopped");
    statusLabel->setStyleSheet("font-weight: bold; color: #f44336; margin: 5px;");
    executionProgress->setVisible(false);
    
    // Reset all steps to pending
    for (int i = 0; i < workflowSteps->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = workflowSteps->topLevelItem(i);
        item->setText(1, "⏳ Pending");
        item->setText(2, "0s");
        item->setForeground(1, QColor("#ffeb3b"));
    }
    
    executionLog->append("<font color='#f44336'>[STOP] Workflow execution stopped</font>");
}

void SecurityOrchestration::createCustomWorkflow() {
    QMessageBox::information(this, "Custom Workflow", 
        "Custom workflow creation would open a visual workflow designer.\n\n"
        "Features would include:\n"
        "• Drag-and-drop workflow builder\n"
        "• Custom action library\n"
        "• Conditional logic support\n"
        "• Integration with external systems\n"
        "• Workflow testing and validation");
}

void SecurityOrchestration::updateWorkflowStatus() {
    if (!isExecuting || currentStep >= currentWorkflowSteps.size()) {
        // Workflow complete
        executionTimer->stop();
        isExecuting = false;
        
        statusLabel->setText("Status: Completed");
        statusLabel->setStyleSheet("font-weight: bold; color: #4caf50; margin: 5px;");
        executionProgress->setValue(executionProgress->maximum());
        
        executionLog->append("<font color='#4caf50'>[COMPLETE] Workflow execution finished successfully</font>");
        
        // Log completion
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
        q.bindValue(":msg", "Automated workflow completed successfully");
        q.exec();
        
        QTimer::singleShot(2000, this, [this]() {
            executionProgress->setVisible(false);
            statusLabel->setText("Status: Ready");
            statusLabel->setStyleSheet("font-weight: bold; color: #4caf50; margin: 5px;");
        });
        
        return;
    }
    
    // Execute current step
    QTreeWidgetItem *item = workflowSteps->topLevelItem(currentStep);
    if (item) {
        item->setText(1, "🔄 Executing");
        item->setForeground(1, QColor("#2196f3"));
        
        QString stepName = item->text(0);
        executionLog->append(QString("<font color='#2196f3'>[STEP %1] %2</font>")
                           .arg(currentStep + 1).arg(stepName));
        
        // Simulate step execution time
        QTimer::singleShot(1500, this, [this, item, stepName]() {
            item->setText(1, "✅ Completed");
            item->setText(2, QString("%1s").arg(1 + qrand() % 3));
            item->setForeground(1, QColor("#4caf50"));
            
            executionLog->append(QString("<font color='#4caf50'>[SUCCESS] %1 completed</font>").arg(stepName));
            
            currentStep++;
            executionProgress->setValue(currentStep);
        });
    }
}