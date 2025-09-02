#include "NetworkMonitor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QHeaderView>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSqlQuery>
#include <QApplication>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QRandomGenerator>
#include <QDateTime>

NetworkMonitor::NetworkMonitor(QWidget *parent) 
    : QWidget(parent), isCapturing(false), isScanning(false), totalPackets(0), currentScanTarget(0) {
    
    auto *layout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("🌐 Network Monitoring & Analysis Suite");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #00bcd4; margin: 10px;");
    layout->addWidget(titleLabel);
    
    // Create tab widget
    monitoringTabs = new QTabWidget();
    layout->addWidget(monitoringTabs);
    
    setupPacketCaptureTab();
    setupNetworkScannerTab();
    setupStyling();
    
    // Initialize timers
    captureTimer = new QTimer(this);
    connect(captureTimer, &QTimer::timeout, this, &NetworkMonitor::simulatePacketCapture);
    
    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &NetworkMonitor::updateNetworkScan);
}

void NetworkMonitor::setupPacketCaptureTab() {
    packetCaptureWidget = new QWidget();
    auto *layout = new QVBoxLayout(packetCaptureWidget);
    
    // Control panel
    auto *controlGroup = new QGroupBox("Packet Capture Control");
    auto *controlLayout = new QGridLayout(controlGroup);
    
    // Interface selection
    controlLayout->addWidget(new QLabel("Interface:"), 0, 0);
    interfaceCombo = new QComboBox();
    
    // Populate with network interfaces
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        if (interface.flags() & QNetworkInterface::IsUp && 
            interface.flags() & QNetworkInterface::IsRunning) {
            interfaceCombo->addItem(QString("%1 (%2)").arg(interface.name()).arg(interface.humanReadableName()));
        }
    }
    if (interfaceCombo->count() == 0) {
        interfaceCombo->addItem("eth0 (Ethernet)");
        interfaceCombo->addItem("wlan0 (Wireless)");
        interfaceCombo->addItem("lo (Loopback)");
    }
    controlLayout->addWidget(interfaceCombo, 0, 1);
    
    // Capture filter
    controlLayout->addWidget(new QLabel("Filter:"), 0, 2);
    captureFilter = new QLineEdit();
    captureFilter->setPlaceholderText("tcp port 80 or udp port 53");
    controlLayout->addWidget(captureFilter, 0, 3);
    
    // Control buttons
    startCaptureBtn = new QPushButton("🔴 Start Capture");
    stopCaptureBtn = new QPushButton("⏹️ Stop Capture");
    auto *clearBtn = new QPushButton("🗑️ Clear");
    auto *exportBtn = new QPushButton("💾 Export");
    
    stopCaptureBtn->setEnabled(false);
    
    controlLayout->addWidget(startCaptureBtn, 1, 0);
    controlLayout->addWidget(stopCaptureBtn, 1, 1);
    controlLayout->addWidget(clearBtn, 1, 2);
    controlLayout->addWidget(exportBtn, 1, 3);
    
    layout->addWidget(controlGroup);
    
    // Status bar
    auto *statusLayout = new QHBoxLayout();
    captureStatus = new QLabel("Status: Ready");
    captureStatus->setStyleSheet("font-weight: bold; color: #4caf50;");
    packetCount = new QLabel("Packets: 0");
    packetCount->setStyleSheet("font-weight: bold; color: #2196f3;");
    
    statusLayout->addWidget(captureStatus);
    statusLayout->addStretch();
    statusLayout->addWidget(packetCount);
    layout->addLayout(statusLayout);
    
    // Main content - splitter with packet table and details
    auto *splitter = new QSplitter(Qt::Vertical);
    
    // Packet table
    packetTable = new QTableWidget(0, 6);
    packetTable->setHorizontalHeaderLabels({"Time", "Source", "Destination", "Protocol", "Length", "Info"});
    packetTable->horizontalHeader()->setStretchLastSection(true);
    packetTable->setAlternatingRowColors(true);
    packetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    splitter->addWidget(packetTable);
    
    // Packet details
    packetDetails = new QTextEdit();
    packetDetails->setReadOnly(true);
    packetDetails->setMaximumHeight(200);
    packetDetails->setPlaceholderText("Select a packet to view detailed information...");
    splitter->addWidget(packetDetails);
    
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);
    
    // Connect signals
    connect(startCaptureBtn, &QPushButton::clicked, this, &NetworkMonitor::startPacketCapture);
    connect(stopCaptureBtn, &QPushButton::clicked, this, &NetworkMonitor::stopPacketCapture);
    connect(clearBtn, &QPushButton::clicked, this, &NetworkMonitor::clearPackets);
    connect(exportBtn, &QPushButton::clicked, this, &NetworkMonitor::exportPackets);
    connect(packetTable, &QTableWidget::itemSelectionChanged, this, &NetworkMonitor::analyzePacket);
    
    monitoringTabs->addTab(packetCaptureWidget, "📡 Packet Capture");
}

