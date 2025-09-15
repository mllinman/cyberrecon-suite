#include "SOARRules.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>
#include <QLabel>

SOARRules::SOARRules(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("SOAR Automation Rules");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    tree = new QTreeWidget();
    tree->setHeaderLabels({"Panel", "Severity", "Actions", "Status"});
    tree->header()->setStretchLastSection(true);
    tree->setStyleSheet("background-color: #2b2b2b; color: white; font-family: 'Consolas', monospace;");
    layout->addWidget(tree);

    // Input fields
    auto *inputLayout = new QVBoxLayout();
    
    auto *panelLayout = new QHBoxLayout();
    panelLayout->addWidget(new QLabel("Panel:"));
    panelEntry = new QLineEdit();
    panelEntry->setPlaceholderText("e.g., SIEM, EDR, IntelHub");
    panelLayout->addWidget(panelEntry);
    inputLayout->addLayout(panelLayout);

    auto *sevLayout = new QHBoxLayout();
    sevLayout->addWidget(new QLabel("Severity:"));
    sevEntry = new QLineEdit();
    sevEntry->setPlaceholderText("Critical, Warning, Info");
    sevLayout->addWidget(sevEntry);
    inputLayout->addLayout(sevLayout);

    auto *actLayout = new QHBoxLayout();
    actLayout->addWidget(new QLabel("Actions:"));
    actEntry = new QLineEdit();
    actEntry->setPlaceholderText("Isolate Host, Block IP, Send Alert");
    actLayout->addWidget(actEntry);
    inputLayout->addLayout(actLayout);

    layout->addLayout(inputLayout);

    auto *buttonLayout = new QHBoxLayout();
    auto *addBtn = new QPushButton("Add/Update Rule");
    auto *delBtn = new QPushButton("Delete Rule");
    auto *testBtn = new QPushButton("Test Rules");

    addBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px; border-radius: 4px; }");
    delBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    testBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 8px; border-radius: 4px; }");

    buttonLayout->addWidget(addBtn);
    buttonLayout->addWidget(delBtn);
    buttonLayout->addWidget(testBtn);
    layout->addLayout(buttonLayout);

    connect(addBtn, &QPushButton::clicked, this, &SOARRules::addRule);
    connect(delBtn, &QPushButton::clicked, this, &SOARRules::deleteRule);
    connect(testBtn, &QPushButton::clicked, this, &SOARRules::testRules);

    // Initialize database
    db = QSqlDatabase::addDatabase("QSQLITE", "soarrules");
    db.setDatabaseName("soar_rules.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Could not open SOAR rules database");
        return;
    }

    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS rules (id INTEGER PRIMARY KEY AUTOINCREMENT, panel TEXT, severity TEXT, actions TEXT, enabled INTEGER DEFAULT 1)");

    // Add some default rules if table is empty
    QSqlQuery checkEmpty("SELECT COUNT(*) FROM rules", db);
    if (checkEmpty.exec() && checkEmpty.next() && checkEmpty.value(0).toInt() == 0) {
        addDefaultRules();
    }

    refreshRules();
}

void SOARRules::addRule() {
    QString panel = panelEntry->text().trimmed();
    QString severity = sevEntry->text().trimmed();
    QString actions = actEntry->text().trimmed();

    if (panel.isEmpty() || severity.isEmpty() || actions.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please fill in all fields.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("INSERT OR REPLACE INTO rules (panel, severity, actions) VALUES (:p, :s, :a)");
    q.bindValue(":p", panel);
    q.bindValue(":s", severity);
    q.bindValue(":a", actions);
    
    if (!q.exec()) {
        QMessageBox::warning(this, "Error", "Database insert failed: " + q.lastError().text());
        return;
    }

    // Clear input fields
    panelEntry->clear();
    sevEntry->clear();
    actEntry->clear();

    refreshRules();
    
    // Log the rule creation
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("New automation rule added: %1 %2 → %3").arg(panel).arg(severity).arg(actions));
    logQuery.exec();
}

void SOARRules::deleteRule() {
    auto item = tree->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a rule to delete.");
        return;
    }

    QString panel = item->text(0);
    QString severity = item->text(1);
    
    QSqlQuery q(db);
    q.prepare("DELETE FROM rules WHERE panel=:p AND severity=:s");
    q.bindValue(":p", panel);
    q.bindValue(":s", severity);
    
    if (!q.exec()) {
        QMessageBox::warning(this, "Error", "Database delete failed: " + q.lastError().text());
        return;
    }

    refreshRules();
    
    // Log the rule deletion
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("Automation rule deleted: %1 %2").arg(panel).arg(severity));
    logQuery.exec();
}

void SOARRules::testRules() {
    QSqlQuery q("SELECT panel, severity, actions FROM rules WHERE enabled=1", db);
    int ruleCount = 0;
    
    while (q.next()) {
        ruleCount++;
        QString panel = q.value(0).toString();
        QString severity = q.value(1).toString();
        QString actions = q.value(2).toString();
        
        // Log test execution
        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('SOAR', 'Info', :msg)");
        logQuery.bindValue(":msg", QString("Rule test: %1 %2 → %3 [SIMULATED]").arg(panel).arg(severity).arg(actions));
        logQuery.exec();
    }
    
    QMessageBox::information(this, "Rule Test Complete", 
                           QString("Tested %1 automation rules. Check the timeline for results.").arg(ruleCount));
}

void SOARRules::refreshRules() {
    tree->clear();
    QSqlQuery q("SELECT panel, severity, actions, enabled FROM rules ORDER BY panel, severity", db);
    
    while (q.next()) {
        QString panel = q.value(0).toString();
        QString severity = q.value(1).toString();
        QString actions = q.value(2).toString();
        bool enabled = q.value(3).toBool();
        
        QString status = enabled ? "🟢 Enabled" : "🔴 Disabled";
        auto *item = new QTreeWidgetItem({panel, severity, actions, status});
        
        // Color code by severity
        if (severity == "Critical") {
            item->setForeground(0, QColor("#f44336"));
        } else if (severity == "Warning") {
            item->setForeground(0, QColor("#ff9800"));
        } else {
            item->setForeground(0, QColor("#4caf50"));
        }
        
        tree->addTopLevelItem(item);
    }
}

void SOARRules::addDefaultRules() {
    QStringList defaultRules = {
        "SIEM,Critical,Isolate Host;Send Alert",
        "EDR,Critical,Block Process;Quarantine File;Send Alert",
        "IntelHub,Critical,Block IP;Update Firewall;Send Alert",
        "SIEM,Warning,Log Event;Send Notification",
        "EDR,Warning,Monitor Process;Log Event"
    };
    
    for (const QString &rule : defaultRules) {
        QStringList parts = rule.split(',');
        if (parts.size() == 3) {
            QSqlQuery q(db);
            q.prepare("INSERT INTO rules (panel, severity, actions) VALUES (:p, :s, :a)");
            q.bindValue(":p", parts[0]);
            q.bindValue(":s", parts[1]);
            q.bindValue(":a", parts[2]);
            q.exec();
        }
    }
}