// StringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "StringDlg.h"
#include "MemIniFile.h"
#include "InputNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStringDlg dialog

void CStringDlgListCtrl::OnKeyDown(UINT ch, UINT n, UINT f)
{
	CStringDlg* p = (CStringDlg*)GetParent();
	if (ch == VK_RIGHT)
	{
		int i = p->tab.GetCurSel();
		int c = p->tab.GetItemCount();
		if (i < c - 1)
		{
			p->tab.SetCurSel(i + 1);
			p->UpdatePage();
		}
	}
	else if (ch == VK_LEFT)
	{
		int i = p->tab.GetCurSel();
		int c = p->tab.GetItemCount();
		if (i > 0)
		{
			p->tab.SetCurSel(i - 1);
			p->UpdatePage();
		}
	}
	else
		CListCtrl::OnKeyDown(ch, n, f);
}

BEGIN_MESSAGE_MAP(CStringDlgListCtrl, CListCtrl)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CStringDlg::CStringDlg(CWnd* pParent)
		: CDialog(CStringDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStringDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CStringDlg, CDialog)
	//{{AFX_MSG_MAP(CStringDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnListItemchanged)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnListGetDispInfo)
	ON_BN_CLICKED(IDC_NEWPAGE, OnNewPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_BN_CLICKED(IDC_DELPAGE, OnDelPage)
	ON_BN_CLICKED(IDC_RENAMEPAGE, OnRenamePage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PAGESPIN, OnDeltaPosPageSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ITEMSPIN, OnDeltaPosItemSpin)
	ON_LBN_DBLCLK(IDC_LIST, OnOK)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CANCEL, OnCancelBtn)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblClkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStringDlg message handlers

void CStringDlg::OnAdd()
{
	add.SetDlgCtrlID(IDC_SAVE);
	add.SetWindowText(LoadString(IDS_SAVE_BTN));
	del.SetDlgCtrlID(IDC_CANCEL);
	del.SetWindowText(LoadString(IDS_CANCEL_BTN));
	modify.EnableWindow(FALSE);
	ansi.EnableWindow(TRUE);
	control.EnableWindow(TRUE);
	hkedit.EnableWindow(TRUE);
	hkedit.SetWindowText(::HotkeyToStr(0, 0));
	hkedit.key = 0;
	hkedit.fVirt = 0;
	ansi.SetCheck(0);
	control.SetCheck(0);
	edit.SetReadOnly(FALSE);
	edit.SetFocus();
	edit.SetWindowText(NULL);
	list.EnableWindow(FALSE);
	EnableOKCancel(FALSE);
	inf = 0;
}

void CStringDlg::OnModify()
{
	int sel = (int)list.GetFirstSelectedItemPosition() - 1;
	if (sel == -1)
		return;

	modify.SetDlgCtrlID(IDC_SAVE);
	modify.SetWindowText(LoadString(IDS_SAVE_BTN));
	del.SetDlgCtrlID(IDC_CANCEL);
	del.SetWindowText(LoadString(IDS_CANCEL_BTN));
	add.EnableWindow(FALSE);
	ansi.EnableWindow(TRUE);
	control.EnableWindow(TRUE);
	hkedit.EnableWindow(TRUE);
	edit.SetReadOnly(FALSE);
	edit.SetFocus();
	list.EnableWindow(FALSE);
	EnableOKCancel(FALSE);
}

