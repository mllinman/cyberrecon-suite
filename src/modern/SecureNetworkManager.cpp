#include "SecureNetworkManager.h"
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QSslSocket>
#include <QNetworkProxy>
#include <QRandomGenerator>
#include <QMutexLocker>
#include <QMutex>

Q_LOGGING_CATEGORY(networkManager, "cyberrecon.network")

namespace CyberRecon {

SecureNetworkManager* SecureNetworkManager::m_instance = nullptr;

SecureNetworkManager* SecureNetworkManager::instance() {
    static QMutex instanceMutex;
    QMutexLocker locker(&instanceMutex);
    
    if (!m_instance) {
        m_instance = new SecureNetworkManager();
    }
    return m_instance;
}

SecureNetworkManager::SecureNetworkManager(QObject* parent)
    : QObject(parent)
    , m_networkManager(std::make_unique<QNetworkAccessManager>(this))
    , m_globalTimeout(30000)
    , m_maxConcurrentRequests(10)
    , m_rateLimitRequestsPerSecond(10)
    , m_requestsThisSecond(0)
    , m_totalRequests(0)
    , m_totalResponseTime(0)
    , m_startTime(QDateTime::currentDateTime())
{
    // Configure network manager for security
    m_networkManager->setStrictTransportSecurityEnabled(true);
    m_networkManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    
    // Set up rate limiting timer
    m_rateLimitTimer = new QTimer(this);
    connect(m_rateLimitTimer, &QTimer::timeout, this, &SecureNetworkManager::processRateLimiting);
    m_rateLimitTimer->start(100); // Process queue every 100ms
    
    qCInfo(networkManager) << "SecureNetworkManager initialized";
}

SecureNetworkManager::~SecureNetworkManager() {
    // Cancel all pending requests
    for (auto it = m_activeRequests.begin(); it != m_activeRequests.end(); ++it) {
        it.key()->abort();
    }
    
    qCInfo(networkManager) << "SecureNetworkManager destroyed";
}

void SecureNetworkManager::requestAsync(RequestType type, const QString& url, ResponseCallback callback, const RequestOptions& options) {
    requestAsync(type, url, QByteArray(), callback, options);
}

void SecureNetworkManager::requestAsync(RequestType type, const QString& url, const QByteArray& data, ResponseCallback callback, const RequestOptions& options) {
    QString requestId = generateRequestId();
    
    PendingRequest pendingRequest;
    pendingRequest.requestId = requestId;
    pendingRequest.type = type;
    pendingRequest.url = url;
    pendingRequest.data = data;
    pendingRequest.options = options;
    pendingRequest.callback = callback;
    pendingRequest.startTime = QDateTime::currentDateTime();
    
    m_pendingRequests[requestId] = pendingRequest;
    
    // Add to rate limiting queue
    m_requestQueue.enqueue(requestId);
    
    qCDebug(networkManager) << "Queued request" << requestId << ":" << url;
}

void SecureNetworkManager::requestAsync(RequestType type, const QString& url, const QJsonObject& jsonData, ResponseCallback callback, const RequestOptions& options) {
    QJsonDocument doc(jsonData);
    requestAsync(type, url, doc.toJson(QJsonDocument::Compact), callback, options);
}

Result<SecureNetworkManager::Response> SecureNetworkManager::requestSync(RequestType type, const QString& url, const RequestOptions& options) {
    return requestSync(type, url, QByteArray(), options);
}

Result<SecureNetworkManager::Response> SecureNetworkManager::requestSync(RequestType type, const QString& url, const QByteArray& data, const RequestOptions& options) {
    qCWarning(networkManager) << "Using synchronous request - consider using async version for better performance";
    
    QEventLoop loop;
    Result<Response> result = Result<Response>::error(ErrorCode::NetworkError, "Request not completed");
    
    requestAsync(type, url, data, [&result, &loop](const Result<Response>& response) {
        result = response;
        loop.quit();
    }, options);
    
    // Set a timeout for the event loop
    QTimer::singleShot(options.timeoutMs + 5000, &loop, &QEventLoop::quit);
    loop.exec();
    
    return result;
}

void SecureNetworkManager::setGlobalTimeout(int timeoutMs) {
    m_globalTimeout = qMax(1000, timeoutMs);
    qCInfo(networkManager) << "Global timeout set to" << m_globalTimeout << "ms";
}

void SecureNetworkManager::setMaxConcurrentRequests(int maxRequests) {
    m_maxConcurrentRequests = qMax(1, maxRequests);
    qCInfo(networkManager) << "Max concurrent requests set to" << m_maxConcurrentRequests;
}

void SecureNetworkManager::enableRateLimiting(int requestsPerSecond) {
    m_rateLimitRequestsPerSecond = qMax(1, requestsPerSecond);
    qCInfo(networkManager) << "Rate limiting set to" << m_rateLimitRequestsPerSecond << "requests per second";
}

void SecureNetworkManager::addTrustedCertificate(const QSslCertificate& certificate) {
    m_trustedCertificates.append(certificate);
    qCInfo(networkManager) << "Added trusted certificate:" << certificate.subjectInfo(QSslCertificate::CommonName);
}

int SecureNetworkManager::getActiveRequestCount() const {
    return m_activeRequests.size();
}

qint64 SecureNetworkManager::getTotalRequestCount() const {
    return m_totalRequests;
}

double SecureNetworkManager::getAverageResponseTime() const {
    if (m_totalRequests == 0) {
        return 0.0;
    }
    return static_cast<double>(m_totalResponseTime) / m_totalRequests;
}

void SecureNetworkManager::generateNetworkReport() {
    QString report = QString("Network Manager Performance Report:\n"
                           "- Active requests: %1/%2\n"
                           "- Queued requests: %3\n"
                           "- Total requests: %4\n"
                           "- Average response time: %5ms\n"
                           "- Rate limit: %6 req/sec\n"
                           "- Uptime: %7 minutes")
                    .arg(getActiveRequestCount())
                    .arg(m_maxConcurrentRequests)
                    .arg(m_requestQueue.size())
                    .arg(getTotalRequestCount())
                    .arg(getAverageResponseTime(), 0, 'f', 2)
                    .arg(m_rateLimitRequestsPerSecond)
                    .arg(m_startTime.secsTo(QDateTime::currentDateTime()) / 60);
    
    qCInfo(networkManager) << report;
}

QString SecureNetworkManager::generateRequestId() {
    return QString("req_%1_%2")
           .arg(QDateTime::currentMSecsSinceEpoch())
           .arg(QRandomGenerator::global()->bounded(1000, 9999));
}

QNetworkRequest SecureNetworkManager::createRequest(const QString& url, const RequestOptions& options) {
    QNetworkRequest request(QUrl(url));
    
    // Set headers
    for (auto it = options.headers.begin(); it != options.headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toString().toUtf8());
    }
    
