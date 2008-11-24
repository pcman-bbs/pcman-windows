#include "stdafx.h"

#include "SearchPlugin.h"
#include "StrUtils.h"
#include "SimpXmlParser.h"
#include "OleImage.h"
#include "TermView.h"

#include <wininet.h>

//by BBcall
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define AGENT_NAME  "CodeguruBrowser1.0"
//by BBcall


CSearchPluginCollection SearchPluginCollection;
CSearchPluginCollection SearchPluginCollection_2;

CString TestString;

CSearchPlugin::CSearchPlugin()
{
	ShortName = Description = InputEncoding = Method = NULL;
	Image = NULL;

	//by BBcall
	DWORD dwError;
    CString PageContent;

    // Initialize the Win32 Internet functions 
    m_Session = ::InternetOpen(AGENT_NAME, 
        INTERNET_OPEN_TYPE_PRECONFIG, // Use registry settings. 
        NULL, // Proxy name. NULL indicates use default.
        NULL, // List of local servers. NULL indicates default. 
        0) ;

    dwError = GetLastError();
	//by BBcall

}

CSearchPlugin::~CSearchPlugin()
{
	if (ShortName)
		free(ShortName);
	if (Description)
		free(Description);
	if (InputEncoding)
		free(InputEncoding);
	if (Image)
		DeleteObject(Image);
	if (Method)
		free(Method);

	//by BBcall
	// Closing the session
    ::InternetCloseHandle(m_Session);

}

// by BBcall //
CString CSearchPlugin::GetWebPage(const CString& Url)
{
    HINTERNET hHttpFile;
    char szSizeBuffer[32];
    DWORD dwLengthSizeBuffer = sizeof(szSizeBuffer); 
    DWORD dwFileSize;
    DWORD dwBytesRead;
    BOOL bSuccessful;
    CString Contents;

    // Setting default error message
    Contents = m_ErrorMessage;
    
    // Opening the Url and getting a Handle for HTTP file
    hHttpFile = InternetOpenUrl(m_Session, (const char *) Url, NULL, 0, 0, 0);

    if (hHttpFile)
    {    
        // Getting the size of HTTP Files
        BOOL bQuery = ::HttpQueryInfo(hHttpFile,HTTP_QUERY_CONTENT_LENGTH, szSizeBuffer, &dwLengthSizeBuffer, NULL) ;

        if(bQuery==TRUE)
        {    
            // Allocating the memory space for HTTP file contents
            dwFileSize=atol(szSizeBuffer);
            LPSTR szContents = Contents.GetBuffer(dwFileSize);

            // Read the HTTP file 
            BOOL bRead = ::InternetReadFile(hHttpFile, szContents, dwFileSize, &dwBytesRead); 
            
            if (bRead) 
                bSuccessful = TRUE;

            ::InternetCloseHandle(hHttpFile); // Close the connection.
        }

    }
    else
    {
        // Connection failed.
        bSuccessful = FALSE;
    } 
    return Contents;
}

void CSearchPlugin::SetErrorMessage(CString s)
{
    m_ErrorMessage = s;
}
// by BBcall //

int CSearchPluginCollection::Load(LPCTSTR filepath)
{
	CSearchPlugin *plugin = new CSearchPlugin();

	HANDLE file = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ,
							 NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		delete plugin;
		return -1;			// file not found
	}
	DWORD size = GetFileSize(file, NULL);
	char* buf = new char [size + 1];
	DWORD rlen = 0;
	ReadFile(file, buf, size * sizeof(char), &rlen, NULL);
	buf[rlen] = '\0';
	CloseHandle(file);

	if (! plugin->ParseXml(buf))
	{
		delete plugin;
		delete []buf;
		return -1;
	}
	delete []buf;

	plugins.Add(plugin);
	return plugins.GetSize() - 1;
}

