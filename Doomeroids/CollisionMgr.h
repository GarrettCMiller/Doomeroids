///////////////////////////////////////////////////////////////////////////
//		File:				CollisionMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that handles collision detection
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "HashTable.h"
#include "Singleton.h"
#include "Entity.h"

//////////////////////////////////////////////////////////////////////////

class CCollisionMgr : public Singleton<CCollisionMgr>
{
	CHashTable<EntityPtr>	m_Table;

public:
	CCollisionMgr(void);
	~CCollisionMgr(void);

	Bool Init();
	Bool Update(Float dt);
	Bool Shutdown();

	void Add(EntityPtr ent);

private:
	void	CheckList(Int32 list);
	static Int32 HashFunction(EntityPtr& ent);
};

#define g_CollisionMgr		CCollisionMgr::GetInstance()