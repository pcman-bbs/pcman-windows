
#include "SearchPlugin.h"
#include "StrUtils.h"
#include "SimpXmlParser.h"

CSearchPluginCollection SearchPluginCollection;

CSearchPlugin::CSearchPlugin()
{
	ShortName = Description = InputEncoding = Image = Method = NULL;
	ImageBytes = 0;
}
CSearchPlugin::~CSearchPlugin()
{
	if(ShortName)
		free( ShortName );
	if(Description)
		free( Description );
	if(InputEncoding)
		free( InputEncoding );
	if(Image)
		free( Image );
	if(Method)
		free( Method );
}

int CSearchPluginCollection::Load(LPCTSTR filepath)
{
	CSearchPlugin *plugin = new CSearchPlugin();

	HANDLE file = CreateFile( filepath, GENERIC_READ, FILE_SHARE_READ, 
							NULL, OPEN_EXISTING, 0, NULL );
	if( file == INVALID_HANDLE_VALUE )
	{
		delete plugin;
		return -1;			// file not found
	}
	DWORD size = GetFileSize( file, NULL );
	char* buf = new char [size + 1];
	DWORD rlen = 0;
	ReadFile( file, buf, size * sizeof(char), &rlen, NULL );
	buf[rlen] = '\0';
	CloseHandle( file );

	if( ! plugin->ParseXml( buf ) )
	{
		delete plugin;
        delete []buf;
		return -1;
	}
    delete []buf;

	plugins.AddTail( plugin );
	return plugins.GetCount()-1;
}

CString CSearchPluginCollection::UrlForSearch(int index, CString searchTerm)
{
	CString url = GetField( index, CSearchPluginCollection::URL);
	char* utf8Str;
	CString encodedTerm = _T("");
	int utf8Len;
	CString tmp;
	char hex[5];

	if( stricmp(GetField( index, CSearchPluginCollection::INPUTENCODING), "utf-8")==0 )
	{
		utf8Str = MultiByteToUTF8( searchTerm, &utf8Len );
		//utf8Str = MultiByteToMultiByte( CP_ACP, CP_UTF8, searchTerm, &utf8Len );
		for (int i = 0; i < utf8Len-1; i++) {
			int ch = (unsigned char)utf8Str[i];
			if( isalnum( ch ) ) { // 'A'-'Z', 'a'-'z', '0'-'9'
				encodedTerm += (char)ch;
			} else if (ch == ' ') {					// space
				encodedTerm += '+';
			} else if (strchr( "-_.!~*\\()", ch)) {		// unreserved
				encodedTerm += (char)ch;
			} else if ( ch <= 127) {				// other ASCII
				encodedTerm += CharToHex( ch, hex );
			} else if ((ch &0xE0)==0xC0) {			// non-ASCII <= 0x7FF
				encodedTerm += CharToHex( ch, hex );
				encodedTerm += CharToHex( ch + 1, hex );
				ch++;
			} else if ((ch&0xF0)==0xE0) {			// 0x7FF < ch <= 0xFFFF
				encodedTerm += CharToHex( ch, hex );
				encodedTerm += CharToHex( (unsigned char)utf8Str[i+1], hex);
				encodedTerm += CharToHex( (unsigned char)utf8Str[i+2], hex);
				i += 2;
			}
			else if ( (ch&0xF8)==0xF0 ) {
				encodedTerm += CharToHex( ch, hex );
				encodedTerm += CharToHex( (unsigned char)utf8Str[i+1], hex);
				encodedTerm += CharToHex( (unsigned char)utf8Str[i+2], hex);
				encodedTerm += CharToHex( (unsigned char)utf8Str[i+3], hex);
				i +=3 ;
			}
			else
				encodedTerm += CharToHex( ch, hex );
		}
		delete[] utf8Str;
	}
	else
	{
		utf8Len = searchTerm.GetLength();
		utf8Str = (char*)(LPCTSTR)searchTerm;

		for (int i = 0; i < utf8Len; i++) {
			int ch = (unsigned char)utf8Str[i];
			if( isalnum( ch ) ) { // 'A'-'Z', 'a'-'z', '0'-'9'{
				encodedTerm += (char)ch;
			} else if (ch == ' ') {					// space
				encodedTerm += '+';
			} else if (strchr( "-_.!~*\\()", ch)) {		// unreserved
				encodedTerm += (char)ch;
			} else
				encodedTerm += CharToHex( ch, hex );
		}
	}
	url.Replace( "{searchTerms}", encodedTerm );
	return url;
}


int CSearchPluginCollection::GetCount()
{
	return plugins.GetCount();
}

char* CSearchPluginCollection::GetField(int index, EField f)
{
	CString value;
	switch( f )
	{
		case SHORTNAME:
			return ((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->ShortName;
			break;
		case DESCRIPTION:
			return ((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->Description;
			break;
		case INPUTENCODING:
			return ((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->InputEncoding;
			break;
		case IMAGE:
			return ((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->Image;
			break;
		case IMAGEBYTES:
			return (char*)((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->ImageBytes;
			break;
		case URL:
			return (char*)(LPCTSTR)((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->Url;
			break;
		case METHOD:
			return ((CSearchPlugin*)(plugins.GetAt( plugins.FindIndex(index))))->Method;
			break;
	}
	return _T("");
}

class CSearchPluginParser : public CSimpXmlParser
{
public:
	CSearchPluginParser(CSearchPlugin& _plugin) : plugin( _plugin ), url(NULL)
	{
	}

	void BeginElement( const char* name, const char** attribs, const char **values )
	{
		if( 0 == strncmp( name, "os:", 3 ) )
			name += 3;
		if( url )
		{
			if( 0 == strcmp( name, "Param" ) )
			{
				const char* name = GetAttrText( "name", attribs, values );
				const char* value = GetAttrText( "value", attribs, values );
				if( name && value )
				{
                    if( plugin.Url.Find('?') == -1 )
    					plugin.Url += '?';
					plugin.Url += name;
					plugin.Url += '=';
					plugin.Url += value;
					plugin.Url += '&';
				}
			}
		}
		else if( 0 == strcmp( name, "Url" ) )
		{
			const char* type = GetAttrText( "type", attribs, values );
			if( type && 0 == strcmp( "text/html", type ) )	// only text/html is supported
			{
				const char* method = GetAttrText( "method", attribs, values );
				if( method && 0 == strcmpi( method, "get" ) ) // only GET is supported
				{
					const char* templ = GetAttrText( "template", attribs, values );
					if( templ )
					{
						url = name;
						plugin.Url = templ;
						plugin.Method = strdup( "method" );
					}
				}
			}
		}
	}

	void ElementData( const char* name, const char* data )
	{
		if( 0 == strncmp( name, "os:", 3 ) )
			name += 3;
		if( !plugin.ShortName && 0 == strcmp(name, "ShortName") )
		{
			if( is_utf8 )
				plugin.ShortName = UTF8ToMultiByte(data, NULL);
			else
				plugin.ShortName = strdup( data );
		}
		if( !plugin.InputEncoding && 0 == strcmp(name, "InputEncoding") )
			plugin.InputEncoding = strdup( data );
		if( !plugin.Description && 0 == strcmp(name, "Description") )
		{
			if( is_utf8 )
				plugin.Description = UTF8ToMultiByte(data, NULL);
			else
				plugin.Description = strdup( data );
		}
	}

	void EndElement( const char* name )
	{
		if( name == url )
			url = NULL;
	}

protected:
	CSearchPlugin& plugin;
	const char* url;
};

bool CSearchPlugin::ParseXml(char *buf)
{
	return CSearchPluginParser(*this).ParseXml(buf);
}

