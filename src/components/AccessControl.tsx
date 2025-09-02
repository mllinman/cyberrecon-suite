import React from 'react'
import { motion } from 'framer-motion'
import { Lock, AlertTriangle, Star } from 'lucide-react'
import { useAuth } from '../contexts/AuthContext'

interface AccessControlProps {
  requiredPlan: 'individual' | 'professional' | 'enterprise'
  feature: string
  children: React.ReactNode
  showUpgrade?: boolean
}

const AccessControl: React.FC<AccessControlProps> = ({ 
  requiredPlan, 
  feature, 
  children, 
  showUpgrade = true 
}) => {
  const { user } = useAuth()

  const planHierarchy = {
    individual: 1,
    professional: 2,
    enterprise: 3
  }

  const hasAccess = user && planHierarchy[user.subscription] >= planHierarchy[requiredPlan]

  if (hasAccess) {
    return <>{children}</>
  }

  const getUpgradePlan = () => {
    if (requiredPlan === 'professional') return 'Professional'
    if (requiredPlan === 'enterprise') return 'Enterprise'
    return 'Professional'
  }

  const getUpgradePrice = () => {
    if (requiredPlan === 'professional') return '$99'
    if (requiredPlan === 'enterprise') return '$299'
    return '$99'
  }

  return (
    <motion.div
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      className="relative"
    >
      {/* Blurred Content */}
      <div className="filter blur-sm pointer-events-none opacity-50">
        {children}
      </div>

      {/* Access Control Overlay */}
      <div className="absolute inset-0 flex items-center justify-center bg-slate-900/80 backdrop-blur-sm rounded-lg">
        <div className="text-center p-8 max-w-md">
          <div className="w-16 h-16 bg-orange-500/20 rounded-full flex items-center justify-center mx-auto mb-4">
            <Lock className="h-8 w-8 text-orange-500" />
          </div>
          
          <h3 className="text-xl font-bold text-white mb-2">
            {getUpgradePlan()} Plan Required
          </h3>
          
          <p className="text-slate-300 mb-4">
            {feature} is available for {getUpgradePlan()} and Enterprise subscribers.
          </p>

          {user?.subscription === 'individual' && requiredPlan === 'professional' && (
            <div className="bg-orange-500/10 border border-orange-500/30 rounded-lg p-4 mb-4">
              <div className="flex items-center space-x-2 mb-2">
                <Star className="h-5 w-5 text-orange-500" />
                <span className="text-orange-400 font-semibold">7-Day Free Trial Available!</span>
              </div>
              <p className="text-sm text-slate-300">
                Try all Professional features including penetration testing tools for 7 days free.
              </p>
            </div>
          )}

          {showUpgrade && (
            <div className="space-y-3">
              <button className="w-full bg-gradient-to-r from-orange-500 to-orange-600 hover:from-orange-600 hover:to-orange-700 text-white font-semibold py-3 px-6 rounded-lg transition-all duration-200 transform hover:scale-105">
                {user?.subscription === 'individual' && requiredPlan === 'professional' 
                  ? 'Start 7-Day Free Trial' 
                  : `Upgrade to ${getUpgradePlan()} - ${getUpgradePrice()}/month`
                }
              </button>
              
              <p className="text-xs text-slate-500">
                {user?.subscription === 'individual' && requiredPlan === 'professional'
                  ? 'No credit card required for trial'
                  : 'Cancel anytime from your dashboard'
                }
              </p>
            </div>
          )}
        </div>
      </div>
    </motion.div>
  )
}

export default AccessControl