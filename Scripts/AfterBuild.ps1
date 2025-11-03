pushd Lite\Release
& 7z a -tzip PCMan.zip PCMan
popd

& 'C:\Program Files (x86)\NSIS\makensis.exe' .\Installer.nsi
