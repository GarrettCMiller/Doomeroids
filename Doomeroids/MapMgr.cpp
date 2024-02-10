///////////////////////////////////////////////////////////////////////////
//		File:				MapMgr.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that handles maps and stuff...?
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\mapmgr.h"
#include "EntityMgr.h"
#include "View.h"
#include "HUD.h"
#include "Player.h"
#include "Game.h"
#include "DPlay.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(MapMgr);

//////////////////////////////////////////////////////////////////////////

MapMgr::MapMgr(void)
{
	m_uNextID = 0;
}

//////////////////////////////////////////////////////////////////////////

MapMgr::~MapMgr(void)
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////

Int32 MapMgr::Init()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 MapMgr::Update(Float dt)
{
	//PROFILE("MapMgr::Update()");

	Map& map = GetCurrent();

	return map.Update(dt);
}

//////////////////////////////////////////////////////////////////////////

Int32 MapMgr::Shutdown()
{
	DblLinkedList<Map>::Iterator i(m_Maps);

	for (i.Begin(); !i.End(); ++i)
	{
		i.Current().Shutdown();
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Bool MapMgr::Add(Map map)
{
	map.SetID(m_uNextID++);
	
	m_Maps.Add(map);

	m_CurrentMap = DblLinkedList<Map>::Iterator(m_Maps);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Map& MapMgr::Get(UInt32 uMapID)
{
	return m_Maps[uMapID];
}

//////////////////////////////////////////////////////////////////////////

Map& MapMgr::Get(String  strName)
{
	DblLinkedList<Map>::Iterator	i(m_Maps);

	for (i.Begin(); !i.End(); ++i)
		if (!strcmp(strName.c_str(), i.Current().GetName().c_str()))
			return i.Current();

	//Shouldn't be here
	ASSERT(false);

	return i.Current();
}

//////////////////////////////////////////////////////////////////////////

void MapMgr::RemoveAll()
{
	m_Maps.Clear();
}

//////////////////////////////////////////////////////////////////////////

Bool MapMgr::PreMapInit()
{
	View*	view	= g_View;

	ASSERT(view);

	view->SetRect(0, 0, g_D3D->GetScreenWidth(), g_D3D->GetScreenHeight());

	if (!g_Game->IsNetworked())
	{
		//
		//Precache all monster images
		//
		for (Int32 i = 0; i < k_MaxPossibleEntityTypes; i++)
			Monster::Spawn(ECreatureType(i));

		//
		//Precache all pickup/item images
		//
		for (Int32 i = 0; i < ePickup_Max; i++)
			Pickup::CreatePickup(k_v2Zero, EPickupType(i));

		g_EntityMgr->RemoveAll();

		GetCurrent().SetBoss(NULL);

		if (!InitPlayer())
			return false;

		if (g_HUD->Init() < 0)
			return false;
	}

	//g_MemMgr->Dump("Post Map Loading.html");

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool MapMgr::PostMapShutdown()
{
	g_EntityMgr->RemoveAll();
	Player::DeleteInstance();
	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool MapMgr::InitPlayer(Bool bLocal, UInt32 playerID)
{
	PlayerPtr player = g_Player;
	
	if (!player)
	{
		player = new Player;

		ASSERT(player);

		if (bLocal)
			Player::SetInstance(player);
		else
			player->bLocal = false;
	
		if (g_Game->IsNetworked())
			g_DPlay->m_PlayerList[playerID] = player;
	}

	player->Init(Vec3(0.0f, 0.0f, 0.0f));

	g_EntityMgr->Add(player);

	return true;
}

//////////////////////////////////////////////////////////////////////////