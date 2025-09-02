import React from 'react'
import { Link } from 'react-router-dom'
import { motion } from 'framer-motion'
import { Shield, Zap, Eye, Network, Wifi, Lock, ArrowRight, CheckCircle } from 'lucide-react'

const Products = () => {
  const products = [
    {
      id: 'cyberrecon',
      name: 'CyberRecon Suite',
      tagline: 'Advanced Cybersecurity Operations Platform',
      description: 'Comprehensive security monitoring, threat intelligence, and automated response platform for enterprise security operations centers.',
      icon: Shield,
      features: [
        'Real-time SIEM & EDR monitoring',
        'Advanced threat intelligence',
        'SOAR automation & orchestration',
        'Wireless penetration testing',
        'Network monitoring & analysis',
        'Digital forensics tools',
        'Compliance dashboard',
        'Team collaboration platform'
      ],
      pricing: {
        individual: '$25/month',
        professional: '$99/month',
        enterprise: '$299/month'
      },
      status: 'Available Now',
      link: '/products/cyberrecon'
    },
    {
      id: 'cyberranger',
      name: 'CyberRanger',
      tagline: 'Mobile Cybersecurity Command Center',
      description: 'Next-generation mobile cybersecurity platform for security professionals on the go. Monitor, analyze, and respond to threats from anywhere.',
      icon: Eye,
      features: [
        'Mobile SIEM dashboard',
        'Real-time threat alerts',
        'Remote incident response',
        'Secure team communication',
        'Offline forensics tools',
        'GPS-based security mapping',
        'Biometric authentication',
        'Emergency response protocols'
      ],
      pricing: {
        professional: 'Coming Soon',
        enterprise: 'Coming Soon'
      },
      status: 'Coming Q2 2024',
      link: '/products/cyberranger'
    },
    {
      id: 'wifi-wolverine',
      name: 'Wifi Wolverine',
      tagline: 'Advanced Wireless Security Testing',
      description: 'Specialized wireless penetration testing platform with advanced WiFi, Bluetooth, and IoT security assessment capabilities.',
      icon: Wifi,
      features: [
        'Advanced WiFi penetration testing',
        'Bluetooth security assessment',
        'IoT device vulnerability scanning',
        'Wireless protocol analysis',
        'RF spectrum monitoring',
        'Rogue access point detection',
        'Wireless forensics tools',
        'Compliance reporting'
      ],
      pricing: {
        professional: 'Coming Soon',
        enterprise: 'Coming Soon'
      },
      status: 'Coming Q3 2024',
      link: '/products/wifi-wolverine'
    },
    {
      id: 'custom',
      name: 'Custom Security Solutions',
      tagline: 'Tailored Cybersecurity Development',
      description: 'Custom-built cybersecurity applications and integrations designed specifically for your organization\'s unique security requirements.',
      icon: Zap,
      features: [
        'Custom SIEM integrations',
        'Bespoke security dashboards',
        'API development & integration',
        'Security automation workflows',
        'Compliance reporting tools',
        'Threat intelligence platforms',
        'Mobile security applications',
        'Cloud security solutions'
      ],
      pricing: {
        consultation: 'Contact for quote',
        development: 'Starting at $5,000'
      },
      status: 'Available Now',
      link: '/products/custom'
    }
  ]

  return (
    <div className="pt-16">
      {/* Hero Section */}
      <section className="py-20 bg-gradient-to-br from-primary-900/20 to-dark-800/40">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.8 }}
            className="text-center"
          >
            <h1 className="text-5xl md:text-6xl font-bold mb-6">
              <span className="gradient-text">Cybersecurity</span>
              <br />
              <span className="text-white">Products & Solutions</span>
            </h1>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto">
              Professional-grade cybersecurity tools and custom solutions designed for 
              modern security operations centers and enterprise environments.
            </p>
          </motion.div>
        </div>
      </section>

      {/* Products Grid */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-12">
            {products.map((product, index) => (
              <motion.div
                key={product.id}
                initial={{ opacity: 0, x: index % 2 === 0 ? -30 : 30 }}
                whileInView={{ opacity: 1, x: 0 }}
                transition={{ duration: 0.6, delay: index * 0.2 }}
                className="card group hover:glow-effect"
              >
                <div className="flex items-center space-x-4 mb-6">
                  <div className="p-3 bg-primary-500/20 rounded-lg group-hover:bg-primary-500/30 transition-colors">
                    <product.icon className="h-8 w-8 text-primary-500" />
                  </div>
                  <div>
                    <h3 className="text-2xl font-bold text-white">{product.name}</h3>
                    <p className="text-primary-400 font-medium">{product.tagline}</p>
                  </div>
                </div>

                <p className="text-gray-300 mb-6 leading-relaxed">
                  {product.description}
                </p>

                <div className="mb-6">
                  <h4 className="text-lg font-semibold text-white mb-3">Key Features:</h4>
                  <ul className="space-y-2">
                    {product.features.map((feature, idx) => (
                      <li key={idx} className="flex items-center space-x-2 text-gray-300">
                        <CheckCircle className="h-4 w-4 text-primary-500 flex-shrink-0" />
                        <span>{feature}</span>
                      </li>
                    ))}
                  </ul>
                </div>

                <div className="border-t border-dark-700 pt-6">
                  <div className="flex justify-between items-center mb-4">
                    <div>
                      <p className="text-sm text-gray-400">{product.status}</p>
                      <p className="text-sm text-gray-400">Starting at</p>
                      <p className="text-2xl font-bold text-primary-400">
                        {Object.values(product.pricing)[0]}
                      </p>
                    </div>
                    <Link
                      to={product.link}
                      className={`group ${product.status.includes('Coming') ? 'btn-secondary' : 'btn-primary'}`}
                    >
                      {product.status.includes('Coming') ? 'Learn More' : 'Get Started'}
                      <ArrowRight className="ml-2 h-4 w-4 group-hover:translate-x-1 transition-transform" />
                    </Link>
                  </div>
                </div>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Technology Stack */}
      <section className="py-20 bg-dark-800/50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Built with</span> Modern Technology
            </h2>
            <p className="text-xl text-gray-300">
              Enterprise-grade architecture using industry-leading technologies
            </p>
          </motion.div>

          <div className="grid grid-cols-2 md:grid-cols-4 gap-8">
            {[
              { name: 'Qt6 C++', description: 'Cross-platform framework' },
              { name: 'SQLite', description: 'Embedded database' },
              { name: 'Stripe', description: 'Payment processing' },
              { name: 'OpenSSL', description: 'Encryption & security' },
              { name: 'CMake', description: 'Build automation' },
              { name: 'REST APIs', description: 'Integration ready' },
              { name: 'Real-time', description: 'Live monitoring' },
              { name: 'Cross-platform', description: 'Windows, macOS, Linux' }
            ].map((tech, index) => (
              <motion.div
                key={tech.name}
                initial={{ opacity: 0, y: 20 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.4, delay: index * 0.1 }}
                className="text-center p-4 rounded-lg bg-dark-700/50 hover:bg-dark-700 transition-colors"
              >
                <h4 className="font-semibold text-white mb-1">{tech.name}</h4>
                <p className="text-sm text-gray-400">{tech.description}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* CTA Section */}
      <section className="py-20">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
          >
            <h2 className="text-4xl font-bold mb-6">
              Ready to Enhance Your Security Operations?
            </h2>
            <p className="text-xl text-gray-300 mb-8">
              Contact our team to discuss your cybersecurity requirements and explore custom solutions.
            </p>
            <Link to="/contact" className="btn-primary text-lg px-8 py-4">
              Get Started Today
            </Link>
          </motion.div>
        </div>
      </section>
    </div>
  )
}

export default Products