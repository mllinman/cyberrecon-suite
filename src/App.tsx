import React, { useState, useEffect } from 'react';
import { Routes, Route, Link, useNavigate, Navigate } from 'react-router-dom';
import { Toaster } from 'react-hot-toast';
import { 
  Shield, Activity, Network, AlertTriangle, FileSearch, 
  Settings, LogOut, Menu, X, BarChart3, Users, Lock
} from 'lucide-react';
import SIEMDashboardApp from './components/dashboards/SIEMDashboardApp';
import ThreatIntelligenceDashboard from './components/ThreatIntelligenceDashboard';
import NetworkMonitoringDashboard from './components/NetworkMonitoringDashboard';

function App() {
  const [isAuthenticated, setIsAuthenticated] = useState(false);
  const [user, setUser] = useState<any>(null);
  const [menuOpen, setMenuOpen] = useState(false);
  const navigate = useNavigate();

  useEffect(() => {
    checkAuth();
  }, []);

  const checkAuth = async () => {
    const token = localStorage.getItem('token');
    if (token) {
      try {
        const res = await fetch('/api/auth/me', {
          headers: { 'Authorization': `Bearer ${token}` }
        });
        if (res.ok) {
          const userData = await res.json();
          setUser(userData);
          setIsAuthenticated(true);
        } else {
          localStorage.removeItem('token');
        }
      } catch (error) {
        console.error('Auth check failed:', error);
      }
    }
  };

  const handleLogout = () => {
    localStorage.removeItem('token');
    setIsAuthenticated(false);
    setUser(null);
    navigate('/login');
  };

  if (!isAuthenticated) {
    return <LoginPage onLogin={() => { checkAuth(); navigate('/'); }} />;
  }

  return (
    <div className="min-h-screen bg-slate-950 text-white">
      {/* Top Navigation */}
      <nav className="bg-slate-900/50 border-b border-cyan-500/20 backdrop-blur-sm">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex items-center justify-between h-16">
            <div className="flex items-center gap-3">
              <Shield className="w-8 h-8 text-cyan-400" />
              <span className="text-xl font-bold text-cyan-400">CyberRecon Suite</span>
            </div>
            
            <div className="hidden md:flex items-center gap-4">
              <span className="text-slate-400">Welcome, {user?.fullName}</span>
              <button
                onClick={handleLogout}
                className="flex items-center gap-2 px-4 py-2 bg-red-500/10 text-red-400 rounded-lg hover:bg-red-500/20 transition-colors"
              >
                <LogOut className="w-4 h-4" />
                Logout
              </button>
            </div>

            <button 
              className="md:hidden text-cyan-400"
              onClick={() => setMenuOpen(!menuOpen)}
            >
              {menuOpen ? <X /> : <Menu />}
            </button>
          </div>
        </div>
      </nav>

      <div className="flex">
        {/* Sidebar */}
        <aside className={`${menuOpen ? 'block' : 'hidden'} md:block w-64 bg-slate-900/30 border-r border-cyan-500/20 min-h-screen p-4`}>
          <nav className="space-y-2">
            <NavLink to="/" icon={<Activity />} label="SIEM Dashboard" />
            <NavLink to="/threat-intel" icon={<AlertTriangle />} label="Threat Intelligence" />
            <NavLink to="/network" icon={<Network />} label="Network Monitoring" />
            <NavLink to="/edr" icon={<Shield />} label="EDR" />
            <NavLink to="/compliance" icon={<FileSearch />} label="Compliance" />
            <NavLink to="/analytics" icon={<BarChart3 />} label="Analytics" />
            <NavLink to="/settings" icon={<Settings />} label="Settings" />
          </nav>
        </aside>

        {/* Main Content */}
        <main className="flex-1">
          <Routes>
            <Route path="/" element={<SIEMDashboardApp />} />
            <Route path="/threat-intel" element={<ThreatIntelligenceDashboard />} />
            <Route path="/network" element={<NetworkMonitoringDashboard />} />
            <Route path="/edr" element={<EDRDashboard />} />
            <Route path="/compliance" element={<ComplianceDashboard />} />
            <Route path="/analytics" element={<AnalyticsDashboard />} />
            <Route path="/settings" element={<SettingsPage />} />
          </Routes>
        </main>
      </div>

      <Toaster 
        position="top-right"
        toastOptions={{
          duration: 4000,
          style: {
            background: '#1e293b',
            color: '#ffffff',
            border: '1px solid rgb(34 211 238 / 0.2)',
          },
        }}
      />
    </div>
  );
}

