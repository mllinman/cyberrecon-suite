#include <QApplication>
#include <QEventLoop>
#include <QStandardPaths>
#include <QDir>
#include <QLoggingCategory>
#include "auth/SplashScreen.h"
#include "auth/LoginWindow.h"
#include "ui/ModernMainWindow.h"

// Enable modern Qt logging
Q_LOGGING_CATEGORY(cyberrecon, "cyberrecon.main")

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set application properties for better security and organization
    app.setApplicationName("CyberRecon Suite");
    app.setApplicationVersion("1.7.0");
    app.setOrganizationName("CyberRecon Technologies");
    app.setOrganizationDomain("cyberrecon.com");
    
    // Enable high DPI support for modern displays
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    
    // Ensure data directory exists with secure permissions
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    
    qCInfo(cyberrecon) << "CyberRecon Suite v1.7.0 starting up";
    qCInfo(cyberrecon) << "Data directory:" << dataDir;
    
    // Show splash screen with proper loading sequence
    SplashScreen splash;
    splash.show();
    
    QEventLoop loop;
    app.processEvents();
    
    // Wait for splash to complete initialization
    QObject::connect(&splash, &SplashScreen::loadingComplete, &loop, &QEventLoop::quit);
    loop.exec();
    
    splash.close();
    
    // Show secure login window
    LoginWindow loginWindow;
    if (loginWindow.exec() != QDialog::Accepted) {
        qCInfo(cyberrecon) << "User cancelled login, exiting";
        return 0; // User cancelled login
    }
    
    QString username = loginWindow.getCurrentUser();
    bool hasSubscription = loginWindow.isSubscriptionActive();
    
    qCInfo(cyberrecon) << "User logged in:" << username << "Subscription active:" << hasSubscription;
    
    // Create and show main window with user context
    ModernMainWindow window(username, hasSubscription);
    window.setWindowTitle(QString("CyberRecon Suite v1.7.0 - %1").arg(username));
    window.show();
    
    qCInfo(cyberrecon) << "Main window initialized, entering event loop";
    
    return app.exec();
}