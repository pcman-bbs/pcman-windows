// HyperLinkPage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "HyperLinkPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHyperLinkPage property page

IMPLEMENT_DYNCREATE(CHyperLinkPage, CPropertyPage)

CHyperLinkPage::CHyperLinkPage() : CPropertyPage(CHyperLinkPage::IDD)
{
	//{{AFX_DATA_INIT(CHyperLinkPage)
	//}}AFX_DATA_INIT
}

CHyperLinkPage::~CHyperLinkPage()
{
}

void CHyperLinkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHyperLinkPage)
	DDX_Control(pDX, IDC_LIST, list);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_LINKUNDERLINE, AppConfig.link_underline);
	DDX_Check(pDX, IDC_LINKAUTOSWITCH, AppConfig.link_autoswitch);
}


BEGIN_MESSAGE_MAP(CHyperLinkPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHyperLinkPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_SAVE, OnEditSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLinkPage message handlers

void CHyperLinkPage::EnableParentBtn(BOOL enable)
{
	HWND p =::GetParent(m_hWnd);
	::EnableWindow(::GetDlgItem(p, IDOK), enable);
	::EnableWindow(::GetDlgItem(p, IDCANCEL), enable);
	HMENU sys =::GetSystemMenu(p, FALSE);
	::EnableMenuItem(sys, SC_CLOSE, MF_BYCOMMAND | (enable ? MF_ENABLED : MF_GRAYED));
}

void CHyperLinkPage::OnAdd()
{
	int i = list.InsertItem(0, NULL);
	list.SetItemData(i, 0x0066ff);
	list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	OnEdit();
}

void CHyperLinkPage::OnEdit()
{
	EnableParentBtn(FALSE);
	list.EnableWindow(FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BROWSE), TRUE);
	int i = int(list.GetFirstSelectedItemPosition()) - 1;
//	if( list.GetItemText(i,0).CompareNoCase("") )	//E-mail為內建型態
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_URLSCHEME), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_PROGRAM), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COLOR), TRUE);

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_EDIT), LoadString(IDS_SAVE_BTN));
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_DEL), LoadString(IDS_CANCEL_BTN2));
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_ADD), FALSE);
	GetDlgItem(IDC_EDIT)->SetDlgCtrlID(IDC_SAVE);
	::SetFocus(::GetDlgItem(m_hWnd, IDC_URLSCHEME));
}

void CHyperLinkPage::OnDel()
{
	if (::GetDlgItem(m_hWnd, IDC_SAVE))	//如果在編輯模式之內，取消編輯
	{
		int i = int(list.GetFirstSelectedItemPosition()) - 1;
		CString txt = list.GetItemText(i, 0);
		if (txt.IsEmpty())
		{
			list.DeleteItem(i);
			list.SetItemState((i == list.GetItemCount() ? i - 1 : i), LVIS_SELECTED, LVIS_SELECTED);;
		}
		LeaveEditMode();
		return;
	}

	if (MessageBox(LoadString(IDS_DEL_CONFIRM), LoadString(IDS_CONFIRM), MB_OKCANCEL) == IDCANCEL)
		return;
	int i = int(list.GetFirstSelectedItemPosition()) - 1;
	list.DeleteItem(i);
	i = (i == list.GetItemCount() ? i - 1 : i);
	list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	LeaveEditMode();
}

void CHyperLinkPage::OnEditSave()
{
	int i = int(list.GetFirstSelectedItemPosition()) - 1;
	CString txt;
	GetDlgItem(IDC_URLSCHEME)->GetWindowText(txt);
	if (txt.IsEmpty())
	{
		::SetFocus(::GetDlgItem(m_hWnd, IDC_URLSCHEME));
		return;
	}
	CString program;
	GetDlgItem(IDC_PROGRAM)->GetWindowText(program);
	list.SetItemData(i, DWORD(tmpclr));
	list.SetItemText(i, 0, txt);
	list.SetItemText(i, 1, program);
	LeaveEditMode();
}

void CHyperLinkPage::OnOK()
{
	int i;
	int c = list.GetItemCount();
	AppConfig.hyper_links.links.SetSize(c);
	CString txt;
	AppConfig.hyper_links.email = -1;
	for (i = 0;i < c;i++)
	{
		CBBSHyperLinkData& d = AppConfig.hyper_links.links[i];
		d.scheme = list.GetItemText(i, 0);
		if (d.scheme == '@')
			AppConfig.hyper_links.email = i;
		d.program = list.GetItemText(i, 1);
		d.color = (COLORREF)list.GetItemData(i);
	}
	CPropertyPage::OnOK();
}

BOOL CHyperLinkPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	list.InsertColumn(0, LoadString(IDS_HYPERLINK));
	list.InsertColumn(1, LoadString(IDS_OPEN_PROGRAM));
	CRect crc;
	list.GetClientRect(crc);
	list.SetColumnWidth(0, 100);
	list.SetColumnWidth(1, crc.Width() - 120);
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CBBSHyperLinkData* links = AppConfig.hyper_links.links.GetData();
	for (int i = 0;i < AppConfig.hyper_links.links.GetSize();i++)
	{
		int idx = list.InsertItem(0, links[i].scheme);
		list.SetItemText(idx, 1, links[i].program);
		list.SetItemData(idx, links[i].color);
	}

	list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	return TRUE;
}

void CHyperLinkPage::LeaveEditMode()
{
	CWnd* wndtmp = GetDlgItem(IDC_SAVE);
	if (wndtmp)	wndtmp->SetDlgCtrlID(IDC_EDIT);
	list.EnableWindow();
	EnableParentBtn(TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_ADD), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BROWSE), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_URLSCHEME), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_PROGRAM), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COLOR), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DEL), TRUE);

	::SetWindowText(::GetDlgItem(m_hWnd, IDC_EDIT), LoadString(IDS_MODIFY_BTN2));
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_DEL), LoadString(IDS_DEL_BTN));
	UpdateUI();
}

void CHyperLinkPage::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateUI();
	*pResult = 0;
}

void CHyperLinkPage::OnBrowse()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
					LoadString(IDS_EXE_FILTER));
	if (dlg.DoModal() == IDOK)
	{
		CWnd* pwnd = GetDlgItem(IDC_PROGRAM);
		pwnd->SetWindowText(dlg.GetPathName());
		pwnd->SetFocus();
	}
}

void CHyperLinkPage::UpdateUI()
{
	int i = int(list.GetFirstSelectedItemPosition()) - 1;
	if (i == -1)
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_DEL), FALSE);
		tmpclr = GetSysColor(COLOR_BTNFACE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT), TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_DEL), TRUE);

		tmpclr = (COLORREF)list.GetItemData(i);
	}

	GetDlgItem(IDC_URLSCHEME)->SetWindowText(list.GetItemText(i, 0));
	GetDlgItem(IDC_PROGRAM)->SetWindowText(list.GetItemText(i, 1));
	GetDlgItem(IDC_COLOR)->Invalidate(TRUE);
}

void CHyperLinkPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int i = int(list.GetFirstSelectedItemPosition()) - 1;
	SetBkColor(lpDrawItemStruct->hDC, tmpclr);
	ExtTextOut(lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &lpDrawItemStruct->rcItem, NULL, 0, NULL);
}

void CHyperLinkPage::OnColor()
{
	int i = int(list.GetFirstSelectedItemPosition()) - 1;
	CColorDialog dlg((COLORREF)list.GetItemData(i), CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		tmpclr = DWORD(dlg.GetColor());
		GetDlgItem(IDC_COLOR)->Invalidate();
	}
}
