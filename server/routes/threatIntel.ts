import express, { Router, Request, Response } from 'express';
import { query } from '../services/database.js';

const router: Router = express.Router();

// Get threat indicators
router.get('/indicators', async (req: Request, res: Response) => {
  try {
    const { type, active = 'true', limit = 100 } = req.query;
    
    let queryText = 'SELECT * FROM threat_indicators WHERE active = $1';
    const params: any[] = [active === 'true'];

    if (type) {
      queryText += ' AND ioc_type = $2';
      params.push(type);
    }

    queryText += ` ORDER BY last_seen DESC LIMIT $${params.length + 1}`;
    params.push(Number(limit));

    const result = await query(queryText, params);

    res.json({
      indicators: result.rows,
      total: result.rowCount,
    });
  } catch (error) {
    console.error('Get indicators error:', error);
    res.status(500).json({ error: 'Failed to fetch indicators' });
  }
});

// Add threat indicator
router.post('/indicators', async (req: Request, res: Response) => {
  try {
    const { iocType, iocValue, threatLevel, source, description, tags } = req.body;

    const result = await query(
      `INSERT INTO threat_indicators 
       (ioc_type, ioc_value, threat_level, source, description, tags) 
       VALUES ($1, $2, $3, $4, $5, $6) 
       RETURNING *`,
      [iocType, iocValue, threatLevel, source, description, tags]
    );

    res.status(201).json(result.rows[0]);
  } catch (error) {
    console.error('Add indicator error:', error);
    res.status(500).json({ error: 'Failed to add indicator' });
  }
});

// Search IOCs
router.post('/search', async (req: Request, res: Response) => {
  try {
    const { value, type } = req.body;

    let queryText = 'SELECT * FROM threat_indicators WHERE ioc_value ILIKE $1';
    const params: any[] = [`%${value}%`];

    if (type) {
      queryText += ' AND ioc_type = $2';
      params.push(type);
    }

    const result = await query(queryText, params);

    res.json({
      results: result.rows,
      found: (result.rowCount || 0) > 0,
    });
  } catch (error) {
    console.error('Search IOC error:', error);
    res.status(500).json({ error: 'Search failed' });
  }
});

// Get threat statistics
router.get('/stats', async (req: Request, res: Response) => {
  try {
    const stats = await query(`
      SELECT 
        COUNT(*) as total_indicators,
        COUNT(CASE WHEN threat_level = 'critical' THEN 1 END) as critical_threats,
        COUNT(CASE WHEN threat_level = 'high' THEN 1 END) as high_threats,
        COUNT(CASE WHEN threat_level = 'medium' THEN 1 END) as medium_threats,
        COUNT(CASE WHEN threat_level = 'low' THEN 1 END) as low_threats,
        COUNT(CASE WHEN active = true THEN 1 END) as active_indicators
      FROM threat_indicators
    `);

    res.json(stats.rows[0]);
  } catch (error) {
    console.error('Get stats error:', error);
    res.status(500).json({ error: 'Failed to fetch statistics' });
  }
});

// Get recent threats
router.get('/recent', async (req: Request, res: Response) => {
  try {
    const { hours = 24, limit = 50 } = req.query;
    
    // Validate hours parameter
    const hoursNum = Number(hours);
    if (isNaN(hoursNum) || hoursNum < 1 || hoursNum > 720) {
      return res.status(400).json({ error: 'Invalid hours parameter' });
    }

    const result = await query(
      `SELECT * FROM threat_indicators 
       WHERE last_seen >= NOW() - INTERVAL '1 hour' * $1
       AND active = true
       ORDER BY last_seen DESC
       LIMIT $2`,
      [hoursNum, Number(limit)]
    );

    res.json({
      threats: result.rows,
      count: result.rowCount || 0,
      timeframe: `${hours} hours`,
    });
  } catch (error) {
    console.error('Get recent threats error:', error);
    res.status(500).json({ error: 'Failed to fetch recent threats' });
  }
});

export default router;
