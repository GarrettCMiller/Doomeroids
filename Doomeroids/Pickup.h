///////////////////////////////////////////////////////////////////////////
//		File:				Pickup.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A basic pickup/powerup
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "entity.h"

//////////////////////////////////////////////////////////////////////////

enum EPickupType
{
	ePickup_HealthPackSmall,
	ePickup_HealthPackLarge,
	//ePickup_

	ePickup_Max
};

//////////////////////////////////////////////////////////////////////////

class Pickup : public Entity
{
public:
	static const Int32 HealthPackLarge_Amt = 25;
	static const Int32 HealthPackSmall_Amt = 10;

	EPickupType		m_ePickupType;

public:
	Pickup();
	~Pickup(void);

	Bool Init(EPickupType type = ePickup_HealthPackLarge, Vec3 pos = k_v3Zero);
	
	static void CreatePickup(Vec2 pos, EPickupType type = ePickup_HealthPackSmall);
	static void CreateRandomPickup(Vec2 pos);

	void Update(Float dt);

	void HandleCollision(EntityPtr ent);

	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Prop", 69, "New");
	}

	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Pickup>	PickupPtr;
#else
typedef Pickup*					PickupPtr;
#endif
