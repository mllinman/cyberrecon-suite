import React from 'react'
import { Link } from 'react-router-dom'
import { Shield, Mail, Phone, MapPin, Github, Linkedin, Twitter } from 'lucide-react'

const Footer = () => {
  return (
    <footer className="bg-slate-900 border-t border-slate-700/50">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-12">
        <div className="grid grid-cols-1 md:grid-cols-4 gap-8">
          {/* Company Info */}
          <div className="col-span-1 md:col-span-2">
            <div className="flex items-center space-x-3 mb-4">
              <img src="/shield-icon.svg" alt="BulletDrop Studios" className="h-8 w-8" />
              <div>
                <h3 className="text-xl font-bold text-white">BulletDrop Studios</h3>
                <p className="text-sm text-slate-400">Advanced Cybersecurity Solutions</p>
              </div>
            </div>
            <p className="text-slate-400 mb-4 max-w-md">
              Empowering organizations with cutting-edge cybersecurity tools and platforms. 
              Specializing in threat intelligence, security orchestration, and penetration testing solutions.
            </p>
            <div className="space-y-2 text-sm text-slate-400">
              <div className="flex items-center space-x-2">
                <MapPin className="h-4 w-4" />
                <span>Owosso, Michigan 48867</span>
              </div>
              <div className="flex items-center space-x-2">
                <Phone className="h-4 w-4" />
                <span>+1 (989) 323-9690</span>
              </div>
              <div className="flex items-center space-x-2">
                <Mail className="h-4 w-4" />
                <span>info@bulletdropstudio.com</span>
              </div>
            </div>
          </div>

          {/* Quick Links */}
          <div>
            <h4 className="text-lg font-semibold text-white mb-4">Quick Links</h4>
            <ul className="space-y-2">
              <li><Link to="/products" className="text-slate-400 hover:text-primary-400 transition-colors">Products</Link></li>
              <li><Link to="/products/cyberrecon" className="text-slate-400 hover:text-primary-400 transition-colors">CyberRecon Suite</Link></li>
              <li><Link to="/about" className="text-slate-400 hover:text-primary-400 transition-colors">About Us</Link></li>
              <li><Link to="/contact" className="text-slate-400 hover:text-primary-400 transition-colors">Contact</Link></li>
              <li><Link to="/login" className="text-slate-400 hover:text-primary-400 transition-colors">Customer Portal</Link></li>
            </ul>
          </div>

          {/* Legal & Support */}
          <div>
            <h4 className="text-lg font-semibold text-white mb-4">Support</h4>
            <ul className="space-y-2">
              <li><a href="mailto:support@bulletdropstudio.com" className="text-slate-400 hover:text-primary-400 transition-colors">Technical Support</a></li>
              <li><a href="mailto:security@bulletdropstudio.com" className="text-slate-400 hover:text-primary-400 transition-colors">Security Issues</a></li>
              <li><a href="/docs" className="text-slate-400 hover:text-primary-400 transition-colors">Documentation</a></li>
              <li><a href="/privacy" className="text-slate-400 hover:text-primary-400 transition-colors">Privacy Policy</a></li>
              <li><a href="/terms" className="text-slate-400 hover:text-primary-400 transition-colors">Terms of Service</a></li>
            </ul>
          </div>
        </div>

        {/* Bottom Section */}
        <div className="mt-8 pt-8 border-t border-slate-700/50 flex flex-col md:flex-row justify-between items-center">
          <div className="text-slate-400 text-sm">
            <p>&copy; 2024 BulletDrop Studios. All rights reserved.</p>
            <p className="mt-1">EIN: 88-2524604 | Owosso, Michigan</p>
          </div>
          
          {/* Social Links */}
          <div className="flex space-x-4 mt-4 md:mt-0">
            <a href="https://github.com/bulletdropstudios" className="text-slate-400 hover:text-primary-400 transition-colors">
              <Github className="h-5 w-5" />
            </a>
            <a href="https://linkedin.com/company/bulletdrop-studios" className="text-slate-400 hover:text-primary-400 transition-colors">
              <Linkedin className="h-5 w-5" />
            </a>
            <a href="https://twitter.com/bulletdropstudios" className="text-slate-400 hover:text-primary-400 transition-colors">
              <Twitter className="h-5 w-5" />
            </a>
          </div>
        </div>
      </div>
    </footer>
  )
}

export default Footer