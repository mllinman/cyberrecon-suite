#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QJsonObject>

class StripeManager;

class PaymentDialog : public QDialog {
    Q_OBJECT

public:
    explicit PaymentDialog(const QString &planName, const QString &priceId, 
                          double amount, QWidget *parent = nullptr);

signals:
    void paymentCompleted(const QString &subscriptionId);

private slots:
    void processPayment();
    void onSubscriptionCreated(const QJsonObject &subscription);
    void onCustomerCreated(const QJsonObject &customer);
    void onPaymentError(const QString &error);

private:
    void setupUI();
    void setupStyling();
    bool validateCardDetails();
    
    // UI Components
    QLineEdit *cardNumberEdit;
    QLineEdit *expiryEdit;
    QLineEdit *cvcEdit;
    QLineEdit *nameEdit;
    QLineEdit *emailEdit;
    QComboBox *countryCombo;
    QPushButton *payButton;
    QPushButton *cancelButton;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QLabel *planLabel;
    QLabel *amountLabel;
    
    // Data
    QString planName;
    QString priceId;
    double amount;
    QString customerId;
    
    StripeManager *stripeManager;
};