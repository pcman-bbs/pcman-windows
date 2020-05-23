// CustomizeDlg.cpp : implementation file
//

#include <functional>
#include <vector>

#include "stdafx.h"
#include "pcman.h"
#include "CustomizeDlg.h"
#include "MainFrm.h"
#include "../BuildMenu/BuildMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {

class MenuTreeInserter : public MenuVisitor
{
public:
	MenuTreeInserter(
		CTreeCtrl *tree,
		AcceleratorTable *accel_table,
		std::function<CCustomizeDlg::HotkeyData*()> new_hotkey_data);

	void VisitMenuItem(CMenu *menu, UINT index) override;
	void LeaveMenuItem(CMenu *menu, UINT index) override;

private:
	CTreeCtrl *tree_;  // Not owned.
	AcceleratorTable *accel_table_;  // Not owned.
	std::function<CCustomizeDlg::HotkeyData*()> new_hotkey_data_;
	std::vector<HTREEITEM> stack_;
};

MenuTreeInserter::MenuTreeInserter(
	CTreeCtrl *tree,
	AcceleratorTable *accel_table,
	std::function<CCustomizeDlg::HotkeyData*()> new_hotkey_data)
	: tree_(tree)
	, accel_table_(accel_table)
	, new_hotkey_data_(new_hotkey_data)
{
	stack_.push_back(TVI_ROOT);
}

void MenuTreeInserter::VisitMenuItem(CMenu *menu, UINT index)
{
	static CString separator = LoadString(IDS_SEPARATOR);

	HTREEITEM parent = stack_.back();
	HTREEITEM item = nullptr;

	WORD state = LOWORD(menu->GetMenuState(index, MF_BYPOSITION));
	CString text;
	menu->GetMenuString(index, text, MF_BYPOSITION);

	if (menu->GetSubMenu(index)) {
		// Popup menu, not customizable.
		item = tree_->InsertItem(text, parent);
	} else if (state & MF_SEPARATOR) {
		// Separator.
		item = tree_->InsertItem(separator, parent);
	} else {
		// Regular menu item.
		item = tree_->InsertItem(text, parent);

		CCustomizeDlg::HotkeyData* data = new_hotkey_data_();
		data->type = (state & (MF_DISABLED | MF_CHECKED | MF_MENUBARBREAK)) ? CT_CMD : CT_MENU;
		data->state = (data->type & CT_MENU) ? state : 0;
		data->cmd = menu->GetMenuItemID(index);
		data->hitem = item;

		tree_->SetItemData(item, reinterpret_cast<DWORD_PTR>(data));
	}
	stack_.push_back(item);
}

void MenuTreeInserter::LeaveMenuItem(CMenu *menu, UINT index)
{
	tree_->Expand(stack_.back(), TVE_EXPAND);
	stack_.pop_back();
}

}  // namespace

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDlg dialog

CCustomizeDlg::CCustomizeDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CCustomizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomizeDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CCustomizeDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomizeDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelChanged)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEYDONE, OnHotkeyDone)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDlg message handlers

BOOL CCustomizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	menu_tree_.SubclassDlgItem(IDC_TREE, this);
	key_list_.SubclassDlgItem(IDC_LIST, this);
	text_assigned_to_.SubclassDlgItem(IDC_ASSIGN, this);
	hkedit_.SubclassDlgItem(IDC_HOTKEY, this);
	btn_apply_hotkey_.SubclassDlgItem(IDC_APPLY, this);
	btn_delete_hotkey_.SubclassDlgItem(IDC_DEL, this);

	// Load customizable menus.
	accel_table_ = AcceleratorTable::Load();
	MenuTreeInserter inserter(&menu_tree_, &accel_table_, [this]() { return NewHotkeyData(); });
	{
		HMENU hmenu = LoadResourceMenu(IDR_BUILD_UI, AcceleratorTable::Load());
		TraverseMenuPreorder(hmenu, &inserter);
		DestroyMenu(hmenu);
	}
	{
		HMENU hmenu = LoadResourceMenu(IDR_MENU_HIDDEN, AcceleratorTable::Load());
		TraverseMenuPreorder(hmenu, &inserter);
		DestroyMenu(hmenu);
	}

	// Build map from cmd to hotkey data.
	for (auto& data : hotkey_data_storage_) {
		cmd_to_hotkey_data_.emplace(data.cmd, &data);
	}

	menu_tree_.SelectSetFirstVisible(menu_tree_.GetChildItem(TVI_ROOT));
	menu_tree_.ShowWindow(SW_SHOW);
	return TRUE;
}


void CCustomizeDlg::OnOK()
{
	CFile ui;
	if (!ui.Open(ConfigPath + UI_FILENAME, CFile::modeWrite | CFile::modeCreate))
		return;

	UIWriteAccels();

	CDialog::OnOK();
}

