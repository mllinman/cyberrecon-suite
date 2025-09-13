#include "InputValidator.h"
#include <QRegularExpression>
#include <QUrl>
#include <QFileInfo>
#include <QHostAddress>
#include <QMutexLocker>
#include <QDebug>
#include <QJsonDocument>

Q_LOGGING_CATEGORY(inputValidator, "cyberrecon.validation")

namespace CyberRecon {

// Static members initialization
const QStringList InputValidator::SQL_INJECTION_PATTERNS = {
    R"(\b(UNION|SELECT|INSERT|UPDATE|DELETE|DROP|CREATE|ALTER|EXEC|EXECUTE)\b)",
    R"((\bOR\b|\bAND\b)\s+\w+\s*=\s*\w+)",
    R"(--|\|\/\*|\*\/)",
    R"(['"]\s*;\s*)",
    R"(\b(INFORMATION_SCHEMA|SYSOBJECTS|SYSCOLUMNS)\b)",
    R"(\b(CHAR|ASCII|SUBSTRING|CONCAT)\s*\()",
    R"(\b(SLEEP|BENCHMARK|WAITFOR)\s*\()"
};

const QStringList InputValidator::XSS_PATTERNS = {
    R"(<script\b[^<]*(?:(?!<\/script>)<[^<]*)*<\/script>)",
    R"(javascript:)",
    R"(on\w+\s*=)",
    R"(<iframe\b)",
    R"(<object\b)",
    R"(<embed\b)",
    R"(<link\b[^>]*href\s*=)",
    R"(expression\s*\()",
    R"(@import)",
    R"(vbscript:)",
    R"(data:text\/html)"
};

const QStringList InputValidator::PATH_TRAVERSAL_PATTERNS = {
    R"(\.\.\/|\.\.\\)",
    R"(%2e%2e%2f|%2e%2e%5c)",
    R"(%c0%ae)",
    R"(%252e)",
    R"(\/\.\.\/|\\\.\.\\)",
    R"(\.\./|\.\.\\"
};

const QStringList InputValidator::COMMAND_INJECTION_PATTERNS = {
    R"([;&|`$(){}[\]])",
    R"(\b(cat|ls|dir|type|echo|cmd|sh|bash|powershell|wget|curl)\b)",
    R"(>|<|>>|<<)",
    R"(\|\||\&\&)",
    R"(\$\{|\$\()",
    R"(%0a|%0d|\n|\r)"
};

QHash<QString, QList<QDateTime>> InputValidator::s_validationAttempts;
QMutex InputValidator::s_rateLimitMutex;
QHash<QString, std::function<InputValidator::ValidationResult(const QString&, const InputValidator::ValidationOptions&)>> InputValidator::s_customValidators;

thread_local std::vector<ValidationContext*> ValidationContext::s_contextStack;

InputValidator::ValidationResult InputValidator::validate(const QString& input, ValidationRule rule, const ValidationOptions& options) {
    VALIDATION_CONTEXT("InputValidator::validate");
    
    if (input.isEmpty() && !options.allowEmpty) {
        return ValidationResult(false, "", "Input cannot be empty");
    }
    
    if (input.isEmpty() && options.allowEmpty) {
        return ValidationResult(true, "", "");
    }
    
    // Check rate limiting
    QString context = ValidationContext::getCurrentContext();
    if (!context.isEmpty() && isRateLimited(context)) {
        logSecurityAttempt(input, "Rate limit exceeded");
        return ValidationResult(false, "", "Too many validation attempts - please wait");
    }
    
    // Apply basic sanitization
    QString sanitized = applyBasicSanitization(input, options);
    
    // Length validation
    if (sanitized.length() < options.minLength) {
        return ValidationResult(false, sanitized, 
                               QString("Input too short (minimum %1 characters)").arg(options.minLength));
    }
    
    if (sanitized.length() > options.maxLength) {
        return ValidationResult(false, sanitized, 
                               QString("Input too long (maximum %1 characters)").arg(options.maxLength));
    }
    
    // Check blocked values
    if (options.blockedValues.contains(sanitized, Qt::CaseInsensitive)) {
        return ValidationResult(false, sanitized, "Input contains blocked value");
    }
    
    // Check allowed values (if specified)
    if (!options.allowedValues.isEmpty() && 
        !options.allowedValues.contains(sanitized, Qt::CaseInsensitive)) {
        return ValidationResult(false, sanitized, "Input not in allowed values list");
    }
    
    // Apply rule-specific validation
    ValidationResult result = applyRule(sanitized, rule, options);
    
    // Record attempt for rate limiting
    if (!context.isEmpty()) {
        recordValidationAttempt(context, result.isValid);
    }
    
    if (!result.isValid) {
        qCDebug(inputValidator) << "Validation failed for rule" << static_cast<int>(rule) << ":" << result.errorMessage;
    }
    
    return result;
}

