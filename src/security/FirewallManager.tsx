import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Shield, Plus, Trash2, Edit, Power, AlertTriangle, CheckCircle, X } from 'lucide-react'

interface FirewallRule {
  id: string
  name: string
  action: 'allow' | 'block' | 'monitor'
  protocol: 'tcp' | 'udp' | 'icmp' | 'any'
  sourceIP: string
  destIP: string
  sourcePort: string
  destPort: string
  enabled: boolean
  priority: number
  description: string
}

interface FirewallStats {
  totalRules: number
  activeRules: number
  blockedConnections: number
  allowedConnections: number
  threatsBlocked: number
}

const FirewallManager = () => {
  const [rules, setRules] = useState<FirewallRule[]>([])
  const [stats, setStats] = useState<FirewallStats>({
    totalRules: 0,
    activeRules: 0,
    blockedConnections: 0,
    allowedConnections: 0,
    threatsBlocked: 0
  })
  const [showAddRule, setShowAddRule] = useState(false)
  const [editingRule, setEditingRule] = useState<FirewallRule | null>(null)
  const [firewallEnabled, setFirewallEnabled] = useState(true)

  useEffect(() => {
    // Load default firewall rules
    const defaultRules: FirewallRule[] = [
      {
        id: '1',
        name: 'Allow HTTP/HTTPS',
        action: 'allow',
        protocol: 'tcp',
        sourceIP: 'any',
        destIP: 'any',
        sourcePort: 'any',
        destPort: '80,443',
        enabled: true,
        priority: 1,
        description: 'Allow standard web traffic'
      },
      {
        id: '2',
        name: 'Block Malicious IPs',
        action: 'block',
        protocol: 'any',
        sourceIP: '192.168.100.0/24',
        destIP: 'any',
        sourcePort: 'any',
        destPort: 'any',
        enabled: true,
        priority: 2,
        description: 'Block known malicious IP range'
      },
      {
        id: '3',
        name: 'Allow SSH Admin',
        action: 'allow',
        protocol: 'tcp',
        sourceIP: '192.168.1.0/24',
        destIP: 'any',
        sourcePort: 'any',
        destPort: '22',
        enabled: true,
        priority: 3,
        description: 'Allow SSH from admin network'
      },
      {
        id: '4',
        name: 'Monitor Suspicious Ports',
        action: 'monitor',
        protocol: 'tcp',
        sourceIP: 'any',
        destIP: 'any',
        sourcePort: 'any',
        destPort: '1337,4444,5555',
        enabled: true,
        priority: 4,
        description: 'Monitor commonly used hacker ports'
      }
    ]
    
    setRules(defaultRules)
    updateStats(defaultRules)
  }, [])

  const updateStats = (currentRules: FirewallRule[]) => {
    setStats({
      totalRules: currentRules.length,
      activeRules: currentRules.filter(r => r.enabled).length,
      blockedConnections: Math.floor(Math.random() * 1000) + 500,
      allowedConnections: Math.floor(Math.random() * 5000) + 2000,
      threatsBlocked: Math.floor(Math.random() * 50) + 25
    })
  }

  const addRule = (rule: Omit<FirewallRule, 'id'>) => {
    const newRule: FirewallRule = {
      ...rule,
      id: Date.now().toString()
    }
    const updatedRules = [...rules, newRule]
    setRules(updatedRules)
    updateStats(updatedRules)
    setShowAddRule(false)
  }

  const updateRule = (updatedRule: FirewallRule) => {
    const updatedRules = rules.map(r => r.id === updatedRule.id ? updatedRule : r)
    setRules(updatedRules)
    updateStats(updatedRules)
    setEditingRule(null)
  }

  const deleteRule = (id: string) => {
    const updatedRules = rules.filter(r => r.id !== id)
    setRules(updatedRules)
    updateStats(updatedRules)
  }

  const toggleRule = (id: string) => {
    const updatedRules = rules.map(r => 
      r.id === id ? { ...r, enabled: !r.enabled } : r
    )
    setRules(updatedRules)
    updateStats(updatedRules)
  }

  return (
    <div className="p-6 space-y-6">
      {/* Header */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold text-white mb-2">
            🛡️ Advanced Firewall Protection
          </h1>
          <p className="text-slate-400">
            Comprehensive network security with real-time threat protection
          </p>
        </div>
        
        <div className="flex items-center space-x-4">
          <div className={`flex items-center space-x-2 px-4 py-2 rounded-lg ${
            firewallEnabled ? 'bg-green-500/20 text-green-400' : 'bg-red-500/20 text-red-400'
          }`}>
            <Power className="h-5 w-5" />
            <span className="font-semibold">
              {firewallEnabled ? 'Firewall Active' : 'Firewall Disabled'}
            </span>
          </div>
          
          <button
            onClick={() => setFirewallEnabled(!firewallEnabled)}
            className={`px-4 py-2 rounded-lg font-semibold transition-colors ${
              firewallEnabled 
                ? 'bg-red-500/20 text-red-400 hover:bg-red-500/30' 
                : 'bg-green-500/20 text-green-400 hover:bg-green-500/30'
            }`}
          >
            {firewallEnabled ? 'Disable' : 'Enable'}
          </button>
        </div>
      </div>

      {/* Stats Dashboard */}
      <div className="grid grid-cols-1 md:grid-cols-5 gap-4">
        <div className="bg-slate-800/50 rounded-lg p-4 border border-slate-700">
          <div className="text-2xl font-bold text-white">{stats.totalRules}</div>
          <div className="text-slate-400 text-sm">Total Rules</div>
        </div>
        <div className="bg-slate-800/50 rounded-lg p-4 border border-slate-700">
          <div className="text-2xl font-bold text-green-400">{stats.activeRules}</div>
          <div className="text-slate-400 text-sm">Active Rules</div>
        </div>
        <div className="bg-slate-800/50 rounded-lg p-4 border border-slate-700">
          <div className="text-2xl font-bold text-red-400">{stats.blockedConnections}</div>
          <div className="text-slate-400 text-sm">Blocked Today</div>
        </div>
        <div className="bg-slate-800/50 rounded-lg p-4 border border-slate-700">
          <div className="text-2xl font-bold text-blue-400">{stats.allowedConnections}</div>
          <div className="text-slate-400 text-sm">Allowed Today</div>
        </div>
        <div className="bg-slate-800/50 rounded-lg p-4 border border-slate-700">
          <div className="text-2xl font-bold text-orange-400">{stats.threatsBlocked}</div>
          <div className="text-slate-400 text-sm">Threats Blocked</div>
        </div>
      </div>

      {/* Controls */}
      <div className="flex items-center justify-between">
        <h2 className="text-xl font-semibold text-white">Firewall Rules</h2>
        <button
          onClick={() => setShowAddRule(true)}
          className="btn-primary"
        >
          <Plus className="mr-2 h-4 w-4" />
          Add Rule
        </button>
      </div>

      {/* Rules Table */}
      <div className="bg-slate-800/50 rounded-lg border border-slate-700 overflow-hidden">
        <div className="overflow-x-auto">
          <table className="w-full">
            <thead className="bg-slate-700/50">
              <tr>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Rule Name</th>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Action</th>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Protocol</th>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Source</th>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Destination</th>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Status</th>
                <th className="px-4 py-3 text-left text-sm font-semibold text-slate-300">Actions</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-700/50">
              {rules.map((rule) => (
                <tr key={rule.id} className="hover:bg-slate-700/30 transition-colors">
                  <td className="px-4 py-3">
                    <div>
                      <div className="font-medium text-white">{rule.name}</div>
                      <div className="text-sm text-slate-400">{rule.description}</div>
                    </div>
                  </td>
                  <td className="px-4 py-3">
                    <span className={`px-2 py-1 rounded-full text-xs font-medium ${
                      rule.action === 'allow' ? 'bg-green-500/20 text-green-400' :
                      rule.action === 'block' ? 'bg-red-500/20 text-red-400' :
                      'bg-yellow-500/20 text-yellow-400'
                    }`}>
                      {rule.action.toUpperCase()}
                    </span>
                  </td>
                  <td className="px-4 py-3 text-slate-300 font-mono text-sm">
                    {rule.protocol.toUpperCase()}
                  </td>
                  <td className="px-4 py-3 text-slate-300 font-mono text-sm">
                    {rule.sourceIP}:{rule.sourcePort}
                  </td>
                  <td className="px-4 py-3 text-slate-300 font-mono text-sm">
                    {rule.destIP}:{rule.destPort}
                  </td>
                  <td className="px-4 py-3">
                    <button
                      onClick={() => toggleRule(rule.id)}
                      className={`flex items-center space-x-1 ${
                        rule.enabled ? 'text-green-400' : 'text-slate-500'
                      }`}
                    >
                      {rule.enabled ? (
                        <CheckCircle className="h-4 w-4" />
                      ) : (
                        <X className="h-4 w-4" />
                      )}
                      <span className="text-sm">{rule.enabled ? 'Enabled' : 'Disabled'}</span>
                    </button>
                  </td>
                  <td className="px-4 py-3">
                    <div className="flex items-center space-x-2">
                      <button
                        onClick={() => setEditingRule(rule)}
                        className="text-slate-400 hover:text-orange-400 transition-colors"
                      >
                        <Edit className="h-4 w-4" />
                      </button>
                      <button
                        onClick={() => deleteRule(rule.id)}
                        className="text-slate-400 hover:text-red-400 transition-colors"
                      >
                        <Trash2 className="h-4 w-4" />
                      </button>
                    </div>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>

      {/* Add/Edit Rule Modal */}
      {(showAddRule || editingRule) && (
        <FirewallRuleModal
          rule={editingRule}
          onSave={editingRule ? updateRule : addRule}
          onClose={() => {
            setShowAddRule(false)
            setEditingRule(null)
          }}
        />
      )}
    </div>
  )
}

interface FirewallRuleModalProps {
  rule?: FirewallRule | null
  onSave: (rule: FirewallRule | Omit<FirewallRule, 'id'>) => void
  onClose: () => void
}

const FirewallRuleModal: React.FC<FirewallRuleModalProps> = ({ rule, onSave, onClose }) => {
  const [formData, setFormData] = useState({
    name: rule?.name || '',
    action: rule?.action || 'allow' as const,
    protocol: rule?.protocol || 'tcp' as const,
    sourceIP: rule?.sourceIP || '',
    destIP: rule?.destIP || '',
    sourcePort: rule?.sourcePort || '',
    destPort: rule?.destPort || '',
    enabled: rule?.enabled ?? true,
    priority: rule?.priority || 1,
    description: rule?.description || ''
  })

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault()
    
    if (rule) {
      onSave({ ...rule, ...formData })
    } else {
      onSave(formData)
    }
  }

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/50 backdrop-blur-sm">
      <motion.div
        initial={{ opacity: 0, scale: 0.9 }}
        animate={{ opacity: 1, scale: 1 }}
        className="bg-slate-800/95 backdrop-blur-sm rounded-2xl border border-slate-700 w-full max-w-2xl shadow-2xl"
      >
        <div className="flex items-center justify-between p-6 border-b border-slate-700">
          <h2 className="text-xl font-bold text-white">
            {rule ? 'Edit Firewall Rule' : 'Add New Firewall Rule'}
          </h2>
          <button onClick={onClose} className="text-slate-400 hover:text-white transition-colors">
            <X className="h-6 w-6" />
          </button>
        </div>

        <form onSubmit={handleSubmit} className="p-6 space-y-6">
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Rule Name</label>
              <input
                type="text"
                value={formData.name}
                onChange={(e) => setFormData({ ...formData, name: e.target.value })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
                placeholder="Enter rule name"
                required
              />
            </div>
            
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Action</label>
              <select
                value={formData.action}
                onChange={(e) => setFormData({ ...formData, action: e.target.value as any })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
              >
                <option value="allow">Allow</option>
                <option value="block">Block</option>
                <option value="monitor">Monitor</option>
              </select>
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Protocol</label>
              <select
                value={formData.protocol}
                onChange={(e) => setFormData({ ...formData, protocol: e.target.value as any })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
              >
                <option value="tcp">TCP</option>
                <option value="udp">UDP</option>
                <option value="icmp">ICMP</option>
                <option value="any">Any</option>
              </select>
            </div>
            
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Priority</label>
              <input
                type="number"
                value={formData.priority}
                onChange={(e) => setFormData({ ...formData, priority: parseInt(e.target.value) })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
                min="1"
                max="100"
              />
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Source IP</label>
              <input
                type="text"
                value={formData.sourceIP}
                onChange={(e) => setFormData({ ...formData, sourceIP: e.target.value })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
                placeholder="192.168.1.0/24 or any"
                required
              />
            </div>
            
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Source Port</label>
              <input
                type="text"
                value={formData.sourcePort}
                onChange={(e) => setFormData({ ...formData, sourcePort: e.target.value })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
                placeholder="80,443 or any"
                required
              />
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Destination IP</label>
              <input
                type="text"
                value={formData.destIP}
                onChange={(e) => setFormData({ ...formData, destIP: e.target.value })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
                placeholder="10.0.0.0/8 or any"
                required
              />
            </div>
            
            <div>
              <label className="block text-sm font-medium text-slate-300 mb-2">Destination Port</label>
              <input
                type="text"
                value={formData.destPort}
                onChange={(e) => setFormData({ ...formData, destPort: e.target.value })}
                className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors"
                placeholder="22,80,443 or any"
                required
              />
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-slate-300 mb-2">Description</label>
            <textarea
              value={formData.description}
              onChange={(e) => setFormData({ ...formData, description: e.target.value })}
              className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-orange-500 focus:ring-1 focus:ring-orange-500 transition-colors resize-none"
              rows={3}
              placeholder="Describe the purpose of this rule"
            />
          </div>

          <div className="flex items-center space-x-2">
            <input
              type="checkbox"
              id="enabled"
              checked={formData.enabled}
              onChange={(e) => setFormData({ ...formData, enabled: e.target.checked })}
              className="w-4 h-4 text-orange-500 bg-slate-700 border-slate-600 rounded focus:ring-orange-500"
            />
            <label htmlFor="enabled" className="text-sm text-slate-300">
              Enable this rule immediately
            </label>
          </div>

          <div className="flex space-x-4">
            <button
              type="submit"
              className="flex-1 btn-primary"
            >
              {rule ? 'Update Rule' : 'Add Rule'}
            </button>
            <button
              type="button"
              onClick={onClose}
              className="flex-1 bg-slate-700/50 border border-slate-600 text-slate-300 hover:text-white hover:bg-slate-700 font-medium py-3 px-6 rounded-lg transition-all duration-200"
            >
              Cancel
            </button>
          </div>
        </form>
      </motion.div>
    </div>
  )
}

export default FirewallManager