CString CSearchPluginCollection::UrlForSearch(int index, CString searchTerm, bool utf8)
{
	CString url = GetField(index, CSearchPluginCollection::URL);
	char* utf8Str;
	CString encodedTerm = _T("");
	int utf8Len;
	CString tmp;
	char hex[5];

	if (stricmp(GetField(index, CSearchPluginCollection::INPUTENCODING), "utf-8") == 0)
	{
		if (utf8)
		{
			utf8Str = (char*)LPCTSTR(searchTerm);
			utf8Len = searchTerm.GetLength();
		}
		else
			utf8Str = MultiByteToUTF8(searchTerm, &utf8Len);
		//utf8Str = MultiByteToMultiByte( CP_ACP, CP_UTF8, searchTerm, &utf8Len );
		for (int i = 0; i < utf8Len; i++)
		{
			int ch = (unsigned char)utf8Str[i];
			if (isalnum(ch))      // 'A'-'Z', 'a'-'z', '0'-'9'
			{
				encodedTerm += (char)ch;
			}
			else if (ch == ' ')  					// space
			{
				encodedTerm += '+';
			}
			else if (strchr("-_.!~*\\()", ch))  		// unreserved
			{
				encodedTerm += (char)ch;
			}
			else if (ch <= 127)  				// other ASCII
			{
				encodedTerm += CharToHex(ch, hex);
			}
			else if ((ch &0xE0) == 0xC0)  			// non-ASCII <= 0x7FF
			{
				encodedTerm += CharToHex(ch, hex);
				encodedTerm += CharToHex(ch + 1, hex);
				ch++;
			}
			else if ((ch&0xF0) == 0xE0)  			// 0x7FF < ch <= 0xFFFF
			{
				encodedTerm += CharToHex(ch, hex);
				encodedTerm += CharToHex((unsigned char)utf8Str[i+1], hex);
				encodedTerm += CharToHex((unsigned char)utf8Str[i+2], hex);
				i += 2;
			}
			else if ((ch&0xF8) == 0xF0)
			{
				encodedTerm += CharToHex(ch, hex);
				encodedTerm += CharToHex((unsigned char)utf8Str[i+1], hex);
				encodedTerm += CharToHex((unsigned char)utf8Str[i+2], hex);
				encodedTerm += CharToHex((unsigned char)utf8Str[i+3], hex);
				i += 3 ;
			}
			else
				encodedTerm += CharToHex(ch, hex);
		}
		if (! utf8)
			delete[] utf8Str;
	}
	else
	{
		if (utf8)
			utf8Str = UTF8ToMultiByte(searchTerm, &utf8Len);
		else
		{
			utf8Len = searchTerm.GetLength();
			utf8Str = (char*)(LPCTSTR)searchTerm;
		}

		for (int i = 0; i < utf8Len; i++)
		{
			int ch = (unsigned char)utf8Str[i];
			if (isalnum(ch))      // 'A'-'Z', 'a'-'z', '0'-'9'{
			{
				encodedTerm += (char)ch;
			}
			else if (ch == ' ')  					// space
			{
				encodedTerm += '+';
			}
			else if (strchr("-_.!~*\\()", ch))  		// unreserved
			{
				encodedTerm += (char)ch;
			}
			else
				encodedTerm += CharToHex(ch, hex);
		}

		if (utf8)
			delete []utf8Str;
	}
	url.Replace("{searchTerms}", encodedTerm);
	TestString = encodedTerm;

	return url;
}


int CSearchPluginCollection::GetCount()
{
	return plugins.GetSize();
}

char* CSearchPluginCollection::GetField(int index, EField f)
{
	CString value;
	if (index < 0 || index >= plugins.GetSize())
		return NULL;

	switch (f)
	{
	case SHORTNAME:
		return ((CSearchPlugin*)plugins[index])->ShortName;
		break;
	case DESCRIPTION:
		return ((CSearchPlugin*)plugins[index])->Description;
		break;
	case INPUTENCODING:
		return ((CSearchPlugin*)plugins[index])->InputEncoding;
		break;
	case URL:
		return (char*)(LPCTSTR)((CSearchPlugin*)plugins[index])->Url;
		break;
	case METHOD:
		return ((CSearchPlugin*)plugins[index])->Method;
		break;
	}
	return _T("");
}


CSearchPluginParser::CSearchPluginParser(CSearchPlugin& _plugin) : plugin(_plugin), url(NULL)
{
}

