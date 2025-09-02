import React from 'react'
import { Link } from 'react-router-dom'
import { motion } from 'framer-motion'
import { Shield, Zap, Lock, Users, ArrowRight, CheckCircle, Star } from 'lucide-react'

const Home = () => {
  const features = [
    {
      icon: Shield,
      title: 'Advanced Threat Detection',
      description: 'AI-powered threat intelligence and real-time security monitoring'
    },
    {
      icon: Zap,
      title: 'Automated Response',
      description: 'SOAR automation for rapid incident response and threat mitigation'
    },
    {
      icon: Lock,
      title: 'Penetration Testing',
      description: 'Comprehensive wireless and network security testing tools'
    },
    {
      icon: Users,
      title: 'Team Collaboration',
      description: 'Unified platform for security operations center teams'
    }
  ]

  const stats = [
    { number: '500+', label: 'Security Professionals' },
    { number: '99.9%', label: 'Uptime Guarantee' },
    { number: '24/7', label: 'Enterprise Support' },
    { number: '50+', label: 'Integrations' }
  ]

  return (
    <div className="pt-16">
      {/* Hero Section */}
      <section className="relative min-h-screen flex items-center justify-center overflow-hidden">
        <div className="absolute inset-0 cyber-grid opacity-20"></div>
        <div className="absolute inset-0 bg-gradient-to-br from-primary-900/30 to-slate-900/60"></div>
        
        <div className="relative z-10 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.8 }}
          >
            <h1 className="text-5xl md:text-7xl font-bold mb-6">
              <span className="gradient-text">Advanced</span>
              <br />
              <span className="text-white">Cybersecurity</span>
              <br />
              <span className="gradient-text">Solutions</span>
            </h1>
            
            <p className="text-xl md:text-2xl text-slate-300 mb-8 max-w-3xl mx-auto">
              Empowering security professionals with cutting-edge threat detection, 
              automated response, and comprehensive penetration testing capabilities.
            </p>
            
            <div className="flex flex-col sm:flex-row gap-4 justify-center items-center">
              <Link to="/products/cyberrecon" className="btn-primary text-lg px-8 py-4">
                Explore CyberRecon Suite
                <ArrowRight className="ml-2 h-5 w-5" />
              </Link>
              <Link to="/contact" className="btn-secondary text-lg px-8 py-4">
                Schedule Demo
              </Link>
            </div>
          </motion.div>
        </div>

        {/* Floating Elements */}
        <div className="absolute top-20 left-10 animate-float">
          <div className="w-16 h-16 bg-primary-500/30 rounded-full blur-xl"></div>
        </div>
        <div className="absolute bottom-20 right-10 animate-float" style={{ animationDelay: '1s' }}>
          <div className="w-24 h-24 bg-primary-400/30 rounded-full blur-xl"></div>
        </div>
      </section>

      {/* Features Section */}
      <section className="py-20 bg-slate-800/30">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl md:text-5xl font-bold mb-4">
              <span className="gradient-text">Enterprise-Grade</span> Security Platform
            </h2>
            <p className="text-xl text-slate-300 max-w-3xl mx-auto">
              Comprehensive cybersecurity operations platform designed for modern security teams
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8">
            {features.map((feature, index) => (
              <motion.div
                key={feature.title}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="card group hover:glow-effect"
              >
                <div className="mb-4">
                  <feature.icon className="h-12 w-12 text-primary-500 group-hover:text-primary-400 transition-colors" />
                </div>
                <h3 className="text-xl font-semibold text-white mb-2">{feature.title}</h3>
                <p className="text-slate-400">{feature.description}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Stats Section */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-2 md:grid-cols-4 gap-8">
            {stats.map((stat, index) => (
              <motion.div
                key={stat.label}
                initial={{ opacity: 0, scale: 0.5 }}
                whileInView={{ opacity: 1, scale: 1 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="text-center"
              >
                <div className="text-4xl md:text-5xl font-bold gradient-text mb-2">
                  {stat.number}
                </div>
                <div className="text-slate-400 font-medium">{stat.label}</div>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* CTA Section */}
      <section className="py-20 bg-gradient-to-r from-primary-900/30 to-slate-800/50">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
          >
            <h2 className="text-4xl md:text-5xl font-bold mb-6">
              Ready to Secure Your Organization?
            </h2>
            <p className="text-xl text-slate-300 mb-8">
              Join hundreds of security professionals using CyberRecon Suite for advanced threat detection and response.
            </p>
            <div className="flex flex-col sm:flex-row gap-4 justify-center">
              <Link to="/products/cyberrecon" className="btn-primary text-lg px-8 py-4">
                Start Free Trial
              </Link>
              <Link to="/contact" className="btn-secondary text-lg px-8 py-4">
                Contact Sales
              </Link>
            </div>
          </motion.div>
        </div>
      </section>
    </div>
  )
}

export default Home