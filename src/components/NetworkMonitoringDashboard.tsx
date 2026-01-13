import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Activity, Wifi, Shield, AlertCircle, Server, Network, Zap, Search, Play, Globe } from 'lucide-react'
import { Toaster, toast } from 'react-hot-toast'
import { CartesianGrid, Line, LineChart, ResponsiveContainer, Tooltip, XAxis, YAxis } from 'recharts'

interface NetworkStats {
  timestamp: string
  bandwidth: {
    upload: number
    download: number
  }
  system: {
    cpuUsage: number
    memoryUsage: number
    uptime: number
  }
  activeConnections: number
}

interface PortScanResult {
  port: number
  state: string
  service: string
}

export default function NetworkMonitoringDashboard() {
  const [metrics, setMetrics] = useState<NetworkStats | null>(null)
  const [metricHistory, setMetricHistory] = useState<any[]>([])
  const [isMonitoring, setIsMonitoring] = useState(true)

  // Port Scanner State
  const [targetIP, setTargetIP] = useState('127.0.0.1')
  const [scanPorts, setScanPorts] = useState('21,22,80,443,3306,5432,8080')
  const [isScanning, setIsScanning] = useState(false)
  const [scanResults, setScanResults] = useState<PortScanResult[]>([])

  useEffect(() => {
    if (!isMonitoring) return

    const fetchMetrics = async () => {
      try {
        const res = await fetch('/api/network/metrics')
        const data = await res.json()
        setMetrics(data)

        setMetricHistory(prev => {
          const newItem = {
            time: new Date().toLocaleTimeString(),
            upload: data.bandwidth.upload,
            download: data.bandwidth.download,
            cpu: data.system.cpuUsage
          }
          const newHistory = [...prev, newItem]
          return newHistory.slice(-20) // Keep last 20 data points
        })
      } catch (err) {
        console.error('Failed to fetch metrics', err)
      }
    }

    fetchMetrics()
    const interval = setInterval(fetchMetrics, 2000)
    return () => clearInterval(interval)
  }, [isMonitoring])

  const handleScan = async (e: React.FormEvent) => {
    e.preventDefault()
    setIsScanning(true)
    setScanResults([])

    try {
      const res = await fetch('/api/network/port-scan', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ target: targetIP, ports: scanPorts })
      })
      const data = await res.json()

      if (data.openPorts) {
        setScanResults(data.openPorts)
        toast.success(`Scan complete. Found ${data.openPorts.length} open ports.`)
      } else {
        toast.error('Scan failed or no ports found.')
      }
    } catch (err) {
      console.error(err)
      toast.error('Failed to initiate scan')
    } finally {
      setIsScanning(false)
    }
  }

  return (
    <div className="min-h-screen bg-slate-900 text-white p-6">
      <div className="max-w-7xl mx-auto">
        <Toaster position="top-right" />

        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <div>
            <h1 className="text-3xl font-bold text-cyan-400 mb-2 flex items-center gap-3">
              <Network className="w-8 h-8" />
              Network Operations Center
            </h1>
            <p className="text-slate-400">Real-time infrastructure monitoring and active reconnaissance</p>
          </div>
          <div className="flex items-center gap-4">
            <div className="flex items-center gap-2 text-sm text-slate-400">
              <span className={`w-3 h-3 rounded-full ${isMonitoring ? 'bg-green-500 animate-pulse' : 'bg-red-500'}`}></span>
              {isMonitoring ? 'Live Feed Active' : 'Feed Paused'}
            </div>
            <button
              onClick={() => setIsMonitoring(!isMonitoring)}
              className="px-4 py-2 bg-slate-800 hover:bg-slate-700 rounded-lg transition-colors text-sm border border-slate-700"
            >
              {isMonitoring ? 'Pause Monitor' : 'Resume Monitor'}
            </button>
          </div>
        </div>

        {/* Real-Time Metrics Cards */}
        <div className="grid grid-cols-1 md:grid-cols-4 gap-6 mb-8">
          <div className="bg-slate-900/50 border border-cyan-500/20 rounded-lg p-6 backdrop-blur">
            <div className="flex items-center justify-between mb-2">
              <span className="text-slate-400 text-sm">CPU Usage</span>
              <Activity className="w-5 h-5 text-cyan-400" />
            </div>
            <div className="text-3xl font-bold text-white">{metrics?.system.cpuUsage}%</div>
            <div className="w-full bg-slate-800 h-1.5 mt-3 rounded-full overflow-hidden">
              <div
                className="bg-cyan-400 h-full transition-all duration-500"
                style={{ width: `${metrics?.system.cpuUsage}%` }}
              />
            </div>
          </div>

          <div className="bg-slate-900/50 border border-purple-500/20 rounded-lg p-6 backdrop-blur">
            <div className="flex items-center justify-between mb-2">
              <span className="text-slate-400 text-sm">Memory Usage</span>
              <Server className="w-5 h-5 text-purple-400" />
            </div>
            <div className="text-3xl font-bold text-white">{metrics?.system.memoryUsage}%</div>
            <div className="w-full bg-slate-800 h-1.5 mt-3 rounded-full overflow-hidden">
              <div
                className="bg-purple-400 h-full transition-all duration-500"
                style={{ width: `${metrics?.system.memoryUsage}%` }}
              />
            </div>
          </div>

          <div className="bg-slate-900/50 border border-green-500/20 rounded-lg p-6 backdrop-blur">
            <div className="flex items-center justify-between mb-2">
              <span className="text-slate-400 text-sm">Network I/O</span>
              <Wifi className="w-5 h-5 text-green-400" />
            </div>
            <div className="flex justify-between items-end">
              <div>
                <div className="text-xs text-slate-500">UL</div>
                <div className="text-xl font-bold text-white">{metrics?.bandwidth.upload.toFixed(1)} <span className="text-xs font-normal text-slate-400">Mbps</span></div>
              </div>
              <div className="text-right">
                <div className="text-xs text-slate-500">DL</div>
                <div className="text-xl font-bold text-white">{metrics?.bandwidth.download.toFixed(1)} <span className="text-xs font-normal text-slate-400">Mbps</span></div>
              </div>
            </div>
          </div>

          <div className="bg-slate-900/50 border border-orange-500/20 rounded-lg p-6 backdrop-blur">
            <div className="flex items-center justify-between mb-2">
              <span className="text-slate-400 text-sm">Active Connections</span>
              <Globe className="w-5 h-5 text-orange-400" />
            </div>
            <div className="text-3xl font-bold text-white">{metrics?.activeConnections}</div>
            <div className="text-xs text-slate-500 mt-2">Uptime: {Math.floor((metrics?.system.uptime || 0) / 3600)}h {Math.floor(((metrics?.system.uptime || 0) % 3600) / 60)}m</div>
          </div>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
          {/* Traffic Chart */}
          <div className="lg:col-span-2 bg-slate-900/50 border border-slate-800 rounded-lg p-6">
            <h2 className="text-xl font-bold text-white mb-6 flex items-center gap-2">
              <Activity className="w-5 h-5 text-cyan-400" />
              Network Traffic History
            </h2>
            <div className="h-[300px] w-full">
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={metricHistory}>
                  <CartesianGrid strokeDasharray="3 3" stroke="#334155" />
                  <XAxis dataKey="time" stroke="#94a3b8" fontSize={12} />
                  <YAxis stroke="#94a3b8" fontSize={12} />
                  <Tooltip
                    contentStyle={{ backgroundColor: '#0f172a', borderColor: '#1e293b' }}
                    itemStyle={{ color: '#e2e8f0' }}
                  />
                  <Line type="monotone" dataKey="upload" stroke="#22d3ee" strokeWidth={2} dot={false} name="Upload (Mbps)" />
                  <Line type="monotone" dataKey="download" stroke="#4ade80" strokeWidth={2} dot={false} name="Download (Mbps)" />
                  <Line type="monotone" dataKey="cpu" stroke="#c084fc" strokeWidth={2} dot={false} name="CPU (%)" />
                </LineChart>
              </ResponsiveContainer>
            </div>
          </div>

          {/* Active Port Scanner */}
          <div className="bg-slate-900/50 border border-slate-800 rounded-lg p-6">
            <h2 className="text-xl font-bold text-white mb-6 flex items-center gap-2">
              <Search className="w-5 h-5 text-purple-400" />
              Active Port Scanner
            </h2>

            <form onSubmit={handleScan} className="space-y-4 mb-6">
              <div>
                <label className="block text-sm text-slate-400 mb-1">Target IP / Hostname</label>
                <input
                  type="text"
                  value={targetIP}
                  onChange={e => setTargetIP(e.target.value)}
                  className="w-full bg-slate-800 border border-slate-700 rounded p-2 text-white focus:border-purple-500 focus:outline-none"
                  placeholder="e.g. 192.168.1.1"
                />
              </div>
              <div>
                <label className="block text-sm text-slate-400 mb-1">Ports (comma separated or range)</label>
                <input
                  type="text"
                  value={scanPorts}
                  onChange={e => setScanPorts(e.target.value)}
                  className="w-full bg-slate-800 border border-slate-700 rounded p-2 text-white focus:border-purple-500 focus:outline-none"
                  placeholder="80,443,8000-8080"
                />
              </div>
              <button
                type="submit"
                disabled={isScanning}
                className="w-full bg-purple-600 hover:bg-purple-700 text-white font-bold py-2 px-4 rounded flex items-center justify-center gap-2 transition-colors disabled:opacity-50 disabled:cursor-not-allowed"
              >
                {isScanning ? <div className="w-4 h-4 border-2 border-white border-t-transparent rounded-full animate-spin"></div> : <Play className="w-4 h-4" />}
                {isScanning ? 'Scanning...' : 'Start Scan'}
              </button>
            </form>

            {/* Scan Results */}
            <div className="border-t border-slate-800 pt-4">
              <h3 className="text-sm font-semibold text-slate-300 mb-3">Recent Scan Results</h3>
              <div className="space-y-2 max-h-[300px] overflow-y-auto custom-scrollbar">
                {scanResults.length === 0 && !isScanning && (
                  <div className="text-center text-slate-500 py-8 text-sm">
                    No open ports found or scan not started.
                  </div>
                )}
                {scanResults.map((result, i) => (
                  <div key={i} className="flex items-center justify-between p-2 bg-slate-800/50 rounded border border-slate-700/50">
                    <div className="flex items-center gap-2">
                      <div className="w-2 h-2 rounded-full bg-green-500"></div>
                      <span className="font-mono text-cyan-400">{result.port}</span>
                    </div>
                    <span className="text-xs px-2 py-0.5 rounded bg-slate-700 text-slate-300">{result.service}</span>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}