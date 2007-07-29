// CSimpXmlParser: An extremely simplified xml parser.
// SimpXmlParser.h: interface for the CSimpXmlParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPXMLPARSER_H__8FCAD776_C29D_4BA6_BF49_0DB8FD38CF4A__INCLUDED_)
#define AFX_SIMPXMLPARSER_H__8FCAD776_C29D_4BA6_BF49_0DB8FD38CF4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSimpXmlParser
{
protected:
class CSimpXmlParserStack : public CPtrArray
	{
	public:
		CSimpXmlParserStack()
		{
			Add(NULL);
		}
		void Push(const char* element)
		{
			Add((void*)element);
		}
		const char* Top()
		{
			return (char*)GetAt(GetSize() - 1);
		}
		const char* Pop()
		{
			int index = GetSize() - 1;
			const char* element = (const char*)GetAt(index);
			RemoveAt(index);
			return element;
		}
	};

	enum XmlParsingState
	{
		XmlParseTagName,
		XmlParseAttrList,
		XmlParseData
	};

	bool is_utf8;
public:
	virtual void ElementData(const char* name, const char* data);
	virtual void EndElement(const char* name);
	virtual void BeginElement(const char* name, const char** attribs, const char **values);
	static int GetAttrInt(const char *key, const char **attribs, const char **values, int def = 0);
	static const char* GetAttrText(const char *key, const char **attribs, const char **values);
	CSimpXmlParser();
	virtual ~CSimpXmlParser();
	bool ParseXml(char *buf);
	inline bool IsUTF8() { return is_utf8; }
};

#endif // !defined(AFX_SIMPXMLPARSER_H__8FCAD776_C29D_4BA6_BF49_0DB8FD38CF4A__INCLUDED_)
