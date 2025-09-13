#include "ErrorHandling.h"
#include <QDebug>

Q_LOGGING_CATEGORY(errorManager, "cyberrecon.error")

namespace CyberRecon {

thread_local std::vector<QString> ErrorContext::s_contextStack;

QString ErrorInfo::toString() const {
    QString result = QString("[%1] %2")
                    .arg(static_cast<int>(code))
                    .arg(message);
    
    if (!context.isEmpty()) {
        result += QString(" (Context: %1)").arg(context);
    }
    
    if (!suggestion.isEmpty()) {
        result += QString(" - Suggestion: %1").arg(suggestion);
    }
    
    return result;
}

ErrorContext::ErrorContext(const QString& context) {
    s_contextStack.push_back(context);
    qCDebug(errorManager) << "Entering error context:" << context;
}

ErrorContext::~ErrorContext() {
    if (!s_contextStack.empty()) {
        QString context = s_contextStack.back();
        s_contextStack.pop_back();
        qCDebug(errorManager) << "Leaving error context:" << context;
    }
}

QString ErrorContext::getCurrentContext() {
    if (s_contextStack.empty()) {
        return QString();
    }
    
    QStringList contexts;
    for (const QString& ctx : s_contextStack) {
        contexts.append(ctx);
    }
    
    return contexts.join(" -> ");
}

void ErrorContext::logError(const ErrorInfo& error) {
    QString fullContext = getCurrentContext();
    QString logMessage = error.toString();
    
    if (!fullContext.isEmpty()) {
        logMessage = QString("[%1] %2").arg(fullContext, logMessage);
    }
    
    switch (error.code) {
        case ErrorCode::Success:
            // Should not log success as error
            break;
        case ErrorCode::SecurityError:
            qCCritical(errorManager) << "SECURITY:" << logMessage;
            break;
        case ErrorCode::DatabaseError:
        case ErrorCode::NetworkError:
        case ErrorCode::FileSystemError:
            qCCritical(errorManager) << logMessage;
            break;
        case ErrorCode::AuthenticationError:
        case ErrorCode::ValidationError:
            qCWarning(errorManager) << logMessage;
            break;
        case ErrorCode::PerformanceError:
            qCInfo(errorManager) << "PERFORMANCE:" << logMessage;
            break;
        case ErrorCode::UnknownError:
        default:
            qCCritical(errorManager) << "UNKNOWN:" << logMessage;
            break;
    }
}

} // namespace CyberRecon