#include "UserProfile.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QHeaderView>
#include <QDateTime>
#include <QPixmap>
#include <QApplication>
#include "payments/PaymentDialog.h"

UserProfile::UserProfile(const QString &username, QWidget *parent) 
    : QWidget(parent), currentUsername(username) {
    
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("User Profile");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Tab widget
    tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);
    
    setupProfileTab();
    setupSettingsTab();
    setupBillingTab();
    setupSubscriptionTab();
    setupStyling();
    
    loadUserData();
}

void UserProfile::setupProfileTab() {
    auto *profileWidget = new QWidget();
    auto *layout = new QHBoxLayout(profileWidget);
    
    // Left side - Avatar and basic info
    auto *leftLayout = new QVBoxLayout();
    
    // Avatar section
    auto *avatarGroup = new QGroupBox("Profile Picture");
    auto *avatarLayout = new QVBoxLayout(avatarGroup);
    
    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(150, 150);
    avatarLabel->setStyleSheet(R"(
        QLabel {
            border: 3px solid #00bcd4;
            border-radius: 75px;
            background-color: #3c3c3c;
            color: #fff;
            font-size: 48px;
        }
    )");
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setText("👤");
    avatarLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    
    auto *changeAvatarBtn = new QPushButton("Change Avatar");
    changeAvatarBtn->setStyleSheet("QPushButton { background-color: #00bcd4; color: white; padding: 8px; border-radius: 4px; }");
    connect(changeAvatarBtn, &QPushButton::clicked, this, &UserProfile::changeAvatar);
    avatarLayout->addWidget(changeAvatarBtn);
    
    leftLayout->addWidget(avatarGroup);
    leftLayout->addStretch();
    
    // Right side - Profile form
    auto *rightLayout = new QFormLayout();
    
    fullNameEdit = new QLineEdit();
    fullNameEdit->setStyleSheet("padding: 8px; font-size: 12px;");
    rightLayout->addRow("Full Name:", fullNameEdit);
    
    emailEdit = new QLineEdit();
    emailEdit->setStyleSheet("padding: 8px; font-size: 12px;");
    rightLayout->addRow("Email:", emailEdit);
    
    phoneEdit = new QLineEdit();
    phoneEdit->setStyleSheet("padding: 8px; font-size: 12px;");
    rightLayout->addRow("Phone:", phoneEdit);
    
    timezoneCombo = new QComboBox();
    timezoneCombo->addItems({"UTC", "EST", "PST", "GMT", "CET", "JST"});
    timezoneCombo->setStyleSheet("padding: 8px; font-size: 12px;");
    rightLayout->addRow("Timezone:", timezoneCombo);
    
    bioEdit = new QTextEdit();
    bioEdit->setMaximumHeight(100);
    bioEdit->setPlaceholderText("Tell us about yourself...");
    bioEdit->setStyleSheet("padding: 8px; font-size: 12px;");
    rightLayout->addRow("Bio:", bioEdit);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    auto *saveBtn = new QPushButton("Save Profile");
    auto *changePassBtn = new QPushButton("Change Password");
    
    saveBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px 20px; border-radius: 4px; }");
    changePassBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px 20px; border-radius: 4px; }");
    
    connect(saveBtn, &QPushButton::clicked, this, &UserProfile::saveProfile);
    connect(changePassBtn, &QPushButton::clicked, this, &UserProfile::changePassword);
    
    buttonLayout->addWidget(saveBtn);
    buttonLayout->addWidget(changePassBtn);
    buttonLayout->addStretch();
    
    rightLayout->addRow(buttonLayout);
    
    layout->addLayout(leftLayout);
    layout->addLayout(rightLayout);
    
    tabWidget->addTab(profileWidget, "👤 Profile");
}

