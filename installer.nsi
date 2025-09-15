; CyberRecon Suite v1.7 - NSIS Installer Script
; Advanced Windows Installer with proper registry entries and uninstaller

;--------------------------------
; Include Modern UI and additional functions
!include "MUI2.nsh"
!include "FileFunc.nsh"

;--------------------------------
; General Configuration
Name "CyberRecon Suite v1.7"
OutFile "CyberReconSuite-v1.7-Setup.exe"
Unicode True

; Default installation folder
InstallDir "$PROGRAMFILES64\CyberRecon Suite"

; Get installation folder from registry if available
InstallDirRegKey HKLM "Software\CyberRecon Suite" ""

; Request application privileges
RequestExecutionLevel admin

; Version Information
VIProductVersion "1.7.0.0"
VIAddVersionKey "ProductName" "CyberRecon Suite"
VIAddVersionKey "Comments" "Modern Cybersecurity Operations Platform"
VIAddVersionKey "CompanyName" "CyberRecon Solutions"
VIAddVersionKey "LegalCopyright" "© 2024 CyberRecon Solutions"
VIAddVersionKey "FileDescription" "CyberRecon Suite Installer"
VIAddVersionKey "FileVersion" "1.7.0.0"
VIAddVersionKey "ProductVersion" "1.7.0.0"
VIAddVersionKey "InternalName" "cyberrecon-suite"
VIAddVersionKey "LegalTrademarks" "CyberRecon Suite™"
VIAddVersionKey "OriginalFilename" "CyberReconSuite-v1.7-Setup.exe"

;--------------------------------
; Interface Configuration
!define MUI_ABORTWARNING
!define MUI_ICON "shield-icon.ico"
!define MUI_UNICON "shield-icon.ico"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "CyberRecon Suite v1.7 Setup"
!define MUI_WELCOMEPAGE_TEXT "This will install CyberRecon Suite v1.7 on your computer.$\r$\n$\r$\nCyberRecon Suite is a comprehensive cybersecurity operations platform featuring SIEM, threat intelligence, security orchestration, and compliance management.$\r$\n$\r$\nClick Next to continue."

; License page
!define MUI_LICENSEPAGE_TEXT_TOP "Please review the license terms before installing CyberRecon Suite."
!define MUI_LICENSEPAGE_TEXT_BOTTOM "If you accept the terms of the agreement, click I Agree to continue. You must accept the agreement to install CyberRecon Suite."
!define MUI_LICENSEPAGE_BUTTON "I &Agree"

; Components page
!define MUI_COMPONENTSPAGE_SMALLDESC

; Directory page
!define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will install CyberRecon Suite in the following folder. To install in a different folder, click Browse and select another folder. Click Next to continue."

; Installation page
!define MUI_INSTFILESPAGE_FINISHHEADER_TEXT "Installation Complete"
!define MUI_INSTFILESPAGE_FINISHHEADER_SUBTEXT "CyberRecon Suite has been successfully installed."

; Finish page
!define MUI_FINISHPAGE_TITLE "CyberRecon Suite Setup Complete"
!define MUI_FINISHPAGE_TEXT "CyberRecon Suite v1.7 has been installed on your computer.$\r$\n$\r$\nClick Finish to close this wizard."
!define MUI_FINISHPAGE_RUN "$INSTDIR\CyberReconSuite.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch CyberRecon Suite"
!define MUI_FINISHPAGE_LINK "Visit the CyberRecon Suite website"
!define MUI_FINISHPAGE_LINK_LOCATION "https://github.com/mllinman/cyberrecon-suite"

; Uninstaller
!define MUI_UNCONFIRMPAGE_TEXT_TOP "CyberRecon Suite will be uninstalled from the following folder. Click Uninstall to start the uninstallation."

;--------------------------------
; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Installer Sections

Section "CyberRecon Suite (required)" SecCore
  SectionIn RO
  
  ; Set output path to the installation directory
  SetOutPath $INSTDIR
  
  ; Main application files
  File "CyberReconSuite-v1.7-Windows\CyberReconSuite.exe"
  File "CyberReconSuite-v1.7-Windows\README.txt"
  File "CyberReconSuite-v1.7-Windows\LICENSE"
  File "CyberReconSuite-v1.7-Windows\VERSION.txt"
  
  ; Qt libraries and dependencies
  File /r "CyberReconSuite-v1.7-Windows\*.dll"
  File /r "CyberReconSuite-v1.7-Windows\platforms"
  File /r /x "*.exe" /x "*.bat" /x "*.txt" /x "LICENSE" "CyberReconSuite-v1.7-Windows\*.*"
  
  ; Store installation folder
  WriteRegStr HKLM "Software\CyberRecon Suite" "" $INSTDIR
  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ; Add to Add/Remove Programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "DisplayName" "CyberRecon Suite v1.7"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "DisplayIcon" "$INSTDIR\CyberReconSuite.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "Publisher" "CyberRecon Solutions"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "DisplayVersion" "1.7.0"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "NoRepair" 1
  
  ; Estimate installation size
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "EstimatedSize" "$0"
  