void CStringDlg::OnDel()
{
	int sel = (int)list.GetFirstSelectedItemPosition() - 1;
	if (sel == -1)
		return;

	if (MessageBox(LoadString(IDS_DEL_CONFIRM), LoadString(IDS_CONFIRM), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	list.DeleteItem(sel);
	stra->RemoveAt(sel);

	stra->inf.RemoveAt(sel);
	int c = list.GetItemCount() - 1;
	if (sel == 0)
		list.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	else if (sel == c)
		list.SetItemState(c - 1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	else
		list.SetItemState(sel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	UpdateDisplay();

	data_changed = TRUE;
}

DLGLAYOUT CStringDlg_layout[] =
{
	{IDC_ADD, DLA_BOTTOM | DLA_RIGHT},
	{IDC_MODIFY, DLA_BOTTOM | DLA_RIGHT},
	{IDC_SAVE, DLA_BOTTOM | DLA_RIGHT},
	{IDC_CANCEL, DLA_BOTTOM | DLA_RIGHT},
	{IDC_DEL, DLA_BOTTOM | DLA_RIGHT},
	{IDOK, DLA_BOTTOM | DLA_RIGHT},
	{IDCANCEL, DLA_BOTTOM | DLA_RIGHT},
	{IDC_EDIT, DLA_LEFT | DLA_BOTTOM | DLA_RIGHT},
	{IDC_CONTROL, DLA_LEFT | DLA_BOTTOM | DLA_RIGHT},
	{IDC_ANSI, DLA_LEFT | DLA_BOTTOM | DLA_RIGHT},
	{IDC_STREDIT, DLA_LEFT | DLA_BOTTOM | DLA_RIGHT},
	{IDC_STATIC1, DLA_LEFT | DLA_BOTTOM},
	{IDC_STATIC2, DLA_RIGHT | DLA_BOTTOM},
	{IDC_HOTKEY, DLA_RIGHT | DLA_BOTTOM},
	{IDC_LIST, DLA_ALL},
	{IDC_TAB, DLA_LEFT | DLA_RIGHT | DLA_TOP},
	{IDC_NEWPAGE, DLA_RIGHT | DLA_TOP},
	{IDC_RENAMEPAGE, DLA_RIGHT | DLA_TOP},
	{IDC_DELPAGE, DLA_RIGHT | DLA_TOP},
	{IDC_PAGESPIN, DLA_RIGHT | DLA_TOP},
	{IDC_ITEMSPIN, DLA_RIGHT | DLA_BOTTOM}
};


BOOL CStringDlg::OnInitDialog()
{
	dlgl.Load(m_hWnd, CStringDlg_layout, sizeof(CStringDlg_layout) / sizeof(DLGLAYOUT));
	SetIcon(GetParent()->GetIcon(TRUE), TRUE);
	SetIcon(GetParent()->GetIcon(FALSE), FALSE);

	data_changed = 0;

	list.SubclassDlgItem(IDC_LIST, this);
	edit.SubclassDlgItem(IDC_STREDIT, this);
	add.Attach(::GetDlgItem(m_hWnd, IDC_ADD));
	modify.Attach(::GetDlgItem(m_hWnd, IDC_MODIFY));
	del.Attach(::GetDlgItem(m_hWnd, IDC_DEL));
	ansi.Attach(::GetDlgItem(m_hWnd, IDC_ANSI));
	control.Attach(::GetDlgItem(m_hWnd, IDC_CONTROL));
	tab.Attach(::GetDlgItem(m_hWnd, IDC_TAB));

	hkedit.SubclassDlgItem(IDC_HOTKEY, this);

	list.InsertColumn(0, LoadString(IDS_FREQ_USED_TEXT));
	list.InsertColumn(1, LoadString(IDS_HOTKEY));
	CRect crc;
	list.GetClientRect(crc);
	list.SetColumnWidth(1, 150);
	list.SetColumnWidth(0, crc.Width() - 150);
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	LoadList();
//還原視窗狀態
	AppConfig.freqstr_state.Restore(m_hWnd);
	UpdateDisplay();
	ResetUI();
	UpdatePage();

	CDialog::OnInitDialog();
	return TRUE;
}

void CStringDlg::OnOK()
{
	int sel = (int)list.GetFirstSelectedItemPosition() - 1;
	if (sel != -1)
	{
		inf = stra->inf[sel];
		str = stra->ElementAt(sel);
	}
	else
	{
		str.Empty();
		inf = 0;
	}
	OnClose();
	CDialog::OnOK();
}

void CStringDlg::OnCancel()
{
	OnClose();
	CDialog::OnCancel();
}

void CStringDlg::SaveList()
{
	short tc = tab.GetItemCount();
	CFile f;
	DWORD tinf;
	if (f.Open(ConfigPath + FUS_FILENAME, CFile::modeWrite | CFile::modeCreate))
	{
		f.Write(&tc, sizeof(short));	//儲存頁籤數
		CString tmp;
		for (int i = 0;i < tc;i++)
		{
			TCITEM item;
			item.mask = TCIF_TEXT | TCIF_PARAM;
			item.pszText = tmp.GetBuffer(64);
			item.cchTextMax = 64;
			tab.GetItem(i, &item);
			tmp.ReleaseBuffer();
			stra = (MyStringArray*)item.lParam;
			short c = stra->GetSize();

			SaveString(f, tmp);		//儲存頁籤名稱
			f.Write(&c, sizeof(short));	//儲存該頁的字串數
			for (int i2 = 0;i2 < c;i2++)
			{
				tinf = stra->inf.ElementAt(i2);	//儲存字串資訊
				f.Write(&tinf, sizeof(DWORD));
				SaveString(f, stra->ElementAt(i2));	//儲存字串
			}
		}
	}
	stra = NULL;
}

void CStringDlg::ResetUI()
{
	add.SetDlgCtrlID(IDC_ADD);
	add.SetWindowText(LoadString(IDS_ADD_BTN));
	modify.SetDlgCtrlID(IDC_MODIFY);
	modify.SetWindowText(LoadString(IDS_MODIFY_BTN));
	del.SetDlgCtrlID(IDC_DEL);
	del.SetWindowText(LoadString(IDS_DEL_BTN));

	add.EnableWindow(TRUE);
	if (list.GetFirstSelectedItemPosition())
	{
		modify.EnableWindow(TRUE);
		del.EnableWindow(TRUE);
	}
	edit.SetReadOnly(TRUE);

	list.EnableWindow(TRUE);
	ansi.EnableWindow(FALSE);
	control.EnableWindow(FALSE);
	hkedit.EnableWindow(FALSE);
	EnableOKCancel(TRUE);
	list.SetFocus();
	UpdateDisplay();
}

void CStringDlg::OnCancelBtn()
{
	ResetUI();
}

void CStringDlg::OnSave()
{
	//key: low word		fVirt: high word
	WORD key = hkedit.key;
	BYTE fVirt = hkedit.fVirt;
	DWORD ninf = MAKELONG(key, (WORD)fVirt);
	if (ansi.GetCheck())
		ninf |= (1 << 31);		//bansi: highest bit 31
	if (control.GetCheck())
		ninf |= (1 << 30);		//bcontrol: bit 30

	CString tmp;
	edit.GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		MessageBox(LoadString(IDS_NOT_INPUT_YET), NULL, MB_OK | MB_ICONSTOP);
		edit.SetFocus();
		return;
	}
//----------------------------

	BOOL badd = (add.GetDlgCtrlID() == IDC_SAVE);
	int i = (int)list.GetFirstSelectedItemPosition() - 1;
	if (badd)	//如果新增
	{
		i = list.InsertItem((i == -1 ? list.GetItemCount() : i) , LPSTR_TEXTCALLBACK);
		stra->InsertAt(i, tmp);
		stra->inf.InsertAt(i, ninf);

//		if((stra->inf.ElementAt(i)&~(3<<30)))	//設了熱鍵
//			hotkey_changed=TRUE;

	}
	else	//如果修改
	{
//		if( (stra->inf.ElementAt(i)&~(3<<30)) != (ninf&~(3<<30)) )	//熱鍵改變
//			hotkey_changed=TRUE;

		list.SetItemText(i, 0, LPSTR_TEXTCALLBACK);
		stra->ElementAt(i) = tmp;
		stra->inf.ElementAt(i) = ninf;
	}

	list.SetItemText(i, 1, key ?
					 LPCTSTR(::HotkeyToStr(fVirt , key)) : NULL);

	list.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	ResetUI();

	data_changed = TRUE;
}

void CStringDlg::EnableOKCancel(BOOL enable)
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDOK), enable);
	::EnableWindow(::GetDlgItem(m_hWnd, IDCANCEL), enable);
}