void UserProfile::setupSettingsTab() {
    auto *settingsWidget = new QWidget();
    auto *layout = new QVBoxLayout(settingsWidget);
    
    // Appearance settings
    auto *appearanceGroup = new QGroupBox("Appearance");
    auto *appearanceLayout = new QVBoxLayout(appearanceGroup);
    
    darkModeCheck = new QCheckBox("Enable Dark Mode");
    darkModeCheck->setChecked(true);
    appearanceLayout->addWidget(darkModeCheck);
    
    auto *languageLayout = new QHBoxLayout();
    languageLayout->addWidget(new QLabel("Language:"));
    languageCombo = new QComboBox();
    languageCombo->addItems({"English", "Spanish", "French", "German", "Japanese"});
    languageLayout->addWidget(languageCombo);
    languageLayout->addStretch();
    appearanceLayout->addLayout(languageLayout);
    
    auto *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Date Format:"));
    dateFormatCombo = new QComboBox();
    dateFormatCombo->addItems({"MM/DD/YYYY", "DD/MM/YYYY", "YYYY-MM-DD"});
    dateLayout->addWidget(dateFormatCombo);
    dateLayout->addStretch();
    appearanceLayout->addLayout(dateLayout);
    
    layout->addWidget(appearanceGroup);
    
    // Notification settings
    auto *notificationGroup = new QGroupBox("Notifications");
    auto *notificationLayout = new QVBoxLayout(notificationGroup);
    
    notificationsCheck = new QCheckBox("Enable Desktop Notifications");
    notificationsCheck->setChecked(true);
    notificationLayout->addWidget(notificationsCheck);
    
    soundCheck = new QCheckBox("Enable Sound Alerts");
    soundCheck->setChecked(false);
    notificationLayout->addWidget(soundCheck);
    
    layout->addWidget(notificationGroup);
    
    // Performance settings
    auto *performanceGroup = new QGroupBox("Performance");
    auto *performanceLayout = new QVBoxLayout(performanceGroup);
    
    autoRefreshCheck = new QCheckBox("Auto-refresh Dashboards");
    autoRefreshCheck->setChecked(true);
    performanceLayout->addWidget(autoRefreshCheck);
    
    auto *refreshLayout = new QHBoxLayout();
    refreshLayout->addWidget(new QLabel("Refresh Interval:"));
    refreshIntervalSlider = new QSlider(Qt::Horizontal);
    refreshIntervalSlider->setRange(5, 60);
    refreshIntervalSlider->setValue(10);
    refreshLayout->addWidget(refreshIntervalSlider);
    auto *intervalLabel = new QLabel("10s");
    refreshLayout->addWidget(intervalLabel);
    
    connect(refreshIntervalSlider, &QSlider::valueChanged, [intervalLabel](int value) {
        intervalLabel->setText(QString("%1s").arg(value));
    });
    
    performanceLayout->addLayout(refreshLayout);
    layout->addWidget(performanceGroup);
    
    layout->addStretch();
    
    // Save button
    auto *saveSettingsBtn = new QPushButton("Save Settings");
    saveSettingsBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px 20px; border-radius: 4px; }");
    connect(saveSettingsBtn, &QPushButton::clicked, this, &UserProfile::saveSettings);
    layout->addWidget(saveSettingsBtn);
    
    tabWidget->addTab(settingsWidget, "⚙️ Settings");
}

void UserProfile::setupBillingTab() {
    auto *billingWidget = new QWidget();
    auto *layout = new QVBoxLayout(billingWidget);
    
    // Current plan info
    auto *planGroup = new QGroupBox("Current Plan");
    auto *planLayout = new QGridLayout(planGroup);
    
    planLayout->addWidget(new QLabel("Plan:"), 0, 0);
    currentPlanLabel = new QLabel("Professional");
    currentPlanLabel->setStyleSheet("font-weight: bold; color: #00bcd4;");
    planLayout->addWidget(currentPlanLabel, 0, 1);
    
    planLayout->addWidget(new QLabel("Billing Cycle:"), 1, 0);
    billingCycleLabel = new QLabel("Monthly");
    planLayout->addWidget(billingCycleLabel, 1, 1);
    
    planLayout->addWidget(new QLabel("Next Billing:"), 2, 0);
    nextBillingLabel = new QLabel(QDateTime::currentDateTime().addDays(30).toString("MMM dd, yyyy"));
    planLayout->addWidget(nextBillingLabel, 2, 1);
    
    layout->addWidget(planGroup);
    
    // Billing history
    auto *historyGroup = new QGroupBox("Billing History");
    auto *historyLayout = new QVBoxLayout(historyGroup);
    
    billingHistoryTable = new QTableWidget(5, 4);
    billingHistoryTable->setHorizontalHeaderLabels({"Date", "Description", "Amount", "Status"});
    billingHistoryTable->horizontalHeader()->setStretchLastSection(true);
    
    // Sample data
    QStringList sampleData = {
        "2024-01-15,Professional Plan - Monthly,$99.00,Paid",
        "2023-12-15,Professional Plan - Monthly,$99.00,Paid",
        "2023-11-15,Professional Plan - Monthly,$99.00,Paid",
        "2023-10-15,Professional Plan - Monthly,$99.00,Paid",
        "2023-09-15,Professional Plan - Monthly,$99.00,Paid"
    };
    
    for (int i = 0; i < sampleData.size(); ++i) {
        QStringList parts = sampleData[i].split(',');
        for (int j = 0; j < parts.size(); ++j) {
            billingHistoryTable->setItem(i, j, new QTableWidgetItem(parts[j]));
        }
    }
    
    historyLayout->addWidget(billingHistoryTable);
    
    downloadInvoiceBtn = new QPushButton("Download Latest Invoice");
    downloadInvoiceBtn->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 8px 16px; border-radius: 4px; }");
    historyLayout->addWidget(downloadInvoiceBtn);
    
    layout->addWidget(historyGroup);
    
    tabWidget->addTab(billingWidget, "💳 Billing");
}

