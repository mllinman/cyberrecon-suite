#include "StripeManager.h"
#include <QNetworkRequest>
#include <QJsonArray>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QDebug>

StripeManager::StripeManager(QObject *parent) 
    : QObject(parent), stripeBaseUrl("https://api.stripe.com/v1") {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, 
            this, &StripeManager::handleNetworkReply);
}

void StripeManager::setApiKey(const QString &apiKey) {
    stripeApiKey = apiKey;
}

void StripeManager::createSubscription(const QString &customerId, const QString &priceId) {
    QJsonObject data;
    data["customer"] = customerId;
    data["items[0][price]"] = priceId;
    data["payment_behavior"] = "default_incomplete";
    data["expand[0]"] = "latest_invoice.payment_intent";
    
    makeStripeRequest("/subscriptions", "POST", data);
}

void StripeManager::cancelSubscription(const QString &subscriptionId) {
    makeStripeRequest(QString("/subscriptions/%1").arg(subscriptionId), "DELETE");
}

void StripeManager::updateSubscription(const QString &subscriptionId, const QString &newPriceId) {
    QJsonObject data;
    data["items[0][price]"] = newPriceId;
    data["proration_behavior"] = "create_prorations";
    
    makeStripeRequest(QString("/subscriptions/%1").arg(subscriptionId), "POST", data);
}

void StripeManager::createCustomer(const QString &email, const QString &name) {
    QJsonObject data;
    data["email"] = email;
    data["name"] = name;
    data["description"] = "CyberRecon Suite Customer";
    
    makeStripeRequest("/customers", "POST", data);
}

void StripeManager::getCustomer(const QString &customerId) {
    makeStripeRequest(QString("/customers/%1").arg(customerId), "GET");
}

void StripeManager::createPaymentMethod(const QString &customerId, const QJsonObject &paymentData) {
    QJsonObject data = paymentData;
    data["customer"] = customerId;
    
    makeStripeRequest("/payment_methods", "POST", data);
}

void StripeManager::attachPaymentMethod(const QString &paymentMethodId, const QString &customerId) {
    QJsonObject data;
    data["customer"] = customerId;
    
    makeStripeRequest(QString("/payment_methods/%1/attach").arg(paymentMethodId), "POST", data);
}

void StripeManager::getProducts() {
    makeStripeRequest("/products", "GET");
}

void StripeManager::getPrices() {
    makeStripeRequest("/prices", "GET");
}

void StripeManager::makeStripeRequest(const QString &endpoint, const QString &method, const QJsonObject &data) {
    if (stripeApiKey.isEmpty()) {
        emit errorOccurred("Stripe API key not set");
        return;
    }
    
    QNetworkRequest request = createStripeRequest(endpoint);
    
    if (method == "GET") {
        networkManager->get(request);
    } else if (method == "POST") {
        // Convert JSON to form data for Stripe API
        QUrlQuery query;
        for (auto it = data.begin(); it != data.end(); ++it) {
            query.addQueryItem(it.key(), it.value().toString());
        }
        
        QByteArray postData = query.toString(QUrl::FullyEncoded).toUtf8();
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        networkManager->post(request, postData);
    } else if (method == "DELETE") {
        networkManager->deleteResource(request);
    }
}

QNetworkRequest StripeManager::createStripeRequest(const QString &endpoint) {
    QNetworkRequest request;
    request.setUrl(QUrl(stripeBaseUrl + endpoint));
    
    // Set authorization header
    QString auth = QString("Bearer %1").arg(stripeApiKey);
    request.setRawHeader("Authorization", auth.toUtf8());
    request.setRawHeader("Stripe-Version", "2023-10-16");
    
    return request;
}

void StripeManager::handleNetworkReply(QNetworkReply *reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject response = doc.object();
    
    if (reply->error() != QNetworkReply::NoError) {
        QString error = response["error"].toObject()["message"].toString();
        if (error.isEmpty()) {
            error = reply->errorString();
        }
        emit errorOccurred(error);
        reply->deleteLater();
        return;
    }
    
    QString url = reply->url().toString();
    
    if (url.contains("/subscriptions")) {
        if (reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString() == "DELETE") {
            emit subscriptionCancelled(response["id"].toString());
        } else {
            emit subscriptionCreated(response);
        }
    } else if (url.contains("/customers")) {
        emit customerCreated(response);
    } else if (url.contains("/payment_methods")) {
        emit paymentMethodCreated(response);
    } else if (url.contains("/products")) {
        emit productsReceived(response);
    } else if (url.contains("/prices")) {
        emit pricesReceived(response);
    }
    
    reply->deleteLater();
}