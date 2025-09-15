#include "CollaborationChat.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QTime>

CollaborationChat::CollaborationChat(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Team Collaboration Chat");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    chatBox = new QTextEdit();
    chatBox->setReadOnly(true);
    chatBox->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace; padding: 10px;");
    layout->addWidget(chatBox);

    auto *inputLayout = new QHBoxLayout();
    input = new QLineEdit();
    input->setPlaceholderText("Type your message here...");
    input->setStyleSheet("padding: 8px; font-size: 12pt;");
    
    QPushButton *sendBtn = new QPushButton("Send");
    sendBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px 15px; border-radius: 4px; }");
    
    inputLayout->addWidget(input);
    inputLayout->addWidget(sendBtn);
    layout->addLayout(inputLayout);

    auto *buttonLayout = new QHBoxLayout();
    QPushButton *clearBtn = new QPushButton("Clear Chat");
    QPushButton *statusBtn = new QPushButton("Set Status");
    
    clearBtn->setStyleSheet("QPushButton { background-color: #607d8b; color: white; padding: 6px; border-radius: 3px; }");
    statusBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 6px; border-radius: 3px; }");
    
    buttonLayout->addWidget(clearBtn);
    buttonLayout->addWidget(statusBtn);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    connect(sendBtn, &QPushButton::clicked, this, &CollaborationChat::sendMessage);
    connect(input, &QLineEdit::returnPressed, this, &CollaborationChat::sendMessage);
    connect(clearBtn, &QPushButton::clicked, [this]() { chatBox->clear(); });
    connect(statusBtn, &QPushButton::clicked, this, &CollaborationChat::setStatus);

    // Get current user
    currentUser = qgetenv("USER");
    if (currentUser.isEmpty()) {
        currentUser = qgetenv("USERNAME"); // Windows compatibility
    }
    if (currentUser.isEmpty()) {
        currentUser = "analyst";
    }

    // Auto-generate team messages
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CollaborationChat::simulateTeamActivity);
    timer->start(25000); // every 25 seconds

    // Welcome message
    chatBox->append("<font color='#00bcd4'>[System] Welcome to the SOC team chat!</font>");
    chatBox->append("<font color='#4caf50'>[System] You are logged in as: " + currentUser + "</font>");
    
    simulateTeamActivity();
}

void CollaborationChat::sendMessage() {
    QString msg = input->text().trimmed();
    if (msg.isEmpty()) return;

    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString fullMsg = QString("<font color='#2196f3'>[%1] %2:</font> %3")
                     .arg(timestamp).arg(currentUser).arg(msg);
    
    chatBox->append(fullMsg);
    input->clear();

    // Log to events for case management integration
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('Chat', 'Info', :msg)");
    q.bindValue(":msg", QString("Chat (%1): %2").arg(currentUser).arg(msg));
    q.exec();
}

void CollaborationChat::setStatus() {
    QStringList statuses = {"🟢 Available", "🟡 Busy", "🔴 Do Not Disturb", "🌙 Away"};
    static int statusIndex = 0;
    
    QString status = statuses[statusIndex % statuses.size()];
    statusIndex++;
    
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    chatBox->append(QString("<font color='#ff9800'>[%1] %2 status: %3</font>")
                   .arg(timestamp).arg(currentUser).arg(status));
}

void CollaborationChat::simulateTeamActivity() {
    QStringList teamMembers = {"alice", "bob", "charlie", "diana", "eve"};
    QStringList messages = {
        "Just finished analyzing the malware sample from case #1234",
        "Seeing increased phishing attempts in the last hour",
        "Can someone help me with this suspicious PowerShell script?",
        "Updated the IOC feed with new threat indicators",
        "Incident response team is investigating the data breach",
        "All clear on the network monitoring front",
        "Found some interesting artifacts in the memory dump",
        "Compliance audit is scheduled for next week",
        "New YARA rules deployed to all endpoints",
        "Coffee break anyone? ☕"
    };
    
    QString member = teamMembers[QRandomGenerator::global()->bounded(teamMembers.size())];
    QString message = messages[QRandomGenerator::global()->bounded(messages.size())];
    QString color = "#4caf50"; // Default green
    if (message.contains("suspicious") || message.contains("breach") || message.contains("malware")) {
        color = "#f44336"; // Red for security-related
    } else if (message.contains("help") || message.contains("investigating")) {
        color = "#ff9800"; // Orange for requests/investigations
    }
    
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString formattedMsg = QString("<font color='%1'>[%2] %3:</font> %4")
                          .arg(color).arg(timestamp).arg(member).arg(message);
    
    chatBox->append(formattedMsg);
    
    // Scroll to bottom
    chatBox->moveCursor(QTextCursor::End);
}