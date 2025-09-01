#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QSqlDatabase>

class SOARRules : public QWidget {
    Q_OBJECT
public:
    explicit SOARRules(QWidget *parent = nullptr);

private slots:
    void addRule();
    void deleteRule();
    void testRules();

private:
    void refreshRules();
    void addDefaultRules();
    
    QTreeWidget *tree;
    QLineEdit *panelEntry;
    QLineEdit *sevEntry;
    QLineEdit *actEntry;
    QSqlDatabase db;
};