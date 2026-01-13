import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Laptop, Shield, AlertTriangle, Activity, CheckCircle, Smartphone, Server } from 'lucide-react'

interface Endpoint {
    id: string
    name: string
    type: 'workstation' | 'server' | 'mobile'
    status: 'online' | 'offline' | 'compromised' | 'scanning'
    ip: string
    os: string
    lastCheckin: Date
    riskLevel: 'low' | 'medium' | 'high' | 'critical'
    healthScore: number
}

export default function EDRDashboard() {
    const [endpoints, setEndpoints] = useState<Endpoint[]>([])

    // Simulation of EDR data (In real implementation, fetch from /api/edr/endpoints)
    useEffect(() => {
        // Generate some dummy real-time data for the view
        const generateEndpoints = () => {
            const types: Array<'workstation' | 'server' | 'mobile'> = ['workstation', 'server', 'mobile']
            const statuses: Array<'online' | 'offline' | 'compromised' | 'scanning'> = ['online', 'online', 'online', 'scanning', 'offline', 'compromised']

            return Array.from({ length: 12 }).map((_, i) => ({
                id: `ep-${i}`,
                name: `HOST-${Math.random().toString(36).substr(2, 5).toUpperCase()}`,
                type: types[i % 3],
                status: statuses[Math.floor(Math.random() * statuses.length)],
                ip: `10.0.${Math.floor(i / 255)}.${i % 255}`,
                os: i % 3 === 0 ? 'Windows 11' : i % 3 === 1 ? 'Ubuntu 22.04' : 'macOS 14',
                lastCheckin: new Date(),
                riskLevel: Math.random() > 0.8 ? 'high' : 'low',
                healthScore: Math.floor(Math.random() * 20) + 80
            })) as Endpoint[]
        }
        setEndpoints(generateEndpoints())
    }, [])

    const getStatusColor = (status: string) => {
        switch (status) {
            case 'online': return 'text-green-400 bg-green-500/10'
            case 'offline': return 'text-slate-400 bg-slate-500/10'
            case 'compromised': return 'text-red-400 bg-red-500/10 animate-pulse'
            case 'scanning': return 'text-blue-400 bg-blue-500/10'
            default: return 'text-slate-400'
        }
    }

    return (
        <div className="min-h-screen bg-slate-900 text-white p-6">
            <div className="max-w-7xl mx-auto">
                <div className="flex items-center justify-between mb-8">
                    <div>
                        <h1 className="text-3xl font-bold text-primary-400 mb-2 flex items-center gap-3">
                            <Laptop className="w-8 h-8" />
                            Endpoint Detection & Response
                        </h1>
                        <p className="text-gray-400">Device fleet monitoring and automated response</p>
                    </div>
                    <div className="flex gap-2">
                        <button className="px-4 py-2 bg-slate-800 rounded border border-slate-700 hover:bg-slate-700">Scan All</button>
                        <button className="px-4 py-2 bg-primary-600 rounded hover:bg-primary-700">Install Agent</button>
                    </div>
                </div>

                <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                    {endpoints.map((ep) => (
                        <motion.div
                            key={ep.id}
                            initial={{ opacity: 0, scale: 0.9 }}
                            animate={{ opacity: 1, scale: 1 }}
                            className={`border rounded-lg p-4 ${ep.riskLevel === 'high' ? 'border-red-500/50 bg-red-900/10' : 'border-slate-700 bg-slate-800/20'}`}
                        >
                            <div className="flex justify-between items-start mb-4">
                                <div className="flex items-center gap-3">
                                    <div className={`p-2 rounded-lg ${getStatusColor(ep.status)}`}>
                                        {ep.type === 'mobile' ? <Smartphone size={20} /> : ep.type === 'server' ? <Server size={20} /> : <Laptop size={20} />}
                                    </div>
                                    <div>
                                        <h3 className="font-bold text-slate-200">{ep.name}</h3>
                                        <p className="text-xs text-slate-500">{ep.ip}</p>
                                    </div>
                                </div>
                                <span className={`px-2 py-1 text-xs rounded font-medium ${getStatusColor(ep.status)}`}>
                                    {ep.status.toUpperCase()}
                                </span>
                            </div>

                            <div className="space-y-3">
                                <div className="flex justify-between text-sm">
                                    <span className="text-slate-400">OS</span>
                                    <span className="text-slate-200">{ep.os}</span>
                                </div>
                                <div className="flex justify-between text-sm">
                                    <span className="text-slate-400">Health Score</span>
                                    <div className="flex items-center gap-2">
                                        <div className="w-24 h-2 bg-slate-700 rounded-full overflow-hidden">
                                            <div className={`h-full ${ep.healthScore > 90 ? 'bg-green-500' : 'bg-yellow-500'}`} style={{ width: `${ep.healthScore}%` }}></div>
                                        </div>
                                        <span className="text-slate-200">{ep.healthScore}%</span>
                                    </div>
                                </div>
                            </div>

                            <div className="mt-4 pt-3 border-t border-slate-700/50 flex justify-between items-center text-xs">
                                <span className="text-slate-500">Last seen: {ep.lastCheckin.toLocaleTimeString()}</span>
                                {ep.status === 'compromised' && (
                                    <button className="text-red-400 border border-red-500/30 px-2 py-1 rounded hover:bg-red-500/10">isolate host</button>
                                )}
                            </div>
                        </motion.div>
                    ))}
                </div>
            </div>
        </div>
    )
}
