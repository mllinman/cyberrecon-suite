import React, { createContext, useContext, useState, useEffect } from 'react'
import { loadStripe, Stripe } from '@stripe/stripe-js'
import { Elements } from '@stripe/react-stripe-js'

// For development, we'll use test keys. In production, replace with your actual keys.
const STRIPE_PUBLIC_KEY = import.meta.env.VITE_STRIPE_PUBLIC_KEY || 'pk_test_51Oxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'

const stripePromise = loadStripe(STRIPE_PUBLIC_KEY)

interface StripeContextType {
  stripe: Stripe | null
  isLoading: boolean
  error: string | null
  createPaymentIntent: (amount: number, currency?: string) => Promise<any>
  createSubscription: (priceId: string, customerId?: string) => Promise<any>
}

const StripeContext = createContext<StripeContextType | undefined>(undefined)

export const useStripe = () => {
  const context = useContext(StripeContext)
  if (context === undefined) {
    throw new Error('useStripe must be used within a StripeProvider')
  }
  return context
}

export const StripeProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [stripe, setStripe] = useState<Stripe | null>(null)
  const [isLoading, setIsLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    const initializeStripe = async () => {
      try {
        const stripeInstance = await stripePromise
        setStripe(stripeInstance)
        setError(null)
      } catch (err) {
        setError('Failed to load Stripe. Payment functionality will be limited.')
        console.warn('Stripe initialization failed:', err)
      } finally {
        setIsLoading(false)
      }
    }

    initializeStripe()
  }, [])

  const createPaymentIntent = async (amount: number, currency: string = 'usd') => {
    try {
      // In a real app, this would call your backend
      const response = await fetch('/api/create-payment-intent', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ amount, currency }),
      })
      return await response.json()
    } catch (err) {
      throw new Error('Failed to create payment intent')
    }
  }

  const createSubscription = async (priceId: string, customerId?: string) => {
    try {
      // In a real app, this would call your backend
      const response = await fetch('/api/create-subscription', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ priceId, customerId }),
      })
      return await response.json()
    } catch (err) {
      throw new Error('Failed to create subscription')
    }
  }

  const value: StripeContextType = {
    stripe,
    isLoading,
    error,
    createPaymentIntent,
    createSubscription
  }

  return (
    <Elements stripe={stripePromise}>
      <StripeContext.Provider value={value}>
        {children}
      </StripeContext.Provider>
    </Elements>
  )
}