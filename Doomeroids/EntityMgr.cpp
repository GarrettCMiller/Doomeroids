/////////////////////////////////////////////////////////////////////////////
//		File:			EntityMgr.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:58 AM
//
//		Description:	A manager the stores and manipulates active entites
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "EntityMgr.h"

//#include "Globals.h"

#include "Bullet.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(EntityMgr);

static Int32 timesTotal = 0;

//////////////////////////////////////////////////////////////////////////

EntityMgr::EntityMgr(void)
{
	m_fTimeScale	= 1.0f;
	m_bPaused		= false;
	m_uNextID		= 0;

	Entity::m_svMaxVelocity = Vec2(1.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////

//Last March of the Ents... :D ;)
EntityMgr::~EntityMgr(void)
{
	RemoveAll();
}

//////////////////////////////////////////////////////////////////////////

void EntityMgr::RemoveAll()
{
	Int32 count = m_Ents.Size();

	DblLinkedList<EntityPtr >::Iterator i(m_Ents);

	for (i.Begin(); !i.End(); ++i)
		if (i.Current()->m_eClassType != CT_BULLET)
			delete i.Current();

	g_Log << "Deleted " << count << " entities from the list!\n";

	g_Log << "There were " << timesTotal << " problems removing entities...\n";

	timesTotal = 0;

	m_Ents.Clear();

	Bullet::ms_Factory.~ObjectFactory();
}

//////////////////////////////////////////////////////////////////////////

Bool EntityMgr::Add(EntityPtr ent)
{
	ASSERT(ent);

#ifdef _DEBUG
	if (ent->m_eClassType != CT_BULLET)
		ASSERT(!m_Ents.Contains(ent));
#endif

	ent->m_nID = m_uNextID++;
	
	m_Ents.Add(ent);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool EntityMgr::Remove(EntityPtr ent)
{
	ASSERT(ent);
	ASSERT(m_Ents.Contains(ent));
	m_EntsToRemove.Add(ent);
	return true;
}

//////////////////////////////////////////////////////////////////////////

EntityPtr EntityMgr::Get(UInt32 id)
{
	return m_Ents[id];
}

//////////////////////////////////////////////////////////////////////////

EntityPtr EntityMgr::GetByName(String Name)
{
	DblLinkedList<EntityPtr >::Iterator	i(m_Ents);
	EntityPtr ent = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		ent = i.Current();

		ASSERT(ent);

		if (Name == ent->m_strName)
			return ent;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

Int32 EntityMgr::Update(Float dt)
{
	//PROFILE_THIS(8);

	DblLinkedList<EntityPtr >::Iterator	i(m_Ents);
	EntityPtr ent = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		ent = i.Current();

		ASSERT(ent);
		ASSERT(!BAD_PTR(ent));
		
		{
			//PROFILE("Entity::Update()", 9);
			ent->Update(dt * m_fTimeScale);
		}
	}

	RemoveFlaggedEnts();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

void EntityMgr::RemoveFlaggedEnts()
{
	//PROFILE("EntityMgr::RemoveFlaggedEnts()");

	if (!m_EntsToRemove.IsEmpty())
	{
		EntityPtr ent = NULL;
		DblLinkedList<EntityPtr>::Iterator	del(m_EntsToRemove);

		for (del.Begin(); !del.End(); ++del)
		{
			ent = del.Current();

			ASSERT(ent);

			m_Ents.Remove(ent);

#if 0
			ASSERT(!m_Ents.Contains(ent));
#else
			while (m_Ents.Contains(ent))
			{
				timesTotal++;
				m_Ents.Remove(ent);
			}
#endif
			
			if (ent->m_eClassType != CT_BULLET)
				delete ent;
		}

		m_EntsToRemove.Clear();
	}
}

//////////////////////////////////////////////////////////////////////////