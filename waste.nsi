!define APP_NAME_BIG "WASTE"
!define APP_NAME_VER "WASTE 1.5 Beta 2"
!define APP_NAME_SMALL "WASTE" ; used for directory name and registry
!define APP_EXENAME "waste.exe"
; Disable if you don't have upx.exe in the folder
!define HAVE_UPX "upx.exe"
OutFile waste-setup.exe
Icon "modern-install.ico"
UninstallIcon "modern-uninstall.ico"
BrandingText "${APP_NAME_VER}"
CheckBitmap "modern.bmp"
; Disable below when using NSIS older than 2.x
XPStyle on

Name "${APP_NAME_VER}"
Caption "${APP_NAME_VER} - Setup"

LicenseText "WASTE is free software. Please agree to the license below to continue."
LicenseData license.txt

ComponentText "This will install ${APP_NAME_VER}"
AutoCloseWindow true
ShowInstDetails show
ShowUninstDetails show
DirText "Please select a location to install ${APP_NAME_BIG} (or use the default):"
SetOverwrite on
SetDateSave on
!ifdef HAVE_UPX
  !packhdr tmp.dat "upx --force --best tmp.dat"
!endif

InstallDir $PROGRAMFILES\${APP_NAME_SMALL}
InstallDirRegKey HKLM SOFTWARE\${APP_NAME_SMALL} ""

Function .onInit
  ReadRegStr $0 HKLM SOFTWARE\${APP_NAME_BIG} ""
  StrCmp $0 "" end
   IfFileExists $SMPROGRAMS\${APP_NAME_BIG}\*.* smthere
     SectionSetFlags 3 0
   smthere:
end:
   IfFileExists $SMSTARTUP\${APP_NAME_BIG}.lnk slthere ; by default do not do startup
     SectionSetFlags 4 0
   slthere:

SectionSetFlags 3 0

FunctionEnd

Section "${APP_NAME_BIG} (required)" !Required
  SectionIn RO
  SetOutPath $INSTDIR
  File release\${APP_EXENAME}
  File release\${APP_EXENAME}.manifest
  File license.txt
  CreateDirectory $INSTDIR\Downloads
SectionEnd

Section "${APP_NAME_SMALL} Documentation (PDF)"
  SetOutPath $INSTDIR\Docs
  File Docs\documentation.pdf
  CreateDirectory $INSTDIR\Docs
  CreateShortCut "$SMPROGRAMS\${APP_NAME_BIG}\${APP_NAME_SMALL} Documentation (pdf).lnk" \
                 "$INSTDIR\Docs\documentation.pdf"
SectionEnd

Section "${APP_NAME_SMALL} Documentation (HTML)"
  SetOutPath $INSTDIR\Docs
  File Docs\documentation.html
  CreateDirectory $INSTDIR\Docs
  CreateShortCut "$SMPROGRAMS\${APP_NAME_BIG}\${APP_NAME_SMALL} Documentation (html).lnk" \
                 "$INSTDIR\Docs\documentation.html"
SectionEnd

Section "Start Menu shortcuts"
  CreateDirectory $SMPROGRAMS\${APP_NAME_BIG}
  CreateShortCut "$SMPROGRAMS\${APP_NAME_BIG}\Uninstall ${APP_NAME_BIG}.lnk" \
                 "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\${APP_NAME_BIG}\${APP_NAME_BIG}.lnk" \
                 "$INSTDIR\${APP_EXENAME}"
  CreateShortCut "$SMPROGRAMS\${APP_NAME_BIG}\${APP_NAME_BIG} License.lnk" \
                 "$INSTDIR\license.txt"
  CreateShortCut "$SMPROGRAMS\${APP_NAME_BIG}\${APP_NAME_BIG} Downloads Directory.lnk" \
                 "$INSTDIR\downloads"
SectionEnd

Section "Launch on startup"
  CreateShortCut "$SMSTARTUP\${APP_NAME_BIG}.lnk" \
                 "$INSTDIR\${APP_EXENAME}"
SectionEnd

Section -post

  WriteRegStr HKLM SOFTWARE\${APP_NAME_SMALL} "" $INSTDIR
  WriteRegStr HKCR waste "" "URL: WASTE Command Protocol"
  WriteRegStr HKCR waste "URL Protocol" ""
  WriteRegStr HKCR .wastestate "" "WASTESTATE"
  WriteRegStr HKCR WASTESTATE "" "WASTE transfer information file"
  WriteRegStr HKCR WASTESTATE\DefaultIcon "" "$INSTDIR\WASTE.exe,1"
  WriteRegStr HKCR waste\shell\open\command "" '"$INSTDIR\waste.exe" "%1"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME_SMALL}" \
                   "DisplayName" "${APP_NAME_BIG} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME_SMALL}" \
                   "UninstallString" '"$INSTDIR\uninst.exe"'


  ; since the installer is now created last (in 1.2+), this makes sure 
  ; that any old installer that is readonly is overwritten.
  Delete $INSTDIR\uninst.exe 
  WriteUninstaller $INSTDIR\uninst.exe
SectionEnd

Function .onInstSuccess
  Exec '"$INSTDIR\${APP_EXENAME}"'
FunctionEnd

!ifndef NO_UNINST
UninstallText "This will uninstall ${APP_NAME_BIG} from your system:"

Section Uninstall
TryAgain:
  SetFileAttributes $INSTDIR\${APP_EXENAME} NORMAL
  Delete $INSTDIR\${APP_EXENAME}
  IfFileExists "$INSTDIR\${APP_EXENAME}" 0 DeletedEXE
    MessageBox MB_RETRYCANCEL|MB_ICONQUESTION "${APP_EXENAME} appears to be locked, please make sure ${APP_NAME_BIG} is not running before uninstalling." IDRETRY TryAgain
    Abort "${APP_EXENAME} could not be removed. ${APP_NAME_BIG} may have been running."
DeletedEXE:
  
  Delete $INSTDIR\readme.txt
  Delete $INSTDIR\uninst.exe
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME_SMALL}"
  DeleteRegKey HKLM "Software\${APP_NAME_SMALL}"
  DeleteRegKey HKCR waste
  DeleteRegKey HKCR .wastestate
  DeleteRegKey HKCR WASTESTATE
  RMDir /r $SMPROGRAMS\${APP_NAME_BIG}
  Delete $SMSTARTUP\${APP_NAME_BIG}.lnk

  RMDir $INSTDIR\Downloads
  RMDir $INSTDIR\Docs

  IfFileExists $INSTDIR\downloads\*.* 0 NoDownloads

    MessageBox MB_YESNO|MB_ICONQUESTION "Remove any files you have downloaded (in $INSTDIR\downloads)?" IDNO NoDownloads
    Delete $INSTDIR\Downloads\*.*
    RMDir $INSTDIR\Downloads

NoDownloads:

  RMDir $INSTDIR
  
  MessageBox MB_YESNO|MB_ICONQUESTION "Remove network profiles from your ${APP_NAME_BIG} directory?" IDNO AllDone
    Delete $INSTDIR\*.*
    RMDir $INSTDIR
  AllDone:
SectionEnd

!endif
