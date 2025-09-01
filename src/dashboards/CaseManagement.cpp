#include "CaseManagement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlError>
#include <QTimer>

CaseManagement::CaseManagement(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Case Management System");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // SQLite DB setup
    db = QSqlDatabase::addDatabase("QSQLITE", "cases");
    db.setDatabaseName("cyberrecon_cases.db");
    if (!db.open()) {
        QMessageBox::critical(this, "DB Error", db.lastError().text());
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS cases (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, status TEXT, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)");
    query.exec("CREATE TABLE IF NOT EXISTS case_logs (id INTEGER PRIMARY KEY AUTOINCREMENT, case_id INTEGER, action TEXT, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // List of cases
    caseList = new QListWidget();
    caseList->setStyleSheet("background-color: #2b2b2b; color: white; font-family: 'Consolas', monospace;");
    layout->addWidget(caseList);

    // Add case entry
    caseEntry = new QLineEdit();
    caseEntry->setPlaceholderText("Enter case name...");
    caseEntry->setStyleSheet("padding: 8px; font-size: 12pt;");
    layout->addWidget(caseEntry);

    auto *buttonLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("Open New Case");
    QPushButton *closeBtn = new QPushButton("Close Selected Case");
    QPushButton *evidenceBtn = new QPushButton("View Evidence");

    addBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px; border-radius: 4px; }");
    closeBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    evidenceBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 8px; border-radius: 4px; }");

    buttonLayout->addWidget(addBtn);
    buttonLayout->addWidget(closeBtn);
    buttonLayout->addWidget(evidenceBtn);
    layout->addLayout(buttonLayout);

    connect(addBtn, &QPushButton::clicked, this, &CaseManagement::addCase);
    connect(closeBtn, &QPushButton::clicked, this, &CaseManagement::closeCase);
    connect(evidenceBtn, &QPushButton::clicked, this, &CaseManagement::viewEvidence);
    connect(caseEntry, &QLineEdit::returnPressed, this, &CaseManagement::addCase);

    refreshCases();
}

void CaseManagement::addCase() {
    QString caseName = caseEntry->text().trimmed();
    if (caseName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a case name.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO cases (name, status) VALUES (:name, 'Open')");
    query.bindValue(":name", caseName);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    // Log case creation
    logAction(query.lastInsertId().toInt(), QString("Case '%1' created").arg(caseName));

    // Log to main events table
    QSqlQuery ev;
    ev.prepare("INSERT INTO events (panel, severity, message) VALUES ('CaseMgmt', 'Info', :msg)");
    ev.bindValue(":msg", "Case '" + caseName + "' opened");
    ev.exec();

    caseEntry->clear();
    refreshCases();
}

void CaseManagement::closeCase() {
    QListWidgetItem *item = caseList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a case to close.");
        return;
    }

    QString caseText = item->text();
    QString caseName = caseText.split(" - ").first();

    QSqlQuery query(db);
    query.prepare("UPDATE cases SET status='Closed' WHERE name=:name");
    query.bindValue(":name", caseName);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    // Get case ID for logging
    QSqlQuery idQuery(db);
    idQuery.prepare("SELECT id FROM cases WHERE name=:name");
    idQuery.bindValue(":name", caseName);
    if (idQuery.exec() && idQuery.next()) {
        logAction(idQuery.value(0).toInt(), QString("Case '%1' closed").arg(caseName));
    }

    // Log to main events table
    QSqlQuery ev;
    ev.prepare("INSERT INTO events (panel, severity, message) VALUES ('CaseMgmt', 'Info', :msg)");
    ev.bindValue(":msg", "Case '" + caseName + "' closed");
    ev.exec();

    refreshCases();
}

void CaseManagement::viewEvidence() {
    QListWidgetItem *item = caseList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a case to view evidence.");
        return;
    }

    QString caseText = item->text();
    QString caseName = caseText.split(" - ").first();

    QSqlQuery query(db);
    query.prepare("SELECT id FROM cases WHERE name=:n");
    query.bindValue(":n", caseName);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Error", "Could not find case in database.");
        return;
    }
    
    int caseId = query.value(0).toInt();

    QSqlQuery logQuery(db);
    logQuery.prepare("SELECT action, timestamp FROM case_logs WHERE case_id=:cid ORDER BY timestamp DESC");
    logQuery.bindValue(":cid", caseId);
    
    QString evidence = QString("Evidence for case: %1\n\n").arg(caseName);
    
    if (logQuery.exec()) {
        while(logQuery.next()) {
            evidence += QString("%1 - %2\n")
                       .arg(logQuery.value(1).toString())
                       .arg(logQuery.value(0).toString());
        }
    }
    
    if (evidence.split('\n').count() <= 3) {
        evidence += "No evidence logged for this case yet.\n";
    }
    
    QMessageBox::information(this, "Case Evidence", evidence);
}

void CaseManagement::refreshCases() {
    caseList->clear();
    QSqlQuery query("SELECT name, status, created_at FROM cases ORDER BY created_at DESC", db);
    while (query.next()) {
        QString name = query.value(0).toString();
        QString status = query.value(1).toString();
        QString created = query.value(2).toString();
        
        QString statusIcon = (status == "Open") ? "🟢" : "🔴";
        caseList->addItem(QString("%1 - %2 %3 (Created: %4)")
                         .arg(name).arg(statusIcon).arg(status).arg(created));
    }
}

void CaseManagement::logAction(int caseId, const QString &action) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO case_logs (case_id, action) VALUES (:cid, :act)");
    query.bindValue(":cid", caseId);
    query.bindValue(":act", action);
    query.exec();
}