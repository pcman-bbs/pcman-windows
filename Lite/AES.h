// AES.h: interface for the CAES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AES_H__194282C2_CF67_11D8_9B99_0040F427D59A__INCLUDED_)
#define AFX_AES_H__194282C2_CF67_11D8_9B99_0040F427D59A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "Rijndael.h"

class CAES : public CRijndael
{
public:
	void EnterPassword(CString passwd);
	inline BOOL IsInitialized(){	return m_bInit;	}
	CAES();
	virtual ~CAES();

protected:
	bool m_bInit;
};

#endif // !defined(AFX_AES_H__194282C2_CF67_11D8_9B99_0040F427D59A__INCLUDED_)
