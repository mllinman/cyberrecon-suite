import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Activity, Wifi, Shield, AlertCircle, Server, Network, Zap } from 'lucide-react'

interface NetworkConnection {
  id: string
  sourceIP: string
  destIP: string
  port: number
  protocol: string
  status: 'active' | 'blocked' | 'monitoring'
  bytes: number
  packets: number
  risk: 'low' | 'medium' | 'high' | 'critical'
  timestamp: Date
  country?: string
  asn?: string
}

interface NetworkStats {
  totalConnections: number
  activeConnections: number
  blockedConnections: number
  bandwidth: number
  packetsPerSecond: number
  alertsGenerated: number
}

const NetworkMonitoringDashboard: React.FC = () => {
  const [connections, setConnections] = useState<NetworkConnection[]>([])
  const [stats, setStats] = useState<NetworkStats>({
    totalConnections: 0,
    activeConnections: 0,
    blockedConnections: 0,
    bandwidth: 0,
    packetsPerSecond: 0,
    alertsGenerated: 0
  })
  const [isMonitoring, setIsMonitoring] = useState(true)

  useEffect(() => {
    const generateConnection = (): NetworkConnection => {
      const protocols = ['TCP', 'UDP', 'ICMP', 'HTTP', 'HTTPS', 'SSH', 'FTP', 'DNS']
      const ports = [80, 443, 22, 21, 53, 3389, 8080, 8443, 445, 135, 139]
      const risks: Array<'low' | 'medium' | 'high' | 'critical'> = ['low', 'medium', 'high', 'critical']
      const statuses: Array<'active' | 'blocked' | 'monitoring'> = ['active', 'blocked', 'monitoring']
      const countries = ['US', 'CN', 'RU', 'DE', 'GB', 'FR', 'KR', 'JP', 'CA', 'AU']
      
      const generateIP = () => {
        // Generate realistic internal/external IP addresses
        const isInternal = Math.random() > 0.7
        if (isInternal) {
          return `192.168.${Math.floor(Math.random() * 256)}.${Math.floor(Math.random() * 256)}`
        } else {
          return `${Math.floor(Math.random() * 223) + 1}.${Math.floor(Math.random() * 256)}.${Math.floor(Math.random() * 256)}.${Math.floor(Math.random() * 256)}`
        }
      }

      const risk = risks[Math.floor(Math.random() * risks.length)]
      const status = risk === 'critical' ? 'blocked' : statuses[Math.floor(Math.random() * statuses.length)]

      return {
        id: `conn-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
        sourceIP: generateIP(),
        destIP: generateIP(),
        port: ports[Math.floor(Math.random() * ports.length)],
        protocol: protocols[Math.floor(Math.random() * protocols.length)],
        status,
        bytes: Math.floor(Math.random() * 1000000) + 1024,
        packets: Math.floor(Math.random() * 10000) + 100,
        risk,
        timestamp: new Date(),
        country: countries[Math.floor(Math.random() * countries.length)],
        asn: `AS${Math.floor(Math.random() * 70000) + 1000}`
      }
    }

    const updateStats = (newConnection: NetworkConnection) => {
      setStats(prevStats => ({
        totalConnections: prevStats.totalConnections + 1,
        activeConnections: Math.floor(Math.random() * 50) + 20,
        blockedConnections: newConnection.status === 'blocked' 
          ? prevStats.blockedConnections + 1 
          : prevStats.blockedConnections,
        bandwidth: Math.floor(Math.random() * 1000) + 100, // Mbps
        packetsPerSecond: Math.floor(Math.random() * 50000) + 10000,
        alertsGenerated: newConnection.risk === 'critical' || newConnection.risk === 'high'
          ? prevStats.alertsGenerated + 1
          : prevStats.alertsGenerated
      }))
    }

    const addConnection = () => {
      if (isMonitoring) {
        const newConnection = generateConnection()
        setConnections(prevConnections => [newConnection, ...prevConnections.slice(0, 49)]) // Keep latest 50
        updateStats(newConnection)
      }
    }

    // Initialize with some connections
    const initialConnections = Array.from({ length: 20 }, generateConnection)
    setConnections(initialConnections)
    
    // Set up real-time updates
    const interval = setInterval(addConnection, 1000 + Math.random() * 2000) // 1-3 seconds

    return () => clearInterval(interval)
  }, [isMonitoring])

  const getRiskColor = (risk: string) => {
    switch (risk) {
      case 'critical': return 'text-red-400 bg-red-500/20'
      case 'high': return 'text-orange-400 bg-orange-500/20'
      case 'medium': return 'text-yellow-400 bg-yellow-500/20'
      case 'low': return 'text-green-400 bg-green-500/20'
      default: return 'text-gray-400 bg-gray-500/20'
    }
  }

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'active': return 'text-green-400 bg-green-500/20'
      case 'blocked': return 'text-red-400 bg-red-500/20'
      case 'monitoring': return 'text-blue-400 bg-blue-500/20'
      default: return 'text-gray-400 bg-gray-500/20'
    }
  }

  const formatBytes = (bytes: number) => {
    if (bytes === 0) return '0 B'
    const k = 1024
    const sizes = ['B', 'KB', 'MB', 'GB']
    const i = Math.floor(Math.log(bytes) / Math.log(k))
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i]
  }

  return (
    <div className="min-h-screen bg-slate-900 text-white p-6">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <div>
            <h1 className="text-3xl font-bold text-primary-400 mb-2">
              🌐 Real-Time Network Monitoring
            </h1>
            <p className="text-gray-400">Live network traffic analysis and threat detection</p>
          </div>
          <div className="flex items-center gap-4">
            <motion.div
              className={`flex items-center gap-2 px-4 py-2 rounded-full ${
                isMonitoring ? 'bg-green-500/20 text-green-400' : 'bg-gray-500/20 text-gray-400'
              }`}
              animate={{ scale: isMonitoring ? [1, 1.05, 1] : 1 }}
              transition={{ duration: 2, repeat: isMonitoring ? Infinity : 0 }}
            >
              <Activity className="w-4 h-4" />
              {isMonitoring ? 'Live Monitoring' : 'Paused'}
            </motion.div>
            <button
              onClick={() => setIsMonitoring(!isMonitoring)}
              className="px-4 py-2 bg-primary-600 hover:bg-primary-700 rounded-lg transition-colors"
            >
              {isMonitoring ? 'Pause' : 'Resume'}
            </button>
          </div>
        </div>

        {/* Network Stats */}
        <div className="grid grid-cols-1 md:grid-cols-3 lg:grid-cols-6 gap-6 mb-8">
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Total</p>
                <p className="text-xl font-bold text-white">{stats.totalConnections.toLocaleString()}</p>
              </div>
              <Network className="w-6 h-6 text-primary-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.1 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Active</p>
                <p className="text-xl font-bold text-green-400">{stats.activeConnections}</p>
              </div>
              <Wifi className="w-6 h-6 text-green-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.2 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Blocked</p>
                <p className="text-xl font-bold text-red-400">{stats.blockedConnections}</p>
              </div>
              <Shield className="w-6 h-6 text-red-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.3 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Bandwidth</p>
                <p className="text-xl font-bold text-blue-400">{stats.bandwidth} Mbps</p>
              </div>
              <Zap className="w-6 h-6 text-blue-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.4 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">PPS</p>
                <p className="text-xl font-bold text-yellow-400">{stats.packetsPerSecond.toLocaleString()}</p>
              </div>
              <Server className="w-6 h-6 text-yellow-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.5 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Alerts</p>
                <p className="text-xl font-bold text-orange-400">{stats.alertsGenerated}</p>
              </div>
              <AlertCircle className="w-6 h-6 text-orange-400" />
            </div>
          </motion.div>
        </div>

        {/* Connection Feed */}
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ delay: 0.6 }}
          className="glass-card p-6"
        >
          <div className="flex items-center justify-between mb-6">
            <h2 className="text-xl font-bold text-white">Live Network Connections</h2>
            <Activity className="w-6 h-6 text-primary-400" />
          </div>

          <div className="overflow-x-auto">
            <table className="w-full text-sm">
              <thead>
                <tr className="border-b border-slate-700">
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Time</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Source</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Destination</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Port/Proto</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Data</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Risk</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Status</th>
                  <th className="text-left py-3 px-2 text-gray-400 font-medium">Location</th>
                </tr>
              </thead>
              <tbody>
                {connections.map((conn, index) => (
                  <motion.tr
                    key={conn.id}
                    initial={{ opacity: 0, x: -20 }}
                    animate={{ opacity: 1, x: 0 }}
                    transition={{ delay: index * 0.05 }}
                    className="border-b border-slate-800 hover:bg-slate-800/30"
                  >
                    <td className="py-3 px-2 text-gray-300">
                      {conn.timestamp.toLocaleTimeString()}
                    </td>
                    <td className="py-3 px-2">
                      <code className="text-primary-400 text-xs">{conn.sourceIP}</code>
                    </td>
                    <td className="py-3 px-2">
                      <code className="text-primary-400 text-xs">{conn.destIP}</code>
                    </td>
                    <td className="py-3 px-2 text-gray-300">
                      {conn.port}/{conn.protocol}
                    </td>
                    <td className="py-3 px-2 text-gray-300">
                      <div className="text-xs">
                        <div>{formatBytes(conn.bytes)}</div>
                        <div className="text-gray-500">{conn.packets} pkts</div>
                      </div>
                    </td>
                    <td className="py-3 px-2">
                      <span className={`px-2 py-1 rounded text-xs font-medium ${getRiskColor(conn.risk)}`}>
                        {conn.risk}
                      </span>
                    </td>
                    <td className="py-3 px-2">
                      <span className={`px-2 py-1 rounded text-xs font-medium ${getStatusColor(conn.status)}`}>
                        {conn.status}
                      </span>
                    </td>
                    <td className="py-3 px-2 text-gray-400 text-xs">
                      {conn.country} / {conn.asn}
                    </td>
                  </motion.tr>
                ))}
              </tbody>
            </table>
          </div>
        </motion.div>
      </div>
    </div>
  )
}

export default NetworkMonitoringDashboard