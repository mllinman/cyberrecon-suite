#include "LoginWindow.h"
#include <QApplication>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSettings>
#include <QGridLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include "payments/PaymentDialog.h"
#include "compliance/AuditLogger.h"

LoginWindow::LoginWindow(QWidget *parent) 
    : QDialog(parent), subscriptionActive(false) {
    setWindowTitle("CyberRecon Suite - Login");
    setFixedSize(450, 600);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    
    initializeDatabase();
    setupUI();
    setupStyling();
    
    // Center window
    move(QApplication::primaryScreen()->geometry().center() - rect().center());
}

void LoginWindow::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Legal disclaimer for penetration testing tools
    auto *disclaimerLabel = new QLabel("⚠️ By logging in, you agree to use penetration testing tools only for authorized security testing");
    disclaimerLabel->setStyleSheet("color: #ff9800; font-size: 10px; margin: 5px; text-align: center;");
    disclaimerLabel->setAlignment(Qt::AlignCenter);
    disclaimerLabel->setWordWrap(true);
    mainLayout->addWidget(disclaimerLabel);

    // Logo and title
    logoLabel = new QLabel();
    logoLabel->setText("🛡️ CyberRecon Suite");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #00bcd4; margin-bottom: 10px;");
    mainLayout->addWidget(logoLabel);

    auto *subtitleLabel = new QLabel("Advanced Cybersecurity Operations Platform");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #888; margin-bottom: 30px;");
    mainLayout->addWidget(subtitleLabel);

    // Login form container
    auto *formFrame = new QFrame();
    formFrame->setObjectName("loginForm");
    auto *formLayout = new QVBoxLayout(formFrame);
    formLayout->setSpacing(15);

    // Username field
    auto *usernameLabel = new QLabel("Username:");
    usernameLabel->setStyleSheet("font-weight: bold; color: #fff;");
    formLayout->addWidget(usernameLabel);
    
    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Enter your username");
    usernameEdit->setMinimumHeight(40);
    formLayout->addWidget(usernameEdit);

    // Password field
    auto *passwordLabel = new QLabel("Password:");
    passwordLabel->setStyleSheet("font-weight: bold; color: #fff;");
    formLayout->addWidget(passwordLabel);
    
    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Enter your password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(40);
    formLayout->addWidget(passwordEdit);

    // Remember me checkbox
    rememberMeCheck = new QCheckBox("Remember me");
    rememberMeCheck->setStyleSheet("color: #ccc;");
    formLayout->addWidget(rememberMeCheck);

    mainLayout->addWidget(formFrame);

    // Buttons
    loginBtn = new QPushButton("Login");
    loginBtn->setMinimumHeight(45);
    loginBtn->setObjectName("primaryButton");
    mainLayout->addWidget(loginBtn);

    signupBtn = new QPushButton("Create Account");
    signupBtn->setMinimumHeight(40);
    signupBtn->setObjectName("secondaryButton");
    mainLayout->addWidget(signupBtn);

    subscribeBtn = new QPushButton("View Subscription Plans");
    subscribeBtn->setMinimumHeight(40);
    subscribeBtn->setObjectName("subscriptionButton");
    mainLayout->addWidget(subscribeBtn);

    forgotPasswordBtn = new QPushButton("Forgot Password?");
    forgotPasswordBtn->setObjectName("linkButton");
    mainLayout->addWidget(forgotPasswordBtn);

    // Status label
    statusLabel = new QLabel();
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);

    // Connect signals
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(signupBtn, &QPushButton::clicked, this, &LoginWindow::handleSignup);
    connect(forgotPasswordBtn, &QPushButton::clicked, this, &LoginWindow::handleForgotPassword);
    connect(subscribeBtn, &QPushButton::clicked, this, &LoginWindow::showSubscriptionPlans);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::handleLogin);
}

