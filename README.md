# 🛡️ CyberRecon Suite v2.0

<div align="center">

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg?style=for-the-badge)](https://github.com/mllinman/cyberrecon-suite)
[![License](https://img.shields.io/badge/license-MIT-green.svg?style=for-the-badge)](LICENSE)
[![Node](https://img.shields.io/badge/Node-18+-brightgreen.svg?style=for-the-badge&logo=node.js)]()
[![TypeScript](https://img.shields.io/badge/TypeScript-5.6-blue.svg?style=for-the-badge&logo=typescript)]()

**🚀 Enterprise-Grade Cybersecurity Operations Platform**

**Real-time SIEM • Threat Intelligence • Network Monitoring • EDR • Compliance • Vulnerability Scanning**

*The most advanced, fully functional cybersecurity platform - now cloud-native*

[![Deploy on Railway](https://railway.app/button.svg)](https://railway.app/template/cyberrecon)

[⚡ **Quick Start**](#-quick-start) • [📖 **Features**](#-features) • [📚 **Documentation**](DEPLOYMENT.md) • [🔐 **Security**](#-security)

</div>

---

## 🎯 Overview

**CyberRecon Suite 2.0** is a completely reengineered, cloud-native cybersecurity operations platform designed to dominate the market with advanced, fully functional features. Built from the ground up as a modern web application, it delivers enterprise-grade security capabilities accessible from anywhere.

### 🌟 **What's New in v2.0**

- ✅ **Cloud-Native Architecture** - Deploy instantly to Railway
- ✅ **Real-Time Operations** - WebSocket-powered live monitoring
- ✅ **Modern Tech Stack** - React + TypeScript + Node.js + PostgreSQL
- ✅ **Production Ready** - Containerized with Docker, scalable infrastructure
- ✅ **Advanced Security** - Helmet, rate limiting, JWT authentication
- ✅ **RESTful APIs** - Complete API for integrations
- ✅ **Best-in-Class Features** - Industry-leading security tools

---

## 🚀 Features

### 🛡️ **Security Information & Event Management (SIEM)**
- **Real-time Event Processing** - Process thousands of security events per second
- **Advanced Correlation** - Intelligent event correlation and pattern detection
- **Custom Dashboards** - Configurable security operations center dashboards
- **Alerting System** - Multi-channel alerting with severity-based routing
- **Event Timeline** - Complete event reconstruction for forensic analysis
- **Statistical Analysis** - Hourly, daily, and custom trend analysis

### 🎯 **Threat Intelligence Hub**
- **Multi-Source Intelligence** - Aggregate threat data from multiple feeds
- **IOC Management** - Track indicators of compromise with tagging
- **Threat Search** - Rapid IOC lookup and enrichment
- **Live Threat Feed** - Real-time threat intelligence updates
- **Threat Analytics** - Statistical analysis of threat landscape
- **Custom Indicators** - User-defined threat indicators

### 📡 **Network Security**
- **Network Scanning** - Port scanning and service detection
- **Vulnerability Assessment** - Automated vulnerability discovery
- **Traffic Monitoring** - Real-time network traffic analysis
- **Bandwidth Monitoring** - Upload/download metrics
- **Connection Tracking** - Active connection monitoring
- **Packet Analysis** - Deep packet inspection capabilities

### 🖥️ **Endpoint Detection & Response (EDR)**
- **Endpoint Monitoring** - Real-time endpoint health tracking
- **Threat Detection** - Behavioral analysis and threat detection
- **Incident Response** - Automated endpoint isolation
- **Process Monitoring** - Track running processes and resource usage
- **Compliance Tracking** - Endpoint compliance monitoring
- **Agent Management** - Centralized agent deployment and updates

### 📋 **Compliance Management**
- **SOC 2 Compliance** - Complete SOC 2 Type II framework
- **ISO 27001** - Information security management
- **NIST CSF** - Cybersecurity Framework implementation
- **GDPR** - Data privacy compliance
- **HIPAA** - Healthcare information security
- **Custom Frameworks** - Build custom compliance frameworks

### 🔍 **Vulnerability Management**
- **Web App Scanning** - OWASP Top 10 vulnerability detection
- **Infrastructure Scanning** - Server and network vulnerability assessment
- **Malware Analysis** - File hash checking and malware detection
- **CVE Database** - Integration with CVE/NVD databases
- **Remediation Tracking** - Track vulnerability remediation efforts
- **Risk Scoring** - CVSS-based risk assessment

### 📊 **Advanced Analytics**
- **Real-Time Dashboards** - Live data visualization
- **Historical Analysis** - Trend analysis over time
- **Reporting Engine** - Automated report generation
- **Custom Metrics** - Define custom security metrics
- **Export Capabilities** - Export data in multiple formats
- **API Access** - Complete API for custom integrations

---

## ⚡ Quick Start

### **Option 1: Deploy to Railway (Recommended)**

1. **Click Deploy Button**
   ```bash
   # Or use Railway CLI
   railway login
   railway init
   railway up
   ```

2. **Add PostgreSQL Database**
   - In Railway dashboard: Add PostgreSQL service
   - Database will auto-configure via `DATABASE_URL`

3. **Set Environment Variables**
   ```bash
   JWT_SECRET=your-secure-secret-key
   NODE_ENV=production
   ```

4. **Access Your Application**
   - Railway will provide a URL: `https://your-app.railway.app`

### **Option 2: Local Development**

```bash
# 1. Clone repository
git clone https://github.com/mllinman/cyberrecon-suite.git
cd cyberrecon-suite

# 2. Install dependencies
npm install

# 3. Set up environment variables
cp .env.example .env
# Edit .env with your configuration

# 4. Start PostgreSQL (Docker)
docker run -d \
  --name cyberrecon-db \
  -e POSTGRES_PASSWORD=postgres \
  -e POSTGRES_DB=cyberrecon \
  -p 5432:5432 \
  postgres:15-alpine

# 5. Update .env with database URL
# DATABASE_URL=postgresql://postgres:postgres@localhost:5432/cyberrecon

# 6. Start development servers
npm run dev
# Server: http://localhost:3001
# Client: http://localhost:3000
```

### **Option 3: Docker Deployment**

```bash
# Build and run with Docker
docker build -t cyberrecon-suite .
docker run -p 3001:3001 \
  -e DATABASE_URL=your-postgres-url \
  -e JWT_SECRET=your-secret \
  cyberrecon-suite
```

---

## 🏗️ Architecture

### **Technology Stack**

**Frontend:**
- React 18.3 with TypeScript
- Socket.IO Client for real-time updates
- Recharts for data visualization
- TailwindCSS for styling
- Framer Motion for animations
- React Router for navigation

**Backend:**
- Node.js 18+ with Express
- TypeScript for type safety
- Socket.IO for WebSocket communication
- PostgreSQL for data persistence
- JWT for authentication
- Bcrypt for password hashing
- Helmet for security headers
- Rate limiting for API protection

**Infrastructure:**
- Railway for hosting
- Docker for containerization
- PostgreSQL database
- Health check monitoring

### **System Architecture**

```
┌─────────────────────────────────────────────────────────┐
│                     Client (React)                       │
│  ┌──────────┬──────────┬──────────┬──────────┐          │
│  │  SIEM    │  Threat  │ Network  │   EDR    │          │
│  │Dashboard │  Intel   │ Monitor  │ Dashboard│          │
│  └──────────┴──────────┴──────────┴──────────┘          │
└─────────────────┬───────────────────────────────────────┘
                  │ WebSocket + REST API
┌─────────────────┴───────────────────────────────────────┐
│              Express API Server (Node.js)                │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Routes: Auth, SIEM, Threat Intel, Network, EDR  │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Services: Monitoring, Database, Authentication  │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  WebSocket: Real-time Event Broadcasting         │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────┬───────────────────────────────────────┘
                  │
┌─────────────────┴───────────────────────────────────────┐
│              PostgreSQL Database                         │
│  • Users & Authentication                                │
│  • Security Events                                       │
│  • Threat Indicators                                     │
│  • Network Scans                                         │
│  • Compliance Reports                                    │
│  • Audit Logs                                           │
└─────────────────────────────────────────────────────────┘
```

---

## 📡 API Documentation

### **Authentication**

```typescript
// Register
POST /api/auth/register
Body: { email, password, fullName }

// Login
POST /api/auth/login
Body: { email, password }

// Get current user
GET /api/auth/me
Headers: { Authorization: "Bearer <token>" }
```

### **SIEM**

```typescript
// Get security events
GET /api/siem/events?severity=high&limit=100

// Create security event
POST /api/siem/events
Body: { eventType, severity, source, destination, description }

// Get dashboard statistics
GET /api/siem/dashboard
```

### **Threat Intelligence**

```typescript
// Get threat indicators
GET /api/threat-intel/indicators?type=ip&limit=50

// Add threat indicator
POST /api/threat-intel/indicators
Body: { iocType, iocValue, threatLevel, source, description }

// Search IOCs
POST /api/threat-intel/search
Body: { value, type }
```

### **Network Monitoring**

```typescript
// Start network scan
POST /api/network/scan
Body: { userId, scanType, target }

// Get scan status
GET /api/network/scan/:id

// Get real-time metrics
GET /api/network/metrics
```

### **EDR**

```typescript
// Get endpoints
GET /api/edr/endpoints

// Get endpoint details
GET /api/edr/endpoints/:id

// Isolate endpoint
POST /api/edr/endpoints/:id/isolate
```

---

## 🔐 Security

### **Built-in Security Features**

- **🔒 JWT Authentication** - Secure token-based authentication
- **🛡️ Helmet.js** - Security headers (CSP, XSS protection)
- **⚡ Rate Limiting** - DDoS protection and abuse prevention
- **🔐 Bcrypt** - Industry-standard password hashing
- **🚫 CORS** - Configured cross-origin resource sharing
- **📝 Audit Logging** - Complete activity tracking
- **🔍 Input Validation** - Zod schema validation
- **💾 SQL Injection Protection** - Parameterized queries

### **Deployment Security**

```env
# Always set these in production:
NODE_ENV=production
JWT_SECRET=<strong-random-secret>
DATABASE_URL=<secure-connection-string>
```

### **Best Practices**

1. ✅ Use environment variables for secrets
2. ✅ Enable HTTPS in production (Railway auto-provides)
3. ✅ Regularly update dependencies
4. ✅ Monitor audit logs
5. ✅ Implement rate limiting
6. ✅ Use strong JWT secrets
7. ✅ Enable database SSL in production

---

## 📊 Environment Variables

```env
# Server
NODE_ENV=production
PORT=3001

# Database
DATABASE_URL=postgresql://user:pass@host:port/db

# Authentication
JWT_SECRET=your-super-secret-jwt-key

# CORS
CLIENT_URL=https://your-app.railway.app

# Rate Limiting
RATE_LIMIT_WINDOW_MS=900000
RATE_LIMIT_MAX_REQUESTS=100

# Optional: Stripe
STRIPE_SECRET_KEY=sk_xxx
STRIPE_PUBLISHABLE_KEY=pk_xxx
```

---

## 🚀 Deployment

### **Railway Deployment Steps**

1. **Fork this repository**

2. **Connect to Railway**
   - Visit [railway.app](https://railway.app)
   - Connect your GitHub account
   - Select this repository

3. **Add PostgreSQL**
   - Click "Add Database"
   - Select PostgreSQL
   - Railway auto-configures `DATABASE_URL`

4. **Configure Environment Variables**
   ```
   JWT_SECRET=generate-a-secure-secret
   NODE_ENV=production
   ```

5. **Deploy**
   - Railway auto-detects configuration
   - Builds and deploys automatically
   - Provides public URL

6. **Access Application**
   - Visit your Railway URL
   - Create an account
   - Start using CyberRecon Suite!

### **Production Checklist**

- [ ] Set strong `JWT_SECRET`
- [ ] Configure `DATABASE_URL`
- [ ] Enable database SSL
- [ ] Set up monitoring
- [ ] Configure backup strategy
- [ ] Review security headers
- [ ] Test health endpoint
- [ ] Enable auto-restart
- [ ] Set up alerts

---

## 💻 Development

### **Project Structure**

```
cyberrecon-suite/
├── server/                 # Backend source
│   ├── routes/            # API routes
│   ├── services/          # Business logic
│   ├── websocket/         # WebSocket handlers
│   └── index.ts           # Server entry
├── src/                   # Frontend source
│   ├── components/        # React components
│   ├── pages/             # Page components
│   ├── contexts/          # React contexts
│   └── App.tsx            # Main app
├── public/                # Static assets
├── Dockerfile             # Container config
├── railway.json           # Railway config
└── package.json           # Dependencies
```

### **Development Commands**

```bash
# Install dependencies
npm install

# Run development (client + server)
npm run dev

# Build for production
npm run build

# Start production server
npm start

# Type check
npm run type-check
```

---

## 🤝 Contributing

We welcome contributions! CyberRecon Suite 2.0 is built to be the best in the world, and your expertise can help make it even better.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🌟 Acknowledgments

Built with cutting-edge technologies:
- [React](https://react.dev) - UI framework
- [Node.js](https://nodejs.org) - JavaScript runtime
- [Express](https://expressjs.com) - Web framework
- [PostgreSQL](https://postgresql.org) - Database
- [Socket.IO](https://socket.io) - Real-time engine
- [Railway](https://railway.app) - Cloud platform

---

<div align="center">

**🛡️ Built with ❤️ by security professionals, for security professionals**

**© 2024 BulletDrop Studios. All rights reserved.**

*"The most advanced, fully functional cybersecurity platform in the world."*

**⭐ Star this repo if CyberRecon Suite helps secure your operations!**

[🚀 Deploy Now](https://railway.app) • [📖 Documentation](#) • [💬 Support](#)

</div>
