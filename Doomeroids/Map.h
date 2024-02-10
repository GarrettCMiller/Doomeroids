///////////////////////////////////////////////////////////////////////////
//		File:				Map.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A map is the level or area you are on/in
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vec.h"
#include "Tile.h"
#include "TileSet.h"
#include "Creature.h"
#include "Monster.h"
#include "MemObj.h"
#include "2dDynamicArray.h"
#include "Terrain.h"

#include "CameraView.h"

//////////////////////////////////////////////////////////////////////////

#define CURRENT_MAP_VERSION			0x0100
#define MAX_DESCRIPTION_LENGTH		128

//////////////////////////////////////////////////////////////////////////

struct MapFileHeader
{
	// Version 1.0
	char			Name			[256];
	char			Description		[MAX_DESCRIPTION_LENGTH];
	char			DefaultTileSet	[128];

	TileSetChunk	TileSetInfo;

	UInt32			Version;
	UInt32			Width;
	UInt32			Height;
};

//////////////////////////////////////////////////////////////////////////

class Map;

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Map>		MapPtr;
#else
typedef Map*					MapPtr;
#endif

typedef C2dDynamicArray<Tile>	Tiles;

//////////////////////////////////////////////////////////////////////////

class Map
{
	char			m_strName		[64];
	char			m_strDescription[MAX_DESCRIPTION_LENGTH];

	Int32				m_nID;

	Terrain			m_Terrain;

	UInt32			m_uWidth;
	UInt32			m_uHeight;
	UInt32			m_uTileSize;
	UInt32			m_ShaderID;

	TileSet			m_TileSet;

	Tiles			m_Tiles;

	MonsterPtr		boss;

	CCameraView		m_Camera;

	D3DMesh			m_Walls;

	static Bool		m_bAutoSpawn;

	static Timer	SpawnDelay;

public:
	Map();
	~Map(void);

	Map& operator = (const Map& rhs)
	{
		CopyFrom(rhs);
		return *this;
	}

	Bool			Init(String  FileName);
	Bool			Init(UInt32 Width, UInt32 Height);
	
	Bool			Update(Float dt);
	Bool			Shutdown();

	void			LoadTileSet();
	Bool			CreateTiles(UInt32 Width, UInt32 Height);

	String 	GetName() const								{ return m_strName; }

	Int32			GetID() const						{ return m_nID; }
	void			SetID(UInt32 id)					{ m_nID = id; }

	UInt32			GetWidth() const					{ return m_uWidth; }
	UInt32			GetHeight() const					{ return m_uHeight; }

	UInt32			GetTileSize() const					{ return m_uTileSize; }
	TileSet&		GetTileSet()						{ return m_TileSet; }

	Tile&			GetTile(UInt32 x, UInt32 y)			{ return m_Tiles(x, y);	}

	Bool			LoadMap(String  FileName);

	void			SetBoss(MonsterPtr ent)				{ boss = ent;}
	MonsterPtr		GetBoss()							{ return boss; }

	void			CreateRandomBarrels();

	void			Render();

	void			RenderTile(UInt32 id, Float px, Float py);

	CCameraView&	GetCamera()							{ return m_Camera;	}

	void			SetShader(const String& tech)		
	{
		m_Terrain.SetShaderID(g_EffectMgr->GetID("Map.fx"));
		m_Terrain.SetShaderTech(tech);
	}

private:
	void			Draw();
	void			Load();

	void			CopyFrom(const Map& rhs);
};
