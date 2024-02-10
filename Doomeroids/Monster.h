/////////////////////////////////////////////////////////////////////////////
//		File:			Monster.h
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:48:21 AM
//
//		Description:	A monster/bad guy/enemy/etc...	
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "creature.h"

//////////////////////////////////////////////////////////////////////////

class Monster : public Creature
{
	static const Int32	MaxMonsters		= 50;
	static Int32		MonsterCount;

	static const Float	MinAtkDelay;
	static const Int32	AtkChance		= 400;	//One in four hundred chance of attacking when ready

	static const Float	MinActionDelay;
	static const Int32	NewActionChance	= 200;

	static const Float	MinDist;				//20 units
	static const Float	MinDistSqd;				//20 units squared

public:
	Monster();

	virtual ~Monster(void);

	Bool Init(ECreatureType id, Vec3 pos = k_v3Zero);

	void Update(Float dt);

	Bool Hurt(Int32 damage);

	void HandleDeath();

	void DoAI(Float dt);

	Timer		m_tAttackTimer;
	Timer		m_tActionTimer;

	//
	//AI functions
	//
	void Attack(Float dt);
	void Follow(Float dt);
	void Move(Float dt);

    UInt32	hitcount;

	static Bool CanSpawn() { return MonsterCount < MaxMonsters; }

	static void Spawn(ECreatureType type, Bool bRandom = true, Vec2 pos = k_v2Zero);
	static void	Spawn(String type, Bool bRandom = true, Vec2 pos = k_v2Zero);

	//////////////////////////////////////////////////////////////////////////
	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Monster", 49, "HERE");
	}

	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Monster>	MonsterPtr;
#else
typedef Monster*				MonsterPtr;
#endif