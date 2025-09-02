import React from 'react'
import { motion } from 'framer-motion'
import { Wifi, Radio, Bluetooth, Scan, Shield, Zap, Network, Lock, ArrowRight } from 'lucide-react'
import { Link } from 'react-router-dom'

const WifiWolverine = () => {
  const features = [
    {
      icon: Wifi,
      title: 'Advanced WiFi Testing',
      description: 'Comprehensive WiFi security assessment with support for WPA3, enterprise networks, and mesh systems.'
    },
    {
      icon: Bluetooth,
      title: 'Bluetooth Security',
      description: 'Complete Bluetooth and BLE security testing including modern protocols and IoT device assessment.'
    },
    {
      icon: Radio,
      title: 'RF Spectrum Analysis',
      description: 'Radio frequency spectrum monitoring and analysis for detecting rogue devices and interference.'
    },
    {
      icon: Scan,
      title: 'IoT Device Discovery',
      description: 'Automated discovery and security assessment of IoT devices across multiple wireless protocols.'
    },
    {
      icon: Network,
      title: 'Protocol Analysis',
      description: 'Deep packet inspection and protocol analysis for wireless communications and security protocols.'
    },
    {
      icon: Lock,
      title: 'Compliance Testing',
      description: 'Automated compliance testing for wireless security standards and regulatory requirements.'
    }
  ]

  const capabilities = [
    'WPA3 and enterprise network testing',
    'Bluetooth 5.0+ and BLE assessment',
    'IoT device vulnerability scanning',
    'Rogue access point detection',
    'Wireless forensics and analysis',
    'RF spectrum monitoring',
    'Mesh network security testing',
    'Compliance reporting (PCI, HIPAA)'
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
                <span className="text-primary-400 font-semibold">Wifi Wolverine - Coming Q3 2024</span>
              </div>
              
              <h1 className="text-5xl md:text-6xl font-bold mb-6">
                <span className="gradient-text">Advanced</span>
                <br />
                <span className="text-white">Wireless</span>
                <br />
                <span className="gradient-text">Security Testing</span>
              </h1>
              
              <p className="text-xl text-gray-300 mb-8 leading-relaxed">
                Specialized wireless penetration testing platform with cutting-edge capabilities for 
                WiFi, Bluetooth, IoT, and RF security assessment in modern enterprise environments.
              </p>
              
              <div className="flex flex-col sm:flex-row gap-4">
                <Link to="/contact" className="btn-primary text-lg px-8 py-4">
                  Join Beta Program
                  <ArrowRight className="ml-2 h-5 w-5" />
                </Link>
                <a href="#features" className="btn-secondary text-lg px-8 py-4">
                  Explore Features
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
                    <span className="text-primary-400 font-mono text-sm">Wifi Wolverine v1.0</span>
                    <div className="flex space-x-2">
                      <div className="w-3 h-3 bg-green-500 rounded-full animate-pulse"></div>
                      <span className="text-green-400 text-xs">SCANNING</span>
                    </div>
                  </div>
                  <div className="bg-dark-900 rounded-lg p-4 font-mono text-sm">
                    <div className="text-green-400">📡 [WIFI] 15 networks discovered</div>
                    <div className="text-blue-400">🔵 [BLE] 8 Bluetooth devices found</div>
                    <div className="text-yellow-400">🌐 [IOT] 12 IoT devices detected</div>
                    <div className="text-red-400">⚠️ [VULN] 3 vulnerabilities identified</div>
                    <div className="text-purple-400">📊 [RF] Spectrum analysis complete</div>
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
              <span className="gradient-text">Next-Generation</span> Wireless Testing
            </h2>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto">
              Advanced wireless security assessment capabilities for modern enterprise environments
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

      {/* Capabilities Section */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-12 items-center">
            <motion.div
              initial={{ opacity: 0, x: -30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6 }}
            >
              <h2 className="text-4xl font-bold mb-6">
                <span className="gradient-text">Comprehensive</span> Testing Capabilities
              </h2>
              <p className="text-lg text-gray-300 mb-8">
                Wifi Wolverine provides the most advanced wireless security testing capabilities 
                available, designed for professional penetration testers and security assessors.
              </p>
              
              <ul className="space-y-3">
                {capabilities.map((capability, index) => (
                  <motion.li
                    key={index}
                    initial={{ opacity: 0, x: -20 }}
                    whileInView={{ opacity: 1, x: 0 }}
                    transition={{ duration: 0.4, delay: index * 0.1 }}
                    className="flex items-center space-x-3"
                  >
                    <div className="w-2 h-2 bg-primary-500 rounded-full"></div>
                    <span className="text-gray-300">{capability}</span>
                  </motion.li>
                ))}
              </ul>
            </motion.div>

            <motion.div
              initial={{ opacity: 0, x: 30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6, delay: 0.2 }}
              className="relative"
            >
              <div className="bg-dark-800 rounded-2xl p-8 border border-dark-700">
                <h3 className="text-xl font-semibold text-white mb-6">Technical Specifications</h3>
                <div className="space-y-4 text-sm">
                  <div className="flex justify-between">
                    <span className="text-gray-400">WiFi Standards</span>
                    <span className="text-white">802.11a/b/g/n/ac/ax (WiFi 6)</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-gray-400">Bluetooth</span>
                    <span className="text-white">Classic, BLE, Bluetooth 5.0+</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-gray-400">IoT Protocols</span>
                    <span className="text-white">Zigbee, Z-Wave, LoRa, NFC</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-gray-400">RF Range</span>
                    <span className="text-white">2.4GHz, 5GHz, Sub-GHz</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-gray-400">Platform</span>
                    <span className="text-white">Windows, macOS, Linux</span>
                  </div>
                </div>
              </div>
            </motion.div>
          </div>
        </div>
      </section>

      {/* CTA Section */}
      <section className="py-20 bg-gradient-to-r from-primary-900/20 to-dark-800/40">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
          >
            <h2 className="text-4xl font-bold mb-6">
              Ready for Advanced Wireless Testing?
            </h2>
            <p className="text-xl text-gray-300 mb-8">
              Join our beta program and be among the first to experience the future of wireless security testing.
            </p>
            <Link to="/contact" className="btn-primary text-lg px-8 py-4">
              Request Beta Access
            </Link>
          </motion.div>
        </div>
      </section>
    </div>
  )
}

export default WifiWolverine