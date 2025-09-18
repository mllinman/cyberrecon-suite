import React from 'react'
import { Routes, Route } from 'react-router-dom'
import { Toaster } from 'react-hot-toast'
import Navbar from './components/Navbar'
import ThemeSelector from './components/ThemeSelector'
import Footer from './components/Footer'
import Home from './pages/Home'
import Products from './pages/Products'
import CyberRecon from './pages/CyberRecon'
import CyberRanger from './pages/CyberRanger'
import WifiWolverine from './pages/WifiWolverine'
import CustomSolutions from './pages/CustomSolutions'
import About from './pages/About'
import Contact from './pages/Contact'
import Login from './pages/Login'
import Dashboard from './pages/Dashboard'
import Subscribe from './pages/Subscribe'
import ThreatIntelligenceDashboard from './components/ThreatIntelligenceDashboard'
import NetworkMonitoringDashboard from './components/NetworkMonitoringDashboard'
import { AuthProvider } from './contexts/AuthContext'
import { StripeProvider } from './contexts/StripeContext'
import AccessControl from './components/AccessControl'
import FirewallManager from './security/FirewallManager'

function App() {
  return (
    <AuthProvider>
      <StripeProvider>
        <div className="min-h-screen bg-slate-900 text-slate-50">
          <Navbar />
          <ThemeSelector />
          <main>
            <Routes>
              <Route path="/" element={<Home />} />
              <Route path="/products" element={<Products />} />
              <Route path="/products/cyberrecon" element={<CyberRecon />} />
              <Route path="/products/cyberranger" element={<CyberRanger />} />
              <Route path="/products/wifi-wolverine" element={<WifiWolverine />} />
              <Route path="/products/custom" element={<CustomSolutions />} />
              <Route path="/about" element={<About />} />
              <Route path="/contact" element={<Contact />} />
              <Route path="/subscribe" element={<Subscribe />} />
              <Route path="/login" element={<Login />} />
              <Route path="/dashboard" element={<Dashboard />} />
              <Route path="/threat-intelligence" element={
                <AccessControl requiredPlan="individual" feature="Threat Intelligence Dashboard">
                  <ThreatIntelligenceDashboard />
                </AccessControl>
              } />
              <Route path="/network-monitoring" element={
                <AccessControl requiredPlan="individual" feature="Network Monitoring Dashboard">
                  <NetworkMonitoringDashboard />
                </AccessControl>
              } />
              <Route path="/firewall" element={
                <AccessControl requiredPlan="professional" feature="Advanced Firewall Protection">
                  <FirewallManager />
                </AccessControl>
              } />
            </Routes>
          </main>
          <Footer />
          <Toaster 
            position="top-right"
            toastOptions={{
              duration: 4000,
              style: {
                background: '#1e293b',
                color: '#ffffff',
                border: '1px solid var(--color-primary-500)',
              },
            }}
          />
        </div>
      </StripeProvider>
    </AuthProvider>
  )
}

export default App