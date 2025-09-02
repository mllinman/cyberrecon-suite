# CyberRecon Suite - Deployment Guide

## 🚀 Production Deployment Guide

This guide covers deploying CyberRecon Suite in production environments for enterprise cybersecurity operations.

## 📋 Prerequisites

### System Requirements

#### Minimum Requirements
- **CPU**: 4 cores, 2.4 GHz
- **RAM**: 8 GB
- **Storage**: 50 GB SSD
- **Network**: 1 Gbps connection
- **OS**: Linux (Ubuntu 20.04+), Windows Server 2019+, or macOS 12+

#### Recommended Requirements
- **CPU**: 8+ cores, 3.0+ GHz
- **RAM**: 16+ GB
- **Storage**: 200+ GB NVMe SSD
- **Network**: 10 Gbps connection
- **OS**: Ubuntu 22.04 LTS or Windows Server 2022

### Software Dependencies
- Qt6 runtime libraries
- SQLite 3.35+
- OpenSSL 1.1.1+
- Network monitoring tools (optional)

## 🔧 Installation Methods

### Method 1: Binary Distribution (Recommended)

#### Linux
```bash
# Download latest release
wget https://github.com/yourusername/cyberrecon-suite/releases/latest/download/cyberrecon-suite-linux.tar.gz

# Extract and install
tar -xzf cyberrecon-suite-linux.tar.gz
cd cyberrecon-suite
sudo ./install.sh

# Start service
sudo systemctl start cyberrecon-suite
sudo systemctl enable cyberrecon-suite
```

#### Windows
```powershell
# Download and run installer
# CyberReconSuite-v1.7-Setup.exe

# Or manual installation
Expand-Archive CyberReconSuite-Windows.zip -DestinationPath "C:\Program Files\CyberRecon"
cd "C:\Program Files\CyberRecon"
.\CyberReconSuite.exe
```

### Method 2: Source Compilation

#### Build from Source
```bash
# Clone repository
git clone https://github.com/yourusername/cyberrecon-suite.git
cd cyberrecon-suite

# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install -y qt6-base-dev qt6-charts-dev cmake build-essential

# Build application
chmod +x build.sh
./build.sh

# Install system-wide
sudo cp build/CyberReconSuite /usr/local/bin/
sudo cp -r resources /usr/local/share/cyberrecon/
```

## 🔐 Security Configuration

### Database Security
```bash
# Set proper database permissions
chmod 600 *.db
chown cyberrecon:cyberrecon *.db

# Enable database encryption (if supported)
sqlite3 cyberrecon_main.db "PRAGMA key = 'your-encryption-key';"
```

### Network Security
```bash
# Configure firewall rules
sudo ufw allow from 192.168.1.0/24 to any port 8080
sudo ufw deny 8080

# SSL/TLS configuration
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365
```

### User Access Control
```bash
# Create dedicated user account
sudo useradd -r -s /bin/false cyberrecon
sudo usermod -aG cyberrecon your-admin-user

# Set file permissions
sudo chown -R cyberrecon:cyberrecon /opt/cyberrecon/
sudo chmod -R 750 /opt/cyberrecon/
```

## 🌐 Network Configuration

### Port Requirements
- **8080**: Web interface (configurable)
- **8443**: HTTPS interface (configurable)
- **5432**: Database connections (if using external DB)
- **514**: Syslog ingestion (optional)
- **162**: SNMP traps (optional)

### Firewall Configuration
```bash
# Ubuntu/Debian with UFW
sudo ufw allow 8080/tcp
sudo ufw allow 8443/tcp
sudo ufw allow from 192.168.1.0/24 to any port 514

# CentOS/RHEL with firewalld
sudo firewall-cmd --permanent --add-port=8080/tcp
sudo firewall-cmd --permanent --add-port=8443/tcp
sudo firewall-cmd --reload
```

## 📊 Monitoring & Logging

### System Monitoring
```bash
# Monitor application performance
top -p $(pgrep CyberReconSuite)
htop -p $(pgrep CyberReconSuite)

# Check memory usage
ps aux | grep CyberReconSuite

# Monitor network connections
netstat -tulpn | grep CyberReconSuite
```

### Log Configuration
```bash
# Configure log rotation
sudo tee /etc/logrotate.d/cyberrecon << EOF
/var/log/cyberrecon/*.log {
    daily
    rotate 30
    compress
    delaycompress
    missingok
    notifempty
    create 644 cyberrecon cyberrecon
}
EOF
```

### Health Checks
```bash
#!/bin/bash
# health_check.sh

# Check if process is running
if pgrep -x "CyberReconSuite" > /dev/null; then
    echo "✅ CyberRecon Suite is running"
else
    echo "❌ CyberRecon Suite is not running"
    exit 1
fi

# Check database connectivity
if sqlite3 cyberrecon_main.db "SELECT 1;" > /dev/null 2>&1; then
    echo "✅ Database is accessible"
else
    echo "❌ Database connection failed"
    exit 1
fi

# Check disk space
DISK_USAGE=$(df /opt/cyberrecon | tail -1 | awk '{print $5}' | sed 's/%//')
if [ $DISK_USAGE -gt 90 ]; then
    echo "⚠️  Disk usage high: ${DISK_USAGE}%"
else
    echo "✅ Disk usage normal: ${DISK_USAGE}%"
fi
```

