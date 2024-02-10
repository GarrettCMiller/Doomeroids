///////////////////////////////////////////////////////////////////////////
//		File:				MapMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that handles maps and stuff...?
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include "LList.h"

#include "Map.h"

//////////////////////////////////////////////////////////////////////////

class MapMgr :	public Singleton<MapMgr>
{
	DblLinkedList<Map>				m_Maps;
	DblLinkedList<Map>::Iterator	m_CurrentMap;

	UInt32							m_uNextID;
	Bool							m_bPaused;
public:
	MapMgr(void);
	virtual ~MapMgr(void);

	Bool	Add(Map map);

	Map&	Get(UInt32 uMapID);
	Map&	Get(String  strName);

	Int32		Init();
	Int32		Update(Float dt);
	Int32		Shutdown();

	Map&	GetCurrent()		{ m_CurrentMap.Begin();	return m_CurrentMap.Current(); }

	Bool	PreMapInit();
	Bool	PostMapShutdown();

	void	RemoveAll();

	Bool	InitPlayer(Bool bLocal = true, UInt32 playerID = 0);
};

#define g_MapMgr		MapMgr::GetInstance()