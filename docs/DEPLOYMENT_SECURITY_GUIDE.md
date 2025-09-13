# CyberRecon Suite v1.7 - Deployment Security Guide

## 🛡️ Secure Deployment Checklist

### Pre-Deployment Security Assessment

- [ ] **Code Review**: All code changes reviewed by security team
- [ ] **Dependency Scan**: All dependencies scanned for vulnerabilities
- [ ] **Static Analysis**: Code analyzed with security-focused tools
- [ ] **Penetration Testing**: Security testing of all endpoints
- [ ] **Configuration Review**: All configuration files reviewed for hardening

### Environment Hardening

#### Operating System Security

```bash
# Ubuntu/Debian hardening
sudo apt update && sudo apt upgrade -y
sudo apt install fail2ban ufw apparmor-profiles -y

# Enable firewall
sudo ufw enable
sudo ufw default deny incoming
sudo ufw default allow outgoing

# Allow only necessary ports
sudo ufw allow 22/tcp    # SSH (limit this to specific IPs in production)
sudo ufw allow 80/tcp    # HTTP (if needed)
sudo ufw allow 443/tcp   # HTTPS

# Enable automatic security updates
sudo apt install unattended-upgrades -y
sudo dpkg-reconfigure -plow unattended-upgrades
```

#### File System Security

```bash
# Set secure permissions for application directory
sudo chown -R cyberrecon:cyberrecon /opt/cyberrecon-suite
sudo chmod -R 750 /opt/cyberrecon-suite
sudo chmod 600 /opt/cyberrecon-suite/config/*
sudo chmod 600 /opt/cyberrecon-suite/*.db

# Secure log directory
sudo chmod 750 /var/log/cyberrecon-suite
sudo chmod 640 /var/log/cyberrecon-suite/*

# Disable unnecessary services
sudo systemctl disable avahi-daemon
sudo systemctl disable cups
sudo systemctl disable bluetooth
```

### Application Security Configuration

#### Database Security

```bash
# SQLite security settings
sqlite3 /opt/cyberrecon-suite/cyberrecon_main.db "PRAGMA secure_delete = ON;"
sqlite3 /opt/cyberrecon-suite/cyberrecon_main.db "PRAGMA journal_mode = WAL;"

# Set database permissions
chmod 600 /opt/cyberrecon-suite/*.db
chown cyberrecon:cyberrecon /opt/cyberrecon-suite/*.db
```

#### Network Security

```bash
# Configure SSL/TLS certificates
sudo mkdir -p /opt/cyberrecon-suite/ssl
sudo chmod 700 /opt/cyberrecon-suite/ssl

# Generate self-signed certificate for development
sudo openssl req -x509 -nodes -days 365 -newkey rsa:4096 \
  -keyout /opt/cyberrecon-suite/ssl/server.key \
  -out /opt/cyberrecon-suite/ssl/server.crt \
  -subj "/C=US/ST=State/L=City/O=Organization/CN=cyberrecon-suite"

sudo chmod 600 /opt/cyberrecon-suite/ssl/server.key
sudo chmod 644 /opt/cyberrecon-suite/ssl/server.crt
```

#### Application Configuration

Create secure configuration file:

```ini
# /opt/cyberrecon-suite/config/security.conf

[Authentication]
# Password policy
MinPasswordLength=12
RequireUppercase=true
RequireLowercase=true
RequireNumbers=true
RequireSpecialChars=true
PasswordExpiryDays=90

# Account lockout
MaxFailedAttempts=3
LockoutDurationMinutes=15
SessionTimeoutMinutes=30

[Database]
# Connection settings
MaxConnections=10
ConnectionTimeoutSeconds=30
QueryTimeoutSeconds=60

# Security settings
EncryptionEnabled=true
BackupEncryption=true
AuditLogging=true

[Network]
# Security settings
SSLEnabled=true
MinTLSVersion=1.2
SecurityHeaders=true
RateLimitingEnabled=true
MaxRequestsPerMinute=100

[Logging]
# Security logging
SecurityEventsEnabled=true
AuditTrailEnabled=true
LogRotationEnabled=true
MaxLogSizeMB=100
LogRetentionDays=365
```

### Production Deployment

#### Systemd Service Configuration

Create `/etc/systemd/system/cyberrecon-suite.service`:

```ini
[Unit]
Description=CyberRecon Suite Security Platform
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=cyberrecon
Group=cyberrecon
WorkingDirectory=/opt/cyberrecon-suite
ExecStart=/opt/cyberrecon-suite/CyberReconSuite
Restart=always
RestartSec=10

# Security settings
NoNewPrivileges=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/opt/cyberrecon-suite /var/log/cyberrecon-suite
PrivateTmp=true
ProtectKernelTunables=true
ProtectControlGroups=true
RestrictRealtime=true
RestrictNamespaces=true

# Resource limits
MemoryMax=2G
CPUQuota=200%
TasksMax=1000

# Environment
Environment=QT_QPA_PLATFORM=minimal
Environment=CYBERRECON_CONFIG_PATH=/opt/cyberrecon-suite/config

[Install]
WantedBy=multi-user.target
```

