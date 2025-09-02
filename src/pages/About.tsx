import React from 'react'
import { motion } from 'framer-motion'
import { Shield, Target, Users, Award, MapPin, Mail, Phone } from 'lucide-react'

const About = () => {
  const values = [
    {
      icon: Shield,
      title: 'Security First',
      description: 'Every solution we build prioritizes security, privacy, and data protection from the ground up.'
    },
    {
      icon: Target,
      title: 'Precision Engineering',
      description: 'We deliver precise, reliable cybersecurity tools that meet the exact needs of security professionals.'
    },
    {
      icon: Users,
      title: 'Client Success',
      description: 'Our success is measured by our clients\' enhanced security posture and operational efficiency.'
    },
    {
      icon: Award,
      title: 'Excellence',
      description: 'We maintain the highest standards in cybersecurity development and professional service delivery.'
    }
  ]

  const team = [
    {
      name: 'Michael Linman',
      role: 'Founder & CEO',
      bio: 'Cybersecurity expert with 15+ years of experience in security operations, threat intelligence, and enterprise security architecture.',
      image: 'https://images.pexels.com/photos/2379004/pexels-photo-2379004.jpeg?auto=compress&cs=tinysrgb&w=400'
    }
  ]

  return (
    <div className="pt-16">
      {/* Hero Section */}
      <section className="py-20 bg-gradient-to-br from-primary-900/20 to-dark-800/40">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.8 }}
            className="text-center"
          >
            <h1 className="text-5xl md:text-6xl font-bold mb-6">
              <span className="text-white">About</span>
              <br />
              <span className="gradient-text">BulletDrop Studios</span>
            </h1>
            <p className="text-xl text-gray-300 max-w-3xl mx-auto">
              Founded in Owosso, Michigan, we specialize in developing advanced cybersecurity 
              solutions that empower security professionals to protect their organizations.
            </p>
          </motion.div>
        </div>
      </section>

      {/* Mission Section */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-12 items-center">
            <motion.div
              initial={{ opacity: 0, x: -30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6 }}
            >
              <h2 className="text-4xl font-bold mb-6">
                <span className="gradient-text">Our Mission</span>
              </h2>
              <p className="text-lg text-gray-300 mb-6 leading-relaxed">
                At BulletDrop Studios, we believe that effective cybersecurity requires more than just tools—it requires 
                intelligent, integrated platforms that enhance human expertise and accelerate response capabilities.
              </p>
              <p className="text-lg text-gray-300 leading-relaxed">
                We develop cutting-edge cybersecurity solutions that combine advanced threat detection, 
                automated response capabilities, and comprehensive security testing tools to help organizations 
                stay ahead of evolving cyber threats.
              </p>
            </motion.div>

            <motion.div
              initial={{ opacity: 0, x: 30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6, delay: 0.2 }}
              className="relative"
            >
              <div className="bg-dark-800 rounded-2xl p-8 border border-dark-700">
                <div className="grid grid-cols-2 gap-6">
                  <div className="text-center">
                    <div className="text-3xl font-bold text-primary-400 mb-2">15+</div>
                    <div className="text-gray-400">Years Experience</div>
                  </div>
                  <div className="text-center">
                    <div className="text-3xl font-bold text-primary-400 mb-2">500+</div>
                    <div className="text-gray-400">Security Professionals</div>
                  </div>
                  <div className="text-center">
                    <div className="text-3xl font-bold text-primary-400 mb-2">99.9%</div>
                    <div className="text-gray-400">Platform Uptime</div>
                  </div>
                  <div className="text-center">
                    <div className="text-3xl font-bold text-primary-400 mb-2">24/7</div>
                    <div className="text-gray-400">Enterprise Support</div>
                  </div>
                </div>
              </div>
            </motion.div>
          </div>
        </div>
      </section>

      {/* Values Section */}
      <section className="py-20 bg-dark-800/50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Our Values</span>
            </h2>
            <p className="text-xl text-gray-300">
              The principles that guide everything we do
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8">
            {values.map((value, index) => (
              <motion.div
                key={value.title}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                className="card text-center group"
              >
                <div className="mb-4">
                  <value.icon className="h-12 w-12 text-primary-500 mx-auto group-hover:text-primary-400 transition-colors" />
                </div>
                <h3 className="text-xl font-semibold text-white mb-3">{value.title}</h3>
                <p className="text-gray-400 leading-relaxed">{value.description}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Team Section */}
      <section className="py-20">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <motion.div
            initial={{ opacity: 0, y: 30 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
            className="text-center mb-16"
          >
            <h2 className="text-4xl font-bold mb-4">
              <span className="gradient-text">Leadership Team</span>
            </h2>
            <p className="text-xl text-gray-300">
              Experienced cybersecurity professionals leading innovation
            </p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8 justify-center">
            {team.map((member, index) => (
              <motion.div
                key={member.name}
                initial={{ opacity: 0, y: 30 }}
                whileInView={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, delay: index * 0.2 }}
                className="card text-center max-w-sm mx-auto"
              >
                <div className="mb-6">
                  <img
                    src={member.image}
                    alt={member.name}
                    className="w-32 h-32 rounded-full mx-auto object-cover border-4 border-primary-500/20"
                  />
                </div>
                <h3 className="text-xl font-semibold text-white mb-2">{member.name}</h3>
                <p className="text-primary-400 font-medium mb-4">{member.role}</p>
                <p className="text-gray-400 leading-relaxed">{member.bio}</p>
              </motion.div>
            ))}
          </div>
        </div>
      </section>

      {/* Company Info Section */}
      <section className="py-20 bg-dark-800/50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-12">
            <motion.div
              initial={{ opacity: 0, x: -30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6 }}
            >
              <h2 className="text-3xl font-bold mb-6">
                <span className="gradient-text">Company Information</span>
              </h2>
              <div className="space-y-4">
                <div className="flex items-center space-x-3">
                  <MapPin className="h-5 w-5 text-primary-500" />
                  <span className="text-gray-300">Owosso, Michigan 48867, United States</span>
                </div>
                <div className="flex items-center space-x-3">
                  <Phone className="h-5 w-5 text-primary-500" />
                  <span className="text-gray-300">+1 (989) 323-9690</span>
                </div>
                <div className="flex items-center space-x-3">
                  <Mail className="h-5 w-5 text-primary-500" />
                  <span className="text-gray-300">info@bulletdropstudio.com</span>
                </div>
              </div>
              
              <div className="mt-8 p-6 bg-dark-700/50 rounded-lg">
                <h3 className="text-lg font-semibold text-white mb-3">Business Details</h3>
                <div className="space-y-2 text-sm text-gray-400">
                  <p><strong>EIN:</strong> 88-2524604</p>
                  <p><strong>Founded:</strong> 2024</p>
                  <p><strong>Industry:</strong> Cybersecurity Software Development</p>
                  <p><strong>Specialization:</strong> Security Operations Platforms</p>
                </div>
              </div>
            </motion.div>

            <motion.div
              initial={{ opacity: 0, x: 30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6, delay: 0.2 }}
            >
              <h2 className="text-3xl font-bold mb-6">
                <span className="gradient-text">Why Choose Us?</span>
              </h2>
              <div className="space-y-6">
                <div className="flex items-start space-x-4">
                  <div className="w-8 h-8 bg-primary-500/20 rounded-lg flex items-center justify-center flex-shrink-0 mt-1">
                    <CheckCircle className="h-5 w-5 text-primary-500" />
                  </div>
                  <div>
                    <h4 className="font-semibold text-white mb-2">Industry Expertise</h4>
                    <p className="text-gray-400">Deep understanding of cybersecurity challenges and modern threat landscapes.</p>
                  </div>
                </div>
                
                <div className="flex items-start space-x-4">
                  <div className="w-8 h-8 bg-primary-500/20 rounded-lg flex items-center justify-center flex-shrink-0 mt-1">
                    <CheckCircle className="h-5 w-5 text-primary-500" />
                  </div>
                  <div>
                    <h4 className="font-semibold text-white mb-2">Cutting-Edge Technology</h4>
                    <p className="text-gray-400">Modern development practices using the latest cybersecurity frameworks and tools.</p>
                  </div>
                </div>
                
                <div className="flex items-start space-x-4">
                  <div className="w-8 h-8 bg-primary-500/20 rounded-lg flex items-center justify-center flex-shrink-0 mt-1">
                    <CheckCircle className="h-5 w-5 text-primary-500" />
                  </div>
                  <div>
                    <h4 className="font-semibold text-white mb-2">Proven Results</h4>
                    <p className="text-gray-400">Track record of successful deployments in enterprise security environments.</p>
                  </div>
                </div>
                
                <div className="flex items-start space-x-4">
                  <div className="w-8 h-8 bg-primary-500/20 rounded-lg flex items-center justify-center flex-shrink-0 mt-1">
                    <CheckCircle className="h-5 w-5 text-primary-500" />
                  </div>
                  <div>
                    <h4 className="font-semibold text-white mb-2">Ongoing Support</h4>
                    <p className="text-gray-400">Comprehensive support and maintenance to ensure optimal security operations.</p>
                  </div>
                </div>
              </div>
            </motion.div>
          </div>
        </div>
      </section>
    </div>
  )
}

export default About