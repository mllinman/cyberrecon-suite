#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QProgressBar>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ThreatIntelligence : public QWidget {
    Q_OBJECT

public:
    explicit ThreatIntelligence(QWidget *parent = nullptr);

private slots:
    void searchIOC();
    void refreshFeeds();
    void exportIOCs();
    void onNetworkReply(QNetworkReply *reply);

private:
    void setupUI();
    void updateThreatMap();
    void addIOCToTable(const QString &ioc, const QString &type, const QString &severity, const QString &source);
    
    QLineEdit *searchInput;
    QComboBox *iocTypeCombo;
    QTextEdit *resultsDisplay;
    QTableWidget *iocTable;
    QProgressBar *scanProgress;
    QNetworkAccessManager *networkManager;
    QTimer *refreshTimer;
};