/////////////////////////////////////////////////////////////////////////////
//		File:			Creature.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:48:49 AM
//
//		Description:	A creature (NEED TO RENAME) is an entity with health
//						and possbily weapons
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "entity.h"
#include "Weapon.h"
//#include "../../Quantum/Quantum/Particle.h"

//////////////////////////////////////////////////////////////////////////
static const Int32 k_MaxPossibleEntityTypes = 20;

typedef Int32 ECreatureType;

//////////////////////////////////////////////////////////////////////////

class Creature : public Entity
{
public:

	//ParticleEmitter		Blood;

	Timer				m_tDeathTimer;

	Bool				m_bWeapons[k_MaxPossibleWeapons];
	Int32				m_nCurrentWeapon;

	Int32				m_nSoundHurt, 
						m_nSoundDie,
						m_nSoundAlert;

	Int32				m_nHP;
	Int32				m_nHPMax;

	static String		TypeIdNames[k_MaxPossibleEntityTypes];
	static Int32		ms_MaxFrames[NUM_ANIMATION_STATES];
	static Int32		m_nSoundSplatter;
	static const Float	m_fCreatureScale;

	//Type of creature
	ECreatureType		m_uTypeID;

public:
	Creature();
	virtual ~Creature(void);

	Bool Init(ECreatureType type, Vec3 pos = k_v3Zero);

	void InitBlood();

	void Update(Float dt);

	//Hurts the creature, returns whether or not the creature is dead (use negative numbers to heal)
	virtual Bool Hurt(Int32 damage);

	void HandleCollision(EntityPtr ent);
	void HandleHurting(Int32 amt);
	void HandleDeath();
	void HandleBlast(Float x, Float y);

	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Creature", 49, "HERE");
	}

	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}

	static ECreatureType NameToType(String  name);
	static void ClearAllNames();
};

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Creature>		CreaturePtr;
#else
typedef Creature*					CreaturePtr;
#endif
