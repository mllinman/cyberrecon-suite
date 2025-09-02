import React from 'react'
import { motion } from 'framer-motion'
import { Eye, Smartphone, Shield, Zap, MapPin, Bell, Fingerprint, AlertTriangle, ArrowRight } from 'lucide-react'
import { Link } from 'react-router-dom'

const CyberRanger = () => {
  const features = [
    {
      icon: Smartphone,
      title: 'Mobile SIEM Dashboard',
      description: 'Full-featured security monitoring dashboard optimized for mobile devices with real-time threat visualization.'
    },
    {
      icon: Bell,
      title: 'Real-time Threat Alerts',
      description: 'Instant push notifications for critical security events with customizable alert thresholds and escalation.'
    },
    {
      icon: Shield,
      title: 'Remote Incident Response',
      description: 'Complete incident response capabilities from mobile devices including containment and remediation actions.'
    },
    {
      icon: MapPin,
      title: 'GPS Security Mapping',
      description: 'Location-aware security monitoring with geographic threat visualization and proximity-based alerts.'
    },
    {
      icon: Fingerprint,
      title: 'Biometric Authentication',
      description: 'Advanced biometric security with fingerprint, face recognition, and behavioral authentication.'
    },
    {
      icon: AlertTriangle,
      title: 'Emergency Response',
      description: 'Dedicated emergency response protocols with one-touch incident escalation and team coordination.'
    }
  ]

  return (
    <div className="pt-16">
      {/* Hero Section */}
      <section className="relative py-20 overflow-hidden">
        <div className="absolute inset-0 cyber-grid opacity-10"></div>
        <div className="relative z-10 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-12 items-center">
            <motion.div
              initial={{ opacity: 0, x: -30 }}
              animate={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.8 }}
            >
              <div className="flex items-center space-x-2 mb-4">
                <img src="/shield-icon.svg" alt="BulletDrop Studios" className="h-8 w-8" />
                <span className="text-primary-400 font-semibold">CyberRanger - Coming Q2 2024</span>
              </div>
              
              <h1 className="text-5xl md:text-6xl font-bold mb-6">
                <span className="gradient-text">Mobile</span>
                <br />
                <span className="text-white">Cybersecurity</span>
                <br />
                <span className="gradient-text">Command Center</span>
              </h1>
              
              <p className="text-xl text-gray-300 mb-8 leading-relaxed">
                Next-generation mobile cybersecurity platform designed for security professionals who need 
                full security operations capabilities while on the move.
              </p>
              
              <div className="flex flex-col sm:flex-row gap-4">
                <Link to="/contact" className="btn-primary text-lg px-8 py-4">
                  Request Early Access
                  <ArrowRight className="ml-2 h-5 w-5" />
                </Link>
                <a href="#features" className="btn-secondary text-lg px-8 py-4">
                  Learn More
                </a>
              </div>
            </motion.div>

            <motion.div
              initial={{ opacity: 0, x: 30 }}
              animate={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.8, delay: 0.2 }}
              className="relative"
            >
              <div className="bg-dark-800 rounded-2xl p-8 border border-dark-700 glow-effect">
                <div className="space-y-4">
                  <div className="flex items-center justify-between">
                    <span className="text-primary-400 font-mono text-sm">CyberRanger Mobile v1.0</span>
                    <div className="flex space-x-2">
                      <div className="w-3 h-3 bg-green-500 rounded-full animate-pulse"></div>
                      <span className="text-green-400 text-xs">LIVE</span>
                    </div>
                  </div>
                  <div className="bg-dark-900 rounded-lg p-4 font-mono text-sm">
                    <div className="text-green-400">📱 [MOBILE] Security dashboard active</div>
                    <div className="text-blue-400">🌍 [GPS] Location: SOC Facility</div>
                    <div className="text-yellow-400">🔔 [ALERT] 2 critical threats detected</div>
                    <div className="text-red-400">🚨 [EMERGENCY] Incident response triggered</div>
                    <div className="text-purple-400">👥 [TEAM] 5 analysts coordinating</div>
                  </div>
                </div>
              </div>
            </motion.div>
          </div>
        </div>
      </section>

      {/* Features Section */}
      <section id="features" className="py-20 bg-dark-800/50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Mobile-First</span> Security Operations
            </h2>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto">
              Full cybersecurity operations capabilities designed specifically for mobile devices
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
            {features.map((feature, index) => (
              <motion.div
                key={feature.title}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="card group"
              >
                <div className="mb-4">
                  <feature.icon className="h-12 w-12 text-primary-500 group-hover:text-primary-400 transition-colors" />
                </div>
                <h3 className="text-xl font-semibold text-white mb-3">{feature.title}</h3>
                <p className="text-gray-400 leading-relaxed">{feature.description}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Coming Soon Section */}
      <section className="py-20">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="card"
          >
            <div className="mb-6">
              <div className="w-20 h-20 bg-primary-500/20 rounded-full flex items-center justify-center mx-auto mb-4">
                <Eye className="h-10 w-10 text-primary-500" />
              </div>
              <h2 className="text-3xl font-bold mb-4">
                <span className="gradient-text">Coming Q2 2024</span>
              </h2>
              <p className="text-xl text-gray-300">
                CyberRanger is currently in development. Be among the first to experience 
                mobile cybersecurity operations.
              </p>
            </div>

            <div className="bg-dark-700/50 rounded-lg p-6 mb-6">
              <h3 className="text-lg font-semibold text-white mb-3">Early Access Benefits</h3>
              <ul className="space-y-2 text-left max-w-md mx-auto">
                <li className="flex items-center space-x-2">
                  <div className="w-1.5 h-1.5 bg-primary-500 rounded-full"></div>
                  <span className="text-gray-300">Beta testing access</span>
                </li>
                <li className="flex items-center space-x-2">
                  <div className="w-1.5 h-1.5 bg-primary-500 rounded-full"></div>
                  <span className="text-gray-300">50% discount on launch</span>
                </li>
                <li className="flex items-center space-x-2">
                  <div className="w-1.5 h-1.5 bg-primary-500 rounded-full"></div>
                  <span className="text-gray-300">Direct feedback to development team</span>
                </li>
                <li className="flex items-center space-x-2">
                  <div className="w-1.5 h-1.5 bg-primary-500 rounded-full"></div>
                  <span className="text-gray-300">Priority customer support</span>
                </li>
              </ul>
            </div>

            <Link to="/contact" className="btn-primary text-lg px-8 py-4">
              Request Early Access
            </Link>
          </motion.div>
        </div>
      </section>
    </div>
  )
}

export default CyberRanger