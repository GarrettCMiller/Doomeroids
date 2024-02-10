///////////////////////////////////////////////////////////////////////////////
//		File:						MinimapDlg.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// MinimapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Map Editor.h"
#include "MinimapDlg.h"
#include "MainFrm.h"
#include ".\minimapdlg.h"


// MinimapDlg dialog

IMPLEMENT_DYNAMIC(MinimapDlg, CDialog)
MinimapDlg::MinimapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MinimapDlg::IDD, pParent)
{
	m_fZoom = 0.1;
}

MinimapDlg::~MinimapDlg()
{
}

void MinimapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MinimapDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// MinimapDlg message handlers

void MinimapDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (!m_pParentWnd)
		return;

	CMainFrame	*parent		= (CMainFrame *)m_pParentWnd;//(CMainFrame*)GetParent();
	CChildView	*cv			= &parent->m_wndView;

	float psize = cv->TileSize.cx * m_fZoom;

	SetWindowPos(NULL, 
				50, 50, 
				cv->MapSize.cx * psize + 8, cv->MapSize.cy * psize + 25, 
				SWP_NOMOVE | SWP_NOZORDER);

	dc.SetStretchBltMode(HALFTONE);

	if (!dc.StretchBlt(	0, 0, 
						cv->DocSize.cx * m_fZoom, cv->DocSize.cy * m_fZoom, 
						&cv->mapdc, 
						0, 0, 
						cv->DocSize.cx, cv->DocSize.cy, 
						SRCCOPY))
		MessageBox("Failed to blit!");

	CRect c, dest;
	cv->GetClientRect(&c);

	CPen		penr(PS_SOLID, 1, RGB(255, 255, 255));
	CBrush bg;
	bg.FromHandle(HBRUSH(GetStockObject(NULL_BRUSH)));

	dc.SelectObject(penr);

	CSize viewport = CSize(	(cv->DocSize.cx - cv->MaxScroll.x) * m_fZoom,
							(cv->DocSize.cy - cv->MaxScroll.y) * m_fZoom);

	float ox = cv->Scroll.x * m_fZoom;
	float oy = cv->Scroll.y * m_fZoom;

	dest = CRect(
				ox,
				oy,
				viewport.cx, 
				viewport.cy);

	dest.right += dest.left;
	dest.bottom += dest.top;

	dc.FrameRect(&dest, &bg);

}

void MinimapDlg::OnClose()
{
	pFrame->MinimapVisible	 = false;

	CDialog::OnClose();
}
