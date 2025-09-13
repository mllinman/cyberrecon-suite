#pragma once
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QLoggingCategory>
#include <functional>
#include "modern/ErrorHandling.h"

Q_DECLARE_LOGGING_CATEGORY(inputValidator)

namespace CyberRecon {

/**
 * @brief Comprehensive input validation system with security-focused rules
 */
class InputValidator {
public:
    enum class ValidationRule {
        None,
        AlphaNumeric,
        Numeric,
        Email,
        Username,
        Password,
        IPAddress,
        URL,
        SqlSafe,
        XssSafe,
        FilePath,
        DatabaseName,
        Custom
    };

    struct ValidationOptions {
        int minLength = 0;
        int maxLength = 1000;
        bool allowEmpty = false;
        bool trimWhitespace = true;
        QString customRegex;
        QStringList allowedValues;
        QStringList blockedValues;
        std::function<bool(const QString&)> customValidator;
        
        ValidationOptions() = default;
    };

    struct ValidationResult {
        bool isValid = false;
        QString sanitizedValue;
        QString errorMessage;
        QStringList warnings;
        
        ValidationResult() = default;
        ValidationResult(bool valid, const QString& value = "", const QString& error = "")
            : isValid(valid), sanitizedValue(value), errorMessage(error) {}
    };

    // Single value validation
    static ValidationResult validate(const QString& input, ValidationRule rule, const ValidationOptions& options = {});
    static ValidationResult validateUsername(const QString& username);
    static ValidationResult validatePassword(const QString& password);
    static ValidationResult validateEmail(const QString& email);
    static ValidationResult validateIPAddress(const QString& ip);
    static ValidationResult validateURL(const QString& url);
    static ValidationResult validateFilePath(const QString& path);
    
    // Bulk validation
    static QHash<QString, ValidationResult> validateFields(const QHash<QString, QString>& fields, 
                                                           const QHash<QString, ValidationRule>& rules);
    static ValidationResult validateJson(const QJsonObject& json, const QJsonObject& schema);
    
    // Security-specific validation
    static bool containsSqlInjection(const QString& input);
    static bool containsXssAttempt(const QString& input);
    static bool containsPathTraversal(const QString& input);
    static bool containsCommandInjection(const QString& input);
    static QString sanitizeForSql(const QString& input);
    static QString sanitizeForHtml(const QString& input);
    static QString sanitizeForFilePath(const QString& input);
    
    // Rate limiting and abuse detection
    static bool isRateLimited(const QString& identifier);
    static void recordValidationAttempt(const QString& identifier, bool success);
    static void clearRateLimit(const QString& identifier);
    
    // Custom rule registration
    static void registerCustomRule(const QString& ruleName, std::function<ValidationResult(const QString&, const ValidationOptions&)> validator);
    
private:
    static ValidationResult applyRule(const QString& input, ValidationRule rule, const ValidationOptions& options);
    static QString applyBasicSanitization(const QString& input, const ValidationOptions& options);
    static bool matchesRegex(const QString& input, const QString& pattern);
    static void logSecurityAttempt(const QString& input, const QString& threat);
    
    // Threat patterns
    static const QStringList SQL_INJECTION_PATTERNS;
    static const QStringList XSS_PATTERNS;
    static const QStringList PATH_TRAVERSAL_PATTERNS;
    static const QStringList COMMAND_INJECTION_PATTERNS;
    
    // Rate limiting storage
    static QHash<QString, QList<QDateTime>> s_validationAttempts;
    static QMutex s_rateLimitMutex;
    
    // Custom validators
    static QHash<QString, std::function<ValidationResult(const QString&, const ValidationOptions&)>> s_customValidators;
};

/**
 * @brief Form validation helper for UI components
 */
class FormValidator {
public:
    struct FieldRule {
        InputValidator::ValidationRule rule;
        InputValidator::ValidationOptions options;
        QString errorMessage;
        bool required = true;
        
        FieldRule() : rule(InputValidator::ValidationRule::None) {}
        FieldRule(InputValidator::ValidationRule r, bool req = true) 
            : rule(r), required(req) {}
    };

    FormValidator() = default;
    
    void addField(const QString& fieldName, const FieldRule& rule);
    void addField(const QString& fieldName, InputValidator::ValidationRule rule, bool required = true);
    
    Result<QHash<QString, QString>> validate(const QHash<QString, QString>& formData);
    QStringList getFieldErrors(const QString& fieldName) const;
    bool isFormValid() const;
    void clearErrors();
    
private:
    QHash<QString, FieldRule> m_rules;
    QHash<QString, QStringList> m_fieldErrors;
    bool m_lastValidationResult = false;
};

/**
 * @brief RAII validation context for better error tracking
 */
class ValidationContext {
public:
    explicit ValidationContext(const QString& context);
    ~ValidationContext();
    
    static QString getCurrentContext();
    void addWarning(const QString& warning);
    QStringList getWarnings() const;
    
private:
    static thread_local std::vector<ValidationContext*> s_contextStack;
    QString m_context;
    QStringList m_warnings;
};

// Convenience macros
#define VALIDATE_INPUT(input, rule) InputValidator::validate(input, InputValidator::ValidationRule::rule)
#define VALIDATE_USERNAME(input) InputValidator::validateUsername(input)
#define VALIDATE_PASSWORD(input) InputValidator::validatePassword(input)
#define VALIDATE_EMAIL(input) InputValidator::validateEmail(input)

#define VALIDATION_CONTEXT(name) ValidationContext __validation_ctx(name)

} // namespace CyberRecon