set Version_H_PATH=Lite\Version.h
set Version_Set_PATH=Version_Set_Lite.bat
set Version_Create_PATH=VersionCTL\Version_Create.bat
set Update_File_PATH=Release\update_lite.txt
SET APP_SubWCRev=C:\Program Files\TortoiseSVN\bin\SubWCRev.exe
set Svn_WorkingCopyPath=Lite

set ShowDbgMsg=

if "%1" == "1" (
	pushd ..
)

if "%ShowDbgMsg%" == "1" (
	call %Version_Create_PATH%  %Version_Set_PATH% %Version_H_PATH%
)else (
	call %Version_Create_PATH%  %Version_Set_PATH% %Version_H_PATH% > nul
)
	


