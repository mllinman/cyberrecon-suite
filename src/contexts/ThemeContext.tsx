import React, { createContext, useContext, useState, useEffect, ReactNode } from 'react'

export interface Theme {
  id: string
  name: string
  primary: string
  accent: string
  background: string
  surface: string
  text: string
  textSecondary: string
  border: string
  cssVars: Record<string, string>
  accessibility: {
    contrastRatio: number
    wcagLevel: 'AA' | 'AAA'
  }
}

export const defaultThemes: Theme[] = [
  {
    id: 'dark-slate-orange',
    name: 'Dark Slate Orange',
    primary: '#f97316',
    accent: '#fb923c',
    background: '#0f172a',
    surface: '#1e293b',
    text: '#f8fafc',
    textSecondary: '#cbd5e1',
    border: '#334155',
    accessibility: {
      contrastRatio: 7.2,
      wcagLevel: 'AAA'
    },
    cssVars: {
      '--color-primary-400': '#fb923c',
      '--color-primary-500': '#f97316',
      '--color-primary-600': '#ea580c',
      '--color-primary-700': '#c2410c',
      '--color-background': '#0f172a',
      '--color-surface': '#1e293b',
      '--color-surface-hover': '#334155',
      '--color-text': '#f8fafc',
      '--color-text-secondary': '#cbd5e1',
      '--color-border': '#334155',
    }
  },
  {
    id: 'dark-slate-blue',
    name: 'Dark Slate Blue',
    primary: '#3b82f6',
    accent: '#60a5fa',
    background: '#0f172a',
    surface: '#1e293b',
    text: '#f8fafc',
    textSecondary: '#cbd5e1',
    border: '#334155',
    accessibility: {
      contrastRatio: 8.1,
      wcagLevel: 'AAA'
    },
    cssVars: {
      '--color-primary-400': '#60a5fa',
      '--color-primary-500': '#3b82f6',
      '--color-primary-600': '#2563eb',
      '--color-primary-700': '#1d4ed8',
      '--color-background': '#0f172a',
      '--color-surface': '#1e293b',
      '--color-surface-hover': '#334155',
      '--color-text': '#f8fafc',
      '--color-text-secondary': '#cbd5e1',
      '--color-border': '#334155',
    }
  },
  {
    id: 'dark-slate-cyan',
    name: 'Dark Slate Cyan',
    primary: '#06b6d4',
    accent: '#22d3ee',
    background: '#0f172a',
    surface: '#1e293b',
    text: '#f8fafc',
    textSecondary: '#cbd5e1',
    border: '#334155',
    accessibility: {
      contrastRatio: 7.8,
      wcagLevel: 'AAA'
    },
    cssVars: {
      '--color-primary-400': '#22d3ee',
      '--color-primary-500': '#06b6d4',
      '--color-primary-600': '#0891b2',
      '--color-primary-700': '#0e7490',
      '--color-background': '#0f172a',
      '--color-surface': '#1e293b',
      '--color-surface-hover': '#334155',
      '--color-text': '#f8fafc',
      '--color-text-secondary': '#cbd5e1',
      '--color-border': '#334155',
    }
  },
  {
    id: 'dark-slate-emerald',
    name: 'Dark Slate Emerald',
    primary: '#10b981',
    accent: '#34d399',
    background: '#0f172a',
    surface: '#1e293b',
    text: '#f8fafc',
    textSecondary: '#cbd5e1',
    border: '#334155',
    accessibility: {
      contrastRatio: 7.5,
      wcagLevel: 'AAA'
    },
    cssVars: {
      '--color-primary-400': '#34d399',
      '--color-primary-500': '#10b981',
      '--color-primary-600': '#059669',
      '--color-primary-700': '#047857',
      '--color-background': '#0f172a',
      '--color-surface': '#1e293b',
      '--color-surface-hover': '#334155',
      '--color-text': '#f8fafc',
      '--color-text-secondary': '#cbd5e1',
      '--color-border': '#334155',
    }
  },
  {
    id: 'dark-slate-purple',
    name: 'Dark Slate Purple',
    primary: '#8b5cf6',
    accent: '#a78bfa',
    background: '#0f172a',
    surface: '#1e293b',
    text: '#f8fafc',
    textSecondary: '#cbd5e1',
    border: '#334155',
    accessibility: {
      contrastRatio: 7.0,
      wcagLevel: 'AA'
    },
    cssVars: {
      '--color-primary-400': '#a78bfa',
      '--color-primary-500': '#8b5cf6',
      '--color-primary-600': '#7c3aed',
      '--color-primary-700': '#6d28d9',
      '--color-background': '#0f172a',
      '--color-surface': '#1e293b',
      '--color-surface-hover': '#334155',
      '--color-text': '#f8fafc',
      '--color-text-secondary': '#cbd5e1',
      '--color-border': '#334155',
    }
  }
]

