#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class StripeManager : public QObject {
    Q_OBJECT

public:
    explicit StripeManager(QObject *parent = nullptr);
    void setApiKey(const QString &apiKey);
    
    // Subscription management
    void createSubscription(const QString &customerId, const QString &priceId);
    void cancelSubscription(const QString &subscriptionId);
    void updateSubscription(const QString &subscriptionId, const QString &newPriceId);
    
    // Customer management
    void createCustomer(const QString &email, const QString &name);
    void getCustomer(const QString &customerId);
    
    // Payment methods
    void createPaymentMethod(const QString &customerId, const QJsonObject &paymentData);
    void attachPaymentMethod(const QString &paymentMethodId, const QString &customerId);
    
    // Products and prices
    void getProducts();
    void getPrices();

signals:
    void subscriptionCreated(const QJsonObject &subscription);
    void subscriptionCancelled(const QString &subscriptionId);
    void customerCreated(const QJsonObject &customer);
    void paymentMethodCreated(const QJsonObject &paymentMethod);
    void productsReceived(const QJsonObject &products);
    void pricesReceived(const QJsonObject &prices);
    void errorOccurred(const QString &error);

private slots:
    void handleNetworkReply(QNetworkReply *reply);

private:
    void makeStripeRequest(const QString &endpoint, const QString &method, const QJsonObject &data = QJsonObject());
    QNetworkRequest createStripeRequest(const QString &endpoint);
    
    QNetworkAccessManager *networkManager;
    QString stripeApiKey;
    QString stripeBaseUrl;
};