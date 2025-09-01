#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QThread>

class MLCorrelation : public QWidget {
    Q_OBJECT
public:
    explicit MLCorrelation(QWidget *parent = nullptr);

private slots:
    void runCorrelation();
    void trainModel();
    void autoCorrelate();

private:
    QStringList generateCorrelationResults(const QString &algorithm);
    
    QTextEdit *output;
    QComboBox *algorithmCombo;
    QProgressBar *progressBar;
    QString currentUser;
};