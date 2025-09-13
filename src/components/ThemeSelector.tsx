import React, { useState, useEffect, useRef } from 'react'
import { motion, AnimatePresence } from 'framer-motion'
import { Palette, X, Monitor, Settings, Check, Sun, Moon, Sparkles, Zap, Eye, EyeOff } from 'lucide-react'
import { useTheme } from '../contexts/ThemeContext'

const ThemeSelector = () => {
  const [isOpen, setIsOpen] = useState(false)
  const [showAdvanced, setShowAdvanced] = useState(false)
  const { 
    currentTheme, 
    themes, 
    setTheme, 
    systemPreference, 
    followSystem, 
    setFollowSystem,
    animationsEnabled,
    setAnimationsEnabled,
    reducedMotion
  } = useTheme()
  
  const containerRef = useRef<HTMLDivElement>(null)
  const buttonRef = useRef<HTMLButtonElement>(null)

  // Handle escape key and outside clicks
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === 'Escape') {
        setIsOpen(false)
      }
    }

    const handleClickOutside = (event: MouseEvent) => {
      if (containerRef.current && !containerRef.current.contains(event.target as Node)) {
        setIsOpen(false)
      }
    }

    if (isOpen) {
      document.addEventListener('keydown', handleKeyDown)
      document.addEventListener('mousedown', handleClickOutside)
    }

    return () => {
      document.removeEventListener('keydown', handleKeyDown)
      document.removeEventListener('mousedown', handleClickOutside)
    }
  }, [isOpen])

  // Focus management
  useEffect(() => {
    if (isOpen && containerRef.current) {
      const firstFocusable = containerRef.current.querySelector('button, [tabindex="0"]') as HTMLElement
      firstFocusable?.focus()
    }
  }, [isOpen])

  const handleThemeSelect = (themeId: string) => {
    setTheme(themeId)
    // Don't close immediately for better UX - let user see the change
    setTimeout(() => setIsOpen(false), 300)
  }

  return (
    <>
      {/* Theme Toggle Button */}
      <motion.button
        ref={buttonRef}
        initial={{ opacity: 0, scale: 0.8 }}
        animate={{ opacity: 1, scale: 1 }}
        onClick={() => setIsOpen(!isOpen)}
        className="fixed top-20 right-4 z-50 w-12 h-12 bg-slate-800/90 backdrop-blur-sm border border-slate-700 rounded-full flex items-center justify-center text-white hover:bg-slate-700/90 transition-all duration-200 shadow-lg focus:outline-none focus:ring-2 focus:ring-primary-500 focus:ring-offset-2 focus:ring-offset-slate-900"
        title="Change Theme"
        aria-label="Open theme selector"
        aria-expanded={isOpen}
      >
        <Palette className="h-5 w-5" />
      </motion.button>

      {/* Theme Selector Panel */}
      <AnimatePresence>
        {isOpen && (
          <motion.div
            ref={containerRef}
            initial={{ opacity: 0, x: 20, scale: 0.9 }}
            animate={{ opacity: 1, x: 0, scale: 1 }}
            exit={{ opacity: 0, x: 20, scale: 0.9 }}
            className="fixed top-20 right-16 z-40 bg-slate-800/95 backdrop-blur-sm border border-slate-700 rounded-xl p-6 shadow-2xl min-w-[320px] max-w-[400px]"
            role="dialog"
            aria-labelledby="theme-selector-title"
            aria-describedby="theme-selector-description"
          >
            {/* Header */}
            <div className="flex items-center justify-between mb-6">
              <div>
                <h3 id="theme-selector-title" className="text-lg font-semibold text-white flex items-center gap-2">
                  <Palette className="h-5 w-5 text-primary-400" />
                  Theme Settings
                </h3>
                <p id="theme-selector-description" className="text-xs text-slate-400 mt-1">
                  Customize your visual experience
                </p>
              </div>
              <button
                onClick={() => setIsOpen(false)}
                className="text-slate-400 hover:text-white transition-colors p-1 rounded-md focus:outline-none focus:ring-2 focus:ring-primary-500"
                aria-label="Close theme selector"
              >
                <X className="h-5 w-5" />
              </button>
            </div>

            {/* System Preference Toggle */}
            <div className="mb-6">
              <div className="flex items-center justify-between p-3 bg-slate-700/30 rounded-lg">
                <div className="flex items-center gap-2">
                  <Monitor className="h-4 w-4 text-slate-400" />
                  <span className="text-sm text-white">Follow System</span>
                </div>
                <button
                  onClick={() => setFollowSystem(!followSystem)}
                  className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors focus:outline-none focus:ring-2 focus:ring-primary-500 ${
                    followSystem ? 'bg-primary-500' : 'bg-slate-600'
                  }`}
                  aria-pressed={followSystem}
                  aria-label="Toggle system theme following"
                >
                  <span
                    className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${
                      followSystem ? 'translate-x-6' : 'translate-x-1'
                    }`}
                  />
                </button>
              </div>
              {systemPreference && (
                <p className="text-xs text-slate-400 mt-2 ml-6">
                  System preference: {systemPreference === 'dark' ? <Moon className="inline h-3 w-3" /> : <Sun className="inline h-3 w-3" />} {systemPreference}
                </p>
              )}
            </div>
            
            {/* Theme Selection */}
            <div className="mb-6">
              <h4 className="text-sm font-medium text-slate-300 mb-3">Color Themes</h4>
              <div className="grid grid-cols-1 gap-2">
                {themes.map((theme) => (
                  <motion.button
                    key={theme.id}
                    whileHover={animationsEnabled ? { scale: 1.02, x: 4 } : {}}
                    whileTap={animationsEnabled ? { scale: 0.98 } : {}}
                    onClick={() => handleThemeSelect(theme.id)}
                    disabled={followSystem}
                    className={`w-full flex items-center space-x-3 p-3 rounded-lg transition-all duration-200 text-left focus:outline-none focus:ring-2 focus:ring-primary-500 ${
                      currentTheme.id === theme.id 
                        ? 'bg-slate-700/50 ring-2 ring-primary-500/50' 
                        : 'hover:bg-slate-700/30'
                    } ${followSystem ? 'opacity-50 cursor-not-allowed' : ''}`}
                    aria-pressed={currentTheme.id === theme.id}
                    title={`${theme.name} - ${theme.accessibility.wcagLevel} compliant (${theme.accessibility.contrastRatio}:1 contrast)`}
                  >
                    <div className="flex items-center gap-3 flex-1">
                      <div 
                        className="w-6 h-6 rounded-full border-2 border-slate-600 shadow-inner"
                        style={{ backgroundColor: theme.primary }}
                        role="img"
                        aria-label={`${theme.name} theme color preview`}
                      />
                      <div className="flex-1">
                        <span className="text-white font-medium block">{theme.name}</span>
                        <div className="flex items-center gap-1 mt-1">
                          <span className={`text-xs px-1.5 py-0.5 rounded ${
                            theme.accessibility.wcagLevel === 'AAA' 
                              ? 'bg-emerald-500/20 text-emerald-400' 
                              : 'bg-blue-500/20 text-blue-400'
                          }`}>
                            {theme.accessibility.wcagLevel}
                          </span>
                          <span className="text-xs text-slate-400">
                            {theme.accessibility.contrastRatio}:1
                          </span>
                        </div>
                      </div>
                    </div>
                    {currentTheme.id === theme.id && (
                      <Check className="w-4 h-4 text-primary-400 flex-shrink-0" />
                    )}
                  </motion.button>
                ))}
              </div>
            </div>

            {/* Advanced Settings Toggle */}
            <div className="border-t border-slate-700 pt-4">
              <button
                onClick={() => setShowAdvanced(!showAdvanced)}
                className="w-full flex items-center justify-between text-sm text-slate-300 hover:text-white transition-colors focus:outline-none focus:ring-2 focus:ring-primary-500 rounded p-2"
                aria-expanded={showAdvanced}
              >
                <div className="flex items-center gap-2">
                  <Settings className="h-4 w-4" />
                  Advanced Settings
                </div>
                <motion.div
                  animate={{ rotate: showAdvanced ? 180 : 0 }}
                  transition={{ duration: 0.2 }}
                >
                  ▼
                </motion.div>
              </button>

              <AnimatePresence>
                {showAdvanced && (
                  <motion.div
                    initial={{ opacity: 0, height: 0 }}
                    animate={{ opacity: 1, height: 'auto' }}
                    exit={{ opacity: 0, height: 0 }}
                    transition={{ duration: 0.2 }}
                    className="mt-4 space-y-4"
                  >
                    {/* Animation Settings */}
                    <div className="flex items-center justify-between p-3 bg-slate-700/30 rounded-lg">
                      <div className="flex items-center gap-2">
                        <Sparkles className="h-4 w-4 text-slate-400" />
                        <div>
                          <span className="text-sm text-white block">Animations</span>
                          {reducedMotion && (
                            <span className="text-xs text-amber-400">Disabled by system preference</span>
                          )}
                        </div>
                      </div>
                      <button
                        onClick={() => setAnimationsEnabled(!animationsEnabled)}
                        disabled={reducedMotion}
                        className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors focus:outline-none focus:ring-2 focus:ring-primary-500 ${
                          animationsEnabled && !reducedMotion ? 'bg-primary-500' : 'bg-slate-600'
                        } ${reducedMotion ? 'opacity-50 cursor-not-allowed' : ''}`}
                        aria-pressed={animationsEnabled && !reducedMotion}
                        title={reducedMotion ? 'Animations disabled by system preference' : 'Toggle animations'}
                      >
                        <span
                          className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${
                            animationsEnabled && !reducedMotion ? 'translate-x-6' : 'translate-x-1'
                          }`}
                        />
                      </button>
                    </div>

                    {/* Performance Info */}
                    <div className="p-3 bg-slate-700/30 rounded-lg">
                      <div className="flex items-center gap-2 mb-2">
                        <Zap className="h-4 w-4 text-slate-400" />
                        <span className="text-sm text-white">Performance</span>
                      </div>
                      <div className="space-y-1 text-xs text-slate-400">
                        <div>Reduced Motion: {reducedMotion ? 'Active' : 'Inactive'}</div>
                        <div>Animations: {animationsEnabled ? 'Enabled' : 'Disabled'}</div>
                        <div>Current Theme: {currentTheme.accessibility.wcagLevel} compliant</div>
                      </div>
                    </div>
                  </motion.div>
                )}
              </AnimatePresence>
            </div>

            {/* Footer */}
            <div className="mt-4 pt-4 border-t border-slate-700">
              <p className="text-xs text-slate-400 text-center">
                Settings are saved automatically
              </p>
            </div>
          </motion.div>
        )}
      </AnimatePresence>
    </>
  )
}

export default ThemeSelector