    // Set user agent
    request.setHeader(QNetworkRequest::UserAgentHeader, options.userAgent);
    
    // Security headers
    request.setRawHeader("X-Requested-With", "CyberRecon-Suite");
    request.setRawHeader("Cache-Control", "no-cache");
    
    // Set redirect policy
    if (options.followRedirects) {
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, 
                           QNetworkRequest::SameOriginRedirectPolicy);
    } else {
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, 
                           QNetworkRequest::ManualRedirectPolicy);
    }
    
    // Set timeout
    request.setTransferTimeout(options.timeoutMs);
    
    setupSSLConfiguration(request, options);
    
    return request;
}

void SecureNetworkManager::setupSSLConfiguration(QNetworkRequest& request, const RequestOptions& options) {
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    
    if (options.verifySSL) {
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
        sslConfig.setPeerVerifyDepth(3);
        
        // Use only secure protocols
        sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
        
        // Set secure cipher suites
        QList<QSslCipher> secureCiphers;
        for (const QSslCipher& cipher : QSslConfiguration::supportedCiphers()) {
            if (cipher.usedBits() >= 256 && 
                !cipher.name().contains("RC4") && 
                !cipher.name().contains("MD5") &&
                !cipher.name().contains("DES")) {
                secureCiphers.append(cipher);
            }
        }
        sslConfig.setCiphers(secureCiphers);
        
        // Add trusted certificates
        if (!m_trustedCertificates.isEmpty()) {
            sslConfig.setCaCertificates(sslConfig.caCertificates() + m_trustedCertificates);
        }
    } else {
        qCWarning(networkManager) << "SSL verification disabled - use only for testing!";
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    }
    
    request.setSslConfiguration(sslConfig);
}

