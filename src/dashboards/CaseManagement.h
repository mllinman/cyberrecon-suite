#pragma once
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>

class CaseManagement : public QWidget {
    Q_OBJECT
public:
    explicit CaseManagement(QWidget *parent = nullptr);
    void logAction(int caseId, const QString &action);

private slots:
    void addCase();
    void closeCase();
    void viewEvidence();

private:
    void refreshCases();
    QListWidget *caseList;
    QLineEdit *caseEntry;
    QSqlDatabase db;
};