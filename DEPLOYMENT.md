# 🚀 CyberRecon Suite - Railway Deployment Guide

This guide will help you deploy CyberRecon Suite to Railway in under 10 minutes.

## Prerequisites

- GitHub account
- Railway account (sign up at [railway.app](https://railway.app))
- Git installed locally (optional, for local development)

## 🎯 Quick Deploy (Recommended)

### Step 1: Fork the Repository

1. Go to [github.com/mllinman/cyberrecon-suite](https://github.com/mllinman/cyberrecon-suite)
2. Click the "Fork" button in the top right
3. Wait for the fork to complete

### Step 2: Connect to Railway

1. Visit [railway.app](https://railway.app) and sign in
2. Click "New Project"
3. Select "Deploy from GitHub repo"
4. Choose your forked `cyberrecon-suite` repository
5. Railway will automatically detect the configuration

### Step 3: Add PostgreSQL Database

1. In your Railway project, click "New"
2. Select "Database" → "PostgreSQL"
3. Railway will automatically provision the database
4. The `DATABASE_URL` environment variable is automatically configured

### Step 4: Configure Environment Variables

Click on your service, go to "Variables" tab, and add:

```env
NODE_ENV=production
JWT_SECRET=<generate-a-strong-random-secret-here>
```

**To generate a secure JWT secret:**
```bash
# On Linux/Mac:
openssl rand -base64 32

# On Windows (PowerShell):
[Convert]::ToBase64String((1..32 | ForEach-Object { Get-Random -Minimum 0 -Maximum 256 }))

# Or use an online generator (for non-production):
# https://www.grc.com/passwords.htm
```

### Step 5: Deploy

1. Railway will automatically build and deploy your application
2. Wait for the build to complete (usually 2-5 minutes)
3. Once deployed, Railway will provide a public URL

### Step 6: Access Your Application

1. Click on the public URL provided by Railway
2. You'll see the login page for CyberRecon Suite
3. Click "Register" to create your first account
4. Start using your cybersecurity platform!

## 🔧 Local Development Setup

Want to test locally before deploying?

### 1. Clone Your Fork

```bash
git clone https://github.com/YOUR-USERNAME/cyberrecon-suite.git
cd cyberrecon-suite
```

### 2. Install Dependencies

```bash
npm install
```

### 3. Start PostgreSQL Database

**Option A: Docker (Recommended)**
```bash
docker run -d \
  --name cyberrecon-db \
  -e POSTGRES_PASSWORD=postgres \
  -e POSTGRES_DB=cyberrecon \
  -p 5432:5432 \
  postgres:15-alpine
```

**Option B: Local PostgreSQL**
- Install PostgreSQL on your system
- Create a database named `cyberrecon`

### 4. Configure Environment

```bash
# Copy example environment file
cp .env.example .env

# Edit .env with your settings
nano .env  # or use your preferred editor
```

Update `.env`:
```env
NODE_ENV=development
PORT=3001
DATABASE_URL=postgresql://postgres:postgres@localhost:5432/cyberrecon
JWT_SECRET=your-development-secret-key
CLIENT_URL=http://localhost:3000
```

### 5. Start Development Servers

```bash
# Starts both backend (port 3001) and frontend (port 3000)
npm run dev
```

Access the application:
- Frontend: http://localhost:3000
- Backend API: http://localhost:3001
- Health Check: http://localhost:3001/health

## 📊 Verifying Deployment

### Check Application Health

Visit: `https://your-app.railway.app/health`

You should see:
```json
{
  "status": "healthy",
  "timestamp": "2024-01-13T...",
  "service": "CyberRecon Suite API",
  "version": "2.0.0"
}
```

### Test API Endpoints

```bash
# Register a user
curl -X POST https://your-app.railway.app/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "email": "admin@example.com",
    "password": "SecurePassword123!",
    "fullName": "Admin User"
  }'

# Login
curl -X POST https://your-app.railway.app/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "email": "admin@example.com",
    "password": "SecurePassword123!"
  }'
```

## 🔐 Security Checklist

Before going to production, ensure:

- [ ] Strong `JWT_SECRET` is set (minimum 32 characters)
- [ ] `NODE_ENV=production` is configured
- [ ] Database has SSL enabled (Railway does this automatically)
- [ ] HTTPS is enabled (Railway provides this automatically)
- [ ] Regular backups are configured
- [ ] Monitoring is set up
- [ ] Update password policy for users
- [ ] Review and adjust rate limits if needed

## 🔄 Updating Your Deployment

### Automatic Deployments

Railway automatically deploys when you push to your main branch:

```bash
git add .
git commit -m "Your update message"
git push origin main
```

Railway will:
1. Detect the push
2. Build the new version
3. Deploy automatically
4. Keep zero downtime

### Manual Deployment

In Railway dashboard:
1. Go to your project
2. Click "Deployments"
3. Click "Deploy" on any previous commit

## 📈 Monitoring & Logs

### View Logs in Railway

1. Go to your project in Railway
2. Click on your service
3. Click "Logs" tab
4. View real-time logs

### Key Metrics to Monitor

- Response times
- Error rates
- Database connection pool
- Memory usage
- Active WebSocket connections

## 🔧 Troubleshooting

### Build Fails

**Check:**
1. All dependencies are in `package.json`
2. TypeScript compiles without errors: `npm run build:server`
3. Vite builds successfully: `npm run build:client`

**Fix:**
```bash
# Locally test the build
npm run build

# Check for errors
npm run type-check
```

### Database Connection Issues

**Check:**
1. DATABASE_URL is set correctly
2. PostgreSQL service is running
3. Database exists

**Fix:**
```bash
# In Railway, check the PostgreSQL service variables
# Ensure DATABASE_URL is linked to your main service
```

### Application Won't Start

**Check:**
1. PORT environment variable (Railway sets this automatically)
2. Health check endpoint returns 200
3. Logs for specific errors

**Fix:**
```bash
# Check logs in Railway dashboard
# Look for startup errors
# Verify all required environment variables are set
```

### WebSocket Connection Fails

**Check:**
1. CORS configuration allows your domain
2. CLIENT_URL is set to your Railway app URL
3. Socket.IO is properly configured

**Fix:**
Update environment variables:
```env
CLIENT_URL=https://your-app.railway.app
```

## 🎨 Customization

### Branding

Update these files:
- `/public/shield-icon.svg` - Application icon
- `/src/App.tsx` - Application title and branding

### Features

Enable/disable features in:
- `/server/routes/` - Backend API routes
- `/src/components/` - Frontend components

### Database Schema

Modify schema in:
- `/server/services/database.ts`

After changes, redeploy to apply updates.

## 📚 Additional Resources

- [Railway Documentation](https://docs.railway.app)
- [CyberRecon Suite GitHub](https://github.com/mllinman/cyberrecon-suite)
- [PostgreSQL Documentation](https://www.postgresql.org/docs/)
- [Express.js Guide](https://expressjs.com/en/guide/routing.html)
- [React Documentation](https://react.dev)

## 🆘 Getting Help

- **GitHub Issues**: [Create an issue](https://github.com/mllinman/cyberrecon-suite/issues)
- **Discussions**: [Join discussions](https://github.com/mllinman/cyberrecon-suite/discussions)
- **Railway Support**: [Railway Discord](https://discord.gg/railway)

## 🎉 Success!

Your CyberRecon Suite is now running on Railway! Start securing your operations with:

- **SIEM Dashboard** - Real-time security event monitoring
- **Threat Intelligence** - IOC tracking and threat analysis
- **Network Monitoring** - Network traffic and vulnerability scanning
- **EDR** - Endpoint detection and response
- **Compliance** - SOC 2, ISO 27001, NIST CSF monitoring

---

**Built with ❤️ for security professionals**

Need enterprise support or custom features? Contact us at support@bulletdropstudios.com
