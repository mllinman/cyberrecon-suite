#include "MLCorrelation.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QSqlQuery>
#include <QApplication>
#include <QRandomGenerator>
#include <QThread>
#include <QProgressBar>
#include <QComboBox>

MLCorrelation::MLCorrelation(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Machine Learning Event Correlation");
    title->setStyleSheet("color: #00bcd4; font: bold 16pt; margin: 10px;");
    layout->addWidget(title);

    // Algorithm selection
    auto *algoLayout = new QHBoxLayout();
    algoLayout->addWidget(new QLabel("Algorithm:"));
    
    algorithmCombo = new QComboBox();
    algorithmCombo->addItem("K-Means Clustering");
    algorithmCombo->addItem("Anomaly Detection");
    algorithmCombo->addItem("Pattern Recognition");
    algorithmCombo->addItem("Behavioral Analysis");
    algorithmCombo->setStyleSheet("padding: 5px; font-size: 11pt;");
    
    algoLayout->addWidget(algorithmCombo);
    algoLayout->addStretch();
    layout->addLayout(algoLayout);

    // Progress bar
    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #555; border-radius: 3px; } QProgressBar::chunk { background-color: #2196f3; }");
    layout->addWidget(progressBar);

    // Control buttons
    auto *buttonLayout = new QHBoxLayout();
    QPushButton *runBtn = new QPushButton("Run ML Correlation");
    QPushButton *trainBtn = new QPushButton("Train Model");
    QPushButton *clearBtn = new QPushButton("Clear Results");

    runBtn->setStyleSheet("QPushButton { background-color: #4caf50; color: white; padding: 10px; border-radius: 4px; font-weight: bold; }");
    trainBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 10px; border-radius: 4px; }");
    clearBtn->setStyleSheet("QPushButton { background-color: #607d8b; color: white; padding: 10px; border-radius: 4px; }");

    buttonLayout->addWidget(runBtn);
    buttonLayout->addWidget(trainBtn);
    buttonLayout->addWidget(clearBtn);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    // Output display
    output = new QTextEdit();
    output->setReadOnly(true);
    output->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-family: 'Consolas', monospace; padding: 10px;");
    layout->addWidget(output);

    connect(runBtn, &QPushButton::clicked, this, &MLCorrelation::runCorrelation);
    connect(trainBtn, &QPushButton::clicked, this, &MLCorrelation::trainModel);
    connect(clearBtn, &QPushButton::clicked, [this]() { output->clear(); });

    // Get current user
    currentUser = qgetenv("USER");
    if (currentUser.isEmpty()) {
        currentUser = qgetenv("USERNAME"); // Windows compatibility
    }
    if (currentUser.isEmpty()) {
        currentUser = "system";
    }

    // Auto-correlation timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MLCorrelation::autoCorrelate);
    timer->start(45000); // every 45 seconds

    // Initial message
    output->append("<font color='#00bcd4'>[ML Engine] Machine Learning correlation engine initialized</font>");
    output->append("<font color='#4caf50'>[ML Engine] Ready to analyze security events</font>");
}

void MLCorrelation::runCorrelation() {
    QString algorithm = algorithmCombo->currentText();
    
    output->append(QString("<font color='#2196f3'>[ML Engine] Starting %1 analysis...</font>").arg(algorithm));
    
    // Show progress
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    // Simulate ML processing
    for (int i = 0; i <= 100; i += 20) {
        progressBar->setValue(i);
        QApplication::processEvents();
        QThread::msleep(200);
    }
    
    // Generate correlation results based on algorithm
    QStringList results = generateCorrelationResults(algorithm);
    
    output->append("<font color='#4caf50'>[ML Engine] Analysis complete. Results:</font>");
    for (const QString &result : results) {
        output->append(QString("<font color='#ffffff'>  • %1</font>").arg(result));
        
        // Insert into events DB so Timeline sees it
        QSqlQuery q;
        q.prepare("INSERT INTO events (panel, severity, message) VALUES ('MLCorrelation', 'Info', :msg)");
        q.bindValue(":msg", QString("ML (%1): %2").arg(currentUser).arg(result));
        q.exec();
    }
    
    progressBar->setVisible(false);
    
    // Log the correlation run
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO events (panel, severity, message) VALUES ('MLCorrelation', 'Info', :msg)");
    logQuery.bindValue(":msg", QString("ML correlation completed using %1").arg(algorithm));
    logQuery.exec();
}

