///////////////////////////////////////////////////////////////////////////////
//		File:						TileProps.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ChildView.h"
#include "Tile.h"
// TileProps dialog

class TileProps : public CDialog
{
	DECLARE_DYNAMIC(TileProps)

public:
	TileProps(CWnd* pParent = NULL);   // standard constructor
	virtual ~TileProps();

	void SetParent(CWnd *parent)
	{
		m_pParentWnd	= parent;
	}

	Tile		*CurrentTile;
	UINT		CurrentX,
				CurrentY;

	void SetCurrentTile(Tile *tile, UINT x, UINT y);
	void UpdateTile();

// Dialog Data
	enum { IDD = IDD_TILE_PROPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	UINT tileId;
	BOOL impassable;
	BOOL spawn_pt;
	BOOL door;
	UINT target_x;
	UINT target_y;
	afx_msg void OnEnChangeTileId();
	afx_msg void OnBnClickedImpassable();
	afx_msg void OnBnClickedSpawnPt();
	afx_msg void OnBnClickedDoor();
	afx_msg void OnEnChangeTargetX();
	afx_msg void OnEnChangeTargetY();
	afx_msg void OnClose();
	bool NoFlags;
	bool Damage;
	bool Teleporter;
	afx_msg void OnBnClickedNoflags();
	afx_msg void OnBnClickedDamage();
	afx_msg void OnBnClickedTeleporter();
};
