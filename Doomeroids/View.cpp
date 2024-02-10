///////////////////////////////////////////////////////////////////////////
//		File:				View.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Not sure if I'm using this still or not
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\view.h"
#include "MapMgr.h"
#include "Map.h"
#include "Player.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(View);

//////////////////////////////////////////////////////////////////////////

View::View(void)
{
	m_pTarget = NULL;
}

//////////////////////////////////////////////////////////////////////////

View::~View(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 View::Update()
{
	//m_Rect.SetCenter(Vec2(g_Player->m_vPosition.x, g_Player->m_vPosition.z));

	Map& map = g_MapMgr->GetCurrent();

	if (m_Rect.tlx < 0)
		m_Rect.MoveTo(abs(m_Rect.tlx), 0, abs(m_Rect.tlx), 0);
	
	if (m_Rect.brx > map.GetWidth() * map.GetTileSize())
		m_Rect.MoveTo(map.GetWidth() - m_Rect.brx, 0, map.GetWidth() - m_Rect.brx, 0);

	if (m_Rect.tly < 0)
		m_Rect.MoveTo(0, abs(m_Rect.tly), 0, abs(m_Rect.tly));
	
	if (m_Rect.bry > map.GetHeight() * map.GetTileSize())
		m_Rect.MoveTo(0, map.GetHeight() - m_Rect.bry, 0, map.GetHeight() - m_Rect.bry);

	return 1;
}

//////////////////////////////////////////////////////////////////////////