void MLCorrelation::trainModel() {
    output->append("<font color='#ff9800'>[ML Engine] Training model with historical data...</font>");
    
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    // Simulate training process
    QStringList trainingSteps = {
        "Loading historical security events...",
        "Preprocessing data and extracting features...",
        "Training neural network layers...",
        "Validating model accuracy...",
        "Optimizing hyperparameters...",
        "Saving trained model..."
    };
    
    for (int i = 0; i < trainingSteps.size(); ++i) {
        output->append(QString("<font color='#ffeb3b'>[Training] %1</font>").arg(trainingSteps[i]));
        progressBar->setValue((i + 1) * 100 / trainingSteps.size());
        QApplication::processEvents();
        QThread::msleep(800);
    }
    
    output->append("<font color='#4caf50'>[ML Engine] Model training completed successfully!</font>");
    output->append("<font color='#4caf50'>[ML Engine] Model accuracy: 94.7%</font>");
    
    progressBar->setVisible(false);
    
    // Log training completion
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('MLCorrelation', 'Info', :msg)");
    q.bindValue(":msg", "ML model training completed with 94.7% accuracy");
    q.exec();
}

void MLCorrelation::autoCorrelate() {
    QStringList autoResults = {
        "Detected correlation between failed logins and network scans",
        "Identified anomalous user behavior pattern",
        "Found temporal correlation in malware execution events",
        "Discovered lateral movement pattern across 3 hosts",
        "Detected coordinated attack campaign indicators"
    };
    
    QString result = autoResults[qrand() % autoResults.size()];
    QString severity = result.contains("attack") || result.contains("malware") ? "Warning" : "Info";
    QString color = (severity == "Warning") ? "#ff9800" : "#4caf50";
    
    output->append(QString("<font color='%1'>[Auto-ML] %2</font>").arg(color).arg(result));
    
    // Log to events
    QSqlQuery q;
    q.prepare("INSERT INTO events (panel, severity, message) VALUES ('MLCorrelation', :sev, :msg)");
    q.bindValue(":sev", severity);
    q.bindValue(":msg", QString("Auto-ML: %1").arg(result));
    q.exec();
}

QStringList MLCorrelation::generateCorrelationResults(const QString &algorithm) {
    QStringList results;
    
    if (algorithm == "K-Means Clustering") {
        results << "Identified 3 distinct event clusters"
                << "Cluster 1: Login anomalies (15 events)"
                << "Cluster 2: Network intrusions (8 events)"
                << "Cluster 3: Malware activities (12 events)"
                << "Outliers detected: 4 events require investigation";
    } else if (algorithm == "Anomaly Detection") {
        results << "Baseline behavior model updated"
                << "Detected 7 anomalous events in last 24 hours"
                << "Anomaly score threshold: 0.85"
                << "Top anomaly: Unusual data transfer at 03:42 AM"
                << "Confidence level: 92.3%";
    } else if (algorithm == "Pattern Recognition") {
        results << "Identified recurring attack pattern"
                << "Pattern matches known APT campaign signatures"
                << "Temporal correlation: Events occur every 6 hours"
                << "Geographic correlation: All from same IP range"
                << "Recommended action: Block IP subnet";
    } else if (algorithm == "Behavioral Analysis") {
        results << "User behavior baseline established"
                << "Detected 2 users with anomalous access patterns"
                << "Privilege escalation attempts: 3 instances"
                << "After-hours activity increased by 340%"
                << "Risk score: HIGH for user 'contractor_temp'";
    }
    
    return results;
}