void SecureNetworkManager::processRateLimiting() {
    // Reset counter every second
    QDateTime now = QDateTime::currentDateTime();
    if (m_lastRequestTime.secsTo(now) >= 1) {
        m_requestsThisSecond = 0;
        m_lastRequestTime = now;
    }
    
    // Process requests within rate limit
    while (!m_requestQueue.isEmpty() && 
           m_requestsThisSecond < m_rateLimitRequestsPerSecond &&
           m_activeRequests.size() < m_maxConcurrentRequests) {
        
        QString requestId = m_requestQueue.dequeue();
        
        if (!m_pendingRequests.contains(requestId)) {
            continue; // Request was cancelled
        }
        
        PendingRequest& pendingRequest = m_pendingRequests[requestId];
        
        QNetworkRequest request = createRequest(pendingRequest.url, pendingRequest.options);
        QNetworkReply* reply = nullptr;
        
        switch (pendingRequest.type) {
            case RequestType::GET:
                reply = m_networkManager->get(request);
                break;
            case RequestType::POST:
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                reply = m_networkManager->post(request, pendingRequest.data);
                break;
            case RequestType::PUT:
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                reply = m_networkManager->put(request, pendingRequest.data);
                break;
            case RequestType::DELETE:
                reply = m_networkManager->deleteResource(request);
                break;
            case RequestType::PATCH:
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                reply = m_networkManager->sendCustomRequest(request, "PATCH", pendingRequest.data);
                break;
        }
        
        if (reply) {
            m_activeRequests[reply] = requestId;
            
            connect(reply, &QNetworkReply::finished, this, &SecureNetworkManager::onRequestFinished);
            
            // Set up timeout
            QTimer* timeoutTimer = new QTimer(this);
            timeoutTimer->setSingleShot(true);
            connect(timeoutTimer, &QTimer::timeout, [this, reply]() {
                if (m_activeRequests.contains(reply)) {
                    reply->abort();
                }
            });
            timeoutTimer->start(pendingRequest.options.timeoutMs);
            
            m_requestsThisSecond++;
            m_totalRequests++;
            
            emit requestStarted(requestId, pendingRequest.url);
            
            qCDebug(networkManager) << "Started request" << requestId << ":" << pendingRequest.url;
        }
    }
    
    // Check for performance alerts
    if (m_requestQueue.size() > 50) {
        emit performanceAlert(QString("High network queue: %1 requests pending").arg(m_requestQueue.size()));
    }
    
    if (getActiveRequestCount() == m_maxConcurrentRequests) {
        emit performanceAlert("Maximum concurrent network requests reached");
    }
}

void SecureNetworkManager::onRequestFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    QString requestId = m_activeRequests.value(reply);
    if (requestId.isEmpty()) {
        reply->deleteLater();
        return;
    }
    
    m_activeRequests.remove(reply);
    
    if (!m_pendingRequests.contains(requestId)) {
        reply->deleteLater();
        return;
    }
    
    PendingRequest pendingRequest = m_pendingRequests[requestId];
    m_pendingRequests.remove(requestId);
    
    handleResponse(reply, requestId, pendingRequest.callback);
    reply->deleteLater();
}

