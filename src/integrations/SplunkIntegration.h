#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QProgressBar>
#include <QTimer>
#include <QSqlDatabase>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class SplunkIntegration : public QWidget {
    Q_OBJECT

public:
    explicit SplunkIntegration(QWidget *parent = nullptr);

private slots:
    void executeSearch();
    void saveSearch();
    void loadSavedSearch();
    void createAlert();
    void exportResults();
    void refreshDashboard();
    void onSearchComplete();

private:
    void setupUI();
    void setupStyling();
    void initializeDatabase();
    void loadSampleData();
    void executeSearchQuery(const QString &query);
    void updateSearchResults(const QJsonObject &results);
    void createVisualization(const QString &type);
    
    // UI Components
    QLineEdit *searchInput;
    QComboBox *timeRangeCombo;
    QComboBox *indexCombo;
    QComboBox *visualizationCombo;
    QTableWidget *resultsTable;
    QTextEdit *queryEditor;
    QTextEdit *searchResults;
    QProgressBar *searchProgress;
    
    // Search functionality
    QTimer *searchTimer;
    QSqlDatabase splunkDb;
    QNetworkAccessManager *networkManager;
    
    // Data
    QString currentQuery;
    int totalResults;
    bool isSearching;
};