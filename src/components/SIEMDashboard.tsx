import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Activity, Shield, AlertTriangle, TrendingUp, Eye, Users, Server, Zap, Clock, Target } from 'lucide-react'

interface SecurityEvent {
  id: string
  timestamp: Date
  severity: 'critical' | 'high' | 'medium' | 'low' | 'info'
  category: string
  source: string
  description: string
  user?: string
  host?: string
  action: 'allowed' | 'blocked' | 'quarantined' | 'investigating'
  details: string[]
}

interface SIEMMetrics {
  totalEvents: number
  criticalAlerts: number
  resolvedIncidents: number
  activeThreats: number
  systemsMonitored: number
  complianceScore: number
  responseTime: number
  threatsBlocked: number
}

const SIEMDashboard: React.FC = () => {
  const [events, setEvents] = useState<SecurityEvent[]>([])
  const [metrics, setMetrics] = useState<SIEMMetrics>({
    totalEvents: 0,
    criticalAlerts: 0,
    resolvedIncidents: 0,
    activeThreats: 0,
    systemsMonitored: 0,
    complianceScore: 0,
    responseTime: 0,
    threatsBlocked: 0
  })
  const [isActive, setIsActive] = useState(true)

  useEffect(() => {
    const generateSecurityEvent = (): SecurityEvent => {
      const categories = [
        'Authentication',
        'Network Access',
        'File System',
        'Process Execution',
        'Data Transfer',
        'Privilege Escalation',
        'Malware Detection',
        'Vulnerability Scan',
        'Policy Violation',
        'Compliance Check'
      ]
      
      const sources = [
        'Windows Security Log',
        'Linux Audit Log', 
        'Network Firewall',
        'Endpoint Protection',
        'Web Application Firewall',
        'Identity Management',
        'Database Security',
        'Email Security Gateway',
        'Cloud Security Posture',
        'Vulnerability Scanner'
      ]

      const severities: Array<'critical' | 'high' | 'medium' | 'low' | 'info'> = 
        ['critical', 'high', 'medium', 'low', 'info']
      
      const actions: Array<'allowed' | 'blocked' | 'quarantined' | 'investigating'> = 
        ['allowed', 'blocked', 'quarantined', 'investigating']

      const hosts = [
        'WS-FINANCE-01', 'SRV-DC-01', 'WEB-PROD-02', 'DB-MAIN-01', 
        'WS-HR-05', 'SRV-FILE-01', 'WS-DEV-03', 'SRV-MAIL-01'
      ]

      const users = [
        'john.smith', 'admin', 'sarah.jones', 'mike.wilson', 'SYSTEM', 
        'svc_backup', 'test.user', 'root', 'apache', 'sql_service'
      ]

      const severity = severities[Math.floor(Math.random() * severities.length)]
      const category = categories[Math.floor(Math.random() * categories.length)]
      const source = sources[Math.floor(Math.random() * sources.length)]
      
      const generateDescription = (sev: string, cat: string): string => {
        const descriptions = {
          'Authentication': [
            'Multiple failed login attempts detected',
            'Successful login from unusual location',
            'Account locked due to policy violation',
            'Privileged account access granted'
          ],
          'Network Access': [
            'Suspicious outbound connection blocked',
            'Unauthorized port scan detected',
            'VPN connection from high-risk country',
            'Unusual network traffic pattern identified'
          ],
          'File System': [
            'Sensitive file access attempt',
            'Mass file deletion detected',
            'Unauthorized file modification',
            'Encryption activity on shared folder'
          ],
          'Malware Detection': [
            'Malicious file quarantined',
            'Suspicious process behavior detected',
            'Command and control communication blocked',
            'Potential ransomware activity identified'
          ]
        }
        
        const categoryDescs = descriptions[cat as keyof typeof descriptions] || [
          'Security event requiring investigation',
          'Policy compliance check performed',
          'System activity monitored',
          'Security control executed'
        ]
        
        return categoryDescs[Math.floor(Math.random() * categoryDescs.length)]
      }

      return {
        id: `event-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
        timestamp: new Date(),
        severity,
        category,
        source,
        description: generateDescription(severity, category),
        user: Math.random() > 0.3 ? users[Math.floor(Math.random() * users.length)] : undefined,
        host: hosts[Math.floor(Math.random() * hosts.length)],
        action: severity === 'critical' ? 'blocked' : actions[Math.floor(Math.random() * actions.length)],
        details: [
          `Event ID: ${Math.floor(Math.random() * 10000) + 1000}`,
          `Process: ${['svchost.exe', 'chrome.exe', 'powershell.exe', 'cmd.exe', 'explorer.exe'][Math.floor(Math.random() * 5)]}`,
          `IP: ${Math.floor(Math.random() * 255)}.${Math.floor(Math.random() * 255)}.${Math.floor(Math.random() * 255)}.${Math.floor(Math.random() * 255)}`
        ]
      }
    }

    const updateMetrics = (newEvent: SecurityEvent) => {
      setMetrics(prevMetrics => ({
        totalEvents: prevMetrics.totalEvents + 1,
        criticalAlerts: newEvent.severity === 'critical' 
          ? prevMetrics.criticalAlerts + 1 
          : prevMetrics.criticalAlerts,
        resolvedIncidents: newEvent.action === 'blocked' 
          ? prevMetrics.resolvedIncidents + 1 
          : prevMetrics.resolvedIncidents,
        activeThreats: Math.max(0, Math.floor(Math.random() * 15) + 2),
        systemsMonitored: 247 + Math.floor(Math.random() * 10),
        complianceScore: Math.min(100, 87 + Math.floor(Math.random() * 10)),
        responseTime: Math.floor(Math.random() * 300) + 50, // milliseconds
        threatsBlocked: newEvent.action === 'blocked' 
          ? prevMetrics.threatsBlocked + 1 
          : prevMetrics.threatsBlocked
      }))
    }

    const addEvent = () => {
      if (isActive) {
        const newEvent = generateSecurityEvent()
        setEvents(prevEvents => [newEvent, ...prevEvents.slice(0, 99)]) // Keep latest 100
        updateMetrics(newEvent)
      }
    }

    // Initialize with some events
    const initialEvents = Array.from({ length: 20 }, generateSecurityEvent)
    setEvents(initialEvents)

    // Set up real-time updates
    const interval = setInterval(addEvent, 2000 + Math.random() * 3000) // 2-5 seconds

    return () => clearInterval(interval)
  }, [isActive])

  const getSeverityColor = (severity: string) => {
    switch (severity) {
      case 'critical': return 'text-red-400 bg-red-500/20 border-red-500/50'
      case 'high': return 'text-orange-400 bg-orange-500/20 border-orange-500/50'
      case 'medium': return 'text-yellow-400 bg-yellow-500/20 border-yellow-500/50'
      case 'low': return 'text-blue-400 bg-blue-500/20 border-blue-500/50'
      case 'info': return 'text-green-400 bg-green-500/20 border-green-500/50'
      default: return 'text-gray-400 bg-gray-500/20 border-gray-500/50'
    }
  }

  const getActionColor = (action: string) => {
    switch (action) {
      case 'blocked': return 'text-red-400 bg-red-500/20'
      case 'quarantined': return 'text-orange-400 bg-orange-500/20'
      case 'investigating': return 'text-yellow-400 bg-yellow-500/20'
      case 'allowed': return 'text-green-400 bg-green-500/20'
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
              🛡️ Security Information & Event Management
            </h1>
            <p className="text-gray-400">Comprehensive security monitoring and incident response</p>
          </div>
          <div className="flex items-center gap-4">
            <motion.div
              className={`flex items-center gap-2 px-4 py-2 rounded-full ${
                isActive ? 'bg-green-500/20 text-green-400' : 'bg-gray-500/20 text-gray-400'
              }`}
              animate={{ scale: isActive ? [1, 1.05, 1] : 1 }}
              transition={{ duration: 2, repeat: isActive ? Infinity : 0 }}
            >
              <Activity className="w-4 h-4" />
              {isActive ? 'SIEM Active' : 'Monitoring Paused'}
            </motion.div>
            <button
              onClick={() => setIsActive(!isActive)}
              className="px-4 py-2 bg-primary-600 hover:bg-primary-700 rounded-lg transition-colors"
            >
              {isActive ? 'Pause' : 'Resume'}
            </button>
          </div>
        </div>

        {/* Metrics Grid */}
        <div className="grid grid-cols-2 md:grid-cols-4 lg:grid-cols-8 gap-4 mb-8">
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-xs">Events</p>
                <p className="text-lg font-bold text-white">{metrics.totalEvents.toLocaleString()}</p>
              </div>
              <Activity className="w-5 h-5 text-primary-400" />
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
                <p className="text-gray-400 text-xs">Critical</p>
                <p className="text-lg font-bold text-red-400">{metrics.criticalAlerts}</p>
              </div>
              <AlertTriangle className="w-5 h-5 text-red-400" />
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
                <p className="text-gray-400 text-xs">Resolved</p>
                <p className="text-lg font-bold text-green-400">{metrics.resolvedIncidents}</p>
              </div>
              <Shield className="w-5 h-5 text-green-400" />
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
                <p className="text-gray-400 text-xs">Threats</p>
                <p className="text-lg font-bold text-orange-400">{metrics.activeThreats}</p>
              </div>
              <Target className="w-5 h-5 text-orange-400" />
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
                <p className="text-gray-400 text-xs">Systems</p>
                <p className="text-lg font-bold text-blue-400">{metrics.systemsMonitored}</p>
              </div>
              <Server className="w-5 h-5 text-blue-400" />
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
                <p className="text-gray-400 text-xs">Compliance</p>
                <p className="text-lg font-bold text-primary-400">{metrics.complianceScore}%</p>
              </div>
              <TrendingUp className="w-5 h-5 text-primary-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.6 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-xs">Response</p>
                <p className="text-lg font-bold text-yellow-400">{metrics.responseTime}ms</p>
              </div>
              <Clock className="w-5 h-5 text-yellow-400" />
            </div>
          </motion.div>

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.7 }}
            className="glass-card p-4"
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-xs">Blocked</p>
                <p className="text-lg font-bold text-red-400">{metrics.threatsBlocked}</p>
              </div>
              <Zap className="w-5 h-5 text-red-400" />
            </div>
          </motion.div>
        </div>

        {/* Security Events Feed */}
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ delay: 0.8 }}
          className="glass-card p-6"
        >
          <div className="flex items-center justify-between mb-6">
            <h2 className="text-xl font-bold text-white">Live Security Event Feed</h2>
            <Eye className="w-6 h-6 text-primary-400" />
          </div>

          <div className="space-y-3 max-h-96 overflow-y-auto">
            {events.map((event, index) => (
              <motion.div
                key={event.id}
                initial={{ opacity: 0, x: -20 }}
                animate={{ opacity: 1, x: 0 }}
                transition={{ delay: index * 0.05 }}
                className={`border border-slate-700 rounded-lg p-4 hover:border-primary-500/50 transition-colors ${getSeverityColor(event.severity)}`}
              >
                <div className="flex items-start justify-between mb-2">
                  <div className="flex items-center gap-3">
                    <span className={`px-2 py-1 rounded text-xs font-medium ${getSeverityColor(event.severity)}`}>
                      {event.severity.toUpperCase()}
                    </span>
                    <span className="text-gray-300 font-medium">{event.category}</span>
                  </div>
                  <div className="text-right">
                    <p className="text-xs text-gray-400">{event.source}</p>
                    <p className="text-xs text-gray-500">
                      {event.timestamp.toLocaleTimeString()}
                    </p>
                  </div>
                </div>

                <p className="text-gray-300 mb-3">{event.description}</p>

                <div className="grid grid-cols-2 md:grid-cols-4 gap-4 text-xs">
                  {event.user && (
                    <div>
                      <span className="text-gray-500">User:</span>
                      <code className="ml-2 bg-slate-800 px-2 py-1 rounded text-primary-400">
                        {event.user}
                      </code>
                    </div>
                  )}
                  <div>
                    <span className="text-gray-500">Host:</span>
                    <code className="ml-2 bg-slate-800 px-2 py-1 rounded text-primary-400">
                      {event.host}
                    </code>
                  </div>
                  <div>
                    <span className="text-gray-500">Action:</span>
                    <span className={`ml-2 px-2 py-1 rounded text-xs font-medium ${getActionColor(event.action)}`}>
                      {event.action}
                    </span>
                  </div>
                  <div>
                    <span className="text-gray-500">Details:</span>
                    <span className="ml-2 text-gray-400">{event.details.length} items</span>
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

export default SIEMDashboard