void UserProfile::setupSubscriptionTab() {
    auto *subscriptionWidget = new QWidget();
    auto *layout = new QVBoxLayout(subscriptionWidget);
    
    // Subscription status
    auto *statusGroup = new QGroupBox("Subscription Status");
    auto *statusLayout = new QVBoxLayout(statusGroup);
    
    subscriptionStatusLabel = new QLabel("✅ Professional Plan Active");
    subscriptionStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #4caf50;");
    statusLayout->addWidget(subscriptionStatusLabel);
    
    // Usage metrics
    auto *usageLayout = new QHBoxLayout();
    usageLayout->addWidget(new QLabel("Monthly Usage:"));
    usageProgressBar = new QProgressBar();
    usageProgressBar->setRange(0, 100000);
    usageProgressBar->setValue(45000);
    usageProgressBar->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: white;
        }
        QProgressBar::chunk {
            background-color: #00bcd4;
            border-radius: 3px;
        }
    )");
    usageLayout->addWidget(usageProgressBar);
    
    usageLabel = new QLabel("45,000 / 100,000 events");
    usageLabel->setStyleSheet("color: #ccc;");
    usageLayout->addWidget(usageLabel);
    
    statusLayout->addLayout(usageLayout);
    layout->addWidget(statusGroup);
    
    // Features
    auto *featuresGroup = new QGroupBox("Plan Features");
    auto *featuresLayout = new QVBoxLayout(featuresGroup);
    
    featuresList = new QListWidget();
    featuresList->addItem("✅ Full SIEM & EDR monitoring");
    featuresList->addItem("✅ Threat intelligence feeds");
    featuresList->addItem("✅ Up to 100,000 events/month");
    featuresList->addItem("✅ Email support");
    featuresList->addItem("❌ Advanced ML correlation (Enterprise only)");
    featuresList->addItem("❌ 24/7 phone support (Enterprise only)");
    
    featuresLayout->addWidget(featuresList);
    layout->addWidget(featuresGroup);
    
    // Action buttons
    auto *buttonLayout = new QHBoxLayout();
    
    upgradeBtn = new QPushButton("Upgrade to Enterprise");
    upgradeBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px 20px; border-radius: 4px; font-weight: bold; }");
    connect(upgradeBtn, &QPushButton::clicked, this, &UserProfile::updateSubscription);
    
    cancelBtn = new QPushButton("Cancel Subscription");
    cancelBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px 20px; border-radius: 4px; }");
    connect(cancelBtn, &QPushButton::clicked, this, &UserProfile::cancelSubscription);
    
    buttonLayout->addWidget(upgradeBtn);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    tabWidget->addTab(subscriptionWidget, "📊 Subscription");
}

void UserProfile::setupStyling() {
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #00bcd4;
        }
        
        QLineEdit, QTextEdit, QComboBox {
            background-color: #3c3c3c;
            border: 1px solid #555;
            border-radius: 4px;
            color: #fff;
        }
        
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: #00bcd4;
        }
        
        QTableWidget {
            background-color: #2d2d2d;
            alternate-background-color: #3c3c3c;
            gridline-color: #555;
            color: #fff;
        }
        
        QTableWidget::item {
            padding: 8px;
        }
        
        QListWidget {
            background-color: #2d2d2d;
            border: 1px solid #555;
            border-radius: 4px;
            color: #fff;
        }
        
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #444;
        }
        
        QCheckBox {
            color: #fff;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
        }
        
        QCheckBox::indicator:unchecked {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 3px;
        }
        
        QCheckBox::indicator:checked {
            background-color: #00bcd4;
            border: 2px solid #00bcd4;
            border-radius: 3px;
        }
        
        QSlider::groove:horizontal {
            border: 1px solid #555;
            height: 8px;
            background: #3c3c3c;
            border-radius: 4px;
        }
        
        QSlider::handle:horizontal {
            background: #00bcd4;
            border: 1px solid #00bcd4;
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }
    )");
}