InputValidator::ValidationResult InputValidator::validateUsername(const QString& username) {
    ValidationOptions options;
    options.minLength = 3;
    options.maxLength = 32;
    options.allowEmpty = false;
    
    return validate(username, ValidationRule::Username, options);
}

InputValidator::ValidationResult InputValidator::validatePassword(const QString& password) {
    ValidationOptions options;
    options.minLength = 8;
    options.maxLength = 128;
    options.allowEmpty = false;
    options.trimWhitespace = false; // Preserve whitespace in passwords
    
    auto result = validate(password, ValidationRule::Password, options);
    
    if (result.isValid) {
        // Additional password strength checks
        bool hasUpper = password.contains(QRegularExpression("[A-Z]"));
        bool hasLower = password.contains(QRegularExpression("[a-z]"));
        bool hasDigit = password.contains(QRegularExpression("[0-9]"));
        bool hasSpecial = password.contains(QRegularExpression("[^A-Za-z0-9]"));
        
        int strength = hasUpper + hasLower + hasDigit + hasSpecial;
        
        if (strength < 3) {
            result.warnings.append("Password should contain uppercase, lowercase, numbers, and special characters");
        }
        
        // Check for common weak patterns
        if (password.toLower().contains("password") || 
            password.toLower().contains("123456") ||
            password == password.toLower() ||
            password == password.toUpper()) {
            result.warnings.append("Password appears to be weak - avoid common patterns");
        }
    }
    
    return result;
}

InputValidator::ValidationResult InputValidator::validateEmail(const QString& email) {
    ValidationOptions options;
    options.minLength = 5;
    options.maxLength = 254; // RFC 5321 limit
    
    return validate(email, ValidationRule::Email, options);
}

InputValidator::ValidationResult InputValidator::validateIPAddress(const QString& ip) {
    QHostAddress address(ip);
    if (address.isNull()) {
        return ValidationResult(false, ip, "Invalid IP address format");
    }
    
    return ValidationResult(true, ip);
}

InputValidator::ValidationResult InputValidator::validateURL(const QString& url) {
    return validate(url, ValidationRule::URL);
}

InputValidator::ValidationResult InputValidator::validateFilePath(const QString& path) {
    return validate(path, ValidationRule::FilePath);
}

bool InputValidator::containsSqlInjection(const QString& input) {
    for (const QString& pattern : SQL_INJECTION_PATTERNS) {
        if (matchesRegex(input, pattern)) {
            logSecurityAttempt(input, "SQL Injection attempt detected");
            return true;
        }
    }
    return false;
}

bool InputValidator::containsXssAttempt(const QString& input) {
    for (const QString& pattern : XSS_PATTERNS) {
        if (matchesRegex(input, pattern)) {
            logSecurityAttempt(input, "XSS attempt detected");
            return true;
        }
    }
    return false;
}

bool InputValidator::containsPathTraversal(const QString& input) {
    for (const QString& pattern : PATH_TRAVERSAL_PATTERNS) {
        if (matchesRegex(input, pattern)) {
            logSecurityAttempt(input, "Path traversal attempt detected");
            return true;
        }
    }
    return false;
}

bool InputValidator::containsCommandInjection(const QString& input) {
    for (const QString& pattern : COMMAND_INJECTION_PATTERNS) {
        if (matchesRegex(input, pattern)) {
            logSecurityAttempt(input, "Command injection attempt detected");
            return true;
        }
    }
    return false;
}

QString InputValidator::sanitizeForSql(const QString& input) {
    QString sanitized = input;
    // Escape single quotes
    sanitized.replace("'", "''");
    return sanitized;
}

QString InputValidator::sanitizeForHtml(const QString& input) {
    QString sanitized = input;
    sanitized.replace("&", "&amp;");
    sanitized.replace("<", "&lt;");
    sanitized.replace(">", "&gt;");
    sanitized.replace("\"", "&quot;");
    sanitized.replace("'", "&#x27;");
    return sanitized;
}

