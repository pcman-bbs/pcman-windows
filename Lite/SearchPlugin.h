#ifndef __SEARCHPLUGIN__
#define __SEARCHPLUGIN__

#include <afxtempl.h>
#include <afxwin.h>

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
	HBITMAP Image;
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
	enum SearchMenuItemId {
		ID_SEARCHPLUGIN_MENU = 65535,
		ID_SEARCHPLUGIN31 = ID_SEARCHPLUGIN_MENU-1,
		ID_SEARCHPLUGIN30 = ID_SEARCHPLUGIN31-1,
		ID_SEARCHPLUGIN29 = ID_SEARCHPLUGIN30-1,
		ID_SEARCHPLUGIN28 = ID_SEARCHPLUGIN29-1,
		ID_SEARCHPLUGIN27 = ID_SEARCHPLUGIN28-1,
		ID_SEARCHPLUGIN26 = ID_SEARCHPLUGIN27-1,
		ID_SEARCHPLUGIN25 = ID_SEARCHPLUGIN26-1,
		ID_SEARCHPLUGIN24 = ID_SEARCHPLUGIN25-1,
		ID_SEARCHPLUGIN23 = ID_SEARCHPLUGIN24-1,
		ID_SEARCHPLUGIN22 = ID_SEARCHPLUGIN23-1,
		ID_SEARCHPLUGIN21 = ID_SEARCHPLUGIN22-1,
		ID_SEARCHPLUGIN20 = ID_SEARCHPLUGIN21-1,
		ID_SEARCHPLUGIN19 = ID_SEARCHPLUGIN20-1,
		ID_SEARCHPLUGIN18 = ID_SEARCHPLUGIN19-1,
		ID_SEARCHPLUGIN17 = ID_SEARCHPLUGIN18-1,
		ID_SEARCHPLUGIN16 = ID_SEARCHPLUGIN17-1,
		ID_SEARCHPLUGIN15 = ID_SEARCHPLUGIN16-1,
		ID_SEARCHPLUGIN14 = ID_SEARCHPLUGIN15-1,
		ID_SEARCHPLUGIN13 = ID_SEARCHPLUGIN14-1,
		ID_SEARCHPLUGIN12 = ID_SEARCHPLUGIN13-1,
		ID_SEARCHPLUGIN11 = ID_SEARCHPLUGIN12-1,
		ID_SEARCHPLUGIN10 = ID_SEARCHPLUGIN11-1,
		ID_SEARCHPLUGIN09 = ID_SEARCHPLUGIN10-1,
		ID_SEARCHPLUGIN08 = ID_SEARCHPLUGIN09-1,
		ID_SEARCHPLUGIN07 = ID_SEARCHPLUGIN08-1,
		ID_SEARCHPLUGIN06 = ID_SEARCHPLUGIN07-1,
		ID_SEARCHPLUGIN05 = ID_SEARCHPLUGIN06-1,
		ID_SEARCHPLUGIN04 = ID_SEARCHPLUGIN05-1,
		ID_SEARCHPLUGIN03 = ID_SEARCHPLUGIN04-1,
		ID_SEARCHPLUGIN02 = ID_SEARCHPLUGIN03-1,
		ID_SEARCHPLUGIN01 = ID_SEARCHPLUGIN02-1,
		ID_SEARCHPLUGIN00 = ID_SEARCHPLUGIN01-1,
	};

	HMENU CreateSearchMenu();
	void LoadAll();
	enum EField { SHORTNAME, DESCRIPTION, INPUTENCODING, IMAGE, URL, METHOD, IMAGEBYTES };
	int Load(LPCTSTR path);
	int GetCount();
	CString UrlForSearch(int index, CString searchTerm);
	char* GetField(int index, EField f);
	HBITMAP GetImage(int index) {
		return ((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->Image; 
	}

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
