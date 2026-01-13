import express, { Router, Request, Response } from 'express';
import { query } from '../services/database.js';

const router: Router = express.Router();

// Get security events
router.get('/events', async (req: Request, res: Response) => {
  try {
    const { 
      severity, 
      resolved = 'false',
      limit = 100,
      offset = 0 
    } = req.query;

    let queryText = 'SELECT * FROM security_events WHERE 1=1';
    const params: any[] = [];
    let paramIndex = 1;

    if (severity) {
      queryText += ` AND severity = $${paramIndex}`;
      params.push(severity);
      paramIndex++;
    }

    if (resolved !== 'all') {
      queryText += ` AND resolved = $${paramIndex}`;
      params.push(resolved === 'true');
      paramIndex++;
    }

    queryText += ` ORDER BY timestamp DESC LIMIT $${paramIndex} OFFSET $${paramIndex + 1}`;
    params.push(Number(limit), Number(offset));

    const result = await query(queryText, params);
    
    // Get total count
    const countResult = await query(
      'SELECT COUNT(*) FROM security_events WHERE resolved = $1',
      [resolved === 'true']
    );

    res.json({
      events: result.rows,
      total: parseInt(countResult.rows[0].count),
      limit: Number(limit),
      offset: Number(offset),
    });
  } catch (error) {
    console.error('Get events error:', error);
    res.status(500).json({ error: 'Failed to fetch events' });
  }
});

// Create security event
router.post('/events', async (req: Request, res: Response) => {
  try {
    const {
      eventType,
      severity,
      source,
      destination,
      description,
      rawData,
      userId,
    } = req.body;

    const result = await query(
      `INSERT INTO security_events 
       (user_id, event_type, severity, source, destination, description, raw_data)
       VALUES ($1, $2, $3, $4, $5, $6, $7)
       RETURNING *`,
      [userId, eventType, severity, source, destination, description, JSON.stringify(rawData)]
    );

    res.status(201).json(result.rows[0]);
  } catch (error) {
    console.error('Create event error:', error);
    res.status(500).json({ error: 'Failed to create event' });
  }
});

// Resolve event
router.patch('/events/:id/resolve', async (req: Request, res: Response) => {
  try {
    const { id } = req.params;

    const result = await query(
      'UPDATE security_events SET resolved = true WHERE id = $1 RETURNING *',
      [id]
    );

    if (result.rowCount === 0) {
      return res.status(404).json({ error: 'Event not found' });
    }

    res.json(result.rows[0]);
  } catch (error) {
    console.error('Resolve event error:', error);
    res.status(500).json({ error: 'Failed to resolve event' });
  }
});

// Get SIEM dashboard statistics
router.get('/dashboard', async (req: Request, res: Response) => {
  try {
    const stats = await query(`
      SELECT 
        COUNT(*) as total_events,
        COUNT(CASE WHEN severity = 'critical' THEN 1 END) as critical_events,
        COUNT(CASE WHEN severity = 'high' THEN 1 END) as high_events,
        COUNT(CASE WHEN severity = 'medium' THEN 1 END) as medium_events,
        COUNT(CASE WHEN severity = 'low' THEN 1 END) as low_events,
        COUNT(CASE WHEN resolved = false THEN 1 END) as unresolved_events,
        COUNT(CASE WHEN timestamp >= NOW() - INTERVAL '1 hour' THEN 1 END) as events_last_hour,
        COUNT(CASE WHEN timestamp >= NOW() - INTERVAL '24 hours' THEN 1 END) as events_last_24h
      FROM security_events
    `);

    // Get event types distribution
    const typeStats = await query(`
      SELECT event_type, COUNT(*) as count
      FROM security_events
      WHERE timestamp >= NOW() - INTERVAL '24 hours'
      GROUP BY event_type
      ORDER BY count DESC
      LIMIT 10
    `);

    // Get hourly trend
    const trendData = await query(`
      SELECT 
        DATE_TRUNC('hour', timestamp) as hour,
        COUNT(*) as event_count
      FROM security_events
      WHERE timestamp >= NOW() - INTERVAL '24 hours'
      GROUP BY hour
      ORDER BY hour
    `);

    res.json({
      overview: stats.rows[0],
      eventTypes: typeStats.rows,
      trend: trendData.rows,
    });
  } catch (error) {
    console.error('Get dashboard error:', error);
    res.status(500).json({ error: 'Failed to fetch dashboard data' });
  }
});

export default router;
