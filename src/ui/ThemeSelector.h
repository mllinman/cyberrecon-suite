#pragma once
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSettings>

class ThemeSelector : public QWidget {
    Q_OBJECT
public:
    explicit ThemeSelector(QWidget *parent = nullptr);

private slots:
    void applySelectedTheme();
    void resetTheme();
    void onThemeChanged();
    void saveSettings();

private:
    QString generateThemeCSS(const QString &themeKey);
    void saveTheme(const QString &themePath);
    QString loadTheme();
    void loadSettings();
    
    QComboBox *themeBox;
    QLabel *statusLabel;
};