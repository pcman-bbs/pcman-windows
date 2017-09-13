set Version_H_PATH=Lite\Version.h
set Version_Set_PATH=Version_Set_Lite.bat
set Version_Create_PATH=VersionCTL\Version_Create.bat
set Update_File_PATH=Release\update_lite.txt

if "%1" == "1" (
	pushd ..
)

call %Version_Create_PATH%  %Version_Set_PATH% %Version_H_PATH%
