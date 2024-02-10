///////////////////////////////////////////////////////////////////////////////
//		File:						PalDlg.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// PalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Map Editor.h"
#include "PalDlg.h"
#include "MainFrm.h"
#include "ChildView.h"


#include ".\paldlg.h"


// PalDlg dialog

IMPLEMENT_DYNAMIC(PalDlg, CDialog)
PalDlg::PalDlg(/*CImage *palette,*/ CWnd* pParent /*=NULL*/)
	: CDialog(PalDlg::IDD, pParent)
{
	tileset = NULL;
}

PalDlg::~PalDlg()
{
}

void PalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PalDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// PalDlg message handlers

void PalDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	/*CRect	client;
	GetClientRect(&client);*/

	if (tileset)
		tileset->BitBlt(dc, 0, 0);
}

void PalDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	int xi = point.x / 64,
		 yi = point.y / 64;

	int tile = yi * TILE_COUNT + xi;

	/*char buffer[80];
	sprintf(buffer, "Px: %d\tPy: %d\nXi: %d\tYi: %d\ncx: %d\tcy: %d\ntile: %d", point.x, point.y, xi, yi, cx, cy, tile);
	MessageBox(buffer);*/

	((CMainFrame*)m_pParentWnd)->m_wndView.CurrentTile.SetID(tile);

	CDialog::OnLButtonDown(nFlags, point);
}

void PalDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	pFrame->PalWndVisible	= false;

	CDialog::OnClose();
}

void PalDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_ESCAPE:
		break;

	default:
		CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}
