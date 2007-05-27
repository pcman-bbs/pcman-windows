' This is a tool used to automate release build
' Copyright (C) 2007 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>

Set fs = CreateObject("Scripting.FileSystemObject")

'Delete old files
if fs.FileExists(".\Release\PCMan.exe") then
   fs.DeleteFile( ".\Release\PCMan.exe" )
end if
if fs.FileExists(".\Release\PCManCB.exe") then
   fs.DeleteFile( ".\Release\PCManCB.exe" )
end if
if fs.FileExists(".\Release\PCMan.zip") then
   fs.DeleteFile( ".\Release\PCMan.zip" )
end if
if fs.FileExists(".\Release\PCManCB.zip") then
   fs.DeleteFile( ".\Release\PCManCB.zip" )
end if
if fs.FileExists(".\Release\PCMan.7z") then
   fs.DeleteFile( ".\Release\PCMan.7z" )
end if
if fs.FileExists(".\Release\PCManCB.7z") then
   fs.DeleteFile( ".\Release\PCManCB.7z" )
end if

'Try to include non-free icons from PCMan 2004
dim use_nonfree_icon
use_nonfree_icon = False

if fs.FileExists(".\NonFree\Combo\Toolbar.bmp") and fs.FileExists(".\NonFree\Lite\Toolbar.bmp") then
'  MsgBox "Use non-free icons"
   fs.CopyFile ".\Combo\Release\PCMan Combo\Config\Toolbar.bmp", "NonFree\Combo\Toolbar.bak", True
   fs.CopyFile ".\Combo\Release\PCMan Combo\Config\Icons.bmp", "NonFree\Combo\Icons.bak", True
   fs.CopyFile ".\Combo\Release\PCMan Combo\Config\Webbar.bmp", "NonFree\Combo\Webbar.bak", True
   fs.CopyFile "NonFree\Combo\Toolbar.bmp", ".\Combo\Release\PCMan Combo\Config\Toolbar.bmp", True
   fs.CopyFile "NonFree\Combo\Icons.bmp", ".\Combo\Release\PCMan Combo\Config\Icons.bmp", True
   fs.CopyFile "NonFree\Combo\Webbar.bmp", ".\Combo\Release\PCMan Combo\Config\Webbar.bmp", True

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
'Build installer of Combo with NSIS
sh.Run (nsis+"/D_COMBO_ .\Installer.nsi"), 1, True

'Enable portable mode
fs.MoveFile ".\Lite\Release\PCMan\_Portable", ".\Lite\Release\PCMan\Portable"
fs.MoveFile ".\Combo\Release\PCMan Combo\_Portable", ".\Combo\Release\PCMan Combo\Portable"

'Find 7-zip
seven_zip=sh.RegRead("HKLM\Software\7-Zip\Path")
seven_zip="""" + seven_zip + "\7z.exe"""

'Build zip version
sh.Run (seven_zip + " a -tzip -mx=9 -mpass=15 -xr!.svn .\Release\PCMan.zip .\Lite\Release\PCMan"), 1, True
sh.Run (seven_zip + " a -tzip -mx=9 -mpass=15 -xr!.svn .\Release\PCManCB.zip "".\Combo\Release\PCMan Combo"""), 1, True
sh.Run (seven_zip + " a -tzip -mx=9 -mpass=15 .\Release\Migrate.zip "".\Migrate\Release\Migrate.exe"""), 1, True

'Build 7-zip version
sh.Run (seven_zip + " a -t7z -mx=5 -ms=on -xr!.svn .\Release\PCMan.7z .\Lite\Release\PCMan"), 1, True
sh.Run (seven_zip + " a -t7z -mx=5 -ms=on -xr!.svn .\Release\PCManCB.7z "".\Combo\Release\PCMan Combo"""), 1, True
sh.Run (seven_zip + " a -t7z -mx=5 -ms=on -xr!.svn .\Release\Migrate.7z "".\Migrate\Release\Migrate.exe"""), 1, True

'Disable portable mode
fs.MoveFile ".\Lite\Release\PCMan\Portable", ".\Lite\Release\PCMan\_Portable"
fs.MoveFile ".\Combo\Release\PCMan Combo\Portable", ".\Combo\Release\PCMan Combo\_Portable"

'Restore GPL'd icons
if use_nonfree_icon = True then
   fs.CopyFile "NonFree\Combo\Toolbar.bak", ".\Combo\Release\PCMan Combo\Config\Toolbar.bmp", True
   fs.CopyFile "NonFree\Combo\Icons.bak", ".\Combo\Release\PCMan Combo\Config\Icons.bmp", True
   fs.CopyFile "NonFree\Combo\Webbar.bak", ".\Combo\Release\PCMan Combo\Config\Webbar.bmp", True

   fs.CopyFile "NonFree\Lite\Toolbar.bak", ".\Lite\Release\PCMan\Config\Toolbar.bmp", True
   fs.CopyFile "NonFree\Lite\Icons.bak", ".\Lite\Release\PCMan\Config\Icons.bmp", True
end if