void CStringDlg::UpdateDisplay()
{
	int sel = (int)list.GetFirstSelectedItemPosition() - 1;
	if (sel == -1)
	{
		edit.SetWindowText(NULL);
		hkedit.SetWindowText(NULL);
		ansi.SetCheck(0);
		control.SetCheck(0);
	}
	else if (stra)
	{
		CString tmp;
		tmp = stra->ElementAt(sel);
		edit.SetWindowText(tmp);
		inf = stra->inf.ElementAt(sel);
		ansi.SetCheck(inf & 1 << 31);
		control.SetCheck(inf & 1 << 30);
		BYTE fvirt = BYTE(HIWORD(inf) & ~(3 << 30));	// 11000000000....binary
		hkedit.fVirt = fvirt;	hkedit.key = LOWORD(inf);
		hkedit.SetWindowText(::HotkeyToStr(fvirt , hkedit.key));
	}
}


void CStringDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	dlgl.OnSize(nType, cx, cy);
	CRect crc;
	CListCtrl* plist = (CListCtrl*)GetDlgItem(IDC_LIST);
	if (plist)
	{
		plist->GetClientRect(crc);
		plist->SetColumnWidth(1, 150);
		plist->SetColumnWidth(0, crc.Width() - 150);
	}
}

void CStringDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	dlgl.OnGetMinMaxInfo(lpMMI);
}

