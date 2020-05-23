// AddressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Address.h"
#include "AddressDlg.h"
#include "AppConfig.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddressDlg dialog

CAddressDlg::CAddressDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CAddressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddressDlg)
	port = 23;
	name = _T("");
	//}}AFX_DATA_INIT
}

CString CAddressDlg::GetFormattedAddress() const
{
	// XXX: Duplicate code.
	CAddress addr(address);
	if (addr.IsValid())
		return addr.URL();
	if (port == 23 || port <= 0)
		return address;
	CString buf;
	buf.Format("%s:%d", LPCTSTR(address), port);
	return buf;
}

BEGIN_MESSAGE_MAP(CAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CAddressDlg)
	ON_CBN_EDITCHANGE(IDC_ADDRESS, &CAddressDlg::OnAddressChanged)
	ON_CBN_SELCHANGE(IDC_ADDRESS, &CAddressDlg::OnAddressChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressDlg message handlers

BOOL CAddressDlg::OnInitDialog()
{
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_ADDRESS);
	combo->SetWindowText(address);
	GetDlgItem(IDC_PORT)->SetWindowText("23");

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

void CAddressDlg::OnOK()
{
	CDialog::OnOK();

	GetDlgItem(IDC_ADDRESS)->GetWindowText(address);
	if (name.IsEmpty())
		name = address;

	if (port <= 0)
		port = 23;
}

CAddressDlg::~CAddressDlg()
{
}

void CAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CAddressDlg)
	DDX_Text(pDX, IDC_PORT, port);
	DDV_MinMaxInt(pDX, port, 0, 65535);
	DDX_Text(pDX, IDC_NAME, name);
	//}}AFX_DATA_MAP
	CDialog::DoDataExchange(pDX);
}

void CAddressDlg::OnAddressChanged()
{
	// XXX: Duplicate code.
	CEdit* port_field = (CEdit*)GetDlgItem(IDC_PORT);

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