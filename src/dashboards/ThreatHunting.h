#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>

class ThreatHunting : public QWidget {
    Q_OBJECT
public:
    explicit ThreatHunting(QWidget *parent = nullptr);

private slots:
    void runHunt();
    void refreshResults();

private:
    QLineEdit *queryInput;
    QTextEdit *results;
};