// The class created by BBcall //

#include <afxcoll.h>
#include "../Resource/resource.h"

#pragma once


// BY BBcall //
//IMPLEMENT_DYNCREATE(CTranslationDlg, CPropertyPage)
class CTranslationDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CTranslationDlg)

public:
	CTranslationDlg();
	~CTranslationDlg();
	CString isInstantTranStr;

	enum { IDD = IDD_INSTANT_TRAN };
	CComboBox cIsTran;
	CString tmpBBcallStr1;
	CString tmpBBcallStr2;
	int tmpBBcallInt;

	int SelectedCombo1;
	int SelectedCombo2;
	bool isEnableTran;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	//以下兩個變數很重要, 不能刪掉
	virtual void InitCombo1(int ID);
	virtual void InitCombo2(int ID);

	virtual BOOL OnApply();
	void OnOK();
	void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	afx_msg void OnCbnSelchangeComboDroplist1();
	afx_msg void OnCbnSelchangeComboDroplist2();
};