void CCustomizeDlg::OnApply()
{
	HTREEITEM hitem = menu_tree_.GetSelectedItem();
	if (hitem == nullptr)
		return;

	HotkeyData *data = reinterpret_cast<HotkeyData *>(menu_tree_.GetItemData(hitem));
	if (data == nullptr)
		return;

	if (hkedit_.key) {
		ACCEL accel;
		accel.cmd = data->cmd;
		accel.fVirt = hkedit_.fVirt | (FNOINVERT | FVIRTKEY);
		accel.key = hkedit_.key;

		std::optional<ACCEL> existing_accel = accel_table_.GetByKey(accel.fVirt, accel.key);
		HotkeyData *existing_data = nullptr;

		if (existing_accel && existing_accel->cmd != data->cmd)	//如果本來是被別的功能使用
		{
			if (MessageBox(LoadString(IDS_REPLACE_OLD_HOTKEY_CONFIRM), NULL, MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
				return;

			existing_data = cmd_to_hotkey_data_.at(existing_accel->cmd);
		}

		accel_table_.Set(accel);

		if (existing_data) {
			UpdateTreeItemDisplay(existing_data);
		}
	}

	UpdateHotkeyDisplay(data);
}

void CCustomizeDlg::OnDel()
{
	int sel = key_list_.GetCurSel();
	if (sel == -1)
		return;

	// Confirm deletion.
	if (MessageBox(LoadString(IDS_DEL_CONFIRM) , NULL, MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		return;

	hkedit_.Reset();

	DeleteHotkeyForID(reinterpret_cast<HotkeyData *>(key_list_.GetItemData(sel)));
}

void CCustomizeDlg::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	HotkeyData *data = nullptr;

	if (auto item = menu_tree_.GetSelectedItem(); item) {
		data = reinterpret_cast<HotkeyData *>(menu_tree_.GetItemData(item));
	}

	UpdateHotkeyDisplay(data);
}

LRESULT CCustomizeDlg::OnHotkeyDone(WPARAM wparam, LPARAM lparam)
{
	// Show which command the hotkey is currently assigned to.
	HotkeyData* hkitem = HotkeyDataFromKeys(hkedit_.fVirt, hkedit_.key);
	if (hkitem)
		text_assigned_to_.SetWindowText(menu_tree_.GetItemText(hkitem->hitem));
	else
		text_assigned_to_.SetWindowText(LoadString(IDS_NOT_ASSIGNED));
	return 0;
}

void CCustomizeDlg::UpdateHotkeyDisplay(CCustomizeDlg::HotkeyData *data)
{
	key_list_.ResetContent();

	bool enable_edit = data != nullptr;
	btn_apply_hotkey_.EnableWindow(enable_edit);
	btn_delete_hotkey_.EnableWindow(enable_edit);
	hkedit_.EnableWindow(enable_edit);

	if (!data) {
		hkedit_.Reset();
		text_assigned_to_.SetWindowText(nullptr);
		return;
	}

	UpdateTreeItemDisplay(data);

	std::optional<ACCEL> accel = accel_table_.GetByCmd(data->cmd);
	if (accel) {
		CString hotkey_text = HotkeyToStr(accel->fVirt, accel->key);
		int index = key_list_.AddString(hotkey_text);
		key_list_.SetItemData(index, reinterpret_cast<DWORD_PTR>(data));
		key_list_.SetCurSel(0);

		hkedit_.SetWindowText(hotkey_text);
		text_assigned_to_.SetWindowText(menu_tree_.GetItemText(data->hitem));
	} else {
		btn_delete_hotkey_.EnableWindow(false);
		hkedit_.Reset();
		text_assigned_to_.SetWindowText(LoadString(IDS_NOT_ASSIGNED));
	}
}

void CCustomizeDlg::UpdateTreeItemDisplay(CCustomizeDlg::HotkeyData *data)
{
	CString text = menu_tree_.GetItemText(data->hitem);
	int p;
	if ((p = text.Find('\t')) != -1)
		text = text.Left(p);

	std::optional<ACCEL> accel = accel_table_.GetByCmd(data->cmd);
	if (accel) {
		text += '\t';
		text += HotkeyToStr(accel->fVirt, accel->key);
	}
	menu_tree_.SetItemText(data->hitem, text);
}

void CCustomizeDlg::UIWriteAccels()
{
	accel_table_.Save();
}

void CCustomizeDlg::OnCancel()
{
	CDialog::OnCancel();
}

CCustomizeDlg::HotkeyData* CCustomizeDlg::NewHotkeyData()
{
	hotkey_data_storage_.emplace_back();
	return &hotkey_data_storage_.back();
}

CCustomizeDlg::HotkeyData* CCustomizeDlg::HotkeyDataFromKeys(BYTE fVirt, WORD key)
{
	fVirt |= (FNOINVERT | FVIRTKEY);
	std::optional<ACCEL> accel = accel_table_.GetByKey(fVirt, key);
	if (!accel) {
		return nullptr;
	}
	auto it = cmd_to_hotkey_data_.find(accel->cmd);
	if (it == cmd_to_hotkey_data_.end()) {
		return nullptr;
	}
	return it->second;
}

// 從所有具有相同 ID (hkdel->id) 的選單項目中，移除和 hkdel 相同的熱鍵 (hkdel->fVirt + hkdel->key)
void CCustomizeDlg::DeleteHotkeyForID(CCustomizeDlg::HotkeyData* hkdel)
{
	accel_table_.DeleteByCmd(hkdel->cmd);
	UpdateHotkeyDisplay(hkdel);
}
