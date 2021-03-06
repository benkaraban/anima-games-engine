; _makeInstall.nsi

;--------------------------------
;Include Modern UI

!include "MUI2.nsh"

;--------------------------------
;General

; The name of the installer
Name "Chrono Rage Press Preview"

; The file to write
OutFile "ChronoRage_press_preview.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Chrono Rage"
InstallDirRegKey HKLM "Software\ChronoRage" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

!insertmacro MUI_PAGE_LICENSE ".\licence_ChronoRage.txt"
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
Section "Chrono Rage (required)" ChronoRage
  ; SectionIn RO -> section is in ReadOnly, users can't change it
  SectionIn RO
  
  WriteRegStr HKLM "Software\ChronoRageDemo" "Install_Dir" "$INSTDIR"
  ; write uninstall strings
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage" "DisplayName" "Chrono Rage (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage" "DisplayIcon" '"$INSTDIR\ChronoRage.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage" "Publisher" "Anima Games"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage" "HelpLink" "http://www.chronorage.com"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage" "URLInfoAbout" "http://www.chronorage.com"
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\ChronoRage.exe"
  
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\ChronoRageData.pak"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\ChronoRageDict.pak"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\ChronoRageLevels.pak"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\ChronoRageShaders.pak"
  
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Assets.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Audio.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\AudioOpenAL.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\boost_date_time-vc90-mt-1_40.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Core.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Gfx.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\GfxDx9.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\GUI.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\lua52.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\ogg.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Renderer.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\RendererSM2.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Universe.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\vorbis.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\vorbisfile.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\Window.dll"
  
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\VFS-ChronoRage.xml"
  
  ; Redistributable install files
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\oalinst.exe"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\vcredist_x86.exe"

  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\DXSETUP.exe"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\DSETUP.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\dsetup32.dll"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\dxdllreg_x86.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\dxupdate.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\Aug2009_d3dx9_42_x86.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\Aug2009_d3dx9_42_x64.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\Oct2005_xinput_x86.cab"
  File "..\..\Build\VS2008_Shared\OutputExe\Release\ChronoRage\dxsetup\Oct2005_xinput_x64.cab"

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

  CreateDirectory "$SMPROGRAMS\Chrono Rage"
  CreateShortCut "$SMPROGRAMS\Chrono Rage\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Chrono Rage\Chrono Rage.lnk" "$INSTDIR\ChronoRage.exe" "" "$INSTDIR\ChronoRage.exe" 0
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Desktop Shortcut" Desktop

  CreateShortCut "$DESKTOP\Chrono Rage.lnk" "$INSTDIR\ChronoRage.exe" "" "$INSTDIR\ChronoRage.exe" 0
  
SectionEnd

;--------------------------------
; Descriptions

LangString DESC_ChronoRage ${LANG_ENGLISH} "Install Chrono Rage's files."
LangString DESC_StartMenu ${LANG_ENGLISH} "Create start menu shortcuts."
LangString DESC_Desktop ${LANG_ENGLISH} "Create a desktop shortcut."

LangString DESC_ChronoRage ${LANG_FRENCH} "Installe les fichiers n�c�ssaires au jeu Chrono Rage."
LangString DESC_StartMenu ${LANG_FRENCH} "Cr�e des raccourcis dans le menu D�marrer."
LangString DESC_Desktop ${LANG_FRENCH} "Cr�e un raccourcis sur le bureau."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${ChronoRage} $(DESC_ChronoRage)
  !insertmacro MUI_DESCRIPTION_TEXT ${StartMenu} $(DESC_StartMenu)
  !insertmacro MUI_DESCRIPTION_TEXT ${Desktop} $(DESC_Desktop)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChronoRage"
  DeleteRegKey HKLM "Software\ChronoRage"
  
  ; Remove files and uninstaller
  Delete '$INSTDIR\ChronoRage.exe'
  
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
  
  Delete '$INSTDIR\oalinst.exe'
  Delete '$INSTDIR\vcredist_x86.exe'
  
  Delete '$INSTDIR\uninstall.exe'
  
  ; Remove Settings File, if any
  Delete '$LOCALAPPDATA\AnimaGames\ChronoRage\Settings.xml'

  ; Remove Start Menu shortcuts, if any
  Delete "$SMPROGRAMS\Chrono Rage\*.*"
  
  ; Remove Desktop shortcuts if any
  Delete "$DESKTOP\Chrono Rage.lnk"
  
  ; Remove directories used
  RMDir "$SMPROGRAMS\Chrono Rage"
  RMDir "$INSTDIR"

SectionEnd
