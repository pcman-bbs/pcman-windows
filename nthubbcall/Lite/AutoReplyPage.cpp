// AutoReplyPage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "AutoReplyPage.h"
#include "PasswdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Mainfrm.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoReplyPage property page


CAutoReplyPage::CAutoReplyPage() : CPropertyPage(CAutoReplyPage::IDD)
{
	triggers = NULL;
	//{{AFX_DATA_INIT(CAutoReplyPage)
	//}}AFX_DATA_INIT
}

CAutoReplyPage::~CAutoReplyPage()
{
}

void CAutoReplyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoReplyPage)
	DDX_Control(pDX, IDC_LIST, list);
	DDX_Control(pDX, IDC_MSG, msg);
	DDX_Control(pDX, IDC_RESPOND, respond);
	DDX_Control(pDX, IDC_FIRST, first);
	DDX_Control(pDX, IDC_COUNT, count);
	DDX_Control(pDX, IDC_ADD, add);
	DDX_Control(pDX, IDC_MODIFY, modify);
	DDX_Control(pDX, IDC_DEL, del);
	DDX_Control(pDX, IDC_PROTECT, protect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoReplyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAutoReplyPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelChange)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CANCEL, OnCancelBtn)
	ON_BN_CLICKED(IDC_PROTECT, OnProtect)
	ON_BN_CLICKED(IDC_SHOWHELP, OnHelp)
	ON_BN_CLICKED(IDC_PASSWORD, OnPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoReplyPage message handlers

void CAutoReplyPage::OnAdd()
{
	add.SetDlgCtrlID(IDC_SAVE);
	add.SetWindowText(LoadString(IDS_SAVE_BTN));
	del.SetDlgCtrlID(IDC_CANCEL);
	del.SetWindowText(LoadString(IDS_CANCEL_BTN));
	modify.EnableWindow(FALSE);
	EnableEdits(TRUE);
	msg.SetWindowText(NULL);
	respond.SetWindowText(NULL);
	protect.SetCheck(FALSE);
	first.SetWindowText("1");
	count.SetWindowText("1");
	list.EnableWindow(FALSE);
}

void CAutoReplyPage::OnDel()
{
	int sel = list.GetCurSel();
	if (sel == LB_ERR)
		return;
	if (MessageBox(LoadString(IDS_DEL_CONFIRM), LoadString(IDS_CONFIRM) , MB_OKCANCEL) == IDCANCEL)
		return;
	CTriggerItem* item = (CTriggerItem*)list.GetItemDataPtr(sel);
	list.DeleteString(sel);
	delete item;
	int c = list.GetCount() - 1;
	if (sel == 0)
		list.SetCurSel(0);
	else if (sel == c)
		list.SetCurSel(c - 1);
	UpdateDisplay();
}

void CAutoReplyPage::OnModify()
{
	int sel = list.GetCurSel();
	if (sel == LB_ERR)
		return;

	modify.SetDlgCtrlID(IDC_SAVE);
	modify.SetWindowText(LoadString(IDS_SAVE_BTN));
	del.SetDlgCtrlID(IDC_CANCEL);
	del.SetWindowText(LoadString(IDS_CANCEL_BTN));
	add.EnableWindow(FALSE);
	EnableEdits(TRUE);
	list.EnableWindow(FALSE);
}

void CAutoReplyPage::OnSelChange()
{
	UpdateDisplay();
}


void CAutoReplyPage::OnOK()
{
	if (triggers)
	{
		triggers->RemoveAll();
		int c = list.GetCount();
		triggers->count = c;
		for (int i = 0;i < c;i++)
		{
			CTriggerItem* item = (CTriggerItem*)list.GetItemDataPtr(i);
			item->pnext = NULL;
			if (triggers->plast)
			{
				triggers->plast->pnext = item;
				triggers->plast = item;
			}
			else
				triggers->pfirst = triggers->plast = item;
		}
	}
	CPropertyPage::OnOK();
}

BOOL CAutoReplyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	list.SetItemHeight(0, 16);
	if (triggers->count > 0)
	{
		CTriggerItem* item = triggers->pfirst;
		while (item)
		{
			CTriggerItem* tmp = new CTriggerItem;
			tmp->msg = item->msg;
			tmp->respond = item->respond;
			tmp->first = item->first;
			tmp->count = item->count;
			tmp->pnext = NULL;
			int i = list.AddString(item->msg);
			list.SetItemDataPtr(i, tmp);
			item = item->pnext;
		}
		list.SetCurSel(0);
		UpdateDisplay();
	}
	ResetUI();
	return TRUE;
}

void CAutoReplyPage::OnCancel()
{
	if (respond.IsWindowEnabled())	//還在編輯狀態
		return;

	int c = list.GetCount();
	for (int i = 0;i < c;i++)
	{
		CTriggerItem* tmp = (CTriggerItem*)list.GetItemDataPtr(i);
		delete tmp;
	}
	CPropertyPage::OnCancel();
}

void CAutoReplyPage::UpdateDisplay()
{
	int sel = list.GetCurSel();
	if (sel == LB_ERR)
	{
		msg.SetWindowText(NULL);
		respond.SetWindowText(NULL);
		protect.SetCheck(FALSE);
		return;
	}

	CTriggerItem* item = (CTriggerItem*)list.GetItemDataPtr(sel);
	msg.SetWindowText(item->msg);
	respond.SetPasswordChar(*LPCTSTR(item->respond) == '+' ? '*' : 0);
	CString tmp = LPCTSTR(item->respond) + 1;
	tmp.Replace("^M^J", "\r\n");
	tmp.Replace("^M", "\r\n");
	respond.SetWindowText(tmp);
	protect.SetCheck(*LPCTSTR(item->respond) == '+');

	char sfirst[8];
	itoa(item->first, sfirst, 10);
	first.SetWindowText(sfirst);
	char scount[8];
	if (item->count == 0)
		count.SetWindowText(NULL);
	else
	{
		itoa(item->count, scount, 10);
		count.SetWindowText(scount);
	}
}

