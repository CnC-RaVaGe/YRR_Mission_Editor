/*
    FinalSun/FinalAlert 2 Mission Editor

    Copyright (C) 1999-2024 Electronic Arts, Inc.
    Authored by Matthias Wagner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// MyViewFrame.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FinalSun.h"
#include "MyViewFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyViewFrame

IMPLEMENT_DYNCREATE(CMyViewFrame, CFrameWnd)

CMyViewFrame::CMyViewFrame()
{
	m_isoview=0;
}

CMyViewFrame::~CMyViewFrame()
{
}


BEGIN_MESSAGE_MAP(CMyViewFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMyViewFrame)
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMyViewFrame 

BOOL CMyViewFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	SIZE z;
	z.cx=200;
	z.cy=200;

	CRect r;
	r.right=200;
	r.bottom=200;
	


	if(!m_Splitter.CreateStatic(this,1,2)) return FALSE;
		

	if(!m_Splitter.CreateView(0,0,
		RUNTIME_CLASS(CViewObjects),
		z,
		pContext)) return FALSE;

	if(!m_Splitter.CreateView(0,1,
		RUNTIME_CLASS(CRightFrame),
		z,
		pContext)) return FALSE;

	OutputDebugString("CMyViewFrame::OnCreateClient(): windows created\n");

	m_rightFrame=(CRightFrame*)m_Splitter.GetPane(0,1);
	
	m_isoview=(CIsoView*)m_rightFrame->m_Splitter.GetPane(0,0);
	m_isoview->owner=this;
	m_browser=(CTileSetBrowserFrame*)m_rightFrame->m_Splitter.GetPane(1,0);
	m_objectview=(CViewObjects*)m_Splitter.GetPane(0,0);

	// the minimap is not a child window right now, but it is created here though
	auto miniMapClass = AfxRegisterWndClass(0, m_hArrowCursor, static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH)));
    m_minimap.CreateEx(0, miniMapClass, "Minimap", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE, r, NULL, 0);
    //m_minimap.Create(NULL, "Minimap", WS_OVERLAPPED)
	m_minimap.UpdateView();
	
	if(!m_statbar.CreateEx(this, SBT_TOOLTIPS)) return FALSE;
	
	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMyViewFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
if(nID==SC_CLOSE)
	{
		// ok now just hide the window
		ShowWindow(SW_HIDE);
		return;
	}	
	CFrameWnd::OnSysCommand(nID, lParam);
}

// Create Statusbar, Mouse Cursor object properties.
void CMyViewFrame::OnSize(UINT nType, int cx, int cy) 
{
	// we now check if our frame window has already created its child windows
	// this is true at the second OnSize
	// TODO: replace static
	static BOOL wasHere=FALSE;
	if(wasHere==FALSE)
	{
		wasHere=TRUE;
		return;
	}

	CFrameWnd::OnSize(nType, cx, cy);
	CStatusBarCtrl& stat=m_statbar.GetStatusBarCtrl();

	// Statusbar Icons
	HICON i_position = AfxGetApp()->LoadIcon(SB_ICON1);
	HICON i_selection = AfxGetApp()->LoadIcon(SB_ICON2);
	HICON i_terrain = AfxGetApp()->LoadIcon(SB_ICON3);
	HICON i_overlay = AfxGetApp()->LoadIcon(SB_ICON4);
	HICON i_overlaydata = AfxGetApp()->LoadIcon(SB_ICON5);
	HICON i_money = AfxGetApp()->LoadIcon(SB_ICON6);
	
	m_statbar.ShowWindow(SW_SHOW);
	CRect sr;
	GetWindowRect(sr);

	int SegmentWidth = sr.right / 8;

	// Statusbar item (part) widths.
	int Widths[6] = {
		308,
		sr.right - (SegmentWidth * 4),
		sr.right - (SegmentWidth * 3),
		sr.right - (SegmentWidth * 2),
		sr.right - 300,
		-1
	};

	// Item widths when screen width is below 1920 pixels.
	if (sr.right <= 1920)
	{
		Widths[1] = sr.right - (SegmentWidth * 4.5);
		Widths[2] = 0;
		Widths[3] = sr.right - (SegmentWidth * 3);
	} 

	// Item widths when screen width is below 1600 pixels.
	if (sr.right <= 1600)
	{
		Widths[1] = sr.right - 300;
		Widths[2] = 0;
		Widths[3] = 0;
		Widths[4] = 0;
	}
	
	stat.SetMinHeight(32);
	stat.SetParts(6, Widths);

	stat.SetIcon(0, i_position);
	stat.SetIcon(1, i_selection);
	stat.SetIcon(2, i_terrain);
	stat.SetIcon(3, i_overlay);
	stat.SetIcon(4, i_overlaydata);
	stat.SetIcon(5, i_money);

	stat.SetSimple(FALSE);
	m_statbar.ShowWindow(SW_SHOW);	
}

void CMyViewFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x=260;
	lpMMI->ptMinTrackSize.y=150;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMyViewFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_isoview->SendMessage(WM_KEYDOWN, nChar, nFlags);
	// CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CMyViewFrame::PostNcDestroy() 
{
	// do not call CFrameWnd::PostNcDestroy(), as long as MyViewFrame is not on the heap!	
	// CFrameWnd::PostNcDestroy();
}

void CMyViewFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	// CFrameWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMyViewFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	// CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}