#### Log Rotation Configuration

Create `/etc/logrotate.d/cyberrecon-suite`:

```
/var/log/cyberrecon-suite/*.log {
    daily
    missingok
    rotate 365
    compress
    delaycompress
    notifempty
    create 640 cyberrecon cyberrecon
    postrotate
        systemctl reload cyberrecon-suite
    endscript
}
```

### Monitoring and Alerting

#### Security Event Monitoring

```bash
# Install monitoring tools
sudo apt install auditd aide rkhunter chkrootkit -y

# Configure audit rules
cat >> /etc/audit/rules.d/cyberrecon.rules << EOF
# Monitor CyberRecon Suite files
-w /opt/cyberrecon-suite -p wa -k cyberrecon_files
-w /opt/cyberrecon-suite/config -p wa -k cyberrecon_config
-w /var/log/cyberrecon-suite -p wa -k cyberrecon_logs

# Monitor authentication events
-w /var/log/auth.log -p wa -k auth_events
-w /etc/passwd -p wa -k passwd_changes
-w /etc/group -p wa -k group_changes
EOF

sudo systemctl restart auditd
```

#### Intrusion Detection

```bash
# Configure AIDE for file integrity monitoring
sudo aide --init
sudo mv /var/lib/aide/aide.db.new /var/lib/aide/aide.db

# Create daily integrity check
cat > /etc/cron.daily/aide-check << 'EOF'
#!/bin/bash
/usr/bin/aide --check | /usr/bin/logger -t aide
EOF

chmod +x /etc/cron.daily/aide-check
```

### Network Security

#### Reverse Proxy Configuration (Nginx)

```nginx
# /etc/nginx/sites-available/cyberrecon-suite
server {
    listen 443 ssl http2;
    server_name cyberrecon-suite.local;

    # SSL Configuration
    ssl_certificate /opt/cyberrecon-suite/ssl/server.crt;
    ssl_certificate_key /opt/cyberrecon-suite/ssl/server.key;
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers ECDHE-RSA-AES256-GCM-SHA512:DHE-RSA-AES256-GCM-SHA512:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384;
    ssl_prefer_server_ciphers off;
    ssl_session_cache shared:SSL:10m;
    ssl_session_timeout 10m;

    # Security Headers
    add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;
    add_header X-Content-Type-Options nosniff always;
    add_header X-Frame-Options DENY always;
    add_header X-XSS-Protection "1; mode=block" always;
    add_header Referrer-Policy "strict-origin-when-cross-origin" always;
    add_header Content-Security-Policy "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; img-src 'self' data:; connect-src 'self'; font-src 'self'; object-src 'none'; media-src 'self'; frame-src 'none';" always;

    # Rate Limiting
    limit_req_zone $binary_remote_addr zone=login:10m rate=5r/m;
    limit_req_zone $binary_remote_addr zone=api:10m rate=100r/m;

    location / {
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # Security headers
        proxy_set_header X-Frame-Options DENY;
        proxy_set_header X-Content-Type-Options nosniff;
    }

    location /api/auth/login {
        limit_req zone=login burst=3 nodelay;
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }

    location /api/ {
        limit_req zone=api burst=20 nodelay;
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}

# Redirect HTTP to HTTPS
server {
    listen 80;
    server_name cyberrecon-suite.local;
    return 301 https://$server_name$request_uri;
}
```

#### Firewall Configuration (UFW)

```bash
# Advanced UFW rules
sudo ufw --force reset

# Default policies
sudo ufw default deny incoming
sudo ufw default allow outgoing

# SSH access (restrict to specific IPs in production)
sudo ufw allow from 192.168.1.0/24 to any port 22

# Web traffic
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp

# Application specific ports (adjust as needed)
sudo ufw allow from 192.168.1.0/24 to any port 8080

# Logging
sudo ufw logging on

# Enable firewall
sudo ufw --force enable
```

### Backup and Recovery

#### Secure Backup Strategy

```bash
# Create backup script
cat > /opt/cyberrecon-suite/scripts/backup.sh << 'EOF'
#!/bin/bash

BACKUP_DIR="/opt/backups/cyberrecon-suite"
DATE=$(date +%Y%m%d_%H%M%S)
BACKUP_FILE="cyberrecon_backup_${DATE}.tar.gz.gpg"

# Create backup directory
mkdir -p "$BACKUP_DIR"

# Create encrypted backup
tar -czf - /opt/cyberrecon-suite/config /opt/cyberrecon-suite/*.db \
    /var/log/cyberrecon-suite | \
    gpg --symmetric --cipher-algo AES256 --compress-algo 1 \
    --output "$BACKUP_DIR/$BACKUP_FILE"

# Set permissions
chmod 600 "$BACKUP_DIR/$BACKUP_FILE"

# Clean old backups (keep 30 days)
find "$BACKUP_DIR" -name "cyberrecon_backup_*.tar.gz.gpg" -mtime +30 -delete

echo "Backup completed: $BACKUP_FILE"
EOF

chmod +x /opt/cyberrecon-suite/scripts/backup.sh

# Schedule daily backups
echo "0 2 * * * /opt/cyberrecon-suite/scripts/backup.sh" | sudo crontab -u cyberrecon -
```

