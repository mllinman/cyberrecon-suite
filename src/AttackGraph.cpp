#include "AttackGraph.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QSqlQuery>
#include <QApplication>
#include <QThread>
#include <QRandomGenerator>
#include <cmath>

AttackGraph::AttackGraph(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Attack Graph Visualization");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setStyleSheet("background-color: #1e1e1e; border: 1px solid #555555;");
    layout->addWidget(view);

    auto *buttonLayout = new QHBoxLayout();
    QPushButton *generateBtn = new QPushButton("Generate Attack Graph");
    QPushButton *clearBtn = new QPushButton("Clear Graph");
    QPushButton *exportBtn = new QPushButton("Export Graph");

    generateBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    clearBtn->setStyleSheet("QPushButton { background-color: #607d8b; color: white; padding: 8px; border-radius: 4px; }");
    exportBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 8px; border-radius: 4px; }");

    buttonLayout->addWidget(generateBtn);
    buttonLayout->addWidget(clearBtn);
    buttonLayout->addWidget(exportBtn);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    connect(generateBtn, &QPushButton::clicked, this, &AttackGraph::generateGraph);
    connect(clearBtn, &QPushButton::clicked, this, &AttackGraph::clearGraph);
    connect(exportBtn, &QPushButton::clicked, this, &AttackGraph::exportGraph);

    // Auto-update timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AttackGraph::updateGraph);
    timer->start(30000); // every 30 seconds

    generateGraph();
}

void AttackGraph::generateGraph() {
    clearGraph();
    
    // Create attack chain nodes
    createNode(100, 100, "Initial Access", "#ff5722", "Phishing Email");
    createNode(300, 100, "Execution", "#ff9800", "Malicious Macro");
    createNode(500, 100, "Persistence", "#ffc107", "Registry Key");
    createNode(700, 100, "Privilege Escalation", "#ffeb3b", "UAC Bypass");
    
    createNode(100, 250, "Defense Evasion", "#cddc39", "Process Hollowing");
    createNode(300, 250, "Credential Access", "#8bc34a", "LSASS Dump");
    createNode(500, 250, "Discovery", "#4caf50", "Network Scan");
    createNode(700, 250, "Lateral Movement", "#009688", "SMB/WMI");
    
    createNode(100, 400, "Collection", "#00bcd4", "File Enumeration");
    createNode(300, 400, "C&C", "#2196f3", "DNS Tunneling");
    createNode(500, 400, "Exfiltration", "#3f51b5", "HTTPS Upload");
    createNode(700, 400, "Impact", "#9c27b0", "Data Encryption");

    // Create connections between nodes
    createConnection(100, 100, 300, 100); // Initial Access -> Execution
    createConnection(300, 100, 500, 100); // Execution -> Persistence
    createConnection(500, 100, 700, 100); // Persistence -> Privilege Escalation
    createConnection(700, 100, 100, 250); // Privilege Escalation -> Defense Evasion
    createConnection(100, 250, 300, 250); // Defense Evasion -> Credential Access
    createConnection(300, 250, 500, 250); // Credential Access -> Discovery
    createConnection(500, 250, 700, 250); // Discovery -> Lateral Movement
    createConnection(700, 250, 100, 400); // Lateral Movement -> Collection
    createConnection(100, 400, 300, 400); // Collection -> C&C
    createConnection(300, 400, 500, 400); // C&C -> Exfiltration
    createConnection(500, 400, 700, 400); // Exfiltration -> Impact

    // Log graph generation
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('AttackGraph', 'Info', :msg)");
    q.bindValue(":msg", "Attack graph generated with 12 nodes");
    q.exec();
}

void AttackGraph::clearGraph() {
    scene->clear();
}

void AttackGraph::exportGraph() {
    // In a real implementation, this would export to PNG/SVG
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('AttackGraph', 'Info', :msg)");
    q.bindValue(":msg", "Attack graph exported (simulated)");
    q.exec();
}

void AttackGraph::updateGraph() {
    // Simulate dynamic updates to the attack graph
    static int updateCount = 0;
    updateCount++;
    
    // Add a new threat indicator
    if (updateCount % 3 == 0) {
        createNode(400 + (updateCount * 20), 500, "New Threat", "#e91e63", "IOC Detected");
        
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('AttackGraph', 'Warning', :msg)");
        q.bindValue(":msg", "New threat node added to attack graph");
        q.exec();
    }
}

void AttackGraph::createNode(int x, int y, const QString &title, const QString &color, const QString &description) {
    // Create node circle
    QGraphicsEllipseItem *node = scene->addEllipse(x - 30, y - 30, 60, 60);
    node->setBrush(QBrush(QColor(color)));
    node->setPen(QPen(QColor("#ffffff"), 2));

    // Add title text
    QGraphicsTextItem *titleText = scene->addText(title, QFont("Arial", 8, QFont::Bold));
    titleText->setPos(x - 25, y - 10);
    titleText->setDefaultTextColor(QColor("#ffffff"));

    // Add description text
    QGraphicsTextItem *descText = scene->addText(description, QFont("Arial", 6));
    descText->setPos(x - 30, y + 35);
    descText->setDefaultTextColor(QColor("#cccccc"));
}

void AttackGraph::createConnection(int x1, int y1, int x2, int y2) {
    QGraphicsLineItem *line = scene->addLine(x1, y1, x2, y2);
    line->setPen(QPen(QColor("#888888"), 2));
    
    // Add arrow head (simplified)
    int dx = x2 - x1;
    int dy = y2 - y1;
    double length = sqrt(dx*dx + dy*dy);
    if (length > 0) {
        double arrowLength = 10;
        double arrowX = x2 - (dx / length) * arrowLength;
        double arrowY = y2 - (dy / length) * arrowLength;
        
        scene->addLine(arrowX - 5, arrowY - 5, x2, y2, QPen(QColor("#888888"), 2));
        scene->addLine(arrowX + 5, arrowY + 5, x2, y2, QPen(QColor("#888888"), 2));
    }
}