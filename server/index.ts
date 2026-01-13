import express, { Express } from 'express';
import helmet from 'helmet';
import cors from 'cors';
import { createServer } from 'http';
import { Server as SocketIOServer } from 'socket.io';
import dotenv from 'dotenv';
import path from 'path';
import { fileURLToPath } from 'url';
import rateLimit from 'express-rate-limit';

// Import routes
import authRoutes from './routes/auth.js';
import threatIntelRoutes from './routes/threatIntel.js';
import networkRoutes from './routes/network.js';
import siemRoutes from './routes/siem.js';
import edrRoutes from './routes/edr.js';
import complianceRoutes from './routes/compliance.js';
import scanRoutes from './routes/scan.js';
import analyticsRoutes from './routes/analytics.js';
import settingsRoutes from './routes/settings.js';

// Import services
import { initializeDatabase } from './services/database.js';
import { startRealtimeMonitoring } from './services/monitoring.js';
import { initializeWebSocket } from './websocket/handler.js';

dotenv.config();

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app: Express = express();
const httpServer = createServer(app);
const io = new SocketIOServer(httpServer, {
  cors: {
    origin: process.env.CLIENT_URL || 'http://localhost:3000',
    credentials: true,
  },
});

const PORT = process.env.PORT || 3001;

// Security middleware
app.use(helmet({
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ["'self'"],
      styleSrc: ["'self'", "'unsafe-inline'"],
      scriptSrc: ["'self'"],
      imgSrc: ["'self'", 'data:', 'https:'],
    },
  },
  crossOriginEmbedderPolicy: false,
}));

// CORS configuration
app.use(cors({
  origin: process.env.CLIENT_URL || 'http://localhost:3000',
  credentials: true,
}));

// Enable trust proxy for Railway deployment (Load Balancers)
app.set('trust proxy', 1);

// Rate limiting
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100, // limit each IP to 100 requests per windowMs
  message: 'Too many requests from this IP, please try again later.',
});

app.use('/api/', limiter);

// Body parsing middleware
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// Health check endpoint
app.get('/health', (req, res) => {
  res.status(200).json({
    status: 'healthy',
    timestamp: new Date().toISOString(),
    service: 'CyberRecon Suite API',
    version: '2.0.0'
  });
});

// API routes
app.use('/api/auth', authRoutes);
app.use('/api/threat-intel', threatIntelRoutes);
app.use('/api/network', networkRoutes);
app.use('/api/siem', siemRoutes);
app.use('/api/edr', edrRoutes);
app.use('/api/compliance', complianceRoutes);
app.use('/api/scan', scanRoutes);
app.use('/api/analytics', analyticsRoutes);
app.use('/api/settings', settingsRoutes);

// Serve static files in production
if (process.env.NODE_ENV === 'production') {
  const clientBuildPath = path.join(__dirname, '../../dist/client');
  app.use(express.static(clientBuildPath));

  app.get('*', (req, res) => {
    res.sendFile(path.join(clientBuildPath, 'index.html'));
  });
}

// Error handling middleware
app.use((err: any, req: express.Request, res: express.Response, next: express.NextFunction) => {
  console.error('Error:', err);
  res.status(err.status || 500).json({
    error: {
      message: err.message || 'Internal server error',
      ...(process.env.NODE_ENV === 'development' && { stack: err.stack }),
    },
  });
});

// Initialize services
async function startServer() {
  try {
    // Initialize database
    await initializeDatabase();
    console.log('✓ Database initialized');

    // Initialize WebSocket
    initializeWebSocket(io);
    console.log('✓ WebSocket initialized');

    // Start real-time monitoring services
    startRealtimeMonitoring(io);
    console.log('✓ Real-time monitoring started');

    // Start server
    httpServer.listen(PORT, () => {
      console.log(`\n🚀 CyberRecon Suite Server running on port ${PORT}`);
      console.log(`   Environment: ${process.env.NODE_ENV || 'development'}`);
      console.log(`   Health check: http://localhost:${PORT}/health\n`);
    });
  } catch (error) {
    console.error('Failed to start server:', error);
    process.exit(1);
  }
}

// Graceful shutdown
process.on('SIGTERM', () => {
  console.log('SIGTERM received, shutting down gracefully...');
  httpServer.close(() => {
    console.log('Server closed');
    process.exit(0);
  });
});

startServer();