QString InputValidator::sanitizeForFilePath(const QString& input) {
    QString sanitized = input;
    // Remove dangerous characters
    sanitized.remove(QRegularExpression(R"([<>:"|?*])"));
    // Remove path traversal attempts
    sanitized.remove(QRegularExpression(R"(\.\.\/|\.\.\\)"));
    return sanitized;
}

bool InputValidator::isRateLimited(const QString& identifier) {
    QMutexLocker locker(&s_rateLimitMutex);
    
    if (!s_validationAttempts.contains(identifier)) {
        return false;
    }
    
    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-60); // 1 minute window
    QList<QDateTime>& attempts = s_validationAttempts[identifier];
    
    // Remove old attempts
    attempts.erase(std::remove_if(attempts.begin(), attempts.end(),
                                  [cutoff](const QDateTime& dt) { return dt < cutoff; }),
                   attempts.end());
    
    // Check if rate limit exceeded (10 attempts per minute)
    return attempts.size() > 10;
}

void InputValidator::recordValidationAttempt(const QString& identifier, bool success) {
    QMutexLocker locker(&s_rateLimitMutex);
    
    if (!success) {
        s_validationAttempts[identifier].append(QDateTime::currentDateTime());
    }
}

void InputValidator::clearRateLimit(const QString& identifier) {
    QMutexLocker locker(&s_rateLimitMutex);
    s_validationAttempts.remove(identifier);
}

InputValidator::ValidationResult InputValidator::applyRule(const QString& input, ValidationRule rule, const ValidationOptions& options) {
    switch (rule) {
        case ValidationRule::None:
            return ValidationResult(true, input);
            
        case ValidationRule::AlphaNumeric:
            if (matchesRegex(input, R"(^[a-zA-Z0-9]+$)")) {
                return ValidationResult(true, input);
            }
            return ValidationResult(false, input, "Input must contain only letters and numbers");
            
        case ValidationRule::Numeric:
            if (matchesRegex(input, R"(^[0-9]+$)")) {
                return ValidationResult(true, input);
            }
            return ValidationResult(false, input, "Input must contain only numbers");
            
        case ValidationRule::Email: {
            QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
            if (emailRegex.match(input).hasMatch()) {
                return ValidationResult(true, input);
            }
            return ValidationResult(false, input, "Invalid email format");
        }
        
        case ValidationRule::Username:
            if (matchesRegex(input, R"(^[a-zA-Z0-9_][a-zA-Z0-9_.-]*[a-zA-Z0-9_]$|^[a-zA-Z0-9_]$)")) {
                return ValidationResult(true, input);
            }
            return ValidationResult(false, input, "Username must contain only letters, numbers, underscore, dot, and hyphen");
            
        case ValidationRule::Password: {
            ValidationResult result(true, input);
            
            if (input.length() < 8) {
                result.isValid = false;
                result.errorMessage = "Password must be at least 8 characters long";
            }
            
            return result;
        }
        
        case ValidationRule::IPAddress:
            return validateIPAddress(input);
            
        case ValidationRule::URL: {
            QUrl url(input);
            if (url.isValid() && !url.scheme().isEmpty() && !url.host().isEmpty()) {
                return ValidationResult(true, input);
            }
            return ValidationResult(false, input, "Invalid URL format");
        }
        
        case ValidationRule::SqlSafe:
            if (containsSqlInjection(input)) {
                return ValidationResult(false, sanitizeForSql(input), "Input contains SQL injection attempt");
            }
            return ValidationResult(true, sanitizeForSql(input));
            
        case ValidationRule::XssSafe:
            if (containsXssAttempt(input)) {
                return ValidationResult(false, sanitizeForHtml(input), "Input contains XSS attempt");
            }
            return ValidationResult(true, sanitizeForHtml(input));
            
        case ValidationRule::FilePath:
            if (containsPathTraversal(input)) {
                return ValidationResult(false, sanitizeForFilePath(input), "Path contains traversal attempt");
            }
            return ValidationResult(true, sanitizeForFilePath(input));
            
        case ValidationRule::Custom:
            if (options.customValidator) {
                return options.customValidator(input) ? 
                       ValidationResult(true, input) : 
                       ValidationResult(false, input, "Custom validation failed");
            }
            if (!options.customRegex.isEmpty()) {
                if (matchesRegex(input, options.customRegex)) {
                    return ValidationResult(true, input);
                }
                return ValidationResult(false, input, "Input does not match required pattern");
            }
            return ValidationResult(true, input);
            
        default:
            return ValidationResult(true, input);
    }
}

