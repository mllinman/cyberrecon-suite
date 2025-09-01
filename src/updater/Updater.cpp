#include "Updater.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

Updater::Updater(QObject *parent) : QObject(parent) {
    net = new QNetworkAccessManager(this);
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &Updater::checkForUpdate);
    connect(net, &QNetworkAccessManager::finished, this, &Updater::onReply);

    // Check every 6 hours
    timer->start(6 * 60 * 60 * 1000);
    checkForUpdate(); // also run once at startup
}

void Updater::checkForUpdate() {
    QUrl url("https://api.github.com/repos/cyberrecon/suite/releases/latest");
    net->get(QNetworkRequest(url));
}

void Updater::onReply(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString latest = json.object()["tag_name"].toString(); // e.g., "v1.7"
    QString current = "v1.7"; // adjust for build

    if(latest != current && !latest.isEmpty()) {
        QMessageBox::information(nullptr, "Update Available",
            "New version " + latest + " is available!\n\n"
            "Click OK to open the download page.");
        QDesktopServices::openUrl(QUrl(json.object()["html_url"].toString()));
    }
    
    reply->deleteLater();
}