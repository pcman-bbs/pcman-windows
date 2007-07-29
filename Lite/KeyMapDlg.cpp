// KeyMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "KeyMapDlg.h"
#include "StrUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyMapDlg property page

IMPLEMENT_DYNCREATE(CKeyMapDlg, CDialog)

CKeyMapDlg::CKeyMapDlg() : CDialog(CKeyMapDlg::IDD)
{
	//{{AFX_DATA_INIT(CKeyMapDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CKeyMapDlg::~CKeyMapDlg()
{
}

BEGIN_MESSAGE_MAP(CKeyMapDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyMapDlg)
	ON_BN_CLICKED(IDC_DELKEY, OnDelKey)
	ON_BN_CLICKED(IDC_ADDKEY, OnAddKey)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYLIST, OnItemchangedKeylist)
	ON_BN_CLICKED(IDC_SAVEKEY, OnSavekey)
	ON_BN_CLICKED(IDC_MODIFYKEY, OnModifykey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyMapDlg message handlers

BOOL CKeyMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	list.SubclassDlgItem(IDC_KEYLIST, this);
	list.InsertColumn(0, LoadString(IDS_KEYBOARD_KEY));
	list.InsertColumn(1, LoadString(IDS_CONFIG_OF_KEY));
	CRect crc;
	list.GetClientRect(crc);
	list.SetColumnWidth(0, 200);
	list.SetColumnWidth(1, crc.Width() - 240);
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	hkedit.accepted &= ~FALT;
	hkedit.accepted |= (FBACK | FNUMPAD);
	hkedit.SubclassDlgItem(IDC_KEY, this);

	edit.SubclassDlgItem(IDC_STR, this);
	edit.SetLimitText(14);

	addbtn =::GetDlgItem(m_hWnd, IDC_ADDKEY);
	editbtn =::GetDlgItem(m_hWnd, IDC_MODIFYKEY);
	delbtn =::GetDlgItem(m_hWnd, IDC_DELKEY);
	okb =::GetDlgItem(m_hWnd, IDOK);
	cancelb =::GetDlgItem(m_hWnd, IDCANCEL);

	CString title;
	title.LoadString(IDS_EDIT_KEY_MAP_TITLE);

	//char *ptitle = new char[ title.GetLength() + 12 ];
	//memcpy( ptitle, LPCTSTR(title), title.GetLength() );
	//strncpy( ptitle + title.GetLength(), pmap->name, 11 );
	//Bug: without NULL char at the end of ptitle[]
	//SetWindowText(ptitle);
	//delete []ptitle;

	SetWindowText(title + pmap->name);

	int s = pmap ? pmap->GetSize() : 0;
	for (int i = 0;i < s;i++)
	{
		CString str;
		int idx = list.InsertItem(0,::HotkeyToStr(pmap->GetAt(i).fVirt, pmap->GetAt(i).key));
		list.SetItemText(idx, 1, EscapeControlChars(pmap->GetAt(i).str));	//將控制碼轉成可見字元
		list.SetItemData(idx, MAKELONG(pmap->GetAt(i).key, pmap->GetAt(i).fVirt));
	}
	list.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	UpdateDisplay();
	return TRUE;
}

void CKeyMapDlg::OnOK()
{
	pmap->RemoveAll();
	int c = list.GetItemCount();
	CString str;
	for (int i = 0;i < c;i++)
	{
		str = list.GetItemText(i, 1);
		CKeyMapEntry ent;
		DWORD inf = list.GetItemData(i);
		ent.key = LOWORD(inf);
		ent.fVirt = (BYTE)HIWORD(inf);
		strncpy(ent.str, UnescapeControlChars(str), 12);
		pmap->Add(ent);
	}
	pmap->FreeExtra();
	pmap->Save();
	CDialog::OnOK();
}


void CKeyMapDlg::OnAddKey()
{
	edit.SetWindowText(NULL);
	hkedit.SetWindowText(NULL);
	EnterEditMode(addbtn);
}