void NetworkMonitor::setupNetworkScannerTab() {
    networkScannerWidget = new QWidget();
    auto *layout = new QVBoxLayout(networkScannerWidget);
    
    // Scan configuration
    auto *configGroup = new QGroupBox("Scan Configuration");
    auto *configLayout = new QGridLayout(configGroup);
    
    // Target input
    configLayout->addWidget(new QLabel("Target:"), 0, 0);
    targetInput = new QLineEdit();
    targetInput->setPlaceholderText("192.168.1.0/24 or 192.168.1.1-100 or single IP");
    configLayout->addWidget(targetInput, 0, 1, 1, 2);
    
    // Scan type
    configLayout->addWidget(new QLabel("Scan Type:"), 1, 0);
    scanTypeCombo = new QComboBox();
    scanTypeCombo->addItems({
        "TCP SYN Scan (-sS)",
        "TCP Connect Scan (-sT)", 
        "UDP Scan (-sU)",
        "Ping Sweep (-sn)",
        "Comprehensive Scan (-A)",
        "Stealth Scan (-sF)",
        "Version Detection (-sV)"
    });
    configLayout->addWidget(scanTypeCombo, 1, 1);
    
    // Port range
    configLayout->addWidget(new QLabel("Port Range:"), 2, 0);
    auto *portLayout = new QHBoxLayout();
    portRangeStart = new QSpinBox();
    portRangeStart->setRange(1, 65535);
    portRangeStart->setValue(1);
    portLayout->addWidget(portRangeStart);
    portLayout->addWidget(new QLabel("to"));
    portRangeEnd = new QSpinBox();
    portRangeEnd->setRange(1, 65535);
    portRangeEnd->setValue(1000);
    portLayout->addWidget(portRangeEnd);
    portLayout->addStretch();
    configLayout->addLayout(portLayout, 2, 1);
    
    // Options
    osDetectionCheck = new QCheckBox("OS Detection");
    serviceDetectionCheck = new QCheckBox("Service Detection");
    configLayout->addWidget(osDetectionCheck, 3, 0);
    configLayout->addWidget(serviceDetectionCheck, 3, 1);
    
    layout->addWidget(configGroup);
    
    // Control buttons
    auto *buttonLayout = new QHBoxLayout();
    startScanBtn = new QPushButton("🔍 Start Scan");
    stopScanBtn = new QPushButton("⏹️ Stop Scan");
    auto *clearScanBtn = new QPushButton("🗑️ Clear Results");
    auto *exportScanBtn = new QPushButton("💾 Export Results");
    
    stopScanBtn->setEnabled(false);
    
    buttonLayout->addWidget(startScanBtn);
    buttonLayout->addWidget(stopScanBtn);
    buttonLayout->addWidget(clearScanBtn);
    buttonLayout->addWidget(exportScanBtn);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    // Progress and status
    scanProgress = new QProgressBar();
    scanProgress->setVisible(false);
    layout->addWidget(scanProgress);
    
    scanStatus = new QLabel("Status: Ready");
    scanStatus->setStyleSheet("font-weight: bold; color: #4caf50; margin: 5px;");
    layout->addWidget(scanStatus);
    
    // Results area - splitter with results table and output
    auto *resultsSplitter = new QSplitter(Qt::Horizontal);
    
    // Scan results table
    scanResultsTable = new QTableWidget(0, 5);
    scanResultsTable->setHorizontalHeaderLabels({"Host", "Status", "Open Ports", "OS", "Services"});
    scanResultsTable->horizontalHeader()->setStretchLastSection(true);
    scanResultsTable->setAlternatingRowColors(true);
    resultsSplitter->addWidget(scanResultsTable);
    
    // Scan output
    scanOutput = new QTextEdit();
    scanOutput->setReadOnly(true);
    scanOutput->setPlaceholderText("Scan output will appear here...");
    resultsSplitter->addWidget(scanOutput);
    
    resultsSplitter->setStretchFactor(0, 2);
    resultsSplitter->setStretchFactor(1, 1);
    layout->addWidget(resultsSplitter);
    
    // Connect signals
    connect(startScanBtn, &QPushButton::clicked, this, &NetworkMonitor::startNetworkScan);
    connect(stopScanBtn, &QPushButton::clicked, this, &NetworkMonitor::stopNetworkScan);
    connect(clearScanBtn, &QPushButton::clicked, this, &NetworkMonitor::clearScanResults);
    connect(exportScanBtn, &QPushButton::clicked, this, &NetworkMonitor::exportPackets);
    
    monitoringTabs->addTab(networkScannerWidget, "🔍 Network Scanner");
}

