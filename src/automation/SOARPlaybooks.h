#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QComboBox>

class SOARPlaybooks : public QWidget {
    Q_OBJECT
public:
    explicit SOARPlaybooks(QWidget *parent = nullptr);

private slots:
    void executePlaybook();
    void savePlaybook();
    void loadPredefinedPlaybook();

private:
    QString simulateStepExecution(const QString &step);
    
    QTextEdit *playbookBox;
    QTextEdit *executionLog;
    QComboBox *playbookSelector;
};