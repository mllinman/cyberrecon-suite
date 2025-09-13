#pragma once
#include <optional>
#include <string>
#include <exception>
#include <memory>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(errorManager)

/**
 * @brief Modern error handling system with std::optional and structured error reporting
 */
namespace CyberRecon {

enum class ErrorCode {
    Success = 0,
    DatabaseError,
    NetworkError,
    AuthenticationError,
    ValidationError,
    FileSystemError,
    SecurityError,
    PerformanceError,
    UnknownError
};

/**
 * @brief Structured error information
 */
struct ErrorInfo {
    ErrorCode code;
    QString message;
    QString context;
    QString suggestion;
    QDateTime timestamp;
    
    ErrorInfo() : code(ErrorCode::Success), timestamp(QDateTime::currentDateTime()) {}
    
    ErrorInfo(ErrorCode c, const QString &msg, const QString &ctx = "", const QString &sug = "")
        : code(c), message(msg), context(ctx), suggestion(sug), timestamp(QDateTime::currentDateTime()) {}
    
    bool isSuccess() const { return code == ErrorCode::Success; }
    QString toString() const;
};

/**
 * @brief Result type that can hold either a value or an error
 */
template<typename T>
class Result {
public:
    // Constructors
    Result(const T& value) : m_value(value) {}
    Result(T&& value) : m_value(std::move(value)) {}
    Result(const ErrorInfo& error) : m_error(error) {}
    Result(ErrorCode code, const QString& message, const QString& context = "")
        : m_error(ErrorInfo{code, message, context}) {}
    
    // Success factory methods
    static Result<T> success(const T& value) { return Result<T>(value); }
    static Result<T> success(T&& value) { return Result<T>(std::move(value)); }
    
    // Error factory methods
    static Result<T> error(ErrorCode code, const QString& message, const QString& context = "") {
        return Result<T>(ErrorInfo{code, message, context});
    }
    
    static Result<T> error(const ErrorInfo& error) {
        return Result<T>(error);
    }
    
    // Check methods
    bool isSuccess() const { return m_value.has_value(); }
    bool isError() const { return !m_value.has_value(); }
    
    // Value access
    const T& value() const { 
        if (!m_value.has_value()) {
            throw std::runtime_error("Attempting to access value of failed Result");
        }
        return m_value.value(); 
    }
    
    T& value() { 
        if (!m_value.has_value()) {
            throw std::runtime_error("Attempting to access value of failed Result");
        }
        return m_value.value(); 
    }
    
    T valueOr(const T& defaultValue) const {
        return m_value.value_or(defaultValue);
    }
    
    // Error access
    const ErrorInfo& error() const { return m_error; }
    
    // Transformation methods
    template<typename F>
    auto map(F&& func) -> Result<decltype(func(std::declval<T>()))> {
        using ReturnType = decltype(func(std::declval<T>()));
        
        if (isSuccess()) {
            try {
                return Result<ReturnType>::success(func(value()));
            } catch (const std::exception& e) {
                return Result<ReturnType>::error(ErrorCode::UnknownError, 
                                               QString::fromStdString(e.what()));
            }
        } else {
            return Result<ReturnType>::error(m_error);
        }
    }
    
    template<typename F>
    auto flatMap(F&& func) -> decltype(func(std::declval<T>())) {
        if (isSuccess()) {
            return func(value());
        } else {
            using ReturnType = decltype(func(std::declval<T>()));
            return ReturnType::error(m_error);
        }
    }
    
private:
    std::optional<T> m_value;
    ErrorInfo m_error;
};

// Specialization for void operations
template<>
class Result<void> {
public:
    Result() = default;
    Result(const ErrorInfo& error) : m_error(error) {}
    Result(ErrorCode code, const QString& message, const QString& context = "")
        : m_error(ErrorInfo{code, message, context}) {}
    
    static Result<void> success() { return Result<void>(); }
    static Result<void> error(ErrorCode code, const QString& message, const QString& context = "") {
        return Result<void>(ErrorInfo{code, message, context});
    }
    
    bool isSuccess() const { return m_error.code == ErrorCode::Success; }
    bool isError() const { return m_error.code != ErrorCode::Success; }
    
    const ErrorInfo& error() const { return m_error; }
    
private:
    ErrorInfo m_error;
};

/**
 * @brief Exception class for critical errors
 */
class CyberReconException : public std::exception {
public:
    explicit CyberReconException(const ErrorInfo& error) : m_error(error) {
        m_what = m_error.toString().toStdString();
    }
    
    const char* what() const noexcept override {
        return m_what.c_str();
    }
    
    const ErrorInfo& errorInfo() const { return m_error; }
    
private:
    ErrorInfo m_error;
    std::string m_what;
};

/**
 * @brief RAII error context manager for better error reporting
 */
class ErrorContext {
public:
    explicit ErrorContext(const QString& context);
    ~ErrorContext();
    
    static QString getCurrentContext();
    static void logError(const ErrorInfo& error);
    
private:
    static thread_local std::vector<QString> s_contextStack;
};

// Convenience macros
#define CYBERRECON_TRY(expression) \
    do { \
        auto result = (expression); \
        if (result.isError()) { \
            ErrorContext::logError(result.error()); \
            return Result<void>::error(result.error()); \
        } \
    } while(0)

#define CYBERRECON_TRY_RETURN(expression) \
    do { \
        auto result = (expression); \
        if (result.isError()) { \
            ErrorContext::logError(result.error()); \
            return result; \
        } \
        return Result<decltype(result.value())>::success(result.value()); \
    } while(0)

// Helper functions
template<typename T>
Result<T> makeResult(const T& value) {
    return Result<T>::success(value);
}

Result<void> makeSuccessResult() {
    return Result<void>::success();
}

template<typename T>
Result<T> makeErrorResult(ErrorCode code, const QString& message, const QString& context = "") {
    return Result<T>::error(code, message, context);
}

// Type aliases for common patterns
using StringResult = Result<QString>;
using IntResult = Result<int>;
using BoolResult = Result<bool>;
using VoidResult = Result<void>;

} // namespace CyberRecon