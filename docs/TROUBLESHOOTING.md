# CyberRecon Suite - Troubleshooting Guide

## Common Issues and Solutions

### Installation Issues

#### Qt6 Not Found
**Problem**: CMake cannot find Qt6 during build
**Solution**: 
```bash
# Install Qt6 development packages
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-charts-dev

# Ensure Qt6 is in PATH
export Qt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```

#### Build Failures on Windows
**Problem**: Build fails with MSVC compiler errors
**Solution**:
- Ensure Visual Studio 2019+ is installed
- Use the Visual Studio Developer Command Prompt
- Verify Qt6 is properly installed and in PATH

#### Permission Denied on Linux
**Problem**: Cannot execute build scripts
**Solution**:
```bash
chmod +x build.sh
chmod +x install.sh
```

### Runtime Issues

#### Application Won't Start
**Problem**: CyberReconSuite executable fails to launch
**Solutions**:

1. **Check Dependencies**:
```bash
# Linux
ldd ./CyberReconSuite
# Look for missing libraries

# Windows  
# Use Dependency Walker or run from command prompt to see error messages
```

2. **Database Permissions**:
```bash
# Ensure database directory is writable
chmod 755 ~/.cyberrecon/
chmod 644 ~/.cyberrecon/*.db
```

3. **Qt Runtime Libraries**:
```bash
# Windows: Ensure Qt6 runtime DLLs are in PATH or application directory
# Linux: Install Qt6 runtime packages
sudo apt install qt6-base-runtime
```

#### Database Connection Errors
**Problem**: "Cannot open database" errors
**Solution**:
```bash
# Check database file permissions
ls -la *.db

# Recreate corrupted database
rm cyberrecon_main.db
# Restart application to recreate
```

#### Network Monitoring Requires Root
**Problem**: Packet capture fails with permission errors
**Solution**:
```bash
# Linux: Grant network capabilities
sudo setcap cap_net_raw,cap_net_admin=eip ./CyberReconSuite

# Or run specific monitoring tools as root
sudo ./CyberReconSuite --network-only
```

### Performance Issues

#### High Memory Usage
**Problem**: Application consumes excessive RAM
**Solutions**:
- Reduce event retention period in settings
- Limit concurrent monitoring sessions
- Close unused dashboard tabs
- Consider upgrading to 16GB+ RAM for production use

#### Slow Dashboard Loading
**Problem**: Security dashboards load slowly
**Solutions**:
- Optimize database with VACUUM command:
```bash
sqlite3 cyberrecon_main.db "VACUUM;"
```
- Reduce dashboard refresh rate
- Limit time range for large datasets
- Enable database caching in settings

#### Network Scanning Performance
**Problem**: Network scans are slow or timeout
**Solutions**:
- Reduce scan thread count in settings
- Limit port range for scans
- Increase timeout values for slow networks
- Use targeted scanning instead of broad discovery

### Feature-Specific Issues

#### Penetration Testing Tools Not Working
**Problem**: Wireless or network testing tools fail
**Solutions**:
1. Verify proper authorization and legal compliance
2. Check network interface permissions
3. Ensure required system tools are installed:
```bash
# Linux
sudo apt install aircrack-ng nmap wireshark-common
```

#### SOAR Automation Failures
**Problem**: Automation workflows fail to execute
**Solutions**:
- Check automation logs in `/var/log/cyberrecon/`
- Verify API credentials and endpoints
- Test individual workflow steps manually
- Check network connectivity to external services

#### Threat Intelligence Feed Updates Failing
**Problem**: IOC feeds not updating
**Solutions**:
- Verify internet connectivity
- Check API rate limits and quotas
- Update feed URLs in configuration
- Clear feed cache and force refresh

### Platform-Specific Issues

#### Windows-Specific

**Problem**: Windows Defender blocks execution
**Solution**: Add exception for CyberReconSuite.exe and installation directory

**Problem**: Firewall blocks network features
**Solution**: Configure Windows Firewall to allow CyberRecon Suite

#### macOS-Specific

**Problem**: "App can't be opened because it is from an unidentified developer"
**Solution**: 
```bash
xattr -d com.apple.quarantine CyberReconSuite.app
```

#### Linux-Specific

**Problem**: Audio/video features not working
**Solution**: Install multimedia codecs and plugins

### Getting More Help

#### Debug Logging
Enable verbose logging for troubleshooting:
```bash
# Set environment variable
export CYBERRECON_DEBUG=1
./CyberReconSuite

# Or use command line flag
./CyberReconSuite --debug --verbose
```

#### Log Files
Check application logs for detailed error information:
- Linux: `~/.cyberrecon/logs/`
- Windows: `%APPDATA%\CyberRecon\logs\`
- macOS: `~/Library/Application Support/CyberRecon/logs/`

#### System Information
Gather system information for support requests:
```bash
# Linux
uname -a
lsb_release -a
qt6-config --version

# Windows
systeminfo | findstr /C:"OS Name" /C:"OS Version"

# macOS
sw_vers
system_profiler SPSoftwareDataType
```

#### Contact Support
If issues persist:
- **Community**: [GitHub Discussions](https://github.com/mllinman/cyberrecon-suite/discussions)
- **Bug Reports**: [GitHub Issues](https://github.com/mllinman/cyberrecon-suite/issues)
- **Professional Support**: support@bulletdropstudios.com
- **Emergency**: Enterprise customers receive dedicated support contacts

---

*This troubleshooting guide is updated regularly. Please check for the latest version when encountering issues.*