///////////////////////////////////////////////////////////////////////////
//		File:				TileSet.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A tileset holds the tile information for a given map
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

#define MAX_TILESET_SIZE		32

//////////////////////////////////////////////////////////////////////////

struct TileSetChunk
{
	UInt32	TextureSize;			//The length of one square texture, must be power of 2
	UInt32	Entries;
	char	Textures[MAX_TILESET_SIZE][128];

	TileSetChunk& operator = (TileSetChunk tsc)
	{
		Entries = tsc.Entries;
		memcpy(Textures, tsc.Textures, MAX_TILESET_SIZE * 128);
	}
};

//////////////////////////////////////////////////////////////////////////

class TileSet
{
	UInt32		m_uEntries;
	UInt32		m_uTileSet[MAX_TILESET_SIZE];
	UInt32		m_uTextureSize;

//	LPDIRECT3DTEXTURE9	m_TextureMap;

public:
	TileSet(void);
	~TileSet(void);

	void LoadTileSet(TileSetChunk& tsc);

	Int32	GetTile(UInt32 index)	const			
	{ 
		ASSERT(index < m_uEntries);		
		return m_uTileSet[index];
	}

	UInt32 GetNumEntries() const	{ return m_uEntries; }

	UInt32 GetTextureSize() const	{ return m_uTextureSize; }
};
