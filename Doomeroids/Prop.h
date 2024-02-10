///////////////////////////////////////////////////////////////////////////
//		File:				Prop.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A basic prop class (for semi-static objects)
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Creature.h"

//////////////////////////////////////////////////////////////////////////

enum EPropType
{
	eProp_Barrel,
};

//////////////////////////////////////////////////////////////////////////

class Prop : public Creature
{
	Bool				m_bDynamic;
	EPropType			m_ePropType;

	EntityPtr			m_pAggressor;

	static const Int32	m_nDamage		= 200;
	static const Int32	m_nSplashRange	= 125;

public:
	Prop();
	virtual ~Prop(void);

	Int32 Init(EPropType ept, Vec3 pos, Bool bDynamic = false);

	void Draw();

	void HandleCollision(EntityPtr ent);
	void HandleBlast(Float x, Float y);
	void HandleAnimation(Float dt);

	Bool Hurt(Int32 damage);

	void DoSplashDamage();

	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Prop", 49, "HERE");
	}

	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};