QString InputValidator::applyBasicSanitization(const QString& input, const ValidationOptions& options) {
    QString sanitized = input;
    
    if (options.trimWhitespace) {
        sanitized = sanitized.trimmed();
    }
    
    // Remove control characters
    sanitized.remove(QRegularExpression(R"([\x00-\x1f\x7f])"));
    
    return sanitized;
}

bool InputValidator::matchesRegex(const QString& input, const QString& pattern) {
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
    return regex.match(input).hasMatch();
}

void InputValidator::logSecurityAttempt(const QString& input, const QString& threat) {
    qCWarning(inputValidator) << "SECURITY THREAT:" << threat << "- Input:" << input.left(100);
    // Additional logging to security audit system would go here
}

// FormValidator implementation
void FormValidator::addField(const QString& fieldName, const FieldRule& rule) {
    m_rules[fieldName] = rule;
}

void FormValidator::addField(const QString& fieldName, InputValidator::ValidationRule rule, bool required) {
    FieldRule fieldRule(rule, required);
    m_rules[fieldName] = fieldRule;
}

Result<QHash<QString, QString>> FormValidator::validate(const QHash<QString, QString>& formData) {
    m_fieldErrors.clear();
    QHash<QString, QString> sanitizedData;
    bool allValid = true;
    
    // Check all defined fields
    for (auto it = m_rules.begin(); it != m_rules.end(); ++it) {
        const QString& fieldName = it.key();
        const FieldRule& rule = it.value();
        
        QString value = formData.value(fieldName);
        
        if (value.isEmpty() && rule.required) {
            m_fieldErrors[fieldName].append("Field is required");
            allValid = false;
            continue;
        }
        
        if (!value.isEmpty() || rule.required) {
            auto result = InputValidator::validate(value, rule.rule, rule.options);
            
            if (!result.isValid) {
                QString errorMsg = rule.errorMessage.isEmpty() ? result.errorMessage : rule.errorMessage;
                m_fieldErrors[fieldName].append(errorMsg);
                allValid = false;
            } else {
                sanitizedData[fieldName] = result.sanitizedValue;
                
                // Add warnings as field notes
                for (const QString& warning : result.warnings) {
                    m_fieldErrors[fieldName].append(QString("Warning: %1").arg(warning));
                }
            }
        }
    }
    
    // Check for unexpected fields (potential attack)
    for (auto it = formData.begin(); it != formData.end(); ++it) {
        if (!m_rules.contains(it.key())) {
            qCWarning(inputValidator) << "Unexpected form field:" << it.key();
        }
    }
    
    m_lastValidationResult = allValid;
    
    if (allValid) {
        return Result<QHash<QString, QString>>::success(sanitizedData);
    } else {
        return Result<QHash<QString, QString>>::error(ErrorCode::ValidationError, "Form validation failed");
    }
}

QStringList FormValidator::getFieldErrors(const QString& fieldName) const {
    return m_fieldErrors.value(fieldName);
}

bool FormValidator::isFormValid() const {
    return m_lastValidationResult;
}

void FormValidator::clearErrors() {
    m_fieldErrors.clear();
    m_lastValidationResult = false;
}

// ValidationContext implementation
ValidationContext::ValidationContext(const QString& context) : m_context(context) {
    s_contextStack.push_back(this);
}

ValidationContext::~ValidationContext() {
    if (!s_contextStack.empty() && s_contextStack.back() == this) {
        s_contextStack.pop_back();
    }
}

QString ValidationContext::getCurrentContext() {
    if (s_contextStack.empty()) {
        return QString();
    }
    
    QStringList contexts;
    for (const ValidationContext* ctx : s_contextStack) {
        contexts.append(ctx->m_context);
    }
    
    return contexts.join(" -> ");
}

void ValidationContext::addWarning(const QString& warning) {
    m_warnings.append(warning);
}

QStringList ValidationContext::getWarnings() const {
    return m_warnings;
}

} // namespace CyberRecon