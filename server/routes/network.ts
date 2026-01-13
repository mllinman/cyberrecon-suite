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

    // triggers active scanning asynchronously
    (async () => {
      try {
        let results;
        if (scanType === 'port_scan' || scanType === 'full_scan') {
          const defaultPorts = '21,22,23,25,53,80,110,135,139,143,443,445,3306,3389,5432,8080';
          const portList = parsePorts(defaultPorts);
          const openPorts: any[] = [];

          // Scan with concurrency limit (same as /port-scan)
          const batchSize = 10;
          for (let i = 0; i < portList.length; i += batchSize) {
            const batch = portList.slice(i, i + batchSize);
            const batchResults = await Promise.all(batch.map(port => checkPort(target, port)));
            openPorts.push(...batchResults.filter(r => r !== null));
          }
          results = { openPorts, scannedPorts: portList.length };
        } else {
          // Placeholder for other scan types (e.g. ping)
          results = { message: 'Scan type simulation completed' };
        }

        await query(
          `UPDATE network_scans 
           SET status = 'completed', results = $1, completed_at = CURRENT_TIMESTAMP
           WHERE id = $2`,
          [JSON.stringify(results), scan.id]
        );
      } catch (err) {
        console.error('Async scan failed:', err);
        await query(
          `UPDATE network_scans SET status = 'failed' WHERE id = $1`,
          [scan.id]
        );
      }
    })();

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

import net from 'net';
import os from 'os';

// ... (existing imports)

// Network monitoring - Get real-time system metrics
router.get('/metrics', async (req: Request, res: Response) => {
  try {
    const cpus = os.cpus();
    const totalMem = os.totalmem();
    const freeMem = os.freemem();
    const usedMem = totalMem - freeMem;
    const cpuUsage = cpus.reduce((acc, cpu) => {
      const total = Object.values(cpu.times).reduce((a, b) => a + b, 0);
      const idle = cpu.times.idle;
      return acc + ((total - idle) / total);
    }, 0) / cpus.length;

    // Simulate network stats since we can't access low-level driver stats easily in node without libs
    // But we serve real OS stats for CPU/RAM
    const metrics = {
      timestamp: new Date().toISOString(),
      bandwidth: {
        upload: Math.random() * 50, // Placeholder for real network I/O
        download: Math.random() * 50,
      },
      system: {
        cpuUsage: Math.round(cpuUsage * 100),
        memoryUsage: Math.round((usedMem / totalMem) * 100),
        uptime: os.uptime()
      },
      latency: Math.random() * 20, // Placeholder
      activeConnections: Math.floor(Math.random() * 500) // Placeholder
    };

    res.json(metrics);
  } catch (error) {
    console.error('Get metrics error:', error);
    res.status(500).json({ error: 'Failed to fetch metrics' });
  }
});

// Real Port scan
router.post('/port-scan', async (req: Request, res: Response) => {
  try {
    const { target, ports = '21,22,23,25,53,80,110,135,139,143,443,445,3306,3389,5432,8080' } = req.body;

    // Parse ports
    const portList = parsePorts(ports);
    const openPorts: any[] = [];

    // Scan ports with a concurrency limit
    const batchSize = 10;
    for (let i = 0; i < portList.length; i += batchSize) {
      const batch = portList.slice(i, i + batchSize);
      const results = await Promise.all(batch.map(port => checkPort(target, port)));
      openPorts.push(...results.filter(r => r !== null));
    }

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

// Helper: Check a single port
function checkPort(host: string, port: number): Promise<any> {
  return new Promise((resolve) => {
    const socket = new net.Socket();
    socket.setTimeout(2000); // 2s timeout

    socket.on('connect', () => {
      socket.destroy();
      resolve({
        port,
        state: 'open',
        service: getServiceName(port)
      });
    });

    socket.on('timeout', () => {
      socket.destroy();
      resolve(null);
    });

    socket.on('error', (err) => {
      socket.destroy();
      resolve(null);
    });

    socket.connect(port, host);
  });
}

// Helper: Parse port string (e.g., "80,443,8000-8010")
function parsePorts(portStr: string): number[] {
  const ports: Set<number> = new Set();
  const parts = portStr.split(',');

  parts.forEach(part => {
    if (part.includes('-')) {
      const [start, end] = part.split('-').map(Number);
      for (let i = start; i <= end; i++) {
        if (i > 0 && i <= 65535) ports.add(i);
      }
    } else {
      const port = Number(part);
      if (port > 0 && port <= 65535) ports.add(port);
    }
  });

  return Array.from(ports).slice(0, 50); // Limit to 50 ports for safety/speed in this demo
}

function getServiceName(port: number): string {
  const services: { [key: number]: string } = {
    21: 'FTP', 22: 'SSH', 23: 'Telnet', 25: 'SMTP', 53: 'DNS',
    80: 'HTTP', 110: 'POP3', 135: 'RPC', 139: 'NetBIOS', 143: 'IMAP',
    443: 'HTTPS', 445: 'SMB', 3306: 'MySQL', 3389: 'RDP',
    5432: 'PostgreSQL', 8080: 'HTTP-Proxy'
  };
  return services[port] || 'Unknown';
}

export default router;
