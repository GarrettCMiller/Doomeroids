/////////////////////////////////////////////////////////////////////////////
//		File:			EntityMgr.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:54 AM
//
//		Description:	A manager that stores and manipulates active entities
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "stdafx.h"

#include "Singleton.h"

#include "Entity.h"

#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////

class EntityMgr : public Singleton<EntityMgr>
{
	UInt32		m_uNextID;
	Bool		m_bPaused;
	Float		m_fTimeScale;

public:
	EntityMgr(void);
	virtual ~EntityMgr(void);

	DblLinkedList<EntityPtr >	m_Ents;
	DblLinkedList<EntityPtr >	m_EntsToRemove;
	DblLinkedList<EntityPtr >	m_EntsRemoved;

	Bool		Add(EntityPtr ent);
	Bool		Remove(EntityPtr ent);

	EntityPtr	Get(UInt32 id);
	EntityPtr	GetByName(String Name);

	Bool		IsManaged(EntityPtr ent)		{	return m_Ents.Contains(ent);	}

	Int32		Update(Float dt);

	void		Pause()							{ m_bPaused = true; }
	void		UnPause()						{ m_bPaused = false; }
	Bool		IsPaused()	const				{ return m_bPaused; }

	void		RemoveAll();
	void		RemoveFlaggedEnts();

	Float		GetTimeScale() const			{ return m_fTimeScale;	}
	void		SetTimeScale(Float timeScale)
	{
		ASSERT(timeScale >= 0.0f);
		m_fTimeScale = timeScale;
	}
};

#define g_EntityMgr EntityMgr::GetInstance()