import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Shield, AlertTriangle, Globe, Activity, TrendingUp, Eye, Plus, Search } from 'lucide-react'
import { Toaster, toast } from 'react-hot-toast'

interface ThreatIndicator {
  id: number
  ioc_type: string
  ioc_value: string
  threat_level: 'critical' | 'high' | 'medium' | 'low'
  source: string
  description: string
  last_seen: string
  active: boolean
}

interface ThreatStats {
  total_indicators: number
  critical_threats: number
  active_indicators: number
  high_threats: number
}

export default function ThreatIntelligenceDashboard() {
  const [threats, setThreats] = useState<ThreatIndicator[]>([])
  const [stats, setStats] = useState<ThreatStats | null>(null)
  const [isLive, setIsLive] = useState(true)
  const [showAddForm, setShowAddForm] = useState(false)

  // Form State
  const [newIOC, setNewIOC] = useState({
    iocType: 'ip',
    iocValue: '',
    threatLevel: 'medium',
    source: 'Manual Entry',
    description: '',
    tags: []
  })

  const fetchData = async () => {
    try {
      const [statsRes, threatsRes] = await Promise.all([
        fetch('/api/threat-intel/stats'),
        fetch('/api/threat-intel/recent?limit=20')
      ])

      const statsData = await statsRes.json()
      const threatsData = await threatsRes.json()

      setStats(statsData)
      if (threatsData.threats) {
        setThreats(threatsData.threats)
      }
    } catch (err) {
      console.error('Failed to fetch threat intel', err)
    }
  }

  useEffect(() => {
    fetchData()
    if (!isLive) return

    const interval = setInterval(fetchData, 5000)
    return () => clearInterval(interval)
  }, [isLive])

  const handleSubmitIOC = async (e: React.FormEvent) => {
    e.preventDefault()
    try {
      const res = await fetch('/api/threat-intel/indicators', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(newIOC)
      })

      if (res.ok) {
        toast.success('IOC added successfully')
        setShowAddForm(false)
        fetchData()
        setNewIOC({ ...newIOC, iocValue: '', description: '' })
      } else {
        toast.error('Failed to add IOC')
      }
    } catch (err) {
      toast.error('Error submitting IOC')
    }
  }

  const getSeverityColor = (severity: string) => {
    switch (severity) {
      case 'critical': return 'text-red-400 bg-red-500/10 border-red-500/20'
      case 'high': return 'text-orange-400 bg-orange-500/10 border-orange-500/20'
      case 'medium': return 'text-yellow-400 bg-yellow-500/10 border-yellow-500/20'
      case 'low': return 'text-blue-400 bg-blue-500/10 border-blue-500/20'
      default: return 'text-gray-400 bg-gray-500/10 border-gray-500/20'
    }
  }

  return (
    <div className="min-h-screen bg-slate-900 text-white p-6">
      <div className="max-w-7xl mx-auto">
        <Toaster position="top-right" />

        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <div>
            <h1 className="text-3xl font-bold text-primary-400 mb-2 flex items-center gap-3">
              <Shield className="w-8 h-8" />
              Threat Intelligence
            </h1>
            <p className="text-gray-400">Live security threat monitoring and IOC management</p>
          </div>
          <div className="flex items-center gap-4">
            <button
              onClick={() => setShowAddForm(!showAddForm)}
              className="flex items-center gap-2 px-4 py-2 bg-purple-600 hover:bg-purple-700 rounded-lg transition-colors border border-purple-500"
            >
              <Plus className="w-4 h-4" />
              Add IOC
            </button>
            <motion.div
              className={`flex items-center gap-2 px-4 py-2 rounded-full ${isLive ? 'bg-green-500/20 text-green-400' : 'bg-gray-500/20 text-gray-400'
                }`}
              animate={{ scale: isLive ? [1, 1.05, 1] : 1 }}
              transition={{ duration: 2, repeat: isLive ? Infinity : 0 }}
            >
              <Activity className="w-4 h-4" />
              {isLive ? 'Live Feed' : 'Paused'}
            </motion.div>
          </div>
        </div>

        {/* Add IOC Form */}
        {showAddForm && (
          <motion.div
            initial={{ opacity: 0, height: 0 }}
            animate={{ opacity: 1, height: 'auto' }}
            className="mb-8 bg-slate-900/50 border border-purple-500/30 rounded-lg p-6"
          >
            <h3 className="text-lg font-bold text-white mb-4">Submit New Indicator of Compromise</h3>
            <form onSubmit={handleSubmitIOC} className="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div>
                <label className="block text-sm text-slate-400 mb-1">IOC Value</label>
                <input
                  required
                  className="w-full bg-slate-800 border border-slate-700 rounded p-2 text-white"
                  placeholder="e.g. 1.2.3.4 or malicious.com"
                  value={newIOC.iocValue}
                  onChange={e => setNewIOC({ ...newIOC, iocValue: e.target.value })}
                />
              </div>
              <div>
                <label className="block text-sm text-slate-400 mb-1">Type</label>
                <select
                  className="w-full bg-slate-800 border border-slate-700 rounded p-2 text-white"
                  value={newIOC.iocType}
                  onChange={e => setNewIOC({ ...newIOC, iocType: e.target.value })}
                >
                  <option value="ip">IP Address</option>
                  <option value="domain">Domain</option>
                  <option value="hash">File Hash</option>
                  <option value="email">Email</option>
                </select>
              </div>
              <div>
                <label className="block text-sm text-slate-400 mb-1">Threat Level</label>
                <select
                  className="w-full bg-slate-800 border border-slate-700 rounded p-2 text-white"
                  value={newIOC.threatLevel}
                  onChange={e => setNewIOC({ ...newIOC, threatLevel: e.target.value })}
                >
                  <option value="low">Low</option>
                  <option value="medium">Medium</option>
                  <option value="high">High</option>
                  <option value="critical">Critical</option>
                </select>
              </div>
              <div>
                <label className="block text-sm text-slate-400 mb-1">Description</label>
                <input
                  className="w-full bg-slate-800 border border-slate-700 rounded p-2 text-white"
                  placeholder="Context about this threat..."
                  value={newIOC.description}
                  onChange={e => setNewIOC({ ...newIOC, description: e.target.value })}
                />
              </div>
              <div className="md:col-span-2">
                <button type="submit" className="bg-purple-600 hover:bg-purple-700 text-white px-6 py-2 rounded">Submit Indicator</button>
              </div>
            </form>
          </motion.div>
        )}

        {/* Stats Dashboard */}
        <div className="grid grid-cols-1 md:grid-cols-4 gap-6 mb-8">
          <div className="bg-slate-900/50 border border-cyan-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Total Indicators</p>
                <p className="text-2xl font-bold text-white">{stats?.total_indicators || 0}</p>
              </div>
              <Shield className="w-8 h-8 text-cyan-400" />
            </div>
          </div>

          <div className="bg-slate-900/50 border border-red-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Critical Threats</p>
                <p className="text-2xl font-bold text-red-400">{stats?.critical_threats || 0}</p>
              </div>
              <AlertTriangle className="w-8 h-8 text-red-400" />
            </div>
          </div>

          <div className="bg-slate-900/50 border border-green-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">Active</p>
                <p className="text-2xl font-bold text-green-400">{stats?.active_indicators || 0}</p>
              </div>
              <Globe className="w-8 h-8 text-green-400" />
            </div>
          </div>

          <div className="bg-slate-900/50 border border-orange-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-gray-400 text-sm">High Risk</p>
                <p className="text-2xl font-bold text-orange-400">{stats?.high_threats || 0}</p>
              </div>
              <Eye className="w-8 h-8 text-orange-400" />
            </div>
          </div>
        </div>

        {/* Threat Feed */}
        <div className="bg-slate-900/50 border border-slate-700/50 rounded-lg p-6">
          <div className="flex items-center justify-between mb-6">
            <h2 className="text-xl font-bold text-white flex items-center gap-2">
              <TrendingUp className="w-5 h-5 text-cyan-400" />
              Live Threat Intelligence Feed
            </h2>
            <div className="text-sm text-slate-400">Updating live...</div>
          </div>

          <div className="space-y-4 max-h-[600px] overflow-y-auto custom-scrollbar">
            {threats.length === 0 && (
              <div className="text-center py-10 text-slate-500">No active threats detected.</div>
            )}
            {threats.map((threat) => (
              <motion.div
                key={threat.id}
                initial={{ opacity: 0, x: -20 }}
                animate={{ opacity: 1, x: 0 }}
                className="border border-slate-700/50 bg-slate-800/20 rounded-lg p-4 hover:border-cyan-500/30 transition-colors"
              >
                <div className="flex items-start justify-between mb-2">
                  <div className="flex items-center gap-3">
                    <span className={`px-2 py-0.5 rounded text-xs font-bold border ${getSeverityColor(threat.threat_level)}`}>
                      {threat.threat_level?.toUpperCase()}
                    </span>
                    <span className="text-slate-300 font-medium font-mono">{threat.ioc_value}</span>
                    <span className="text-xs bg-slate-700 px-2 py-0.5 rounded text-slate-400">{threat.ioc_type}</span>
                  </div>
                  <div className="text-right">
                    <p className="text-xs text-slate-500">
                      {new Date(threat.last_seen).toLocaleString()}
                    </p>
                  </div>
                </div>

                <p className="text-slate-400 text-sm mb-2">{threat.description}</p>

                <div className="flex items-center justify-between text-xs text-slate-500">
                  <span>Source: {threat.source}</span>
                  {threat.active && <span className="text-green-400 flex items-center gap-1"><div className="w-1.5 h-1.5 rounded-full bg-green-500"></div> Active</span>}
                </div>
              </motion.div>
            ))}
          </div>
        </div>
      </div>
    </div>
  )
}