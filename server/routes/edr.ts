import express, { Router, Request, Response } from 'express';

const router: Router = express.Router();

// EDR endpoint monitoring
router.get('/endpoints', async (req: Request, res: Response) => {
  try {
    // Mock endpoint data
    const endpoints = [
      {
        id: 1,
        hostname: 'WORKSTATION-001',
        ipAddress: '192.168.1.100',
        os: 'Windows 11 Pro',
        status: 'healthy',
        lastSeen: new Date().toISOString(),
        threats: 0,
        vulnerabilities: 2,
      },
      {
        id: 2,
        hostname: 'SERVER-PROD-01',
        ipAddress: '10.0.1.50',
        os: 'Ubuntu 22.04 LTS',
        status: 'healthy',
        lastSeen: new Date().toISOString(),
        threats: 0,
        vulnerabilities: 0,
      },
      {
        id: 3,
        hostname: 'LAPTOP-DEV-05',
        ipAddress: '192.168.1.105',
        os: 'macOS Sonoma 14.2',
        status: 'warning',
        lastSeen: new Date(Date.now() - 3600000).toISOString(),
        threats: 1,
        vulnerabilities: 3,
      },
    ];

    res.json({
      endpoints,
      summary: {
        total: endpoints.length,
        healthy: endpoints.filter(e => e.status === 'healthy').length,
        warning: endpoints.filter(e => e.status === 'warning').length,
        critical: endpoints.filter(e => e.status === 'critical').length,
      },
    });
  } catch (error) {
    console.error('Get endpoints error:', error);
    res.status(500).json({ error: 'Failed to fetch endpoints' });
  }
});

// Get endpoint details
router.get('/endpoints/:id', async (req: Request, res: Response) => {
  try {
    const { id } = req.params;

    // Mock detailed endpoint data
    const endpoint = {
      id: Number(id),
      hostname: `ENDPOINT-${id}`,
      ipAddress: '192.168.1.100',
      macAddress: '00:11:22:33:44:55',
      os: 'Windows 11 Pro',
      osVersion: '22H2',
      status: 'healthy',
      lastSeen: new Date().toISOString(),
      agent: {
        version: '2.0.0',
        status: 'active',
        lastUpdate: new Date().toISOString(),
      },
      security: {
        antivirusStatus: 'enabled',
        firewallStatus: 'enabled',
        encryptionStatus: 'enabled',
      },
      processes: [
        { pid: 1234, name: 'chrome.exe', cpu: 5.2, memory: 512 },
        { pid: 5678, name: 'explorer.exe', cpu: 1.1, memory: 128 },
      ],
      threats: [],
      vulnerabilities: [
        {
          id: 'VULN-001',
          severity: 'medium',
          description: 'Outdated software version',
          remediation: 'Update to latest version',
        },
      ],
    };

    res.json(endpoint);
  } catch (error) {
    console.error('Get endpoint details error:', error);
    res.status(500).json({ error: 'Failed to fetch endpoint details' });
  }
});

// Get EDR dashboard statistics
router.get('/dashboard', async (req: Request, res: Response) => {
  try {
    const stats = {
      endpoints: {
        total: 150,
        online: 142,
        offline: 8,
        healthy: 135,
        warning: 10,
        critical: 5,
      },
      threats: {
        detected: 23,
        blocked: 21,
        investigating: 2,
        resolved: 150,
      },
      vulnerabilities: {
        critical: 5,
        high: 15,
        medium: 45,
        low: 80,
      },
      compliance: {
        score: 92,
        compliant: 138,
        nonCompliant: 12,
      },
    };

    res.json(stats);
  } catch (error) {
    console.error('Get EDR dashboard error:', error);
    res.status(500).json({ error: 'Failed to fetch dashboard data' });
  }
});

// Isolate endpoint
router.post('/endpoints/:id/isolate', async (req: Request, res: Response) => {
  try {
    const { id } = req.params;

    // Simulate endpoint isolation
    res.json({
      success: true,
      message: `Endpoint ${id} isolated successfully`,
      timestamp: new Date().toISOString(),
    });
  } catch (error) {
    console.error('Isolate endpoint error:', error);
    res.status(500).json({ error: 'Failed to isolate endpoint' });
  }
});

export default router;
