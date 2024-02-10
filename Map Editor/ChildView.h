///////////////////////////////////////////////////////////////////////////////
//		File:						ChildView.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// ChildView.h : interface of the CChildView class
//
#pragma once
#include <cmath>
#include <afxtempl.h>
#include <atlimage.h>

#include "Flag.h"

#include "Tile.h"
#include "TileSet.h"
#include "Map.h"


////Version 5.00
//struct DoorTag
//{
//	explicit DoorTag(const char *tmap = "", UINT tx = 0, UINT ty = 0) : Tx(tx), Ty(ty)
//	{
//		strncpy(TargetMap, tmap, DEF_TSET_LEN);
//	}
//
//	char		TargetMap[DEF_TSET_LEN];
//	UINT	Tx, Ty;
//};
/////////////////////////////////////////////////////////

// CChildView window
class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	bool		DC_Created;
	bool		Saved;

	CImage		TileSet;
	
	String		FileName;
	String		CurrentTileSet;

	CBitmap		bitmap;
	CDC			mapdc;

	enum TILE_TOOL 
	{ 
		TT_SELECTOR, TT_CHANGE
	};

	//Map vars
	TILE_TOOL			CurrentTool;
	Tile				CurrentTile;
	CSize				TileSize;
	static const int	InitWidth		= 15,
						InitHeight		= 15,
						NumTiles		= 6;
	
	Map			map;
	
	//CArray<DoorTag>	doortags;

	CSize		MapSize;

	String		MapName,
				MapDesc;

	CPoint	Scroll;
	CPoint	MaxScroll;
	CSize	DocSize;
	CSize	ZoomDocSize;
	double	Zoom;

// Operations
public:
	void DrawMap(bool erase = true);
	void SaveMap(String path);
	void LoadMap(String path);

	void SetupDC();

// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFileProperties();
	afx_msg void OnFileSaveas();
	afx_msg void OnOpenMap();
	afx_msg void OnFileSave();
	afx_msg void OnFileNew();
	afx_msg void OnOpenCustomtileset();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnToolsSelector();
	afx_msg void OnUpdateToolsSelector(CCmdUI *pCmdUI);
	afx_msg void OnToolsChange();
	afx_msg void OnUpdateToolsChange(CCmdUI *pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

double Round(double value);

/////////////////////////////////////////