// BuildMenu.h : main header file for the BUILDMENU application
//

#if !defined(AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_)
#define AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_

#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "..\Lite\StdAfx.h"

class AcceleratorTable {
public:
	void Set(const ACCEL &accel);
	void DeleteByCmd(WORD cmd);
	void DeleteByKey(BYTE fVirt, WORD key);
	std::optional<ACCEL> GetByCmd(WORD cmd) const;
	std::optional<ACCEL> GetByKey(BYTE fVirt, WORD key) const;

	HACCEL CreateHandle() const;
	bool Save();

	static AcceleratorTable Default();
	static AcceleratorTable Load();
	static AcceleratorTable From(HACCEL haccel);
	
private:
	using Key = std::pair<BYTE, WORD>;

	std::map<WORD, ACCEL> cmd_to_accel_;
	std::map<Key, WORD> key_to_cmd_;
};

class MenuVisitor {
public:
	virtual ~MenuVisitor() {}

	// Visit menu item at index. menu is the parent menu which item resides.
	virtual void VisitMenuItem(CMenu *menu, UINT index) {}

	// Called after leaving menu item at index.
	virtual void LeaveMenuItem(CMenu *menu, UINT index) {}
};

void TraverseMenuPreorder(HMENU handle, MenuVisitor *visitor);

HMENU LoadResourceMenu(UINT resource_id, const AcceleratorTable &accel_table);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_)
