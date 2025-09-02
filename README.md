# CyberRecon Suite v1.7 - Modern Cybersecurity Operations Platform

A comprehensive cybersecurity operations platform built with C++ and Qt6, featuring a modern sleek dark interface with sidebar navigation, advanced authentication, user profiles, subscription management, wireless penetration testing tools, and security orchestration.

## 🎨 Modern UI Features

- **Sleek Dark Interface**: Professional dark slate theme with 6 customizable variants
- **Left Sidebar Navigation**: Organized module groups (Dashboards, Intelligence, Automation, Operations, Administration)
- **Professional Authentication**: Splash screen, secure login, and subscription management
- **Stripe Payment Integration**: Complete payment processing for subscriptions
- **Wireless Penetration Testing**: WiFi and Bluetooth security assessment tools
- **Network Monitoring Suite**: Packet capture and network scanning capabilities

## Prerequisites

- Qt6 (Core, Widgets, Sql, Network, Charts, PrintSupport)
- CMake 3.16 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)
- OpenGL libraries (for Qt6 Charts visualization)

## 🎨 Available Themes

1. **🔵 Dark Slate with Light Blue** (Default)
2. **🟠 Dark Slate with Orange**
3. **🔴 Dark Slate with Red**
4. **🟢 Dark Slate with Bright Green**
5. **⚪ Dark Slate with White**
6. **🎨 Custom Theme Builder** (Color picker + darkness slider)

## Demo Credentials

⚠️ **IMPORTANT**: By using these credentials, you agree to the penetration testing disclaimer and will only use security testing tools on authorized systems.

For testing purposes, use these credentials:

**Professional Plan User:**
- Username: `demo`
- Password: `demo123`

**Enterprise Plan User:**
- Username: `admin`
- Password: `admin123`

## Building the Application

### Linux/macOS
```bash
# Make build script executable
chmod +x build.sh

# Build the application
./build.sh
```

### Windows
```bash
# Using the batch script
build.bat

# Or manually with PowerShell/Command Prompt
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

## Running the Application

After building, run the executable:
```bash
# Linux/macOS
./build/CyberReconSuite

# Windows
./build/Release/CyberReconSuite.exe
```

## New Features in v1.7

### 📡 Wireless Penetration Testing Suite
- **WiFi Security Testing**: Network discovery, deauth attacks, evil twin APs, WPS attacks
- **Bluetooth Security Testing**: Device discovery, MAC spoofing, BlueJacking, BlueSnarfing
- **Professional Interface**: Real-time monitoring with export capabilities
- **Ethical Guidelines**: Clear authorization warnings and best practices

### 🌐 Network Monitoring & Analysis
- **Packet Capture**: Wireshark-like packet analysis with filtering
- **Network Scanner**: Nmap-like host discovery and port scanning
- **Protocol Analysis**: Deep packet inspection and protocol decoding
- **Export Capabilities**: CSV export for further analysis

### 🔐 Authentication System
- Professional splash screen with loading animation
- Secure login with user management
- Subscription-based access control
- Remember me functionality

### 👤 User Profile Management
- **Profile Tab**: Avatar upload, personal information, bio
- **Settings Tab**: Appearance, notifications, performance settings
- **Billing Tab**: Current plan, billing history, invoice downloads
- **Subscription Tab**: Usage metrics, feature comparison, upgrade options

### 💳 Stripe Payment Integration
- Complete Stripe API integration for secure payments
- Professional payment forms with real-time validation
- Subscription management (create, update, cancel)
- Test card support for development

### 🔍 Advanced Threat Intelligence
- Multi-source IOC enrichment (VirusTotal, AlienVault OTX, Abuse.CH)
- Real-time threat feed updates
- IOC categorization and severity scoring
- Export capabilities for threat indicators

### 🔄 Security Orchestration & Automated Response (SOAR)
- Visual workflow execution with real-time progress
- Predefined incident response playbooks
- Custom workflow creation capabilities
- Automated step execution with logging

### 💼 Subscription Plans

**🆓 Free Plan** - Basic monitoring, 1,000 events/day
**💼 Professional** - Full SIEM/EDR, 100,000 events/day, $99/month
**🏢 Enterprise** - Advanced ML, unlimited events, 24/7 support, $299/month

## Features

- **SIEM Dashboard**: Real-time security event monitoring
- **EDR Monitoring**: Endpoint detection and response
- **Network Monitoring**: Packet capture and network scanning (Wireshark/Nmap-like tools)
- **Wireless Penetration Testing**: WiFi and Bluetooth security testing
- **Penetration Testing Toolkit**: Web application testing, password cracking, social engineering
- **Threat Intelligence Hub**: IOC enrichment and analysis
- **Digital Forensics**: Investigation tools and evidence management
- **Compliance Dashboard**: NIST, GDPR, HIPAA tracking
- **SOAR Automation**: Security orchestration and response
- **Case Management**: Incident tracking and collaboration
- **Attack Graph Visualization**: Threat modeling and analysis
- **ML Correlation Engine**: Advanced event correlation
- **SOC 2 Compliance**: Trust Service Criteria monitoring and audit trail
- **Team Collaboration**: Real-time chat and coordination
- **User Authentication**: Secure login and session management
- **Profile Management**: Comprehensive user settings and preferences
- **Subscription Management**: Tiered access control and billing

## Database

The application uses SQLite for data persistence. Multiple database files are created automatically:
- `cyberrecon_main.db` - Main events and timeline data
- `cyberrecon_auth.db` - User authentication and profiles
- `cyberrecon_cases.db` - Case management and evidence
- `soar_rules.db` - SOAR automation rules
- `soc2_compliance.db` - SOC 2 compliance controls and audit data
- `cyberrecon_audit.db` - Comprehensive audit trail for compliance

## Stripe Integration

To enable payment processing:

1. Get your Stripe API keys from https://dashboard.stripe.com
2. Update `src/payments/PaymentDialog.cpp` with your secret key
3. Create products in Stripe dashboard for Professional and Enterprise plans
4. Test with Stripe test cards (4242 4242 4242 4242)

## Architecture

The application follows a modular architecture with clear separation of concerns:
- **Authentication Layer**: Secure login and session management
- **Data Layer**: SQLite databases with proper schema design
- **Business Logic**: Security operations and automation
- **Presentation Layer**: Modern Qt6 interface with responsive design
- **Integration Layer**: External API connections and data feeds
- **Payment Layer**: Stripe integration for subscription management

## Project Structure

```
src/
├── auth/                 # Authentication system
├── profile/              # User profile management
├── monitoring/           # Network monitoring tools
├── modern/               # Modern UI components
├── dashboards/           # Security dashboards
├── automation/           # SOAR automation
├── admin/                # Administration tools
├── ui/                   # UI components and themes
├── updater/              # Application updater
├── payments/             # Stripe payment integration
└── main.cpp             # Application entry point
```