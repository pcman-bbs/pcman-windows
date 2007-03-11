#ifndef __SEARCHPLUGIN__
#define __SEARCHPLUGIN__

#include <afxtempl.h>

#if _MFC_VER >= 0x0700	// MFC 7.0 and 4.2 are not compatible
#include <atlenc.h>
#include <atlstr.h>
#endif

class CSearchPlugin {
public:
	bool ParseXml( char* buf );
	CSearchPlugin();
	~CSearchPlugin();
	char* ShortName;
	char* Description;
	char* InputEncoding;
	char* Image;
	int ImageBytes;
	CString Url;
	// CString Suggestions;
	char* Method;
};

class CSearchPluginCollection {
private:

#if _MFC_VER >= 0x0700	// MFC 7.0 and 4.2 are not compatible
	CList<CSearchPlugin*> plugins;
#else
	CList<CSearchPlugin*, CSearchPlugin*&> plugins;
#endif
	// static char hextable[][4];
public:
	enum EField { SHORTNAME, DESCRIPTION, INPUTENCODING, IMAGE, URL, METHOD, IMAGEBYTES };
	int Load(LPCTSTR path);
	int GetCount();
	CString UrlForSearch(int index, CString searchTerm);
	char* GetField(int index, EField f);

    CSearchPluginCollection(){}
    ~CSearchPluginCollection(){
        for( POSITION pos = plugins.GetHeadPosition(); pos; plugins.GetNext(pos) ) {
            CSearchPlugin* plugin = plugins.GetAt(pos);
            if( plugin )
                delete plugin;
        }
    }
};


extern CSearchPluginCollection SearchPluginCollection;

#endif
