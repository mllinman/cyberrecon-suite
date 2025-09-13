#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <memory>
#include <functional>
#include "ErrorHandling.h"

Q_DECLARE_LOGGING_CATEGORY(networkManager)

namespace CyberRecon {

/**
 * @brief Modern secure network manager with retry logic, rate limiting, and security controls
 */
class SecureNetworkManager : public QObject {
    Q_OBJECT

public:
    enum class RequestType {
        GET,
        POST,
        PUT,
        DELETE,
        PATCH
    };

    struct RequestOptions {
        int timeoutMs = 30000;
        int maxRetries = 3;
        int retryDelayMs = 1000;
        bool followRedirects = false;
        bool verifySSL = true;
        QVariantMap headers;
        QString userAgent = "CyberRecon-Suite/1.7.0";
        
        RequestOptions() = default;
    };

    struct Response {
        int statusCode = 0;
        QByteArray data;
        QVariantMap headers;
        QString error;
        qint64 requestTime = 0;
        
        bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
        QJsonDocument jsonDocument() const;
        QJsonObject jsonObject() const;
    };

    using ResponseCallback = std::function<void(const Result<Response>&)>;

    static SecureNetworkManager* instance();
    
    // Async request methods
    void request(RequestType type, const QString& url, const RequestOptions& options = {});
    void request(RequestType type, const QString& url, const QByteArray& data, const RequestOptions& options = {});
    void request(RequestType type, const QString& url, const QJsonObject& jsonData, const RequestOptions& options = {});
    
    // Async request methods with callbacks
    void requestAsync(RequestType type, const QString& url, ResponseCallback callback, const RequestOptions& options = {});
    void requestAsync(RequestType type, const QString& url, const QByteArray& data, ResponseCallback callback, const RequestOptions& options = {});
    void requestAsync(RequestType type, const QString& url, const QJsonObject& jsonData, ResponseCallback callback, const RequestOptions& options = {});
    
    // Synchronous methods (use sparingly)
    Result<Response> requestSync(RequestType type, const QString& url, const RequestOptions& options = {});
    Result<Response> requestSync(RequestType type, const QString& url, const QByteArray& data, const RequestOptions& options = {});
    
    // Security and performance configuration
    void setGlobalTimeout(int timeoutMs);
    void setMaxConcurrentRequests(int maxRequests);
    void enableRateLimiting(int requestsPerSecond);
    void addTrustedCertificate(const QSslCertificate& certificate);
    void setProxyConfiguration(const QNetworkProxy& proxy);
    
    // Monitoring and metrics
    int getActiveRequestCount() const;
    qint64 getTotalRequestCount() const;
    double getAverageResponseTime() const;
    void generateNetworkReport();

signals:
    void requestCompleted(const QString& requestId, const Result<Response>& response);
    void requestStarted(const QString& requestId, const QString& url);
    void networkError(const QString& error);
    void performanceAlert(const QString& message);

private slots:
    void onRequestFinished();
    void onRequestTimeout();
    void processRateLimiting();

private:
    explicit SecureNetworkManager(QObject* parent = nullptr);
    ~SecureNetworkManager();
    
    QString generateRequestId();
    QNetworkRequest createRequest(const QString& url, const RequestOptions& options);
    void setupSSLConfiguration(QNetworkRequest& request, const RequestOptions& options);
    void handleResponse(QNetworkReply* reply, const QString& requestId, ResponseCallback callback);
    void retryRequest(const QString& originalRequestId, int currentRetry);
    
    struct PendingRequest {
        QString requestId;
        RequestType type;
        QString url;
        QByteArray data;
        RequestOptions options;
        ResponseCallback callback;
        QDateTime startTime;
        int retryCount;
        
        PendingRequest() : type(RequestType::GET), retryCount(0) {}
    };
    
    static SecureNetworkManager* m_instance;
    
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    QHash<QNetworkReply*, QString> m_activeRequests;
    QHash<QString, PendingRequest> m_pendingRequests;
    QTimer* m_rateLimitTimer;
    
    // Configuration
    int m_globalTimeout;
    int m_maxConcurrentRequests;
    int m_rateLimitRequestsPerSecond;
    QList<QSslCertificate> m_trustedCertificates;
    
    // Rate limiting
    QQueue<QString> m_requestQueue;
    QDateTime m_lastRequestTime;
    int m_requestsThisSecond;
    
    // Metrics
    qint64 m_totalRequests;
    qint64 m_totalResponseTime;
    QDateTime m_startTime;
};

// Convenience functions
Result<SecureNetworkManager::Response> httpGet(const QString& url, const SecureNetworkManager::RequestOptions& options = {});
Result<SecureNetworkManager::Response> httpPost(const QString& url, const QJsonObject& data, const SecureNetworkManager::RequestOptions& options = {});
Result<SecureNetworkManager::Response> httpPost(const QString& url, const QByteArray& data, const SecureNetworkManager::RequestOptions& options = {});

void httpGetAsync(const QString& url, SecureNetworkManager::ResponseCallback callback, const SecureNetworkManager::RequestOptions& options = {});
void httpPostAsync(const QString& url, const QJsonObject& data, SecureNetworkManager::ResponseCallback callback, const SecureNetworkManager::RequestOptions& options = {});

} // namespace CyberRecon