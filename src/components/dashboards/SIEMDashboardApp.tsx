import React, { useEffect, useState } from 'react';
import { Activity, AlertTriangle, Shield, TrendingUp } from 'lucide-react';

interface SIEMStats {
  total_events: number;
  critical_events: number;
  high_events: number;
  medium_events: number;
  low_events: number;
  unresolved_events: number;
  events_last_hour: number;
  events_last_24h: number;
}

interface SecurityEvent {
  id: number;
  event_type: string;
  severity: string;
  source: string;
  destination: string;
  description: string;
  timestamp: string;
  resolved: boolean;
}

export default function SIEMDashboardApp() {
  const [stats, setStats] = useState<SIEMStats | null>(null);
  const [events, setEvents] = useState<SecurityEvent[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchDashboardData();
    const interval = setInterval(fetchDashboardData, 10000); // Refresh every 10s
    return () => clearInterval(interval);
  }, []);

  const fetchDashboardData = async () => {
    try {
      const [dashboardRes, eventsRes] = await Promise.all([
        fetch('/api/siem/dashboard'),
        fetch('/api/siem/events?limit=10')
      ]);
      
      const dashboardData = await dashboardRes.json();
      const eventsData = await eventsRes.json();
      
      setStats(dashboardData.overview);
      setEvents(eventsData.events);
      setLoading(false);
    } catch (error) {
      console.error('Failed to fetch SIEM data:', error);
      setLoading(false);
    }
  };

  const getSeverityColor = (severity: string) => {
    switch (severity) {
      case 'critical': return 'text-red-500 bg-red-500/10 border-red-500/20';
      case 'high': return 'text-orange-500 bg-orange-500/10 border-orange-500/20';
      case 'medium': return 'text-yellow-500 bg-yellow-500/10 border-yellow-500/20';
      case 'low': return 'text-blue-500 bg-blue-500/10 border-blue-500/20';
      default: return 'text-gray-500 bg-gray-500/10 border-gray-500/20';
    }
  };

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen bg-slate-950">
        <div className="text-cyan-400 text-xl">Loading SIEM Dashboard...</div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-slate-950 text-white p-6">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="mb-8">
          <h1 className="text-4xl font-bold text-cyan-400 mb-2 flex items-center gap-3">
            <Shield className="w-10 h-10" />
            SIEM Dashboard
          </h1>
          <p className="text-slate-400">Security Information and Event Management</p>
        </div>

        {/* Stats Cards */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8">
          <div className="bg-slate-900/50 border border-cyan-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between mb-2">
              <Activity className="w-8 h-8 text-cyan-400" />
              <span className="text-sm text-slate-400">Total Events</span>
            </div>
            <div className="text-3xl font-bold text-cyan-400">{stats?.total_events || 0}</div>
            <div className="text-sm text-slate-400 mt-1">Last 24h: {stats?.events_last_24h || 0}</div>
          </div>

          <div className="bg-slate-900/50 border border-red-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between mb-2">
              <AlertTriangle className="w-8 h-8 text-red-400" />
              <span className="text-sm text-slate-400">Critical</span>
            </div>
            <div className="text-3xl font-bold text-red-400">{stats?.critical_events || 0}</div>
            <div className="text-sm text-slate-400 mt-1">High: {stats?.high_events || 0}</div>
          </div>

          <div className="bg-slate-900/50 border border-yellow-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between mb-2">
              <TrendingUp className="w-8 h-8 text-yellow-400" />
              <span className="text-sm text-slate-400">Unresolved</span>
            </div>
            <div className="text-3xl font-bold text-yellow-400">{stats?.unresolved_events || 0}</div>
            <div className="text-sm text-slate-400 mt-1">Needs attention</div>
          </div>

          <div className="bg-slate-900/50 border border-green-500/20 rounded-lg p-6">
            <div className="flex items-center justify-between mb-2">
              <Shield className="w-8 h-8 text-green-400" />
              <span className="text-sm text-slate-400">Last Hour</span>
            </div>
            <div className="text-3xl font-bold text-green-400">{stats?.events_last_hour || 0}</div>
            <div className="text-sm text-slate-400 mt-1">Recent activity</div>
          </div>
        </div>

        {/* Events Table */}
        <div className="bg-slate-900/50 border border-cyan-500/20 rounded-lg overflow-hidden">
          <div className="p-6 border-b border-cyan-500/20">
            <h2 className="text-2xl font-bold text-cyan-400">Recent Security Events</h2>
          </div>
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead className="bg-slate-900">
                <tr>
                  <th className="px-6 py-3 text-left text-xs font-medium text-slate-400 uppercase tracking-wider">
                    Timestamp
                  </th>
                  <th className="px-6 py-3 text-left text-xs font-medium text-slate-400 uppercase tracking-wider">
                    Event Type
                  </th>
                  <th className="px-6 py-3 text-left text-xs font-medium text-slate-400 uppercase tracking-wider">
                    Severity
                  </th>
                  <th className="px-6 py-3 text-left text-xs font-medium text-slate-400 uppercase tracking-wider">
                    Source
                  </th>
                  <th className="px-6 py-3 text-left text-xs font-medium text-slate-400 uppercase tracking-wider">
                    Destination
                  </th>
                  <th className="px-6 py-3 text-left text-xs font-medium text-slate-400 uppercase tracking-wider">
                    Status
                  </th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-800">
                {events.map((event) => (
                  <tr key={event.id} className="hover:bg-slate-800/30 transition-colors">
                    <td className="px-6 py-4 whitespace-nowrap text-sm text-slate-300">
                      {new Date(event.timestamp).toLocaleString()}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm text-slate-300">
                      {event.event_type}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm">
                      <span className={`px-2 py-1 rounded-full text-xs font-medium border ${getSeverityColor(event.severity)}`}>
                        {event.severity.toUpperCase()}
                      </span>
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm font-mono text-slate-300">
                      {event.source}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm font-mono text-slate-300">
                      {event.destination}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm">
                      <span className={`px-2 py-1 rounded-full text-xs font-medium ${
                        event.resolved 
                          ? 'text-green-400 bg-green-500/10' 
                          : 'text-yellow-400 bg-yellow-500/10'
                      }`}>
                        {event.resolved ? 'Resolved' : 'Open'}
                      </span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      </div>
    </div>
  );
}
