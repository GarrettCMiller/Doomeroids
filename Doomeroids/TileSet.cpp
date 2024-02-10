///////////////////////////////////////////////////////////////////////////
//		File:				TileSet.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A tileset holds the tile information for a given map
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\tileset.h"
#include "SpriteMgr.h"

//////////////////////////////////////////////////////////////////////////

TileSet::TileSet(void)
{
	ZeroMemory(m_uTileSet, sizeof(UInt32) * MAX_TILESET_SIZE);
}

//////////////////////////////////////////////////////////////////////////

TileSet::~TileSet(void)
{
}

//////////////////////////////////////////////////////////////////////////

void TileSet::LoadTileSet(TileSetChunk& tsc)
{
#ifdef _DOOMEROIDS_
	m_uEntries = tsc.Entries;

	ZeroMemory(m_uTileSet, sizeof(UInt32) * MAX_TILESET_SIZE);

	for (UInt32 i = 0; i < tsc.Entries; i++)
	{
		m_uTileSet[i] = g_SpriteMgr->LoadSprite(tsc.Textures[i]);
	}

	m_uTextureSize = g_SpriteMgr->GetSprite(m_uTileSet[0]).width;
#endif
}

//////////////////////////////////////////////////////////////////////////