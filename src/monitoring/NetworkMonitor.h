#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QTreeWidget>
#include <QSplitter>
#include <QDateTime>

class NetworkMonitor : public QWidget {
    Q_OBJECT

public:
    explicit NetworkMonitor(QWidget *parent = nullptr);

private slots:
    // Packet Capture (Wireshark-like)
    void startPacketCapture();
    void stopPacketCapture();
    void clearPackets();
    void filterPackets();
    void exportPackets();
    void analyzePacket();
    
    // Network Scanner (Nmap-like)
    void startNetworkScan();
    void stopNetworkScan();
    void clearScanResults();
    void scanSingleHost();
    void performPortScan();
    
    // Auto-update functions
    void simulatePacketCapture();
    void updateNetworkScan();

private:
    void setupPacketCaptureTab();
    void setupNetworkScannerTab();
    void setupStyling();
    void addPacketToTable(const QString &time, const QString &source, const QString &dest, 
                         const QString &protocol, const QString &length, const QString &info);
    void addScanResult(const QString &host, const QString &status, const QString &ports, const QString &os, const QString &services);
    QString generatePacketDetails(const QString &protocol, const QString &source, const QString &dest);
    QString simulatePortScan(const QString &host);
    QString detectOS(const QString &host);
    
    QTabWidget *monitoringTabs;
    
    // Packet Capture Tab
    QWidget *packetCaptureWidget;
    QTableWidget *packetTable;
    QTextEdit *packetDetails;
    QLineEdit *captureFilter;
    QComboBox *interfaceCombo;
    QPushButton *startCaptureBtn;
    QPushButton *stopCaptureBtn;
    QLabel *captureStatus;
    QLabel *packetCount;
    QTimer *captureTimer;
    bool isCapturing;
    int totalPackets;
    
    // Network Scanner Tab
    QWidget *networkScannerWidget;
    QLineEdit *targetInput;
    QComboBox *scanTypeCombo;
    QSpinBox *portRangeStart;
    QSpinBox *portRangeEnd;
    QCheckBox *osDetectionCheck;
    QCheckBox *serviceDetectionCheck;
    QTableWidget *scanResultsTable;
    QTextEdit *scanOutput;
    QPushButton *startScanBtn;
    QPushButton *stopScanBtn;
    QProgressBar *scanProgress;
    QLabel *scanStatus;
    QTimer *scanTimer;
    bool isScanning;
    int currentScanTarget;
    QStringList scanTargets;
};