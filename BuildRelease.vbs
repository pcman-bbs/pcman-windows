' This is a tool used to automate release build
' Copyright (C) 2007 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>

Set fs = CreateObject("Scripting.FileSystemObject")

'Delete old files
if fs.FileExists(".\Release\PCMan.exe") then
   fs.DeleteFile( ".\Release\PCMan.exe" )
end if
if fs.FileExists(".\Release\PCMan.zip") then
   fs.DeleteFile( ".\Release\PCMan.zip" )
end if
if fs.FileExists(".\Release\PCMan.7z") then
   fs.DeleteFile( ".\Release\PCMan.7z" )
end if

'Try to include non-free icons from PCMan 2004
dim use_nonfree_icon
use_nonfree_icon = False

if fs.FileExists(".\NonFree\Lite\Toolbar.bmp") then
'  MsgBox "Use non-free icons"
   fs.CopyFile ".\Lite\Release\PCMan\Config\Toolbar.bmp", "NonFree\Lite\Toolbar.bak", True
   fs.CopyFile ".\Lite\Release\PCMan\Config\Icons.bmp", "NonFree\Lite\Icons.bak", True
   fs.CopyFile "NonFree\Lite\Toolbar.bmp", ".\Lite\Release\PCMan\Config\Toolbar.bmp", True
   fs.CopyFile "NonFree\Lite\Icons.bmp", ".\Lite\Release\PCMan\Config\Icons.bmp", True

   use_nonfree_icon = True
'else
'    MsgBox "use standard icons"
end if

'Build installer with NSIS
Set sh = WScript.CreateObject("WScript.Shell")
'Find NSIS
nsis=sh.RegRead("HKLM\Software\NSIS\")
nsis="""" + nsis + "\makensis.exe"" "
'Build installer of Lite with NSIS
sh.Run (nsis+".\Installer.nsi"), 1, True

'Enable portable mode
fs.MoveFile ".\Lite\Release\PCMan\_Portable", ".\Lite\Release\PCMan\Portable"

'Find 7-zip
seven_zip=sh.RegRead("HKLM\Software\7-Zip\Path")
seven_zip="""" + seven_zip + "\7z.exe"""

'Build zip version
sh.Run (seven_zip + " a -tzip -mx=9 -mpass=15 -xr!.svn .\Release\PCMan.zip .\Lite\Release\PCMan"), 1, True
sh.Run (seven_zip + " a -tzip -mx=9 -mpass=15 .\Release\Migrate.zip "".\Migrate\Release\Migrate.exe"""), 1, True

'Build 7-zip version
sh.Run (seven_zip + " a -t7z -mx=5 -ms=on -xr!.svn .\Release\PCMan.7z .\Lite\Release\PCMan"), 1, True
sh.Run (seven_zip + " a -t7z -mx=5 -ms=on -xr!.svn .\Release\Migrate.7z "".\Migrate\Release\Migrate.exe"""), 1, True

'Disable portable mode
fs.MoveFile ".\Lite\Release\PCMan\Portable", ".\Lite\Release\PCMan\_Portable"

'Restore GPL'd icons
if use_nonfree_icon = True then
   fs.CopyFile "NonFree\Lite\Toolbar.bak", ".\Lite\Release\PCMan\Config\Toolbar.bmp", True
   fs.CopyFile "NonFree\Lite\Icons.bak", ".\Lite\Release\PCMan\Config\Icons.bmp", True
end if
