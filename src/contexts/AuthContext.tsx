import React, { createContext, useContext, useState, useEffect } from 'react'

interface User {
  id: string
  name: string
  email: string
  subscription: 'individual' | 'professional' | 'enterprise'
  subscriptionStatus: 'active' | 'inactive' | 'trial'
  trialEndsAt?: string
}

interface AuthContextType {
  user: User | null
  login: (email: string, password: string) => Promise<boolean>
  logout: () => void
  updateSubscription: (subscription: User['subscription']) => void
}

const AuthContext = createContext<AuthContextType | undefined>(undefined)

export const useAuth = () => {
  const context = useContext(AuthContext)
  if (context === undefined) {
    throw new Error('useAuth must be used within an AuthProvider')
  }
  return context
}

export const AuthProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [user, setUser] = useState<User | null>(null)

  useEffect(() => {
    // Check for stored user session
    const storedUser = localStorage.getItem('bulletdrop_user')
    if (storedUser) {
      setUser(JSON.parse(storedUser))
    }
  }, [])

  const login = async (email: string, password: string): Promise<boolean> => {
    // Simulate authentication
    if (email === 'demo@bulletdropstudio.com' && password === 'demo123') {
      const newUser: User = {
        id: '1',
        name: 'Demo User',
        email: 'demo@bulletdropstudio.com',
        subscription: 'individual',
        subscriptionStatus: 'active'
      }
      setUser(newUser)
      localStorage.setItem('bulletdrop_user', JSON.stringify(newUser))
      return true
    }
    
    if (email === 'admin@bulletdropstudio.com' && password === 'admin123') {
      const newUser: User = {
        id: '2',
        name: 'Michael Linman',
        email: 'admin@bulletdropstudio.com',
        subscription: 'enterprise',
        subscriptionStatus: 'active'
      }
      setUser(newUser)
      localStorage.setItem('bulletdrop_user', JSON.stringify(newUser))
      return true
    }
    
    return false
  }

  const logout = () => {
    setUser(null)
    localStorage.removeItem('bulletdrop_user')
  }

  const updateSubscription = (subscription: User['subscription']) => {
    if (user) {
      const updatedUser = { ...user, subscription, subscriptionStatus: 'active' as const }
      setUser(updatedUser)
      localStorage.setItem('bulletdrop_user', JSON.stringify(updatedUser))
    }
  }

  return (
    <AuthContext.Provider value={{ user, login, logout, updateSubscription }}>
      {children}
    </AuthContext.Provider>
  )
}