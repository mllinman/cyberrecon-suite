import React, { useState } from 'react'
import { motion, AnimatePresence } from 'framer-motion'
import { useAuth } from '../contexts/AuthContext'
import { useTheme } from '../contexts/ThemeContext'
import WidgetGrid from '../components/WidgetGrid'
import { WidgetConfig } from '../components/Widget'
import { 
  Download, 
  Shield, 
  Users, 
  Activity, 
  Settings, 
  CreditCard,
  BarChart3,
  AlertTriangle,
  CheckCircle,
  TrendingUp,
  Menu,
  X,
  Grid3x3,
  Layout
} from 'lucide-react'

const Dashboard = () => {
  const { user } = useAuth()
  const { currentTheme, animationsEnabled } = useTheme()
  const [sidebarOpen, setSidebarOpen] = useState(false)
  const [dashboardView, setDashboardView] = useState<'overview' | 'widgets' | 'analytics'>('overview')
  const [widgetLayout, setWidgetLayout] = useState<WidgetConfig[]>([])

  // Load saved widget layout
  React.useEffect(() => {
    const savedLayout = localStorage.getItem('cyberrecon-widget-layout')
    if (savedLayout) {
      try {
        setWidgetLayout(JSON.parse(savedLayout))
      } catch (error) {
        console.error('Error loading widget layout:', error)
      }
    }
  }, [])

  if (!user) {
    return (
      <div className="pt-16 min-h-screen flex items-center justify-center px-4">
        <div className="text-center max-w-md">
          <motion.div
            initial={{ opacity: 0, scale: 0.9 }}
            animate={{ opacity: 1, scale: 1 }}
            className="bg-slate-800/50 backdrop-blur-sm rounded-2xl p-8 border border-slate-700"
          >
            <AlertTriangle className="h-16 w-16 text-amber-400 mx-auto mb-4" />
            <h1 className="text-2xl font-bold text-white mb-4">Access Denied</h1>
            <p className="text-slate-400 mb-6">Please log in to access your dashboard.</p>
            <a href="/login" className="btn-primary">
              Login to Continue
            </a>
          </motion.div>
        </div>
      </div>
    )
  }

  // Enhanced stats data
  const dashboardStats = [
    {
      title: 'Events Processed',
      value: '45,000',
      limit: '100,000',
      percentage: 45,
      trend: '+12%',
      trendUp: true,
      icon: Activity,
      color: 'blue'
    },
    {
      title: 'Threats Detected',
      value: '127',
      trend: '+3%',
      trendUp: true,
      icon: AlertTriangle,
      color: 'red'
    },
    {
      title: 'Uptime',
      value: '99.97%',
      trend: '+0.1%',
      trendUp: true,
      icon: CheckCircle,
      color: 'green'
    },
    {
      title: 'Response Time',
      value: '1.2s',
      trend: '-0.3s',
      trendUp: true,
      icon: TrendingUp,
      color: 'purple'
    }
  ]

  const getStatColorClasses = (color: string) => {
    const colorMap = {
      blue: 'bg-blue-500/20 text-blue-400 border-blue-500/30',
      red: 'bg-red-500/20 text-red-400 border-red-500/30', 
      green: 'bg-emerald-500/20 text-emerald-400 border-emerald-500/30',
      purple: 'bg-purple-500/20 text-purple-400 border-purple-500/30'
    }
    return colorMap[color] || colorMap.blue
  }

  const handleDownload = (filename: string) => {
    // Simulate download with filename
    const link = document.createElement('a')
    link.href = `/downloads/${filename}`
    link.download = filename
    link.click()
  }

  // Widget management functions
  const handleWidgetLayoutSave = (layout: WidgetConfig[]) => {
    setWidgetLayout(layout)
    localStorage.setItem('cyberrecon-widget-layout', JSON.stringify(layout))
  }

  const handleWidgetAdd = (widget: WidgetConfig) => {
    const newLayout = [...widgetLayout, widget]
    handleWidgetLayoutSave(newLayout)
  }

  const handleWidgetRemove = (id: string) => {
    const newLayout = widgetLayout.filter(w => w.id !== id)
    handleWidgetLayoutSave(newLayout)
  }

  const handleWidgetResize = (id: string, size: WidgetConfig['size']) => {
    const newLayout = widgetLayout.map(w => w.id === id ? { ...w, size } : w)
    handleWidgetLayoutSave(newLayout)
  }

  // Widget components for modular dashboard
  const StatWidget = ({ stat, index }: { stat: any, index: number }) => (
    <motion.div
      initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ duration: 0.4, delay: index * 0.1 }}
      className="card p-4 sm:p-6"
    >
      <div className="flex items-center justify-between mb-4">
        <div className={`p-2 sm:p-3 rounded-xl border ${getStatColorClasses(stat.color)}`}>
          <stat.icon className="h-5 w-5 sm:h-6 sm:w-6" />
        </div>
        {stat.trend && (
          <div className={`flex items-center text-xs sm:text-sm font-medium ${
            stat.trendUp ? 'text-emerald-400' : 'text-red-400'
          }`}>
            {stat.trendUp ? '↗' : '↘'} {stat.trend}
          </div>
        )}
      </div>
      <div className="space-y-1">
        <h3 className="text-xs sm:text-sm font-medium text-slate-400">{stat.title}</h3>
        <p className="text-xl sm:text-2xl font-bold text-white">{stat.value}</p>
        {stat.limit && (
          <div className="mt-2">
            <div className="flex justify-between text-xs text-slate-400 mb-1">
              <span>Usage</span>
              <span>{stat.value} / {stat.limit}</span>
            </div>
            <div className="w-full bg-slate-700 rounded-full h-2">
              <div 
                className="bg-gradient-to-r from-primary-500 to-primary-400 h-2 rounded-full transition-all duration-500"
                style={{ width: `${stat.percentage}%` }}
              />
            </div>
          </div>
        )}
      </div>
    </motion.div>
  )

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
    <div className="pt-16 min-h-screen bg-gradient-to-br from-slate-900 via-slate-900 to-slate-800">
      {/* Mobile Header */}
      <div className="lg:hidden bg-slate-800/50 backdrop-blur-sm border-b border-slate-700 sticky top-16 z-40">
        <div className="px-4 py-3 flex items-center justify-between">
          <div>
            <h1 className="text-lg font-bold text-white truncate">Hi, {user.name}</h1>
            <p className="text-xs text-slate-400 capitalize">{user.subscription} Plan</p>
          </div>
          <button
            onClick={() => setSidebarOpen(!sidebarOpen)}
            className="p-2 rounded-lg bg-slate-700/50 hover:bg-slate-700 transition-colors"
            aria-label="Toggle sidebar"
          >
            {sidebarOpen ? <X className="h-5 w-5" /> : <Menu className="h-5 w-5" />}
          </button>
        </div>
        
        {/* Dashboard View Toggle */}
        <div className="px-4 pb-3">
          <div className="flex space-x-1 bg-slate-700/50 p-1 rounded-lg">
            {[
              { id: 'overview', label: 'Overview', icon: BarChart3 },
              { id: 'widgets', label: 'Widgets', icon: Grid3x3 },
              { id: 'analytics', label: 'Analytics', icon: TrendingUp }
            ].map((view) => (
              <button
                key={view.id}
                onClick={() => setDashboardView(view.id as any)}
                className={`flex-1 flex items-center justify-center space-x-1 py-2 px-3 rounded-md text-xs font-medium transition-all ${
                  dashboardView === view.id
                    ? 'bg-primary-500 text-white shadow-lg'
                    : 'text-slate-400 hover:text-white hover:bg-slate-600/50'
                }`}
              >
                <view.icon className="h-4 w-4" />
                <span className="hidden sm:inline">{view.label}</span>
              </button>
            ))}
          </div>
        </div>
      </div>

      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4 sm:py-8">
        {/* Desktop Header */}
        <motion.div
          initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.6 }}
          className="hidden lg:block mb-8"
        >
          <div className="flex items-center justify-between">
            <div>
              <h1 className="text-2xl sm:text-3xl font-bold text-white mb-2">
                Welcome back, {user.name}
              </h1>
              <p className="text-slate-400">
                Manage your CyberRecon Suite subscription and monitor your security operations
              </p>
            </div>
            <div className="flex items-center space-x-3">
              <div className="flex items-center space-x-2 bg-slate-800/50 px-3 py-2 rounded-lg border border-slate-700">
                <Layout className="h-4 w-4 text-primary-400" />
                <span className="text-sm text-slate-300">Dashboard Layout</span>
              </div>
              <div className={`px-3 py-2 rounded-lg text-sm font-medium border ${
                user.subscriptionStatus === 'active' 
                  ? 'bg-emerald-500/20 text-emerald-400 border-emerald-500/30' 
                  : 'bg-amber-500/20 text-amber-400 border-amber-500/30'
              }`}>
                {user.subscriptionStatus === 'active' ? '✅ Active' : '⏳ Pending'}
              </div>
            </div>
          </div>
        </motion.div>

        {/* Stats Grid - Always visible */}
        <div className="mb-6 sm:mb-8">
          <div className="grid grid-cols-1 sm:grid-cols-2 xl:grid-cols-4 gap-4 sm:gap-6">
            {dashboardStats.map((stat, index) => (
              <StatWidget key={stat.title} stat={stat} index={index} />
            ))}
          </div>
        </div>

        {/* Main Content Area */}
        <div className="flex flex-col lg:flex-row gap-6 lg:gap-8">
          {/* Main Content */}
          <div className="flex-1 space-y-6 sm:space-y-8">
            {dashboardView === 'overview' && (
              <>
                {/* Subscription Status */}
                <motion.div
                  initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 0.6, delay: 0.1 }}
                  className="card"
                >
                  <div className="flex flex-col sm:flex-row sm:items-center justify-between mb-6 gap-4">
                    <h2 className="text-xl font-semibold text-white">Subscription Overview</h2>
                    <div className="flex items-center space-x-3">
                      <div className="text-right">
                        <p className="text-sm text-slate-400">Current Plan</p>
                        <p className="text-lg font-bold text-primary-400 capitalize">{user.subscription}</p>
                      </div>
                    </div>
                  </div>
                  
                  <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 sm:gap-6">
                    <div className="text-center sm:text-left">
                      <p className="text-slate-400 text-sm mb-1">Monthly Usage</p>
                      <p className="text-2xl font-bold text-white">45,000</p>
                      <p className="text-xs text-slate-500">of 100,000 events</p>
                      <div className="mt-2 w-full bg-slate-700 rounded-full h-2">
                        <div className="bg-gradient-to-r from-primary-500 to-primary-400 h-2 rounded-full w-[45%]"></div>
                      </div>
                    </div>
                    <div className="text-center sm:text-left">
                      <p className="text-slate-400 text-sm mb-1">Next Billing</p>
                      <p className="text-lg font-semibold text-white">Feb 15, 2024</p>
                      <p className="text-xs text-slate-500">Auto-renewal enabled</p>
                    </div>
                    <div className="text-center sm:text-left">
                      <p className="text-slate-400 text-sm mb-1">Support Level</p>
                      <p className="text-lg font-semibold text-white">
                        {user.subscription === 'enterprise' ? '24/7 Priority' : 'Email Support'}
                      </p>
                      <p className="text-xs text-slate-500">Response within 2-4 hours</p>
                    </div>
                  </div>
                </motion.div>

                {/* Downloads Section */}
                <motion.div
                  initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 0.6, delay: 0.2 }}
                  className="card"
                >
                  <h2 className="text-xl font-semibold text-white mb-6">Available Downloads</h2>
                  
                  <div className="space-y-4">
                    {[
                      {
                        name: 'CyberRecon Suite v1.7',
                        description: 'Complete cybersecurity operations platform',
                        size: 'Windows, macOS, Linux • 245 MB',
                        icon: Shield,
                        filename: 'CyberReconSuite-v1.7.zip',
                        primary: true
                      },
                      {
                        name: 'Configuration Templates',
                        description: 'Pre-configured settings for common deployments',
                        size: 'JSON, XML • 2.1 MB',
                        icon: Settings,
                        filename: 'config-templates.zip',
                        primary: false
                      },
                      {
                        name: 'Documentation & Guides',
                        description: 'Complete user manual and API documentation',
                        size: 'PDF • 15.3 MB',
                        icon: Users,
                        filename: 'documentation.zip',
                        primary: false
                      }
                    ].map((download, index) => (
                      <div key={download.name} className="flex flex-col sm:flex-row sm:items-center justify-between p-4 bg-slate-700/30 rounded-xl border border-slate-600/30 hover:border-primary-500/30 transition-all duration-200 gap-4">
                        <div className="flex items-center space-x-4">
                          <div className="w-12 h-12 bg-primary-500/20 rounded-xl flex items-center justify-center flex-shrink-0">
                            <download.icon className="h-6 w-6 text-primary-400" />
                          </div>
                          <div className="min-w-0 flex-1">
                            <h3 className="font-semibold text-white text-sm sm:text-base">{download.name}</h3>
                            <p className="text-sm text-slate-400 mt-1">{download.description}</p>
                            <p className="text-xs text-slate-500 mt-1">{download.size}</p>
                          </div>
                        </div>
                        <button
                          onClick={() => handleDownload(download.filename)}
                          className={`w-full sm:w-auto flex items-center justify-center ${
                            download.primary ? 'btn-primary' : 'btn-secondary'
                          } text-sm px-4 py-2`}
                        >
                          <Download className="mr-2 h-4 w-4" />
                          Download
                        </button>
                      </div>
                    ))}
                  </div>
                </motion.div>
              </>
            )}

            {dashboardView === 'widgets' && (
              <WidgetGrid
                widgets={widgetLayout}
                onWidgetAdd={handleWidgetAdd}
                onWidgetRemove={handleWidgetRemove}
                onWidgetResize={handleWidgetResize}
                onLayoutSave={handleWidgetLayoutSave}
                editable={true}
                className="pb-8"
              />
            )}

            {dashboardView === 'analytics' && (
              <motion.div
                initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
                animate={{ opacity: 1, y: 0 }}
                className="card"
              >
                <h2 className="text-xl font-semibold text-white mb-6">Security Analytics</h2>
                <p className="text-slate-400 mb-8">Advanced analytics and reporting for your security operations.</p>
                <div className="text-center py-12">
                  <TrendingUp className="h-16 w-16 text-slate-500 mx-auto mb-4" />
                  <h3 className="text-lg font-semibold text-white mb-2">Advanced Analytics</h3>
                  <p className="text-slate-400">Detailed security analytics dashboard coming in the next release.</p>
                </div>
              </motion.div>
            )}

            {/* Recent Activity - Always visible */}
            <motion.div
              initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.6, delay: 0.3 }}
              className="card"
            >
              <h2 className="text-xl font-semibold text-white mb-6">Recent Activity</h2>
              
              <div className="space-y-3">
                {[
                  { action: 'Downloaded CyberRecon Suite v1.7', time: '2 hours ago', type: 'download' },
                  { action: 'Subscription activated', time: '1 day ago', type: 'subscription' },
                  { action: 'Account created', time: '1 day ago', type: 'account' },
                  { action: 'Security scan completed', time: '2 days ago', type: 'security' }
                ].map((activity, index) => (
                  <div key={index} className="flex items-center space-x-3 p-3 bg-slate-700/30 rounded-lg hover:bg-slate-700/50 transition-colors">
                    <div className={`w-2 h-2 rounded-full flex-shrink-0 ${
                      activity.type === 'download' ? 'bg-emerald-500' :
                      activity.type === 'subscription' ? 'bg-blue-500' : 
                      activity.type === 'security' ? 'bg-amber-500' :
                      'bg-purple-500'
                    }`}></div>
                    <div className="flex-1 min-w-0">
                      <p className="text-white text-sm">{activity.action}</p>
                      <p className="text-slate-400 text-xs">{activity.time}</p>
                    </div>
                  </div>
                ))}
              </div>
            </motion.div>
          </div>

          {/* Sidebar */}
          <AnimatePresence>
            {(sidebarOpen || window.innerWidth >= 1024) && (
              <motion.div
                initial={{ opacity: 0, x: 20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: 20 }}
                className="w-full lg:w-80 space-y-6 lg:space-y-8"
              >
                {/* Account Info */}
                <motion.div
                  initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 0.6, delay: 0.4 }}
                  className="card"
                >
                  <h2 className="text-xl font-semibold text-white mb-6">Account Information</h2>
                  
                  <div className="space-y-4">
                    <div>
                      <p className="text-slate-400 text-sm">Email</p>
                      <p className="text-white break-all">{user.email}</p>
                    </div>
                    <div>
                      <p className="text-slate-400 text-sm">Subscription</p>
                      <p className="text-primary-400 font-semibold capitalize">{user.subscription}</p>
                    </div>
                    <div>
                      <p className="text-slate-400 text-sm">Status</p>
                      <p className="text-emerald-400 capitalize">{user.subscriptionStatus}</p>
                    </div>
                  </div>
                  
                  <button className="w-full mt-6 btn-secondary">
                    <Settings className="mr-2 h-4 w-4" />
                    Manage Account
                  </button>
                </motion.div>

                {/* Plan Features */}
                <motion.div
                  initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 0.6, delay: 0.5 }}
                  className="card"
                >
                  <h2 className="text-xl font-semibold text-white mb-6">Plan Features</h2>
                  
                  <ul className="space-y-3">
                    {subscriptionFeatures[user.subscription]?.map((feature, index) => (
                      <li key={index} className="flex items-start space-x-3 text-sm">
                        <div className="w-1.5 h-1.5 bg-primary-500 rounded-full mt-2 flex-shrink-0"></div>
                        <span className="text-slate-300">{feature}</span>
                      </li>
                    ))}
                  </ul>
                  
                  {user.subscription !== 'enterprise' && (
                    <button className="w-full mt-6 btn-primary">
                      <CreditCard className="mr-2 h-4 w-4" />
                      Upgrade Plan
                    </button>
                  )}
                </motion.div>

                {/* Support */}
                <motion.div
                  initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 0.6, delay: 0.6 }}
                  className="card"
                >
                  <h2 className="text-xl font-semibold text-white mb-4">Need Help?</h2>
                  <p className="text-slate-400 text-sm mb-4">
                    Our support team is here to help you get the most out of CyberRecon Suite.
                  </p>
                  <div className="space-y-3">
                    <a 
                      href="mailto:support@bulletdropstudio.com" 
                      className="flex items-center space-x-2 text-primary-400 hover:text-primary-300 text-sm p-2 rounded-lg hover:bg-slate-700/30 transition-colors"
                    >
                      <span>📧</span>
                      <span className="break-all">support@bulletdropstudio.com</span>
                    </a>
                    <a 
                      href="tel:+19893239690" 
                      className="flex items-center space-x-2 text-primary-400 hover:text-primary-300 text-sm p-2 rounded-lg hover:bg-slate-700/30 transition-colors"
                    >
                      <span>📞</span>
                      <span>+1 (989) 323-9690</span>
                    </a>
                    {user.subscription === 'enterprise' && (
                      <div className="p-3 bg-emerald-500/10 border border-emerald-500/30 rounded-lg">
                        <p className="text-emerald-400 text-sm font-medium">
                          🟢 24/7 Priority Support Available
                        </p>
                      </div>
                    )}
                  </div>
                </motion.div>
              </motion.div>
            )}
          </AnimatePresence>
        </div>
      </div>
    </div>
  )
}

export default Dashboard