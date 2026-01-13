import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import {
  Settings, User, Bell, Shield, Lock, Key, Save, RefreshCw,
  Eye, EyeOff, Plus, Trash2, Copy, Check, Server, Database
} from 'lucide-react';
import toast from 'react-hot-toast';

interface SettingsData {
  profile: {
    fullName: string;
    email: string;
    role: string;
    department: string;
    timezone: string;
    language: string;
  };
  notifications: {
    email: {
      enabled: boolean;
      frequency: string;
      types: string[];
    };
    sms: {
      enabled: boolean;
      phone: string;
    };
    desktop: {
      enabled: boolean;
      types: string[];
    };
  };
  security: {
    twoFactorAuth: boolean;
    sessionTimeout: number;
    passwordExpiry: number;
    loginNotifications: boolean;
  };
  dashboard: {
    theme: string;
    defaultView: string;
    refreshInterval: number;
    widgets: string[];
  };
}

interface ApiKey {
  id: number;
  name: string;
  key: string;
  created: string;
  lastUsed: string | null;
  status: string;
}

export default function SettingsPage() {
  const [activeTab, setActiveTab] = useState('profile');
  const [settings, setSettings] = useState<SettingsData | null>(null);
  const [apiKeys, setApiKeys] = useState<ApiKey[]>([]);
  const [loading, setLoading] = useState(true);
  const [saving, setSaving] = useState(false);
  const [copiedKey, setCopiedKey] = useState<number | null>(null);
  const [newKeyName, setNewKeyName] = useState('');
  const [showNewKeyDialog, setShowNewKeyDialog] = useState(false);
  const [confirmRevokeKeyId, setConfirmRevokeKeyId] = useState<number | null>(null);

  useEffect(() => {
    fetchSettings();
    fetchApiKeys();
  }, []);

  const fetchSettings = async () => {
    try {
      setLoading(true);
      const response = await fetch('/api/settings/profile');
      const data = await response.json();
      setSettings(data);
    } catch (error) {
      console.error('Failed to fetch settings:', error);
      toast.error('Failed to load settings');
    } finally {
      setLoading(false);
    }
  };

  const fetchApiKeys = async () => {
    try {
      const response = await fetch('/api/settings/api-keys');
      const data = await response.json();
      setApiKeys(data.apiKeys || []);
    } catch (error) {
      console.error('Failed to fetch API keys:', error);
    }
  };

  const handleSaveProfile = async () => {
    if (!settings) return;
    
    try {
      setSaving(true);
      const response = await fetch('/api/settings/profile', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(settings.profile),
      });
      
      if (response.ok) {
        toast.success('Profile updated successfully');
      } else {
        toast.error('Failed to update profile');
      }
    } catch (error) {
      console.error('Failed to save profile:', error);
      toast.error('Failed to update profile');
    } finally {
      setSaving(false);
    }
  };

  const handleSaveNotifications = async () => {
    if (!settings) return;
    
    try {
      setSaving(true);
      const response = await fetch('/api/settings/notifications', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(settings.notifications),
      });
      
      if (response.ok) {
        toast.success('Notification settings updated');
      } else {
        toast.error('Failed to update notifications');
      }
    } catch (error) {
      console.error('Failed to save notifications:', error);
      toast.error('Failed to update notifications');
    } finally {
      setSaving(false);
    }
  };

  const handleSaveSecurity = async () => {
    if (!settings) return;
    
    try {
      setSaving(true);
      const response = await fetch('/api/settings/security', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(settings.security),
      });
      
      if (response.ok) {
        toast.success('Security settings updated');
      } else {
        toast.error('Failed to update security settings');
      }
    } catch (error) {
      console.error('Failed to save security:', error);
      toast.error('Failed to update security settings');
    } finally {
      setSaving(false);
    }
  };

  const handleSaveDashboard = async () => {
    if (!settings) return;
    
    try {
      setSaving(true);
      const response = await fetch('/api/settings/dashboard', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(settings.dashboard),
      });
      
      if (response.ok) {
        toast.success('Dashboard settings updated');
      } else {
        toast.error('Failed to update dashboard settings');
      }
    } catch (error) {
      console.error('Failed to save dashboard:', error);
      toast.error('Failed to update dashboard settings');
    } finally {
      setSaving(false);
    }
  };

  const handleGenerateApiKey = async () => {
    if (!newKeyName.trim()) {
      toast.error('Please enter a name for the API key');
      return;
    }

    try {
      const response = await fetch('/api/settings/api-keys', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ name: newKeyName }),
      });
      
      const data = await response.json();
      
      if (response.ok) {
        toast.success('API key generated successfully');
        setApiKeys([...apiKeys, data.apiKey]);
        setNewKeyName('');
        setShowNewKeyDialog(false);
      } else {
        toast.error('Failed to generate API key');
      }
    } catch (error) {
      console.error('Failed to generate API key:', error);
      toast.error('Failed to generate API key');
    }
  };

  const handleRevokeApiKey = async (id: number) => {
    try {
      const response = await fetch(`/api/settings/api-keys/${id}`, {
        method: 'DELETE',
      });
      
      if (response.ok) {
        toast.success('API key revoked successfully');
        setApiKeys(apiKeys.filter(key => key.id !== id));
        setConfirmRevokeKeyId(null);
      } else {
        toast.error('Failed to revoke API key');
      }
    } catch (error) {
      console.error('Failed to revoke API key:', error);
      toast.error('Failed to revoke API key');
    }
  };

  const handleCopyKey = (keyId: number, key: string) => {
    navigator.clipboard.writeText(key);
    setCopiedKey(keyId);
    toast.success('API key copied to clipboard');
    setTimeout(() => setCopiedKey(null), 2000);
  };

  if (loading) {
    return (
      <div className="min-h-screen bg-slate-900 text-white p-6 flex items-center justify-center">
        <div className="text-center">
          <RefreshCw className="w-12 h-12 text-cyan-400 animate-spin mx-auto mb-4" />
          <p className="text-slate-400">Loading settings...</p>
        </div>
      </div>
    );
  }

  if (!settings) {
    return (
      <div className="min-h-screen bg-slate-900 text-white p-6">
        <p className="text-slate-400">Failed to load settings</p>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-slate-900 text-white p-6">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="mb-8">
          <h1 className="text-3xl font-bold text-cyan-400 mb-2 flex items-center gap-3">
            <Settings className="w-8 h-8" />
            Settings
          </h1>
          <p className="text-slate-400">Manage your account and system preferences</p>
        </div>

        {/* Tabs */}
        <div className="flex gap-4 mb-6 border-b border-slate-700">
          <TabButton
            active={activeTab === 'profile'}
            onClick={() => setActiveTab('profile')}
            icon={<User className="w-4 h-4" />}
            label="Profile"
          />
          <TabButton
            active={activeTab === 'notifications'}
            onClick={() => setActiveTab('notifications')}
            icon={<Bell className="w-4 h-4" />}
            label="Notifications"
          />
          <TabButton
            active={activeTab === 'security'}
            onClick={() => setActiveTab('security')}
            icon={<Shield className="w-4 h-4" />}
            label="Security"
          />
          <TabButton
            active={activeTab === 'dashboard'}
            onClick={() => setActiveTab('dashboard')}
            icon={<Server className="w-4 h-4" />}
            label="Dashboard"
          />
          <TabButton
            active={activeTab === 'api'}
            onClick={() => setActiveTab('api')}
            icon={<Key className="w-4 h-4" />}
            label="API Keys"
          />
        </div>

        {/* Content */}
        <div className="bg-slate-800/50 border border-slate-700 rounded-lg p-6">
          {activeTab === 'profile' && (
            <ProfileSettings settings={settings} setSettings={setSettings} onSave={handleSaveProfile} saving={saving} />
          )}
          {activeTab === 'notifications' && (
            <NotificationSettings settings={settings} setSettings={setSettings} onSave={handleSaveNotifications} saving={saving} />
          )}
          {activeTab === 'security' && (
            <SecuritySettings settings={settings} setSettings={setSettings} onSave={handleSaveSecurity} saving={saving} />
          )}
          {activeTab === 'dashboard' && (
            <DashboardSettings settings={settings} setSettings={setSettings} onSave={handleSaveDashboard} saving={saving} />
          )}
          {activeTab === 'api' && (
            <ApiKeySettings
              apiKeys={apiKeys}
              onGenerate={handleGenerateApiKey}
              onRevoke={handleRevokeApiKey}
              onCopy={handleCopyKey}
              copiedKey={copiedKey}
              newKeyName={newKeyName}
              setNewKeyName={setNewKeyName}
              showDialog={showNewKeyDialog}
              setShowDialog={setShowNewKeyDialog}
              confirmRevokeKeyId={confirmRevokeKeyId}
              setConfirmRevokeKeyId={setConfirmRevokeKeyId}
            />
          )}
        </div>
      </div>
    </div>
  );
}

