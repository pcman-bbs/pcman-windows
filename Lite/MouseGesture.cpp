/****************************************************************************
MouseGesture.cpp: implementation of the CMouseGesture class.
written by PJ Arends
pja@telus.net

Copyright (c) 2005

-----------------------------------------------------------------------------
This code is provided as is, with no warranty as to it's suitability or usefulness
in any application in which it may be used. This code has not been tested for
UNICODE builds, nor has it been tested on a network ( with UNC paths ).

This code may be used in any way you desire. This file may be redistributed by any
means as long as it is not sold for profit, and providing that this notice and the
authors name are included.

If any bugs are found and fixed, a note to the author explaining the problem and
fix would be nice.
-----------------------------------------------------------------------------

Revision History:
    June 2005 - initial release on The CodeProject - http://www.codeproject.com

****************************************************************************/

#include "stdafx.h"
#include "MouseGesture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Registered message sent to the window that initiated the gesture
// when a gesture is recognized
//   WPARAM: The ID of the recognized gesture
//   LPARAM: A CMouseGesture* pointer to the CGesture object that sent the message
//////////////////////////////////////////////////////////////////////

//const UINT WMU_MOUSEGESTURE = RegisterWindowMessage(MOUSE_GESTURE_MESSAGE_STRING);

//////////////////////////////////////////////////////////////////////
// DEBUG only stuff
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Uncomment the #define _SHOW_GESTURE line to draw mouse points and
// bounding squares on screen when the gesture is being made.
//////////////////////////////////////////////////////////////////////

//#   define _SHOW_GESTURE  // this line controls visual feedback

#   ifdef _SHOW_GESTURE

void SHOW_BOUNDING_SQUARE(MOUSEHOOKSTRUCT *pMHS,
						  RECT rc)
{
	HWND hWnd = WindowFromPoint(pMHS->pt);
	MapWindowPoints(NULL, hWnd, (POINT *)&rc, 2);
	HDC hdc = GetDC(hWnd);
	DrawFocusRect(hdc, &rc);
	ReleaseDC(hWnd, hdc);
}

#   else // _SHOW_GESTURE
#       define SHOW_BOUNDING_SQUARE(pMHS, rc)
#   endif // _SHOW_GESTURE
#else // _DEBUG
#   define SHOW_BOUNDING_SQUARE(pMHS, rc)
#endif // _DEBUG

#ifdef _DEBUG
#define DEBUG_PRINT(str) OutputDebugString(str)
#else
#define DEBUG_PRINT(str)
#endif


//////////////////////////////////////////////////////////////////////
// handy functions borrowed from Chris Maunder's CGridCtrl class
//////////////////////////////////////////////////////////////////////

#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

//////////////////////////////////////////////////////////////////////
// Static private member variables
//////////////////////////////////////////////////////////////////////

CMouseGesture* CMouseGesture::Current = NULL;

//////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMouseGesture::CMouseGesture() :
		m_hWnd(NULL),
		m_bCaptured(false),
		m_bShift(false),
		m_bControl(false),
		m_ButtonFlag(0),
		m_ButtonDown(None),
		m_CurrentGesture(NULL),
		m_nDistance(0),
		m_LastDirection(None),
		m_szDirctionNow(NULL),
		fnUpdateMessage(NULL),
		m_dwDirction_BufLen(10)
{
}

CMouseGesture::~CMouseGesture()
{
	Detach();
}

//////////////////////////////////////////////////////////////////////
// Initialization functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Attach
//    public member function
//
// Sets up the CMouseGesture object for use
//
// Parameters:
//    hWnd     - The window that uses the gestures
//    Distance - The minimum distance, in pixels, that the mouse must
//               travel in order for a motion to be recognized
//
// Returns:
//    true if successful
//    false if not successful
//////////////////////////////////////////////////////////////////////

bool CMouseGesture::Attach(HWND hWnd,
						   UINT Distance /* = 25 */)
{
	_ASSERT(Distance > 9);
	_ASSERT(::IsWindow(hWnd));

	if (Distance < 10 || !IsWindow(hWnd))
	{
		return false;
	}

	if (m_hWnd != NULL)
	{
		// This CMouseGesture is already attached to a window
		// call Detach() before calling Attach()
		_ASSERT(false);
		return false;
	}

	m_hWnd = hWnd;
	m_nDistance = Distance;

	return true;
}