void CKeyMapDlg::OnDelKey()
{
	if (IsAdd() || IsEdit())	//如果是取消編輯
	{
		LeaveEditMode();
		return;
	}

	int sel = (int)list.GetFirstSelectedItemPosition() - 1;
	if (sel == -1)
		return;
	if (MessageBox(LoadString(IDS_DEL_CONFIRM), LoadString(IDS_CONFIRM), MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		return;

	list.DeleteItem(sel);
	int c = list.GetItemCount() - 1;
	if (sel == 0)
		list.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	else if (sel == c)
		list.SetItemState(c - 1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	else
		list.SetItemState(sel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	UpdateDisplay();
}

void CKeyMapDlg::OnItemchangedKeylist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateDisplay();
	*pResult = 0;
}

void CKeyMapDlg::UpdateDisplay()
{
	int sel = (int)list.GetFirstSelectedItemPosition() - 1;
	if (sel == -1)
	{
		hkedit.fVirt = 0;
		hkedit.key = 0;
		edit.SetWindowText(NULL);
		hkedit.SetWindowText(NULL);
	}
	else
	{
		DWORD inf = list.GetItemData(sel);
		hkedit.key = LOWORD(inf);
		hkedit.fVirt = (BYTE)HIWORD(inf);
		CString tmp;
		tmp = list.GetItemText(sel, 1);
		edit.SetWindowText(tmp);
		hkedit.SetWindowText(::HotkeyToStr(hkedit.fVirt , hkedit.key));
	}

}

void CKeyMapDlg::EnterEditMode(HWND savebtn)
{
	list.EnableWindow(FALSE);
	::EnableWindow(okb, FALSE);
	::EnableWindow(cancelb, FALSE);
	edit.EnableWindow();
	hkedit.EnableWindow();

	::SetWindowText(savebtn, LoadString(IDS_SAVE_BTN));
	::SetWindowLong(savebtn, GWL_ID, IDC_SAVEKEY);
	::SetWindowText(delbtn, LoadString(IDS_CANCEL_BTN2));
	::EnableWindow((savebtn == addbtn ? editbtn : addbtn), FALSE);
	if (savebtn == addbtn)
		hkedit.SetFocus();
	else
	{
		edit.SetFocus();
		edit.SetSel(0, -1);
	}
}

void CKeyMapDlg::LeaveEditMode()
{
	list.EnableWindow(TRUE);
	::EnableWindow(okb, TRUE);
	::EnableWindow(cancelb, TRUE);
	edit.EnableWindow(FALSE);
	hkedit.EnableWindow(FALSE);

	if (::GetDlgCtrlID(addbtn) == IDC_SAVEKEY)
	{
		::SetWindowText(addbtn, LoadString(IDS_ADD_BTN));
		::SetWindowLong(addbtn, GWL_ID, IDC_ADDKEY);
		::EnableWindow(editbtn, TRUE);
	}
	else
	{
		::SetWindowText(editbtn, LoadString(IDS_MODIFY_BTN));
		::SetWindowLong(editbtn, GWL_ID, IDC_MODIFYKEY);
		::EnableWindow(addbtn, TRUE);
	}
	::SetWindowText(delbtn, LoadString(IDS_DEL_BTN));
	UpdateDisplay();
}

void CKeyMapDlg::OnSavekey()
{
	CString str;
	edit.GetWindowText(str);
	if (str.IsEmpty() || !hkedit.key)
	{
		MessageBox(LoadString(IDS_SETTINGS_INCOMPLETE), LoadString(IDS_ERR), MB_ICONSTOP | MB_OK);
		return;
	}
	CString hkstr =::HotkeyToStr(hkedit.fVirt, hkedit.key);
	DWORD data = MAKELONG(hkedit.key, hkedit.fVirt);

	int sel;
	if (IsAdd())	//add
	{
		int c = list.GetItemCount();
		for (int i = 0;i < c;i++)
			if (list.GetItemData(i) == data)
			{
				MessageBox(LoadString(IDS_SPECIFIED_KEY_EXISTS) , LoadString(IDS_ERR), MB_ICONSTOP | MB_OK);
				return;
			}
		sel = list.InsertItem(0, "");
	}
	else	//Modify
		sel = (int)list.GetFirstSelectedItemPosition() - 1;

	list.SetItemData(sel, data);
	list.SetItemText(sel, 0, hkstr);
	list.SetItemText(sel, 1, str);
	list.SetItemState(sel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	LeaveEditMode();
}

void CKeyMapDlg::OnModifykey()
{
	int i = (int)list.GetFirstSelectedItemPosition() - 1;
	if (i == -1)
		return;
	EnterEditMode(editbtn);
}

void CKeyMapDlg::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CKeyMapDlg)
	//}}AFX_DATA_MAP
}


