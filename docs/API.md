# CyberRecon Suite - API Documentation

## 🔌 API Reference Guide

CyberRecon Suite provides RESTful APIs for integration with external security tools and automation platforms.

## 🔐 Authentication

### API Key Authentication
```http
GET /api/v1/events
Authorization: Bearer your-api-key-here
Content-Type: application/json
```

### Session-Based Authentication
```http
POST /api/v1/auth/login
Content-Type: application/json

{
  "username": "analyst",
  "password": "secure_password"
}
```

## 📊 Events API

### Get Security Events
```http
GET /api/v1/events
GET /api/v1/events?panel=SIEM&severity=Critical
GET /api/v1/events?limit=100&offset=0
```

**Response:**
```json
{
  "events": [
    {
      "id": 1,
      "timestamp": "2024-01-15T10:30:00Z",
      "panel": "SIEM",
      "severity": "Critical",
      "message": "Suspicious login attempt detected"
    }
  ],
  "total": 1,
  "page": 1
}
```

### Create Security Event
```http
POST /api/v1/events
Content-Type: application/json

{
  "panel": "CustomTool",
  "severity": "Warning",
  "message": "Custom security event detected"
}
```

## 🎯 Threat Intelligence API

### IOC Lookup
```http
GET /api/v1/intel/ioc/{ioc}
GET /api/v1/intel/ioc/192.168.1.100
```

**Response:**
```json
{
  "ioc": "192.168.1.100",
  "type": "ip",
  "reputation": "suspicious",
  "sources": ["VirusTotal", "AlienVault"],
  "last_seen": "2024-01-15T10:30:00Z",
  "confidence": 85
}
```

### Submit IOC
```http
POST /api/v1/intel/ioc
Content-Type: application/json

{
  "ioc": "malicious.example.com",
  "type": "domain",
  "severity": "high",
  "source": "internal_analysis"
}
```

## 🔄 SOAR Automation API

### Execute Workflow
```http
POST /api/v1/soar/workflows/{workflow_id}/execute
Content-Type: application/json

{
  "parameters": {
    "target_ip": "192.168.1.100",
    "action": "isolate"
  }
}
```

### Get Workflow Status
```http
GET /api/v1/soar/workflows/{workflow_id}/status
```

**Response:**
```json
{
  "workflow_id": "malware_response_001",
  "status": "executing",
  "current_step": 3,
  "total_steps": 7,
  "progress": 42.8,
  "started_at": "2024-01-15T10:30:00Z"
}
```

## 💼 Case Management API

### Get Cases
```http
GET /api/v1/cases
GET /api/v1/cases?status=open&assigned_to=analyst1
```

### Create Case
```http
POST /api/v1/cases
Content-Type: application/json

{
  "name": "Malware Investigation #2024-001",
  "description": "Suspicious executable detected on endpoint",
  "severity": "high",
  "assigned_to": "analyst1"
}
```

### Update Case
```http
PUT /api/v1/cases/{case_id}
Content-Type: application/json

{
  "status": "investigating",
  "notes": "Memory dump collected, analyzing artifacts"
}
```

## 📡 Network Monitoring API

### Get Network Scan Results
```http
GET /api/v1/network/scans
GET /api/v1/network/scans/{scan_id}
```

### Start Network Scan
```http
POST /api/v1/network/scans
Content-Type: application/json

{
  "target": "192.168.1.0/24",
  "scan_type": "tcp_syn",
  "ports": "1-1000",
  "options": {
    "os_detection": true,
    "service_detection": true
  }
}
```

## 📶 Wireless Testing API

### Get WiFi Scan Results
```http
GET /api/v1/wireless/wifi/scans
```

### Start WiFi Scan
```http
POST /api/v1/wireless/wifi/scans
Content-Type: application/json

{
  "interface": "wlan0",
  "channel": "all",
  "duration": 300
}
```

### Execute WiFi Attack (Authorized Testing Only)
```http
POST /api/v1/wireless/wifi/attacks
Content-Type: application/json

{
  "attack_type": "deauth",
  "target_bssid": "aa:bb:cc:dd:ee:ff",
  "authorization_token": "authorized_pentest_token"
}
```

## 👥 User Management API

### Get Users
```http
GET /api/v1/users
```

