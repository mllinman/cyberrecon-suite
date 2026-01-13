import pg from 'pg';
import dotenv from 'dotenv';

dotenv.config();

const { Pool } = pg;

// Validate DATABASE_URL is properly set
const databaseUrl = process.env.DATABASE_URL;

if (!databaseUrl || databaseUrl.trim() === '') {
  const errorMessage = 'DATABASE_URL environment variable is not set or is empty!';
  console.error(`❌ ${errorMessage}`);
  console.error('   Please set DATABASE_URL to connect to your PostgreSQL database.');
  console.error('   Example: postgresql://user:password@host:port/database');
  
  // In production (Railway), this is a critical error
  if (process.env.NODE_ENV === 'production') {
    console.error('   For Railway deployment:');
    console.error('   1. Add PostgreSQL plugin to your Railway project');
    console.error('   2. Link DATABASE_URL variable to your service');
    console.error('   3. Redeploy the application');
    throw new Error('DATABASE_URL is required in production');
  } else {
    console.warn('⚠️  Running without DATABASE_URL - database features will be unavailable');
  }
}

// Only log in development for debugging
if (process.env.NODE_ENV !== 'production' && databaseUrl) {
  console.log(`🔌 Connecting to database: ${databaseUrl.split('@')[1] || 'configured'}`);
} else if (databaseUrl) {
  console.log('🔌 Database configured');
}

export const pool = databaseUrl ? new Pool({
  connectionString: databaseUrl,
  ssl: process.env.NODE_ENV === 'production' ? { rejectUnauthorized: false } : false,
  max: 20,
  idleTimeoutMillis: 30000,
  connectionTimeoutMillis: 10000, // Increased timeout for Railway
}) : null;

export async function initializeDatabase() {
  if (!pool) {
    throw new Error('Database pool is not initialized. DATABASE_URL is required.');
  }

  try {
    // Test database connection first
    console.log('Testing database connection...');
    const result = await pool.query('SELECT NOW() as current_time');
    console.log(`✓ Database connection successful at ${result.rows[0].current_time}`);

    // Create tables
    await pool.query(`
      CREATE TABLE IF NOT EXISTS users (
        id SERIAL PRIMARY KEY,
        email VARCHAR(255) UNIQUE NOT NULL,
        password_hash VARCHAR(255) NOT NULL,
        full_name VARCHAR(255),
        role VARCHAR(50) DEFAULT 'user',
        subscription_tier VARCHAR(50) DEFAULT 'free',
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        last_login TIMESTAMP
      );
    `);

    await pool.query(`
      CREATE TABLE IF NOT EXISTS security_events (
        id SERIAL PRIMARY KEY,
        user_id INTEGER REFERENCES users(id),
        event_type VARCHAR(100) NOT NULL,
        severity VARCHAR(20) NOT NULL,
        source VARCHAR(255),
        destination VARCHAR(255),
        description TEXT,
        raw_data JSONB,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        resolved BOOLEAN DEFAULT FALSE
      );
    `);

    await pool.query(`
      CREATE TABLE IF NOT EXISTS threat_indicators (
        id SERIAL PRIMARY KEY,
        ioc_type VARCHAR(50) NOT NULL,
        ioc_value VARCHAR(500) NOT NULL,
        threat_level VARCHAR(20),
        source VARCHAR(255),
        description TEXT,
        tags VARCHAR(255)[],
        first_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        active BOOLEAN DEFAULT TRUE
      );
    `);

    await pool.query(`
      CREATE TABLE IF NOT EXISTS network_scans (
        id SERIAL PRIMARY KEY,
        user_id INTEGER REFERENCES users(id),
        scan_type VARCHAR(50) NOT NULL,
        target VARCHAR(255) NOT NULL,
        status VARCHAR(50) DEFAULT 'pending',
        results JSONB,
        started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        completed_at TIMESTAMP
      );
    `);

    await pool.query(`
      CREATE TABLE IF NOT EXISTS compliance_reports (
        id SERIAL PRIMARY KEY,
        user_id INTEGER REFERENCES users(id),
        framework VARCHAR(100) NOT NULL,
        score DECIMAL(5,2),
        findings JSONB,
        generated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      );
    `);

    await pool.query(`
      CREATE TABLE IF NOT EXISTS audit_logs (
        id SERIAL PRIMARY KEY,
        user_id INTEGER REFERENCES users(id),
        action VARCHAR(255) NOT NULL,
        resource VARCHAR(255),
        details JSONB,
        ip_address VARCHAR(45),
        user_agent TEXT,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      );
    `);

    // Create indexes for performance
    await pool.query(`
      CREATE INDEX IF NOT EXISTS idx_security_events_timestamp ON security_events(timestamp DESC);
      CREATE INDEX IF NOT EXISTS idx_security_events_user_id ON security_events(user_id);
      CREATE INDEX IF NOT EXISTS idx_threat_indicators_active ON threat_indicators(active);
      CREATE INDEX IF NOT EXISTS idx_network_scans_user_id ON network_scans(user_id);
      CREATE INDEX IF NOT EXISTS idx_audit_logs_timestamp ON audit_logs(timestamp DESC);
    `);

    console.log('Database schema initialized successfully');
  } catch (error) {
    console.error('Database initialization error:', error);
    throw error;
  }
}

export async function query(text: string, params?: any[]) {
  if (!pool) {
    throw new Error('Database pool is not initialized. DATABASE_URL is required.');
  }
  
  const start = Date.now();
  try {
    const res = await pool.query(text, params);
    const duration = Date.now() - start;
    console.log('Executed query', { text, duration, rows: res.rowCount });
    return res;
  } catch (error) {
    console.error('Query error:', error);
    throw error;
  }
}

export default { pool, query, initializeDatabase };
