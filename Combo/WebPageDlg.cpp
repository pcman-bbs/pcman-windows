// WebPageDlg.cpp : implementation file
//

#include "../Lite/stdafx.h"
#include "WebPageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebPageDlg dialog


CWebPageDlg::CWebPageDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CWebPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebPageDlg)
	m_Name = _T("");
	m_URL = _T("");
	m_NamePrompt = _T("");
	m_URLPrompt = _T("");
	//}}AFX_DATA_INIT
}


void CWebPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPageDlg)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_URL, m_URL);
	if (!m_NamePrompt.IsEmpty())
		DDX_Text(pDX, IDC_NAME_STATIC, m_NamePrompt);
	if (!m_URLPrompt.IsEmpty())
		DDX_Text(pDX, IDC_URL_STATIC, m_URLPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPageDlg, CDialog)
	//{{AFX_MSG_MAP(CWebPageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebPageDlg message handlers

void CWebPageDlg::OnOK()
{
	// TODO: Add extra validation here
	UpdateData();
	if (m_Name.IsEmpty() || m_URL.IsEmpty())
		return;
	CDialog::OnOK();
}
