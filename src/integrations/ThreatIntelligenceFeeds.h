#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSqlDatabase>

class ThreatIntelligenceFeeds : public QWidget {
    Q_OBJECT

public:
    explicit ThreatIntelligenceFeeds(QWidget *parent = nullptr);

private slots:
    void refreshFeeds();
    void queryIOC();
    void addCustomIOC();
    void exportIOCs();
    void onNetworkReply(QNetworkReply *reply);

private:
    void setupUI();
    void initializeDatabase();
    void loadThreatFeeds();
    void updateFeedStatus();
    void addIOCToTable(const QString &ioc, const QString &type, const QString &severity, 
                      const QString &source, const QString &confidence);
    
    QTableWidget *iocTable;
    QTextEdit *feedOutput;
    QComboBox *feedSourceCombo;
    QComboBox *iocTypeCombo;
    QNetworkAccessManager *networkManager;
    QTimer *feedTimer;
    QSqlDatabase intelDb;
};