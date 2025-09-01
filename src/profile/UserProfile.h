#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QProgressBar>
#include <QListWidget>
#include <QTableWidget>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>

class UserProfile : public QWidget {
    Q_OBJECT

public:
    explicit UserProfile(const QString &username, QWidget *parent = nullptr);
    void updateUserInfo(const QString &username);

private slots:
    void saveProfile();
    void changeAvatar();
    void changePassword();
    void saveSettings();
    void viewBillingHistory();
    void updateSubscription();
    void cancelSubscription();

private:
    void setupProfileTab();
    void setupSettingsTab();
    void setupBillingTab();
    void setupSubscriptionTab();
    void loadUserData();
    void setupStyling();
    
    QString currentUsername;
    QTabWidget *tabWidget;
    
    // Profile tab
    QLabel *avatarLabel;
    QLineEdit *fullNameEdit;
    QLineEdit *emailEdit;
    QLineEdit *phoneEdit;
    QComboBox *timezoneCombo;
    QTextEdit *bioEdit;
    
    // Settings tab
    QCheckBox *darkModeCheck;
    QCheckBox *notificationsCheck;
    QCheckBox *soundCheck;
    QCheckBox *autoRefreshCheck;
    QSlider *refreshIntervalSlider;
    QComboBox *languageCombo;
    QComboBox *dateFormatCombo;
    
    // Billing tab
    QLabel *currentPlanLabel;
    QLabel *billingCycleLabel;
    QLabel *nextBillingLabel;
    QTableWidget *billingHistoryTable;
    QPushButton *downloadInvoiceBtn;
    
    // Subscription tab
    QLabel *subscriptionStatusLabel;
    QProgressBar *usageProgressBar;
    QLabel *usageLabel;
    QListWidget *featuresList;
    QPushButton *upgradeBtn;
    QPushButton *cancelBtn;
};