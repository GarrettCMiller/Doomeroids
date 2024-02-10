/////////////////////////////////////////////////////////////////////////////
//		File:			Weapon.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:09 AM
//
//		Description:	A weapon class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vec.h"
#include "Entity.h"

//////////////////////////////////////////////////////////////////////////

static const Int32 k_MaxPossibleWeapons = 15;

typedef Int32 EWeaponType;

//////////////////////////////////////////////////////////////////////////

class Weapon
{
public:
	Timer			m_tDelayTimer;

	UInt32			m_uDamage;
	UInt32			m_uSplashDamage;
	UInt32			m_uShotCount;

	Int32				m_nHitSound;			//For bullet transfer only
	Int32				m_nFireSound;

	Float			m_fSpread;				//For scatter shot
	Float			m_fDelay;
	Float			m_fBulletSpeed;
	Float			m_fSplashRadius;

	Bool			m_bRandomScatter;

	Vec2			m_BulletSize;
	UInt32			m_BulletImage;

	UInt32			m_Type;
	static Weapon	m_Weapons[k_MaxPossibleWeapons];
	static String  WeaponNames[k_MaxPossibleWeapons];

public:
	Weapon();
	virtual ~Weapon(void);
	
	Bool Init(UInt32 xmlTableIndex);

	Bool IsReady();

	void Shoot(Vec3 pos, Vec3 target, EntityPtr parent);

	static void InitAll();

	static EWeaponType NameToType(String  name);
	static void ClearAllNames();
};