//////////////////////////////////////////////////////////////////////
// AddGesture
//    public member functions
//
// Adds mouse movement patterns for the CMouseGesture object to recognize
//
// Parameters:
//    ID       - An identifier number for the pattern
//    Motions  - A pointer to an array of Motions
//    count    - The number of Motions in the array
//    rGesture - A Gesture vector containing the Motions
//
// Returns:
//    -2 : gesture already present
//    -1 : a gesture with given ID already present
//    0  : error in the supplied gesture or invalid ID
//    >0 : number of gestures
//////////////////////////////////////////////////////////////////////

int CMouseGesture::AddGesture(UINT ID,
							  const Motion *Motions,
							  size_t count)
{
	_ASSERT(Motions != NULL);
	_ASSERT(count > 1);

	if (Motions == NULL || count < 2)
	{
		return 0;
	}

	Gesture gesture;
	for (size_t i = 0; i < count; ++i)
	{
		gesture.push_back(Motions[i]);
	}

	return AddGesture(ID, gesture);
}

int CMouseGesture::AddGesture(UINT ID,
							  const Gesture &rGesture)
{
	_ASSERT(ID > UNKNOWN_GETURE_ID);
	_ASSERT(rGesture.size() > 1);

	if (rGesture.size() < 2 || ID < 1)
	{
		return 0;
	}

	// Make sure the supplied gesture is valid
	if (rGesture[0] != LeftButton &&
		rGesture[0] != MiddleButton &&
		rGesture[0] != RightButton)
	{
		// The first element must be the mouse button
		_ASSERT(false);
		return 0;
	}

	Motion last = None;
	for (Gesture::const_iterator it = rGesture.begin(); it != rGesture.end(); ++it)
	{
		if (*it == LeftButton ||
			*it == MiddleButton ||
			*it == RightButton)
		{
			if (it != rGesture.begin())
			{
				// mouse button can only be first
				_ASSERT(false);
				return 0;
			}
			continue;
		}

		if (*it == Shift)
		{
			if (rGesture.size() < 3)
			{
				// Gesture must be more than just Shift
				_ASSERT(false);
				return 0;
			}

			if (it != rGesture.begin() + 1)
			{
				// Shift must be in second spot
				_ASSERT(false);
				return 0;
			}
			continue;
		}

		if (*it == Control)
		{
			if (rGesture.size() < 3)
			{
				// Gesture must be more than just Control
				_ASSERT(false);
				return 0;
			}

			if (it != rGesture.begin() + 1 &&
				it != rGesture.begin() + 2)
			{
				// Control must be either second or third
				_ASSERT(false);
				return 0;
			}

			if (it == rGesture.begin() + 2 &&
				(rGesture[1] != Shift || rGesture.size() < 4))
			{
				// if control is third, shift must be second, and there must be more motions
				_ASSERT(false);
				return 0;
			}
			continue;
		}

		if (*it == last ||
			*it < Up ||
			*it > Right)
		{
			// The motion has to be Up, Down, Left, or Right
			// The same motion can not immediately repeat
			_ASSERT(false);
			return 0;
		}
		last = *it;
	}

	// make sure the supplied ID is unique
	if (m_GestureMap.find(ID) != m_GestureMap.end())
	{
		_ASSERT(false);
		return -1;
	}

	// make sure the supplied gesture is unique
	if (GetGestureIdFromMap(rGesture) != UNKNOWN_GETURE_ID)
	{
		_ASSERT(false);
		return -2;
	}

	m_ButtonFlag |= rGesture[0];
	m_GestureMap.insert(std::make_pair(ID, rGesture));

	return m_GestureMap.size();
}

int CMouseGesture::AddGesture(UINT ID, const char *szGesture_In)
{
	const char *szGesture;
	Motion Motion_Now;
	Gesture g;
	int xRet;
	BOOL bHaveData;

	xRet = 0;
	bHaveData = FALSE;
	szGesture = szGesture_In;

	while (szGesture && *szGesture)
	{
		switch (*szGesture)
		{
		case 'U':
			Motion_Now = Up;
			break;
		case 'D':
			Motion_Now = Down;
			break;
		case 'L':
			Motion_Now = Left;
			break;
		case 'R':
			Motion_Now = Right;
			break;

		case 's':
			Motion_Now = Shift;
			break;
		case 'c':
			Motion_Now = Control;
			break;
		case 'l':
			Motion_Now = LeftButton;
			break;
		case 'm':
			Motion_Now = MiddleButton;
			break;
		case 'r':
			Motion_Now = RightButton;
			break;
		default:
			Motion_Now = None;
		}

		if (Motion_Now != None)
		{
			bHaveData = TRUE;
			g.push_back(Motion_Now);
		}
		szGesture++;
	}

	if (bHaveData)
		xRet = AddGesture(ID, g);

	VERIFY(xRet > 0);

	return xRet;
}