void CStringDlg::OnListItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateDisplay();
	*pResult = 0;
}

void CStringDlg::OnListGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	if (!stra)
		return;
	LV_DISPINFO* pdi = (LV_DISPINFO*)pNMHDR;
	pdi->item.mask = LVIF_TEXT;
	if (pdi->item.iSubItem == 0 && stra)
		pdi->item.pszText = LPSTR(LPCTSTR(stra->ElementAt(pdi->item.iItem)));
	else if (pdi->item.iSubItem == 1)
	{
		DWORD dwinf = stra->inf.ElementAt(pdi->item.iItem);
		WORD key = LOWORD(dwinf);
		BYTE fvirt = BYTE(HIWORD(dwinf) & ~(1 << 31));
		list.SetItemText(pdi->item.iItem, 1,::HotkeyToStr(fvirt , key));
	}
}

void CStringDlg::OnNewPage()
{
	CInputNameDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		int i = tab.InsertItem(tab.GetItemCount(), dlg.name);
		tab.SetCurSel(i);
		stra = new MyStringArray;
		stra->SetSize(0, 8);
		TCITEM item;
		item.mask = TCIF_PARAM;
		item.lParam = (LPARAM)stra;
		tab.SetItem(i, &item);

		UpdatePage();

		data_changed = TRUE;
	}
}

void CStringDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdatePage();
	*pResult = 0;
}

void CStringDlg::OnDelPage()
{
	int i = tab.GetCurSel();
	if (tab.GetItemCount() <= 1)
		return;

	if (MessageBox(LoadString(IDS_DEL_PAGE_CONFIRM), LoadString(IDS_CONFIRM), MB_YESNO) == IDYES)
	{
		TCITEM item;
		item.mask = TCIF_PARAM;
		tab.GetItem(i, &item);
		stra = (MyStringArray*)item.lParam;
		delete stra;
		tab.DeleteItem(i);
		int c = tab.GetItemCount();
		if (i >= c)
			i = c - 1;
		tab.SetCurSel(i);
		UpdatePage();

		data_changed = TRUE;
	}
}

void CStringDlg::OnRenamePage()
{
	CInputNameDlg dlg;
	int i = tab.GetCurSel();
	TCITEM item;
	char buf[64];
	item.mask = TCIF_TEXT;
	item.pszText = buf;
	item.cchTextMax = 64;
	tab.GetItem(i, &item);
	dlg.name = buf;
	if (dlg.DoModal() == IDOK)
	{
		item.pszText = (LPSTR)(LPCTSTR)dlg.name;
		tab.SetItem(i, &item);

		data_changed = TRUE;
	}
}

