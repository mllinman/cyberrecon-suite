import React, { useState } from 'react'
import { motion } from 'framer-motion'
import { X, CreditCard, Lock, CheckCircle } from 'lucide-react'
import { useForm } from 'react-hook-form'
import toast from 'react-hot-toast'
import { useAuth } from '../contexts/AuthContext'

interface SubscriptionModalProps {
  plan: 'professional' | 'enterprise'
  onClose: () => void
}

interface PaymentForm {
  cardNumber: string
  expiryDate: string
  cvc: string
  cardName: string
  email: string
  country: string
}

const SubscriptionModal: React.FC<SubscriptionModalProps> = ({ plan, onClose }) => {
  const [isProcessing, setIsProcessing] = useState(false)
  const [step, setStep] = useState<'plan' | 'payment' | 'success'>('plan')
  const { register, handleSubmit, formState: { errors } } = useForm<PaymentForm>()
  const { updateSubscription } = useAuth()

  const planDetails = {
    professional: {
      name: 'Professional Plan',
      price: 99,
      features: [
        'Full SIEM & EDR monitoring',
        'Threat intelligence feeds',
        'Up to 100,000 events/month',
        'Basic penetration testing tools',
        'Email support',
        'Standard integrations'
      ]
    },
    enterprise: {
      name: 'Enterprise Plan',
      price: 299,
      features: [
        'All Professional features',
        'Advanced ML correlation',
        'Unlimited events processing',
        'Full penetration testing suite',
        'Custom integrations',
        '24/7 phone support',
        'Dedicated account manager'
      ]
    }
  }

  const selectedPlan = planDetails[plan]

  const onSubmit = async (data: PaymentForm) => {
    setIsProcessing(true)
    
    // Simulate payment processing
    await new Promise(resolve => setTimeout(resolve, 3000))
    
    // Update user subscription
    updateSubscription(plan)
    
    setStep('success')
    setIsProcessing(false)
    
    toast.success('Subscription activated successfully!')
  }

  const formatCardNumber = (value: string) => {
    const v = value.replace(/\s+/g, '').replace(/[^0-9]/gi, '')
    const matches = v.match(/\d{4,16}/g)
    const match = matches && matches[0] || ''
    const parts = []
    for (let i = 0, len = match.length; i < len; i += 4) {
      parts.push(match.substring(i, i + 4))
    }
    if (parts.length) {
      return parts.join(' ')
    } else {
      return v
    }
  }

  const formatExpiryDate = (value: string) => {
    const v = value.replace(/\s+/g, '').replace(/[^0-9]/gi, '')
    if (v.length >= 2) {
      return v.substring(0, 2) + '/' + v.substring(2, 4)
    }
    return v
  }

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/50 backdrop-blur-sm">
      <motion.div
        initial={{ opacity: 0, scale: 0.9 }}
        animate={{ opacity: 1, scale: 1 }}
        exit={{ opacity: 0, scale: 0.9 }}
        className="bg-slate-800/95 backdrop-blur-sm rounded-2xl border border-slate-700 w-full max-w-2xl max-h-[90vh] overflow-y-auto shadow-2xl"
      >
        {/* Header */}
        <div className="flex items-center justify-between p-6 border-b border-slate-700">
          <h2 className="text-2xl font-bold text-white">
            {step === 'success' ? 'Subscription Activated!' : `Subscribe to ${selectedPlan.name}`}
          </h2>
          <button
            onClick={onClose}
            className="text-slate-400 hover:text-white transition-colors"
          >
            <X className="h-6 w-6" />
          </button>
        </div>

        <div className="p-6">
          {step === 'plan' && (
            <motion.div
              initial={{ opacity: 0, x: 20 }}
              animate={{ opacity: 1, x: 0 }}
              className="space-y-6"
            >
              {/* Plan Summary */}
              <div className="bg-slate-700/30 rounded-lg p-6 border border-slate-600/30">
                <div className="flex items-center justify-between mb-4">
                  <h3 className="text-xl font-semibold text-white">{selectedPlan.name}</h3>
                  <div className="text-right">
                    <div className="text-3xl font-bold text-primary-400">${selectedPlan.price}</div>
                    <div className="text-slate-400 text-sm">per month</div>
                  </div>
                </div>
                
                <ul className="space-y-2">
                  {selectedPlan.features.map((feature, index) => (
                    <li key={index} className="flex items-center space-x-2">
                      <CheckCircle className="h-4 w-4 text-primary-500" />
                      <span className="text-slate-300 text-sm">{feature}</span>
                    </li>
                  ))}
                </ul>
              </div>

              <button
                onClick={() => setStep('payment')}
                className="w-full btn-primary py-4 text-lg"
              >
                Continue to Payment
              </button>
            </motion.div>
          )}

          {step === 'payment' && (
            <motion.div
              initial={{ opacity: 0, x: 20 }}
              animate={{ opacity: 1, x: 0 }}
              className="space-y-6"
            >
              {/* Security Notice */}
              <div className="flex items-center space-x-3 p-4 bg-primary-900/20 border border-primary-500/30 rounded-lg">
                <Lock className="h-5 w-5 text-primary-500" />
                <p className="text-sm text-primary-300">
                  Your payment information is secured with 256-bit SSL encryption
                </p>
              </div>

              <form onSubmit={handleSubmit(onSubmit)} className="space-y-6">
                {/* Card Information */}
                <div>
                  <label className="block text-sm font-medium text-slate-300 mb-2">
                    Card Number
                  </label>
                  <input
                    {...register('cardNumber', { 
                      required: 'Card number is required',
                      pattern: {
                        value: /^[0-9\s]{13,19}$/,
                        message: 'Invalid card number'
                      }
                    })}
                    type="text"
                    placeholder="4242 4242 4242 4242"
                    maxLength={19}
                    onChange={(e) => {
                      e.target.value = formatCardNumber(e.target.value)
                    }}
                    className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                  />
                  {errors.cardNumber && (
                    <p className="mt-1 text-sm text-red-400">{errors.cardNumber.message}</p>
                  )}
                </div>

                <div className="grid grid-cols-2 gap-4">
                  <div>
                    <label className="block text-sm font-medium text-slate-300 mb-2">
                      Expiry Date
                    </label>
                    <input
                      {...register('expiryDate', { required: 'Expiry date is required' })}
                      type="text"
                      placeholder="MM/YY"
                      maxLength={5}
                      onChange={(e) => {
                        e.target.value = formatExpiryDate(e.target.value)
                      }}
                      className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                    />
                    {errors.expiryDate && (
                      <p className="mt-1 text-sm text-red-400">{errors.expiryDate.message}</p>
                    )}
                  </div>
                  
                  <div>
                    <label className="block text-sm font-medium text-slate-300 mb-2">
                      CVC
                    </label>
                    <input
                      {...register('cvc', { required: 'CVC is required' })}
                      type="text"
                      placeholder="123"
                      maxLength={4}
                      className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                    />
                    {errors.cvc && (
                      <p className="mt-1 text-sm text-red-400">{errors.cvc.message}</p>
                    )}
                  </div>
                </div>

                <div>
                  <label className="block text-sm font-medium text-slate-300 mb-2">
                    Cardholder Name
                  </label>
                  <input
                    {...register('cardName', { required: 'Cardholder name is required' })}
                    type="text"
                    placeholder="John Doe"
                    className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                  />
                  {errors.cardName && (
                    <p className="mt-1 text-sm text-red-400">{errors.cardName.message}</p>
                  )}
                </div>

                <div>
                  <label className="block text-sm font-medium text-slate-300 mb-2">
                    Email Address
                  </label>
                  <input
                    {...register('email', { 
                      required: 'Email is required',
                      pattern: {
                        value: /^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$/i,
                        message: 'Invalid email address'
                      }
                    })}
                    type="email"
                    placeholder="john@company.com"
                    className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                  />
                  {errors.email && (
                    <p className="mt-1 text-sm text-red-400">{errors.email.message}</p>
                  )}
                </div>

                <div>
                  <label className="block text-sm font-medium text-slate-300 mb-2">
                    Country
                  </label>
                  <select
                    {...register('country', { required: 'Country is required' })}
                    className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                  >
                    <option value="">Select country</option>
                    <option value="US">United States</option>
                    <option value="CA">Canada</option>
                    <option value="GB">United Kingdom</option>
                    <option value="DE">Germany</option>
                    <option value="FR">France</option>
                    <option value="AU">Australia</option>
                  </select>
                  {errors.country && (
                    <p className="mt-1 text-sm text-red-400">{errors.country.message}</p>
                  )}
                </div>

                {/* Order Summary */}
                <div className="bg-slate-700/30 rounded-lg p-4 border border-slate-600/30">
                  <h4 className="font-semibold text-white mb-3">Order Summary</h4>
                  <div className="flex justify-between items-center">
                    <span className="text-slate-300">{selectedPlan.name}</span>
                    <span className="text-white font-semibold">${selectedPlan.price}/month</span>
                  </div>
                </div>

                <button
                  type="submit"
                  disabled={isProcessing}
                  className="w-full btn-primary py-4 disabled:opacity-50 disabled:cursor-not-allowed"
                >
                  {isProcessing ? (
                    <div className="flex items-center justify-center">
                      <div className="animate-spin rounded-full h-5 w-5 border-b-2 border-white mr-2"></div>
                      Processing Payment...
                    </div>
                  ) : (
                    <div className="flex items-center justify-center">
                      <CreditCard className="mr-2 h-5 w-5" />
                      Subscribe for ${selectedPlan.price}/month
                    </div>
                  )}
                </button>
              </form>

              <p className="text-xs text-slate-500 text-center mt-4">
                By subscribing, you agree to our Terms of Service and Privacy Policy. 
                You can cancel anytime from your dashboard.
              </p>
            </motion.div>
          )}

          {step === 'success' && (
            <motion.div
              initial={{ opacity: 0, scale: 0.9 }}
              animate={{ opacity: 1, scale: 1 }}
              className="text-center space-y-6"
            >
              <div className="w-20 h-20 bg-green-500/20 rounded-full flex items-center justify-center mx-auto">
                <CheckCircle className="h-10 w-10 text-green-500" />
              </div>
              
              <div>
                <h3 className="text-2xl font-bold text-white mb-2">Welcome to {selectedPlan.name}!</h3>
                <p className="text-slate-300">
                  Your subscription has been activated. You can now download CyberRecon Suite.
                </p>
              </div>

              <div className="bg-slate-700/30 rounded-lg p-6 border border-slate-600/30">
                <h4 className="font-semibold text-white mb-3">What's Next?</h4>
                <ul className="space-y-2 text-left">
                  <li className="flex items-center space-x-2">
                    <CheckCircle className="h-4 w-4 text-green-500" />
                    <span className="text-slate-300">Download CyberRecon Suite from your dashboard</span>
                  </li>
                  <li className="flex items-center space-x-2">
                    <CheckCircle className="h-4 w-4 text-green-500" />
                    <span className="text-slate-300">Access installation guides and documentation</span>
                  </li>
                  <li className="flex items-center space-x-2">
                    <CheckCircle className="h-4 w-4 text-green-500" />
                    <span className="text-slate-300">Contact support for deployment assistance</span>
                  </li>
                </ul>
              </div>

              <button
                onClick={onClose}
                className="w-full btn-primary py-3"
              >
                Go to Dashboard
              </button>
            </motion.div>
          )}
        </div>
      </motion.div>
    </div>
  )
}

export default SubscriptionModal