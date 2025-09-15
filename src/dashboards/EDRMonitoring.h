#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>
#include <QTreeWidget>
#include <QLineEdit>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>

class EDRMonitoring : public QWidget {
    Q_OBJECT
public:
    explicit EDRMonitoring(QWidget *parent = nullptr);

private slots:
    // Alert and monitoring
    void simulateAlert();
    void refreshAlerts();
    
    // Endpoint management
    void scanEndpoints();
    void quarantineEndpoint();
    void releaseQuarantine();
    void deployPolicy();
    void huntThreats();
    void analyzeProcess();
    void blockHash();
    void refreshEndpoints();
    void viewEndpointDetails();
    void exportThreatData();

private:
    void createChart();
    void updateChart();
    void setupUI();
    void setupStyling();
    void initializeDatabase();
    void loadEndpointData();
    void simulateEndpointScan();
    void addEndpointToTable(const QString &hostname, const QString &ip, const QString &os, 
                           const QString &status, const QString &lastSeen, const QString &threats);
    void addThreatDetection(const QString &endpoint, const QString &threat, const QString &severity);
    QString generateProcessAnalysis();
    QString simulateThreatHunt();
    
    // Original EDR components
    QTextEdit *output;
    QBarSet *edrSet;
    QChartView *chartView;
    
    // Enhanced EDR components
    QTableWidget *endpointsTable;
    QTableWidget *threatsTable;
    QTextEdit *activityLog;
    QTextEdit *processAnalysis;
    QComboBox *policyCombo;
    QComboBox *actionCombo;
    QLineEdit *hashInput;
    QLineEdit *huntQuery;
    QProgressBar *scanProgress;
    QLabel *endpointCount;
    QLabel *threatCount;
    QLabel *quarantineCount;
    
    // Data management
    QSqlDatabase edrDb;
    QTimer *refreshTimer;
    int totalEndpoints;
    int totalThreats;
    int quarantinedEndpoints;
};