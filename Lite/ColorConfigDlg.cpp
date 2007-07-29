// ColorConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "ColorConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorConfigDlg dialog

CColorConfigDlg::CColorConfigDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CColorConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorConfigDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CColorConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CColorConfigDlg)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(5, 23, OnSelColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorConfigDlg message handlers

BOOL CColorConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CColorConfigDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	COLORREF clr = colormap[nIDCtl-5];
	CDC dc;
	char txt[10];
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rc;
	::GetClientRect(lpDrawItemStruct->hwndItem, &rc);
	::GetWindowText(lpDrawItemStruct->hwndItem, txt, 10);
	dc.DrawEdge(rc, lpDrawItemStruct->itemState&ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED, BF_RECT | BF_ADJUST);
	dc.FillSolidRect(rc, clr);
	dc.SetBkColor(clr);
	if (GetRValue(clr) > 128 || GetGValue(clr) > 128 || GetBValue(clr) > 128)
		dc.SetTextColor(0);
	else
		dc.SetTextColor(0xffffff);
	dc.DrawText(txt, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	dc.Detach();
}

void CColorConfigDlg::OnSelColor(UINT id)
{
	CColorDialog dlg(colormap[id-5], CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		colormap[id-5] = dlg.GetColor();
		GetDlgItem(id)->Invalidate();
	}
}

void CColorConfigDlg::OnDefault()
{
	memcpy(colormap, CAppConfig::default_colormap, sizeof(AppConfig.colormap));
	Invalidate(FALSE);
}