void SecureNetworkManager::handleResponse(QNetworkReply* reply, const QString& requestId, ResponseCallback callback) {
    QDateTime endTime = QDateTime::currentDateTime();
    qint64 requestTime = m_pendingRequests.contains(requestId) ? 
                        m_pendingRequests[requestId].startTime.msecsTo(endTime) : 0;
    
    m_totalResponseTime += requestTime;
    
    Response response;
    response.statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    response.data = reply->readAll();
    response.requestTime = requestTime;
    
    // Extract headers
    for (const auto& header : reply->rawHeaderPairs()) {
        response.headers[QString::fromUtf8(header.first)] = QString::fromUtf8(header.second);
    }
    
    if (reply->error() == QNetworkReply::NoError) {
        qCDebug(networkManager) << "Request" << requestId << "completed successfully in" << requestTime << "ms";
        callback(Result<Response>::success(response));
        emit requestCompleted(requestId, Result<Response>::success(response));
    } else {
        response.error = reply->errorString();
        
        // Check if we should retry
        if (m_pendingRequests.contains(requestId)) {
            PendingRequest& pendingRequest = m_pendingRequests[requestId];
            if (pendingRequest.retryCount < pendingRequest.options.maxRetries) {
                qCDebug(networkManager) << "Retrying request" << requestId << "- attempt" << (pendingRequest.retryCount + 1);
                retryRequest(requestId, pendingRequest.retryCount + 1);
                return;
            }
        }
        
        ErrorCode errorCode = ErrorCode::NetworkError;
        QString errorMessage = QString("Network request failed: %1").arg(reply->errorString());
        
        qCWarning(networkManager) << "Request" << requestId << "failed:" << errorMessage;
        
        Result<Response> errorResult = Result<Response>::error(errorCode, errorMessage);
        callback(errorResult);
        emit requestCompleted(requestId, errorResult);
    }
}

void SecureNetworkManager::retryRequest(const QString& originalRequestId, int currentRetry) {
    if (!m_pendingRequests.contains(originalRequestId)) {
        return;
    }
    
    PendingRequest originalRequest = m_pendingRequests[originalRequestId];
    originalRequest.retryCount = currentRetry;
    
    // Exponential backoff
    int delay = originalRequest.options.retryDelayMs * (1 << (currentRetry - 1));
    
    QTimer::singleShot(delay, this, [this, originalRequest]() {
        m_pendingRequests[originalRequest.requestId] = originalRequest;
        m_requestQueue.enqueue(originalRequest.requestId);
    });
}

QJsonDocument SecureNetworkManager::Response::jsonDocument() const {
    return QJsonDocument::fromJson(data);
}

QJsonObject SecureNetworkManager::Response::jsonObject() const {
    return jsonDocument().object();
}

// Convenience functions
Result<SecureNetworkManager::Response> httpGet(const QString& url, const SecureNetworkManager::RequestOptions& options) {
    return SecureNetworkManager::instance()->requestSync(SecureNetworkManager::RequestType::GET, url, options);
}

Result<SecureNetworkManager::Response> httpPost(const QString& url, const QJsonObject& data, const SecureNetworkManager::RequestOptions& options) {
    QJsonDocument doc(data);
    return SecureNetworkManager::instance()->requestSync(SecureNetworkManager::RequestType::POST, url, doc.toJson(), options);
}

Result<SecureNetworkManager::Response> httpPost(const QString& url, const QByteArray& data, const SecureNetworkManager::RequestOptions& options) {
    return SecureNetworkManager::instance()->requestSync(SecureNetworkManager::RequestType::POST, url, data, options);
}

void httpGetAsync(const QString& url, SecureNetworkManager::ResponseCallback callback, const SecureNetworkManager::RequestOptions& options) {
    SecureNetworkManager::instance()->requestAsync(SecureNetworkManager::RequestType::GET, url, callback, options);
}

void httpPostAsync(const QString& url, const QJsonObject& data, SecureNetworkManager::ResponseCallback callback, const SecureNetworkManager::RequestOptions& options) {
    SecureNetworkManager::instance()->requestAsync(SecureNetworkManager::RequestType::POST, url, data, callback, options);
}

} // namespace CyberRecon