function TabButton({ active, onClick, icon, label }: {
  active: boolean;
  onClick: () => void;
  icon: React.ReactNode;
  label: string;
}) {
  return (
    <button
      onClick={onClick}
      className={`flex items-center gap-2 px-4 py-3 border-b-2 transition-colors ${
        active
          ? 'border-cyan-400 text-cyan-400'
          : 'border-transparent text-slate-400 hover:text-slate-300'
      }`}
    >
      {icon}
      <span>{label}</span>
    </button>
  );
}

function ProfileSettings({ settings, setSettings, onSave, saving }: {
  settings: SettingsData;
  setSettings: (s: SettingsData) => void;
  onSave: () => void;
  saving: boolean;
}) {
  return (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold text-white mb-4">Profile Information</h2>
      
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Full Name</label>
          <input
            type="text"
            value={settings.profile.fullName}
            onChange={(e) => setSettings({
              ...settings,
              profile: { ...settings.profile, fullName: e.target.value }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          />
        </div>
        
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Email</label>
          <input
            type="email"
            value={settings.profile.email}
            onChange={(e) => setSettings({
              ...settings,
              profile: { ...settings.profile, email: e.target.value }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          />
        </div>
        
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Department</label>
          <input
            type="text"
            value={settings.profile.department}
            onChange={(e) => setSettings({
              ...settings,
              profile: { ...settings.profile, department: e.target.value }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          />
        </div>
        
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Timezone</label>
          <select
            value={settings.profile.timezone}
            onChange={(e) => setSettings({
              ...settings,
              profile: { ...settings.profile, timezone: e.target.value }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          >
            <option value="UTC-5">UTC-5 (Eastern)</option>
            <option value="UTC-6">UTC-6 (Central)</option>
            <option value="UTC-7">UTC-7 (Mountain)</option>
            <option value="UTC-8">UTC-8 (Pacific)</option>
            <option value="UTC">UTC</option>
          </select>
        </div>
      </div>
      
      <div className="flex justify-end">
        <button
          onClick={onSave}
          disabled={saving}
          className="flex items-center gap-2 px-6 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700 disabled:opacity-50 disabled:cursor-not-allowed"
        >
          {saving ? <RefreshCw className="w-4 h-4 animate-spin" /> : <Save className="w-4 h-4" />}
          {saving ? 'Saving...' : 'Save Changes'}
        </button>
      </div>
    </div>
  );
}

function NotificationSettings({ settings, setSettings, onSave, saving }: {
  settings: SettingsData;
  setSettings: (s: SettingsData) => void;
  onSave: () => void;
  saving: boolean;
}) {
  return (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold text-white mb-4">Notification Preferences</h2>
      
      <div className="space-y-6">
        <div className="border-b border-slate-700 pb-4">
          <div className="flex items-center justify-between mb-4">
            <div>
              <h3 className="text-lg font-semibold text-white">Email Notifications</h3>
              <p className="text-sm text-slate-400">Receive alerts via email</p>
            </div>
            <label className="relative inline-flex items-center cursor-pointer">
              <input
                type="checkbox"
                checked={settings.notifications.email.enabled}
                onChange={(e) => setSettings({
                  ...settings,
                  notifications: {
                    ...settings.notifications,
                    email: { ...settings.notifications.email, enabled: e.target.checked }
                  }
                })}
                className="sr-only peer"
              />
              <div className="w-11 h-6 bg-slate-700 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-cyan-800 rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-cyan-600"></div>
            </label>
          </div>
        </div>
        
        <div className="border-b border-slate-700 pb-4">
          <div className="flex items-center justify-between mb-4">
            <div>
              <h3 className="text-lg font-semibold text-white">Desktop Notifications</h3>
              <p className="text-sm text-slate-400">Receive browser notifications</p>
            </div>
            <label className="relative inline-flex items-center cursor-pointer">
              <input
                type="checkbox"
                checked={settings.notifications.desktop.enabled}
                onChange={(e) => setSettings({
                  ...settings,
                  notifications: {
                    ...settings.notifications,
                    desktop: { ...settings.notifications.desktop, enabled: e.target.checked }
                  }
                })}
                className="sr-only peer"
              />
              <div className="w-11 h-6 bg-slate-700 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-cyan-800 rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-cyan-600"></div>
            </label>
          </div>
        </div>
      </div>
      
      <div className="flex justify-end">
        <button
          onClick={onSave}
          disabled={saving}
          className="flex items-center gap-2 px-6 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700 disabled:opacity-50 disabled:cursor-not-allowed"
        >
          {saving ? <RefreshCw className="w-4 h-4 animate-spin" /> : <Save className="w-4 h-4" />}
          {saving ? 'Saving...' : 'Save Changes'}
        </button>
      </div>
    </div>
  );
}

function SecuritySettings({ settings, setSettings, onSave, saving }: {
  settings: SettingsData;
  setSettings: (s: SettingsData) => void;
  onSave: () => void;
  saving: boolean;
}) {
  return (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold text-white mb-4">Security Settings</h2>
      
      <div className="space-y-6">
        <div className="flex items-center justify-between border-b border-slate-700 pb-4">
          <div>
            <h3 className="text-lg font-semibold text-white">Two-Factor Authentication</h3>
            <p className="text-sm text-slate-400">Add an extra layer of security</p>
          </div>
          <label className="relative inline-flex items-center cursor-pointer">
            <input
              type="checkbox"
              checked={settings.security.twoFactorAuth}
              onChange={(e) => setSettings({
                ...settings,
                security: { ...settings.security, twoFactorAuth: e.target.checked }
              })}
              className="sr-only peer"
            />
            <div className="w-11 h-6 bg-slate-700 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-cyan-800 rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-cyan-600"></div>
          </label>
        </div>
        
        <div className="border-b border-slate-700 pb-4">
          <label className="block text-sm font-medium text-slate-300 mb-2">Session Timeout (minutes)</label>
          <input
            type="number"
            value={settings.security.sessionTimeout}
            onChange={(e) => setSettings({
              ...settings,
              security: { ...settings.security, sessionTimeout: Number(e.target.value) }
            })}
            className="w-full md:w-1/3 px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          />
        </div>
        
        <div className="flex items-center justify-between">
          <div>
            <h3 className="text-lg font-semibold text-white">Login Notifications</h3>
            <p className="text-sm text-slate-400">Get notified of new login attempts</p>
          </div>
          <label className="relative inline-flex items-center cursor-pointer">
            <input
              type="checkbox"
              checked={settings.security.loginNotifications}
              onChange={(e) => setSettings({
                ...settings,
                security: { ...settings.security, loginNotifications: e.target.checked }
              })}
              className="sr-only peer"
            />
            <div className="w-11 h-6 bg-slate-700 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-cyan-800 rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-cyan-600"></div>
          </label>
        </div>
      </div>
      
      <div className="flex justify-end">
        <button
          onClick={onSave}
          disabled={saving}
          className="flex items-center gap-2 px-6 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700 disabled:opacity-50 disabled:cursor-not-allowed"
        >
          {saving ? <RefreshCw className="w-4 h-4 animate-spin" /> : <Save className="w-4 h-4" />}
          {saving ? 'Saving...' : 'Save Changes'}
        </button>
      </div>
    </div>
  );
}

function DashboardSettings({ settings, setSettings, onSave, saving }: {
  settings: SettingsData;
  setSettings: (s: SettingsData) => void;
  onSave: () => void;
  saving: boolean;
}) {
  return (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold text-white mb-4">Dashboard Preferences</h2>
      
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Theme</label>
          <select
            value={settings.dashboard.theme}
            onChange={(e) => setSettings({
              ...settings,
              dashboard: { ...settings.dashboard, theme: e.target.value }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          >
            <option value="dark">Dark</option>
            <option value="light">Light</option>
            <option value="auto">Auto</option>
          </select>
        </div>
        
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Default View</label>
          <select
            value={settings.dashboard.defaultView}
            onChange={(e) => setSettings({
              ...settings,
              dashboard: { ...settings.dashboard, defaultView: e.target.value }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          >
            <option value="siem">SIEM Dashboard</option>
            <option value="threat-intel">Threat Intelligence</option>
            <option value="network">Network Monitoring</option>
            <option value="edr">EDR</option>
            <option value="compliance">Compliance</option>
          </select>
        </div>
        
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-2">Refresh Interval (seconds)</label>
          <input
            type="number"
            value={settings.dashboard.refreshInterval}
            onChange={(e) => setSettings({
              ...settings,
              dashboard: { ...settings.dashboard, refreshInterval: Number(e.target.value) }
            })}
            className="w-full px-4 py-2 bg-slate-900 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none"
          />
        </div>
      </div>
      
      <div className="flex justify-end">
        <button
          onClick={onSave}
          disabled={saving}
          className="flex items-center gap-2 px-6 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700 disabled:opacity-50 disabled:cursor-not-allowed"
        >
          {saving ? <RefreshCw className="w-4 h-4 animate-spin" /> : <Save className="w-4 h-4" />}
          {saving ? 'Saving...' : 'Save Changes'}
        </button>
      </div>
    </div>
  );
}

function ApiKeySettings({ apiKeys, onGenerate, onRevoke, onCopy, copiedKey, newKeyName, setNewKeyName, showDialog, setShowDialog, confirmRevokeKeyId, setConfirmRevokeKeyId }: {
  apiKeys: ApiKey[];
  onGenerate: () => void;
  onRevoke: (id: number) => void;
  onCopy: (id: number, key: string) => void;
  copiedKey: number | null;
  newKeyName: string;
  setNewKeyName: (name: string) => void;
  showDialog: boolean;
  setShowDialog: (show: boolean) => void;
  confirmRevokeKeyId: number | null;
  setConfirmRevokeKeyId: (id: number | null) => void;
}) {
  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h2 className="text-2xl font-bold text-white mb-2">API Keys</h2>
          <p className="text-slate-400">Manage API keys for integrations</p>
        </div>
        <button
          onClick={() => setShowDialog(true)}
          className="flex items-center gap-2 px-4 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700"
        >
          <Plus className="w-4 h-4" />
          Generate New Key
        </button>
      </div>
      
      {showDialog && (
        <div className="bg-slate-900 border border-slate-700 rounded-lg p-4">
          <h3 className="text-lg font-semibold text-white mb-3">Generate New API Key</h3>
          <input
            type="text"
            value={newKeyName}
            onChange={(e) => setNewKeyName(e.target.value)}
            placeholder="Enter key name (e.g., Production API)"
            className="w-full px-4 py-2 bg-slate-800 border border-slate-700 rounded-lg text-white focus:border-cyan-400 focus:outline-none mb-3"
          />
          <div className="flex gap-2">
            <button
              onClick={onGenerate}
              className="px-4 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700"
            >
              Generate
            </button>
            <button
              onClick={() => {
                setShowDialog(false);
                setNewKeyName('');
              }}
              className="px-4 py-2 bg-slate-700 rounded-lg hover:bg-slate-600"
            >
              Cancel
            </button>
          </div>
        </div>
      )}
      
      <div className="space-y-4">
        {apiKeys.map((key) => (
          <div
            key={key.id}
            className="bg-slate-900 border border-slate-700 rounded-lg p-4"
          >
            <div className="flex items-center justify-between mb-3">
              <div>
                <h3 className="text-lg font-semibold text-white">{key.name}</h3>
                <p className="text-sm text-slate-400">Created: {new Date(key.created).toLocaleDateString()}</p>
              </div>
              <span className={`px-3 py-1 rounded-full text-xs font-semibold ${
                key.status === 'active' ? 'bg-green-500/20 text-green-400' : 'bg-red-500/20 text-red-400'
              }`}>
                {key.status.toUpperCase()}
              </span>
            </div>
            
            <div className="flex items-center gap-2 mb-3">
              <code className="flex-1 px-3 py-2 bg-slate-800 rounded text-slate-300 text-sm font-mono">
                {key.key}
              </code>
              <button
                onClick={() => onCopy(key.id, key.key)}
                className="p-2 bg-slate-800 rounded hover:bg-slate-700"
              >
                {copiedKey === key.id ? (
                  <Check className="w-4 h-4 text-green-400" />
                ) : (
                  <Copy className="w-4 h-4" />
                )}
              </button>
            </div>
            
            <div className="flex items-center justify-between">
              <p className="text-sm text-slate-400">
                Last used: {key.lastUsed ? new Date(key.lastUsed).toLocaleString() : 'Never'}
              </p>
              {confirmRevokeKeyId === key.id ? (
                <div className="flex gap-2">
                  <button
                    onClick={() => onRevoke(key.id)}
                    className="flex items-center gap-2 px-3 py-1 text-sm bg-red-600 text-white rounded hover:bg-red-700"
                  >
                    Confirm Revoke
                  </button>
                  <button
                    onClick={() => setConfirmRevokeKeyId(null)}
                    className="flex items-center gap-2 px-3 py-1 text-sm bg-slate-700 text-white rounded hover:bg-slate-600"
                  >
                    Cancel
                  </button>
                </div>
              ) : (
                <button
                  onClick={() => setConfirmRevokeKeyId(key.id)}
                  className="flex items-center gap-2 px-3 py-1 text-sm text-red-400 hover:text-red-300"
                >
                  <Trash2 className="w-4 h-4" />
                  Revoke
                </button>
              )}
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
