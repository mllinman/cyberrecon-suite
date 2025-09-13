import React, { useState, useEffect } from 'react'
import { motion, AnimatePresence } from 'framer-motion'
import { Plus, Layout, Save, RotateCcw, Grid } from 'lucide-react'
import Widget, { WidgetConfig } from './Widget'
import { useTheme } from '../contexts/ThemeContext'

interface WidgetGridProps {
  widgets: WidgetConfig[]
  onWidgetAdd?: (widget: WidgetConfig) => void
  onWidgetRemove?: (id: string) => void
  onWidgetResize?: (id: string, size: WidgetConfig['size']) => void
  onLayoutSave?: (layout: WidgetConfig[]) => void
  editable?: boolean
  className?: string
}

interface WidgetInstance extends WidgetConfig {
  component: React.ReactNode
  loading?: boolean
  error?: string
  lastUpdated?: Date
}

// Sample widget components for demonstration
const SecurityStatsWidget = () => (
  <div className="h-32">
    <div className="flex items-center justify-between mb-3">
      <span className="text-2xl font-bold text-white">24</span>
      <div className="flex items-center text-emerald-400 text-sm">
        <span>↗ +2</span>
      </div>
    </div>
    <p className="text-sm text-slate-400">Active Threats Detected</p>
    <div className="mt-3 h-2 bg-slate-700 rounded-full overflow-hidden">
      <div className="h-full w-3/4 bg-gradient-to-r from-red-500 to-red-400 rounded-full"></div>
    </div>
  </div>
)

const SystemHealthWidget = () => (
  <div className="h-32">
    <div className="flex items-center justify-between mb-3">
      <span className="text-2xl font-bold text-white">98.5%</span>
      <div className="flex items-center text-emerald-400 text-sm">
        <span>↗ +0.2%</span>
      </div>
    </div>
    <p className="text-sm text-slate-400">System Uptime</p>
    <div className="mt-3 flex space-x-2">
      {[...Array(24)].map((_, i) => (
        <div
          key={i}
          className={`h-6 w-1 rounded-full ${
            Math.random() > 0.1 ? 'bg-emerald-500' : 'bg-red-500'
          }`}
        />
      ))}
    </div>
  </div>
)

const NetworkTrafficWidget = () => (
  <div className="h-48">
    <div className="mb-4">
      <div className="flex items-center justify-between mb-2">
        <span className="text-xl font-bold text-white">1.2 GB/s</span>
        <div className="flex items-center text-blue-400 text-sm">
          <span>↗ +15%</span>
        </div>
      </div>
      <p className="text-sm text-slate-400">Network Traffic</p>
    </div>
    <div className="space-y-3">
      <div className="flex items-center justify-between">
        <span className="text-sm text-slate-300">Inbound</span>
        <span className="text-sm text-white">720 MB/s</span>
      </div>
      <div className="h-2 bg-slate-700 rounded-full overflow-hidden">
        <div className="h-full w-3/5 bg-gradient-to-r from-blue-500 to-blue-400"></div>
      </div>
      <div className="flex items-center justify-between">
        <span className="text-sm text-slate-300">Outbound</span>
        <span className="text-sm text-white">480 MB/s</span>
      </div>
      <div className="h-2 bg-slate-700 rounded-full overflow-hidden">
        <div className="h-full w-2/5 bg-gradient-to-r from-emerald-500 to-emerald-400"></div>
      </div>
    </div>
  </div>
)