void NetworkMonitor::setupStyling() {
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 15px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px 0 8px;
            color: #00bcd4;
            font-size: 14px;
        }
        
        QTableWidget {
            background-color: #2d2d2d;
            alternate-background-color: #3c3c3c;
            gridline-color: #555;
            color: #fff;
            border: 1px solid #555;
            border-radius: 6px;
        }
        
        QTableWidget::item {
            padding: 6px;
        }
        
        QTableWidget::item:selected {
            background-color: #00bcd4;
        }
        
        QHeaderView::section {
            background-color: #404040;
            color: #00bcd4;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        QTextEdit {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: 'Consolas', monospace;
            font-size: 11px;
            border: 1px solid #555;
            border-radius: 6px;
            padding: 8px;
        }
        
        QLineEdit, QComboBox, QSpinBox {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 4px;
            padding: 6px;
            color: #fff;
        }
        
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus {
            border-color: #00bcd4;
        }
        
        QPushButton {
            background-color: #00bcd4;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #26c6da;
        }
        
        QPushButton:disabled {
            background-color: #666;
            color: #999;
        }
        
        QProgressBar {
            border: 2px solid #555;
            border-radius: 5px;
            text-align: center;
            color: white;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00bcd4, stop:1 #26c6da);
            border-radius: 3px;
        }
        
        QCheckBox {
            color: #fff;
        }
        
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
        }
        
        QCheckBox::indicator:unchecked {
            background-color: #3c3c3c;
            border: 2px solid #555;
            border-radius: 3px;
        }
        
        QCheckBox::indicator:checked {
            background-color: #00bcd4;
            border: 2px solid #00bcd4;
            border-radius: 3px;
        }
    )");
}

void NetworkMonitor::startPacketCapture() {
    if (isCapturing) return;
    
    // Show legal disclaimer before starting
    int ret = QMessageBox::question(this, "⚠️ Legal Authorization Required", 
        "IMPORTANT LEGAL NOTICE:\n\n"
        "You are about to use packet capture tools that can:\n"
        "• Intercept network communications\n"
        "• Capture sensitive data\n"
        "• Monitor user activities\n\n"
        "These tools must ONLY be used on networks you own or have explicit written authorization to monitor.\n\n"
        "Unauthorized packet capture may violate:\n"
        "• Wiretapping laws\n"
        "• Privacy regulations (GDPR, CCPA)\n"
        "• Electronic communications acts\n\n"
        "Do you have proper authorization to capture packets on this network?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        scanOutput->append("<font color='#f44336'>[CANCELLED] User declined legal authorization confirmation</font>");
        return;
    }
    
    QString interface = interfaceCombo->currentText();
    QString filter = captureFilter->text().trimmed();
    
    isCapturing = true;
    totalPackets = 0;
    
    startCaptureBtn->setEnabled(false);
    stopCaptureBtn->setEnabled(true);
    
    captureStatus->setText("Status: Capturing");
    captureStatus->setStyleSheet("font-weight: bold; color: #4caf50;");
    
    scanOutput->append(QString("<font color='#4caf50'>[CAPTURE] Starting packet capture on %1</font>").arg(interface));
    if (!filter.isEmpty()) {
        scanOutput->append(QString("<font color='#2196f3'>[FILTER] Applied filter: %1</font>").arg(filter));
    }
    
    captureTimer->start(500); // Capture packets every 500ms
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('NetworkMonitor', 'Info', :msg)");
    q.bindValue(":msg", QString("Packet capture started on %1").arg(interface));
    q.exec();
}

