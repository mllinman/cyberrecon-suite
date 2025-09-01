#pragma once
#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Updater : public QObject {
    Q_OBJECT
public:
    explicit Updater(QObject *parent = nullptr);

private slots:
    void checkForUpdate();
    void onReply(QNetworkReply *reply);

private:
    QTimer *timer;
    QNetworkAccessManager *net;
};