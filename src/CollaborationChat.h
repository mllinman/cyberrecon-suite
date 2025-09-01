#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>

class CollaborationChat : public QWidget {
    Q_OBJECT
public:
    explicit CollaborationChat(QWidget *parent = nullptr);

private slots:
    void sendMessage();
    void setStatus();
    void simulateTeamActivity();

private:
    QTextEdit *chatBox;
    QLineEdit *input;
    QString currentUser;
};