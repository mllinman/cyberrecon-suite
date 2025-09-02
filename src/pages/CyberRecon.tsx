import React, { useState } from 'react'
import { motion } from 'framer-motion'
import { Shield, Monitor, Wifi, Network, Brain, Users, Download, CheckCircle, Star, ArrowRight } from 'lucide-react'
import SubscriptionModal from '../components/SubscriptionModal'
import { useAuth } from '../contexts/AuthContext'

const CyberRecon = () => {
  const [showSubscription, setShowSubscription] = useState(false)
  const [selectedPlan, setSelectedPlan] = useState<'professional' | 'enterprise'>('professional')
  const { user } = useAuth()

  const features = [
    {
      icon: Monitor,
      title: 'SIEM & EDR Monitoring',
      description: 'Real-time security information and event management with endpoint detection and response capabilities.'
    },
    {
      icon: Brain,
      title: 'AI Threat Intelligence',
      description: 'Machine learning-powered threat detection and correlation with global intelligence feeds.'
    },
    {
      icon: Wifi,
      title: 'Wireless Penetration Testing',
      description: 'Comprehensive WiFi and Bluetooth security testing tools for authorized penetration testing.'
    },
    {
      icon: Network,
      title: 'Network Analysis',
      description: 'Advanced packet capture and network scanning capabilities for security assessment.'
    },
    {
      icon: Shield,
      title: 'SOAR Automation',
      description: 'Security orchestration and automated response workflows for rapid incident handling.'
    },
    {
      icon: Users,
      title: 'Team Collaboration',
      description: 'Unified platform for security operations center teams with real-time communication.'
    }
  ]

  const plans = [
    {
      name: 'Individual',
      price: '$25',
      period: 'per month',
      description: 'Perfect for individual security professionals',
      features: [
        'Basic SIEM & EDR monitoring',
        'Limited threat intelligence feeds',
        'Up to 10,000 events/month',
        'Basic dashboards and reporting',
        'Email support',
        'Single user license',
        'Community forum access'
      ],
      popular: false,
      trial: '7-day Professional trial included'
    },
    {
      name: 'Individual',
      price: '$25',
      period: 'per month',
      description: 'Perfect for individual security professionals',
      features: [
        'Basic SIEM & EDR monitoring',
        'Limited threat intelligence feeds',
        'Up to 10,000 events/month',
        'Basic dashboards and reporting',
        'Email support',
        'Single user license',
        'Community forum access'
      ],
      popular: false,
      trial: '7-day free trial with Professional features'
    },
    {
      name: 'Professional',
      price: '$99',
      period: 'per month',
      description: 'Perfect for small to medium security teams',
      features: [
        'Full SIEM & EDR monitoring',
        'Complete threat intelligence feeds',
        'Up to 100,000 events/month',
        'Penetration testing tools (WiFi, Network)',
        'Advanced firewall protection',
        'Email support',
        'Team collaboration (up to 10 users)',
        'Compliance dashboard'
      ],
      popular: true
    },
    {
      name: 'Enterprise',
      price: '$299',
      period: 'per month',
      description: 'Advanced features for large organizations',
      features: [
        'All Professional features',
        'Advanced ML correlation',
        'Unlimited events processing',
        'Full penetration testing suite',
        'Enterprise firewall with custom rules',
        'Enterprise firewall with custom rules',
        'Custom integrations',
        '24/7 phone support',
        'Dedicated account manager'
      ],
      popular: false
    }
  ]

  const handleSubscribe = (plan: 'professional' | 'enterprise') => {
    setSelectedPlan(plan)
    setShowSubscription(true)
  }

  const handleDownload = () => {
    if (user && user.subscriptionStatus === 'active') {
      // Simulate download
      const link = document.createElement('a')
      link.href = '/downloads/CyberReconSuite-v1.7.zip'
      link.download = 'CyberReconSuite-v1.7.zip'
      link.click()
    } else {
      setShowSubscription(true)
    }
  }

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
                <span className="text-primary-400 font-semibold">CyberRecon Suite v1.7</span>
              </div>
              
              <h1 className="text-5xl md:text-6xl font-bold mb-6">
                <span className="gradient-text">Advanced</span>
                <br />
                <span className="text-white">Cybersecurity</span>
                <br />
                <span className="gradient-text">Operations</span>
              </h1>
              
              <p className="text-xl text-gray-300 mb-8 leading-relaxed">
                Professional-grade security operations platform featuring real-time monitoring, 
                threat intelligence, automated response, and comprehensive penetration testing tools.
              </p>
              
              <div className="flex flex-col sm:flex-row gap-4">
                {user && user.subscriptionStatus === 'active' ? (
                  <button onClick={handleDownload} className="btn-primary text-lg px-8 py-4">
                    <Download className="mr-2 h-5 w-5" />
                    Download Now
                  </button>
                ) : (
                  <button onClick={() => handleSubscribe('professional')} className="btn-primary text-lg px-8 py-4">
                    Start Free Trial
                  </button>
                )}
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
                    <span className="text-primary-400 font-mono text-sm">CyberRecon Suite v1.7</span>
                    <div className="flex space-x-2">
                      <div className="w-3 h-3 bg-red-500 rounded-full"></div>
                      <div className="w-3 h-3 bg-yellow-500 rounded-full"></div>
                      <div className="w-3 h-3 bg-green-500 rounded-full"></div>
                    </div>
                  </div>
                  <div className="bg-dark-900 rounded-lg p-4 font-mono text-sm">
                    <div className="text-green-400">[SIEM] Real-time monitoring active</div>
                    <div className="text-blue-400">[EDR] 247 endpoints protected</div>
                    <div className="text-yellow-400">[INTEL] Threat feeds updated</div>
                    <div className="text-red-400">[ALERT] 3 critical events detected</div>
                    <div className="text-purple-400">[SOAR] Automated response triggered</div>
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
              <span className="gradient-text">Comprehensive</span> Security Platform
            </h2>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto">
              Everything you need for modern cybersecurity operations in one integrated platform
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

      {/* Pricing Section */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Flexible</span> Pricing Plans
            </h2>
            <p className="text-xl text-gray-300">
              Choose the plan that fits your organization's security needs
            </p>
          </motion.div>

          <div className="grid grid-cols-1 lg:grid-cols-2 gap-8 max-w-5xl mx-auto">
            {plans.map((plan, index) => (
              <motion.div
                key={plan.name}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.2 }}
                className={`card relative ${plan.popular ? 'ring-2 ring-primary-500 glow-effect' : ''}`}
              >
                {plan.popular && (
                  <div className="absolute -top-4 left-1/2 transform -translate-x-1/2">
                    <span className="bg-primary-500 text-white px-4 py-1 rounded-full text-sm font-semibold">
                      Most Popular
                    </span>
                  </div>
                )}

                <div className="text-center mb-6">
                  <h3 className="text-2xl font-bold text-white mb-2">{plan.name}</h3>
                  <p className="text-gray-400 mb-4">{plan.description}</p>
                  <div className="flex items-baseline justify-center">
                    <span className="text-4xl font-bold text-primary-400">{plan.price}</span>
                    <span className="text-gray-400 ml-2">{plan.period}</span>
                  </div>
                </div>

                <ul className="space-y-3 mb-8">
                  {plan.features.map((feature, idx) => (
                    <li key={idx} className="flex items-center space-x-3">
                      <CheckCircle className="h-5 w-5 text-primary-500 flex-shrink-0" />
                      <span className="text-gray-300">{feature}</span>
                    </li>
                  ))}
                </ul>

                <button
                  onClick={() => handleSubscribe(plan.name.toLowerCase() as 'professional' | 'enterprise')}
                  className={`w-full py-3 px-6 rounded-lg font-semibold transition-all duration-200 ${
                    plan.popular 
                      ? 'btn-primary' 
                      : 'bg-dark-700 text-white hover:bg-dark-600'
                  }`}
                >
                  {user && user.subscriptionStatus === 'active' ? 'Download' : 'Subscribe Now'}
                </button>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Subscription Modal */}
      {showSubscription && (
        <SubscriptionModal
          plan={selectedPlan}
          onClose={() => setShowSubscription(false)}
        />
      )}
    </div>
  )
}

export default CyberRecon