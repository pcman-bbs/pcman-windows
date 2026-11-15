#pragma once

class CCtrlEdit : public CEdit
{
public:
	UINT codePage = 950;
	bool Install(HWND parent, UINT id);
	CStringW Display(const CString& bytes, bool ansi) const;
	void ShowBytes(const CString& bytes, bool ansi);
	bool ReadBytes(CString& bytes) const;
private:
	CString original;
	CStringW shown;
	void Paste();
	static LRESULT CALLBACK EditProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
};
