#include "PaymentDialog.h"
#include "StripeManager.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QJsonObject>
#include <QTimer>

PaymentDialog::PaymentDialog(const QString &planName, const QString &priceId, 
                           double amount, QWidget *parent)
    : QDialog(parent), planName(planName), priceId(priceId), amount(amount) {
    
    setWindowTitle("Subscribe to " + planName);
    setModal(true);
    setFixedSize(500, 600);
    
    stripeManager = new StripeManager(this);
    // Set your Stripe secret key here
    stripeManager->setApiKey("sk_test_your_stripe_secret_key_here");
    
    connect(stripeManager, &StripeManager::subscriptionCreated, 
            this, &PaymentDialog::onSubscriptionCreated);
    connect(stripeManager, &StripeManager::customerCreated, 
            this, &PaymentDialog::onCustomerCreated);
    connect(stripeManager, &StripeManager::errorOccurred, 
            this, &PaymentDialog::onPaymentError);
    
    setupUI();
    setupStyling();
}

void PaymentDialog::setupUI() {
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // Plan information
    auto *planGroup = new QGroupBox("Subscription Plan");
    auto *planLayout = new QVBoxLayout(planGroup);
    
    planLabel = new QLabel(planName);
    planLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4;");
    planLayout->addWidget(planLabel);
    
    amountLabel = new QLabel(QString("$%1/month").arg(amount, 0, 'f', 2));
    amountLabel->setStyleSheet("font-size: 16px; color: #4caf50; font-weight: bold;");
    planLayout->addWidget(amountLabel);
    
    layout->addWidget(planGroup);
    
    // Customer information
    auto *customerGroup = new QGroupBox("Customer Information");
    auto *customerLayout = new QVBoxLayout(customerGroup);
    
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Full Name");
    customerLayout->addWidget(nameEdit);
    
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Email Address");
    customerLayout->addWidget(emailEdit);
    
    layout->addWidget(customerGroup);
    
    // Payment information
    auto *paymentGroup = new QGroupBox("Payment Information");
    auto *paymentLayout = new QVBoxLayout(paymentGroup);
    
    cardNumberEdit = new QLineEdit();
    cardNumberEdit->setPlaceholderText("Card Number (4242 4242 4242 4242 for testing)");
    cardNumberEdit->setMaxLength(19);
    paymentLayout->addWidget(cardNumberEdit);
    
    auto *cardDetailsLayout = new QHBoxLayout();
    
    expiryEdit = new QLineEdit();
    expiryEdit->setPlaceholderText("MM/YY");
    expiryEdit->setMaxLength(5);
    cardDetailsLayout->addWidget(expiryEdit);
    
    cvcEdit = new QLineEdit();
    cvcEdit->setPlaceholderText("CVC");
    cvcEdit->setMaxLength(4);
    cardDetailsLayout->addWidget(cvcEdit);
    
    paymentLayout->addLayout(cardDetailsLayout);
    
    countryCombo = new QComboBox();
    countryCombo->addItems({"United States", "Canada", "United Kingdom", "Germany", "France", "Australia"});
    paymentLayout->addWidget(countryCombo);
    
    layout->addWidget(paymentGroup);
    
    // Progress bar
    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    layout->addWidget(progressBar);
    
    // Status label
    statusLabel = new QLabel();
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    
    cancelButton = new QPushButton("Cancel");
    payButton = new QPushButton("Subscribe Now");
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(payButton);
    layout->addLayout(buttonLayout);
    
    // Connect signals
    connect(payButton, &QPushButton::clicked, this, &PaymentDialog::processPayment);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // Format card number input
    connect(cardNumberEdit, &QLineEdit::textChanged, [this](const QString &text) {
        QString formatted = text;
        formatted.remove(QRegularExpression("[^0-9]"));
        
        QString result;
        for (int i = 0; i < formatted.length(); ++i) {
            if (i > 0 && i % 4 == 0) {
                result += " ";
            }
            result += formatted[i];
        }
        
        if (result != text) {
            cardNumberEdit->setText(result);
        }
    });
    
    // Format expiry input
    connect(expiryEdit, &QLineEdit::textChanged, [this](const QString &text) {
        QString formatted = text;
        formatted.remove(QRegularExpression("[^0-9]"));
        
        if (formatted.length() >= 2) {
            formatted.insert(2, "/");
        }
        
        if (formatted != text && formatted.length() <= 5) {
            expiryEdit->setText(formatted);
        }
    });
}

