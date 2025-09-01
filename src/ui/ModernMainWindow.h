#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QButtonGroup>
#include "monitoring/NetworkMonitor.h"
#include "tools/WirelessPentesting.h"
#include "tools/PentestingTools.h"

class ModernMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ModernMainWindow(const QString &username, QWidget *parent = nullptr);

private slots:
    void switchToModule(int moduleIndex);
    void showUserProfile();
    void showSettings();
    void logout();

private:
    void setupModernUI();
    void createSidebar();
    void createTopBar();
    void createModules();
    void applyModernTheme();
    void addSidebarButton(const QString &icon, const QString &text, const QString &tooltip, int index);
    void addSidebarSeparator();
    
    QString currentUser;
    
    // UI Components
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QFrame *sidebar;
    QVBoxLayout *sidebarLayout;
    QFrame *topBar;
    QStackedWidget *moduleStack;
    QButtonGroup *sidebarButtons;
    
    // Top bar elements
    QLabel *userAvatar;
    QLabel *userName;
    QPushButton *profileBtn;
    QPushButton *settingsBtn;
    QPushButton *logoutBtn;
    
    // Module widgets
    QWidget *dashboardModule;
    QWidget *monitoringModule;
    QWidget *intelligenceModule;
    QWidget *automationModule;
    QWidget *forensicsModule;
    QWidget *adminModule;
};