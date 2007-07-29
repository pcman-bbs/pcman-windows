// OleImage.cpp: implementation of the COleImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OleImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef  HRESULT(STDAPICALLTYPE *CreateStreamOnHGlobalFunc)(HGLOBAL, BOOL, LPSTREAM*);
CreateStreamOnHGlobalFunc __CreateStreamOnHGlobal = NULL;
typedef HRESULT(STDAPICALLTYPE *OleLoadPictureFunc)(IStream*, LONG , BOOL , REFIID, VOID**);
OleLoadPictureFunc __OleLoadPicture = NULL;
HINSTANCE ole32_dll = NULL;
HINSTANCE olepro32_dll = NULL;

COleImage::COleImage() : pic(NULL)
{

}

COleImage::~COleImage()
{
	Destroy();
}

bool COleImage::LoadFromFile(LPCTSTR file_name)
{
	HANDLE file = CreateFile(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == file)
		return false;
	DWORD size = GetFileSize(file, NULL);
	if (0 == size)
		return false;
	HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, size);
	if (global)
	{
		LPVOID data = GlobalLock(global);
		if (data)
		{
			DWORD dwBytesRead = 0;
			// read file and store in global memory
			BOOL bRead = ReadFile(file, data, size, &dwBytesRead, NULL);
			_ASSERTE(FALSE != bRead);
			GlobalUnlock(global);
		}
	}
	CloseHandle(file);
	return LoadFromGlobalMem(global, size);
}

void COleImage::Initialize()
{
	ole32_dll = LoadLibrary("ole32.dll");
	olepro32_dll = LoadLibrary("olepro32.dll");
	__CreateStreamOnHGlobal = (CreateStreamOnHGlobalFunc)GetProcAddress(ole32_dll, "CreateStreamOnHGlobal");
	__OleLoadPicture = (OleLoadPictureFunc)GetProcAddress(olepro32_dll, "OleLoadPicture");
}

void COleImage::Finalize()
{
	__CreateStreamOnHGlobal = NULL;
	__OleLoadPicture = NULL;
	ole32_dll = NULL;
	olepro32_dll = NULL;
}

HBITMAP COleImage::CopyBitmap()
{
	OLE_HANDLE handle = NULL;
	HANDLE result = NULL;
	if ((handle = (OLE_HANDLE)GetHandle()))
	{
		SHORT type = 0;
		UINT utype = IMAGE_BITMAP;
		if (SUCCEEDED(pic->get_Type(&type)))
		{
			ICONINFO inf;
			if (type == PICTYPE_ICON)
			{
				GetIconInfo((HICON)handle, &inf);
				handle = (OLE_HANDLE)inf.hbmColor;
			}
			result = CopyImage((HANDLE)handle, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
		}
	}
	return (HBITMAP)result;
}

bool COleImage::LoadFromMem(LPVOID data, DWORD size)
{
	HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, size);
	if (global)
	{
		LPVOID data2 = GlobalLock(global);
		if (data)
		{
			memcpy(data2, data, size);
			GlobalUnlock(global);
		}
	}
	return LoadFromGlobalMem(global, size);
}

bool COleImage::LoadFromGlobalMem(HGLOBAL mem, DWORD size)
{
	IStream* stream = NULL;
	if (FAILED(__CreateStreamOnHGlobal(mem, TRUE, &stream)))
		return false;
	Destroy();
	HRESULT r = __OleLoadPicture(stream, size, FALSE, IID_IPicture, (LPVOID*) & pic);
	stream->Release();
	GlobalFree(mem);
	return SUCCEEDED(r) && pic;
}

void COleImage::Destroy()
{
	if (pic)
	{
		pic->Release();
		pic = NULL;
	}
}

HANDLE COleImage::GetHandle()
{
	OLE_HANDLE handle = NULL;
	if (SUCCEEDED(pic->get_Handle(&handle)))
		return (HANDLE)handle;
	return NULL;
}
