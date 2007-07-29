// AdItem.cpp : implementation file
//

#include "../Lite/stdafx.h"
#include "../Lite/pcman.h"
#include "AdItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdItem dialog


CAdItem::CAdItem(CWnd* pParent /*=NULL*/)
		: CDialog(CAdItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdItem)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CAdItem, CDialog)
	//{{AFX_MSG_MAP(CAdItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdItem message handlers

void CAdItem::OnOK()
{
	GetDlgItem(IDC_TITLE)->GetWindowText(title);
	GetDlgItem(IDC_ADS)->GetWindowText(url);
	if (title.IsEmpty() && url.IsEmpty())
	{
		MessageBox(LoadString(IDS_ALL_EMPTY_DISALLOWED) , NULL, MB_OK | MB_ICONSTOP);
		return;
	}
	CDialog::OnOK();
}

BOOL CAdItem::OnInitDialog()
{
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_TITLE), title);
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_ADS), url);
	return TRUE;
}
