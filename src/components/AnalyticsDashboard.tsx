import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import {
  BarChart3, TrendingUp, TrendingDown, Activity, Shield,
  AlertTriangle, CheckCircle, Download, RefreshCw, Calendar
} from 'lucide-react';
import {
  LineChart, Line, BarChart, Bar, PieChart, Pie, Cell,
  XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer
} from 'recharts';

interface AnalyticsData {
  summary: {
    totalEvents: number;
    activeThreats: number;
    blockedAttacks: number;
    complianceScore: number;
    avgResponseTime: string;
  };
  eventTrends: {
    daily: Array<{
      date: string;
      events: number;
      threats: number;
      blocked: number;
    }>;
  };
  topThreats: Array<{
    type: string;
    count: number;
    percentage: number;
    trend: string;
  }>;
  securityMetrics: {
    vulnerabilities: {
      critical: number;
      high: number;
      medium: number;
      low: number;
    };
    compliance: {
      [key: string]: number;
    };
  };
}

export default function AnalyticsDashboard() {
  const [analyticsData, setAnalyticsData] = useState<AnalyticsData | null>(null);
  const [loading, setLoading] = useState(true);
  const [timeframe, setTimeframe] = useState('7d');

  useEffect(() => {
    fetchAnalyticsData();
  }, [timeframe]);

  const fetchAnalyticsData = async () => {
    try {
      setLoading(true);
      const response = await fetch('/api/analytics/dashboard');
      const data = await response.json();
      setAnalyticsData(data);
    } catch (error) {
      console.error('Failed to fetch analytics:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleExport = async () => {
    try {
      const response = await fetch('/api/analytics/export', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ format: 'json', dateRange: timeframe }),
      });
      const data = await response.json();
      alert('Report generated successfully!');
    } catch (error) {
      console.error('Failed to export analytics:', error);
    }
  };

  if (loading) {
    return (
      <div className="min-h-screen bg-slate-900 text-white p-6 flex items-center justify-center">
        <div className="text-center">
          <RefreshCw className="w-12 h-12 text-cyan-400 animate-spin mx-auto mb-4" />
          <p className="text-slate-400">Loading analytics...</p>
        </div>
      </div>
    );
  }

  if (!analyticsData) {
    return (
      <div className="min-h-screen bg-slate-900 text-white p-6">
        <p className="text-slate-400">Failed to load analytics data</p>
      </div>
    );
  }

  const COLORS = ['#22d3ee', '#3b82f6', '#8b5cf6', '#ec4899', '#f59e0b'];

  const vulnerabilityData = [
    { name: 'Critical', value: analyticsData.securityMetrics.vulnerabilities.critical, color: '#ef4444' },
    { name: 'High', value: analyticsData.securityMetrics.vulnerabilities.high, color: '#f97316' },
    { name: 'Medium', value: analyticsData.securityMetrics.vulnerabilities.medium, color: '#eab308' },
    { name: 'Low', value: analyticsData.securityMetrics.vulnerabilities.low, color: '#22c55e' },
  ];

  const complianceData = Object.entries(analyticsData.securityMetrics.compliance).map(([name, score]) => ({
    name,
    score,
  }));

  return (
    <div className="min-h-screen bg-slate-900 text-white p-6">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <div>
            <h1 className="text-3xl font-bold text-cyan-400 mb-2 flex items-center gap-3">
              <BarChart3 className="w-8 h-8" />
              Advanced Analytics
            </h1>
            <p className="text-slate-400">Real-time security metrics and trend analysis</p>
          </div>
          <div className="flex gap-3">
            <select
              value={timeframe}
              onChange={(e) => setTimeframe(e.target.value)}
              className="px-4 py-2 bg-slate-800 border border-slate-700 rounded-lg text-white"
            >
              <option value="24h">Last 24 Hours</option>
              <option value="7d">Last 7 Days</option>
              <option value="30d">Last 30 Days</option>
              <option value="90d">Last 90 Days</option>
            </select>
            <button
              onClick={fetchAnalyticsData}
              className="px-4 py-2 bg-slate-800 border border-slate-700 rounded-lg hover:bg-slate-700 flex items-center gap-2"
            >
              <RefreshCw className="w-4 h-4" />
              Refresh
            </button>
            <button
              onClick={handleExport}
              className="px-4 py-2 bg-cyan-600 rounded-lg hover:bg-cyan-700 flex items-center gap-2"
            >
              <Download className="w-4 h-4" />
              Export Report
            </button>
          </div>
        </div>

        {/* Summary Cards */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-5 gap-4 mb-8">
          <StatCard
            title="Total Events"
            value={analyticsData.summary.totalEvents.toLocaleString()}
            icon={<Activity className="w-6 h-6" />}
            color="cyan"
          />
          <StatCard
            title="Active Threats"
            value={analyticsData.summary.activeThreats}
            icon={<AlertTriangle className="w-6 h-6" />}
            color="red"
          />
          <StatCard
            title="Blocked Attacks"
            value={analyticsData.summary.blockedAttacks}
            icon={<Shield className="w-6 h-6" />}
            color="green"
          />
          <StatCard
            title="Compliance Score"
            value={`${analyticsData.summary.complianceScore}%`}
            icon={<CheckCircle className="w-6 h-6" />}
            color="blue"
          />
          <StatCard
            title="Avg Response Time"
            value={analyticsData.summary.avgResponseTime}
            icon={<TrendingUp className="w-6 h-6" />}
            color="purple"
          />
        </div>

        {/* Charts */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
          {/* Event Trends Chart */}
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            className="bg-slate-800/50 border border-slate-700 rounded-lg p-6"
          >
            <h3 className="text-xl font-bold text-white mb-4">Event Trends</h3>
            <ResponsiveContainer width="100%" height={300}>
              <LineChart data={analyticsData.eventTrends.daily}>
                <CartesianGrid strokeDasharray="3 3" stroke="#334155" />
                <XAxis dataKey="date" stroke="#94a3b8" />
                <YAxis stroke="#94a3b8" />
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#1e293b',
                    border: '1px solid #334155',
                    borderRadius: '8px',
                  }}
                />
                <Legend />
                <Line
                  type="monotone"
                  dataKey="events"
                  stroke="#22d3ee"
                  strokeWidth={2}
                  name="Events"
                />
                <Line
                  type="monotone"
                  dataKey="threats"
                  stroke="#ef4444"
                  strokeWidth={2}
                  name="Threats"
                />
                <Line
                  type="monotone"
                  dataKey="blocked"
                  stroke="#22c55e"
                  strokeWidth={2}
                  name="Blocked"
                />
              </LineChart>
            </ResponsiveContainer>
          </motion.div>

          {/* Vulnerability Distribution */}
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.1 }}
            className="bg-slate-800/50 border border-slate-700 rounded-lg p-6"
          >
            <h3 className="text-xl font-bold text-white mb-4">Vulnerability Distribution</h3>
            <ResponsiveContainer width="100%" height={300}>
              <PieChart>
                <Pie
                  data={vulnerabilityData}
                  cx="50%"
                  cy="50%"
                  labelLine={false}
                  label={({ name, percent }) => `${name}: ${(percent * 100).toFixed(0)}%`}
                  outerRadius={100}
                  fill="#8884d8"
                  dataKey="value"
                >
                  {vulnerabilityData.map((entry, index) => (
                    <Cell key={`cell-${index}`} fill={entry.color} />
                  ))}
                </Pie>
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#1e293b',
                    border: '1px solid #334155',
                    borderRadius: '8px',
                  }}
                />
              </PieChart>
            </ResponsiveContainer>
          </motion.div>

          {/* Compliance Scores */}
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.2 }}
            className="bg-slate-800/50 border border-slate-700 rounded-lg p-6"
          >
            <h3 className="text-xl font-bold text-white mb-4">Compliance Scores</h3>
            <ResponsiveContainer width="100%" height={300}>
              <BarChart data={complianceData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#334155" />
                <XAxis dataKey="name" stroke="#94a3b8" />
                <YAxis stroke="#94a3b8" domain={[0, 100]} />
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#1e293b',
                    border: '1px solid #334155',
                    borderRadius: '8px',
                  }}
                />
                <Bar dataKey="score" fill="#22d3ee" />
              </BarChart>
            </ResponsiveContainer>
          </motion.div>

          {/* Top Threats */}
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.3 }}
            className="bg-slate-800/50 border border-slate-700 rounded-lg p-6"
          >
            <h3 className="text-xl font-bold text-white mb-4">Top Threats</h3>
            <div className="space-y-4">
              {analyticsData.topThreats.map((threat, index) => (
                <div key={index} className="flex items-center justify-between">
                  <div className="flex-1">
                    <div className="flex items-center justify-between mb-1">
                      <span className="text-slate-300 font-medium">{threat.type}</span>
                      <span className="text-slate-400 text-sm">{threat.count}</span>
                    </div>
                    <div className="w-full bg-slate-700 rounded-full h-2">
                      <div
                        className="bg-cyan-500 h-2 rounded-full"
                        style={{ width: `${threat.percentage}%` }}
                      />
                    </div>
                  </div>
                  <div className="ml-4">
                    {threat.trend === 'up' ? (
                      <TrendingUp className="w-4 h-4 text-red-400" />
                    ) : threat.trend === 'down' ? (
                      <TrendingDown className="w-4 h-4 text-green-400" />
                    ) : (
                      <Activity className="w-4 h-4 text-slate-400" />
                    )}
                  </div>
                </div>
              ))}
            </div>
          </motion.div>
        </div>
      </div>
    </div>
  );
}

function StatCard({ title, value, icon, color }: {
  title: string;
  value: string | number;
  icon: React.ReactNode;
  color: string;
}) {
  const colorClasses: { [key: string]: string } = {
    cyan: 'text-cyan-400 bg-cyan-500/10',
    red: 'text-red-400 bg-red-500/10',
    green: 'text-green-400 bg-green-500/10',
    blue: 'text-blue-400 bg-blue-500/10',
    purple: 'text-purple-400 bg-purple-500/10',
  };

  return (
    <motion.div
      initial={{ opacity: 0, scale: 0.9 }}
      animate={{ opacity: 1, scale: 1 }}
      className="bg-slate-800/50 border border-slate-700 rounded-lg p-4"
    >
      <div className="flex items-center justify-between mb-2">
        <span className="text-slate-400 text-sm">{title}</span>
        <div className={`p-2 rounded-lg ${colorClasses[color]}`}>
          {icon}
        </div>
      </div>
      <div className="text-2xl font-bold text-white">{value}</div>
    </motion.div>
  );
}