void CSearchPluginParser::BeginElement(const char* name, const char** attribs, const char **values)
{
	if (0 == strncmp(name, "os:", 3))
		name += 3;
	if (url)
	{
		if (0 == strcmp(name, "Param"))
		{
			const char* name = GetAttrText("name", attribs, values);
			const char* value = GetAttrText("value", attribs, values);
			if (name && value)
			{
				if (plugin.Url.Find('?') == -1)
					plugin.Url += '?';
				plugin.Url += name;
				plugin.Url += '=';
				plugin.Url += value;
				plugin.Url += '&';
			}
		}
	}
	else if (0 == strcmp(name, "Url"))
	{
		const char* type = GetAttrText("type", attribs, values);
		if (type && 0 == strcmp("text/html", type))	// only text/html is supported
		{
			const char* method = GetAttrText("method", attribs, values);
			if (method && 0 == strcmpi(method, "get"))    // only GET is supported
			{
				const char* templ = GetAttrText("template", attribs, values);
				if (templ)
				{
					url = name;
					plugin.Url = templ;
					plugin.Method = strdup("method");
				}
			}
		}
	}
	else if (0 == strcmp(name, "Image"))
	{
		int width = GetAttrInt("width", attribs, values);
		int height = GetAttrInt("height", attribs, values);
		if (width == 0 || height == 0)	// error!
			return;
		// plugin.Image = new CBitmap();
	}
}

void CSearchPluginParser::ElementData(const char* name, const char* data)
{
	if (0 == strncmp(name, "os:", 3))
		name += 3;
	if (!plugin.ShortName && 0 == strcmp(name, "ShortName"))
	{
		if (is_utf8)
			plugin.ShortName = UTF8ToMultiByte(data, NULL);
		else
			plugin.ShortName = strdup(data);
	}
	if (!plugin.InputEncoding && 0 == strcmp(name, "InputEncoding"))
		plugin.InputEncoding = strdup(data);
	if (!plugin.Description && 0 == strcmp(name, "Description"))
	{
		if (is_utf8)
			plugin.Description = UTF8ToMultiByte(data, NULL);
		else
			plugin.Description = strdup(data);
	}
	//about the image in the xml files
	if (!plugin.Image && 0 == strcmp(name, "Image"))
	{
		if (0 == strncmp(data, "data:image/", 11))
		{
			data += 11;
			// Currently, only these kinds of format is supported
			if (0 == strncmp(data, "x-icon;base64,", 14))
				data += 14;
			else if (0 == strncmp(data, "gif;base64,", 11))
				data += 11;
			else
				return;

			int ilen = strlen(data);
			int olen = ::Base64Decode((BYTE*)data, ilen, NULL, 0);
			if (olen > 0)
			{
				BYTE* obuf = new BYTE[olen];
				if (::Base64Decode((BYTE*)data, ilen, obuf, olen))
				{
					COleImage img;
					if (img.LoadFromMem(obuf, olen))
					{
						plugin.Image = img.CopyBitmap();
					}
				}
				delete []obuf;
			}
		}
	}
}

void CSearchPluginParser::EndElement(const char* name)
{
	if (name == url)
		url = NULL;
}


bool CSearchPlugin::ParseXml(char *buf)
{
	return CSearchPluginParser(*this).ParseXml(buf);
}

int __cdecl _ComparePlugin(const void* v1, const void* v2)
{
	CSearchPlugin* p1 = *(CSearchPlugin**)v1;
	CSearchPlugin* p2 = *(CSearchPlugin**)v2;
	return _mbsicmp((BYTE*)p1->ShortName, (BYTE*)p2->ShortName);
}

void CSearchPluginCollection::LoadAll()
{
	extern CString AppPath;

	if (plugins.GetSize() > 0)
		return;

	COleImage::Initialize();	// Load OLE for GIF/JPEG loading

	CFileFind searchFind;
	BOOL searchFound;
	int pluginId;
	searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\*.xml");
	while (searchFound)
	{
		searchFound = searchFind.FindNextFile();
		pluginId = SearchPluginCollection.Load(searchFind.GetFilePath());
	}

	COleImage::Finalize();	// Release OLE for GIF/JPEG loading

	plugins.FreeExtra();


	// Brute force!! Sort "internal data" of CPtrArray. Anyway, it works!
	// FIXME: Current sorting result is not very good.
	void** pdata = plugins.GetData();
	if (pdata)
	{
		//qsort(pdata, plugins.GetSize(), sizeof(void*), _ComparePlugin);
	}
}


