set Version_H_PATH=Combo\Version.h
set Version_Set_PATH=Version_Set_Combo.bat
set Version_Create_PATH=VersionCTL\Version_Create.bat

SET APP_SubWCRev=C:\Program Files\TortoiseSVN\bin\SubWCRev.exe
set Svn_WorkingCopyPath=.




REM call %Version_Create_PATH%  %Version_Set_PATH% %Version_H_PATH%
REM pause

call %Version_Create_PATH%  %Version_Set_PATH% %Version_H_PATH% > nul
pause > nul



