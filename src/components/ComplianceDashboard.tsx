import React, { useState } from 'react'
import { motion } from 'framer-motion'
import { FileText, CheckCircle, XCircle, AlertTriangle, PieChart, Download } from 'lucide-react'

interface ComplianceFramework {
    id: string
    name: string
    score: number
    status: 'compliant' | 'non-compliant' | 'warning'
    issues: number
    lastAudit: string
}

export default function ComplianceDashboard() {
    const [frameworks] = useState<ComplianceFramework[]>([
        { id: 'pci', name: 'PCI DSS v4.0', score: 92, status: 'compliant', issues: 2, lastAudit: '2023-10-15' },
        { id: 'hipaa', name: 'HIPAA Security Rule', score: 85, status: 'warning', issues: 5, lastAudit: '2023-09-20' },
        { id: 'iso', name: 'ISO 27001', score: 78, status: 'non-compliant', issues: 12, lastAudit: '2023-11-01' },
        { id: 'nist', name: 'NIST CSF', score: 88, status: 'compliant', issues: 3, lastAudit: '2023-10-30' },
        { id: 'gdpr', name: 'GDPR', score: 95, status: 'compliant', issues: 0, lastAudit: '2023-10-05' }
    ])

    const getStatusColor = (status: string) => {
        switch (status) {
            case 'compliant': return 'text-green-400 bg-green-500/10 border-green-500/20'
            case 'warning': return 'text-yellow-400 bg-yellow-500/10 border-yellow-500/20'
            case 'non-compliant': return 'text-red-400 bg-red-500/10 border-red-500/20'
            default: return 'text-slate-400'
        }
    }

    return (
        <div className="min-h-screen bg-slate-900 text-white p-6">
            <div className="max-w-7xl mx-auto">
                <div className="flex items-center justify-between mb-8">
                    <div>
                        <h1 className="text-3xl font-bold text-primary-400 mb-2 flex items-center gap-3">
                            <FileText className="w-8 h-8" />
                            Compliance & Reports
                        </h1>
                        <p className="text-gray-400">Security framework adherence and audit logging</p>
                    </div>
                    <button className="flex items-center gap-2 px-4 py-2 bg-primary-600 rounded hover:bg-primary-700">
                        <Download className="w-4 h-4" />
                        Export All Reports
                    </button>
                </div>

                <div className="grid grid-cols-1 gap-6">
                    {frameworks.map((fw, idx) => (
                        <motion.div
                            key={fw.id}
                            initial={{ opacity: 0, x: -20 }}
                            animate={{ opacity: 1, x: 0 }}
                            transition={{ delay: idx * 0.1 }}
                            className="glass-card p-6 flex flex-col md:flex-row items-center justify-between gap-6"
                        >
                            <div className="flex items-center gap-4 flex-1">
                                <div className="w-16 h-16 rounded-full bg-slate-800 flex items-center justify-center border border-slate-700">
                                    <span className={`text-xl font-bold ${fw.score >= 90 ? 'text-green-400' : fw.score >= 80 ? 'text-yellow-400' : 'text-red-400'}`}>
                                        {fw.score}%
                                    </span>
                                </div>
                                <div>
                                    <h3 className="text-xl font-bold text-white">{fw.name}</h3>
                                    <p className="text-slate-400 text-sm">Last Audit: {fw.lastAudit}</p>
                                </div>
                            </div>

                            <div className="flex items-center gap-8 flex-1 justify-end">
                                <div className="text-center">
                                    <p className="text-sm text-slate-400 mb-1">Status</p>
                                    <span className={`px-3 py-1 rounded-full text-xs font-bold border ${getStatusColor(fw.status)}`}>
                                        {fw.status.toUpperCase()}
                                    </span>
                                </div>
                                <div className="text-center">
                                    <p className="text-sm text-slate-400 mb-1">Open Issues</p>
                                    <div className="flex items-center gap-2 justify-center">
                                        <AlertTriangle className="w-4 h-4 text-orange-400" />
                                        <span className="font-bold text-white">{fw.issues}</span>
                                    </div>
                                </div>
                                <button className="px-4 py-2 text-sm border border-slate-600 rounded hover:bg-slate-800 text-slate-300">
                                    View Details
                                </button>
                            </div>
                        </motion.div>
                    ))}
                </div>
            </div>
        </div>
    )
}
