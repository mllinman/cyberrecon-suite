import React, { useState } from 'react'
import { motion } from 'framer-motion'
import { Shield, Check, Star, CreditCard, Lock, ArrowRight, Users, Building, Zap } from 'lucide-react'
import { Link } from 'react-router-dom'
import SubscriptionModal from '../components/SubscriptionModal'

const Subscribe = () => {
  const [selectedPlan, setSelectedPlan] = useState<'individual' | 'professional' | 'enterprise' | null>(null)
  const [billingCycle, setBillingCycle] = useState<'monthly' | 'yearly'>('monthly')

  const plans = {
    individual: {
      name: 'Individual',
      price: { monthly: 25, yearly: 250 },
      description: 'Perfect for individual security professionals',
      popular: false,
      icon: Shield,
      features: [
        'Basic SIEM & EDR monitoring',
        'Limited threat intelligence feeds',
        'Up to 10,000 events/month',
        'Basic dashboards and reporting',
        'Email support',
        'Single user license',
        'Community forum access'
      ],
      limitations: [
        'No advanced ML correlation',
        'Limited integrations',
        'No phone support'
      ]
    },
    professional: {
      name: 'Professional',
      price: { monthly: 99, yearly: 990 },
      description: 'Perfect for small to medium security teams',
      popular: true,
      icon: Users,
      features: [
        'Full SIEM & EDR monitoring',
        'Complete threat intelligence feeds',
        'Up to 100,000 events/month',
        'Penetration testing tools (WiFi, Network)',
        'Advanced firewall protection',
        'Email support',
        'Team collaboration (up to 10 users)',
        'Compliance dashboard',
        'Advanced analytics',
        'Custom integrations (5 included)'
      ],
      limitations: [
        'Limited ML correlation',
        'No dedicated account manager'
      ]
    },
    enterprise: {
      name: 'Enterprise',
      price: { monthly: 299, yearly: 2990 },
      description: 'Advanced features for large organizations',
      popular: false,
      icon: Building,
      features: [
        'All Professional features',
        'Advanced ML correlation',
        'Unlimited events processing',
        'Full penetration testing suite',
        'Enterprise firewall with custom rules',
        'Custom integrations (unlimited)',
        '24/7 phone support',
        'Dedicated account manager',
        'Priority feature requests',
        'Custom SLA agreements',
        'On-premise deployment options',
        'Advanced compliance reporting'
      ],
      limitations: []
    }
  }

  const testimonials = [
    {
      name: 'Sarah Johnson',
      role: 'CISO, TechCorp Inc.',
      image: 'https://images.pexels.com/photos/3769021/pexels-photo-3769021.jpeg?auto=compress&cs=tinysrgb&w=400',
      content: 'CyberRecon Suite has transformed our security operations. The real-time threat detection and automated response capabilities have reduced our incident response time by 80%.'
    },
    {
      name: 'Mike Chen',
      role: 'Security Analyst, DataSafe Solutions',
      image: 'https://images.pexels.com/photos/2379004/pexels-photo-2379004.jpeg?auto=compress&cs=tinysrgb&w=400',
      content: 'The penetration testing tools are incredibly comprehensive. We\'ve identified vulnerabilities in our wireless infrastructure that other tools completely missed.'
    },
    {
      name: 'Emily Rodriguez',
      role: 'SOC Manager, SecureNet',
      image: 'https://images.pexels.com/photos/3783725/pexels-photo-3783725.jpeg?auto=compress&cs=tinysrgb&w=400',
      content: 'The team collaboration features and real-time dashboards have greatly improved our SOC efficiency. Highly recommended for any security team.'
    }
  ]

  const faqs = [
    {
      question: 'Can I upgrade or downgrade my plan anytime?',
      answer: 'Yes, you can upgrade or downgrade your subscription at any time. Changes will be prorated and reflected in your next billing cycle.'
    },
    {
      question: 'Is there a free trial available?',
      answer: 'Yes, we offer a 14-day free trial for all plans. No credit card required to start your trial.'
    },
    {
      question: 'What payment methods do you accept?',
      answer: 'We accept all major credit cards (Visa, MasterCard, American Express), PayPal, and bank transfers for enterprise customers.'
    },
    {
      question: 'Is my data secure?',
      answer: 'Absolutely. We use enterprise-grade encryption, SOC 2 compliant infrastructure, and follow industry best practices for data security and privacy.'
    },
    {
      question: 'Do you offer custom enterprise solutions?',
      answer: 'Yes, we offer custom development and enterprise solutions. Contact our sales team to discuss your specific requirements.'
    }
  ]

  const getDiscountedPrice = (price: number) => {
    return billingCycle === 'yearly' ? Math.round(price * 0.83) : price
  }

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
              <span className="text-white">Choose Your</span>
              <br />
              <span className="gradient-text">Security Plan</span>
            </h1>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto mb-8">
              Start securing your organization with CyberRecon Suite. Choose the plan that fits your needs and scale as you grow.
            </p>

            {/* Billing Toggle */}
            <div className="flex items-center justify-center space-x-4 mb-8">
              <span className={`text-sm ${billingCycle === 'monthly' ? 'text-white' : 'text-gray-400'}`}>
                Monthly
              </span>
              <button
                onClick={() => setBillingCycle(billingCycle === 'monthly' ? 'yearly' : 'monthly')}
                className={`relative w-16 h-8 rounded-full transition-colors ${
                  billingCycle === 'yearly' ? 'bg-primary-500' : 'bg-gray-600'
                }`}
              >
                <div
                  className={`absolute top-1 w-6 h-6 bg-white rounded-full transition-transform ${
                    billingCycle === 'yearly' ? 'translate-x-9' : 'translate-x-1'
                  }`}
                />
              </button>
              <span className={`text-sm ${billingCycle === 'yearly' ? 'text-white' : 'text-gray-400'}`}>
                Yearly
                <span className="text-primary-400 ml-1">(Save 17%)</span>
              </span>
            </div>
          </motion.div>
        </div>
      </section>

      {/* Pricing Plans */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
            {Object.entries(plans).map(([planKey, plan], index) => (
              <motion.div
                key={planKey}
                initial={{ opacity: 0, y: 30 }}
                animate={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.2 }}
                className={`relative card group ${plan.popular ? 'ring-2 ring-primary-500 glow-effect' : ''}`}
              >
                {plan.popular && (
                  <div className="absolute -top-4 left-1/2 transform -translate-x-1/2">
                    <div className="bg-primary-500 text-white px-4 py-1 rounded-full text-sm font-medium">
                      Most Popular
                    </div>
                  </div>
                )}

                <div className="text-center mb-8">
                  <plan.icon className="h-12 w-12 text-primary-500 mx-auto mb-4" />
                  <h3 className="text-2xl font-bold text-white mb-2">{plan.name}</h3>
                  <p className="text-gray-400 mb-4">{plan.description}</p>
                  
                  <div className="mb-4">
                    <span className="text-4xl font-bold text-white">
                      ${getDiscountedPrice(plan.price[billingCycle])}
                    </span>
                    <span className="text-gray-400 ml-1">
                      /{billingCycle === 'monthly' ? 'month' : 'year'}
                    </span>
                    {billingCycle === 'yearly' && (
                      <div className="text-sm text-primary-400 mt-1">
                        Save ${plan.price[billingCycle] - getDiscountedPrice(plan.price[billingCycle])} per year
                      </div>
                    )}
                  </div>
                </div>

                <ul className="space-y-3 mb-8">
                  {plan.features.map((feature, idx) => (
                    <li key={idx} className="flex items-start space-x-3">
                      <Check className="h-5 w-5 text-primary-500 mt-0.5 flex-shrink-0" />
                      <span className="text-gray-300 text-sm">{feature}</span>
                    </li>
                  ))}
                </ul>

                {plan.limitations.length > 0 && (
                  <div className="mb-8">
                    <h4 className="text-sm font-medium text-gray-400 mb-2">Not included:</h4>
                    <ul className="space-y-1">
                      {plan.limitations.map((limitation, idx) => (
                        <li key={idx} className="text-xs text-gray-500 flex items-center space-x-2">
                          <div className="w-1 h-1 bg-gray-500 rounded-full"></div>
                          <span>{limitation}</span>
                        </li>
                      ))}
                    </ul>
                  </div>
                )}

                <button
                  onClick={() => setSelectedPlan(planKey as any)}
                  className={`w-full py-3 px-6 rounded-lg font-medium transition-colors ${
                    plan.popular
                      ? 'btn-primary'
                      : 'bg-dark-700 text-white hover:bg-dark-600 border border-dark-600'
                  }`}
                >
                  Get Started
                  <ArrowRight className="ml-2 h-4 w-4 inline" />
                </button>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Features Comparison */}
      <section className="py-20 bg-dark-800/50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Everything</span> You Need
            </h2>
            <p className="text-xl text-gray-300">
              Comprehensive cybersecurity tools and features across all plans
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
            {[
              {
                icon: Shield,
                title: 'Advanced Threat Detection',
                description: 'AI-powered threat intelligence with real-time monitoring and automated correlation.'
              },
              {
                icon: Zap,
                title: 'Automated Response',
                description: 'SOAR automation for rapid incident response and threat mitigation workflows.'
              },
              {
                icon: Lock,
                title: 'Penetration Testing',
                description: 'Comprehensive wireless and network security testing tools for authorized testing.'
              },
              {
                icon: Users,
                title: 'Team Collaboration',
                description: 'Real-time collaboration tools designed specifically for security operations centers.'
              },
              {
                icon: Building,
                title: 'Enterprise Ready',
                description: 'Scalable architecture with enterprise-grade security and compliance features.'
              },
              {
                icon: CreditCard,
                title: 'Flexible Billing',
                description: 'Monthly or yearly billing with easy plan changes and transparent pricing.'
              }
            ].map((feature, index) => (
              <motion.div
                key={feature.title}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="text-center"
              >
                <div className="w-16 h-16 bg-primary-500/20 rounded-full flex items-center justify-center mx-auto mb-4">
                  <feature.icon className="h-8 w-8 text-primary-500" />
                </div>
                <h3 className="text-xl font-semibold text-white mb-3">{feature.title}</h3>
                <p className="text-gray-400">{feature.description}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Testimonials */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Trusted</span> by Security Professionals
            </h2>
            <p className="text-xl text-gray-300">
              See what our customers have to say about CyberRecon Suite
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-3 gap-8">
            {testimonials.map((testimonial, index) => (
              <motion.div
                key={testimonial.name}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.2 }}
                className="card"
              >
                <div className="flex items-center mb-4">
                  <img
                    src={testimonial.image}
                    alt={testimonial.name}
                    className="w-12 h-12 rounded-full mr-4"
                  />
                  <div>
                    <h4 className="text-white font-semibold">{testimonial.name}</h4>
                    <p className="text-gray-400 text-sm">{testimonial.role}</p>
                  </div>
                </div>
                <p className="text-gray-300 italic">"{testimonial.content}"</p>
                <div className="flex mt-4">
                  {[...Array(5)].map((_, i) => (
                    <Star key={i} className="h-4 w-4 text-yellow-400 fill-current" />
                  ))}
                </div>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* FAQ Section */}
      <section className="py-20 bg-dark-800/50">
        <div className="max-w-4xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Frequently</span> Asked Questions
            </h2>
            <p className="text-xl text-gray-300">
              Get answers to common questions about our pricing and features
            </p>
          </motion.div>

          <div className="space-y-6">
            {faqs.map((faq, index) => (
              <motion.div
                key={index}
                initial={{ opacity: 0, y: 20 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="card"
              >
                <h3 className="text-lg font-semibold text-white mb-3">{faq.question}</h3>
                <p className="text-gray-300">{faq.answer}</p>
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
              Ready to Secure Your Organization?
            </h2>
            <p className="text-xl text-gray-300 mb-8">
              Start your free trial today. No credit card required.
            </p>
            <div className="flex flex-col sm:flex-row gap-4 justify-center">
              <button
                onClick={() => setSelectedPlan('professional')}
                className="btn-primary text-lg px-8 py-4"
              >
                Start Free Trial
              </button>
              <Link to="/contact" className="btn-secondary text-lg px-8 py-4">
                Contact Sales
              </Link>
            </div>
          </motion.div>
        </div>
      </section>

      {/* Subscription Modal */}
      {selectedPlan && (
        <SubscriptionModal
          plan={selectedPlan}
          onClose={() => setSelectedPlan(null)}
        />
      )}
    </div>
  )
}

export default Subscribe