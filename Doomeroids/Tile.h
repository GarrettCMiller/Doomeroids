///////////////////////////////////////////////////////////////////////////
//		File:				Tile.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A tile is, well, a tile...?
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Flag.h"

//////////////////////////////////////////////////////////////////////////

enum			EFlags
{
	eFlag_None				/*= 0*/,
	eFlag_Wall				/*= 1*/,
	eFlag_Door				/*= 2*/,
	eFlag_Teleporter		/*= 4*/,
	eFlag_Damage			/*= 8*/,
	eFlag_SpawnPoint,

	eFlag_Max,

	eFlag_Default			= eFlag_None
};

//////////////////////////////////////////////////////////////////////////

class Tile
{
public:
	UInt32					m_uID;
	Flag<EFlags>			m_fFlags;

public:
	explicit		Tile(UInt32 Id = 0,  EFlags flags = eFlag_Default) : m_uID(Id)
	{
		m_fFlags.SetFlag(flags);
	}

					~Tile(void);

	Flag<EFlags>&	GetFlags()					{ return m_fFlags; }
	UInt32			GetID() const			{ return m_uID; }

	void			SetID(UInt32 id)		{ m_uID = id; }

};
