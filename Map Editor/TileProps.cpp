///////////////////////////////////////////////////////////////////////////////
//		File:						TileProps.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// TileProps.cpp : implementation file
//

#include "stdafx.h"
#include "Map Editor.h"
#include "TileProps.h"
#include "MainFrm.h"
#include "ChildView.h"
#include ".\tileprops.h"


// TileProps dialog

IMPLEMENT_DYNAMIC(TileProps, CDialog)
TileProps::TileProps(CWnd* pParent /*=NULL*/)
	: CDialog(TileProps::IDD, pParent)
	, tileId(0)
	, impassable(false)
	, spawn_pt(false)
	, door(false)
	, target_x(0)
	, target_y(0)
	, CurrentTile(NULL)
	, NoFlags(true)
	, Damage(false)
	, Teleporter(false)
{
}

TileProps::~TileProps()
{
}

void TileProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TILE_ID, tileId);
	DDX_Check(pDX, IDC_IMPASSABLE, impassable);
	DDX_Check(pDX, IDC_SPAWN_PT, spawn_pt);
	DDX_Check(pDX, IDC_DOOR, door);
	DDX_Text(pDX, IDC_TARGET_X, target_x);
	DDX_Text(pDX, IDC_TARGET_Y, target_y);
}


BEGIN_MESSAGE_MAP(TileProps, CDialog)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_TILE_ID, OnEnChangeTileId)
	ON_BN_CLICKED(IDC_IMPASSABLE, OnBnClickedImpassable)
	ON_BN_CLICKED(IDC_SPAWN_PT, OnBnClickedSpawnPt)
	ON_BN_CLICKED(IDC_DOOR, OnBnClickedDoor)
	ON_EN_CHANGE(IDC_TARGET_X, OnEnChangeTargetX)
	ON_EN_CHANGE(IDC_TARGET_Y, OnEnChangeTargetY)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NOFLAGS, OnBnClickedNoflags)
	ON_BN_CLICKED(IDC_DAMAGE, OnBnClickedDamage)
	ON_BN_CLICKED(IDC_TELEPORTER, OnBnClickedTeleporter)
END_MESSAGE_MAP()


// TileProps message handlers

void TileProps::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CMainFrame *parent = (CMainFrame*) m_pParentWnd;
	CChildView	*cv = &parent->m_wndView;

	if (CurrentTile)
	{
		CClientDC	prev(GetDlgItem(IDC_PREVIEW));

		int cx, cy, sx, sy, id;

		id = CurrentTile->GetID();
		cx = id % TILE_COUNT;
		cy = id / TILE_COUNT;
		sx = cx * cv->TileSize.cx + cx + 1;		//- 1;
		sy = cy * cv->TileSize.cy + cy + 1;
		
		cv->TileSet.BitBlt(prev, 7, 15, cv->TileSize.cx, cv->TileSize.cy, sx, sy);
	}
}

void TileProps::OnEnChangeTileId()
{
	UpdateTile();
}

void TileProps::OnBnClickedImpassable()
{
	UpdateData();

	if (impassable)
	{
		NoFlags = false;
		door = false;
		Teleporter = false;
	}

	UpdateTile();
}

void TileProps::OnBnClickedSpawnPt()
{
	UpdateData();

	if (spawn_pt)
	{
		NoFlags = false;
	}

	UpdateTile();
}

void TileProps::OnBnClickedDoor()
{
	UpdateData();

	if (door)
	{
		NoFlags = false;
		impassable = false;
		Teleporter = false;
	}

	UpdateTile();
}

void TileProps::OnEnChangeTargetX()
{
	UpdateTile();
}

void TileProps::OnEnChangeTargetY()
{
	UpdateTile();
}

void TileProps::OnBnClickedNoflags()
{
	UpdateData();

	if (NoFlags)
	{
		spawn_pt	= false;
		impassable	= false;
		door		= false;
		Damage		= false;
		Teleporter	= false;
	}

	UpdateTile();
}

void TileProps::OnBnClickedDamage()
{
	UpdateTile();
}

void TileProps::OnBnClickedTeleporter()
{
	UpdateTile();
}

void TileProps::OnClose()
{
	pFrame->TilePropsVisible	= false;

	CDialog::OnClose();
}

void TileProps::SetCurrentTile(Tile *tile, UINT x, UINT y)
{
	if (!tile)
		return;

	CurrentTile		= tile;
	CurrentX		= x;
	CurrentY		= y;

	char str[80];
	sprintf(str, "Coordinate: (%d, %d)", x, y);
	GetDlgItem(IDC_POS)->SetWindowText(str);

	tileId			= CurrentTile->GetID();

	NoFlags			= CurrentTile->GetFlags().TestFlag(eFlag_None);
	
	impassable		= CurrentTile->GetFlags().TestFlag(eFlag_Wall);
	spawn_pt		= CurrentTile->GetFlags().TestFlag(eFlag_SpawnPoint);
	door			= CurrentTile->GetFlags().TestFlag(eFlag_Door);
	Damage			= CurrentTile->GetFlags().TestFlag(eFlag_Damage);
	Teleporter		= CurrentTile->GetFlags().TestFlag(eFlag_Teleporter);

	UpdateData(false);

	Invalidate();
}

void TileProps::UpdateTile()
{
	UpdateData();

	if (CurrentTile)
	{
		CurrentTile->SetID(tileId);

		CurrentTile->GetFlags().SetFlag(eFlag_Wall,		impassable);
		CurrentTile->GetFlags().SetFlag(eFlag_SpawnPoint, spawn_pt);
		CurrentTile->GetFlags().SetFlag(eFlag_Door,		door);
		CurrentTile->GetFlags().SetFlag(eFlag_Damage,		Damage);
		CurrentTile->GetFlags().SetFlag(eFlag_Teleporter,	Teleporter);
	}

	pFrame->m_wndView.DrawMap(false);

	Invalidate();
}