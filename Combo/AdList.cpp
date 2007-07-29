// AdList.cpp : implementation file
//

#include "../Lite/stdafx.h"
#include "../Lite/pcman.h"
#include "AdList.h"
#include "Aditem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdList dialog


CAdList::CAdList(CWnd* pParent /*=NULL*/)
		: CDialog(CAdList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdList)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CAdList, CDialog)
	//{{AFX_MSG_MAP(CAdList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	#if _MSC_VER >= 1310    // vs2003/vc71
	ON_BN_CLICKED(IDC_EDIT, ((AFX_PMSG)OnEdit))
	#else
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	#endif
	ON_BN_CLICKED(IDC_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdList message handlers

void CAdList::OnOK()
{
	int c = list.GetCount();
	CString str;
	AppConfig.webpage_filter.RemoveAll();
	AppConfig.webpage_filter.SetSize(c, 4);
	for (int i = 0; i < c; i++)
	{
		list.GetText(i, str);
		AppConfig.webpage_filter[i] = str;
	}
	list.Detach();
	CDialog::OnOK();
}

BOOL CAdList::OnInitDialog()
{
	list.Attach(::GetDlgItem(m_hWnd, IDC_LIST));
	for (int i = 0; i < AppConfig.webpage_filter.GetSize(); i++)
		list.AddString(AppConfig.webpage_filter[i]);
	return TRUE;
}

void CAdList::OnAdd()
{
	int i = list.AddString("\t");
	if (i == LB_ERR)
		return;
	list.SetCurSel(i);
	if (!OnEdit())
		OnDel();

}

void CAdList::OnDel()
{
	int i = list.GetCurSel();
	if (i == LB_ERR)
		return;
	list.DeleteString(i);
	if (i == list.GetCount())
		list.SetCurSel(i - 1);
	else
		list.SetCurSel(i);
}

BOOL CAdList::OnEdit()
{
	int i = list.GetCurSel();
	if (i == LB_ERR)
		return TRUE;
	CAdItem dlg;
	CString text;
	list.GetText(i, text);
	int p = text.Find('\t');
	dlg.title = text.Left(p);
	dlg.url = text.Mid(p + 1);
	if (dlg.DoModal() == IDOK)
	{
		list.InsertString(i, dlg.title + "\t" + dlg.url);
		list.DeleteString(i + 1);
		list.SetCurSel(i);
		return TRUE;
	}
	return FALSE;
}

void CAdList::OnCancel()
{
	list.Detach();
	CDialog::OnCancel();
}