//////////////////////////////////////////////////////////////////////
// Deinitialize functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// RemoveGesture
//    public member function
//
// Removes the gesture pattern
//
// Parameters:
//    nID - The ID of the pattern to remove
//
// Returns
//   true : the pattern was found and removed
//   false : the pattern was not found
//////////////////////////////////////////////////////////////////////

bool CMouseGesture::RemoveGesture(UINT nID)
{
	bool ret = m_GestureMap.erase(nID) == 1;

	if (ret)
	{
		// reset the m_ButtonFlag in case a mouse button is eliminated
		m_ButtonFlag = 0;
		for (GestureMap::const_iterator it = m_GestureMap.begin(); it != m_GestureMap.end(); ++it)
		{
			m_ButtonFlag |= (*it).second[0];
		}
	}

	return ret;
}

int CMouseGesture::RemoveGesture_All()
{
	m_GestureMap.clear();
	m_ButtonFlag = 0;
	KillGesture();

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Detach
//    public member function
//
// Detaches this CWindowsGesture object from the window
//////////////////////////////////////////////////////////////////////

void CMouseGesture::Detach()
{
	m_hWnd = NULL;

}

//////////////////////////////////////////////////////////////////////
// Mouse message handler functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// MouseMessage
//    private member function
//
// Called by the MouseHookProc to handle the mouse messages
//
// Returns:
//    the this pointer if handling a gesture
//    NULL if not
//////////////////////////////////////////////////////////////////////

UINT CMouseGesture::MouseMessage(WPARAM wp, LPARAM lp)
{
	UINT ret = NULL;
	DWORD Message = (DWORD)wp;
	MOUSEHOOKSTRUCT *pMHS = (MOUSEHOOKSTRUCT *)lp;
	BOOL bKillGesture;
	BOOL bClearMsgBuf;

	bKillGesture = FALSE;
	bClearMsgBuf = FALSE;

	switch (Message)
	{
	case WM_LBUTTONDOWN:
		if (m_ButtonFlag & LeftButton)
		{
			m_CurrentGesture.clear();
			m_CurrentGesture.push_back(LeftButton);
			m_ButtonDown = LeftButton;
			ret = OnButtonDown(pMHS);
			bClearMsgBuf = TRUE;
		}
		break;

	case WM_MBUTTONDOWN:
		if (m_ButtonFlag & MiddleButton)
		{
			m_CurrentGesture.clear();
			m_CurrentGesture.push_back(MiddleButton);
			m_ButtonDown = MiddleButton;
			ret = OnButtonDown(pMHS);
			bClearMsgBuf = TRUE;
		}
		break;

	case WM_RBUTTONDOWN:
		if (m_ButtonFlag & RightButton)
		{
			m_CurrentGesture.clear();
			m_CurrentGesture.push_back(RightButton);
			m_ButtonDown = RightButton;
			ret = OnButtonDown(pMHS);
			bClearMsgBuf = TRUE;
		}
		break;

	case WM_LBUTTONUP:
		if (m_ButtonDown == LeftButton)
		{
			ret = OnButtonUp(pMHS);
		}
		bKillGesture = TRUE;
		break;

	case WM_MBUTTONUP:
		if (m_ButtonDown == MiddleButton)
		{
			ret = OnButtonUp(pMHS);
		}
		bKillGesture = TRUE;
		break;

	case WM_RBUTTONUP:
		if (m_ButtonDown == RightButton)
		{
			ret = OnButtonUp(pMHS);
		}
		bKillGesture = TRUE;
		break;

	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
		if (m_ButtonDown != None)
		{
			ret = OnMouseMove(pMHS);
		}
	}

	if (ret == NULL || bKillGesture)
	{
		KillGesture();
	}

	if (bClearMsgBuf)
	{
		m_szDirctionNow[0] = 0;
		if (fnUpdateMessage) fnUpdateMessage(UNKNOWN_GETURE_ID);
	}

	DBG_PRINT("MouseMessage(0X%04X) = %d, m_ButtonDown:%d\r\n", Message, ret, m_ButtonDown);

	return ret;
}

//////////////////////////////////////////////////////////////////////
// OnButtonDown
//    protected virtual member function
//
// Handles the start of a gesture
//////////////////////////////////////////////////////////////////////

UINT CMouseGesture::OnButtonDown(MOUSEHOOKSTRUCT *pMHS)
{
	m_BoundingSquare.left = pMHS->pt.x - m_nDistance;
	m_BoundingSquare.top = pMHS->pt.y - m_nDistance;
	m_BoundingSquare.right = pMHS->pt.x + m_nDistance;
	m_BoundingSquare.bottom = pMHS->pt.y + m_nDistance;

	SHOW_BOUNDING_SQUARE(pMHS, m_BoundingSquare);

	m_LastDirection = None;

	if (IsSHIFTpressed())
	{
		m_bShift = true;
		m_CurrentGesture.push_back(Shift);
	}

	if (IsCTRLpressed())
	{
		m_bControl = true;
		m_CurrentGesture.push_back(Control);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////
// OnMouseMove
//    protected virtual member function
//
// Handles the building of the gesture
//////////////////////////////////////////////////////////////////////

UINT CMouseGesture::OnMouseMove(MOUSEHOOKSTRUCT *pMHS)
{
#ifdef _SHOW_GESTURE
	{
		POINT pt;
		pt.x = pMHS->pt.x;
		pt.y = pMHS->pt.y;
		HWND hWnd = WindowFromPoint(pt);
		ScreenToClient(hWnd, &pt);
		HDC hdc = GetDC(hWnd);
		SetPixel(hdc, pt.x, pt.y, RGB(255, 0, 0));
		ReleaseDC(hWnd, hdc);
	}
#endif

	// make sure the SHIFT and CTRL keys are in
	// the same state as when the gesture started
	if (m_bShift != IsSHIFTpressed() ||
		m_bControl != IsCTRLpressed())
	{
		DBG_PRINT("OnMouseMove 1\r\n");
		return NULL;
	}

	// looking for next direction mouse is taking
	if (PtInRect(&m_BoundingSquare, pMHS->pt))
	{
		// not yet able to determine
		return 1;
	}

	Motion Direction = None;
	if ((pMHS->pt.x >= m_BoundingSquare.right) &&
		(pMHS->pt.y >= m_BoundingSquare.top) &&
		(pMHS->pt.y <= m_BoundingSquare.bottom))
	{
		Direction = Right;
	}
	else if ((pMHS->pt.x <= m_BoundingSquare.left) &&
			 (pMHS->pt.y >= m_BoundingSquare.top) &&
			 (pMHS->pt.y <= m_BoundingSquare.bottom))
	{
		Direction = Left;
	}
	else if ((pMHS->pt.y >= m_BoundingSquare.bottom) &&
			 (pMHS->pt.x >= m_BoundingSquare.left) &&
			 (pMHS->pt.x <= m_BoundingSquare.right))
	{
		Direction = Down;
	}
	else if ((pMHS->pt.y <= m_BoundingSquare.top) &&
			 (pMHS->pt.x >= m_BoundingSquare.left) &&
			 (pMHS->pt.x <= m_BoundingSquare.right))
	{
		Direction = Up;
	}

	if (Direction == None)
	{
		// Unable to calculate the direction, probably went
		// diagonally off a corner of the bounding square
		DBG_PRINT("OnMouseMove 2(0)\r\n");
		return 2;
	}

	SHOW_BOUNDING_SQUARE(pMHS, m_BoundingSquare);

	// move the bounding square to follow the mouse
	if (m_LastDirection == Direction)
	{
		switch (Direction)
		{
		case Left:
		case Right:
			m_BoundingSquare.left = pMHS->pt.x - m_nDistance;
			m_BoundingSquare.right = pMHS->pt.x + m_nDistance;
			break;
		case Up:
		case Down:
			m_BoundingSquare.top = pMHS->pt.y - m_nDistance;
			m_BoundingSquare.bottom = pMHS->pt.y + m_nDistance;
		}
	}
	else // m_LastDirection != Direction
	{
		m_BoundingSquare.left = pMHS->pt.x - m_nDistance;
		m_BoundingSquare.right = pMHS->pt.x + m_nDistance;
		m_BoundingSquare.top = pMHS->pt.y - m_nDistance;
		m_BoundingSquare.bottom = pMHS->pt.y + m_nDistance;

		// direction changed, save the new direction
		m_CurrentGesture.push_back(Direction);
		m_LastDirection = Direction;

		if (m_szDirctionNow)
		{
			char buf[4];

			switch (Direction)
			{
			case Left:
				buf[0] = 'L';
				break;
			case Right:
				buf[0] = 'R';
				break;
			case Up:
				buf[0] = 'U';
				break;
			case Down:
				buf[0] = 'D';
				break;
			}
			buf[1] = ',';
			buf[2] = 0;

			DWORD dw1;

			dw1 = strlen(buf) + strlen(m_szDirctionNow) + 1;
			if (dw1 < m_dwDirction_BufLen)
			{
				strcat(m_szDirctionNow, buf);

				UINT GestureID = GetGestureIdFromMap(m_CurrentGesture);

				DBG_PRINT("Get GestureID:%d(Move)\r\n", GestureID);

				if (fnUpdateMessage) fnUpdateMessage(GestureID);
			}
		}
	}

	SHOW_BOUNDING_SQUARE(pMHS, m_BoundingSquare);

	if (GetCapture() == NULL)
	{
		// capture the mouse so we can continue recieving messages
		// if the mouse leaves the window
//        SetCapture(m_hWnd);
		m_bCaptured = true;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////
// OnButtonUp
//    protected virtual member function
//
// Handles the end of the mouse gesture
//////////////////////////////////////////////////////////////////////

UINT CMouseGesture::OnButtonUp(MOUSEHOOKSTRUCT *pMHS)
{
	UNREFERENCED_PARAMETER(pMHS);

	// make sure the SHIFT and CTRL keys are in
	// the same state as when the gesture started
	if (m_bShift != IsSHIFTpressed() || m_bControl != IsCTRLpressed())
	{
		return INVALID_GETURE_ID;
	}

	BOOL bHaveMouseMove;

	bHaveMouseMove = FALSE;

	DBG_PRINT("\nDumping gesture:\n");

	for (Gesture::iterator it = m_CurrentGesture.begin(); it != m_CurrentGesture.end(); ++it)
	{
		switch (*it)
		{
		case Up:
			DBG_PRINT("Up, ");
			bHaveMouseMove = TRUE;
			break;
		case Down:
			DBG_PRINT("Down, ");
			bHaveMouseMove = TRUE;
			break;
		case Left:
			DBG_PRINT("Left, ");
			bHaveMouseMove = TRUE;
			break;
		case Right:
			DBG_PRINT("Right, ");
			bHaveMouseMove = TRUE;
			break;
		case LeftButton:
			DBG_PRINT("Left Button, ");
			break;
		case MiddleButton:
			DBG_PRINT("Middle Button, ");
			break;
		case RightButton:
			DBG_PRINT("Right Button, ");
			break;
		case Shift:
			DBG_PRINT("Shift, ");
			break;
		case Control:
			DBG_PRINT("Control, ");
			break;
		default:
			DBG_PRINT("Invalid Gesture");
		}
	}
	DBG_PRINT("\r\n");


	UINT GestureID = GetGestureIdFromMap(m_CurrentGesture);

	DBG_PRINT("Get GestureID:%d\r\n", GestureID);

	if (GestureID == UNKNOWN_GETURE_ID && bHaveMouseMove == FALSE)
		return INVALID_GETURE_ID;

	return GestureID;
}

//////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// GetGestureIdFromMap
//    private member function
//
// Finds the gesture in the map of recognized patterns
//
// Returns:
//    The ID of the pattern found
//    0 if not found
//////////////////////////////////////////////////////////////////////

UINT CMouseGesture::GetGestureIdFromMap(const Gesture &gesture)
{
	UINT ret = UNKNOWN_GETURE_ID;

	for (GestureMap::const_iterator it = m_GestureMap.begin(); it != m_GestureMap.end(); ++it)
	{
		if ((*it).second == gesture)
		{
			ret = (*it).first;
			break;
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////
// KillGesture
//    private member function
//
// Cleans up when a gesture ends or is abandoned
//////////////////////////////////////////////////////////////////////

void CMouseGesture::KillGesture()
{
#ifdef _SHOW_GESTURE
	// remove any drawing done
	RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
#endif
	DBG_PRINT("Gesture Ended\n");

	if (m_bCaptured && GetCapture() == m_hWnd)
	{
//        ReleaseCapture();
	}

	if (m_szDirctionNow[0])
	{
		m_szDirctionNow[0] = 0;
		if (fnUpdateMessage) fnUpdateMessage(UNKNOWN_GETURE_ID);
	}


	m_ButtonDown = None;
	m_bShift = false;
	m_bControl = false;
	m_CurrentGesture.clear();
	m_bCaptured = false;
}

//////////////////////////////////////////////////////////////////////
// End
//////////////////////////////////////////////////////////////////////
