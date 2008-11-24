// BuildMenu.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BuildMenu.h"
#include "..\Lite\AppConfig.h"
#include "..\Resource\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBuildMenuApp

BEGIN_MESSAGE_MAP(CBuildMenuApp, CWinApp)
	//{{AFX_MSG_MAP(CBuildMenuApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildMenuApp construction

CBuildMenuApp::CBuildMenuApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBuildMenuApp object

CBuildMenuApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBuildMenuApp initialization

/* Menu 和 Command Item的結構

檔案內部儲存方式:
WORD MAIN_ITEM_COUNT
CMDITEM ITEMS[TOTAL_COUNT]
DWORD ACCELCOUNT
ACCEL ACC_ITEMS[ACCELCOUNT]

struct CMDITEM
{
	BYTE TYPE,	CT_MENU,CT_HAS_SUB,CT_CMD,如果TYPE=0則為Separator，後面幾項都沒有
	WORD ID_OR_SUBCOUNT		如果有CT_HAS_SUB,為SUBCOUNT,如果沒有則為ID
	WORD state	只有Menu才有此項目
	WORD LEN	TEXT的長度,含0
	CHAR TEXT[]		長度不定,0結尾
}

*/

//		用來產生UI檔的程式碼
void UIWriteMenu(CFile& ui, HMENU hmenu, char* text, WORD state)
{
	CMenu menu;		menu.Attach(hmenu);
	SHORT c = menu.GetMenuItemCount();
	WORD l = strlen(text) + 1;
	BYTE type = (state & (MF_DISABLED | MF_CHECKED | MF_MENUBARBREAK) ? CT_CMD : CT_MENU) | CT_HAS_SUB;
	ui.Write(&type, 1);
	ui.Write(&c, 2);
	if (!(type & CT_MENU))
		state = 0;
	ui.Write(&state, sizeof(WORD));
	ui.Write(&l, 2);
	ui.Write(text, l);

	CString str;
	c--;
	for (int i = 0;i <= c;i++)
	{
		menu.GetMenuString(i, str, MF_BYPOSITION);
		HMENU sub = GetSubMenu(hmenu, i);
		state = LOWORD(menu.GetMenuState(i, MF_BYPOSITION));
		if (sub)
			UIWriteMenu(ui, sub, LPSTR(LPCTSTR(str)), state);
		else
		{
			if (state & MF_SEPARATOR)
			{
				BYTE type = 0;
				ui.Write(&type, 1);
				continue;
			}
			SHORT id = menu.GetMenuItemID(i);
			BYTE type = (state & (MF_DISABLED | MF_CHECKED | MF_MENUBARBREAK) ? CT_CMD : CT_MENU);
			ui.Write(&type, 1);
			ui.Write(&id, sizeof(WORD));
			if (!(type&CT_MENU))
				state = 0;
			ui.Write(&state, sizeof(WORD));
			l = str.GetLength() + 1;
			ui.Write(&l, 2);
			ui.Write(LPCTSTR(str), l);
		}
	}
	menu.Detach();
}

BOOL CBuildMenuApp::InitInstance()
{
//		用來產生UI檔的程式碼
	CFile ui;
	CString OutPath(m_lpCmdLine);
	OutPath += '\\';
	OutPath += UI_FILENAME;

//	MessageBox( NULL, OutPath, NULL, MB_OK );
	ui.Open(OutPath, CFile::modeWrite | CFile::modeCreate);

	HACCEL hacc = LoadAccelerators(AfxGetInstanceHandle(), LPSTR(IDR_BUILD_UI));
	WORD c = CopyAcceleratorTable(hacc, NULL, 0);
	ui.Write(&c, 2);
	ACCEL *accels = new ACCEL[c];
	CopyAcceleratorTable(hacc, accels, c);
	ui.Write(accels, sizeof(ACCEL)*c);
	delete []accels;
	DestroyAcceleratorTable(hacc);

	HMENU tmp = LoadMenu(AfxGetInstanceHandle(), LPSTR(IDR_BUILD_UI));
	UIWriteMenu(ui, tmp, "", 0);
	DestroyMenu(tmp);
	ui.Close();

	return FALSE;
}
