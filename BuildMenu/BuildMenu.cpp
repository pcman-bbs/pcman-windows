// BuildMenu.cpp : Defines the class behaviors for the application.
//

#include <optional>

#include <cpprest/json.h>

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

constexpr const TCHAR *kHotkeyFileName = TEXT("hotkeys.json");

constexpr const wchar_t *kHotkeys = L"hotkeys";
constexpr const wchar_t *kCmd = L"cmd";
constexpr const wchar_t *kFVirt = L"fVirt";
constexpr const wchar_t *kKey = L"key";

constexpr const size_t kReadSize = 4096;

// Appends hot key to right side of menu.
class MenuHotkeyUpdater : public MenuVisitor
{
public:
	MenuHotkeyUpdater(const AcceleratorTable *accel_table)
		: accel_table_(accel_table) {}

	void VisitMenuItem(CMenu *menu, UINT index) override {
		UINT cmd = menu->GetMenuItemID(index);
		std::vector<ACCEL> accels = accel_table_->GetByCmd(cmd);
		if (!accels.empty()) {
			const auto& accel = accels[0];
			CString text;
			menu->GetMenuString(index, text, MF_BYPOSITION);
			text.Append(_T("\t"));
			text.Append(HotkeyToStr(accel.fVirt, accel.key));
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
	Delete(accel);
	key_to_accel_[Key{ accel.fVirt, accel.key }] = accel;
	cmd_to_accel_.emplace(accel.cmd, accel);
}

void AcceleratorTable::Delete(const ACCEL &accel)
{
	Key key = Key{ accel.fVirt, accel.key };

	auto it = key_to_accel_.find(key);
	if (it == key_to_accel_.end())
		return;

	auto range = cmd_to_accel_.equal_range(it->second.cmd);
	for (auto rt = range.first; rt != range.second; rt++) {
		if (key == Key{ rt->second.fVirt, rt->second.key }) {
			cmd_to_accel_.erase(rt);
			break;
		}
	}

	key_to_accel_.erase(it);
}

std::vector<ACCEL> AcceleratorTable::GetByCmd(WORD cmd) const
{
	std::vector<ACCEL> accels;
	auto range = cmd_to_accel_.equal_range(cmd);
	for (auto it = range.first; it != range.second; it++) {
		accels.push_back(it->second);
	}
	return accels;
}

std::optional<ACCEL> AcceleratorTable::GetByKey(BYTE fVirt, WORD key) const
{
	auto it = key_to_accel_.find(Key{fVirt, key});
	if (it == key_to_accel_.end()) {
		return std::nullopt;
	}
	return it->second;
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

bool AcceleratorTable::Save()
{
	using web::json::value;

	value hotkeys = value::array();
	for (const auto &[cmd, accel] : cmd_to_accel_) {
		value jaccel = value::object();
		jaccel[kCmd] = value::number(cmd);
		jaccel[kFVirt] = value::number(accel.fVirt);
		jaccel[kKey] = value::number(accel.key);
		hotkeys[hotkeys.size()] = jaccel;
	}

	std::ostringstream ss;
	value obj = web::json::value::object();
	obj[kHotkeys] = hotkeys;
	obj.serialize(ss);

	std::string data = ss.str();

	CFile output;
	if (!output.Open(ConfigPath + kHotkeyFileName, CFile::modeCreate | CFile::modeWrite))
		return false;
	output.Write(&data[0], data.size());
	output.Close();
	return true;
}

// static
AcceleratorTable AcceleratorTable::Load()
{
	using web::json::value;
	using web::json::array;
	using web::json::object;

	std::stringstream ss;
	{
		CFile input;
		if (!input.Open(ConfigPath + kHotkeyFileName, CFile::modeRead))
			return Default();
		std::string buf;
		buf.resize(kReadSize);
		int n;
		while ((n = input.Read(&buf[0], buf.size())) > 0) {
			ss.write(&buf[0], n);
		}
		input.Close();
	}

	try {
		AcceleratorTable table;
		array hotkeys = value::parse(ss).as_object()[kHotkeys].as_array();
		for (auto &jaccels : hotkeys) {
			ACCEL accel;
			accel.cmd = jaccels[kCmd].as_number().to_int32();
			accel.fVirt = jaccels[kFVirt].as_number().to_int32();
			accel.key = jaccels[kKey].as_number().to_int32();
			table.Set(accel);
		}
		return table;
	} catch (web::json::json_exception &) {
		return Default();
	}
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
