#include "StdAfx.h"
#include "Scaler.h"

namespace {

constexpr UINT kDpiBase = 96;

}  // namespace

CScaler::CScaler()
	: dpi_x_(kDpiBase)
	, dpi_y_(kDpiBase)
{
}

CScaler::~CScaler()
{
}

void CScaler::Update(CWnd* wnd)
{
	dpi_x_ = wnd->GetDC()->GetDeviceCaps(LOGPIXELSY);
	dpi_y_ = wnd->GetDC()->GetDeviceCaps(LOGPIXELSY);
}

int CScaler::CalcX(int x) const
{
	return MulDiv(x, dpi_x_, kDpiBase);
}

int CScaler::CalcY(int y) const
{
	return MulDiv(y, dpi_y_, kDpiBase);
}

CPoint CScaler::Calc(CPoint point) const
{
	return CPoint(
		MulDiv(point.x, dpi_x_, kDpiBase),
		MulDiv(point.y, dpi_y_, kDpiBase));
}

CRect CScaler::Calc(CRect rect) const
{
	return CRect(
		MulDiv(rect.left, dpi_x_, kDpiBase),
		MulDiv(rect.top, dpi_y_, kDpiBase),
		MulDiv(rect.right, dpi_x_, kDpiBase),
		MulDiv(rect.bottom, dpi_y_, kDpiBase));
}

bool CScaler::InflateRect(LPRECT rect, int dx, int dy) const
{
	return ::InflateRect(rect, CalcX(dx), CalcY(dy));
}

bool CScaler::OffsetRect(LPRECT rect, int dx, int dy) const
{
	return ::OffsetRect(rect, CalcX(dx), CalcY(dy));
}
