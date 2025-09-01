#include "AdminPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QMessageBox>
#include <QSqlQuery>

AdminPanel::AdminPanel(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Admin Panel - System Management");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // User Management Group
    QGroupBox *userGroup = new QGroupBox("User Management");
    userGroup->setStyleSheet("QGroupBox { font-weight: bold; margin: 10px; }");
    auto *userLayout = new QVBoxLayout(userGroup);

    userList = new QListWidget();
    userList->setStyleSheet("background-color: #2b2b2b; color: white; font-family: 'Consolas', monospace;");
    userLayout->addWidget(userList);

    auto *userInputLayout = new QHBoxLayout();
    userEntry = new QLineEdit();
    userEntry->setPlaceholderText("Enter username...");
    userEntry->setStyleSheet("padding: 5px;");
    
    roleEntry = new QComboBox();
    roleEntry->addItems({"admin", "analyst", "viewer", "guest"});
    roleEntry->setStyleSheet("padding: 5px;");
    
    userInputLayout->addWidget(new QLabel("User:"));
    userInputLayout->addWidget(userEntry);
    userInputLayout->addWidget(new QLabel("Role:"));
    userInputLayout->addWidget(roleEntry);
    userLayout->addLayout(userInputLayout);

    auto *userButtonLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("Add User");
    QPushButton *removeBtn = new QPushButton("Remove User");
    QPushButton *updateBtn = new QPushButton("Update Role");

    addBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 6px; border-radius: 3px; }");
    removeBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 6px; border-radius: 3px; }");
    updateBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 6px; border-radius: 3px; }");

    userButtonLayout->addWidget(addBtn);
    userButtonLayout->addWidget(removeBtn);
    userButtonLayout->addWidget(updateBtn);
    userLayout->addLayout(userButtonLayout);

    layout->addWidget(userGroup);

    // System Status Group
    QGroupBox *statusGroup = new QGroupBox("System Status");
    statusGroup->setStyleSheet("QGroupBox { font-weight: bold; margin: 10px; }");
    auto *statusLayout = new QVBoxLayout(statusGroup);

    systemStatus = new QLabel("🟢 All systems operational");
    systemStatus->setStyleSheet("color: #4caf50; font-size: 12pt; padding: 5px;");
    statusLayout->addWidget(systemStatus);

    dbStatus = new QLabel("🟢 Database connected");
    dbStatus->setStyleSheet("color: #4caf50; font-size: 12pt; padding: 5px;");
    statusLayout->addWidget(dbStatus);

    QPushButton *refreshStatusBtn = new QPushButton("Refresh System Status");
    refreshStatusBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 8px; border-radius: 4px; }");
    statusLayout->addWidget(refreshStatusBtn);

    layout->addWidget(statusGroup);

    // Connect signals
    connect(addBtn, &QPushButton::clicked, this, &AdminPanel::addUser);
    connect(removeBtn, &QPushButton::clicked, this, &AdminPanel::removeUser);
    connect(updateBtn, &QPushButton::clicked, this, &AdminPanel::updateUser);
    connect(refreshStatusBtn, &QPushButton::clicked, this, &AdminPanel::refreshSystemStatus);
    connect(userEntry, &QLineEdit::returnPressed, this, &AdminPanel::addUser);

    // Initialize with some default users
    initializeDefaultUsers();
    refreshSystemStatus();
}

void AdminPanel::addUser() {
    QString user = userEntry->text().trimmed();
    QString role = roleEntry->currentText();
    
    if (user.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a username.");
        return;
    }

    // Check if user already exists
    for (int i = 0; i < userList->count(); ++i) {
        if (userList->item(i)->text().startsWith(user + " (")) {
            QMessageBox::warning(this, "User Exists", "User already exists. Use Update Role instead.");
            return;
        }
    }

    QString roleIcon = getRoleIcon(role);
    userList->addItem(QString("%1 %2 (%3)").arg(roleIcon).arg(user).arg(role));
    userEntry->clear();

    // Log the action
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Admin', 'Info', :msg)");
    q.bindValue(":msg", QString("User '%1' added with role '%2'").arg(user).arg(role));
    q.exec();
}

void AdminPanel::removeUser() {
    QListWidgetItem *item = userList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a user to remove.");
        return;
    }

    QString userText = item->text();
    QString username = extractUsername(userText);
    
    int ret = QMessageBox::question(this, "Confirm Removal", 
                                   QString("Are you sure you want to remove user '%1'?").arg(username),
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        delete item;
        
        // Log the action
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Admin', 'Warning', :msg)");
        q.bindValue(":msg", QString("User '%1' removed from system").arg(username));
        q.exec();
    }
}

void AdminPanel::updateUser() {
    QListWidgetItem *item = userList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a user to update.");
        return;
    }

    QString userText = item->text();
    QString username = extractUsername(userText);
    QString newRole = roleEntry->currentText();
    QString roleIcon = getRoleIcon(newRole);
    
    item->setText(QString("%1 %2 (%3)").arg(roleIcon).arg(username).arg(newRole));
    
    // Log the action
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Admin', 'Info', :msg)");
    q.bindValue(":msg", QString("User '%1' role updated to '%2'").arg(username).arg(newRole));
    q.exec();
}

void AdminPanel::refreshSystemStatus() {
    // Simulate system status checks
    bool dbConnected = true; // In real implementation, check actual DB connection
    bool systemHealthy = true;
    
    if (systemHealthy) {
        systemStatus->setText("🟢 All systems operational");
        systemStatus->setStyleSheet("color: #4caf50; font-size: 12pt; padding: 5px;");
    } else {
        systemStatus->setText("🔴 System issues detected");
        systemStatus->setStyleSheet("color: #f44336; font-size: 12pt; padding: 5px;");
    }
    
    if (dbConnected) {
        dbStatus->setText("🟢 Database connected");
        dbStatus->setStyleSheet("color: #4caf50; font-size: 12pt; padding: 5px;");
    } else {
        dbStatus->setText("🔴 Database connection failed");
        dbStatus->setStyleSheet("color: #f44336; font-size: 12pt; padding: 5px;");
    }
    
    // Log status check
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Admin', 'Info', :msg)");
    q.bindValue(":msg", "System status refreshed");
    q.exec();
}

void AdminPanel::initializeDefaultUsers() {
    userList->addItem("👑 admin (admin)");
    userList->addItem("🔍 analyst1 (analyst)");
    userList->addItem("🔍 analyst2 (analyst)");
    userList->addItem("👁️ viewer1 (viewer)");
}

QString AdminPanel::getRoleIcon(const QString &role) {
    if (role == "admin") return "👑";
    else if (role == "analyst") return "🔍";
    else if (role == "viewer") return "👁️";
    else return "👤";
}

QString AdminPanel::extractUsername(const QString &userText) {
    // Extract username from "icon username (role)" format
    int spaceIndex = userText.indexOf(' ');
    int parenIndex = userText.indexOf(" (");
    if (spaceIndex > 0 && parenIndex > spaceIndex) {
        return userText.mid(spaceIndex + 1, parenIndex - spaceIndex - 1);
    }
    return userText;
}