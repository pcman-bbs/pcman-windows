// HotkeyEdit.cpp : implementation file

#include "Hotkey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct VKT
{
	BYTE key;
	char* name;
};
static VKT vkt[] =
{
	{VK_BACK, "Backspace"},
	{VK_TAB, "Tab"},
	{VK_CLEAR, "Num Clear"},
	{VK_RETURN, "Enter"},
	{VK_SHIFT, "Shift"},
	{VK_CONTROL, "Ctrl"},
	{VK_MENU, "Alt"},
	{VK_PAUSE, "Pause"},
	{VK_CAPITAL, "Caps Lock"},
	{VK_ESCAPE, "Esc"},
	{VK_SPACE, "Space"},
//	這些是本來的 non extended，把IBM extended key獨立出來了
	{VK_INSERT, "Num Ins"},
	{VK_DELETE, "Num Del"},
	{VK_END, "Num End"},
	{VK_DOWN, "Num Down"},
	{VK_NEXT, "Num PageDown"},
	{VK_LEFT, "Num Left"},
	{VK_RIGHT, "Num Right"},
	{VK_HOME, "Num Home"},
	{VK_UP, "Num Up"},
	{VK_PRIOR, "Num PageUp"},
//----------------------------
	{VK_SNAPSHOT, "Print Screen"},
	{VK_NUMPAD0, "Num 0"},
	{VK_NUMPAD1, "Num 1"},
	{VK_NUMPAD2, "Num 2"},
	{VK_NUMPAD3, "Num 3"},
	{VK_NUMPAD4, "Num 4"},
	{VK_NUMPAD5, "Num 5"},
	{VK_NUMPAD6, "Num 6"},
	{VK_NUMPAD7, "Num 7"},
	{VK_NUMPAD8, "Num 8"},
	{VK_NUMPAD9, "Num 9"},
	{VK_DECIMAL, "Num ."},
	{VK_MULTIPLY, "*"},
	{VK_ADD, "+"},
	{VK_SUBTRACT, "-"},
	{VK_DIVIDE, "/"},
	{VK_F1, "F1"},
	{VK_F2, "F2"},
	{VK_F3, "F3"},
	{VK_F4, "F4"},
	{VK_F5, "F5"},
	{VK_F6, "F6"},
	{VK_F7, "F7"},
	{VK_F8, "F8"},
	{VK_F9, "F9"},
	{VK_F10, "F10"},
	{VK_F11, "F11"},
	{VK_F12, "F12"},
	{VK_NUMLOCK, "Num Lock"},
	{VK_SCROLL, "Scroll Lock"},
	{0xba, ";"},
	{0xbb, "+"},
	{0xbc, ","},
	{0xbd, "-"},
	{0xbe, "."},
	{0xbf, "/"},
	{0xc0, "`"},
	{0xdb, "["},
	{0xdc, "\\"},
	{0xdd, "]"},
	{0xde, "\'"}
};
static VKT extvkt[] =
{
//	IBM extended key
	{VK_PRIOR, "PageUp"},
	{VK_NEXT, "PageDown"},
	{VK_END, "End"},
	{VK_HOME, "Home"},
	{VK_LEFT, "Left"},
	{VK_UP, "Up"},
	{VK_RIGHT, "Right"},
	{VK_DOWN, "Down"},
	{VK_INSERT, "Insert"},
	{VK_DELETE, "Del"},
};

CString HotkeyToStr(BYTE fVirt, WORD key)
{
	if (!fVirt && !key)
		return "";
	CString str;
	if (fVirt & FCONTROL)
		str += "Ctrl+";
	if (fVirt & FALT)
		str += "Alt+";
	if (fVirt & FSHIFT)
		str += "Shift+";

	if (isdigit(key) || isupper(key))
		str += char(key);
	else
	{
		int i;

		char *name = NULL;
		//先在IBM extended裡面找
		for (i = 0;i < sizeof(extvkt) / sizeof(VKT);i++)
		{
			if (key == extvkt[i].key)
			{
				name = extvkt[i].name;
				break;
			}
		}

		if (name && !(fVirt&FNUMPAD))	//如果有找到，並且不是Numpad
			str += name;
		else
		{
			for (i = 0;i < sizeof(vkt) / sizeof(VKT);i++)
			{
				if (key == vkt[i].key)
				{
					str += vkt[i].name;
					break;
				}
			}
		}
	}
	if (str.IsEmpty())
		return "";
	return str;
}

#if 0

static WORD keyname_to_val(const char* name)
{
	if (name[1] == '\0' && name[0] < 127)
		return name[0];
	int i;
	for (i = 0;i < sizeof(extvkt) / sizeof(VKT);i++)
	{
		if (0 == strcmp(name, extvkt[i].name))
			return extvkt[i].key;
	}
	for (i = 0;i < sizeof(vkt) / sizeof(VKT);i++)
	{
		if (0 == strcmp(name, vkt[i].name))
			return vkt[i].key;
	}
	return 0;
}

void StrToHotkey(const char* str, BYTE* fVirt, WORD* key)
{
	*fVirt = 0;
	if (str[0] == '+' && (isdigit(key[0]) || isupper(str[0])))
	{
		*key = 0xbb;	// '+'
		return;
	}

	char tmp[64];	strcpy(tmp, str);
	char* pkey[4];
	pkey[0] = strtok(tmp, "+");
	int c = 1;
	while ((pkey[c] = strtok(NULL, "+")))
		c++;

	*key = keyname_to_val(pkey[c-1]);
	if (c > 1)
	{
		for (c -= 2; c >= 0; c--)
		{
			switch (keyname_to_val(pkey[c]))
			{
			case VK_CONTROL:
				*fVirt |= FCONTROL;
				break;
			case VK_MENU:
				*fVirt |= FALT;
				break;
			case VK_SHIFT:
				*fVirt |= FSHIFT;
				break;
			}
		}
	}
}

#endif