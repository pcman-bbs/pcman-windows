#pragma once

#include "StdAfx.h"

// Helper to scale sizing objects according to DPI settings.
class CScaler
{
public:
	CScaler();
	~CScaler();

	void Update(CWnd* wnd);

	int CalcX(int x) const;
	int CalcY(int y) const;
	CPoint Calc(CPoint point) const;
	CRect Calc(CRect rect) const;

	bool InflateRect(LPRECT rect, int dx, int dy) const;
	bool OffsetRect(LPRECT rect, int dx, int dy) const;

private:
	int dpi_x_;
	int dpi_y_;
};