void LoginWindow::setupStyling() {
    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1a1a1a, stop:1 #2d2d2d);
        }
        
        QFrame#loginForm {
            background-color: rgba(45, 45, 45, 0.8);
            border: 1px solid #444;
            border-radius: 10px;
            padding: 20px;
        }
        
        QLineEdit {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            color: #fff;
        }
        
        QLineEdit:focus {
            border-color: #00bcd4;
        }
        
        QPushButton#primaryButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #00bcd4, stop:1 #0097a7);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
        }
        
        QPushButton#primaryButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #26c6da, stop:1 #00acc1);
        }
        
        QPushButton#secondaryButton {
            background-color: #4a4a4a;
            color: white;
            border: 1px solid #666;
            border-radius: 8px;
            font-size: 14px;
        }
        
        QPushButton#secondaryButton:hover {
            background-color: #555;
        }
        
        QPushButton#subscriptionButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ff9800, stop:1 #f57c00);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
        }
        
        QPushButton#subscriptionButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffb74d, stop:1 #ff9800);
        }
        
        QPushButton#linkButton {
            background: transparent;
            color: #00bcd4;
            border: none;
            text-decoration: underline;
        }
        
        QPushButton#linkButton:hover {
            color: #26c6da;
        }
        
        QCheckBox {
            color: #ccc;
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
    )");
}

void LoginWindow::initializeDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "auth");
    db.setDatabaseName("cyberrecon_auth.db");
    
    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open authentication database");
        return;
    }
    
    QSqlQuery query(db);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            email TEXT,
            full_name TEXT,
            avatar_path TEXT,
            subscription_type TEXT DEFAULT 'free',
            subscription_expires DATETIME,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_login DATETIME
        )
    )");
    
    // Create default admin user if none exists
    QSqlQuery checkQuery("SELECT COUNT(*) FROM users", db);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() == 0) {
        QSqlQuery insertQuery(db);
        insertQuery.prepare(R"(
            INSERT INTO users (username, password_hash, email, full_name, subscription_type) 
            VALUES (?, ?, ?, ?, ?)
        )");
        
        QString adminPassword = QCryptographicHash::hash("admin123", QCryptographicHash::Sha256).toHex();
        insertQuery.addBindValue("admin");
        insertQuery.addBindValue(adminPassword);
        insertQuery.addBindValue("admin@cyberrecon.com");
        insertQuery.addBindValue("System Administrator");
        insertQuery.addBindValue("enterprise");
        insertQuery.exec();
        
        // Create demo user
        QString demoPassword = QCryptographicHash::hash("demo123", QCryptographicHash::Sha256).toHex();
        insertQuery.addBindValue("demo");
        insertQuery.addBindValue(demoPassword);
        insertQuery.addBindValue("demo@cyberrecon.com");
        insertQuery.addBindValue("Demo User");
        insertQuery.addBindValue("professional");
        insertQuery.exec();
    }
}

bool LoginWindow::validateCredentials(const QString &username, const QString &password) {
    QSqlDatabase db = QSqlDatabase::database("auth");
    QSqlQuery query(db);
    
    query.prepare("SELECT password_hash, subscription_type FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        QString storedHash = query.value(0).toString();
        QString inputHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
        
        if (storedHash == inputHash) {
            currentUser = username;
            QString subscriptionType = query.value(1).toString();
            subscriptionActive = (subscriptionType != "free");
            
            // Update last login
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE username = ?");
            updateQuery.addBindValue(username);
            updateQuery.exec();
            
            return true;
        }
    }
    
    return false;
}

void LoginWindow::handleLogin() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("<font color='#f44336'>Please enter both username and password</font>");
        return;
    }
    
    loginBtn->setEnabled(false);
    loginBtn->setText("Logging in...");
    statusLabel->setText("<font color='#2196f3'>Authenticating...</font>");
    
    // Simulate authentication delay
    QTimer::singleShot(1000, this, [this, username, password]() {
        if (validateCredentials(username, password)) {
            if (rememberMeCheck->isChecked()) {
                QSettings settings;
                settings.setValue("auth/remember_user", username);
            }
            
            // Log successful authentication for SOC 2 compliance
            AuditLogger::instance()->logAuthenticationEvent(username, "Success", "127.0.0.1");
            
            statusLabel->setText("<font color='#4caf50'>Login successful!</font>");
            QTimer::singleShot(500, this, &QDialog::accept);
        } else {
            // Log failed authentication for SOC 2 compliance
            AuditLogger::instance()->logAuthenticationEvent(username, "Failed", "127.0.0.1");
            
            statusLabel->setText("<font color='#f44336'>Invalid username or password</font>");
            loginBtn->setEnabled(true);
            loginBtn->setText("Login");
        }
    });
}

