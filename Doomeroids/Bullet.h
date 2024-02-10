/////////////////////////////////////////////////////////////////////////////
//		File:			Bullet.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:48:58 AM
//
//		Description:	A bullet (projectile) class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Entity.h"
#include "Weapon.h"
#include "ObjectFactory.h"

//////////////////////////////////////////////////////////////////////////

class Bullet;

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Bullet>		BulletPtr;
#else
typedef Bullet*						BulletPtr;
#endif

//////////////////////////////////////////////////////////////////////////

class Bullet : public Entity
{
	void HandleCollision(EntityPtr ent);
	void DoSplashDamage();

	friend class Creature;

public:
	Bullet();
	virtual ~Bullet(void);

	Bool Init(EWeaponType type, EntityPtr parent, Vec3 pos);

	void Update(Float dt);
	void Draw();
	
	void Remove();
	
	EWeaponType				m_eType;

	typedef ObjectFactory<Bullet, BulletPtr>	BulletFactory;

	static BulletFactory	ms_Factory;
	static BulletPtr		Create();

	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Bullet", 49, "HERE");
	}

	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}
};