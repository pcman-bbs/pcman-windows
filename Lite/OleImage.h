// OleImage.h: interface for the COleImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEIMAGE_H__F9F166B9_507C_4BC9_870A_C9A170700F43__INCLUDED_)
#define AFX_OLEIMAGE_H__F9F166B9_507C_4BC9_870A_C9A170700F43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <olectl.h>

class COleImage
{
public:
	HANDLE GetHandle();
	void Destroy();
	HBITMAP CopyBitmap();
	static void Finalize();
	static void Initialize();
	bool LoadFromFile(LPCTSTR file_name);
	bool LoadFromMem(LPVOID data, DWORD size);
	COleImage();
	virtual ~COleImage();

protected:
	bool LoadFromGlobalMem(HGLOBAL mem, DWORD size);
	IPicture* pic;
};

#endif // !defined(AFX_OLEIMAGE_H__F9F166B9_507C_4BC9_870A_C9A170700F43__INCLUDED_)
