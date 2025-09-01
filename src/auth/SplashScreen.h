#pragma once
#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QMovie>

class SplashScreen : public QSplashScreen {
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    void showProgressMessage(const QString &message, int progress);

signals:
    void loadingComplete();

private slots:
    void updateProgress();

private:
    void setupUI();
    void startLoadingSequence();
    
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QLabel *versionLabel;
    QTimer *loadingTimer;
    int currentProgress;
    QStringList loadingMessages;
    int messageIndex;
};