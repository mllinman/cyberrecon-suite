import React from 'react'
import { motion } from 'framer-motion'
import { useAuth } from '../contexts/AuthContext'
import { Download, Shield, Users, Activity, Settings, CreditCard, Eye, Network } from 'lucide-react'
import { Link } from 'react-router-dom'

const Dashboard = () => {
  const { user } = useAuth()

  if (!user) {
    return (
      <div className="pt-16 min-h-screen flex items-center justify-center">
        <div className="text-center">
          <h1 className="text-2xl font-bold text-white mb-4">Access Denied</h1>
          <p className="text-gray-400">Please log in to access your dashboard.</p>
        </div>
      </div>
    )
  }

  const handleDownload = () => {
    // Simulate download
    const link = document.createElement('a')
    link.href = '/downloads/CyberReconSuite-v1.7.zip'
    link.download = 'CyberReconSuite-v1.7.zip'
    link.click()
  }

  const subscriptionFeatures = {
    individual: [
      'Basic SIEM & EDR monitoring',
      'Limited threat intelligence feeds',
      'Up to 10,000 events/month',
      'Basic dashboards and reporting',
      'Email support',
      'Single user license'
    ],
    professional: [
      'Full SIEM & EDR monitoring',
      'Complete threat intelligence feeds',
      'Up to 100,000 events/month',
      'Penetration testing tools',
      'Advanced firewall protection',
      'Basic penetration testing tools',
      'Email support',
      'Team collaboration (up to 10 users)'
    ],
    enterprise: [
      'All Professional features',
      'Advanced ML correlation',
      'Unlimited events processing',
      'Full penetration testing suite',
      'Custom integrations',
      '24/7 phone support',
      'Dedicated account manager'
    ]
  }

  return (
    <div className="pt-16 min-h-screen">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
        {/* Header */}
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.6 }}
          className="mb-8"
        >
          <h1 className="text-3xl font-bold text-white mb-2">
            Welcome back, {user.name}
          </h1>
          <p className="text-gray-400">
            Manage your CyberRecon Suite subscription and downloads
          </p>
        </motion.div>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
          {/* Main Content */}
          <div className="lg:col-span-2 space-y-8">
            {/* Subscription Status */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.1 }}
              className="card"
            >
              <div className="flex items-center justify-between mb-6">
                <h2 className="text-xl font-semibold text-white">Subscription Status</h2>
                <div className={`px-3 py-1 rounded-full text-sm font-medium ${
                  user.subscriptionStatus === 'active' 
                    ? 'bg-green-500/20 text-green-400' 
                    : 'bg-yellow-500/20 text-yellow-400'
                }`}>
                  {user.subscriptionStatus === 'active' ? '✅ Active' : '⏳ Pending'}
                </div>
              </div>
              
              <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
                <div>
                  <p className="text-gray-400 text-sm">Current Plan</p>
                  <p className="text-2xl font-bold text-primary-400 capitalize">{user.subscription}</p>
                </div>
                <div>
                  <p className="text-gray-400 text-sm">Monthly Usage</p>
                  <p className="text-2xl font-bold text-white">45,000</p>
                  <p className="text-xs text-gray-500">of 100,000 events</p>
                </div>
                <div>
                  <p className="text-gray-400 text-sm">Next Billing</p>
                  <p className="text-lg font-semibold text-white">Feb 15, 2024</p>
                </div>
              </div>
            </motion.div>

            {/* Real-Time Dashboards */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.15 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-6">🔴 Live Security Dashboards</h2>
              
              <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
                <Link 
                  to="/siem"
                  className="flex items-center justify-between p-4 bg-dark-700/50 rounded-lg hover:bg-primary-500/10 transition-colors group"
                >
                  <div className="flex items-center space-x-4">
                    <div className="w-12 h-12 bg-primary-500/20 rounded-lg flex items-center justify-center">
                      <Shield className="h-6 w-6 text-primary-400" />
                    </div>
                    <div>
                      <h3 className="font-semibold text-white group-hover:text-primary-400 transition-colors">SIEM Dashboard</h3>
                      <p className="text-sm text-gray-400">Security events & monitoring</p>
                      <p className="text-xs text-green-400">🔴 Live SIEM Active</p>
                    </div>
                  </div>
                  <div className="text-primary-400 opacity-0 group-hover:opacity-100 transition-opacity">
                    →
                  </div>
                </Link>

                <Link 
                  to="/threat-intelligence"
                  className="flex items-center justify-between p-4 bg-dark-700/50 rounded-lg hover:bg-primary-500/10 transition-colors group"
                >
                  <div className="flex items-center space-x-4">
                    <div className="w-12 h-12 bg-red-500/20 rounded-lg flex items-center justify-center">
                      <Eye className="h-6 w-6 text-red-400" />
                    </div>
                    <div>
                      <h3 className="font-semibold text-white group-hover:text-primary-400 transition-colors">Threat Intelligence</h3>
                      <p className="text-sm text-gray-400">Real-time threat feeds & analysis</p>
                      <p className="text-xs text-green-400">🔴 Live Feed Active</p>
                    </div>
                  </div>
                  <div className="text-primary-400 opacity-0 group-hover:opacity-100 transition-opacity">
                    →
                  </div>
                </Link>

                <Link 
                  to="/network-monitoring"
                  className="flex items-center justify-between p-4 bg-dark-700/50 rounded-lg hover:bg-primary-500/10 transition-colors group"
                >
                  <div className="flex items-center space-x-4">
                    <div className="w-12 h-12 bg-blue-500/20 rounded-lg flex items-center justify-center">
                      <Network className="h-6 w-6 text-blue-400" />
                    </div>
                    <div>
                      <h3 className="font-semibold text-white group-hover:text-primary-400 transition-colors">Network Monitoring</h3>
                      <p className="text-sm text-gray-400">Live network traffic analysis</p>
                      <p className="text-xs text-green-400">🔴 Monitoring Active</p>
                    </div>
                  </div>
                  <div className="text-primary-400 opacity-0 group-hover:opacity-100 transition-opacity">
                    →
                  </div>
                </Link>
              </div>
            </motion.div>

            {/* Downloads */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.2 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-6">Downloads</h2>
              
              <div className="space-y-4">
                <div className="flex items-center justify-between p-4 bg-dark-700/50 rounded-lg">
                  <div className="flex items-center space-x-4">
                    <div className="w-12 h-12 bg-primary-500/20 rounded-lg flex items-center justify-center">
                      <Shield className="h-6 w-6 text-primary-500" />
                    </div>
                    <div>
                      <h3 className="font-semibold text-white">CyberRecon Suite v1.7</h3>
                      <p className="text-sm text-gray-400">Complete cybersecurity operations platform</p>
                      <p className="text-xs text-gray-500">Windows, macOS, Linux • 245 MB</p>
                    </div>
                  </div>
                  <button
                    onClick={handleDownload}
                    className="btn-primary"
                  >
                    <Download className="mr-2 h-4 w-4" />
                    Download
                  </button>
                </div>

                <div className="flex items-center justify-between p-4 bg-dark-700/50 rounded-lg">
                  <div className="flex items-center space-x-4">
                    <div className="w-12 h-12 bg-primary-500/20 rounded-lg flex items-center justify-center">
                      <Settings className="h-6 w-6 text-primary-500" />
                    </div>
                    <div>
                      <h3 className="font-semibold text-white">Configuration Templates</h3>
                      <p className="text-sm text-gray-400">Pre-configured settings for common deployments</p>
                      <p className="text-xs text-gray-500">JSON, XML • 2.1 MB</p>
                    </div>
                  </div>
                  <button
                    onClick={handleDownload}
                    className="btn-secondary"
                  >
                    <Download className="mr-2 h-4 w-4" />
                    Download
                  </button>
                </div>

                <div className="flex items-center justify-between p-4 bg-dark-700/50 rounded-lg">
                  <div className="flex items-center space-x-4">
                    <div className="w-12 h-12 bg-primary-500/20 rounded-lg flex items-center justify-center">
                      <Users className="h-6 w-6 text-primary-500" />
                    </div>
                    <div>
                      <h3 className="font-semibold text-white">Documentation & Guides</h3>
                      <p className="text-sm text-gray-400">Complete user manual and API documentation</p>
                      <p className="text-xs text-gray-500">PDF • 15.3 MB</p>
                    </div>
                  </div>
                  <button
                    onClick={handleDownload}
                    className="btn-secondary"
                  >
                    <Download className="mr-2 h-4 w-4" />
                    Download
                  </button>
                </div>
              </div>
            </motion.div>

            {/* Recent Activity */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.3 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-6">Recent Activity</h2>
              
              <div className="space-y-3">
                {[
                  { action: 'Downloaded CyberRecon Suite v1.7', time: '2 hours ago', type: 'download' },
                  { action: 'Subscription activated', time: '1 day ago', type: 'subscription' },
                  { action: 'Account created', time: '1 day ago', type: 'account' }
                ].map((activity, index) => (
                  <div key={index} className="flex items-center space-x-3 p-3 bg-dark-700/30 rounded-lg">
                    <div className={`w-2 h-2 rounded-full ${
                      activity.type === 'download' ? 'bg-green-500' :
                      activity.type === 'subscription' ? 'bg-blue-500' : 'bg-purple-500'
                    }`}></div>
                    <div className="flex-1">
                      <p className="text-white text-sm">{activity.action}</p>
                      <p className="text-gray-400 text-xs">{activity.time}</p>
                    </div>
                  </div>
                ))}
              </div>
            </motion.div>
          </div>

          {/* Sidebar */}
          <div className="space-y-8">
            {/* Account Info */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.4 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-6">Account Information</h2>
              
              <div className="space-y-4">
                <div>
                  <p className="text-gray-400 text-sm">Email</p>
                  <p className="text-white">{user.email}</p>
                </div>
                <div>
                  <p className="text-gray-400 text-sm">Subscription</p>
                  <p className="text-primary-400 font-semibold capitalize">{user.subscription}</p>
                </div>
                <div>
                  <p className="text-gray-400 text-sm">Status</p>
                  <p className="text-green-400 capitalize">{user.subscriptionStatus}</p>
                </div>
              </div>
              
              <button className="w-full mt-6 btn-secondary">
                <Settings className="mr-2 h-4 w-4" />
                Manage Account
              </button>
            </motion.div>

            {/* Plan Features */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.5 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-6">Plan Features</h2>
              
              <ul className="space-y-2">
                {subscriptionFeatures[user.subscription]?.map((feature, index) => (
                  <li key={index} className="flex items-center space-x-2 text-sm">
                    <div className="w-1.5 h-1.5 bg-primary-500 rounded-full"></div>
                    <span className="text-gray-300">{feature}</span>
                  </li>
                ))}
              </ul>
              
              {user.subscription === 'professional' && (
                <button className="w-full mt-6 btn-primary">
                  <CreditCard className="mr-2 h-4 w-4" />
                  Upgrade to Enterprise
                </button>
              )}
              
              {user.subscription === 'individual' && (
                <button className="w-full mt-6 btn-primary">
                  <CreditCard className="mr-2 h-4 w-4" />
                  Upgrade to Professional
                </button>
              )}
            </motion.div>

            {/* Support */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.6 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-4">Need Help?</h2>
              <p className="text-gray-400 text-sm mb-4">
                Our support team is here to help you get the most out of CyberRecon Suite.
              </p>
              <div className="space-y-2">
                <a href="mailto:support@bulletdropstudio.com" className="block text-primary-400 hover:text-primary-300 text-sm">
                  📧 support@bulletdropstudio.com
                </a>
                <a href="tel:+19893239690" className="block text-primary-400 hover:text-primary-300 text-sm">
                  📞 +1 (989) 323-9690
                </a>
                {user.subscription === 'enterprise' && (
                  <p className="text-green-400 text-sm">
                    🟢 24/7 Priority Support Available
                  </p>
                )}
              </div>
            </motion.div>
          </div>
        </div>
      </div>
    </div>
  )
}

export default Dashboard