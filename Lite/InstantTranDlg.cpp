// The class created by BBcall //

#include "stdafx.h"
#include "InstantTranDlg.h"
#include "SearchPlugin.h"
#include "TermView.h"

//記憶ComboBox目前所選取的項次
int rememberInt1 = 1;
int rememberInt2 = 0;

//記憶ComboBox目前所選取的文字內容
CString rememberCStr1;
CString rememberCStr2;

IMPLEMENT_DYNAMIC(CTranslationDlg, CPropertyPage)

CTranslationDlg::CTranslationDlg() : CPropertyPage(CTranslationDlg::IDD)
{
	//{{AFX_DATA_INIT(CTranslationDlg)
	isInstantTranStr = _T("");
	//}}AFX_DATA_INIT

	SelectedCombo1 = 0;
	SelectedCombo2 = 10;
}

CTranslationDlg::~CTranslationDlg()
{

}

void CTranslationDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_TRAN_COMBO, tmpBBcallStr1);
	DDX_CBString(pDX, IDC_LENGTH_COMBO, tmpBBcallStr2);
}

BEGIN_MESSAGE_MAP(CTranslationDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_CBN_SELCHANGE(IDC_TRAN_COMBO, OnCbnSelchangeComboDroplist1)
	ON_CBN_SELCHANGE(IDC_LENGTH_COMBO, OnCbnSelchangeComboDroplist2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTranslationDlg::OnCbnSelchangeComboDroplist1()
{
	SetModified();
}

void CTranslationDlg::OnCbnSelchangeComboDroplist2()
{
	SetModified();
}

BOOL CTranslationDlg::OnApply()
{
	CString SelCombo, Buffer;

	CComboBox *TranCombo = (CComboBox *)GetDlgItem(IDC_TRAN_COMBO);
	tmpBBcallInt = TranCombo->GetCurSel();

	return CPropertyPage::OnApply();
}

void CTranslationDlg::OnOK()
{
	CDialog::OnOK();

	//先處理IDC_LENGTH_COMBO
	GetDlgItem(IDC_LENGTH_COMBO)->GetWindowText(tmpBBcallStr2);
	rememberCStr2 = tmpBBcallStr2;
	if (rememberCStr2 == "50")
	{
		SelectedCombo2 = 50;
		rememberInt2 = 4;
	}
	else if (rememberCStr2 == "25")
	{
		SelectedCombo2 = 25;
		rememberInt2 = 3;
	}
	else if (rememberCStr2 == "20")
	{
		SelectedCombo2 = 20;
		rememberInt2 = 2;
	}
	else if (rememberCStr2 == "15")
	{
		SelectedCombo2 = 15;
		rememberInt2 = 1;
	}
	else
	{
		SelectedCombo2 = 10;
		rememberInt2 = 0;
	}

	//在處理IDC_TRAN_COMBO
	GetDlgItem(IDC_TRAN_COMBO)->GetWindowText(tmpBBcallStr1);
	rememberCStr1 = tmpBBcallStr1;
	if (tmpBBcallStr1 == "Enable")
	{
		SelectedCombo1 = 1;
		SearchPluginCollection.setInstantTranslation(SelectedCombo1);
		SearchPluginCollection.setTranLength(SelectedCombo2);	//按下ok的時候, 要記住翻譯長度限制的值為何

		rememberInt1 = SelectedCombo1;		
		MessageBox("啟動即時翻譯功能 \n翻譯的長度限制為" + rememberCStr2 + "個字元", "即時翻譯功能設定");
	}
	else
	{
		SelectedCombo1 = 0;
		SearchPluginCollection.setInstantTranslation(SelectedCombo1);

		rememberInt1 = SelectedCombo1;
		MessageBox("關閉即時翻譯功能", "即時翻譯功能設定");
	}
}

void CTranslationDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CTranslationDlg::InitCombo1(int ID)
{
	CString Item[2] = {"Enable", "Disable"};
	CComboBox *ComboBox = (CComboBox *)GetDlgItem(ID);

	for(int i = 0; i < 2; i++)
	{
		ComboBox->AddString(Item[i]);
	}

	ComboBox->SetCurSel(rememberInt1);
	//SearchPluginCollection.setInstantTranslation(1);
}

void CTranslationDlg::InitCombo2(int ID)
{
	CString Item[5] = {"10", "15", "20", "25", "50"};
	CComboBox *ComboBox = (CComboBox *)GetDlgItem(ID);

	for(int j = 0; j < 5; j++)
	{
		ComboBox->AddString(Item[j]);
	}

	ComboBox->SetCurSel(rememberInt2);
}

BOOL CTranslationDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//ToDo
	InitCombo1(IDC_TRAN_COMBO);
	InitCombo2(IDC_LENGTH_COMBO);

	return TRUE;
}