void CSearchPluginCollection::LoadAll(int bbcall)
{
	int PagCounter = 0;
	extern CString AppPath;

	if (plugins.GetSize() > 0)
		return;

	COleImage::Initialize();	// Load OLE for GIF/JPEG loading

	CFileFind searchFind;
	BOOL searchFound;
	int pluginId;
	while (bbcall == 1){
		switch (PagCounter){
			case 0:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\nciku0.xml");				
				break;
			case 1:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\nciku.xml");
				break;
			case 2:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\yahoo-dict.xml");
				break;
			case 3:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\google0.xml");
				break;
			case 4:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\google.xml");
				break;
			case 5:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\yahoo-zh-TW.xml");
				break;
			case 6:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\wikipedia-zh.xml");
				break;
			case 7:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\youtube.xml");
				break;
			case 8:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\nciku.xml");
				break;
			case 9:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\0rz.xml");
				break;
			case 10:
				searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\tinyurl.xml");
				break;
			default:
				bbcall = -1;
		}

		while (searchFound){
			searchFound = searchFind.FindNextFile();
			pluginId = SearchPluginCollection.Load(searchFind.GetFilePath());
		}

		PagCounter++;
		/*searchFound = searchFind.FindFile(AppPath + "\\searchplugins\\*.xml");
		while (searchFound)
		{
			searchFound = searchFind.FindNextFile();
			pluginId = SearchPluginCollection.Load(searchFind.GetFilePath());
		}*/
	}
	if (bbcall == 2){
		searchFound = searchFind.FindFile(AppPath + "\\searchengines\\*.xml");
		while (searchFound)
		{
			searchFound = searchFind.FindNextFile();
			pluginId = SearchPluginCollection_2.Load(searchFind.GetFilePath());
		}
	}



	COleImage::Finalize();	// Release OLE for GIF/JPEG loading

	plugins.FreeExtra();


	// Brute force!! Sort "internal data" of CPtrArray. Anyway, it works!
	// FIXME: Current sorting result is not very good.
	/*void** pdata = plugins.GetData();
	if (pdata)
	{
		qsort(pdata, plugins.GetSize(), sizeof(void*), _ComparePlugin);
	}*/
}


HMENU CSearchPluginCollection::CreateSearchMenu()
{
	MENUITEMINFO search_menuiteminfo = { sizeof(MENUITEMINFO) };
	HMENU search_menu = NULL;

#if ! defined(_COMBO_)
	// Lite version call this function before showing popup menu to reduce startup time
	SearchPluginCollection.LoadAll(1);
#endif

	if (SearchPluginCollection.GetCount() > 0)
	{
		int i = 0;
		search_menu = CreatePopupMenu();

		for (i = 0; i < SearchPluginCollection.GetCount(); i++)
		{
			if (i == 0){
				InsertMenu(search_menu, i, TRUE, ID_SEARCHPLUGIN00 + i, "--查詢線上翻譯--");
				continue;
			}

			if (i == 3){
				InsertMenu(search_menu, i, TRUE, ID_SEARCHPLUGIN00 + i, "--查詢搜尋引擎--");
				continue;
			}

			if (i == 8){
				InsertMenu(search_menu, i, TRUE, ID_SEARCHPLUGIN00 + i, "--查詢其他網頁--");
				continue;
			}
			
			HBITMAP image = SearchPluginCollection.GetImage(i);
			CString text = "  ";
			text += SearchPluginCollection.GetField(i, CSearchPluginCollection::SHORTNAME);
			InsertMenu(search_menu, i, MF_BYPOSITION | MF_STRING, ID_SEARCHPLUGIN00 + i, text);
			if (image)
			{
				search_menuiteminfo.fMask =  MIIM_BITMAP;
				search_menuiteminfo.hbmpItem = image;
				SetMenuItemInfo(search_menu, i, TRUE, &search_menuiteminfo);
			}
		} 
	}

	return search_menu;
}

//By BBcall
HMENU CSearchPluginCollection::CreateSearchMenu_2(CString TextContent)
{
	MENUITEMINFO search_menuiteminfo_2 = { sizeof(MENUITEMINFO) };
	HMENU search_menu = NULL;

#if ! defined(_COMBO_)
	// Lite version call this function before showing popup menu to reduce startup time
	SearchPluginCollection_2.LoadAll(2);
#endif

	CSearchPlugin *plugin_2 = new CSearchPlugin();
	CString PageContent;
	
	if (SearchPluginCollection.GetCount() > 0)
	{
		int i = 0;
		search_menu = CreatePopupMenu();

		InsertMenu(search_menu, i, TRUE, ID_SEARCHPLUGIN00 + i, "N詞酷即時翻譯");
		InsertMenu(search_menu, i, TRUE, ID_SEARCHPLUGIN00 + i, TextContent);

		CString URLCString = "http://www.google.com.tw";
		PageContent = plugin_2->GetWebPage(URLCString);

		//CString tmpString = PageContent[3];
		InsertMenu(search_menu, i, TRUE, ID_SEARCHPLUGIN00 + i, PageContent);
	}

	return search_menu;
}