void NetworkMonitor::stopPacketCapture() {
    if (!isCapturing) return;
    
    isCapturing = false;
    captureTimer->stop();
    
    startCaptureBtn->setEnabled(true);
    stopCaptureBtn->setEnabled(false);
    
    captureStatus->setText("Status: Stopped");
    captureStatus->setStyleSheet("font-weight: bold; color: #f44336;");
    
    scanOutput->append(QString("<font color='#f44336'>[CAPTURE] Packet capture stopped. Total packets: %1</font>").arg(totalPackets));
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('NetworkMonitor', 'Info', :msg)");
    q.bindValue(":msg", QString("Packet capture stopped. Captured %1 packets").arg(totalPackets));
    q.exec();
}

void NetworkMonitor::clearPackets() {
    packetTable->setRowCount(0);
    packetDetails->clear();
    totalPackets = 0;
    packetCount->setText("Packets: 0");
    scanOutput->append("<font color='#ff9800'>[CLEAR] Packet table cleared</font>");
}

void NetworkMonitor::simulatePacketCapture() {
    if (!isCapturing) return;
    
    // Simulate different types of network packets
    struct PacketData {
        QString source;
        QString dest;
        QString protocol;
        QString info;
        int length;
    };
    
    QVector<PacketData> samplePackets = {
        {"192.168.1.10", "8.8.8.8", "DNS", "Standard query A google.com", 74},
        {"192.168.1.10", "172.217.14.110", "HTTP", "GET /search?q=cybersecurity HTTP/1.1", 512},
        {"192.168.1.100", "192.168.1.10", "TCP", "[SYN] Seq=0 Win=65535 Len=0 MSS=1460", 66},
        {"10.0.0.5", "192.168.1.10", "ICMP", "Echo (ping) request", 98},
        {"192.168.1.10", "443", "TLS", "Client Hello", 517},
        {"192.168.1.50", "192.168.1.10", "SSH", "Encrypted packet", 128},
        {"192.168.1.10", "239.255.255.250", "UDP", "SSDP M-SEARCH", 174},
        {"fe80::1", "ff02::1", "ICMPv6", "Router Advertisement", 86}
    };
    
    // Add 1-3 random packets per update
    int packetsToAdd = 1 + (qrand() % 3);
    for (int i = 0; i < packetsToAdd; ++i) {
        PacketData packet = samplePackets[qrand() % samplePackets.size()];
        
        QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        addPacketToTable(currentTime, packet.source, packet.dest, 
                        packet.protocol, QString::number(packet.length), packet.info);
        
        totalPackets++;
    }
    
    packetCount->setText(QString("Packets: %1").arg(totalPackets));
    
    // Auto-scroll to bottom
    packetTable->scrollToBottom();
}

void NetworkMonitor::addPacketToTable(const QString &time, const QString &source, const QString &dest, 
                                     const QString &protocol, const QString &length, const QString &info) {
    int row = packetTable->rowCount();
    packetTable->insertRow(row);
    
    packetTable->setItem(row, 0, new QTableWidgetItem(time));
    packetTable->setItem(row, 1, new QTableWidgetItem(source));
    packetTable->setItem(row, 2, new QTableWidgetItem(dest));
    
    // Color code protocols
    auto *protocolItem = new QTableWidgetItem(protocol);
    if (protocol == "TCP") {
        protocolItem->setForeground(QColor("#4caf50"));
    } else if (protocol == "UDP") {
        protocolItem->setForeground(QColor("#2196f3"));
    } else if (protocol == "ICMP" || protocol == "ICMPv6") {
        protocolItem->setForeground(QColor("#ff9800"));
    } else if (protocol == "HTTP" || protocol == "HTTPS" || protocol == "TLS") {
        protocolItem->setForeground(QColor("#9c27b0"));
    } else {
        protocolItem->setForeground(QColor("#ffffff"));
    }
    packetTable->setItem(row, 3, protocolItem);
    
    packetTable->setItem(row, 4, new QTableWidgetItem(length));
    packetTable->setItem(row, 5, new QTableWidgetItem(info));
    
    // Keep only last 1000 packets for performance
    if (packetTable->rowCount() > 1000) {
        packetTable->removeRow(0);
    }
}

