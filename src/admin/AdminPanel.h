#pragma once
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

class AdminPanel : public QWidget {
    Q_OBJECT
public:
    explicit AdminPanel(QWidget *parent = nullptr);

private slots:
    void addUser();
    void removeUser();
    void updateUser();
    void refreshSystemStatus();

private:
    void initializeDefaultUsers();
    QString getRoleIcon(const QString &role);
    QString extractUsername(const QString &userText);
    
    QListWidget *userList;
    QLineEdit *userEntry;
    QComboBox *roleEntry;
    QLabel *systemStatus;
    QLabel *dbStatus;
};