## 🔄 Backup & Recovery

### Database Backup
```bash
#!/bin/bash
# backup_databases.sh

BACKUP_DIR="/opt/cyberrecon/backups"
DATE=$(date +%Y%m%d_%H%M%S)

mkdir -p $BACKUP_DIR

# Backup all databases
for db in *.db; do
    sqlite3 $db ".backup $BACKUP_DIR/${db}_$DATE"
    gzip "$BACKUP_DIR/${db}_$DATE"
done

# Keep only last 30 days of backups
find $BACKUP_DIR -name "*.gz" -mtime +30 -delete
```

### Configuration Backup
```bash
# Backup configuration files
tar -czf /opt/cyberrecon/backups/config_$(date +%Y%m%d).tar.gz \
    /opt/cyberrecon/config/ \
    /opt/cyberrecon/themes/ \
    /opt/cyberrecon/rules/
```

## 🔧 Maintenance

### Regular Maintenance Tasks
```bash
# Weekly maintenance script
#!/bin/bash

# Update threat intelligence feeds
curl -o /opt/cyberrecon/feeds/latest.json https://feeds.example.com/latest

# Vacuum databases
sqlite3 cyberrecon_main.db "VACUUM;"
sqlite3 cyberrecon_auth.db "VACUUM;"

# Clear old logs
find /var/log/cyberrecon/ -name "*.log" -mtime +7 -delete

# Update system packages
sudo apt update && sudo apt upgrade -y
```

### Performance Tuning
```bash
# Optimize SQLite performance
sqlite3 cyberrecon_main.db << EOF
PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA cache_size = 10000;
PRAGMA temp_store = memory;
EOF
```

## 🚨 Troubleshooting

### Common Issues

#### Application Won't Start
```bash
# Check dependencies
ldd /usr/local/bin/CyberReconSuite

# Check permissions
ls -la /usr/local/bin/CyberReconSuite

# Check logs
tail -f /var/log/cyberrecon/application.log
```

#### Database Connection Issues
```bash
# Check database file permissions
ls -la *.db

# Test database connectivity
sqlite3 cyberrecon_main.db "SELECT COUNT(*) FROM events;"

# Repair corrupted database
sqlite3 cyberrecon_main.db ".recover" | sqlite3 cyberrecon_main_recovered.db
```

#### Performance Issues
```bash
# Monitor resource usage
iostat -x 1
vmstat 1
sar -u 1

# Check database performance
sqlite3 cyberrecon_main.db "EXPLAIN QUERY PLAN SELECT * FROM events WHERE severity='Critical';"
```

## 🔐 Security Hardening

### Application Security
```bash
# Run as non-privileged user
sudo -u cyberrecon /usr/local/bin/CyberReconSuite

# Enable AppArmor/SELinux profiles
sudo aa-enforce /etc/apparmor.d/cyberrecon-suite

# Restrict file permissions
sudo chmod 750 /opt/cyberrecon/
sudo chmod 640 /opt/cyberrecon/config/*
```

### Network Security
```bash
# Configure SSL/TLS
openssl req -x509 -newkey rsa:4096 -keyout /opt/cyberrecon/ssl/key.pem -out /opt/cyberrecon/ssl/cert.pem -days 365 -nodes

# Set up reverse proxy (nginx)
sudo tee /etc/nginx/sites-available/cyberrecon << EOF
server {
    listen 443 ssl;
    server_name cyberrecon.yourdomain.com;
    
    ssl_certificate /opt/cyberrecon/ssl/cert.pem;
    ssl_certificate_key /opt/cyberrecon/ssl/key.pem;
    
    location / {
        proxy_pass http://localhost:8080;
        proxy_set_header Host \$host;
        proxy_set_header X-Real-IP \$remote_addr;
    }
}
EOF
```

## 📈 Scaling & High Availability

### Load Balancing
```bash
# HAProxy configuration
sudo tee /etc/haproxy/haproxy.cfg << EOF
backend cyberrecon_backend
    balance roundrobin
    server cr1 192.168.1.10:8080 check
    server cr2 192.168.1.11:8080 check
    server cr3 192.168.1.12:8080 check
EOF
```

### Database Clustering
```bash
# SQLite clustering with Litestream
litestream replicate cyberrecon_main.db s3://your-bucket/db-backups/
```

## 📞 Support

### Enterprise Support
- **Email**: support@bulletdropstudios.com
- **Phone**: +1 (989) 555-0123
- **Portal**: https://support.bulletdropstudios.com

### Community Support
- **GitHub Issues**: Bug reports and feature requests
- **Discussions**: Community Q&A and best practices
- **Wiki**: Community-maintained documentation

---
