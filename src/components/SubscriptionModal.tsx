import React, { useState, useEffect } from 'react'
import { motion, AnimatePresence } from 'framer-motion'
import { X, CreditCard, Lock, CheckCircle, Shield, Zap, Loader2 } from 'lucide-react'
import { useForm } from 'react-hook-form'
import { CardElement, useStripe, useElements } from '@stripe/react-stripe-js'
import toast from 'react-hot-toast'
import { useAuth } from '../contexts/AuthContext'

interface SubscriptionModalProps {
  plan: 'professional' | 'enterprise'
  onClose: () => void
}

interface PaymentForm {
  email: string
  name: string
  country: string
}

const SubscriptionModal: React.FC<SubscriptionModalProps> = ({ plan, onClose }) => {
  const [isProcessing, setIsProcessing] = useState(false)
  const [step, setStep] = useState<'plan' | 'payment' | 'processing' | 'success'>('plan')
  const { register, handleSubmit, formState: { errors } } = useForm<PaymentForm>()
  const { updateSubscription, user } = useAuth()
  const stripe = useStripe()
  const elements = useElements()

  const planDetails = {
    professional: {
      name: 'Professional Plan',
      price: 99,
      features: [
        'Full SIEM & EDR monitoring',
        'Threat intelligence feeds', 
        'Up to 100,000 events/month',
        'Penetration testing tools',
        'Advanced firewall protection',
        'Email & chat support',
        'Team collaboration (up to 10 users)',
        'Compliance dashboard',
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
        'Custom integrations (unlimited)',
        '24/7 phone support',
        'Dedicated account manager',
        'Priority feature requests',
        'Custom SLA agreements',
        'On-premise deployment options'
      ]
    }
  }

  const selectedPlan = planDetails[plan]

  const cardElementOptions = {
    style: {
      base: {
        fontSize: '16px',
        color: '#ffffff',
        backgroundColor: 'transparent',
        '::placeholder': {
          color: '#94a3b8',
        },
      },
      invalid: {
        color: '#ef4444',
        iconColor: '#ef4444'
      }
    }
  }

  const onSubmit = async (data: PaymentForm) => {
    if (!stripe || !elements) {
      toast.error('Payment system not ready. Please try again.')
      return
    }

    setIsProcessing(true)
    setStep('processing')
    
    try {
      const cardElement = elements.getElement(CardElement)
      
      if (!cardElement) {
        throw new Error('Card element not found')
      }

      // Create payment method
      const { error, paymentMethod } = await stripe.createPaymentMethod({
        type: 'card',
        card: cardElement,
        billing_details: {
          name: data.name,
          email: data.email,
        },
      })

      if (error) {
        throw new Error(error.message)
      }

      // In a real app, you'd send this to your backend
      // For demo purposes, we'll simulate success after a delay
      await new Promise(resolve => setTimeout(resolve, 3000))
      
      // Simulate successful subscription creation
      updateSubscription(plan)
      
      setStep('success')
      toast.success('🎉 Subscription activated successfully!')
      
      // Auto-close after success
      setTimeout(() => {
        onClose()
      }, 3000)
      
    } catch (error: any) {
      toast.error(error.message || 'Payment failed. Please try again.')
      setStep('payment')
    } finally {
      setIsProcessing(false)
    }
  }

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/50 backdrop-blur-sm">
      <motion.div
        initial={{ opacity: 0, scale: 0.9 }}
        animate={{ opacity: 1, scale: 1 }}
        exit={{ opacity: 0, scale: 0.9 }}
        className="glass-card rounded-2xl border border-slate-700 w-full max-w-2xl max-h-[90vh] overflow-y-auto shadow-2xl"
      >
        {/* Header */}
        <div className="flex items-center justify-between p-6 border-b border-slate-700/50">
          <motion.h2 
            className="text-2xl font-bold text-white flex items-center"
            initial={{ opacity: 0, x: -20 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ delay: 0.1 }}
          >
            {step === 'success' ? (
              <>
                <CheckCircle className="mr-2 h-6 w-6 text-green-500" />
                Subscription Activated!
              </>
            ) : step === 'processing' ? (
              <>
                <Loader2 className="mr-2 h-6 w-6 animate-spin text-primary-500" />
                Processing Payment...
              </>
            ) : (
              <>
                <Shield className="mr-2 h-6 w-6 text-primary-500" />
                Subscribe to {selectedPlan.name}
              </>
            )}
          </motion.h2>
          <button
            onClick={onClose}
            className="p-2 hover:bg-slate-700/50 rounded-lg transition-colors"
            disabled={isProcessing}
          >
            <X className="h-5 w-5 text-slate-400" />
          </button>
        </div>

        <div className="p-6">
          <AnimatePresence mode="wait">
            {step === 'plan' && (
              <motion.div
                key="plan"
                initial={{ opacity: 0, x: 20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -20 }}
                className="space-y-6"
              >
                {/* Plan Summary */}
                <div className="glass-morphism rounded-xl p-6 border border-slate-600/30">
                  <div className="flex items-center justify-between mb-4">
                    <h3 className="text-xl font-semibold text-white">{selectedPlan.name}</h3>
                    <div className="text-right">
                      <motion.div 
                        className="text-3xl font-bold text-primary-400"
                        animate={{ scale: [1, 1.05, 1] }}
                        transition={{ duration: 2, repeat: Infinity }}
                      >
                        ${selectedPlan.price}
                      </motion.div>
                      <div className="text-slate-400 text-sm">per month</div>
                    </div>
                  </div>
                  
                  <ul className="space-y-2">
                    {selectedPlan.features.map((feature, index) => (
                      <motion.li 
                        key={index} 
                        className="flex items-center space-x-2"
                        initial={{ opacity: 0, x: -10 }}
                        animate={{ opacity: 1, x: 0 }}
                        transition={{ delay: index * 0.05 }}
                      >
                        <CheckCircle className="h-4 w-4 text-primary-500 flex-shrink-0" />
                        <span className="text-slate-300 text-sm">{feature}</span>
                      </motion.li>
                    ))}
                  </ul>
                </div>

                <motion.button
                  type="button"
                  onClick={() => setStep('payment')}
                  className="w-full btn-primary py-4"
                  whileHover={{ scale: 1.02 }}
                  whileTap={{ scale: 0.98 }}
                >
                  <CreditCard className="mr-2 h-5 w-5" />
                  Continue to Payment
                </motion.button>
              </motion.div>
            )}

            {step === 'payment' && (
              <motion.div
                key="payment"
                initial={{ opacity: 0, x: 20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -20 }}
              >
                <form onSubmit={handleSubmit(onSubmit)} className="space-y-6">
                  {/* Customer Information */}
                  <div className="glass-morphism rounded-xl p-6 border border-slate-600/30">
                    <h4 className="font-semibold text-white mb-4 flex items-center">
                      <Shield className="mr-2 h-5 w-5 text-primary-500" />
                      Customer Information
                    </h4>
                    
                    <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                      <div>
                        <label className="block text-sm font-medium text-slate-300 mb-2">
                          Full Name *
                        </label>
                        <input
                          {...register('name', { required: 'Name is required' })}
                          className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                          placeholder="John Doe"
                        />
                        {errors.name && (
                          <p className="text-red-400 text-xs mt-1">{errors.name.message}</p>
                        )}
                      </div>
                      
                      <div>
                        <label className="block text-sm font-medium text-slate-300 mb-2">
                          Email Address *
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
                          className="w-full px-4 py-3 bg-slate-700/50 border border-slate-600 rounded-lg text-white placeholder-slate-400 focus:border-primary-500 focus:ring-1 focus:ring-primary-500 transition-colors"
                          placeholder="john@example.com"
                        />
                        {errors.email && (
                          <p className="text-red-400 text-xs mt-1">{errors.email.message}</p>
                        )}
                      </div>
                    </div>
                  </div>

                  {/* Payment Information */}
                  <div className="glass-morphism rounded-xl p-6 border border-slate-600/30">
                    <h4 className="font-semibold text-white mb-4 flex items-center">
                      <CreditCard className="mr-2 h-5 w-5 text-primary-500" />
                      Payment Information
                    </h4>
                    
                    <div className="mb-4">
                      <label className="block text-sm font-medium text-slate-300 mb-2">
                        Card Details *
                      </label>
                      <div className="p-4 bg-slate-700/50 border border-slate-600 rounded-lg focus-within:border-primary-500 transition-colors">
                        <CardElement options={cardElementOptions} />
                      </div>
                    </div>

                    <div className="flex items-center text-sm text-slate-400">
                      <Lock className="h-4 w-4 mr-2 text-green-500" />
                      Your payment information is encrypted and secure
                    </div>
                  </div>

                  {/* Order Summary */}
                  <div className="glass-morphism rounded-xl p-6 border border-slate-600/30">
                    <h4 className="font-semibold text-white mb-3 flex items-center">
                      <Zap className="mr-2 h-5 w-5 text-primary-500" />
                      Order Summary
                    </h4>
                    <div className="flex justify-between items-center text-lg">
                      <span className="text-slate-300">{selectedPlan.name}</span>
                      <span className="text-white font-semibold">${selectedPlan.price}/month</span>
                    </div>
                  </div>

                  <div className="flex space-x-3">
                    <button
                      type="button"
                      onClick={() => setStep('plan')}
                      className="flex-1 py-3 px-4 bg-slate-700 text-white rounded-lg hover:bg-slate-600 transition-colors"
                      disabled={isProcessing}
                    >
                      Back
                    </button>
                    <motion.button
                      type="submit"
                      disabled={isProcessing || !stripe}
                      className="flex-1 btn-primary py-3 disabled:opacity-50 disabled:cursor-not-allowed"
                      whileHover={{ scale: isProcessing ? 1 : 1.02 }}
                      whileTap={{ scale: isProcessing ? 1 : 0.98 }}
                    >
                      {isProcessing ? (
                        <span className="flex items-center justify-center">
                          <Loader2 className="animate-spin h-5 w-5 mr-2" />
                          Processing...
                        </span>
                      ) : (
                        <span className="flex items-center justify-center">
                          <CreditCard className="mr-2 h-5 w-5" />
                          Subscribe for ${selectedPlan.price}/month
                        </span>
                      )}
                    </motion.button>
                  </div>
                </form>
              </motion.div>
            )}

            {step === 'processing' && (
              <motion.div
                key="processing"
                initial={{ opacity: 0, scale: 0.9 }}
                animate={{ opacity: 1, scale: 1 }}
                exit={{ opacity: 0, scale: 0.9 }}
                className="text-center py-12"
              >
                <motion.div
                  animate={{ rotate: 360 }}
                  transition={{ duration: 2, repeat: Infinity, ease: "linear" }}
                  className="inline-block"
                >
                  <Loader2 className="h-16 w-16 text-primary-500" />
                </motion.div>
                <h3 className="text-xl font-semibold text-white mt-4 mb-2">
                  Processing Your Payment
                </h3>
                <p className="text-slate-400">
                  Please don't close this window while we process your subscription...
                </p>
              </motion.div>
            )}

            {step === 'success' && (
              <motion.div
                key="success"
                initial={{ opacity: 0, scale: 0.9 }}
                animate={{ opacity: 1, scale: 1 }}
                exit={{ opacity: 0, scale: 0.9 }}
                className="text-center py-12"
              >
                <motion.div
                  initial={{ scale: 0 }}
                  animate={{ scale: 1 }}
                  transition={{ type: "spring", stiffness: 260, damping: 20, delay: 0.1 }}
                >
                  <CheckCircle className="h-16 w-16 text-green-500 mx-auto mb-4" />
                </motion.div>
                <motion.h3 
                  className="text-2xl font-semibold text-white mb-2"
                  initial={{ opacity: 0, y: 20 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ delay: 0.3 }}
                >
                  Welcome to {selectedPlan.name}! 🎉
                </motion.h3>
                <motion.p 
                  className="text-slate-400 mb-6"
                  initial={{ opacity: 0, y: 20 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ delay: 0.5 }}
                >
                  Your subscription has been activated successfully. You now have access to all premium features.
                </motion.p>
                <motion.button
                  onClick={onClose}
                  className="btn-primary px-8 py-3"
                  initial={{ opacity: 0, y: 20 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ delay: 0.7 }}
                  whileHover={{ scale: 1.05 }}
                  whileTap={{ scale: 0.95 }}
                >
                  Get Started
                </motion.button>
              </motion.div>
            )}
          </AnimatePresence>
        </div>
      </motion.div>
    </div>
  )
}

export default SubscriptionModal