function NavLink({ to, icon, label }: { to: string; icon: React.ReactNode; label: string }) {
  return (
    <Link
      to={to}
      className="flex items-center gap-3 px-4 py-3 text-slate-300 hover:text-cyan-400 hover:bg-cyan-500/10 rounded-lg transition-colors"
    >
      <span className="w-5 h-5">{icon}</span>
      <span>{label}</span>
    </Link>
  );
}

function LoginPage({ onLogin }: { onLogin: () => void }) {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [isRegister, setIsRegister] = useState(false);
  const [fullName, setFullName] = useState('');
  const [loading, setLoading] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);

    try {
      const endpoint = isRegister ? '/api/auth/register' : '/api/auth/login';
      const body = isRegister 
        ? { email, password, fullName }
        : { email, password };

      const res = await fetch(endpoint, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });

      const data = await res.json();

      if (res.ok) {
        localStorage.setItem('token', data.token);
        onLogin();
      } else {
        // Display user-friendly error messages
        const errorMessage = data.error || 'Authentication failed';
        console.error('Authentication error:', errorMessage);
        // TODO: Replace with toast notification for better UX
        alert(errorMessage);
      }
    } catch (error) {
      console.error('Auth error:', error);
      alert('Network error: Unable to connect to server. Please try again.');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-slate-950 flex items-center justify-center p-4">
      <div className="max-w-md w-full">
        <div className="text-center mb-8">
          <Shield className="w-16 h-16 text-cyan-400 mx-auto mb-4" />
          <h1 className="text-4xl font-bold text-cyan-400 mb-2">CyberRecon Suite</h1>
          <p className="text-slate-400">Enterprise Security Operations Platform</p>
        </div>

        <div className="bg-slate-900/50 border border-cyan-500/20 rounded-lg p-8">
          <h2 className="text-2xl font-bold text-white mb-6">
            {isRegister ? 'Create Account' : 'Sign In'}
          </h2>

          <form onSubmit={handleSubmit} className="space-y-4">
            {isRegister && (
              <div>
                <label className="block text-slate-400 mb-2">Full Name</label>
                <input
                  type="text"
                  value={fullName}
                  onChange={(e) => setFullName(e.target.value)}
                  className="w-full px-4 py-2 bg-slate-800 border border-slate-700 rounded-lg text-white focus:outline-none focus:border-cyan-400"
                  required={isRegister}
                />
              </div>
            )}

            <div>
              <label className="block text-slate-400 mb-2">Email</label>
              <input
                type="email"
                value={email}
                onChange={(e) => setEmail(e.target.value)}
                className="w-full px-4 py-2 bg-slate-800 border border-slate-700 rounded-lg text-white focus:outline-none focus:border-cyan-400"
                required
              />
            </div>

            <div>
              <label className="block text-slate-400 mb-2">Password</label>
              <input
                type="password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
                className="w-full px-4 py-2 bg-slate-800 border border-slate-700 rounded-lg text-white focus:outline-none focus:border-cyan-400"
                required
                minLength={8}
              />
            </div>

            <button
              type="submit"
              disabled={loading}
              className="w-full py-3 bg-cyan-500 text-white rounded-lg hover:bg-cyan-600 transition-colors disabled:opacity-50"
            >
              {loading ? 'Please wait...' : (isRegister ? 'Register' : 'Sign In')}
            </button>
          </form>

          <div className="mt-6 text-center">
            <button
              onClick={() => setIsRegister(!isRegister)}
              className="text-cyan-400 hover:text-cyan-300"
            >
              {isRegister ? 'Already have an account? Sign in' : "Don't have an account? Register"}
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}

// Placeholder components
function EDRDashboard() {
  return (
    <div className="p-6">
      <h1 className="text-3xl font-bold text-cyan-400 mb-4">EDR Dashboard</h1>
      <p className="text-slate-400">Endpoint Detection & Response dashboard coming soon...</p>
    </div>
  );
}

function ComplianceDashboard() {
  return (
    <div className="p-6">
      <h1 className="text-3xl font-bold text-cyan-400 mb-4">Compliance Dashboard</h1>
      <p className="text-slate-400">Compliance monitoring dashboard coming soon...</p>
    </div>
  );
}

function AnalyticsDashboard() {
  return (
    <div className="p-6">
      <h1 className="text-3xl font-bold text-cyan-400 mb-4">Analytics Dashboard</h1>
      <p className="text-slate-400">Advanced analytics coming soon...</p>
    </div>
  );
}

function SettingsPage() {
  return (
    <div className="p-6">
      <h1 className="text-3xl font-bold text-cyan-400 mb-4">Settings</h1>
      <p className="text-slate-400">Settings page coming soon...</p>
    </div>
  );
}

export default App;
