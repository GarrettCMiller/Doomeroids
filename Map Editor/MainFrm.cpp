///////////////////////////////////////////////////////////////////////////////
//		File:						MainFrm.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Map Editor.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_PALETTE, OnViewPalette)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE, OnUpdateViewPalette)
	ON_COMMAND(ID_VIEW_MINIMAP, OnViewMinimap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MINIMAP, OnUpdateViewMinimap)
	ON_COMMAND(ID_VIEW_TILEPROPERTIES, OnViewTileproperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TILEPROPERTIES, OnUpdateViewTileproperties)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,				// status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	tp.DestroyWindow();
	minimap.DestroyWindow();
	palwnd.DestroyWindow();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	ShowWindow(SW_MAXIMIZE);

	CRect client;
	GetClientRect(&client);

	palwnd.Create(IDD_PALETTE, this);
	palwnd.ShowWindow(SW_SHOW);

	palwnd.SetParent(this);

	palwnd.SetWindowPos(&wndTop, client.right - 200, client.top + 250, m_wndView.TileSet.GetWidth() + 5, m_wndView.TileSet.GetHeight() + 25, SWP_SHOWWINDOW);
	
	palwnd.SetTileSet(&(m_wndView.TileSet));
	palwnd.Invalidate();

	PalWndVisible = true;

	minimap.Create(IDD_MINIMAP, this);
	minimap.ShowWindow(SW_SHOW);

	minimap.SetParent(this);

	minimap.SetWindowPos(&wndTop, client.left + 200, client.top + 250, 150, 200, SWP_SHOWWINDOW);

	minimap.Invalidate();

	MinimapVisible = true;

	tp.Create(IDD_TILE_PROPS, this);
	tp.ShowWindow(SW_SHOW);

	tp.SetParent(this);

	tp.SetWindowPos(&wndTop, client.right - 500, client.top + 350, 344, 194, SWP_SHOWWINDOW | SWP_NOSIZE);

	tp.Invalidate();

	TilePropsVisible = true;

	m_wndView.SetupDC();

	//tp.DoModal();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnViewPalette()
{
	PalWndVisible = !palwnd.IsWindowVisible();
	palwnd.ShowWindow((PalWndVisible ? SW_SHOW : SW_HIDE));
}

void CMainFrame::OnUpdateViewPalette(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(PalWndVisible);
}

void CMainFrame::OnViewMinimap()
{
	MinimapVisible	= !minimap.IsWindowVisible();
	minimap.ShowWindow(MinimapVisible ? SW_SHOW : SW_HIDE);
}

void CMainFrame::OnUpdateViewMinimap(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(MinimapVisible);
}

void CMainFrame::OnViewTileproperties()
{
	TilePropsVisible = !tp.IsWindowVisible();
	tp.ShowWindow(TilePropsVisible ? SW_SHOW : SW_HIDE);
}

void CMainFrame::OnUpdateViewTileproperties(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(TilePropsVisible);
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	
	return CFrameWnd::WindowProc(message, wParam, lParam);
}
