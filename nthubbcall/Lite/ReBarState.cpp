// ReBarState.cpp: implementation of the CReBarState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "ReBarState.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CReBarBandPos::Restore(CReBarCtrl& rb, CReBarBandPos* bp)
{
	int c = rb.GetBandCount();
	REBARBANDINFO bi;
	bi.cbSize = sizeof(bi);
	bi.fMask = RBBIM_SIZE | RBBIM_ID | RBBIM_STYLE;
	for (int i = 0;i < c;i++)
	{
		if (bp[i].wID == -1)
			continue;
		rb.MoveBand(rb.IDToIndex(bp[i].wID), i);
		bi.wID = bp[i].wID;
		bi.cx = bp[i].cx;
		bi.fStyle = bp[i].fStyle;
		rb.SetBandInfo(i, &bi);
	}
}

void CReBarBandPos::Save(CReBarCtrl& rb, CReBarBandPos* bp)
{
	int c = rb.GetBandCount();
	REBARBANDINFO bi;
	bi.cbSize = sizeof(bi);
	bi.fMask = RBBIM_SIZE | RBBIM_ID | RBBIM_STYLE;
	for (int i = 0;i < c;i++)
	{
		rb.GetBandInfo(i, &bi);
		bp[i].wID = bi.wID;
		bp[i].cx = bi.cx;
		bp[i].fStyle = bi.fStyle;
	}
}
