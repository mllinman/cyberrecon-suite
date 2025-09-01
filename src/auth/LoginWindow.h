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
};