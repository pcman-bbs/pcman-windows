// BuildMenu.cpp : Defines the class behaviors for the application.
//

#include <optional>

#include "BuildMenu.h"
#include "..\Lite\StdAfx.h"
#include "..\Lite\AppConfig.h"
#include "..\Resource\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {

// Appends hot key to right side of menu.
class MenuHotkeyUpdater : public MenuVisitor
{
public:
	MenuHotkeyUpdater(const AcceleratorTable *accel_table)
		: accel_table_(accel_table) {}

	void VisitMenuItem(CMenu *menu, UINT index) override {
		UINT cmd = menu->GetMenuItemID(index);
		std::optional<ACCEL> accel = accel_table_->GetByCmd(cmd);
		if (accel) {
			CString text;
			menu->GetMenuString(index, text, MF_BYPOSITION);
			text.Append(_T("\t"));
			text.Append(HotkeyToStr(accel->fVirt, accel->key));
			menu->ModifyMenu(index, MF_BYPOSITION, cmd, text);
		}
	}

private:
	const AcceleratorTable *accel_table_;  // Not owned.
};

}  // namespace

void TraverseMenuPreorder(CMenu *menu, MenuVisitor *visitor)
{
	UINT count = menu->GetMenuItemCount();
	for (UINT i = 0; i < count; ++i) {
		visitor->VisitMenuItem(menu, i);

		CMenu *submenu = menu->GetSubMenu(i);
		if (submenu != nullptr) {
			TraverseMenuPreorder(submenu, visitor);
		}

		visitor->LeaveMenuItem(menu, i);
	}
}

void TraverseMenuPreorder(HMENU hMenu, MenuVisitor *visitor)
{
	CMenu menu;
	menu.Attach(hMenu);
	TraverseMenuPreorder(&menu, visitor);
	menu.Detach();
}

void AcceleratorTable::Set(const ACCEL &accel)
{
	DeleteByCmd(accel.cmd);
	DeleteByKey(accel.fVirt, accel.key);
	cmd_to_accel_.emplace(accel.cmd, accel);
	key_to_cmd_.emplace(Key{ accel.fVirt, accel.key }, accel.cmd);
}

void AcceleratorTable::DeleteByCmd(WORD cmd)
{
	auto it = cmd_to_accel_.find(cmd);
	if (it == cmd_to_accel_.end()) {
		return;
	}
	key_to_cmd_.erase(Key{it->second.fVirt, it->second.key});
	cmd_to_accel_.erase(it);
}

void AcceleratorTable::DeleteByKey(BYTE fVirt, WORD key)
{
	auto it = key_to_cmd_.find(Key{ fVirt, key });
	if (it == key_to_cmd_.end()) {
		return;
	}
	cmd_to_accel_.erase(it->second);
	key_to_cmd_.erase(it);
}

std::optional<ACCEL> AcceleratorTable::GetByCmd(WORD cmd) const
{
	auto it = cmd_to_accel_.find(cmd);
	if (it == cmd_to_accel_.end()) {
		return std::nullopt;
	}
	return it->second;
}

std::optional<ACCEL> AcceleratorTable::GetByKey(BYTE fVirt, WORD key) const
{
	auto it = key_to_cmd_.find(Key{fVirt, key});
	if (it == key_to_cmd_.end()) {
		return std::nullopt;
	}
	return cmd_to_accel_.at(it->second);
}

HACCEL AcceleratorTable::CreateHandle() const
{
	std::vector<ACCEL> accels;
	std::transform(
		cmd_to_accel_.begin(),
		cmd_to_accel_.end(),
		std::back_inserter(accels),
		[](auto& p) { return p.second; });
	return CreateAcceleratorTable(&accels[0], accels.size());
}

// static
AcceleratorTable AcceleratorTable::Default()
{
	HACCEL hacc = LoadAccelerators(AfxGetInstanceHandle(), LPSTR(IDR_BUILD_UI));
	AcceleratorTable table = From(hacc);
	DestroyAcceleratorTable(hacc);
	return table;
}

// static
AcceleratorTable AcceleratorTable::Load()
{
	return Default();
}

// static
AcceleratorTable AcceleratorTable::From(HACCEL haccel)
{
	std::vector<ACCEL> accels;
	accels.resize(CopyAcceleratorTable(haccel, NULL, 0));
	CopyAcceleratorTable(haccel, &accels[0], accels.size());

	AcceleratorTable table;
	for (const ACCEL &accel : accels) {
		table.Set(accel);
	}
	return table;
}

HMENU LoadResourceMenu(UINT resource_id, const AcceleratorTable &accel_table)
{
	CMenu menu;
	menu.LoadMenu(resource_id);
	MenuHotkeyUpdater updater(&accel_table);
	TraverseMenuPreorder(&menu, &updater);
	return menu.Detach();
}
