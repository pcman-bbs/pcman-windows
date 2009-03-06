為何用這個方法？
在程式中，常常想要用版本資訊所以常常建造一個h檔來include
而且又想要可以在檔案的resouce加入version資訊
(效果產生在exe檔按右鍵，看內容，裡面的版本資訊)
但是這個resouce的資訊很難修改，又常常忘記，所以就有這個方法出現
還有就是可以跟TortoiseSVN的版號做結合


目標:
自動產生版本資訊
而這個版本資訊會自動修改resource檔的version區段
還有產生version.h，這裡面包含程式所需要的版本資訊


方法:

一、
共用檔案:
Version_Create.bat  ==> 產生Version.h
Version_ReadMe.txt  ==> 本檔

私有檔案:
Version_Resource.rc2 ==> 被原始資源檔包含(若跟Version.h和resource的資料夾位置不一樣，則需要修改內容的Version.h路徑)
Version_PreBuildEvent.bat ==> 讓vs2005 vc6觸發或使用者自行觸發來產生版本建造，主要設定Version_Create.bat的路徑
Version_Set.bat  ==>  由使用者自已設定版號和產品資訊
Version.h  ==>  自動產生、更新，使用者不能自行修改

二、
在要加入版本資訊的resource檔，修改如下
============================================================
============================================================
============================================================
3 TEXTINCLUDE 
BEGIN
    "\r\n"
    "#include ""Version_Resource.rc2""  // non-Microsoft Visual C++ edited resources\r\n"
    "\0"
END


#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//

#include "Version_Resource.rc2"  // non-Microsoft Visual C++ edited resources

/////////////////////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED

============================================================
============================================================
============================================================
為何有上面這個東西？  
因為若是在原始的resource檔修改東西的話
那下次若用編輯修改東西存檔的話，我們所修改的東西就會消失
用上面這個方法，就可以避免

三、
1.若是vs2005的編輯器在設定的pre-build event的command line加入Version_PreBuildEvent.bat
2.若是vc6在要手動執行Version_PreBuildEvent.bat，
或是使用非標準的做法在設定的pre-link step加入Version_PreBuildEvent.bat

四、
修改Version_Set.bat內的各項資訊
若要讓svn的版號可以加入，則設定SET UseSvnRevVersion=1
注意:需要灌TortoiseSVN，還有現在的資料夾是由TortoiseSVN所checkout出來的

完工

五、
Version_PreBuildEvent.bat的參數解說

set Version_H_PATH=Version.h  ==> Version.h檔案的位址，有可能在別的目錄，如..\Version.h
set Version_Set_PATH=Version_Set.bat ==> Version_Set.bath檔案的位址，有可能在別的目錄，如..\Version_Set.bat
set Version_Create_PATH=Version_Create.bat ==> Version_Create.bat檔案的位址，有可能在別的目錄，如..\Version_Create.bat
SET APP_SubWCRev=d:\Program Files\TortoiseSVN\bin\SubWCRev.exe ==> SubWCRev.exe這個檔案的位址，需要安裝TortoiseSVN
set Svn_WorkingCopyPath=. ==> svn的WorkingCopyPath，用來取得這個目錄的revision，而 . 表示現在的目錄


Omar (ID:badpp)
下午 08:11 2007/6/26