void PaymentDialog::setupStyling() {
    setStyleSheet(R"(
        QDialog {
            background-color: #2d2d2d;
            color: #ffffff;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 15px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px 0 8px;
            color: #00bcd4;
            font-size: 14px;
        }
        
        QLineEdit, QComboBox {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 6px;
            padding: 10px;
            font-size: 12px;
            color: #ffffff;
        }
        
        QLineEdit:focus, QComboBox:focus {
            border-color: #00bcd4;
        }
        
        QPushButton {
            background-color: #00bcd4;
            color: white;
            border: none;
            padding: 12px 24px;
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #26c6da;
        }
        
        QPushButton:pressed {
            background-color: #0097a7;
        }
        
        QPushButton:disabled {
            background-color: #666;
            color: #999;
        }
        
        QPushButton#cancelButton {
            background-color: #666;
        }
        
        QPushButton#cancelButton:hover {
            background-color: #777;
        }
        
        QProgressBar {
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: white;
            background-color: #3c3c3c;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00bcd4, stop:1 #26c6da);
            border-radius: 3px;
        }
    )");
    
    cancelButton->setObjectName("cancelButton");
}

bool PaymentDialog::validateCardDetails() {
    if (nameEdit->text().trimmed().isEmpty()) {
        statusLabel->setText("Please enter your full name");
        statusLabel->setStyleSheet("color: #f44336;");
        return false;
    }
    
    if (emailEdit->text().trimmed().isEmpty() || !emailEdit->text().contains("@")) {
        statusLabel->setText("Please enter a valid email address");
        statusLabel->setStyleSheet("color: #f44336;");
        return false;
    }
    
    QString cardNumber = cardNumberEdit->text().remove(" ");
    if (cardNumber.length() != 16) {
        statusLabel->setText("Please enter a valid 16-digit card number");
        statusLabel->setStyleSheet("color: #f44336;");
        return false;
    }
    
    if (expiryEdit->text().length() != 5) {
        statusLabel->setText("Please enter expiry date in MM/YY format");
        statusLabel->setStyleSheet("color: #f44336;");
        return false;
    }
    
    if (cvcEdit->text().length() < 3) {
        statusLabel->setText("Please enter a valid CVC code");
        statusLabel->setStyleSheet("color: #f44336;");
        return false;
    }
    
    return true;
}

void PaymentDialog::processPayment() {
    if (!validateCardDetails()) {
        return;
    }
    
    payButton->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setRange(0, 0); // Indeterminate
    statusLabel->setText("Creating customer...");
    statusLabel->setStyleSheet("color: #2196f3;");
    
    // Create customer first
    stripeManager->createCustomer(emailEdit->text().trimmed(), nameEdit->text().trimmed());
}

void PaymentDialog::onCustomerCreated(const QJsonObject &customer) {
    customerId = customer["id"].toString();
    statusLabel->setText("Processing payment...");
    
    // Create subscription
    stripeManager->createSubscription(customerId, priceId);
}

void PaymentDialog::onSubscriptionCreated(const QJsonObject &subscription) {
    progressBar->setVisible(false);
    statusLabel->setText("Payment successful! Subscription activated.");
    statusLabel->setStyleSheet("color: #4caf50;");
    
    payButton->setText("Complete");
    payButton->setEnabled(true);
    
    // Auto-close after 2 seconds
    QTimer::singleShot(2000, this, [this, subscription]() {
        emit paymentCompleted(subscription["id"].toString());
        accept();
    });
}

void PaymentDialog::onPaymentError(const QString &error) {
    progressBar->setVisible(false);
    statusLabel->setText("Payment failed: " + error);
    statusLabel->setStyleSheet("color: #f44336;");
    payButton->setEnabled(true);
}