interface ThemeContextType {
  currentTheme: Theme
  themes: Theme[]
  setTheme: (themeId: string) => void
  isDarkMode: boolean
  systemPreference: 'light' | 'dark' | null
  followSystem: boolean
  setFollowSystem: (follow: boolean) => void
  animationsEnabled: boolean
  setAnimationsEnabled: (enabled: boolean) => void
  reducedMotion: boolean
}

const ThemeContext = createContext<ThemeContextType | undefined>(undefined)

export const useTheme = () => {
  const context = useContext(ThemeContext)
  if (!context) {
    throw new Error('useTheme must be used within a ThemeProvider')
  }
  return context
}

interface ThemeProviderProps {
  children: ReactNode
}

export const ThemeProvider: React.FC<ThemeProviderProps> = ({ children }) => {
  const [currentThemeId, setCurrentThemeId] = useState<string>('dark-slate-cyan')
  const [followSystem, setFollowSystem] = useState<boolean>(false)
  const [animationsEnabled, setAnimationsEnabled] = useState<boolean>(true)
  const [systemPreference, setSystemPreference] = useState<'light' | 'dark' | null>(null)
  const [reducedMotion, setReducedMotion] = useState<boolean>(false)

  // Detect system preferences
  useEffect(() => {
    // Check system dark mode preference
    const darkModeQuery = window.matchMedia('(prefers-color-scheme: dark)')
    setSystemPreference(darkModeQuery.matches ? 'dark' : 'light')

    const handleDarkModeChange = (e: MediaQueryListEvent) => {
      setSystemPreference(e.matches ? 'dark' : 'light')
    }

    darkModeQuery.addEventListener('change', handleDarkModeChange)

    // Check reduced motion preference
    const reducedMotionQuery = window.matchMedia('(prefers-reduced-motion: reduce)')
    setReducedMotion(reducedMotionQuery.matches)
    if (reducedMotionQuery.matches) {
      setAnimationsEnabled(false)
    }

    const handleReducedMotionChange = (e: MediaQueryListEvent) => {
      setReducedMotion(e.matches)
      if (e.matches) {
        setAnimationsEnabled(false)
      }
    }

    reducedMotionQuery.addEventListener('change', handleReducedMotionChange)

    return () => {
      darkModeQuery.removeEventListener('change', handleDarkModeChange)
      reducedMotionQuery.removeEventListener('change', handleReducedMotionChange)
    }
  }, [])

  // Load saved preferences
  useEffect(() => {
    const savedTheme = localStorage.getItem('cyberrecon-theme')
    const savedFollowSystem = localStorage.getItem('cyberrecon-follow-system') === 'true'
    const savedAnimationsEnabled = localStorage.getItem('cyberrecon-animations') !== 'false'

    if (savedTheme && !savedFollowSystem) {
      setCurrentThemeId(savedTheme)
    }
    setFollowSystem(savedFollowSystem)
    setAnimationsEnabled(savedAnimationsEnabled && !reducedMotion)
  }, [reducedMotion])

  const currentTheme = defaultThemes.find(theme => theme.id === currentThemeId) || defaultThemes[0]

  // Apply theme to document
  useEffect(() => {
    const root = document.documentElement
    
    // Apply CSS variables
    Object.entries(currentTheme.cssVars).forEach(([key, value]) => {
      root.style.setProperty(key, value)
    })

    // Apply theme class for Tailwind
    root.className = `theme-${currentTheme.id}`

    // Apply animation preferences
    if (!animationsEnabled) {
      root.style.setProperty('--animation-duration', '0ms')
      root.style.setProperty('--transition-duration', '0ms')
    } else {
      root.style.removeProperty('--animation-duration')
      root.style.removeProperty('--transition-duration')
    }
  }, [currentTheme, animationsEnabled])

  const setTheme = (themeId: string) => {
    setCurrentThemeId(themeId)
    localStorage.setItem('cyberrecon-theme', themeId)
  }

  const handleFollowSystemChange = (follow: boolean) => {
    setFollowSystem(follow)
    localStorage.setItem('cyberrecon-follow-system', follow.toString())
  }

  const handleAnimationsChange = (enabled: boolean) => {
    if (!reducedMotion) {
      setAnimationsEnabled(enabled)
      localStorage.setItem('cyberrecon-animations', enabled.toString())
    }
  }

  const contextValue: ThemeContextType = {
    currentTheme,
    themes: defaultThemes,
    setTheme,
    isDarkMode: true, // We're focusing on dark themes for now
    systemPreference,
    followSystem,
    setFollowSystem: handleFollowSystemChange,
    animationsEnabled,
    setAnimationsEnabled: handleAnimationsChange,
    reducedMotion
  }

  return (
    <ThemeContext.Provider value={contextValue}>
      {children}
    </ThemeContext.Provider>
  )
}