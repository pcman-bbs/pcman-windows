// ConnectPage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "ConnectPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectPage property page

IMPLEMENT_DYNCREATE(CConnectPage, CPropertyPage)

CConnectPage::CConnectPage() : CPropertyPage(CConnectPage::IDD)
{
	//{{AFX_DATA_INIT(CConnectPage)
	//}}AFX_DATA_INIT
}

CConnectPage::~CConnectPage()
{
}

void CConnectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectPage)
	DDX_CBString(pDX, IDC_ADDRESS, address);
	DDX_Text(pDX, IDC_NAME, name);
	DDV_MinMaxInt(pDX, port, 1, 65535);
	DDX_Text(pDX, IDC_PORT, port);
	//}}AFX_DATA_MAP
	if (name.IsEmpty())
		name = address;
	if (port <= 0)
		port = 23;
}


BEGIN_MESSAGE_MAP(CConnectPage, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectPage message handlers

BOOL CConnectPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_ADDRESS);

	POSITION pos = AppConfig.history.GetHeadPosition();
	CString str;
	while (pos)
	{
		str = AppConfig.history.GetAt(pos);
		int i;
		if ((i = str.ReverseFind(':')) >= 0)
			str = str.Left(i);
		combo->AddString(str);
		AppConfig.history.GetNext(pos);
	}

	return TRUE;
}
