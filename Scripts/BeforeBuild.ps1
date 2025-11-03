Invoke-WebRequest -Uri 'https://go.microsoft.com/fwlink/?LinkId=746571' -Out 'vc_redist.x86.exe'

Copy-Item 'OpenSourceLicenses.txt' 'Lite\Debug\PCMan\'
Copy-Item 'OpenSourceLicenses.txt' 'Lite\Release\PCMan\'

md Release
& .\Version_PreBuildEvent_Lite.bat
