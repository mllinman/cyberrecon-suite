#include "ThemeSelector.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QGroupBox>
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <QSlider>
#include <QSpinBox>

ThemeSelector::ThemeSelector(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("🎨 Theme & Interface Customization");
    title->setStyleSheet("color: #00bcd4; font: bold 18pt; margin: 10px;");
    layout->addWidget(title);

    // Theme Selection Group
    auto *themeGroup = new QGroupBox("Color Themes");
    themeGroup->setStyleSheet("QGroupBox { font-weight: bold; margin: 10px; }");
    auto *themeLayout = new QVBoxLayout(themeGroup);

    themeBox = new QComboBox();
    themeBox->addItem("🔵 Dark Slate with Light Blue", "dark_slate_blue");
    themeBox->addItem("🟠 Dark Slate with Orange", "dark_slate_orange");
    themeBox->addItem("🔴 Dark Slate with Red", "dark_slate_red");
    themeBox->addItem("🟢 Dark Slate with Bright Green", "dark_slate_green");
    themeBox->addItem("⚪ Dark Slate with White", "dark_slate_white");
    themeBox->addItem("🎨 Custom Theme", "custom");
    themeBox->setStyleSheet("QComboBox { padding: 10px; font-size: 12pt; }");
    themeLayout->addWidget(themeBox);

    // Theme preview
    auto *previewLayout = new QHBoxLayout();
    
    auto *applyBtn = new QPushButton("Apply Theme");
    applyBtn->setStyleSheet("QPushButton { background-color: #00bcd4; color: white; padding: 10px 20px; border-radius: 6px; font-weight: bold; }");
    
    auto *resetBtn = new QPushButton("Reset to Default");
    resetBtn->setStyleSheet("QPushButton { background-color: #607d8b; color: white; padding: 10px 20px; border-radius: 6px; }");
    
    previewLayout->addWidget(applyBtn);
    previewLayout->addWidget(resetBtn);
    previewLayout->addStretch();
    themeLayout->addLayout(previewLayout);
    
    layout->addWidget(themeGroup);
    
    // Custom Theme Builder
    auto *customGroup = new QGroupBox("Custom Theme Builder");
    customGroup->setStyleSheet("QGroupBox { font-weight: bold; margin: 10px; }");
    auto *customLayout = new QGridLayout(customGroup);
    
    // Color pickers
    customLayout->addWidget(new QLabel("Primary Color:"), 0, 0);
    auto *primaryColorBtn = new QPushButton("#00bcd4");
    primaryColorBtn->setStyleSheet("QPushButton { background-color: #00bcd4; color: white; padding: 8px; border-radius: 4px; }");
    customLayout->addWidget(primaryColorBtn, 0, 1);
    
    customLayout->addWidget(new QLabel("Accent Color:"), 1, 0);
    auto *accentColorBtn = new QPushButton("#ff9800");
    accentColorBtn->setStyleSheet("QPushButton { background-color: #ff9800; color: white; padding: 8px; border-radius: 4px; }");
    customLayout->addWidget(accentColorBtn, 1, 1);
    
    customLayout->addWidget(new QLabel("Background Darkness:"), 2, 0);
    auto *darknessSlider = new QSlider(Qt::Horizontal);
    darknessSlider->setRange(0, 100);
    darknessSlider->setValue(85);
    customLayout->addWidget(darknessSlider, 2, 1);
    
    layout->addWidget(customGroup);
    
    // Interface Settings
    auto *interfaceGroup = new QGroupBox("Interface Settings");
    interfaceGroup->setStyleSheet("QGroupBox { font-weight: bold; margin: 10px; }");
    auto *interfaceLayout = new QVBoxLayout(interfaceGroup);
    
    auto *animationsCheck = new QCheckBox("Enable smooth animations");
    animationsCheck->setChecked(true);
    interfaceLayout->addWidget(animationsCheck);
    
    auto *glowEffectsCheck = new QCheckBox("Enable glow effects");
    glowEffectsCheck->setChecked(true);
    interfaceLayout->addWidget(glowEffectsCheck);
    
    auto *compactModeCheck = new QCheckBox("Compact sidebar mode");
    compactModeCheck->setChecked(false);
    interfaceLayout->addWidget(compactModeCheck);
    
    layout->addWidget(interfaceGroup);
    
    // Status
    statusLabel = new QLabel("Ready to customize your interface");
    statusLabel->setStyleSheet("color: #4caf50; font-weight: bold; padding: 10px;");
    layout->addWidget(statusLabel);

    // Connect signals
    connect(applyBtn, &QPushButton::clicked, this, &ThemeSelector::applySelectedTheme);
    connect(resetBtn, &QPushButton::clicked, this, &ThemeSelector::resetTheme);
    connect(themeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThemeSelector::onThemeChanged);
    connect(primaryColorBtn, &QPushButton::clicked, [this, primaryColorBtn]() {
        QColor color = QColorDialog::getColor(QColor("#00bcd4"), this, "Select Primary Color");
        if (color.isValid()) {
            primaryColorBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 8px; border-radius: 4px; }").arg(color.name()));
            primaryColorBtn->setText(color.name());
        }
    });
    connect(accentColorBtn, &QPushButton::clicked, [this, accentColorBtn]() {
        QColor color = QColorDialog::getColor(QColor("#ff9800"), this, "Select Accent Color");
        if (color.isValid()) {
            accentColorBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 8px; border-radius: 4px; }").arg(color.name()));
            accentColorBtn->setText(color.name());
        }
    });

    // Load saved settings
    loadSettings();
}

