import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'
import { Palette, X } from 'lucide-react'

interface Theme {
  name: string
  primary: string
  accent: string
  background: string
  cssVars: Record<string, string>
}

const themes: Theme[] = [
  {
    name: 'Dark Slate Orange',
    primary: '#f97316',
    accent: '#fb923c',
    background: '#0f172a',
    cssVars: {
      '--color-primary-400': '#fb923c',
      '--color-primary-500': '#f97316',
      '--color-primary-600': '#ea580c',
      '--color-primary-700': '#c2410c',
    }
  },
  {
    name: 'Dark Slate Blue',
    primary: '#3b82f6',
    accent: '#60a5fa',
    background: '#0f172a',
    cssVars: {
      '--color-primary-400': '#60a5fa',
      '--color-primary-500': '#3b82f6',
      '--color-primary-600': '#2563eb',
      '--color-primary-700': '#1d4ed8',
    }
  },
  {
    name: 'Dark Slate Cyan',
    primary: '#06b6d4',
    accent: '#22d3ee',
    background: '#0f172a',
    cssVars: {
      '--color-primary-400': '#22d3ee',
      '--color-primary-500': '#06b6d4',
      '--color-primary-600': '#0891b2',
      '--color-primary-700': '#0e7490',
    }
  },
  {
    name: 'Dark Slate Green',
    primary: '#10b981',
    accent: '#34d399',
    background: '#0f172a',
    cssVars: {
      '--color-primary-400': '#34d399',
      '--color-primary-500': '#10b981',
      '--color-primary-600': '#059669',
      '--color-primary-700': '#047857',
    }
  },
  {
    name: 'Dark Slate Red',
    primary: '#ef4444',
    accent: '#f87171',
    background: '#0f172a',
    cssVars: {
      '--color-primary-400': '#f87171',
      '--color-primary-500': '#ef4444',
      '--color-primary-600': '#dc2626',
      '--color-primary-700': '#b91c1c',
    }
  },
  {
    name: 'Dark Slate Purple',
    primary: '#8b5cf6',
    accent: '#a78bfa',
    background: '#0f172a',
    cssVars: {
      '--color-primary-400': '#a78bfa',
      '--color-primary-500': '#8b5cf6',
      '--color-primary-600': '#7c3aed',
      '--color-primary-700': '#6d28d9',
    }
  }
]

const ThemeSelector = () => {
  const [isOpen, setIsOpen] = useState(false)
  const [currentTheme, setCurrentTheme] = useState(0)

  useEffect(() => {
    const savedTheme = localStorage.getItem('bulletdrop-theme')
    if (savedTheme) {
      const themeIndex = parseInt(savedTheme)
      if (themeIndex >= 0 && themeIndex < themes.length) {
        setCurrentTheme(themeIndex)
        applyTheme(themes[themeIndex])
      }
    }
  }, [])

  const applyTheme = (theme: Theme) => {
    const root = document.documentElement
    Object.entries(theme.cssVars).forEach(([property, value]) => {
      root.style.setProperty(property, value)
    })
    
    // Update Tailwind CSS custom properties
    root.style.setProperty('--tw-color-primary-400', theme.cssVars['--color-primary-400'])
    root.style.setProperty('--tw-color-primary-500', theme.cssVars['--color-primary-500'])
    root.style.setProperty('--tw-color-primary-600', theme.cssVars['--color-primary-600'])
    root.style.setProperty('--tw-color-primary-700', theme.cssVars['--color-primary-700'])
  }

  const selectTheme = (index: number) => {
    setCurrentTheme(index)
    applyTheme(themes[index])
    localStorage.setItem('bulletdrop-theme', index.toString())
  }

  return (
    <>
      {/* Theme Toggle Button */}
      <motion.button
        initial={{ opacity: 0, scale: 0.8 }}
        animate={{ opacity: 1, scale: 1 }}
        onClick={() => setIsOpen(!isOpen)}
        className="fixed top-20 right-4 z-50 w-12 h-12 bg-slate-800/90 backdrop-blur-sm border border-slate-700 rounded-full flex items-center justify-center text-white hover:bg-slate-700/90 transition-all duration-200 shadow-lg"
        title="Change Theme"
      >
        <Palette className="h-5 w-5" />
      </motion.button>

      {/* Theme Selector Panel */}
      {isOpen && (
        <motion.div
          initial={{ opacity: 0, x: 20, scale: 0.9 }}
          animate={{ opacity: 1, x: 0, scale: 1 }}
          exit={{ opacity: 0, x: 20, scale: 0.9 }}
          className="fixed top-20 right-16 z-40 bg-slate-800/95 backdrop-blur-sm border border-slate-700 rounded-xl p-6 shadow-2xl min-w-[280px]"
        >
          <div className="flex items-center justify-between mb-4">
            <h3 className="text-lg font-semibold text-white">Choose Theme</h3>
            <button
              onClick={() => setIsOpen(false)}
              className="text-slate-400 hover:text-white transition-colors"
            >
              <X className="h-5 w-5" />
            </button>
          </div>
          
          <div className="space-y-3">
            {themes.map((theme, index) => (
              <motion.button
                key={theme.name}
                whileHover={{ scale: 1.02 }}
                whileTap={{ scale: 0.98 }}
                onClick={() => selectTheme(index)}
                className={`w-full flex items-center space-x-3 p-3 rounded-lg transition-all duration-200 ${
                  currentTheme === index 
                    ? 'bg-slate-700/50 ring-2 ring-primary-500/50' 
                    : 'hover:bg-slate-700/30'
                }`}
              >
                <div 
                  className="w-6 h-6 rounded-full border-2 border-slate-600"
                  style={{ backgroundColor: theme.primary }}
                ></div>
                <span className="text-white font-medium">{theme.name}</span>
                {currentTheme === index && (
                  <div className="ml-auto w-2 h-2 bg-primary-500 rounded-full"></div>
                )}
              </motion.button>
            ))}
          </div>
          
          <div className="mt-4 pt-4 border-t border-slate-700">
            <p className="text-xs text-slate-400 text-center">
              Theme preferences are saved locally
            </p>
          </div>
        </motion.div>
      )}
    </>
  )
}

export default ThemeSelector