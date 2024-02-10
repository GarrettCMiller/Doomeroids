///////////////////////////////////////////////////////////////////////////////
//		File:						MainFrm.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"
#include "PalDlg.h"
#include "MinimapDlg.h"
#include "TileProps.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CStatusBar			m_wndStatusBar;
	CToolBar			m_wndToolBar;
	CChildView			m_wndView;

	PalDlg				palwnd;
	MinimapDlg			minimap;
	TileProps			tp;

	bool				PalWndVisible;
	bool				MinimapVisible;
	bool				TilePropsVisible;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewPalette();
	afx_msg void OnUpdateViewPalette(CCmdUI *pCmdUI);
	afx_msg void OnViewMinimap();
	afx_msg void OnUpdateViewMinimap(CCmdUI *pCmdUI);
	afx_msg void OnViewTileproperties();
	afx_msg void OnUpdateViewTileproperties(CCmdUI *pCmdUI);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


extern CMainFrame* pFrame;