void NetworkMonitor::analyzePacket() {
    int currentRow = packetTable->currentRow();
    if (currentRow < 0) return;
    
    QString time = packetTable->item(currentRow, 0)->text();
    QString source = packetTable->item(currentRow, 1)->text();
    QString dest = packetTable->item(currentRow, 2)->text();
    QString protocol = packetTable->item(currentRow, 3)->text();
    QString length = packetTable->item(currentRow, 4)->text();
    QString info = packetTable->item(currentRow, 5)->text();
    
    QString details = generatePacketDetails(protocol, source, dest);
    
    packetDetails->setHtml(QString(R"(
        <h3 style="color: #00bcd4;">Packet Analysis</h3>
        <table style="color: #ffffff; font-family: monospace;">
        <tr><td><b>Time:</b></td><td>%1</td></tr>
        <tr><td><b>Source:</b></td><td>%2</td></tr>
        <tr><td><b>Destination:</b></td><td>%3</td></tr>
        <tr><td><b>Protocol:</b></td><td>%4</td></tr>
        <tr><td><b>Length:</b></td><td>%5 bytes</td></tr>
        <tr><td><b>Info:</b></td><td>%6</td></tr>
        </table>
        <h4 style="color: #ff9800;">Protocol Details:</h4>
        <pre style="color: #ffffff;">%7</pre>
    )").arg(time, source, dest, protocol, length, info, details));
}

QString NetworkMonitor::generatePacketDetails(const QString &protocol, const QString &source, const QString &dest) {
    if (protocol == "TCP") {
        return QString(R"(
TCP Header:
  Source Port: %1
  Destination Port: %2
  Sequence Number: 0x%3
  Acknowledgment: 0x%4
  Flags: [SYN, ACK]
  Window Size: 65535
  Checksum: 0x%5
        )").arg(qrand() % 65535).arg(qrand() % 65535)
           .arg(qrand(), 8, 16, QChar('0'))
           .arg(qrand(), 8, 16, QChar('0'))
           .arg(qrand() % 65535, 4, 16, QChar('0'));
    } else if (protocol == "UDP") {
        return QString(R"(
UDP Header:
  Source Port: %1
  Destination Port: %2
  Length: %3
  Checksum: 0x%4
        )").arg(qrand() % 65535).arg(qrand() % 65535)
           .arg(qrand() % 1500).arg(qrand() % 65535, 4, 16, QChar('0'));
    } else if (protocol == "ICMP") {
        return QString(R"(
ICMP Header:
  Type: 8 (Echo Request)
  Code: 0
  Checksum: 0x%1
  Identifier: 0x%2
  Sequence: %3
        )").arg(qrand() % 65535, 4, 16, QChar('0'))
           .arg(qrand() % 65535, 4, 16, QChar('0'))
           .arg(qrand() % 65535);
    } else {
        return QString("Protocol-specific details for %1 packet").arg(protocol);
    }
}

void NetworkMonitor::startNetworkScan() {
    if (isScanning) return;
    
    QString target = targetInput->text().trimmed();
    if (target.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter a target to scan.");
        return;
    }
    
    // Show legal disclaimer before starting
    int ret = QMessageBox::question(this, "⚠️ Legal Authorization Required", 
        "IMPORTANT LEGAL NOTICE:\n\n"
        "You are about to use network scanning tools that can:\n"
        "• Discover network hosts and services\n"
        "• Identify system vulnerabilities\n"
        "• Trigger security alerts\n\n"
        "These tools must ONLY be used on networks you own or have explicit written authorization to scan.\n\n"
        "Unauthorized network scanning may violate:\n"
        "• Computer crime laws\n"
        "• Network terms of service\n"
        "• Cybersecurity regulations\n\n"
        "Do you have proper authorization to scan the target network?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        scanOutput->append("<font color='#f44336'>[CANCELLED] User declined legal authorization confirmation</font>");
        return;
    }
    
    isScanning = true;
    currentScanTarget = 0;
    
    // Parse target into list of IPs to scan
    scanTargets.clear();
    if (target.contains("/24")) {
        // CIDR notation - simulate scanning subnet
        QString baseIP = target.split("/").first();
        QString base = baseIP.left(baseIP.lastIndexOf('.') + 1);
        for (int i = 1; i <= 254; ++i) {
            scanTargets.append(base + QString::number(i));
        }
    } else if (target.contains("-")) {
        // Range notation
        QStringList parts = target.split("-");
        if (parts.size() == 2) {
            QString baseIP = parts[0].left(parts[0].lastIndexOf('.') + 1);
            int start = parts[0].split('.').last().toInt();
            int end = parts[1].toInt();
            for (int i = start; i <= end; ++i) {
                scanTargets.append(baseIP + QString::number(i));
            }
        }
    } else {
        // Single IP
        scanTargets.append(target);
    }
    
    startScanBtn->setEnabled(false);
    stopScanBtn->setEnabled(true);
    
    scanProgress->setVisible(true);
    scanProgress->setRange(0, scanTargets.size());
    scanProgress->setValue(0);
    
    scanStatus->setText("Status: Scanning");
    scanStatus->setStyleSheet("font-weight: bold; color: #ff9800;");
    
    QString scanType = scanTypeCombo->currentText();
    scanOutput->append(QString("<font color='#4caf50'>[SCAN] Starting %1 on %2 targets</font>")
                      .arg(scanType).arg(scanTargets.size()));
    scanOutput->append(QString("<font color='#2196f3'>[SCAN] Port range: %1-%2</font>")
                      .arg(portRangeStart->value()).arg(portRangeEnd->value()));
    
    scanTimer->start(1000); // Scan one target per second
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('NetworkMonitor', 'Info', :msg)");
    q.bindValue(":msg", QString("Network scan started: %1 (%2 targets)").arg(target).arg(scanTargets.size()));
    q.exec();
}

void NetworkMonitor::stopNetworkScan() {
    if (!isScanning) return;
    
    isScanning = false;
    scanTimer->stop();
    
    startScanBtn->setEnabled(true);
    stopScanBtn->setEnabled(false);
    
    scanProgress->setVisible(false);
    scanStatus->setText("Status: Stopped");
    scanStatus->setStyleSheet("font-weight: bold; color: #f44336;");
    
    scanOutput->append("<font color='#f44336'>[SCAN] Network scan stopped by user</font>");
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('NetworkMonitor', 'Info', :msg)");
    q.bindValue(":msg", "Network scan stopped");
    q.exec();
}

void NetworkMonitor::updateNetworkScan() {
    if (!isScanning || currentScanTarget >= scanTargets.size()) {
        // Scan complete
        isScanning = false;
        scanTimer->stop();
        
        startScanBtn->setEnabled(true);
        stopScanBtn->setEnabled(false);
        scanProgress->setVisible(false);
        
        scanStatus->setText("Status: Complete");
        scanStatus->setStyleSheet("font-weight: bold; color: #4caf50;");
        
        scanOutput->append(QString("<font color='#4caf50'>[COMPLETE] Network scan finished. Scanned %1 hosts</font>")
                          .arg(scanTargets.size()));
        
        // Log completion
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('NetworkMonitor', 'Info', :msg)");
        q.bindValue(":msg", QString("Network scan completed: %1 hosts scanned").arg(scanTargets.size()));
        q.exec();
        
        return;
    }
    
    // Scan current target
    QString host = scanTargets[currentScanTarget];
    scanOutput->append(QString("<font color='#2196f3'>[SCANNING] %1</font>").arg(host));
    
    // Simulate host discovery and port scanning
    bool hostUp = QRandomGenerator::global()->bounded(100) < 70; // 70% chance host is up
    
    if (hostUp) {
        QString ports = simulatePortScan(host);
        QString os = osDetectionCheck->isChecked() ? detectOS(host) : "Unknown";
        QString services = serviceDetectionCheck->isChecked() ? "HTTP, SSH, FTP" : "Unknown";
        
        addScanResult(host, "🟢 Up", ports, os, services);
        scanOutput->append(QString("<font color='#4caf50'>[RESULT] %1 is up - %2</font>").arg(host).arg(ports));
    } else {
        addScanResult(host, "🔴 Down", "N/A", "N/A", "N/A");
        scanOutput->append(QString("<font color='#666'>[RESULT] %1 is down or filtered</font>").arg(host));
    }
    
    currentScanTarget++;
    scanProgress->setValue(currentScanTarget);
}

QString NetworkMonitor::simulatePortScan(const QString &host) {
    QStringList commonPorts = {"22", "23", "25", "53", "80", "110", "143", "443", "993", "995"};
    QStringList openPorts;
    
    int numOpenPorts = qrand() % 5; // 0-4 open ports
    int numOpenPorts = QRandomGenerator::global()->bounded(5); // 0-4 open ports
    for (int i = 0; i < numOpenPorts; ++i) {
        if (!commonPorts.isEmpty()) {
            int index = QRandomGenerator::global()->bounded(commonPorts.size());
            openPorts.append(commonPorts.takeAt(index));
        }
    }
    
    if (openPorts.isEmpty()) {
        return "No open ports";
    }
    
    return openPorts.join(", ");
}

QString NetworkMonitor::detectOS(const QString &host) {
    QStringList osList = {
        "Linux 5.4.0",
        "Windows 10",
        "Windows Server 2019",
        "macOS 12.0",
        "FreeBSD 13.0",
        "Ubuntu 20.04",
        "CentOS 8"
    };
    
    return osList[qrand() % osList.size()];
    return osList[QRandomGenerator::global()->bounded(osList.size())];
}

void NetworkMonitor::addScanResult(const QString &host, const QString &status, const QString &ports, const QString &os, const QString &services) {
    int row = scanResultsTable->rowCount();
    scanResultsTable->insertRow(row);
    
    scanResultsTable->setItem(row, 0, new QTableWidgetItem(host));
    
    auto *statusItem = new QTableWidgetItem(status);
    if (status.contains("Up")) {
        statusItem->setForeground(QColor("#4caf50"));
    } else {
        statusItem->setForeground(QColor("#f44336"));
    }
    scanResultsTable->setItem(row, 1, statusItem);
    
    scanResultsTable->setItem(row, 2, new QTableWidgetItem(ports));
    scanResultsTable->setItem(row, 3, new QTableWidgetItem(os));
    scanResultsTable->setItem(row, 4, new QTableWidgetItem(services));
}

void NetworkMonitor::clearScanResults() {
    scanResultsTable->setRowCount(0);
    scanOutput->clear();
    scanOutput->append("<font color='#ff9800'>[CLEAR] Scan results cleared</font>");
}

void NetworkMonitor::exportPackets() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Network Data", 
        QString("NetworkMonitor_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        
        if (monitoringTabs->currentIndex() == 0) {
            // Export packet capture data
            out << "Time,Source,Destination,Protocol,Length,Info\n";
            for (int row = 0; row < packetTable->rowCount(); ++row) {
                QStringList rowData;
                for (int col = 0; col < packetTable->columnCount(); ++col) {
                    QTableWidgetItem *item = packetTable->item(row, col);
                    rowData << (item ? item->text() : "");
                }
                out << rowData.join(",") << "\n";
            }
        } else {
            // Export scan results
            out << "Host,Status,Open_Ports,OS,Services\n";
            for (int row = 0; row < scanResultsTable->rowCount(); ++row) {
                QStringList rowData;
                for (int col = 0; col < scanResultsTable->columnCount(); ++col) {
                    QTableWidgetItem *item = scanResultsTable->item(row, col);
                    rowData << (item ? item->text() : "");
                }
                out << rowData.join(",") << "\n";
            }
        }
        
        file.close();
        QMessageBox::information(this, "Export Complete", 
            QString("Network data exported to:\n%1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Export Failed", "Could not save file.");
    }
}

void NetworkMonitor::filterPackets() {
    // Implementation for packet filtering would go here
}

void NetworkMonitor::scanSingleHost() {
    // Implementation for single host detailed scan would go here
}

void NetworkMonitor::performPortScan() {
    // Implementation for detailed port scanning would go here
}