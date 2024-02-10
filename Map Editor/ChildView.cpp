///////////////////////////////////////////////////////////////////////////////
//		File:						ChildView.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Map Editor.h"
#include "ChildView.h"
#include ".\childview.h"
#include "MapProps.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	DC_Created		= false;
	Saved			= false;
	Zoom			= 1.0;
	CurrentTile.SetID(0);

	CurrentTool	= TT_SELECTOR;

	CurrentTileSet = "Doom.bmp";

	TileSet.Load(CurrentTileSet);

	MapSize		= CSize(InitWidth, InitHeight);
	TileSize	= CSize(64,64);
	Scroll		= CPoint(0,0);
	DocSize		= CSize(MapSize.cx * TileSize.cx, MapSize.cy * TileSize.cy);
	ZoomDocSize = DocSize;

	map.Init(InitWidth, InitHeight);

	MapName = "Map1";
	MapDesc = "Your basic POS tile map :D";
}

CChildView::~CChildView()
{
	bitmap.DeleteObject();
	mapdc.DeleteDC();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_OPEN_MAP, OnOpenMap)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_OPEN_CUSTOMTILESET, OnOpenCustomtileset)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_TOOLS_SELECTOR, OnToolsSelector)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTOR, OnUpdateToolsSelector)
	ON_COMMAND(ID_TOOLS_CHANGE, OnToolsChange)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHANGE, OnUpdateToolsChange)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC	dc(this); // device context for painting

	if (mapdc)
		dc.StretchBlt(0,0, ZoomDocSize.cx, ZoomDocSize.cy, &mapdc, Scroll.x, Scroll.y, DocSize.cx, DocSize.cy, SRCCOPY);
}

void CChildView::SetupDC()
{
	if (bitmap.m_hObject)
		bitmap.DeleteObject();
	
	if (mapdc.m_hDC)
		mapdc.DeleteDC();

	CDC *cdc = GetDC();
	mapdc.CreateCompatibleDC(cdc);
	bitmap.CreateCompatibleBitmap(cdc, DocSize.cx, DocSize.cy);
	mapdc.SelectObject(bitmap);
	ReleaseDC(cdc);

	DrawMap();
}

