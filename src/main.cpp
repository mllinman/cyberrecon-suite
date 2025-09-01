@@ .. @@
 #include <QApplication>
#include <QEventLoop>
#include "auth/SplashScreen.h"
#include "auth/LoginWindow.h"
#include "ui/ModernMainWindow.h"
 
 int main(int argc, char *argv[]) {
     QApplication app(argc, argv);

+    // Set application properties
+    app.setApplicationName("CyberRecon Suite");
+    app.setApplicationVersion("1.7.0");
+    app.setOrganizationName("CyberRecon Technologies");
+    
+    // Show splash screen
+    SplashScreen splash;
+    splash.show();
+    
    QEventLoop loop;

+    app.processEvents();
+    
+    // Wait for splash to complete

+    QObject::connect(&splash, &SplashScreen::loadingComplete, &loop, &QEventLoop::quit);
+    loop.exec();

+    splash.close();
+    
+    // Show login window
+    LoginWindow loginWindow;
+        return 0; // User cancelled login

+    
+    QString username = loginWindow.getCurrentUser();
+    bool hasSubscription = loginWindow.isSubscriptionActive();
+    
+    // Create and show main window
    if (loginWindow.exec() != QDialog::Accepted) {

    }
+    window.show();
 

-    window.setWindowTitle("CyberRecon Suite v1.4 (C++ Qt Build)");
-

     return app.exec();
 }