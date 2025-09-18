import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Shield, AlertTriangle, Globe, Activity, TrendingUp, Eye } from 'lucide-react'

interface ThreatData {
  id: string
  severity: 'critical' | 'high' | 'medium' | 'low'
  type: string
  source: string
  description: string
  timestamp: Date
  confidence: number
  ioCs: string[]
}

interface ThreatStats {
  totalThreats: number
  criticalThreats: number
  blockedAttacks: number
  activeInvestigations: number
}

const ThreatIntelligenceDashboard: React.FC = () => {
  const [threats, setThreats] = useState<ThreatData[]>([])
  const [stats, setStats] = useState<ThreatStats>({
    totalThreats: 0,
    criticalThreats: 0,
    blockedAttacks: 0,
    activeInvestigations: 0
  })
  const [isLive, setIsLive] = useState(true)

  // Real-time threat intelligence simulation
  useEffect(() => {
    const generateThreatData = (): ThreatData => {
      const threatTypes = [
        'Malware Campaign',
        'Phishing Attack',
        'Command & Control',
        'Data Exfiltration',
        'APT Activity',
        'Botnet Traffic',
        'Cryptojacking',
        'Ransomware Indicators'
      ]
      
      const sources = [
        'VirusTotal',
        'AlienVault OTX',
        'Abuse.ch',
        'Internal Honeypot',
        'Network Sensors',
        'Email Gateway',
        'DNS Monitoring',
        'Endpoint Detection'
      ]

      const severities: Array<'critical' | 'high' | 'medium' | 'low'> = ['critical', 'high', 'medium', 'low']
      const severity = severities[Math.floor(Math.random() * severities.length)]
      
      return {
        id: `threat-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
        severity,
        type: threatTypes[Math.floor(Math.random() * threatTypes.length)],
        source: sources[Math.floor(Math.random() * sources.length)],
        description: generateThreatDescription(severity),
        timestamp: new Date(),
        confidence: Math.floor(Math.random() * 30) + 70, // 70-100% confidence
        ioCs: generateIoCs()
      }
    }

    const generateThreatDescription = (severity: string): string => {
      const descriptions = {
        critical: [
          'Active exploitation detected targeting critical infrastructure',
          'Zero-day vulnerability being actively exploited',
          'Advanced persistent threat (APT) command and control identified',
          'Critical ransomware payload detected in network traffic'
        ],
        high: [
          'Suspicious network activity matching known attack patterns',
          'Malicious domain resolution attempts blocked',
          'Phishing campaign targeting organization email addresses',
          'Unusual data transfer patterns detected'
        ],
        medium: [
          'Potential security policy violation detected',
          'Suspicious user behavior pattern identified',
          'Minor configuration drift from security baseline',
          'Elevated privilege usage outside normal hours'
        ],
        low: [
          'Information gathering activity detected',
          'Minor network anomaly within acceptable thresholds',
          'Routine security scan activity observed',
          'Low-confidence indicator observed in logs'
        ]
      }
      
      const severityDescs = descriptions[severity as keyof typeof descriptions] || descriptions.medium
      return severityDescs[Math.floor(Math.random() * severityDescs.length)]
    }

    const generateIoCs = (): string[] => {
      const iocTypes = [
        '192.168.1.100',
        'malicious-domain.com',
        'SHA256:a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3',
        'evil-actor@suspicious.net',
        '/tmp/malware.sh',
        'HKEY_LOCAL_MACHINE\\SOFTWARE\\Malware\\Config'
      ]
      
      const count = Math.floor(Math.random() * 3) + 1
      return iocTypes.sort(() => 0.5 - Math.random()).slice(0, count)
    }

    // Update stats
    const updateStats = () => {
      setStats(prevStats => ({
        totalThreats: prevStats.totalThreats + Math.floor(Math.random() * 5) + 1,
        criticalThreats: prevStats.criticalThreats + Math.floor(Math.random() * 2),
        blockedAttacks: prevStats.blockedAttacks + Math.floor(Math.random() * 10) + 5,
        activeInvestigations: Math.floor(Math.random() * 15) + 5
      }))
    }

    // Add new threats periodically
    const addThreat = () => {
      if (isLive) {
        const newThreat = generateThreatData()
        setThreats(prevThreats => [newThreat, ...prevThreats.slice(0, 19)]) // Keep only latest 20
        updateStats()
      }
    }

    // Initial data
    const initialThreats = Array.from({ length: 10 }, generateThreatData)
    setThreats(initialThreats)
    updateStats()

    // Set up real-time updates
    const interval = setInterval(addThreat, 3000 + Math.random() * 4000) // 3-7 seconds

    return () => clearInterval(interval)
  }, [isLive])

  const getSeverityColor = (severity: string) => {
    switch (severity) {
      case 'critical': return 'text-red-400 bg-red-500/20'
      case 'high': return 'text-orange-400 bg-orange-500/20'
      case 'medium': return 'text-yellow-400 bg-yellow-500/20'
      case 'low': return 'text-blue-400 bg-blue-500/20'
      default: return 'text-gray-400 bg-gray-500/20'
    }
  }

  return (
    <div className="min-h-screen bg-slate-900 text-white p-6">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <div>
            <h1 className="text-3xl font-bold text-primary-400 mb-2">
              🛡️ Real-Time Threat Intelligence
            </h1>
            <p className="text-gray-400">Live security threat monitoring and analysis</p>
          </div>
          <div className="flex items-center gap-4">
            <motion.div
              className={`flex items-center gap-2 px-4 py-2 rounded-full ${
                isLive ? 'bg-green-500/20 text-green-400' : 'bg-gray-500/20 text-gray-400'
              }`}
              animate={{ scale: isLive ? [1, 1.05, 1] : 1 }}
              transition={{ duration: 2, repeat: isLive ? Infinity : 0 }}
            >
              <Activity className="w-4 h-4" />
              {isLive ? 'Live Feed' : 'Paused'}
            </motion.div>
            <button
              onClick={() => setIsLive(!isLive)}
              className="px-4 py-2 bg-primary-600 hover:bg-primary-700 rounded-lg transition-colors"
            >
              {isLive ? 'Pause' : 'Resume'}
            </button>
          </div>
        </div>

        {/* Stats Dashboard */}
        <div className="grid grid-cols-1 md:grid-cols-4 gap-6 mb-8">
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            className="glass-card p-6"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Total Threats</p>
                <p className="text-2xl font-bold text-white">{stats.totalThreats.toLocaleString()}</p>
              </div>
              <Shield className="w-8 h-8 text-primary-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.1 }}
            className="glass-card p-6"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Critical Threats</p>
                <p className="text-2xl font-bold text-red-400">{stats.criticalThreats}</p>
              </div>
              <AlertTriangle className="w-8 h-8 text-red-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.2 }}
            className="glass-card p-6"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Blocked Attacks</p>
                <p className="text-2xl font-bold text-green-400">{stats.blockedAttacks.toLocaleString()}</p>
              </div>
              <Globe className="w-8 h-8 text-green-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.3 }}
            className="glass-card p-6"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Active Cases</p>
                <p className="text-2xl font-bold text-yellow-400">{stats.activeInvestigations}</p>
              </div>
              <Eye className="w-8 h-8 text-yellow-400" />
            </div>
          </motion.div>
        </div>

        {/* Threat Feed */}
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ delay: 0.4 }}
          className="glass-card p-6"
        >
          <div className="flex items-center justify-between mb-6">
            <h2 className="text-xl font-bold text-white">Live Threat Intelligence Feed</h2>
            <TrendingUp className="w-6 h-6 text-primary-400" />
          </div>

          <div className="space-y-4 max-h-96 overflow-y-auto">
            {threats.map((threat, index) => (
              <motion.div
                key={threat.id}
                initial={{ opacity: 0, x: -20 }}
                animate={{ opacity: 1, x: 0 }}
                transition={{ delay: index * 0.1 }}
                className="border border-slate-700 rounded-lg p-4 hover:border-primary-500/50 transition-colors"
              >
                <div className="flex items-start justify-between mb-3">
                  <div className="flex items-center gap-3">
                    <span className={`px-2 py-1 rounded text-xs font-medium ${getSeverityColor(threat.severity)}`}>
                      {threat.severity.toUpperCase()}
                    </span>
                    <span className="text-gray-300 font-medium">{threat.type}</span>
                  </div>
                  <div className="text-right">
                    <p className="text-xs text-gray-400">{threat.source}</p>
                    <p className="text-xs text-gray-500">
                      {threat.timestamp.toLocaleTimeString()}
                    </p>
                  </div>
                </div>

                <p className="text-gray-300 mb-3">{threat.description}</p>

                <div className="flex items-center justify-between">
                  <div className="flex flex-wrap gap-2">
                    {threat.ioCs.slice(0, 2).map((ioc, iocIndex) => (
                      <code key={iocIndex} className="text-xs bg-slate-800 px-2 py-1 rounded text-primary-400">
                        {ioc.length > 30 ? `${ioc.substring(0, 30)}...` : ioc}
                      </code>
                    ))}
                    {threat.ioCs.length > 2 && (
                      <span className="text-xs text-gray-500">
                        +{threat.ioCs.length - 2} more
                      </span>
                    )}
                  </div>
                  <div className="text-xs text-gray-400">
                    Confidence: {threat.confidence}%
                  </div>
                </div>
              </motion.div>
            ))}
          </div>
        </motion.div>
      </div>
    </div>
  )
}

export default ThreatIntelligenceDashboard