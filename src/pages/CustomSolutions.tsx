import React from 'react'
import { motion } from 'framer-motion'
import { Zap, Code, Database, Cloud, Shield, Users, ArrowRight, CheckCircle } from 'lucide-react'
import { Link } from 'react-router-dom'

const CustomSolutions = () => {
  const services = [
    {
      icon: Code,
      title: 'Custom SIEM Development',
      description: 'Tailored security information and event management systems designed for your specific infrastructure and requirements.',
      features: ['Custom data connectors', 'Bespoke dashboards', 'Advanced correlation rules', 'Integration APIs']
    },
    {
      icon: Database,
      title: 'Security Data Analytics',
      description: 'Advanced analytics platforms for security data processing, threat hunting, and compliance reporting.',
      features: ['Big data processing', 'ML/AI integration', 'Custom visualizations', 'Automated reporting']
    },
    {
      icon: Cloud,
      title: 'Cloud Security Solutions',
      description: 'Cloud-native security platforms and integrations for AWS, Azure, GCP, and hybrid environments.',
      features: ['Multi-cloud monitoring', 'Container security', 'Serverless protection', 'DevSecOps integration']
    },
    {
      icon: Shield,
      title: 'Compliance Automation',
      description: 'Automated compliance monitoring and reporting systems for SOC 2, GDPR, HIPAA, and industry standards.',
      features: ['Automated auditing', 'Evidence collection', 'Risk assessment', 'Compliance dashboards']
    },
    {
      icon: Users,
      title: 'Security Orchestration',
      description: 'Custom SOAR platforms and workflow automation for incident response and security operations.',
      features: ['Workflow automation', 'Playbook development', 'Integration platform', 'Response coordination']
    },
    {
      icon: Zap,
      title: 'API Development',
      description: 'RESTful APIs and integrations for connecting security tools and enabling automation workflows.',
      features: ['REST/GraphQL APIs', 'Webhook integrations', 'Real-time streaming', 'Authentication systems']
    }
  ]

  const process = [
    {
      step: '01',
      title: 'Discovery & Analysis',
      description: 'We analyze your current security infrastructure, identify gaps, and understand your specific requirements.'
    },
    {
      step: '02',
      title: 'Architecture & Design',
      description: 'Our team designs a custom solution architecture that integrates seamlessly with your existing systems.'
    },
    {
      step: '03',
      title: 'Development & Testing',
      description: 'Agile development process with continuous testing, security reviews, and client feedback integration.'
    },
    {
      step: '04',
      title: 'Deployment & Training',
      description: 'Professional deployment with comprehensive training and documentation for your security team.'
    }
  ]

  return (
    <div className="pt-16">
      {/* Hero Section */}
      <section className="relative py-20 overflow-hidden">
        <div className="absolute inset-0 cyber-grid opacity-10"></div>
        <div className="relative z-10 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="text-center">
            <motion.div
              initial={{ opacity: 0, y: 30 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.8 }}
            >
              <div className="flex items-center justify-center space-x-2 mb-4">
                <img src="/shield-icon.svg" alt="BulletDrop Studios" className="h-8 w-8" />
                <span className="text-primary-400 font-semibold">Custom Security Solutions</span>
              </div>
              
              <h1 className="text-5xl md:text-6xl font-bold mb-6">
                <span className="gradient-text">Tailored</span>
                <br />
                <span className="text-white">Cybersecurity</span>
                <br />
                <span className="gradient-text">Development</span>
              </h1>
              
              <p className="text-xl text-gray-300 mb-8 max-w-3xl mx-auto leading-relaxed">
                Custom-built cybersecurity applications and integrations designed specifically 
                for your organization's unique security requirements and infrastructure.
              </p>
              
              <div className="flex flex-col sm:flex-row gap-4 justify-center">
                <Link to="/contact" className="btn-primary text-lg px-8 py-4">
                  Start Your Project
                  <ArrowRight className="ml-2 h-5 w-5" />
                </Link>
                <a href="#services" className="btn-secondary text-lg px-8 py-4">
                  View Services
                </a>
              </div>
            </motion.div>
          </div>
        </div>
      </section>

      {/* Services Section */}
      <section id="services" className="py-20 bg-dark-800/50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Custom Development</span> Services
            </h2>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto">
              Specialized cybersecurity solutions built to your exact specifications
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
            {services.map((service, index) => (
              <motion.div
                key={service.title}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="card group"
              >
                <div className="mb-4">
                  <service.icon className="h-12 w-12 text-primary-500 group-hover:text-primary-400 transition-colors" />
                </div>
                <h3 className="text-xl font-semibold text-white mb-3">{service.title}</h3>
                <p className="text-gray-400 mb-4 leading-relaxed">{service.description}</p>
                
                <ul className="space-y-2">
                  {service.features.map((feature, idx) => (
                    <li key={idx} className="flex items-center space-x-2 text-sm">
                      <CheckCircle className="h-4 w-4 text-primary-500 flex-shrink-0" />
                      <span className="text-gray-300">{feature}</span>
                    </li>
                  ))}
                </ul>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Process Section */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Our Development</span> Process
            </h2>
            <p className="text-xl text-gray-300">
              Proven methodology for delivering successful cybersecurity solutions
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8">
            {process.map((step, index) => (
              <motion.div
                key={step.step}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="text-center"
              >
                <div className="w-16 h-16 bg-primary-500/20 rounded-full flex items-center justify-center mx-auto mb-4">
                  <span className="text-2xl font-bold text-primary-500">{step.step}</span>
                </div>
                <h3 className="text-xl font-semibold text-white mb-3">{step.title}</h3>
                <p className="text-gray-400 leading-relaxed">{step.description}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Pricing Section */}
      <section className="py-20 bg-dark-800/50">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8 text-center">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
          >
            <h2 className="text-4xl font-bold mb-6">
              <span className="gradient-text">Investment</span> & Pricing
            </h2>
            <p className="text-xl text-gray-300 mb-8">
              Custom development projects are priced based on scope, complexity, and timeline
            </p>

            <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
              <div className="card">
                <h3 className="text-lg font-semibold text-white mb-2">Consultation</h3>
                <div className="text-3xl font-bold text-primary-400 mb-2">Free</div>
                <p className="text-gray-400 text-sm">Initial assessment and project scoping</p>
              </div>
              
              <div className="card ring-2 ring-primary-500">
                <h3 className="text-lg font-semibold text-white mb-2">Development</h3>
                <div className="text-3xl font-bold text-primary-400 mb-2">$5K+</div>
                <p className="text-gray-400 text-sm">Starting price for custom solutions</p>
              </div>
              
              <div className="card">
                <h3 className="text-lg font-semibold text-white mb-2">Enterprise</h3>
                <div className="text-3xl font-bold text-primary-400 mb-2">$50K+</div>
                <p className="text-gray-400 text-sm">Large-scale enterprise platforms</p>
              </div>
            </div>

            <div className="mt-8">
              <Link to="/contact" className="btn-primary text-lg px-8 py-4">
                Schedule Consultation
              </Link>
            </div>
          </motion.div>
        </div>
      </section>
    </div>
  )
}

export default CustomSolutions