SectionEnd

Section "Desktop Shortcut" SecDesktop
  
  CreateShortCut "$DESKTOP\CyberRecon Suite.lnk" "$INSTDIR\CyberReconSuite.exe" "" "$INSTDIR\CyberReconSuite.exe" 0 SW_SHOWNORMAL ALT|CONTROL|SHIFT|F5 "CyberRecon Suite - Modern Cybersecurity Operations Platform"
  
SectionEnd

Section "Start Menu Shortcuts" SecStartMenu
  
  CreateDirectory "$SMPROGRAMS\CyberRecon Suite"
  CreateShortCut "$SMPROGRAMS\CyberRecon Suite\CyberRecon Suite.lnk" "$INSTDIR\CyberReconSuite.exe" "" "$INSTDIR\CyberReconSuite.exe" 0 SW_SHOWNORMAL "" "CyberRecon Suite - Cybersecurity Operations Platform"
  CreateShortCut "$SMPROGRAMS\CyberRecon Suite\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL "" "Uninstall CyberRecon Suite"
  
SectionEnd

Section "Documentation" SecDocs
  
  SetOutPath "$INSTDIR\docs"
  File /r "CyberReconSuite-v1.7-Windows\docs\*.*"
  
  ; Create documentation shortcut
  CreateShortCut "$SMPROGRAMS\CyberRecon Suite\Documentation.lnk" "$INSTDIR\docs" "" "" 0 SW_SHOWNORMAL "" "CyberRecon Suite Documentation"
  
SectionEnd

Section "Demo Files" SecDemo
  
  SetOutPath "$INSTDIR\demo"
  File /r /x "*.exe" "CyberReconSuite-v1.7-Windows\demo\*.*"
  
SectionEnd

;--------------------------------
; Section Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} "Core application files (required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Create desktop shortcut"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} "Create Start Menu shortcuts"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDocs} "Install documentation files"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDemo} "Install demo and sample files"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Installer Functions

Function .onInit
  
  ; Check if already installed
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite" "UninstallString"
  StrCmp $R0 "" done
  
  MessageBox MB_OKCANCEL|MB_ICONQUESTION \
    "CyberRecon Suite is already installed. $\n$\nClick 'OK' to remove the previous version or 'Cancel' to cancel this upgrade." \
    IDOK uninst
  Abort
  
uninst:
  ClearErrors
  ExecWait '$R0 _?=$INSTDIR'
  
  IfErrors no_remove_uninstaller done
    ; You can either use Delete /REBOOTOK in the uninstaller or add some code
    ; here to remove the uninstaller. Use a registry key to check
    ; whether the user has chosen to uninstall. If you are using an uninstaller
    ; components page, make sure all sections are uninstalled.
  no_remove_uninstaller:
  
done:
  
FunctionEnd

;--------------------------------
; Uninstaller Section

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CyberReconSuite"
  DeleteRegKey HKLM "Software\CyberRecon Suite"
  
  ; Remove files and uninstaller
  Delete "$INSTDIR\CyberReconSuite.exe"
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\VERSION.txt"
  
  ; Remove Qt libraries
  Delete "$INSTDIR\*.dll"
  RMDir /r "$INSTDIR\platforms"
  RMDir /r "$INSTDIR\styles"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\iconengines"
  RMDir /r "$INSTDIR\bearer"
  
  ; Remove documentation
  RMDir /r "$INSTDIR\docs"
  
  ; Remove demo files
  RMDir /r "$INSTDIR\demo"
  
  ; Remove shortcuts
  Delete "$DESKTOP\CyberRecon Suite.lnk"
  Delete "$SMPROGRAMS\CyberRecon Suite\*.*"
  RMDir "$SMPROGRAMS\CyberRecon Suite"
  
  ; Remove installation directory
  RMDir "$INSTDIR"
  
SectionEnd

;--------------------------------
; Uninstaller Functions

Function un.onInit
  
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove CyberRecon Suite and all of its components?" IDYES +2
  Abort
  
FunctionEnd

Function un.onUninstSuccess
  
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "CyberRecon Suite was successfully removed from your computer."
  
FunctionEnd