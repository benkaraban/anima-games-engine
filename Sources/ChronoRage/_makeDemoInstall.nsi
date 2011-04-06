; _makeInstall.nsi

;--------------------------------
;Include Modern UI

!include "MUI2.nsh"

;--------------------------------
;General

; The name of the installer
Name "Chrono Rage Demo"

; The file to write
OutFile "ChronoRage_demo.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Chrono Rage Demo"
InstallDirRegKey HKLM "Software\ChronoRageDemo" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

;--------------------------------

; The stuff to install
Section "Chrono Rage Demo (required)" ChronoRage

  ; SectionIn RO -> section is in ReadOnly, users can't change it
  SectionIn RO
  
  WriteRegStr HKLM "Software\ChronoRageDemo" "Install_Dir" "$INSTDIR"
  ; write uninstall strings
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo" "DisplayName" "Chrono Rage Demo (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo" "DisplayIcon" '"$INSTDIR\ChronoRageDemo.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo" "Publisher" "Anima Games"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo" "HelpLink" "http://www.chronorage.com"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo" "URLInfoAbout" "http://www.chronorage.com"
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\ChronoRageDemo.exe"
  
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\ChronoRageData.pak"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\ChronoRageDict.pak"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\ChronoRageLevels.pak"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\ChronoRageShaders.pak"
  
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Assets.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Audio.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\AudioOpenAL.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\boost_date_time-vc90-mt-1_40.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Core.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Gfx.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\GfxDx9.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\GUI.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\lua52.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\ogg.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Renderer.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\RendererSM2.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Universe.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\vorbis.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\vorbisfile.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\Window.dll"
  
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\VFS-ChronoRage.xml"
   
  ; Redistributable install files
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\oalinst.exe"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\vcredist_x86.exe"

  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\DXSETUP.exe"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\DSETUP.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\dsetup32.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\dxdllreg_x86.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\dxupdate.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\Aug2009_d3dx9_42_x86.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\Aug2009_d3dx9_42_x64.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\Oct2005_xinput_x86.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRageDemo\dxsetup\Oct2005_xinput_x64.cab"
  
  WriteUninstaller "uninstall.exe"
  
  ExecWait '"$INSTDIR\vcredist_x86.exe" /q:a'
  ExecWait '"$INSTDIR\oalinst.exe" /S'
  ExecWait '"$INSTDIR\DXSETUP.exe" /silent'

  Delete "$INSTDIR\oalinst.exe"
  Delete "$INSTDIR\vcredist_x86.exe"

  Delete "$INSTDIR\DXSETUP.exe"
  Delete "$INSTDIR\DSETUP.dll"
  Delete "$INSTDIR\dsetup32.dll"
  Delete "$INSTDIR\dxdllreg_x86.cab"
  Delete "$INSTDIR\dxupdate.cab"
  Delete "$INSTDIR\Aug2009_d3dx9_42_x86.cab"
  Delete "$INSTDIR\Aug2009_d3dx9_42_x64.cab"
  Delete "$INSTDIR\Oct2005_xinput_x86.cab"
  Delete "$INSTDIR\Oct2005_xinput_x64.cab"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts" StartMenu

  CreateDirectory "$SMPROGRAMS\Chrono Rage Demo"
  CreateShortCut "$SMPROGRAMS\Chrono Rage Demo\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Chrono Rage Demo\Chrono Rage Demo.lnk" "$INSTDIR\ChronoRageDemo.exe" "" "$INSTDIR\ChronoRageDemo.exe" 0
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Desktop Shortcut" Desktop

  CreateShortCut "$DESKTOP\Chrono Rage Demo.lnk" "$INSTDIR\ChronoRageDemo.exe" "" "$INSTDIR\ChronoRageDemo.exe" 0
  
SectionEnd

;--------------------------------
; Descriptions

LangString DESC_ChronoRage ${LANG_ENGLISH} "Install Chrono Rage Demo's files."
LangString DESC_StartMenu ${LANG_ENGLISH} "Create start menu shortcuts."
LangString DESC_Desktop ${LANG_ENGLISH} "Create a desktop shortcut."

LangString DESC_ChronoRage ${LANG_FRENCH} "Installe les fichiers nécéssaires à la démo Chrono Rage."
LangString DESC_StartMenu ${LANG_FRENCH} "Crée des raccourcis dans le menu Démarrer."
LangString DESC_Desktop ${LANG_FRENCH} "Crée un raccourcis sur le bureau."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${ChronoRage} $(DESC_ChronoRage)
  !insertmacro MUI_DESCRIPTION_TEXT ${StartMenu} $(DESC_StartMenu)
  !insertmacro MUI_DESCRIPTION_TEXT ${Desktop} $(DESC_Desktop)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------

; Uninstaller

Section "Uninstall"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRageDemo"
  DeleteRegKey HKLM "Software\ChronoRageDemo"
  
  ; Remove files and uninstaller
  Delete '$INSTDIR\ChronoRageDemo.exe'
  
  Delete '$INSTDIR\ChronoRageData.pak'
  Delete '$INSTDIR\ChronoRageDict.pak'
  Delete '$INSTDIR\ChronoRageLevels.pak'
  Delete '$INSTDIR\ChronoRageShaders.pak'
  
  Delete '$INSTDIR\Assets.dll'
  Delete '$INSTDIR\Audio.dll'
  Delete '$INSTDIR\AudioOpenAL.dll'
  Delete '$INSTDIR\boost_date_time-vc90-mt-1_40.dll'
  Delete '$INSTDIR\Core.dll'
  Delete '$INSTDIR\Gfx.dll'
  Delete '$INSTDIR\GfxDx9.dll'
  Delete '$INSTDIR\GUI.dll'
  Delete '$INSTDIR\lua52.dll'
  Delete '$INSTDIR\ogg.dll'
  Delete '$INSTDIR\Renderer.dll'
  Delete '$INSTDIR\RendererSM2.dll'
  Delete '$INSTDIR\Universe.dll'
  Delete '$INSTDIR\vorbis.dll'
  Delete '$INSTDIR\vorbisfile.dll'
  Delete '$INSTDIR\Window.dll'
  
  Delete '$INSTDIR\VFS-ChronoRage.xml'
   
  Delete '$INSTDIR\uninstall.exe'
  
  ; Remove Settings File, if any
  Delete '$LOCALAPPDATA\AnimaGames\ChronoRage\Settings.xml'

  ; Remove Start Menu shortcuts, if any
  Delete "$SMPROGRAMS\Chrono Rage Demo\*.*"
  
  ; Remove Desktop shortcuts if any
  Delete "$DESKTOP\Chrono Rage Demo.lnk"
  
  ; Remove directories used
  RMDir "$SMPROGRAMS\Chrono Rage Demo"
  RMDir "$INSTDIR"

SectionEnd
