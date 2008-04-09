# Microsoft Developer Studio Project File - Name="GSPlayer2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GSPlayer2 - Win32 DebugUnicode
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "GSPlayer2.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "GSPlayer2.mak" CFG="GSPlayer2 - Win32 DebugUnicode"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "GSPlayer2 - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "GSPlayer2 - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE "GSPlayer2 - Win32 ReleaseUnicode" ("Win32 (x86) Application" 用)
!MESSAGE "GSPlayer2 - Win32 DebugUnicode" ("Win32 (x86) Application" 用)
!MESSAGE "GSPlayer2 - Win32 Release64" ("Win32 (x86) Application" 用)
!MESSAGE "GSPlayer2 - Win32 ReleaseUnicode64" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GSPlayer2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 msvcrt.lib ../../lib/Release/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/Release/libmad.lib ../../lib/Release/libovd.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/Release/GSPlayer2.exe"

!ELSEIF  "$(CFG)" == "GSPlayer2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../include" /I "." /I "../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msvcrtd.lib ../../lib/Debug/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/Release/libmad.lib ../../lib/Debug/libovd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/Debug/GSPlayer2.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "GSPlayer2 - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GSPlayer2___Win32_ReleaseUnicode"
# PROP BASE Intermediate_Dir "GSPlayer2___Win32_ReleaseUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msvcrt.lib ../../lib/Release/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/Release/libmad.lib ../../lib/Release/libovd.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/Release/GSPlayer2.exe"
# ADD LINK32 msvcrt.lib ../../lib/ReleaseUnicode/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/ReleaseUnicode/libmad.lib ../../lib/ReleaseUnicode/libovd.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/ReleaseUnicode/GSPlayer2.exe"

!ELSEIF  "$(CFG)" == "GSPlayer2 - Win32 DebugUnicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GSPlayer2___Win32_DebugUnicode"
# PROP BASE Intermediate_Dir "GSPlayer2___Win32_DebugUnicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../include" /I "." /I "../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../include" /I "." /I "../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msvcrtd.lib ../../lib/Debug/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/Release/libmad.lib ../../lib/Debug/libovd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/Debug/GSPlayer2.exe" /pdbtype:sept
# ADD LINK32 msvcrtd.lib ../../lib/DebugUnicode/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/ReleaseUnicode/libmad.lib ../../lib/DebugUnicode/libovd.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/DebugUnicode/GSPlayer2.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "GSPlayer2 - Win32 Release64"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GSPlayer2___Win32_Release64"
# PROP BASE Intermediate_Dir "GSPlayer2___Win32_Release64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release64"
# PROP Intermediate_Dir "Release64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msvcrt.lib ../../lib/Release/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/Release/libmad.lib ../../lib/Release/libovd.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/Release/GSPlayer2.exe"
# ADD LINK32 msvcrt.lib ../../lib/Release/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/Release64/libmad.lib ../../lib/Release/libovd.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/Release64/GSPlayer2.exe"

!ELSEIF  "$(CFG)" == "GSPlayer2 - Win32 ReleaseUnicode64"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GSPlayer2___Win32_ReleaseUnicode64"
# PROP BASE Intermediate_Dir "GSPlayer2___Win32_ReleaseUnicode64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode64"
# PROP Intermediate_Dir "ReleaseUnicode64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../../include" /I "." /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msvcrt.lib ../../lib/ReleaseUnicode/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/ReleaseUnicode/libmad.lib ../../lib/ReleaseUnicode/libovd.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/ReleaseUnicode/GSPlayer2.exe"
# ADD LINK32 msvcrt.lib ../../lib/ReleaseUnicode/maplay.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib ws2_32.lib msacm32.lib ../../lib/ReleaseUnicode64/libmad.lib ../../lib/ReleaseUnicode/libovd.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib /out:"../../bin/jpn/Win32/ReleaseUnicode64/GSPlayer2.exe"

!ENDIF 

# Begin Target

# Name "GSPlayer2 - Win32 Release"
# Name "GSPlayer2 - Win32 Debug"
# Name "GSPlayer2 - Win32 ReleaseUnicode"
# Name "GSPlayer2 - Win32 DebugUnicode"
# Name "GSPlayer2 - Win32 Release64"
# Name "GSPlayer2 - Win32 ReleaseUnicode64"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\EffectDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\EffectDlgSave.cpp
# End Source File
# Begin Source File

SOURCE=..\GSPlayer2.cpp
# End Source File
# Begin Source File

SOURCE=.\GSPlayer2.rc
# End Source File
# Begin Source File

SOURCE=..\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\MainWndCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\MainWndCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\MainWndPls.cpp
# End Source File
# Begin Source File

SOURCE=..\MultiBuff.cpp
# End Source File
# Begin Source File

SOURCE=..\Options.cpp
# End Source File
# Begin Source File

SOURCE=..\PlayerApp.cpp
# End Source File
# Begin Source File

SOURCE=..\PlayListDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Skin.cpp
# End Source File
# Begin Source File

SOURCE=..\SmallSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32Options.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32PlayerApp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\EffectDlg.h
# End Source File
# Begin Source File

SOURCE=..\GSPlayer2.h
# End Source File
# Begin Source File

SOURCE=..\MainWnd.h
# End Source File
# Begin Source File

SOURCE=..\MultiBuff.h
# End Source File
# Begin Source File

SOURCE=..\Options.h
# End Source File
# Begin Source File

SOURCE=..\PlayerApp.h
# End Source File
# Begin Source File

SOURCE=..\PlayListDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\Skin.h
# End Source File
# Begin Source File

SOURCE=..\SmallSlider.h
# End Source File
# Begin Source File

SOURCE=.\Win32MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\Win32Options.h
# End Source File
# Begin Source File

SOURCE=.\Win32PlayerApp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\res\back256.bmp
# End Source File
# Begin Source File

SOURCE=..\res\button.bmp
# End Source File
# Begin Source File

SOURCE=..\res\button_normal.bmp
# End Source File
# Begin Source File

SOURCE=..\res\button_push.bmp
# End Source File
# Begin Source File

SOURCE=..\res\buttons.bmp
# End Source File
# Begin Source File

SOURCE=..\res\file.ico
# End Source File
# Begin Source File

SOURCE=..\res\file_s.ico
# End Source File
# Begin Source File

SOURCE=..\res\main.ico
# End Source File
# Begin Source File

SOURCE=..\res\number.bmp
# End Source File
# Begin Source File

SOURCE=..\res\numbers.bmp
# End Source File
# Begin Source File

SOURCE=..\res\play_s.ico
# End Source File
# Begin Source File

SOURCE=..\res\playlist.ico
# End Source File
# Begin Source File

SOURCE=..\res\playopt.bmp
# End Source File
# Begin Source File

SOURCE=..\res\status.bmp
# End Source File
# Begin Source File

SOURCE=..\res\trackhpc.bmp
# End Source File
# Begin Source File

SOURCE=..\res\vol_slider.bmp
# End Source File
# Begin Source File

SOURCE=..\res\vol_track.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\xpstyle.xml
# End Source File
# End Target
# End Project
