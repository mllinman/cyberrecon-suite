#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class IntelHub : public QWidget {
    Q_OBJECT
public:
    explicit IntelHub(QWidget *parent = nullptr);

private slots:
    void lookupIOC();
    void refreshIntel();
    void onReply(QNetworkReply *reply);

private:
    void showTrend();
    
    QLineEdit *entry;
    QTextEdit *output;
    QNetworkAccessManager *net;
    QTimer *timer;
};