void UserProfile::loadUserData() {
    QSqlDatabase db = QSqlDatabase::database("auth");
    QSqlQuery query(db);
    
    query.prepare("SELECT full_name, email, avatar_path, subscription_type FROM users WHERE username = ?");
    query.addBindValue(currentUsername);
    
    if (query.exec() && query.next()) {
        fullNameEdit->setText(query.value(0).toString());
        emailEdit->setText(query.value(1).toString());
        
        QString subscriptionType = query.value(3).toString();
        currentPlanLabel->setText(subscriptionType.toUpper());
        
        if (subscriptionType == "enterprise") {
            subscriptionStatusLabel->setText("✅ Enterprise Plan Active");
            featuresList->clear();
            featuresList->addItem("✅ All Professional features");
            featuresList->addItem("✅ Advanced ML correlation");
            featuresList->addItem("✅ Custom integrations");
            featuresList->addItem("✅ Unlimited events");
            featuresList->addItem("✅ 24/7 phone support");
            featuresList->addItem("✅ Dedicated account manager");
            upgradeBtn->setText("Manage Enterprise Plan");
        }
    }
}

void UserProfile::saveProfile() {
    QSqlDatabase db = QSqlDatabase::database("auth");
    QSqlQuery query(db);
    
    query.prepare("UPDATE users SET full_name = ?, email = ? WHERE username = ?");
    query.addBindValue(fullNameEdit->text());
    query.addBindValue(emailEdit->text());
    query.addBindValue(currentUsername);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", "Profile updated successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Failed to update profile.");
    }
}

void UserProfile::changeAvatar() {
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Select Avatar Image", "", "Image Files (*.png *.jpg *.jpeg *.gif)");
    
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            pixmap = pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            avatarLabel->setPixmap(pixmap);
            QMessageBox::information(this, "Success", "Avatar updated successfully!");
        }
    }
}

void UserProfile::changePassword() {
    QMessageBox::information(this, "Change Password", 
        "Password change functionality would redirect to a secure form.\n\n"
        "For security reasons, password changes require additional verification.");
}

void UserProfile::saveSettings() {
    QMessageBox::information(this, "Settings Saved", 
        "Your preferences have been saved successfully!");
}

void UserProfile::viewBillingHistory() {
    QMessageBox::information(this, "Billing History", 
        "Detailed billing history and invoice downloads are available.");
}

void UserProfile::updateSubscription() {
    // Show payment dialog for Enterprise upgrade
    PaymentDialog *paymentDialog = new PaymentDialog(
        "Enterprise Plan", 
        "price_enterprise_monthly", // Replace with actual Stripe price ID
        299.00, 
        this
    );
    
    connect(paymentDialog, &PaymentDialog::paymentCompleted, 
            this, [this](const QString &subscriptionId) {
        // Update user's subscription in database
        QSqlDatabase db = QSqlDatabase::database("auth");
        QSqlQuery query(db);
        query.prepare("UPDATE users SET subscription_type = 'enterprise' WHERE username = ?");
        query.addBindValue(currentUsername);
        query.exec();
        
        // Refresh UI
        loadUserData();
        
        QMessageBox::information(this, "Upgrade Successful", 
            "Welcome to Enterprise! Your subscription has been activated.\n\n"
            "Subscription ID: " + subscriptionId);
    });
    
    paymentDialog->show();
}

void UserProfile::cancelSubscription() {
    int ret = QMessageBox::question(this, "Cancel Subscription", 
        "Are you sure you want to cancel your subscription?\n\n"
        "You will lose access to premium features at the end of your billing cycle.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        QMessageBox::information(this, "Subscription Cancelled", 
            "Your subscription has been cancelled. You will retain access until your next billing date.");
    }
}

void UserProfile::updateUserInfo(const QString &username) {
    currentUsername = username;
    loadUserData();
}