const WidgetGrid: React.FC<WidgetGridProps> = ({
  widgets: initialWidgets,
  onWidgetAdd,
  onWidgetRemove,
  onWidgetResize,
  onLayoutSave,
  editable = true,
  className = ''
}) => {
  const { animationsEnabled } = useTheme()
  const [widgets, setWidgets] = useState<WidgetInstance[]>([])
  const [isEditing, setIsEditing] = useState(false)
  const [showAddWidget, setShowAddWidget] = useState(false)

  // Sample widget templates
  const widgetTemplates: Omit<WidgetInstance, 'id'>[] = [
    {
      title: 'Security Alerts',
      description: 'Active security threats and alerts',
      size: 'small',
      category: 'security',
      refreshable: true,
      configurable: true,
      removable: true,
      component: <SecurityStatsWidget />
    },
    {
      title: 'System Health',
      description: 'Overall system performance metrics',
      size: 'small',
      category: 'system',
      refreshable: true,
      configurable: true,
      removable: true,
      component: <SystemHealthWidget />
    },
    {
      title: 'Network Traffic',
      description: 'Real-time network usage statistics',
      size: 'medium',
      category: 'analytics',
      refreshable: true,
      configurable: true,
      removable: true,
      component: <NetworkTrafficWidget />
    }
  ]

  // Initialize widgets
  useEffect(() => {
    const initializedWidgets: WidgetInstance[] = initialWidgets.map((widget, index) => ({
      ...widget,
      component: widgetTemplates[index % widgetTemplates.length]?.component || <div>Widget content</div>,
      lastUpdated: new Date()
    }))

    // If no widgets provided, add some defaults
    if (initializedWidgets.length === 0) {
      const defaultWidgets = widgetTemplates.map((template, index) => ({
        ...template,
        id: `widget-${index + 1}`
      }))
      setWidgets(defaultWidgets)
    } else {
      setWidgets(initializedWidgets)
    }
  }, [initialWidgets])

  const handleWidgetRemove = (id: string) => {
    setWidgets(prev => prev.filter(w => w.id !== id))
    onWidgetRemove?.(id)
  }

  const handleWidgetResize = (id: string, size: WidgetConfig['size']) => {
    setWidgets(prev => prev.map(w => w.id === id ? { ...w, size } : w))
    onWidgetResize?.(id, size)
  }

  const handleWidgetRefresh = (id: string) => {
    setWidgets(prev => prev.map(w => 
      w.id === id 
        ? { ...w, loading: true, error: undefined }
        : w
    ))

    // Simulate refresh
    setTimeout(() => {
      setWidgets(prev => prev.map(w => 
        w.id === id 
          ? { ...w, loading: false, lastUpdated: new Date() }
          : w
      ))
    }, 2000)
  }

  const handleAddWidget = (templateIndex: number) => {
    const template = widgetTemplates[templateIndex]
    const newWidget: WidgetInstance = {
      ...template,
      id: `widget-${Date.now()}`,
      lastUpdated: new Date()
    }
    
    setWidgets(prev => [...prev, newWidget])
    onWidgetAdd?.(newWidget)
    setShowAddWidget(false)
  }

  const handleSaveLayout = () => {
    const layout = widgets.map(({ component, loading, error, lastUpdated, ...config }) => config)
    onLayoutSave?.(layout)
    setIsEditing(false)
  }

  const handleResetLayout = () => {
    const defaultWidgets = widgetTemplates.map((template, index) => ({
      ...template,
      id: `widget-${index + 1}`
    }))
    setWidgets(defaultWidgets)
  }

  return (
    <div className={`space-y-6 ${className}`}>
      {/* Widget Grid Controls */}
      {editable && (
        <motion.div
          initial={animationsEnabled ? { opacity: 0, y: -10 } : { opacity: 1 }}
          animate={{ opacity: 1, y: 0 }}
          className="flex items-center justify-between p-4 bg-slate-800/30 backdrop-blur-sm rounded-xl border border-slate-600/30"
        >
          <div className="flex items-center space-x-3">
            <Grid className="h-5 w-5 text-primary-400" />
            <div>
              <h3 className="text-sm font-semibold text-white">Widget Dashboard</h3>
              <p className="text-xs text-slate-400">
                {isEditing ? 'Click and drag to rearrange widgets' : `${widgets.length} widgets active`}
              </p>
            </div>
          </div>

          <div className="flex items-center space-x-2">
            <button
              onClick={() => setShowAddWidget(true)}
              className="flex items-center space-x-2 px-3 py-2 bg-primary-500/20 hover:bg-primary-500/30 text-primary-400 rounded-lg transition-colors text-sm"
            >
              <Plus className="h-4 w-4" />
              <span>Add Widget</span>
            </button>

            <button
              onClick={() => setIsEditing(!isEditing)}
              className={`flex items-center space-x-2 px-3 py-2 rounded-lg transition-colors text-sm ${
                isEditing 
                  ? 'bg-amber-500/20 text-amber-400 hover:bg-amber-500/30' 
                  : 'bg-slate-600/30 text-slate-300 hover:bg-slate-600/50'
              }`}
            >
              <Layout className="h-4 w-4" />
              <span>{isEditing ? 'Exit Edit' : 'Edit Layout'}</span>
            </button>

            {isEditing && (
              <>
                <button
                  onClick={handleSaveLayout}
                  className="flex items-center space-x-2 px-3 py-2 bg-emerald-500/20 hover:bg-emerald-500/30 text-emerald-400 rounded-lg transition-colors text-sm"
                >
                  <Save className="h-4 w-4" />
                  <span>Save</span>
                </button>

                <button
                  onClick={handleResetLayout}
                  className="flex items-center space-x-2 px-3 py-2 bg-red-500/20 hover:bg-red-500/30 text-red-400 rounded-lg transition-colors text-sm"
                >
                  <RotateCcw className="h-4 w-4" />
                  <span>Reset</span>
                </button>
              </>
            )}
          </div>
        </motion.div>
      )}

      {/* Widget Grid */}
      <div className={`
        grid gap-6 auto-rows-[200px]
        grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4
        ${isEditing ? 'cursor-move' : ''}
      `}>
        <AnimatePresence>
          {widgets.map((widget) => (
            <Widget
              key={widget.id}
              config={widget}
              onRemove={handleWidgetRemove}
              onRefresh={handleWidgetRefresh}
              onResize={handleWidgetResize}
              loading={widget.loading}
              error={widget.error}
              className={isEditing ? 'ring-2 ring-primary-500/30 ring-offset-2 ring-offset-slate-900' : ''}
            >
              {widget.component}
            </Widget>
          ))}
        </AnimatePresence>
      </div>

      {/* Add Widget Modal */}
      <AnimatePresence>
        {showAddWidget && (
          <motion.div
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="fixed inset-0 bg-black/50 backdrop-blur-sm z-50 flex items-center justify-center p-4"
            onClick={() => setShowAddWidget(false)}
          >
            <motion.div
              initial={{ opacity: 0, scale: 0.95 }}
              animate={{ opacity: 1, scale: 1 }}
              exit={{ opacity: 0, scale: 0.95 }}
              onClick={(e) => e.stopPropagation()}
              className="bg-slate-800/95 backdrop-blur-sm border border-slate-600/50 rounded-xl p-6 max-w-2xl w-full max-h-[80vh] overflow-y-auto"
            >
              <div className="flex items-center justify-between mb-6">
                <h3 className="text-lg font-semibold text-white">Add Widget</h3>
                <button
                  onClick={() => setShowAddWidget(false)}
                  className="text-slate-400 hover:text-white transition-colors"
                >
                  ×
                </button>
              </div>

              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                {widgetTemplates.map((template, index) => (
                  <div
                    key={index}
                    onClick={() => handleAddWidget(index)}
                    className="p-4 bg-slate-700/30 hover:bg-slate-700/50 rounded-lg cursor-pointer transition-colors border border-slate-600/30 hover:border-primary-500/30"
                  >
                    <div className="flex items-center space-x-3 mb-3">
                      <div className={`w-3 h-3 rounded-full ${
                        template.category === 'security' ? 'bg-red-500' :
                        template.category === 'analytics' ? 'bg-blue-500' :
                        template.category === 'system' ? 'bg-emerald-500' :
                        'bg-purple-500'
                      }`} />
                      <div>
                        <h4 className="font-semibold text-white">{template.title}</h4>
                        <p className="text-sm text-slate-400">{template.description}</p>
                      </div>
                    </div>
                    <div className="flex items-center justify-between text-xs text-slate-500">
                      <span className="capitalize">{template.category}</span>
                      <span className="capitalize">{template.size} widget</span>
                    </div>
                  </div>
                ))}
              </div>
            </motion.div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Empty State */}
      {widgets.length === 0 && (
        <motion.div
          initial={animationsEnabled ? { opacity: 0, y: 20 } : { opacity: 1 }}
          animate={{ opacity: 1, y: 0 }}
          className="flex flex-col items-center justify-center py-16 text-center"
        >
          <Grid className="h-16 w-16 text-slate-500 mb-4" />
          <h3 className="text-lg font-semibold text-white mb-2">No widgets added</h3>
          <p className="text-slate-400 mb-6">Start building your dashboard by adding some widgets</p>
          <button
            onClick={() => setShowAddWidget(true)}
            className="btn-primary"
          >
            <Plus className="mr-2 h-4 w-4" />
            Add Your First Widget
          </button>
        </motion.div>
      )}
    </div>
  )
}

export default WidgetGrid