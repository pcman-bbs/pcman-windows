#if !defined(_HOTKEY_INCLUDED_)
#define _HOTKEY_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Hotkey.h : header file
//

#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// CHotkeyEdit window
/*
Defines for the fVirt field of the Accelerator table structure.
#define FVIRTKEY  TRUE          // Assumed to be == TRUE
#define FNOINVERT 0x02
#define FSHIFT    0x04
#define FCONTROL  0x08
#define FALT      0x10
*/
const BYTE FNUMPAD = 1 << 6;
CString HotkeyToStr(BYTE fVirt, WORD key);
void StrToHotkey(const char* str, BYTE* fVirt, WORD* key);

#endif // !defined(_HOTKEY_INCLUDED_)
