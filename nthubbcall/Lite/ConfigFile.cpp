// Config.cpp: implementation of the CConfigFile class.
//
/////////////////////////////////////////////////////////////////////////////
// Name:        config.cpp
// Purpose:     Application configuration class, deal with configuration automatically
// Author:      PCMan (HZY)   http://pcman.ptt.cc/
// E-mail:      hzysoft@sina.com.tw
// Created:     2004.7.22
// Copyright:   (C) 2004 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
// Modified by:
/////////////////////////////////////////////////////////////////////////////


//	Copyright (C) 2004 PCMan
//	Author: PCMan (HZY) 2004/07/22	07:51 AM
//	I finally came up with a really smart way to maintain ini file.
//	Every time I add a variable to CAppConfig, all I need to do is
//	adding the variable in my "Config Table," and all the data will
//	be load and save automatically.  This is not the most efficient way.
//	In my first version I use some more efficient method, but at last I change
//	my code to what it is now.  Because I think in a program not time-critical,
//	easy-maintaining is much more important.

#include "ConfigFile.h"
#include <stdio.h>
#include <string.h>
#include "StrUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CConfigFile::CConfigFile() : cur_sect(NULL)
{
}

CConfigFile::~CConfigFile()
{
}

bool CConfigFile::DoLoad(ConfigTable table)
{
	HANDLE hfile = CreateFile(file_path, GENERIC_READ, 0,
							  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
		return false;
	DWORD size = GetFileSize(hfile, NULL), rsize = 0;
	if (size == 0)
	{
		CloseHandle(hfile);
		return false;
	}

	char* buf = new char[ size + 1 ];
	ReadFile(hfile, buf, size, &rsize, NULL);
	CloseHandle(hfile);
	buf[ size ] = '\0';

	cur_sect = NULL;
	char *next_line = NULL;
	for (char* line = buf; line; line = next_line)
	{
		next_line = strnextline(line);
		char* keyname = line;
		SKIP_SPACES(keyname);
		if (keyname[0] == '#')  // comment
			continue;
		if (keyname[0] == '[')  // This line doesn't contain any value.
		{
			++keyname;
			char* tail = strchr(keyname, ']');
			if (!tail)  // error: incorrect section
			{
				cur_sect = NULL;
				continue;
			}
			*tail = '\0';

			ConfigEntry* pent = table;
			for (; pent->name; pent++)
			{
				if (pent->finished)
					continue;
				if (0 == strcmp(keyname, pent->name))
				{
					if (pent->type == VT_CUSTOM_SECT)
					{
						if (next_line)
						{
							ConfigHandler* handler = (ConfigHandler*)pent->data;
							char* eos = next_line, tmp = 0;
							while (*eos && !(*eos == '[' && *(eos - 1) == '\n'))
								++eos;
							tmp = *eos;
							*eos = '\0';
							// pass the whole section to handler
							handler->Load(next_line);
							*eos = tmp;
							next_line = *eos ? (eos - 1) : NULL;
						}
					}
					else
					{
						if (cur_sect)   // mark previous section as finished
							cur_sect->finished = true;
						cur_sect = (ConfigEntry*)pent->data;
					}
					break;
				}
			}
			continue;
		}
		char* pstrval = strchr(keyname, '=');
		if (!pstrval)   // error: key name without value
			continue;
		*pstrval = '\0';    // replace '=' with '\0'

		char* tail = pstrval - 1;
		while (tail < keyname && (*tail == ' ' || *tail == '\t'))
			--tail; // strip spaces at tail of keyname
		tail[1] = '\0';

		if (! *keyname)     // error: keyname is empty
			continue;

		++pstrval;
		SKIP_SPACES(pstrval);   // skip spaces at the beginning of value

		ConfigEntry* pent = cur_sect;
		for (; pent->name; pent++)
		{
			if (pent->finished)     // skip finished entries
				continue;
			if (0 == strcmp(keyname, pent->name))
				break;
		}

		if (pent->name)     // matching entry found
		{
			switch (pent->type)
			{
			case VT_BYTE:
				*((unsigned char*)pent->data) = (unsigned char)atoi(pstrval);
				break;
			case VT_INT:
				*((int*)pent->data) = atoi(pstrval);
				break;
			case VT_STR:
				*((CString*)pent->data) = pstrval;
				break;
			case VT_ESTR:
				*((CString*)pent->data) = UnescapeControlChars(pstrval);
				break;
			case VT_COLOR:
				{
					int r, g, b;
					r = g = b = 0;
					if (3 == sscanf(pstrval, "%d,%d,%d", &r, &g, &b))
						*(COLORREF*)pent->data = RGB(r, g, b);
				}
				break;
			case VT_BOOL:
				*((bool*)pent->data) = !!atoi(pstrval);
				break;
			case VT_CUSTOM:
				{
					ConfigHandler* handler = (ConfigHandler*)pent->data;
					handler->Load(pstrval);
//                (*pent->func)( true, (void*)pstrval, pent->data );
				}
				break;
			case VT_LONG:
				*((long*)pent->data) = atol(pstrval);
				break;
			case VT_SHORT:
				*((short*)pent->data) = atoi(pstrval);
				break;
			}
			pent->finished = true;
		}
	}
	cur_sect = NULL;
	delete []buf;
	return true;
}

bool CConfigFile::DoSave(ConfigTable table)
{
	CFile file;
	if (file.Open(file_path, CFile::modeCreate | CFile::modeWrite))
	{
		CString str, esc_str;
		for (; table->name; table++)
		{
			str.Format("[%s]\r\n", table->name);
			file.Write(LPCTSTR(str), str.GetLength());

			if (table->type == VT_CUSTOM_SECT)
			{
				ConfigHandler* handler = (ConfigHandler*)table->data;
				esc_str.Empty();
				handler->Save(esc_str);
				file.Write((LPCTSTR)esc_str, esc_str.GetLength());
				file.Write("\r\n", 2);
				continue;
			}
			else
				cur_sect = (ConfigEntry*)table->data;

			char strval[64];	const char* pstrval;
			for (; cur_sect->name; cur_sect++)
			{
				pstrval = strval;
				switch (cur_sect->type)
				{
				case VT_BYTE:
					sprintf(strval, "%d", *((unsigned char*)cur_sect->data));
					break;
				case VT_BOOL:
					sprintf(strval, "%d", (int)*((bool*)cur_sect->data));
					break;
				case VT_INT:
					sprintf(strval, "%d", *((int*)cur_sect->data));
					break;
				case VT_STR:
					if (cur_sect->data)
						pstrval = *(CString*)cur_sect->data;
					else
						pstrval = "";
					break;
				case VT_ESTR:
					esc_str = EscapeControlChars((LPCTSTR) * (CString*)cur_sect->data);
					pstrval = esc_str;
					break;
				case VT_COLOR:
					{
						COLORREF clr = *(COLORREF*)cur_sect->data;
						sprintf(strval, "%d,%d,%d", GetRValue(clr), GetGValue(clr), GetBValue(clr));
					}
					break;
				case VT_LONG:
					sprintf(strval, "%ld", *((long*)cur_sect->data));
					break;
				case VT_SHORT:
					sprintf(strval, "%d", *((short*)cur_sect->data));
					break;
				case VT_CUSTOM:
					{
						esc_str.Empty();
//                    cur_sect->func( false, &esc_str, cur_sect->data );
						ConfigHandler* handler = (ConfigHandler*)cur_sect->data;
						handler->Save(esc_str);
						pstrval = esc_str;
					}
					break;
				}
				str.Format("%s=%s\r\n", cur_sect->name, pstrval);
				file.Write(LPCTSTR(str), str.GetLength());
			}
			file.Write("\r\n", 2);
		}
		file.Close();
	}
	else
	{
		cur_sect = NULL;
		return false;
	}
	cur_sect = NULL;
	return true;
}

bool CConfigFile::DoDataExchange(bool load, ConfigTable table)
{
	return load ? DoLoad(table) : DoSave(table);
}

