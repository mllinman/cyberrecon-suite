import React, { useState } from 'react'
import { motion } from 'framer-motion'
import { X, Clock, Star, ArrowRight } from 'lucide-react'
import SubscriptionModal from './SubscriptionModal'

interface TrialUpgradeModalProps {
  onClose: () => void
  trialEndsAt: string
}

const TrialUpgradeModal: React.FC<TrialUpgradeModalProps> = ({ onClose, trialEndsAt }) => {
  const [showSubscription, setShowSubscription] = useState(false)

  const handleUpgrade = () => {
    setShowSubscription(true)
  }

  if (showSubscription) {
    return (
      <SubscriptionModal
        plan="professional"
        onClose={() => {
          setShowSubscription(false)
          onClose()
        }}
      />
    )
  }

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/50 backdrop-blur-sm">
      <motion.div
        initial={{ opacity: 0, scale: 0.9 }}
        animate={{ opacity: 1, scale: 1 }}
        exit={{ opacity: 0, scale: 0.9 }}
        className="bg-slate-800/95 backdrop-blur-sm rounded-2xl border border-slate-700 w-full max-w-md shadow-2xl"
      >
        {/* Header */}
        <div className="flex items-center justify-between p-6 border-b border-slate-700">
          <div className="flex items-center space-x-2">
            <Clock className="h-6 w-6 text-orange-500" />
            <h2 className="text-xl font-bold text-white">Trial Ending Soon</h2>
          </div>
          <button
            onClick={onClose}
            className="text-slate-400 hover:text-white transition-colors"
          >
            <X className="h-6 w-6" />
          </button>
        </div>

        <div className="p-6 space-y-6">
          {/* Trial Info */}
          <div className="text-center">
            <div className="w-16 h-16 bg-orange-500/20 rounded-full flex items-center justify-center mx-auto mb-4">
              <Star className="h-8 w-8 text-orange-500" />
            </div>
            <h3 className="text-lg font-semibold text-white mb-2">
              Your 7-day Professional trial ends on
            </h3>
            <p className="text-2xl font-bold text-orange-400 mb-4">
              {new Date(trialEndsAt).toLocaleDateString()}
            </p>
            <p className="text-slate-300">
              Continue enjoying full Professional features including penetration testing tools, 
              advanced monitoring, and team collaboration.
            </p>
          </div>

          {/* Benefits */}
          <div className="bg-slate-700/30 rounded-lg p-4 border border-slate-600/30">
            <h4 className="font-semibold text-white mb-3">Keep Your Professional Features:</h4>
            <ul className="space-y-2 text-sm">
              <li className="flex items-center space-x-2">
                <div className="w-1.5 h-1.5 bg-orange-500 rounded-full"></div>
                <span className="text-slate-300">Penetration testing tools (WiFi, Network, Web App)</span>
              </li>
              <li className="flex items-center space-x-2">
                <div className="w-1.5 h-1.5 bg-orange-500 rounded-full"></div>
                <span className="text-slate-300">Advanced firewall protection</span>
              </li>
              <li className="flex items-center space-x-2">
                <div className="w-1.5 h-1.5 bg-orange-500 rounded-full"></div>
                <span className="text-slate-300">100,000 events/month (vs 10,000 Individual)</span>
              </li>
              <li className="flex items-center space-x-2">
                <div className="w-1.5 h-1.5 bg-orange-500 rounded-full"></div>
                <span className="text-slate-300">Team collaboration features</span>
              </li>
            </ul>
          </div>

          {/* Action Buttons */}
          <div className="space-y-3">
            <button
              onClick={handleUpgrade}
              className="w-full bg-gradient-to-r from-orange-500 to-orange-600 hover:from-orange-600 hover:to-orange-700 text-white font-semibold py-4 px-6 rounded-lg transition-all duration-200 transform hover:scale-105 shadow-lg hover:shadow-orange-500/25"
            >
              <div className="flex items-center justify-center">
                <Star className="mr-2 h-5 w-5" />
                Upgrade to Professional - $99/month
              </div>
            </button>
            
            <button
              onClick={onClose}
              className="w-full bg-slate-700/50 border border-slate-600 text-slate-300 hover:text-white hover:bg-slate-700 font-medium py-3 px-6 rounded-lg transition-all duration-200"
            >
              Continue with Individual Plan - $25/month
            </button>
          </div>

          <p className="text-xs text-slate-500 text-center">
            No commitment required. Cancel anytime from your dashboard.
          </p>
        </div>
      </motion.div>
    </div>
  )
}

export default TrialUpgradeModal