import { Server as SocketIOServer } from 'socket.io';
import { broadcastToRoom } from '../websocket/handler.js';

export function startRealtimeMonitoring(io: SocketIOServer) {
  // Simulate real-time security events
  setInterval(() => {
    const event = generateMockSecurityEvent();
    broadcastToRoom(io, 'security-events', 'new-event', event);
  }, 5000);

  // Simulate real-time threat intelligence updates
  setInterval(() => {
    const threat = generateMockThreatUpdate();
    broadcastToRoom(io, 'threat-intel', 'new-threat', threat);
  }, 10000);

  // Simulate real-time network metrics
  setInterval(() => {
    const metrics = generateNetworkMetrics();
    broadcastToRoom(io, 'network-metrics', 'metrics-update', metrics);
  }, 2000);

  // Simulate EDR endpoint updates
  setInterval(() => {
    const endpointUpdate = generateEndpointUpdate();
    broadcastToRoom(io, 'edr-updates', 'endpoint-update', endpointUpdate);
  }, 7000);

  console.log('Real-time monitoring services started');
}

function generateMockSecurityEvent() {
  const eventTypes = [
    'Authentication Failure',
    'Port Scan Detected',
    'Malware Detection',
    'Suspicious Activity',
    'Firewall Block',
    'Intrusion Attempt',
  ];

  const severities = ['critical', 'high', 'medium', 'low'];
  const sources = [
    '192.168.1.100',
    '10.0.0.50',
    '172.16.0.1',
    '203.0.113.45',
  ];

  return {
    id: Math.floor(Math.random() * 100000),
    type: eventTypes[Math.floor(Math.random() * eventTypes.length)],
    severity: severities[Math.floor(Math.random() * severities.length)],
    source: sources[Math.floor(Math.random() * sources.length)],
    destination: sources[Math.floor(Math.random() * sources.length)],
    description: 'Automated security event detection',
    timestamp: new Date().toISOString(),
    resolved: false,
  };
}

function generateMockThreatUpdate() {
  const threatTypes = ['malware', 'phishing', 'exploit', 'c2', 'botnet'];
  const levels = ['critical', 'high', 'medium', 'low'];
  
  // Use predefined test IP addresses for more realistic simulation
  const testIPs = [
    '203.0.113.45',  // TEST-NET-3 (RFC 5737)
    '198.51.100.23', // TEST-NET-2 (RFC 5737)
    '192.0.2.100',   // TEST-NET-1 (RFC 5737)
    '10.0.0.50',     // Private network
    '172.16.0.25',   // Private network
  ];
  
  const now = Date.now();
  const randomIndex = Math.floor((now / 1000) % testIPs.length);

  return {
    id: Math.floor(Math.random() * 100000),
    type: threatTypes[Math.floor((now / 2000) % threatTypes.length)],
    indicator: testIPs[randomIndex],
    level: levels[Math.floor((now / 3000) % levels.length)],
    source: 'Threat Intelligence Feed',
    description: 'New threat indicator detected',
    timestamp: new Date().toISOString(),
  };
}

function generateNetworkMetrics() {
  return {
    timestamp: new Date().toISOString(),
    bandwidth: {
      upload: Math.random() * 100,
      download: Math.random() * 100,
    },
    latency: Math.random() * 50,
    packetLoss: Math.random() * 2,
    activeConnections: Math.floor(Math.random() * 1000) + 500,
    threats: {
      detected: Math.floor(Math.random() * 10),
      blocked: Math.floor(Math.random() * 20),
    },
    cpu: Math.random() * 100,
    memory: Math.random() * 100,
  };
}

function generateEndpointUpdate() {
  const statuses = ['healthy', 'warning', 'critical'];
  
  return {
    endpointId: Math.floor(Math.random() * 100) + 1,
    hostname: `ENDPOINT-${Math.floor(Math.random() * 100)}`,
    status: statuses[Math.floor(Math.random() * statuses.length)],
    threats: Math.floor(Math.random() * 3),
    lastSeen: new Date().toISOString(),
  };
}

export default { startRealtimeMonitoring };