### Security Monitoring

#### Real-time Monitoring Script

```bash
cat > /opt/cyberrecon-suite/scripts/security-monitor.sh << 'EOF'
#!/bin/bash

LOG_FILE="/var/log/cyberrecon-suite/security.log"
ALERT_EMAIL="security@yourcompany.com"

# Monitor failed login attempts
tail -f /var/log/auth.log | while read line; do
    if echo "$line" | grep -q "Failed password"; then
        echo "$(date): Failed login attempt detected: $line" >> "$LOG_FILE"
        # Alert on multiple failures
        COUNT=$(grep "$(date +%Y-%m-%d)" "$LOG_FILE" | grep "Failed login" | wc -l)
        if [ "$COUNT" -gt 5 ]; then
            echo "Multiple failed login attempts detected" | mail -s "Security Alert" "$ALERT_EMAIL"
        fi
    fi
done &

# Monitor file changes
inotifywait -m -r -e modify,create,delete /opt/cyberrecon-suite/config | while read path action file; do
    echo "$(date): Configuration file changed: $path$file ($action)" >> "$LOG_FILE"
    # Alert on config changes
    echo "Configuration file modified: $path$file" | mail -s "Config Change Alert" "$ALERT_EMAIL"
done &
EOF

chmod +x /opt/cyberrecon-suite/scripts/security-monitor.sh
```

### Compliance and Auditing

#### SOC 2 Compliance Setup

```bash
# Create audit log directory with proper permissions
sudo mkdir -p /var/log/cyberrecon-suite/audit
sudo chown cyberrecon:cyberrecon /var/log/cyberrecon-suite/audit
sudo chmod 750 /var/log/cyberrecon-suite/audit

# Configure audit logging in application
cat >> /opt/cyberrecon-suite/config/security.conf << EOF

[Audit]
# SOC 2 compliance settings
AuditLoggingEnabled=true
AuditLogDirectory=/var/log/cyberrecon-suite/audit
DataAccessLogging=true
SystemEventLogging=true
SecurityEventLogging=true
ComplianceReporting=true

# Log retention for compliance
AuditLogRetentionDays=2555  # 7 years
EncryptAuditLogs=true
TamperProtection=true
EOF
```

### Incident Response

#### Security Incident Response Plan

1. **Detection**: Monitor logs and alerts for security events
2. **Containment**: Isolate affected systems
3. **Investigation**: Analyze logs and forensic data
4. **Recovery**: Restore systems and patch vulnerabilities
5. **Lessons Learned**: Document and improve security measures

#### Emergency Response Script

```bash
cat > /opt/cyberrecon-suite/scripts/emergency-response.sh << 'EOF'
#!/bin/bash

echo "EMERGENCY: Security incident detected"
echo "Timestamp: $(date)"

# Stop application
sudo systemctl stop cyberrecon-suite

# Create forensic backup
INCIDENT_DIR="/var/log/cyberrecon-suite/incidents/$(date +%Y%m%d_%H%M%S)"
mkdir -p "$INCIDENT_DIR"
cp -r /opt/cyberrecon-suite/config "$INCIDENT_DIR/"
cp -r /var/log/cyberrecon-suite "$INCIDENT_DIR/"

# Block suspicious IPs (example)
# sudo ufw deny from suspicious.ip.address

# Alert security team
echo "Security incident detected. Incident directory: $INCIDENT_DIR" | \
    mail -s "URGENT: Security Incident" security@yourcompany.com

echo "Emergency response completed. System secured."
EOF

chmod +x /opt/cyberrecon-suite/scripts/emergency-response.sh
```

## 📋 Security Verification Checklist

### Pre-Production Checklist

- [ ] All default passwords changed
- [ ] SSL/TLS certificates properly configured
- [ ] Firewall rules tested and documented
- [ ] Backup and recovery procedures tested
- [ ] Monitoring and alerting configured
- [ ] Security patches applied
- [ ] Access controls verified
- [ ] Audit logging enabled and tested
- [ ] Incident response plan documented
- [ ] Security training completed for operations team

### Post-Deployment Verification

```bash
# Run security verification script
/opt/cyberrecon-suite/scripts/security-verification.sh
```

This comprehensive security deployment guide ensures that CyberRecon Suite is deployed with enterprise-grade security controls and monitoring capabilities.