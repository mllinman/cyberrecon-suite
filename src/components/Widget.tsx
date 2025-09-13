import React, { ReactNode } from 'react'
import { motion } from 'framer-motion'
import { MoreVertical, X, RefreshCw, Settings, Maximize2, Minimize2 } from 'lucide-react'

export interface WidgetConfig {
  id: string
  title: string
  description?: string
  size: 'small' | 'medium' | 'large' | 'full'
  category: 'security' | 'analytics' | 'system' | 'custom'
  refreshable: boolean
  configurable: boolean
  removable: boolean
}

interface WidgetProps {
  config: WidgetConfig
  children: ReactNode
  onRemove?: (id: string) => void
  onRefresh?: (id: string) => void
  onConfigure?: (id: string) => void
  onResize?: (id: string, size: WidgetConfig['size']) => void
  loading?: boolean
  error?: string
  className?: string
}

const Widget: React.FC<WidgetProps> = ({
  config,
  children,
  onRemove,
  onRefresh,
  onConfigure,
  onResize,
  loading = false,
  error,
  className = ''
}) => {
  const [isExpanded, setIsExpanded] = React.useState(false)
  const [showMenu, setShowMenu] = React.useState(false)

  const getSizeClasses = (size: WidgetConfig['size'], expanded: boolean) => {
    if (expanded) return 'col-span-full row-span-2'
    
    switch (size) {
      case 'small':
        return 'col-span-1 row-span-1'
      case 'medium':
        return 'col-span-2 row-span-1'
      case 'large':
        return 'col-span-2 row-span-2'
      case 'full':
        return 'col-span-full row-span-1'
      default:
        return 'col-span-1 row-span-1'
    }
  }

  const getCategoryColor = (category: WidgetConfig['category']) => {
    switch (category) {
      case 'security':
        return 'border-red-500/30 bg-red-500/5'
      case 'analytics':
        return 'border-blue-500/30 bg-blue-500/5'
      case 'system':
        return 'border-emerald-500/30 bg-emerald-500/5'
      case 'custom':
        return 'border-purple-500/30 bg-purple-500/5'
      default:
        return 'border-slate-600/30 bg-slate-800/30'
    }
  }

  return (
    <motion.div
      layout
      initial={{ opacity: 0, scale: 0.9 }}
      animate={{ opacity: 1, scale: 1 }}
      exit={{ opacity: 0, scale: 0.9 }}
      transition={{ duration: 0.2 }}
      className={`${getSizeClasses(config.size, isExpanded)} ${className}`}
    >
      <div className={`
        h-full rounded-xl border backdrop-blur-sm transition-all duration-200
        hover:border-primary-500/40 hover:shadow-lg hover:shadow-primary-500/10
        ${getCategoryColor(config.category)}
        ${error ? 'border-red-500/50 bg-red-500/10' : ''}
        ${loading ? 'animate-pulse' : ''}
      `}>
        {/* Widget Header */}
        <div className="flex items-center justify-between p-4 border-b border-slate-600/30">
          <div className="flex-1 min-w-0">
            <h3 className="text-sm font-semibold text-white truncate">
              {config.title}
            </h3>
            {config.description && (
              <p className="text-xs text-slate-400 mt-1 truncate">
                {config.description}
              </p>
            )}
          </div>

          {/* Widget Actions */}
          <div className="flex items-center space-x-1 ml-2">
            {config.refreshable && (
              <button
                onClick={() => onRefresh?.(config.id)}
                className="p-1 rounded-md text-slate-400 hover:text-white hover:bg-slate-600/50 transition-colors"
                title="Refresh widget"
                disabled={loading}
              >
                <RefreshCw className={`h-4 w-4 ${loading ? 'animate-spin' : ''}`} />
              </button>
            )}

            <div className="relative">
              <button
                onClick={() => setShowMenu(!showMenu)}
                className="p-1 rounded-md text-slate-400 hover:text-white hover:bg-slate-600/50 transition-colors"
                title="Widget options"
              >
                <MoreVertical className="h-4 w-4" />
              </button>

              {/* Dropdown Menu */}
              {showMenu && (
                <motion.div
                  initial={{ opacity: 0, scale: 0.95 }}
                  animate={{ opacity: 1, scale: 1 }}
                  exit={{ opacity: 0, scale: 0.95 }}
                  className="absolute right-0 top-full mt-1 w-48 bg-slate-800/95 backdrop-blur-sm border border-slate-600/50 rounded-lg shadow-xl z-20"
                >
                  <div className="py-1">
                    <button
                      onClick={() => {
                        setIsExpanded(!isExpanded)
                        setShowMenu(false)
                      }}
                      className="w-full flex items-center space-x-2 px-3 py-2 text-sm text-slate-300 hover:text-white hover:bg-slate-700/50 transition-colors"
                    >
                      {isExpanded ? <Minimize2 className="h-4 w-4" /> : <Maximize2 className="h-4 w-4" />}
                      <span>{isExpanded ? 'Minimize' : 'Expand'}</span>
                    </button>

                    {config.configurable && (
                      <button
                        onClick={() => {
                          onConfigure?.(config.id)
                          setShowMenu(false)
                        }}
                        className="w-full flex items-center space-x-2 px-3 py-2 text-sm text-slate-300 hover:text-white hover:bg-slate-700/50 transition-colors"
                      >
                        <Settings className="h-4 w-4" />
                        <span>Configure</span>
                      </button>
                    )}

                    {onResize && (
                      <>
                        <div className="border-t border-slate-600/50 my-1"></div>
                        <div className="px-3 py-1">
                          <p className="text-xs text-slate-400 uppercase tracking-wide">Resize</p>
                        </div>
                        {(['small', 'medium', 'large', 'full'] as const).map((size) => (
                          <button
                            key={size}
                            onClick={() => {
                              onResize(config.id, size)
                              setShowMenu(false)
                            }}
                            className={`w-full flex items-center justify-between px-3 py-2 text-sm transition-colors ${
                              config.size === size 
                                ? 'text-primary-400 bg-primary-500/10' 
                                : 'text-slate-300 hover:text-white hover:bg-slate-700/50'
                            }`}
                          >
                            <span className="capitalize">{size}</span>
                            {config.size === size && <span className="text-xs">✓</span>}
                          </button>
                        ))}
                      </>
                    )}

                    {config.removable && (
                      <>
                        <div className="border-t border-slate-600/50 my-1"></div>
                        <button
                          onClick={() => {
                            onRemove?.(config.id)
                            setShowMenu(false)
                          }}
                          className="w-full flex items-center space-x-2 px-3 py-2 text-sm text-red-400 hover:text-red-300 hover:bg-red-500/10 transition-colors"
                        >
                          <X className="h-4 w-4" />
                          <span>Remove</span>
                        </button>
                      </>
                    )}
                  </div>
                </motion.div>
              )}
            </div>
          </div>
        </div>

        {/* Widget Content */}
        <div className="p-4 h-full">
          {error ? (
            <div className="flex items-center justify-center h-32 text-center">
              <div>
                <div className="text-red-400 mb-2">⚠️</div>
                <p className="text-sm text-red-400">{error}</p>
                {config.refreshable && (
                  <button
                    onClick={() => onRefresh?.(config.id)}
                    className="mt-2 text-xs text-slate-400 hover:text-white underline"
                  >
                    Try again
                  </button>
                )}
              </div>
            </div>
          ) : loading ? (
            <div className="flex items-center justify-center h-32">
              <div className="text-center">
                <div className="animate-spin rounded-full h-8 w-8 border-2 border-primary-500 border-t-transparent mx-auto mb-2"></div>
                <p className="text-sm text-slate-400">Loading...</p>
              </div>
            </div>
          ) : (
            children
          )}
        </div>
      </div>

      {/* Click outside handler for dropdown */}
      {showMenu && (
        <div
          className="fixed inset-0 z-10"
          onClick={() => setShowMenu(false)}
        />
      )}
    </motion.div>
  )
}

export default Widget