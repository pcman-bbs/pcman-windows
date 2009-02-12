# Microsoft Developer Studio Project File - Name="Combo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Combo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Combo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Combo.mak" CFG="Combo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Combo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Combo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Combo", WDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Combo - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\SimpXmlParser" /D "NDEBUG" /D "_COMBO_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wininet.lib wsock32.lib winmm.lib msimg32.lib version.lib /nologo /subsystem:windows /machine:I386 /out:"Release/PCMan Combo/PCMan.exe"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PreLink_Cmds=$(OutDir)\BuildMenu.exe $(OutDir)\PCMan Combo\Config	..\Version_PreBuildEvent_Combo.bat 1
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Combo - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SimpXmlParser" /D "_DEBUG" /D "_COMBO_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "USE_DEBUG_CONSOLE" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib wsock32.lib winmm.lib msimg32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/PCMan Combo/PCMan.exe" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PreLink_Cmds=$(OutDir)\BuildMenu.exe $(OutDir)\PCMan Combo\Config	..\Version_PreBuildEvent_Lite.bat 1
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Combo - Win32 Release"
# Name "Combo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Lite\AddressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdItem.cpp
# End Source File
# Begin Source File

SOURCE=.\AdList.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AES.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AnsiBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AppConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoComplete.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoReplyPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoUpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\BBSHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\BrowseDirDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ChiConv\ChiConv.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\ColorConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\Combo.rc
# End Source File
# Begin Source File

SOURCE=..\Lite\ConfigFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\Conn.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\ConnectPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\CtrlEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\CustomizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\CustomTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\CustomToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\custsite.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\Debug_Kit.cpp
# End Source File
# Begin Source File

SOURCE=..\DlgLayout\DlgLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\DragTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\FavMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\GeneralPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\Hotkey.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\HotkeyEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\HyperLinkPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\InputNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\InstantTranDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\KeyMap.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\KeyMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\ListDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\MemIniFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\MouseCTL.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\MouseGesture.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\OleImage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\OtherPage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\PasswdDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\PCMan.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\ReBarState.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\Rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\SearchPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\SetBkDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\SimpXmlParser\SimpXmlParser.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\sitelistctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\SitePage.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\SiteSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\StringDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\strutils.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\TelnetConn.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\TermView.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\TriggerList.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\Ucs2Conv.cpp
# End Source File
# Begin Source File

SOURCE=.\WebBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\WebBrowserCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\WebCfgPage.cpp
# End Source File
# Begin Source File

SOURCE=.\WebConn.cpp
# End Source File
# Begin Source File

SOURCE=.\WebPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\WindowState.cpp
# End Source File
# Begin Source File

SOURCE=..\Lite\WinUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Lite\AddressDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdItem.h
# End Source File
# Begin Source File

SOURCE=.\AdList.h
# End Source File
# Begin Source File

SOURCE=..\Lite\AnsiBar.h
# End Source File
# Begin Source File

SOURCE=..\Lite\AppConfig.h
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoComplete.h
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoReplyPage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=..\Lite\BBSHyperLink.h
# End Source File
# Begin Source File

SOURCE=..\Lite\BrowseDirDlg.h
# End Source File
# Begin Source File

SOURCE=..\ChiConv\ChiConv.h
# End Source File
# Begin Source File

SOURCE=..\Lite\Clipboard.h
# End Source File
# Begin Source File

SOURCE=..\Lite\ColorConfigDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\ConfigFile.h
# End Source File
# Begin Source File

SOURCE=..\Lite\Conn.h
# End Source File
# Begin Source File

SOURCE=..\Lite\ConnectPage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\CtrlEdit.h
# End Source File
# Begin Source File

SOURCE=..\Lite\CustomizeDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\CustomTabCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Lite\CustomToolBar.h
# End Source File
# Begin Source File

SOURCE=.\custsite.h
# End Source File
# Begin Source File

SOURCE=..\Lite\Debug_Kit.h
# End Source File
# Begin Source File

SOURCE=..\Lite\DragTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Lite\FavMenu.h
# End Source File
# Begin Source File

SOURCE=..\Lite\FileUtil.h
# End Source File
# Begin Source File

SOURCE=..\Lite\GeneralPage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\Hotkey.h
# End Source File
# Begin Source File

SOURCE=..\Lite\HotkeyEdit.h
# End Source File
# Begin Source File

SOURCE=..\Lite\HyperLinkPage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\InputNameDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\KeyMap.h
# End Source File
# Begin Source File

SOURCE=..\Lite\KeyMapDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\ListDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\Lite\MemIniFile.h
# End Source File
# Begin Source File

SOURCE=.\MENUMAPITEM.h
# End Source File
# Begin Source File

SOURCE=..\Lite\OleImage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\OtherPage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\PasswdDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\PCMan.h
# End Source File
# Begin Source File

SOURCE=..\Lite\ReBarState.h
# End Source File
# Begin Source File

SOURCE=..\Resource\resource.h
# End Source File
# Begin Source File

SOURCE=..\Lite\RFC854.H
# End Source File
# Begin Source File

SOURCE=..\Lite\Rijndael.h
# End Source File
# Begin Source File

SOURCE=.\SearchBar.h
# End Source File
# Begin Source File

SOURCE=..\Lite\SearchPlugin.h
# End Source File
# Begin Source File

SOURCE=..\Lite\SetBkDlg.h
# End Source File
# Begin Source File

SOURCE=..\SimpXmlParser\SimpXmlParser.h
# End Source File
# Begin Source File

SOURCE=..\Lite\sitelistctrl.h
# End Source File
# Begin Source File

SOURCE=..\Lite\SitePage.h
# End Source File
# Begin Source File

SOURCE=..\Lite\SiteSettings.h
# End Source File
# Begin Source File

SOURCE=..\Lite\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Lite\StringDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\strutils.h
# End Source File
# Begin Source File

SOURCE=..\Lite\TelnetConn.h
# End Source File
# Begin Source File

SOURCE=..\Lite\TermView.h
# End Source File
# Begin Source File

SOURCE=..\Lite\Ucs2Conv.h
# End Source File
# Begin Source File

SOURCE=.\WebBrowser.h
# End Source File
# Begin Source File

SOURCE=.\WebBrowserCtrl.h
# End Source File
# Begin Source File

SOURCE=.\webcfgpage.h
# End Source File
# Begin Source File

SOURCE=.\WebConn.h
# End Source File
# Begin Source File

SOURCE=.\WebPageDlg.h
# End Source File
# Begin Source File

SOURCE=..\Lite\WindowState.h
# End Source File
# Begin Source File

SOURCE=..\Lite\WinUtils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Resource\res\ansfile.ico
# End Source File
# Begin Source File

SOURCE=..\Resource\res\copy_cur.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\res\drag_cur.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\res\hand.cur
# End Source File
# Begin Source File

SOURCE=..\Resource\res\idr_main.ico
# End Source File
# End Group
# End Target
# End Project
