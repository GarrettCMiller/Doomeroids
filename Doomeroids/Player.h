/////////////////////////////////////////////////////////////////////////////
//		File:			Player.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:02 AM
//
//		Description:	A player (whether local or remote)
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "creature.h"
#include "Pickup.h"

//////////////////////////////////////////////////////////////////////////

class Player;

//////////////////////////////////////////////////////////////////////////

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Player>	PlayerPtr;
#else
typedef Player*					PlayerPtr;
#endif

//////////////////////////////////////////////////////////////////////////

class Player : public Creature
{
	static	PlayerPtr		m_pInstance;

	UInt32	Rad;
	Vec2	pos;
	Float	alpha;
	void	HandleRad();

public:
	void SetRad(Vec2 _pos);

	Player();	
	virtual ~Player(void);

	Bool Init(Vec3 pos = k_v3Zero);

	void GetInput();

	void Update(Float dt);
	void Draw();
	void Shoot();

	void HandleDeath();
	void HandleCollision(EntityPtr ent);
	void HandlePickup(PickupPtr pickup);

	void LookAtMouse();

	void HandleWeaponInput();

	void HandleMovement();

	static PlayerPtr	GetInstance()	{ return m_pInstance; }
	
	static Bool SetInstance(PlayerPtr p)	
	{ 
		if (!p || m_pInstance)
			return false;

		m_pInstance = p;

		return true;
	}

	static void DeleteInstance()
	{
		m_pInstance = NULL;
		//SAFE_DELETE(m_pInstance);
	}

	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Player", 49, "MapMgr::PreMapInit()");
	}

	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}

	Bool	bLocal;
	Bool	bIsShooting;
	
	Vec2 lookAt;
};

#define g_Player			Player::GetInstance()