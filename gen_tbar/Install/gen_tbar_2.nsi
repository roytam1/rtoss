Name "gen_tbar_2 Plugin"
OutFile "gen_tbar_2_.exe"
SetCompress auto
SetCompressor lzma
CRCCheck on
SetDatablockOptimize on
SetDateSave on

# -----------------------------------------------------------------------------------------------------

UninstallCaption "gen_tbar"
SilentInstall normal
SilentUnInstall normal
ShowUninstDetails nevershow
LicenseText "To continue installing this software, please accept the software license:" "Accept"
LicenseData "..\gpl.txt"

# -----------------------------------------------------------------------------------------------------

InstallDir $PROGRAMFILES\Winamp
InstallDirRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" "UninstallString"

DirText "Please select your Winamp path below:"

AutoCloseWindow true
ShowInstDetails nevershow

# -----------------------------------------------------------------------------------------------------

Section "gen_tbar (required)"

WriteUninstaller "$INSTDIR\Plugins\un_gen_tbar_2.exe"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\gen_tbar_2" "DisplayName" "gen_tbar (Winamp 2)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\gen_tbar_2" "UninstallString" '"$INSTDIR\Plugins\un_gen_tbar_2.exe"'

SetOutPath "$INSTDIR\Plugins"

# Delete old files and registry entries
Delete "$INSTDIR\Plugins\gen_tbar.dll"
Delete "$INSTDIR\Plugins\gen_tbar_2.chm"
DeleteRegValue HKEY_CURRENT_USER "Software\radix\gen_tbar" "redraw_old_caption"

File "gen_tbar_2.dll"
File "..\Help\gen_tbar.chm"

Exec '"$INSTDIR\Winamp.exe"'
SectionEnd

# -----------------------------------------------------------------------------------------------------

Section "Uninstall"
SetAutoClose true
BringToFront

FindWindow $1 "Winamp v1.x"
IsWindow $1 +1 +2
SendMessage $1 16 0 0 #WM_CLOSE

Delete "$INSTDIR\gen_tbar_2.dll"
IfErrors -1
Delete "$INSTDIR\gen_tbar.chm"
IfErrors -1
Delete "$INSTDIR\un_gen_tbar_2.exe"
IfErrors -1

DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\gen_tbar_2"
DeleteRegKey HKEY_CURRENT_USER "Software\radix\gen_tbar"

SectionEnd

# -----------------------------------------------------------------------------------------------------
