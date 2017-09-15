// ConnectPage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "Address.h"
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

void CConnectPage::InitWithAddress(const CString& addr)
{
	address = addr;
	port = 0;

	CAddress a(address);
	if (!a.IsValid())
	{
		port = 23;
		int pos = address.ReverseFind(':');
		if (pos != -1)
		{
			port = (unsigned short)atoi(address.Mid(pos + 1));
			address = address.Left(pos);
		}
	}
}

CString CConnectPage::GetFormattedAddress() const
{
	CAddress addr(address);
	if (addr.IsValid())
		return addr.URL();
	if (port == 23 || port <= 0)
		return address;
	CString buf;
	buf.Format("%s:%d", LPCTSTR(address), port);
	return buf;
}

void CConnectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectPage)
	DDX_CBString(pDX, IDC_ADDRESS, address);
	DDX_Text(pDX, IDC_NAME, name);
	DDX_Text(pDX, IDC_PORT, port);
	DDV_MinMaxInt(pDX, port, 0, 65535);
	//}}AFX_DATA_MAP
	if (name.IsEmpty())
		name = address;
	if (port <= 0)
		port = 23;
}


BEGIN_MESSAGE_MAP(CConnectPage, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectPage)
	ON_CBN_EDITCHANGE(IDC_ADDRESS, &CConnectPage::OnAddressChanged)
	ON_CBN_SELCHANGE(IDC_ADDRESS, &CConnectPage::OnAddressChanged)
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

	OnAddressChanged();

	return TRUE;
}

void CConnectPage::OnAddressChanged()
{
	CEdit* port_field = (CEdit*) GetDlgItem(IDC_PORT);

	CString addr_text;
	GetDlgItemText(IDC_ADDRESS, addr_text);

	CAddress addr(addr_text);
	port_field->EnableWindow(!addr.IsValid());
	if (addr.IsValid())
	{
		CString port_text;
		port_text.Format("%d", addr.Port());
		port_field->SetWindowText(port_text);
	}
}