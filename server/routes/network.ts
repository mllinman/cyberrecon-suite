import express, { Router, Request, Response } from 'express';
import { query } from '../services/database.js';
import { v4 as uuidv4 } from 'uuid';

const router: Router = express.Router();

// Start network scan
router.post('/scan', async (req: Request, res: Response) => {
  try {
    const { userId, scanType, target } = req.body;

    // Validate input
    if (!scanType || !target) {
      return res.status(400).json({ error: 'Missing required fields' });
    }

    // Create scan record
    const result = await query(
      `INSERT INTO network_scans (user_id, scan_type, target, status)
       VALUES ($1, $2, $3, 'running')
       RETURNING *`,
      [userId, scanType, target]
    );

    const scan = result.rows[0];

    // Simulate scan (in production, this would trigger actual scanning)
    setTimeout(async () => {
      const mockResults = generateMockScanResults(scanType, target);
      await query(
        `UPDATE network_scans 
         SET status = 'completed', results = $1, completed_at = CURRENT_TIMESTAMP
         WHERE id = $2`,
        [JSON.stringify(mockResults), scan.id]
      );
    }, 5000);

    res.status(201).json({
      scanId: scan.id,
      status: 'running',
      message: 'Scan initiated successfully',
    });
  } catch (error) {
    console.error('Start scan error:', error);
    res.status(500).json({ error: 'Failed to start scan' });
  }
});

// Get scan status
router.get('/scan/:id', async (req: Request, res: Response) => {
  try {
    const { id } = req.params;

    const result = await query(
      'SELECT * FROM network_scans WHERE id = $1',
      [id]
    );

    if (result.rowCount === 0) {
      return res.status(404).json({ error: 'Scan not found' });
    }

    res.json(result.rows[0]);
  } catch (error) {
    console.error('Get scan error:', error);
    res.status(500).json({ error: 'Failed to fetch scan' });
  }
});

// Get user's scans
router.get('/scans/user/:userId', async (req: Request, res: Response) => {
  try {
    const { userId } = req.params;
    const { limit = 20 } = req.query;

    const result = await query(
      `SELECT * FROM network_scans 
       WHERE user_id = $1 
       ORDER BY started_at DESC 
       LIMIT $2`,
      [userId, Number(limit)]
    );

    res.json({
      scans: result.rows,
      total: result.rowCount,
    });
  } catch (error) {
    console.error('Get user scans error:', error);
    res.status(500).json({ error: 'Failed to fetch scans' });
  }
});

// Network monitoring - Get real-time metrics
router.get('/metrics', async (req: Request, res: Response) => {
  try {
    // Simulate real-time network metrics
    const metrics = {
      timestamp: new Date().toISOString(),
      bandwidth: {
        upload: Math.random() * 100,
        download: Math.random() * 100,
      },
      latency: Math.random() * 50,
      packetLoss: Math.random() * 2,
      activeConnections: Math.floor(Math.random() * 1000),
      threats: {
        detected: Math.floor(Math.random() * 10),
        blocked: Math.floor(Math.random() * 20),
      },
    };

    res.json(metrics);
  } catch (error) {
    console.error('Get metrics error:', error);
    res.status(500).json({ error: 'Failed to fetch metrics' });
  }
});

// Port scan
router.post('/port-scan', async (req: Request, res: Response) => {
  try {
    const { target, ports = '1-1000' } = req.body;

    // Simulate port scanning
    const openPorts = generateMockPortScan(target, ports);

    res.json({
      target,
      openPorts,
      scanTime: new Date().toISOString(),
    });
  } catch (error) {
    console.error('Port scan error:', error);
    res.status(500).json({ error: 'Port scan failed' });
  }
});

// Helper functions
function generateMockScanResults(scanType: string, target: string) {
  const baseResults = {
    scanId: uuidv4(),
    target,
    scanType,
    timestamp: new Date().toISOString(),
  };

  switch (scanType) {
    case 'vulnerability':
      return {
        ...baseResults,
        vulnerabilities: [
          {
            id: 'CVE-2024-0001',
            severity: 'high',
            description: 'Sample vulnerability found',
            affected: target,
            remediation: 'Update to latest version',
          },
        ],
        summary: {
          critical: 0,
          high: 1,
          medium: 2,
          low: 5,
        },
      };
    case 'port':
      return {
        ...baseResults,
        openPorts: [
          { port: 80, service: 'HTTP', version: 'Apache 2.4' },
          { port: 443, service: 'HTTPS', version: 'nginx 1.18' },
          { port: 22, service: 'SSH', version: 'OpenSSH 8.2' },
        ],
      };
    default:
      return {
        ...baseResults,
        hosts: [
          { ip: target, status: 'up', os: 'Linux', openPorts: [22, 80, 443] },
        ],
      };
  }
}

// Helper function for deterministic port scan simulation
function generateMockPortScan(target: string, ports: string) {
  // Use target string to seed deterministic results
  const seed = target.split('').reduce((acc, char) => acc + char.charCodeAt(0), 0);
  const commonPorts = [21, 22, 23, 25, 80, 110, 143, 443, 3306, 5432, 8080];
  
  // Deterministically select ports based on target
  const openPorts = commonPorts.filter((port, index) => (seed + index) % 3 === 0).map(port => ({
    port,
    state: 'open',
    service: getServiceName(port),
  }));

  return openPorts;
}

function getServiceName(port: number): string {
  const services: { [key: number]: string } = {
    21: 'FTP',
    22: 'SSH',
    23: 'Telnet',
    25: 'SMTP',
    80: 'HTTP',
    110: 'POP3',
    143: 'IMAP',
    443: 'HTTPS',
    3306: 'MySQL',
    5432: 'PostgreSQL',
    8080: 'HTTP-Proxy',
  };
  return services[port] || 'Unknown';
}

export default router;
