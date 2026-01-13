import pg from 'pg';
import dotenv from 'dotenv';

dotenv.config();

const { Pool } = pg;

// Get DATABASE_URL or common alternatives
const databaseUrl = process.env.DATABASE_URL ||
  process.env.POSTGRES_URL ||
  process.env.POSTGRESQL_URL ||
  process.env.SUPABASE_URL ||
  process.env.NEON_URL;

// Create pool only if DATABASE_URL is available
export let pool: pg.Pool | null = null;

function createPool() {
  if (!databaseUrl || databaseUrl.trim() === '') {
    // Log available environment variables (keys only) to help debugging
    console.log('Environment variables available:', Object.keys(process.env).join(', '));
    return null;
  }

  // Only log in development for debugging
  if (process.env.NODE_ENV !== 'production') {
    console.log(`🔌 Connecting to database: ${databaseUrl.split('@')[1] || 'configured'}`);
  } else {
    console.log('🔌 Database configured');
  }

  return new Pool({
    connectionString: databaseUrl,
    ssl: process.env.NODE_ENV === 'production' ? { rejectUnauthorized: false } : false,
    max: 20,
    idleTimeoutMillis: 30000,
    connectionTimeoutMillis: 10000, // Increased timeout for Railway
  });
}

// Initialize pool
pool = createPool();

export async function initializeDatabase() {
  // Validate DATABASE_URL when database initialization is attempted
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
      throw new Error('Database pool is not initialized. DATABASE_URL is required.');
    }
  }

  if (!pool) {
    // Recreate pool if it wasn't created during module load
    pool = createPool();
    if (!pool) {
      throw new Error('Failed to create database pool despite having DATABASE_URL. The connection string may be invalid.');
    }
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
