// The class created by BBcall //

#include <afxcoll.h>
#include "../Resource/resource.h"

#pragma once

//IMPLEMENT_DYNCREATE(CTranslationDlg, CPropertyPage)
class CTranslationDlg : public CDialog
{
	DECLARE_DYNCREATE(CTranslationDlg)

public:
	CTranslationDlg();
	~CTranslationDlg();
	void DoDataExchange(CDataExchange* pDX);
	enum { IDD = IDD_INSTANT_TRAN };

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