void CAutoReplyPage::OnSave()
{
	CString tmp;
	msg.GetWindowText(tmp);
	if (tmp.IsEmpty())
	{
		// 設定不完整，必須輸入要自動回應的訊息!
		MessageBox(LoadString(IDS_INCOMPLETE_AUTO_SETTINGS) , NULL, MB_OK | MB_ICONSTOP);
		msg.SetFocus();
		return;
	}
//----------------------------

	CTriggerItem* item;
	BOOL badd = (add.GetDlgCtrlID() == IDC_SAVE);
	int i;
	if (badd)		//新增
	{
		if (list.FindString(0, tmp) != LB_ERR)
			return;
		item = new CTriggerItem;
		item->pnext = NULL;
	}
	else	//修改
	{
		i = list.GetCurSel();
		item = (CTriggerItem*)list.GetItemDataPtr(i);
	}

	item->msg = tmp;

	respond.GetWindowText(tmp);
	tmp.Replace("\r\n", "^M");
	item->respond = protect.GetCheck() ? "+" : "-";
	item->respond += tmp;

	char sfirst[8];
	first.GetWindowText(sfirst, 8);
	item->first = atoi(sfirst);
	if (item->first < 1)
		item->first = 1;
	char scount[8];
	count.GetWindowText(scount, 8);
	if (!*scount)
		item->count = 0;
	else
		item->count = atoi(scount);

	if (badd)	//如果新增
		i = list.AddString(item->msg);
	else	//如果修改
	{
		list.DeleteString(i);
		list.InsertString(i, item->msg);
	}
	list.SetItemDataPtr(i, item);
	list.SetCurSel(i);

	ResetUI();
}

void CAutoReplyPage::OnCancelBtn()
{
	ResetUI();
}

void CAutoReplyPage::ResetUI()
{
	add.SetDlgCtrlID(IDC_ADD);
	add.SetWindowText(LoadString(IDS_ADD_BTN));
	modify.SetDlgCtrlID(IDC_MODIFY);
	modify.SetWindowText(LoadString(IDS_MODIFY_BTN));
	del.SetDlgCtrlID(IDC_DEL);
	del.SetWindowText(LoadString(IDS_DEL_BTN));

	add.EnableWindow(TRUE);
	if (list.GetCurSel() != LB_ERR)
	{
		modify.EnableWindow(TRUE);
		del.EnableWindow(TRUE);
	}
	EnableEdits(FALSE);
	list.EnableWindow(TRUE);
	UpdateDisplay();
}

void CAutoReplyPage::EnableEdits(BOOL enable)
{
	msg.EnableWindow(enable);
	respond.EnableWindow(enable);
	first.EnableWindow(enable);
	count.EnableWindow(enable);
	protect.EnableWindow(enable);
	EnableParentBtn(!enable);
	if (enable)
		msg.SetFocus();
	else
		list.SetFocus();
}


void CAutoReplyPage::OnProtect()
{
	if (protect.GetCheck())	//如果是勾選密碼保護
	{
		if (!AppConfig.QueryPassword(TRUE))
			protect.SetCheck(0);
		else
			respond.SetPasswordChar('*');
	}
	else	//如果是取消密碼保護
	{
		CPasswdDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			protect.SetCheck(1);
		else
		{
			if (dlg.passwd != AppConfig.passwd)
			{
				protect.SetCheck(1);
				MessageBox(LoadString(IDS_PASSWD_ERR), LoadString(IDS_ERR) , MB_OK | MB_ICONSTOP);
			}
			else if (!AppConfig.passwd.IsEmpty())
				respond.SetPasswordChar(0);
		}
	}
}

void CAutoReplyPage::EnableParentBtn(BOOL enable)
{
	HWND p =::GetParent(m_hWnd);
	::EnableWindow(::GetDlgItem(p, IDOK), enable);
	::EnableWindow(::GetDlgItem(p, IDCANCEL), enable);
	HMENU sys =::GetSystemMenu(p, FALSE);
	::EnableMenuItem(sys, SC_CLOSE, MF_BYCOMMAND | (enable ? MF_ENABLED : MF_GRAYED));
}

void CAutoReplyPage::OnHelp()
{
	MessageBox(
		LoadString(IDS_AUTO_HELP),
		LoadString(IDS_HELP),
		MB_OK | MB_ICONINFORMATION);
}

void CAutoReplyPage::OnPassword()
{
	if (AppConfig.passwd.IsEmpty())
	{
		AppConfig.QueryPassword(TRUE);
		return;
	}

	CString oldpasswd = AppConfig.passwd;
	AppConfig.passwd.Empty();
	if (AppConfig.QueryPassword(FALSE, LoadString(IDS_OLD_PASSWD)))
	{
		if (AppConfig.passwd != oldpasswd)
		{
			MessageBox(LoadString(IDS_PASSWD_ERR), LoadString(IDS_ERR) , MB_OK | MB_ICONSTOP);
		}
		else
		{
			AppConfig.passwd.Empty();
			if (AppConfig.QueryPassword(TRUE, LoadString(IDS_NEW_PASSWD)))
				return;
		}
	}
	AppConfig.passwd = oldpasswd;
}