void LoginWindow::handleSignup() {
    QMessageBox::information(this, "Create Account", 
        "Account creation is available through our website.\n\n"
        "For demo purposes, use:\n"
        "Username: demo\n"
        "Password: demo123\n\n"
        "Or admin/admin123 for full access.");
}

void LoginWindow::handleForgotPassword() {
    QMessageBox::information(this, "Password Recovery", 
        "Password recovery is handled through our secure web portal.\n\n"
        "For demo purposes:\n"
        "• demo/demo123 (Professional Plan)\n"
        "• admin/admin123 (Enterprise Plan)");
}

void LoginWindow::showSubscriptionPlans() {
    // Create subscription selection dialog
    QDialog *planDialog = new QDialog(this);
    planDialog->setWindowTitle("Choose Your Plan");
    planDialog->setModal(true);
    planDialog->resize(600, 500);
    
    auto *layout = new QVBoxLayout(planDialog);
    
    // Title
    auto *titleLabel = new QLabel("🛡️ CyberRecon Suite Subscription Plans");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #00bcd4; margin: 20px; text-align: center;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Professional Plan
    auto *proGroup = new QGroupBox("💼 Professional Plan - $99/month");
    auto *proLayout = new QVBoxLayout(proGroup);
    proLayout->addWidget(new QLabel("• Full SIEM & EDR monitoring"));
    proLayout->addWidget(new QLabel("• Threat intelligence feeds"));
    proLayout->addWidget(new QLabel("• Up to 100,000 events/day"));
    proLayout->addWidget(new QLabel("• Email support"));
    
    auto *proButton = new QPushButton("Subscribe to Professional");
    proButton->setStyleSheet("QPushButton { background-color: #2196f3; color: white; padding: 10px; border-radius: 6px; font-weight: bold; }");
    proLayout->addWidget(proButton);
    layout->addWidget(proGroup);
    
    // Enterprise Plan
    auto *entGroup = new QGroupBox("🏢 Enterprise Plan - $299/month");
    auto *entLayout = new QVBoxLayout(entGroup);
    entLayout->addWidget(new QLabel("• All Professional features"));
    entLayout->addWidget(new QLabel("• Advanced ML correlation"));
    entLayout->addWidget(new QLabel("• Custom integrations"));
    entLayout->addWidget(new QLabel("• Unlimited events"));
    entLayout->addWidget(new QLabel("• 24/7 phone support"));
    entLayout->addWidget(new QLabel("• Dedicated account manager"));
    
    auto *entButton = new QPushButton("Subscribe to Enterprise");
    entButton->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px; border-radius: 6px; font-weight: bold; }");
    entLayout->addWidget(entButton);
    layout->addWidget(entGroup);
    
    // Close button
    auto *closeButton = new QPushButton("Close");
    closeButton->setStyleSheet("QPushButton { background-color: #666; color: white; padding: 8px; border-radius: 4px; }");
    layout->addWidget(closeButton);
    
    // Connect buttons
    connect(proButton, &QPushButton::clicked, [this, planDialog]() {
        planDialog->close();
        PaymentDialog *payment = new PaymentDialog("Professional Plan", "price_professional_monthly", 99.00, this);
        payment->show();
    });
    
    connect(entButton, &QPushButton::clicked, [this, planDialog]() {
        planDialog->close();
        PaymentDialog *payment = new PaymentDialog("Enterprise Plan", "price_enterprise_monthly", 299.00, this);
        payment->show();
    });
    
    connect(closeButton, &QPushButton::clicked, planDialog, &QDialog::close);
    
    // Apply styling
    planDialog->setStyleSheet(R"(
        QDialog { background-color: #2d2d2d; color: #ffffff; }
        QGroupBox { 
            font-weight: bold; 
            border: 2px solid #555; 
            border-radius: 8px; 
            margin: 10px; 
            padding-top: 15px; 
        }
        QGroupBox::title { 
            subcontrol-origin: margin; 
            left: 15px; 
            padding: 0 8px 0 8px; 
            color: #00bcd4; 
            font-size: 16px; 
        }
        QLabel { color: #ffffff; margin: 5px; }
    )");
    
    planDialog->show();
}