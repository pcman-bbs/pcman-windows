pushd Lite\Release
& 7z a -tzip PCMan.zip PCMan
popd
pushd Combo\Release
& 7z a -tzip PCManCB.zip 'PCMan Combo'
popd

md Release
& 'C:\Program Files (x86)\NSIS\makensis.exe' .\Installer.nsi
& 'C:\Program Files (x86)\NSIS\makensis.exe' /D_COMBO_ .\Installer.nsi
