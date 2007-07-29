// SimpXmlParser.cpp: implementation of the CSimpXmlParser class.
//
//////////////////////////////////////////////////////////////////////

#include <afx.h>
#include <afxcoll.h>
#include "SimpXmlParser.h"
#include "..\Lite\StrUtils.h"	// FIXME: This is really bad

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpXmlParser::CSimpXmlParser() : is_utf8(true)	// By default we use utf-8
{

}

CSimpXmlParser::~CSimpXmlParser()
{

}

bool CSimpXmlParser::ParseXml(char *buf)
{
	// Check BOM
	if (strnicmp(buf, "\xEF\xBB\xBF", 3) == 0)
	{
		is_utf8 = true;
		buf += 3;
	}
	SKIP_SPACES_AND_CRLF(buf);
	if (0 == strncmp(buf, "<?xml", 5))	// has standard declaration
	{
		char* tmp = buf + 5;
		char* end = strstr(tmp, "?>");
		if (!end)	// incorrect xml
			return false;
		// skip declaration
		buf = end + 2;
		*(end + 1) = '\0';

		// encoding detection
		if (tmp = strstr(tmp, "encoding"))	// encoding
		{
			tmp += 8;
			while (*tmp && *tmp != '\"')
				++tmp;
			if (*tmp != '\"')	// value without ""
				return false;
			is_utf8 = (0 == strnicmp(tmp, "utf-8", 5));
		}
	}

	// start parsing!
	XmlParsingState state = XmlParseData;
	const char* current = NULL;
	CSimpXmlParserStack stack;
	stack.SetSize(10);

	while (*buf)
	{
		switch (state)
		{
		case XmlParseTagName:
			{
				bool end_tag = false;
				SKIP_SPACES_AND_CRLF(buf);
				if (0 == strncmp(buf, "!--", 3))    // comment
				{
					char* eoc = strstr(buf, "-->");
					if (!eoc)   // error: <!-- without -->
						return false;
					buf = eoc + 1;
					state = XmlParseData;
					continue;
				}

				if (*buf == '/')	// </XXXX> end tag
				{
					++buf;
					end_tag = true;
				}
				current = buf;
				for (;*buf && *buf != '>'; ++buf)
				{
					if (*buf == ' ' || *buf == '\t' || *buf == '/')
						break;
				}
				if (! *buf)	// error: incomplete tag
					return false;
				char* eot = buf;
				SKIP_SPACES_AND_CRLF(buf);
				*eot = '\0';
				if (! end_tag)
					stack.Push(current);
				if ('\0' == *buf && !end_tag)
				{
					const char *attrib = NULL, *value = NULL;
					BeginElement(current, &attrib, &value);
					state = XmlParseData;
					++buf;
				}
				else if (*buf == '/' || end_tag)	// </XXX> or <XXX /> end of element
				{
					const char* tag = stack.Pop();
					if (strcmp(tag, current))	// error: open/close tag mismatch
						return false;
					EndElement(current);
					current = stack.Top();
					state = XmlParseData;
					++buf;
				}
				else
					state = XmlParseAttrList;
				break;
			}
		case XmlParseAttrList:
			{
				CPtrArray attribs, values;
				for (;*buf && *buf != '>'; ++buf)
				{
					SKIP_SPACES_AND_CRLF(buf);
					if (*buf == '/')  // end of element
						break;

					char* attrib = buf;
					while (*buf && *buf != '=')
						++buf;
					if (*buf != '=')	// error: attribute without value
						return false;
					*buf = '\0';
					++buf;
					SKIP_SPACES_AND_CRLF(buf);
					if (*buf != '\"')	// error: value without ""
						return false;
					++buf;
					const char* value = buf;
					while (*buf && *buf != '\"')
						++buf;

					if (*buf != '\"')	// error: incomplete value
						return false;
					*buf = '\0';
					attribs.Add((void*)attrib);
					values.Add((void*)value);
				}
				attribs.Add(NULL);
				values.Add(NULL);
				if (! *buf)	// error: incomplete tag
					return false;
				BeginElement(current,
							 (const char**)attribs.GetData(),
							 (const char**)values.GetData());
				if (*buf == '/')
				{
					EndElement(current);
					stack.Pop();
					current = stack.Top();
				}
				state = XmlParseData;
				++buf;
			}
			break;
		case XmlParseData:
			if (*buf == '<')
			{
				current = NULL;
				state = XmlParseTagName;
				++buf;
			}
			else if (current)
			{
				CString data;
				for (; *buf != '<'; ++buf)
				{
					if ('\0' == *buf)  // error: lack of end tag
						return false;
					if (*buf == '&')	// encoded symbols
					{
						if (0 == strncmp(buf, "amp;", 4))
						{
							data += '&';
							buf += 3;
						}
						else if (0 == strncmp(buf, "lt;", 3))
						{
							data += '<';
							buf += 2;
						}
						else if (0 == strncmp(buf, "gt;", 3))
						{
							data += '>';
							buf += 2;
						}
						else
							data += '&';
					}
					else
						data += *buf;
				}
				ElementData(current, LPCTSTR(data));
			}
			else
				++buf;
			break;
		}
	}
	return true;
}


void CSimpXmlParser::BeginElement(const char* name, const char** attribs, const char **values)
{

}

void CSimpXmlParser::EndElement(const char *name)
{

}

void CSimpXmlParser::ElementData(const char *name, const char *data)
{

}

const char* CSimpXmlParser::GetAttrText(const char *key, const char **attribs, const char **values)
{
	for (const char** attrib = attribs; *attrib; ++attrib)
		if (0 == strcmp(*attrib, key))
			return values[(attrib - attribs)];
	return NULL;
}

int CSimpXmlParser::GetAttrInt(const char *key, const char **attribs, const char **values, int def)
{
	const char *val = GetAttrText(key, attribs, values);
	return val ? atoi(val) : def;
}
