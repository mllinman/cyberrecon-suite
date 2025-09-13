#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QProgressBar>
#include <QTimer>

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    QString getCurrentUser() const { return currentUser; }
    bool isSubscriptionActive() const { return subscriptionActive; }

private slots:
    void handleLogin();
    void handleSignup();
    void handleForgotPassword();
    void showSubscriptionPlans();

private:
    void setupUI();
    void setupStyling();
    bool validateCredentials(const QString &username, const QString &password);
    void initializeDatabase();
    
    // Enhanced security functions
    QString hashPasswordWithSalt(const QString &password, const QString &salt = "");
    QString generateSalt();
    bool verifyPasswordHash(const QString &password, const QString &storedHash);
    void lockoutUser(const QString &username, int minutes = 15);
    bool isUserLockedOut(const QString &username);
    
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginBtn;
    QPushButton *signupBtn;
    QPushButton *forgotPasswordBtn;
    QPushButton *subscribeBtn;
    QCheckBox *rememberMeCheck;
    QLabel *statusLabel;
    QLabel *logoLabel;
    
    QString currentUser;
    bool subscriptionActive;
    
    // Security tracking
    QMap<QString, int> failedAttempts;
    QMap<QString, QDateTime> lockoutTimes;
};