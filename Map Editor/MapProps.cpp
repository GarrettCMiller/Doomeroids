///////////////////////////////////////////////////////////////////////////////
//		File:						MapProps.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
// MapProps.cpp : implementation file
//

#include "stdafx.h"
#include "Map Editor.h"
#include "MapProps.h"
#include "MainFrm.h"

// MapProps dialog

IMPLEMENT_DYNAMIC(MapProps, CDialog)

MapProps::MapProps(CWnd* pParent /*=NULL*/)
	: CDialog(MapProps::IDD, pParent)
	, map_width(0)
	, map_height(0)
	, MapName(_T(""))
	, MapDesc(_T(""))
{
}

MapProps::~MapProps()
{
}

void MapProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WIDTH, map_width);
	DDV_MinMaxUInt(pDX, map_width, 0, 10000);
	DDX_Text(pDX, IDC_HEIGHT, map_height);
	DDV_MinMaxUInt(pDX, map_height, 0, 10000);
	DDX_Text(pDX, IDC_MAP_NAME, MapName);
	DDV_MaxChars(pDX, MapName, 64);
	DDX_Text(pDX, IDC_DESC, MapDesc);
	DDV_MaxChars(pDX, MapDesc, 256);
}


BEGIN_MESSAGE_MAP(MapProps, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// MapProps message handlers

void MapProps::OnBnClickedOk()
{
	UpdateData();

	if (map_width > 50 || map_height > 50)
	{
		if (MessageBox("One or more of your map dimensions is ridiculously high...\nDo you still want me to try and make it?", "Dimension Warning", MB_YESNO) == IDYES)
			OnOK();
	}
	else
		OnOK();
}