void ThemeSelector::applySelectedTheme() {
    QString themeKey = themeBox->currentData().toString();
    QString themeCSS = generateThemeCSS(themeKey);
    
    qApp->setStyleSheet(themeCSS);
    saveTheme(themeKey);
    
    statusLabel->setText("✅ Theme applied successfully!");
    statusLabel->setStyleSheet("color: #4caf50; font-weight: bold; padding: 10px;");
}

void ThemeSelector::resetTheme() {
    themeBox->setCurrentIndex(0); // Default blue theme
    applySelectedTheme();
    statusLabel->setText("🔄 Theme reset to default");
    statusLabel->setStyleSheet("color: #ff9800; font-weight: bold; padding: 10px;");
}

void ThemeSelector::onThemeChanged() {
    statusLabel->setText("Theme selected - click Apply to activate");
    statusLabel->setStyleSheet("color: #2196f3; font-weight: bold; padding: 10px;");
}

QString ThemeSelector::generateThemeCSS(const QString &themeKey) {
    QString primaryColor, accentColor, hoverColor;
    
    if (themeKey == "dark_slate_blue") {
        primaryColor = "#2196f3";
        accentColor = "#64b5f6";
        hoverColor = "rgba(33, 150, 243, 0.3)";
    } else if (themeKey == "dark_slate_orange") {
        primaryColor = "#ff9800";
        accentColor = "#ffb74d";
        hoverColor = "rgba(255, 152, 0, 0.3)";
    } else if (themeKey == "dark_slate_red") {
        primaryColor = "#f44336";
        accentColor = "#ef5350";
        hoverColor = "rgba(244, 67, 54, 0.3)";
    } else if (themeKey == "dark_slate_green") {
        primaryColor = "#4caf50";
        accentColor = "#66bb6a";
        hoverColor = "rgba(76, 175, 80, 0.3)";
    } else if (themeKey == "dark_slate_white") {
        primaryColor = "#ffffff";
        accentColor = "#f5f5f5";
        hoverColor = "rgba(255, 255, 255, 0.3)";
    } else {
        // Default blue
        primaryColor = "#00bcd4";
        accentColor = "#26c6da";
        hoverColor = "rgba(0, 188, 212, 0.3)";
    }
    
    return QString(R"(
        /* Main Application */
        QMainWindow {
            background-color: #0f0f0f;
            color: #ffffff;
        }
        
        /* Modern Sidebar */
        QFrame#modernSidebar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #1a1a1a, stop:1 #2d2d2d);
            border-right: 2px solid #333333;
        }
        
        /* Logo Section */
        QFrame#logoFrame {
            background-color: rgba(%1, 0.1);
            border-radius: 10px;
            margin: 10px;
            padding: 15px;
        }
        
        QLabel#logoLabel {
            color: %1;
            font-size: 24px;
            font-weight: bold;
            margin: 5px;
        }
        
        /* Section Labels */
        QLabel#sectionLabel {
            color: %1;
            font-size: 11px;
            font-weight: bold;
            margin: 15px 20px 5px 20px;
            letter-spacing: 1px;
        }
        
        /* Sidebar Buttons */
        QPushButton#sidebarButton:hover {
            background-color: %3;
        }
        
        QPushButton#sidebarButton:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %3, stop:1 rgba(%1, 0.1));
            border-left: 4px solid %1;
        }
        
        QPushButton#sidebarButton:checked QLabel#buttonIcon {
            color: %1;
        }
        
        QPushButton#sidebarButton:checked QLabel#buttonText {
            color: %1;
            font-weight: bold;
        }
        
        /* User Area */
        QFrame#userArea {
            background-color: %3;
            border: 1px solid rgba(%1, 0.3);
            border-radius: 25px;
        }
        
        QLabel#userAvatar {
            background-color: %1;
            border-radius: 20px;
            font-size: 20px;
            color: #000000;
        }
        
        /* Top Bar Buttons */
        QPushButton#topBarButton:hover {
            background-color: %3;
            border-color: %1;
        }
        
        /* General Buttons */
        QPushButton {
            background-color: %1;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 6px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: %2;
        }
        
        QPushButton:pressed {
            background-color: rgba(%1, 0.8);
        }
        
        /* Input Fields */
        QLineEdit, QTextEdit, QComboBox {
            background-color: #2d2d2d;
            border: 2px solid #444;
            border-radius: 6px;
            padding: 8px;
            color: #ffffff;
        }
        
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: %1;
        }
        
        /* Progress Bars */
        QProgressBar {
            border: 2px solid #444;
            border-radius: 8px;
            text-align: center;
            color: white;
            background-color: #2d2d2d;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %1, stop:1 %2);
            border-radius: 6px;
        }
        
        /* Tables */
        QTableWidget {
            background-color: #1e1e1e;
            alternate-background-color: #2d2d2d;
            gridline-color: #444;
            color: #ffffff;
            border: 1px solid #444;
            border-radius: 6px;
        }
        
        QHeaderView::section {
            background-color: #333;
            color: %1;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        
        /* Group Boxes */
        QGroupBox {
            font-weight: bold;
            border: 2px solid #444;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 15px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px 0 8px;
            color: %1;
            font-size: 14px;
        }
    )").arg(primaryColor).arg(accentColor).arg(hoverColor);
}

void ThemeSelector::saveTheme(const QString &themePath) {
    QSettings settings("CyberRecon", "CyberReconSuite");
    settings.setValue("ui/theme", themePath);
}

QString ThemeSelector::loadTheme() {
    QSettings settings("CyberRecon", "CyberReconSuite");
    return settings.value("ui/theme").toString();
}

void ThemeSelector::saveSettings() {
    QSettings settings("CyberRecon", "CyberReconSuite");
    settings.setValue("ui/theme", themeBox->currentData().toString());
    
    statusLabel->setText("💾 Settings saved successfully");
    statusLabel->setStyleSheet("color: #4caf50; font-weight: bold; padding: 10px;");
}

void ThemeSelector::loadSettings() {
    QSettings settings("CyberRecon", "CyberReconSuite");
    QString savedTheme = settings.value("ui/theme", "dark_slate_blue").toString();
    
    int index = themeBox->findData(savedTheme);
    if (index >= 0) {
        themeBox->setCurrentIndex(index);
        QString themeCSS = generateThemeCSS(savedTheme);
        qApp->setStyleSheet(themeCSS);
    }
}