void CStringDlg::UpdatePage()
{
	int i = tab.GetCurSel();
	if (i >= 0)
	{
		TCITEM item;
		item.mask = TCIF_PARAM;
		tab.GetItem(i, &item);
		stra = (MyStringArray*)item.lParam;

		list.DeleteAllItems();
		int c = stra->GetSize();
		for (int i = 0;i < c;i++)
		{
			list.InsertItem(i, LPSTR_TEXTCALLBACK);
			list.SetItemText(i, 1, LPSTR_TEXTCALLBACK);
		}
		list.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		list.SetFocus();
		UpdateDisplay();
	}
}

void CStringDlg::OnDeltaPosPageSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pud = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;
	int i = tab.GetCurSel();
	int i2 = i - pud->iDelta;
	if (i2 >= tab.GetItemCount() || i2 < 0)
		return;
	TCITEM item, item2;
	char buf[64], buf2[64];
	item.mask = item2.mask = TCIF_TEXT | TCIF_PARAM;
	item.pszText = buf;
	item2.pszText = buf2;
	item.cchTextMax = item2.cchTextMax = 64;
	tab.GetItem(i, &item);
	tab.GetItem(i2, &item2);
	tab.SetItem(i, &item2);
	tab.SetItem(i2, &item);
	tab.SetCurSel(i2);

	data_changed = TRUE;
}

void CStringDlg::OnDeltaPosItemSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pud = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;
	int i = (int)list.GetFirstSelectedItemPosition() - 1;
	int i2 = i + pud->iDelta;
	if (i == -1 || i2 >= list.GetItemCount() || i2 < 0 || !stra)
		return;

	CString tmp;
	tmp = stra->ElementAt(i);
	inf = stra->inf.ElementAt(i);
	stra->ElementAt(i) = stra->ElementAt(i2);
	stra->ElementAt(i2) = tmp;
	stra->inf.ElementAt(i) = stra->inf.ElementAt(i2);
	stra->inf.ElementAt(i2) = inf;
	list.SetItemText(i, 0, LPSTR_TEXTCALLBACK);
	list.SetItemText(i, 1, LPSTR_TEXTCALLBACK);
	list.SetItemText(i2, 0, LPSTR_TEXTCALLBACK);
	list.SetItemText(i2, 1, LPSTR_TEXTCALLBACK);

	list.SetItemState(i2, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);
	UpdateDisplay();
	data_changed = TRUE;
}


void CStringDlg::OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	list.ScreenToClient(&pt);

	int i = list.HitTest(pt);
	if (i != -1)
		OnOK();
	*pResult = 0;
}

void CStringDlg::OnClose()
{
	if (data_changed)
		SaveList();

	AppConfig.freqstr_state.Save(m_hWnd);
	stra = NULL;
	int tc = tab.GetItemCount();
	for (int p = 0;p < tc;p++)
	{
		TCITEM item;	item.mask = TCIF_PARAM;
		tab.GetItem(p, &item);
		MyStringArray* pstra = (MyStringArray*)item.lParam;
		delete pstra;
	}
#ifdef	_DEBUG
	add.Detach();
	modify.Detach();
	del.Detach();
	ansi.Detach();
	control.Detach();
	tab.Detach();
#endif
}

void CStringDlg::LoadList()
{
	CMemIniFile f;
	if (f.Open(ConfigPath + FUS_FILENAME, CFile::modeRead))
	{
		WORD tc;
		WORD c;
		TCITEM item;
		item.mask = TCIF_PARAM | TCIF_TEXT;
		f.Read(&tc, sizeof(WORD));
		for (int p = 0;p < tc;p++)
		{
			CString tmp = LoadString(f);
			item.pszText = (LPSTR)(LPCTSTR)tmp;	//Add Page
			stra = new MyStringArray;
			item.lParam = (LPARAM)stra;
			tab.InsertItem(p, &item);

			f.Read(&c, sizeof(WORD));	//Load StringArray
			stra->SetSize(c, 8);
			stra->inf.SetSize(c, 8);
			for (int i = 0;i < c;i++)
			{
				f.Read(&inf, sizeof(DWORD));
				stra->inf.SetAt(i, inf);
				stra->SetAt(i, LoadString(f));
			}
		}
		f.Close();
	}
}
