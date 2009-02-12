# Microsoft Developer Studio Project File - Name="Lite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Lite - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lite.mak" CFG="Lite - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lite - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Lite - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Lite", EAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lite - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\SimpXmlParser" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib winmm.lib msimg32.lib Version.lib /nologo /subsystem:windows /machine:I386 /out:"Release/PCMan/PCMan.exe"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PreLink_Cmds=$(OutDir)\BuildMenu.exe $(OutDir)\PCMan\Config	..\Version_PreBuildEvent_Lite.bat 1
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Lite - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SimpXmlParser" /D "_DEBUG" /D "BUILD_UI" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "USE_DEBUG_CONSOLE" /Fr /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib winmm.lib msimg32.lib Version.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"Debug/PCMan/PCMan.exe"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PreLink_Cmds=$(OutDir)\BuildMenu.exe $(OutDir)\PCMan\Config	..\Version_PreBuildEvent_Lite.bat 1
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Lite - Win32 Release"
# Name "Lite - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AES.cpp
# End Source File
# Begin Source File

SOURCE=.\AnsiBar.cpp
# End Source File
# Begin Source File

SOURCE=.\AppConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoComplete.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoReplyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoUpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BBSHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseDirDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ChiConv\ChiConv.cpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\Common.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ConfigFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Conn.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug_Kit.cpp
# End Source File
# Begin Source File

SOURCE=..\DlgLayout\DlgLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\DragTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FavMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Hotkey.cpp
# End Source File
# Begin Source File

SOURCE=.\HotkeyEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLinkPage.cpp
# End Source File
# Begin Source File

SOURCE=.\InputNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InstantTranDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyMap.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\Lite.rc
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MemIniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseCTL.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseGesture.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseGesture.h
# End Source File
# Begin Source File

SOURCE=.\OleImage.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PCMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ReBarState.cpp
# End Source File
# Begin Source File

SOURCE=.\Rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\SetBkDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SimpXmlParser\SimpXmlParser.cpp
# End Source File
# Begin Source File

SOURCE=.\sitelistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SitePage.cpp
# End Source File
# Begin Source File

SOURCE=.\SiteSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\strutils.cpp
# End Source File
# Begin Source File

SOURCE=.\TelnetConn.cpp
# End Source File
# Begin Source File

SOURCE=.\TermView.cpp
# End Source File
# Begin Source File

SOURCE=.\TriggerList.cpp
# End Source File
# Begin Source File

SOURCE=.\Ucs2Conv.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowState.cpp
# End Source File
# Begin Source File

SOURCE=.\WinUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddressDlg.h
# End Source File
# Begin Source File

SOURCE=.\AES.h
# End Source File
# Begin Source File

SOURCE=.\AnsiBar.h
# End Source File
# Begin Source File

SOURCE=.\AppConfig.h
# End Source File
# Begin Source File

SOURCE=.\AutoComplete.h
# End Source File
# Begin Source File

SOURCE=.\AutoReplyPage.h
# End Source File
# Begin Source File

SOURCE=.\AutoUpdateDlg.h
# End Source File
# Begin Source File

SOURCE=.\BBSHyperLink.h
# End Source File
# Begin Source File

SOURCE=.\BrowseDirDlg.h
# End Source File
# Begin Source File

SOURCE=..\ChiConv\ChiConv.h
# End Source File
# Begin Source File

SOURCE=.\Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\ColorConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConfigFile.h
# End Source File
# Begin Source File

SOURCE=.\Conn.h
# End Source File
# Begin Source File

SOURCE=.\ConnectPage.h
# End Source File
# Begin Source File

SOURCE=.\CtrlEdit.h
# End Source File
# Begin Source File

SOURCE=.\CustomizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\CustomTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CustomToolBar.h
# End Source File
# Begin Source File

SOURCE=.\Debug_Kit.h
# End Source File
# Begin Source File

SOURCE=..\DlgLayout\DlgLayout.h
# End Source File
# Begin Source File

SOURCE=.\DragTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FavMenu.h
# End Source File
# Begin Source File

SOURCE=.\FileUtil.h
# End Source File
# Begin Source File

SOURCE=.\GeneralPage.h
# End Source File
# Begin Source File

SOURCE=.\Hotkey.h
# End Source File
# Begin Source File

SOURCE=.\HotkeyEdit.h
# End Source File
# Begin Source File

SOURCE=.\HyperLinkPage.h
# End Source File
# Begin Source File

SOURCE=.\InputNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\InstantTranDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyMap.h
# End Source File
# Begin Source File

SOURCE=.\KeyMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\ListDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MemIniFile.h
# End Source File
# Begin Source File

SOURCE=.\OleImage.h
# End Source File
# Begin Source File

SOURCE=.\OtherPage.h
# End Source File
# Begin Source File

SOURCE=.\PasswdDlg.h
# End Source File
# Begin Source File

SOURCE=.\PCMan.h
# End Source File
# Begin Source File

SOURCE=.\ReBarState.h
# End Source File
# Begin Source File

SOURCE=..\Resource\resource.h
# End Source File
# Begin Source File

SOURCE=.\RFC854.H
# End Source File
# Begin Source File

SOURCE=.\Rijndael.h
# End Source File
# Begin Source File

SOURCE=.\SearchPlugin.h
# End Source File
# Begin Source File

SOURCE=.\SetBkDlg.h
# End Source File
# Begin Source File

SOURCE=..\SimpXmlParser\SimpXmlParser.h
# End Source File
# Begin Source File

SOURCE=.\sitelistctrl.h
# End Source File
# Begin Source File

SOURCE=.\SitePage.h
# End Source File
# Begin Source File

SOURCE=.\SiteSettings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StringDlg.h
# End Source File
# Begin Source File

SOURCE=.\strutils.h
# End Source File
# Begin Source File

SOURCE=.\TelnetConn.h
# End Source File
# Begin Source File

SOURCE=.\TermView.h
# End Source File
# Begin Source File

SOURCE=.\TriggerList.h
# End Source File
# Begin Source File

SOURCE=.\Ucs2Conv.h
# End Source File
# Begin Source File

SOURCE=.\WindowState.h
# End Source File
# Begin Source File

SOURCE=.\WinUtils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ansfile.ico
# End Source File
# Begin Source File

SOURCE=..\Resource\res\ansfile.ico
# End Source File
# Begin Source File

SOURCE=.\res\copy_cur.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\res\copy_cur.cur
# End Source File
# Begin Source File

SOURCE=.\res\drag_cur.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\res\drag_cur.cur
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=..\Resource\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=..\Resource\res\page_cur.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\Res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=..\Resource\res\topic_cur.cur
# End Source File
# End Group
# End Target
# End Project
