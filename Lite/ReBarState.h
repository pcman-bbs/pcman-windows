// ReBarState.h: interface for the CReBarState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REBARSTATE_H__D025F5BB_1B4D_4F7C_AE72_4D87C0EC23A8__INCLUDED_)
#define AFX_REBARSTATE_H__D025F5BB_1B4D_4F7C_AE72_4D87C0EC23A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class  CReBarBandPos
{
public:
	UINT wID;
	UINT fStyle;
	UINT cx;
	static void Restore(CReBarCtrl& rb,CReBarBandPos* bp);
	static void Save(CReBarCtrl& rb,CReBarBandPos* bp);
	inline CReBarBandPos(){wID=-1;}
};

#endif // !defined(AFX_REBARSTATE_H__D025F5BB_1B4D_4F7C_AE72_4D87C0EC23A8__INCLUDED_)
