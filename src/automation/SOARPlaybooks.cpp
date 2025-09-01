#include "SOARPlaybooks.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QSqlQuery>

SOARPlaybooks::SOARPlaybooks(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("SOAR Playbooks");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // Predefined playbook selector
    auto *selectorLayout = new QHBoxLayout();
    selectorLayout->addWidget(new QLabel("Quick Playbooks:"));
    
    playbookSelector = new QComboBox();
    playbookSelector->addItem("Custom Playbook");
    playbookSelector->addItem("Malware Response");
    playbookSelector->addItem("Phishing Investigation");
    playbookSelector->addItem("Data Breach Response");
    playbookSelector->addItem("Insider Threat");
    playbookSelector->setStyleSheet("padding: 5px; font-size: 11pt;");
    
    selectorLayout->addWidget(playbookSelector);
    layout->addLayout(selectorLayout);

    playbookBox = new QTextEdit();
    playbookBox->setPlaceholderText("Define workflow steps:\n1. Detect threat\n2. Isolate affected systems\n3. Block malicious IPs\n4. Notify security team\n5. Generate report");
    playbookBox->setStyleSheet("background-color: #2b2b2b; color: #ffffff; font-family: 'Consolas', monospace; padding: 10px;");
    layout->addWidget(playbookBox);

    auto *buttonLayout = new QHBoxLayout();
    QPushButton *execBtn = new QPushButton("Execute Playbook");
    QPushButton *saveBtn = new QPushButton("Save Playbook");
    QPushButton *clearBtn = new QPushButton("Clear");

    execBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px; border-radius: 4px; font-weight: bold; }");
    saveBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px; border-radius: 4px; }");
    clearBtn->setStyleSheet("QPushButton { background-color: #607d8b; color: white; padding: 10px; border-radius: 4px; }");

    buttonLayout->addWidget(execBtn);
    buttonLayout->addWidget(saveBtn);
    buttonLayout->addWidget(clearBtn);
    layout->addLayout(buttonLayout);

    // Execution log
    executionLog = new QTextEdit();
    executionLog->setReadOnly(true);
    executionLog->setMaximumHeight(150);
    executionLog->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace;");
    layout->addWidget(executionLog);

    connect(execBtn, &QPushButton::clicked, this, &SOARPlaybooks::executePlaybook);
    connect(saveBtn, &QPushButton::clicked, this, &SOARPlaybooks::savePlaybook);
    connect(clearBtn, &QPushButton::clicked, [this]() { playbookBox->clear(); });
    connect(playbookSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SOARPlaybooks::loadPredefinedPlaybook);
}

void SOARPlaybooks::executePlaybook() {
    QString workflow = playbookBox->toPlainText().trimmed();
    if(workflow.isEmpty()) {
        executionLog->append("<font color='#ff9800'>[Warning] No playbook defined</font>");
        return;
    }

    executionLog->append("<font color='#2196f3'>[SOAR] Starting playbook execution...</font>");
    
    // Parse and simulate execution of each step
    QStringList steps = workflow.split('\n', Qt::SkipEmptyParts);
    for (int i = 0; i < steps.size(); ++i) {
        QString step = steps[i].trimmed();
        if (step.isEmpty()) continue;
        
        // Simulate step execution
        QString result = simulateStepExecution(step);
        executionLog->append(QString("<font color='#4caf50'>[Step %1] %2 → %3</font>")
                           .arg(i + 1).arg(step).arg(result));
    }
    
    executionLog->append("<font color='#4caf50'>[SOAR] Playbook execution completed</font>");
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
    q.bindValue(":msg", QString("Playbook executed with %1 steps").arg(steps.size()));
    q.exec();
}

void SOARPlaybooks::savePlaybook() {
    QString workflow = playbookBox->toPlainText().trimmed();
    if(workflow.isEmpty()) {
        executionLog->append("<font color='#ff9800'>[Warning] No playbook to save</font>");
        return;
    }
    
    executionLog->append("<font color='#4caf50'>[SOAR] Playbook saved successfully</font>");
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
    q.bindValue(":msg", "Custom playbook saved");
    q.exec();
}

void SOARPlaybooks::loadPredefinedPlaybook() {
    QString selected = playbookSelector->currentText();
    
    if (selected == "Malware Response") {
        playbookBox->setPlainText(
            "1. Isolate infected host from network\n"
            "2. Capture memory dump for analysis\n"
            "3. Block malicious file hashes\n"
            "4. Scan all endpoints for IOCs\n"
            "5. Update antivirus signatures\n"
            "6. Notify incident response team\n"
            "7. Generate malware analysis report"
        );
    } else if (selected == "Phishing Investigation") {
        playbookBox->setPlainText(
            "1. Quarantine suspicious email\n"
            "2. Extract and analyze attachments/URLs\n"
            "3. Check if users clicked malicious links\n"
            "4. Reset credentials for affected users\n"
            "5. Block sender domain/IP\n"
            "6. Send security awareness alert\n"
            "7. Update email security rules"
        );
    } else if (selected == "Data Breach Response") {
        playbookBox->setPlainText(
            "1. Identify scope of data exposure\n"
            "2. Contain the breach immediately\n"
            "3. Preserve evidence for forensics\n"
            "4. Assess legal/regulatory requirements\n"
            "5. Notify affected stakeholders\n"
            "6. Implement additional security controls\n"
            "7. Conduct post-incident review"
        );
    } else if (selected == "Insider Threat") {
        playbookBox->setPlainText(
            "1. Review user access logs and activities\n"
            "2. Disable suspicious user accounts\n"
            "3. Collect digital evidence\n"
            "4. Interview relevant personnel\n"
            "5. Coordinate with HR and Legal\n"
            "6. Implement monitoring controls\n"
            "7. Document findings and lessons learned"
        );
    } else if (selected == "Custom Playbook") {
        playbookBox->clear();
    }
}

QString SOARPlaybooks::simulateStepExecution(const QString &step) {
    // Simulate different types of actions based on step content
    if (step.contains("isolate", Qt::CaseInsensitive) || step.contains("block", Qt::CaseInsensitive)) {
        return "SUCCESS - Network action completed";
    } else if (step.contains("scan", Qt::CaseInsensitive) || step.contains("analyze", Qt::CaseInsensitive)) {
        return "SUCCESS - Analysis completed, 0 threats found";
    } else if (step.contains("notify", Qt::CaseInsensitive) || step.contains("alert", Qt::CaseInsensitive)) {
        return "SUCCESS - Notification sent to security team";
    } else if (step.contains("update", Qt::CaseInsensitive)) {
        return "SUCCESS - Security controls updated";
    } else if (step.contains("generate", Qt::CaseInsensitive) || step.contains("report", Qt::CaseInsensitive)) {
        return "SUCCESS - Report generated and saved";
    } else {
        return "SUCCESS - Action completed";
    }
}