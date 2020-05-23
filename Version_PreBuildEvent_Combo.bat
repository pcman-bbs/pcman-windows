set Version_H_PATH=Combo\Version.h
set Version_Set_PATH=Version_Set_Combo.bat
set Version_Create_PATH=VersionCTL\Version_Create.bat
set Update_File_PATH=Release\update_combo.txt

if "%1" == "1" (
	pushd ..
)

call %Version_Create_PATH%  %Version_Set_PATH% %Version_H_PATH%