### Create User
```http
POST /api/v1/users
Content-Type: application/json

{
  "username": "new_analyst",
  "email": "analyst@company.com",
  "role": "analyst",
  "subscription_type": "professional"
}
```

## 📈 Analytics API

### Get Dashboard Metrics
```http
GET /api/v1/analytics/dashboard
GET /api/v1/analytics/compliance
GET /api/v1/analytics/threats
```

**Response:**
```json
{
  "metrics": {
    "total_events": 15420,
    "critical_events": 23,
    "events_last_24h": 1205,
    "active_cases": 8,
    "compliance_score": 87.5
  },
  "timestamp": "2024-01-15T10:30:00Z"
}
```

## 🔧 Configuration API

### Get System Configuration
```http
GET /api/v1/config
```

### Update Configuration
```http
PUT /api/v1/config
Content-Type: application/json

{
  "refresh_interval": 10,
  "max_events": 100000,
  "enable_notifications": true,
  "theme": "dark_slate_blue"
}
```

## 📋 Webhooks

### Event Webhooks
Configure webhooks to receive real-time security events:

```http
POST /api/v1/webhooks
Content-Type: application/json

{
  "url": "https://your-system.com/webhook",
  "events": ["critical_event", "case_created", "workflow_completed"],
  "secret": "webhook_secret_key"
}
```

**Webhook Payload:**
```json
{
  "event_type": "critical_event",
  "timestamp": "2024-01-15T10:30:00Z",
  "data": {
    "event_id": 12345,
    "panel": "SIEM",
    "severity": "Critical",
    "message": "Ransomware detected on endpoint"
  },
  "signature": "sha256_hmac_signature"
}
```

## 🛡️ Security Considerations

### Rate Limiting
- **Standard Users**: 100 requests/minute
- **Professional**: 500 requests/minute  
- **Enterprise**: 2000 requests/minute

### Input Validation
- All inputs are sanitized and validated
- SQL injection protection enabled
- XSS prevention implemented
- CSRF tokens required for state-changing operations

### Audit Logging
All API calls are logged with:
- User identification
- Timestamp and duration
- Request/response details
- IP address and user agent

## 📚 SDK & Libraries

### Python SDK
```python
from cyberrecon import CyberReconClient

client = CyberReconClient(
    base_url="https://your-instance.com",
    api_key="your-api-key"
)

# Get recent events
events = client.events.list(severity="Critical", limit=10)

# Create case
case = client.cases.create(
    name="Security Incident #2024-001",
    severity="high"
)

# Execute SOAR workflow
workflow = client.soar.execute_workflow(
    "malware_response",
    parameters={"target_ip": "192.168.1.100"}
)
```

### JavaScript SDK
```javascript
import { CyberReconAPI } from 'cyberrecon-js-sdk';

const api = new CyberReconAPI({
  baseURL: 'https://your-instance.com',
  apiKey: 'your-api-key'
});

// Get threat intelligence
const intel = await api.intel.lookup('192.168.1.100');

// Create security event
const event = await api.events.create({
  panel: 'CustomTool',
  severity: 'Warning',
  message: 'Suspicious activity detected'
});
```

## 🔗 Integration Examples

### SIEM Integration
```bash
# Send events to CyberRecon from Splunk
curl -X POST https://cyberrecon.company.com/api/v1/events \
  -H "Authorization: Bearer $API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "panel": "Splunk",
    "severity": "Critical", 
    "message": "Malware detected on endpoint-001"
  }'
```

### SOAR Integration
```python
# Trigger automated response from external system
import requests

response = requests.post(
    'https://cyberrecon.company.com/api/v1/soar/workflows/incident_response/execute',
    headers={'Authorization': 'Bearer your-api-key'},
    json={
        'parameters': {
            'incident_type': 'malware',
            'affected_hosts': ['192.168.1.100', '192.168.1.101'],
            'severity': 'critical'
        }
    }
)
```

## 📞 Support

### API Support
- **Documentation**: https://docs.cyberrecon.com/api
- **Support Email**: api-support@cyberrecon.com
- **Status Page**: https://status.cyberrecon.com

### Rate Limit Headers
```http
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1642694400
```

---

*For complete API documentation and interactive testing, visit our API portal at https://api.cyberrecon.com*