$ProgressPreference = 'SilentlyContinue'  # For Invoke-WebRequest
[Net.ServicePointManager]::SecurityProtocol = 'Tls, Tls11, Tls12'  # For Invoke-WebRequest
Invoke-WebRequest -Uri 'https://github.com/pcman-bbs/pcman-windows/releases/download/9.4.2/NonFree.zip' -Out 'NonFree.zip'
Expand-Archive 'NonFree.zip' -DestinationPath '.'
Copy-Item 'NonFree\Combo\*' 'Combo\Debug\PCMan Combo\Config\'
Copy-Item 'NonFree\Combo\*' 'Combo\Release\PCMan Combo\Config\'
Copy-Item 'NonFree\Lite\*' 'Lite\Debug\PCMan\Config\'
Copy-Item 'NonFree\Lite\*' 'Lite\Release\PCMan\Config\'