void CChildView::DrawMap(bool erase)
{
	CClientDC dc(this);
	CRect client;
	GetClientRect(&client);

	if (erase)
	{
		mapdc.FillSolidRect(0, 0, DocSize.cx, DocSize.cy, RGB(0, 0, 0));
		dc.FillSolidRect(0, 0, ZoomDocSize.cx, ZoomDocSize.cy, RGB(255, 255, 255));
		mapdc.FloodFill(0, 0, RGB(255, 255, 255));
		dc.FloodFill(0, 0, RGB(255, 255, 255));
	}

	int sx, sy, cx,cy;

	int id;
 
 	for (int y = 0; y < MapSize.cy; y++)
 	{
 		for (int x = 0; x < MapSize.cx; x++)
 		{
 			id = map.GetTile(x, y).GetID();
 			cx = id % TILE_COUNT;
 			cy = id / TILE_COUNT;
 			sx = cx * TileSize.cx;
 			sy = cy * TileSize.cy;
 			
 			TileSet.BitBlt(mapdc, x * TileSize.cx, y * TileSize.cy, TileSize.cx, TileSize.cy, sx, sy);
 		}
 	}

	Invalidate(false);
	pFrame->minimap.Invalidate(0);	
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!CRect(0,0, ZoomDocSize.cx, ZoomDocSize.cy).PtInRect(point))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	int		xi = (point.x + Scroll.x) / (TileSize.cx * int(Zoom)), 
			yi = (point.y + Scroll.y) / (TileSize.cy * int(Zoom));

	switch (CurrentTool)
	{
	case TT_CHANGE:
		map.GetTile(xi, yi).SetID(CurrentTile.GetID());
        DrawMap(false);
		Invalidate(0);
		break;
		
	case TT_SELECTOR:
		pFrame->tp.SetCurrentTile(&map.GetTile(xi, yi), xi, yi);
		pFrame->tp.Invalidate(0);
		break;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON)
	{
		if (!CRect(0,0, ZoomDocSize.cx, ZoomDocSize.cy).PtInRect(point))
		{
			CWnd::OnLButtonDown(nFlags, point);
			return;
		}

		int	xi = (point.x + Scroll.x) / (TileSize.cx * int(Zoom)), 
				yi = (point.y + Scroll.y) / (TileSize.cy * int(Zoom));

		switch (CurrentTool)
		{
		case TT_CHANGE:
			map.GetTile(xi, yi).SetID(CurrentTile.GetID());
			map.GetTile(xi, yi).GetFlags()	= CurrentTile.GetFlags();
			DrawMap(false);
			Invalidate(0);
			break;
			
		case TT_SELECTOR:
			pFrame->tp.SetCurrentTile(&map.GetTile(xi, yi), xi, yi);
			pFrame->tp.Invalidate(0);
			break;
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

double Round(double value)
{
    // Get fractional part.
    double fpart = value - int(value);

    // Move decimal point five places.
    //fpart = int(fpart * 100000.0);
	fpart = int(fpart * 100.0);

    // Check if last digit if over five.
    if ( (fmod(fpart,10.0)) > 5.0 )
        // Round number up 1.
        fpart += 10.0;

    // Knock off the trailing fractions
    fpart = ((int)fpart) / 10;

    // Add new fractional part on whole part.
//    value = (int)value + fpart/10000;
	value = (int)value + fpart/100;

    return value;
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	static double	ZoomAmt	= 0.10,
					ZoomMax	= 2.00,
					ZoomMin	= 0.50;

	//////////////////////////////////////////////////////////////////////////
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	//////////////////////////////////////////////////////////////////////////

	CRect client;
	GetClientRect(&client);

	if (zDelta > 0 && Zoom < ZoomMax)
		Zoom += ZoomAmt;
	else if (zDelta < 0 && Zoom > ZoomMin)
		Zoom -= ZoomAmt;
	else
		return CWnd::OnMouseWheel(nFlags, zDelta, pt);

	Round(Zoom);

	ZoomDocSize = CSize(DocSize.cx * int(Zoom), DocSize.cy * int(Zoom));

	OnSize(SIZE_RESTORED, client.Width(), client.Height());

	Invalidate(0);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch(nSBCode)
	{
	case SB_LEFT:
		Scroll.x = 0;
		break;
	case SB_RIGHT:
		Scroll.x = MaxScroll.x;
		break;
	case SB_LINELEFT:
		Scroll.x -= TileSize.cx;
		break;
	case SB_LINERIGHT:
		Scroll.x += TileSize.cx;
		break;
	case SB_PAGELEFT:
		Scroll.x -= TileSize.cx * 10;
		break;
	case SB_PAGERIGHT:
		Scroll.x += TileSize.cx * 10;
		break;
	case SB_THUMBTRACK:
		Scroll.x = nPos;
		break;
	}

	// range check the current scroll position
	Scroll.x = min(Scroll.x, MaxScroll.x);
	Scroll.x = max(Scroll.x, 0);

	// update the thumb position
	SetScrollPos(SB_HORZ, Scroll.x);

	pFrame->minimap.Invalidate(0);
	Invalidate(0);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	MaxScroll.x = max(ZoomDocSize.cx - cx, 0);
	MaxScroll.y = max(ZoomDocSize.cy - cy, 0);

	Scroll.x = min(Scroll.x, MaxScroll.x);
	Scroll.y = min(Scroll.y, MaxScroll.y);

	SetScrollRange(SB_HORZ, 0, MaxScroll.x);
	SetScrollRange(SB_VERT, 0, MaxScroll.y);
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch(nSBCode)
	{
	case SB_TOP:
		Scroll.y = 0;
		break;
	case SB_BOTTOM:
		Scroll.y = MaxScroll.y;
		break;
	case SB_LINEUP:
		Scroll.y -= TileSize.cy;
		break;
	case SB_LINEDOWN:
		Scroll.y += TileSize.cy;
		break;
	case SB_PAGEUP:
		Scroll.y -= TileSize.cy * 10;
		break;
	case SB_PAGEDOWN:
		Scroll.y += TileSize.cy * 10;
		break;
	case SB_THUMBTRACK:
		Scroll.y = nPos;
		break;
	}

	// range check the current scroll position
	Scroll.y = min(Scroll.y, MaxScroll.y);
	Scroll.y = max(Scroll.y, 0);

	// update the thumb position
	SetScrollPos(SB_VERT, Scroll.y);

	pFrame->minimap.Invalidate(0);
	Invalidate(0);
}

void CChildView::OnFileProperties()
{
	MapProps mp;
	mp.map_width	= MapSize.cx;
	mp.map_height	= MapSize.cy;
	mp.MapName		= MapName;
	mp.MapDesc		= MapDesc;

	if (mp.DoModal() == IDOK)
	{
		MapSize = CSize(mp.map_width, mp.map_height);

		MapName = mp.MapName;
		MapDesc	= mp.MapDesc;

		map.CreateTiles(MapSize.cx, MapSize.cy);

		DocSize	= CSize(MapSize.cx * TileSize.cx, MapSize.cy * TileSize.cy);
		ZoomDocSize = DocSize;

		SetupDC();

		Zoom			= 1.00;

		CRect rect;
		GetClientRect(rect);

		OnSize(SIZE_RESTORED, rect.Width(), rect.Height());

		DrawMap();

		Invalidate(0);
	}
}

void CChildView::OnFileSaveas()
{
	CFileDialog fd(false, "*.map", "map1", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Map files (*.map)|*.map");

	if (fd.DoModal() == IDOK)
	{
		Saved++;
		FileName = fd.GetPathName();
		SaveMap(FileName);
	}
}

void CChildView::SaveMap(String path)
{
	if (!path)
		return;
}

void CChildView::LoadMap(String path)
{
	if (!path)
		return;

	map.LoadMap((const char *)path);

	DocSize	= CSize(MapSize.cx * TileSize.cx, MapSize.cy * TileSize.cy);
	ZoomDocSize = DocSize;

	SetupDC();

	CRect rect;
	GetClientRect(&rect);

	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());

	DrawMap();
	Zoom			= 1.00;

	Invalidate();
}

void CChildView::OnOpenMap()
{
	CFileDialog fd(true, "*.map", "", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Map files (*.map)|*.map");

	if (fd.DoModal() == IDOK)
	{
		LoadMap(fd.GetPathName());
	}
}

void CChildView::OnFileSave()
{
	if (Saved)
		SaveMap(FileName);
	else
		OnFileSaveas();
}

void CChildView::OnFileNew()
{
	//
	//TODO: NEW MAP
	//

// 	for (int r = 0; r < MapSize.cy; r++)
// 			delete [] Tiles[r];
// 
// 		delete [] Tiles;
	/*for (int r = 0; r < MapSize.cy; r++)
		Tiles[r].RemoveAll();
	Tiles.RemoveAll();*/

	MapSize = CSize(InitWidth, InitHeight);

	//SetMap();
	/**Tiles	=	new Tile*[MapSize.cy];
	for (int r = 0; r < MapSize.cy; r++)
		*Tiles[r] = new Tile[MapSize.cx];

	for (int y = 0; y < MapSize.cy; y++)
		for (int x = 0; x < MapSize.cx; x++)
			Tiles[y][x]		= new Tile;*/
	/*Tiles.SetSize(MapSize.cy);
	for (int r = 0; r < MapSize.cy; r++)
		Tiles[r].SetSize(MapSize.cx);
	
	for (int y = 0; y < MapSize.cy; y++)
		for (int x = 0; x < MapSize.cx; x++)
			Tiles[y][x]		= Tile();*/

	DocSize	= CSize(MapSize.cx * TileSize.cx, MapSize.cy * TileSize.cy);
	ZoomDocSize = DocSize;

	SetupDC();

	Zoom			= 1.00;

	DrawMap();

	Invalidate(0);
}

void CChildView::OnOpenCustomtileset()
{
	//MessageBox("Soon to be implemented!", "COMING SOON!");
	CFileDialog fd(true, "*.bmp", "", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Bitmap files (*.bmp)|*.bmp");

	if (fd.DoModal() == IDOK)
	{
		TileSet.Destroy();
		CurrentTileSet = fd.GetPathName();
		TileSet.Load(CurrentTileSet);
		OnFileNew();
		pFrame->palwnd.Invalidate();
	}
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	/*if (++CurrentTile > 12)
		CurrentTile = 0;*/
}

void CChildView::OnToolsSelector()
{
	CurrentTool	= TT_SELECTOR;
}

void CChildView::OnUpdateToolsSelector(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(CurrentTool == TT_SELECTOR);
}

void CChildView::OnToolsChange()
{
	CurrentTool	= TT_CHANGE;
}

void CChildView::OnUpdateToolsChange(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(CurrentTool == TT_CHANGE);
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	//return false;
	// TODO: Add your message handler code here and/or call default

	return CWnd::OnEraseBkgnd(pDC);
}
