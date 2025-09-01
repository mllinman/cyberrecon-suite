#pragma once
#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initializeDatabase();
    void setupWindow();
    void setupMenuBar();
    void setupStatusBar();
    void applyTheme(const QString &themeFile);
    void centerWindow();
    
    QTabWidget *tabs;
    QString currentUser;
};