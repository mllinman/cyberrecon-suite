#include "SplashScreen.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QPainter>
#include <QPixmap>
#include <QScreen>

SplashScreen::SplashScreen(QWidget *parent) 
    : QSplashScreen(), currentProgress(0), messageIndex(0) {
    
    // Create splash pixmap
    QPixmap splashPixmap(500, 300);
    splashPixmap.fill(QColor(26, 26, 26));
    
    QPainter painter(&splashPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw gradient background
    QLinearGradient gradient(0, 0, 0, 300);
    gradient.setColorAt(0, QColor(26, 26, 26));
    gradient.setColorAt(1, QColor(45, 45, 45));
    painter.fillRect(splashPixmap.rect(), gradient);
    
    // Draw logo area
    painter.setPen(QPen(QColor(0, 188, 212), 3));
    painter.setBrush(QBrush(QColor(0, 188, 212, 30)));
    painter.drawRoundedRect(150, 50, 200, 80, 10, 10);
    
    // Draw logo text
    painter.setPen(QColor(0, 188, 212));
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(QRect(150, 50, 200, 80), Qt::AlignCenter, "🛡️\nCyberRecon");
    
    // Draw subtitle
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(QFont("Arial", 12));
    painter.drawText(QRect(0, 140, 500, 30), Qt::AlignCenter, "Advanced Cybersecurity Operations Platform");
    
    setPixmap(splashPixmap);
    
    setupUI();
    startLoadingSequence();
    
    // Center on screen
    move(QApplication::primaryScreen()->geometry().center() - rect().center());
}

void SplashScreen::setupUI() {
    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setGeometry(50, 220, 400, 20);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid #555;
            border-radius: 10px;
            background-color: #2d2d2d;
            text-align: center;
            color: white;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00bcd4, stop:1 #26c6da);
            border-radius: 8px;
        }
    )");
    
    // Status label
    statusLabel = new QLabel("Initializing...", this);
    statusLabel->setGeometry(50, 250, 400, 20);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #00bcd4; font-size: 12px;");
    
    // Version label
    versionLabel = new QLabel("Version 1.7.0", this);
    versionLabel->setGeometry(50, 270, 400, 15);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #888; font-size: 10px;");
}

void SplashScreen::startLoadingSequence() {
    loadingMessages = {
        "Initializing security modules...",
        "Loading SIEM engine...",
        "Starting EDR monitoring...",
        "Connecting to threat intelligence feeds...",
        "Initializing ML correlation engine...",
        "Loading forensics tools...",
        "Setting up SOAR automation...",
        "Preparing user interface...",
        "Finalizing startup sequence..."
    };
    
    loadingTimer = new QTimer(this);
    connect(loadingTimer, &QTimer::timeout, this, &SplashScreen::updateProgress);
    loadingTimer->start(300); // Update every 300ms
}

void SplashScreen::updateProgress() {
    currentProgress += 2 + (QRandomGenerator::global()->bounded(8)); // Random progress increment
    
    if (currentProgress >= 100) {
        currentProgress = 100;
        progressBar->setValue(currentProgress);
        statusLabel->setText("Startup complete!");
        
        loadingTimer->stop();
        QTimer::singleShot(500, this, &SplashScreen::loadingComplete);
        return;
    }
    
    progressBar->setValue(currentProgress);
    
    // Update status message
    if (messageIndex < loadingMessages.size()) {
        if (currentProgress > (messageIndex + 1) * (100 / loadingMessages.size())) {
            messageIndex++;
        }
        if (messageIndex < loadingMessages.size()) {
            statusLabel->setText(loadingMessages[messageIndex]);
        }
    }
}

void SplashScreen::showProgressMessage(const QString &message, int progress) {
    statusLabel->setText(message);
    progressBar->setValue(progress);
    QApplication::processEvents();
}