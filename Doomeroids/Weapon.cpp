/////////////////////////////////////////////////////////////////////////////
//		File:			Weapon.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:13 AM
//
//		Description:	A general weapon class
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Weapon.h"

//#include "Globals.h"

#include "Bullet.h"
#include "DirectInput.h"
#include "EntityMgr.h"
#include "SoundMgr.h"
#include "MiscMath.h"
#include "GameXMLTables.h"
//#include "ProfileSample.h"

//////////////////////////////////////////////////////////////////////////

String		Weapon::WeaponNames[k_MaxPossibleWeapons];
Weapon		Weapon::m_Weapons[k_MaxPossibleWeapons];

//////////////////////////////////////////////////////////////////////////

EWeaponType Weapon::NameToType(String  name)
{
	for (Int32 i = 0; i < k_MaxPossibleWeapons; i++)
	{
		if (stricmp(name.c_str(), WeaponNames[i].c_str()) == 0)
			return EWeaponType(i);
	}

	ASSERT(false);
	return -1;
}

//////////////////////////////////////////////////////////////////////////

void Weapon::ClearAllNames()
{
	for (Int32 i = 0; i < k_MaxPossibleWeapons; i++)
	{
		WeaponNames[i].clear();
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Weapon::Weapon()
{
}

//////////////////////////////////////////////////////////////////////////

Weapon::~Weapon(void)
{
}

//////////////////////////////////////////////////////////////////////////

void Weapon::InitAll()
{
	g_Log << "Initializing weapon data...\n";

	for (Int32 i = 0; i < k_MaxPossibleWeapons; i++)
		m_Weapons[i].Init(i);

	g_Log << "All weapons initialized!\n";
}

//////////////////////////////////////////////////////////////////////////

Bool Weapon::Init(UInt32 xmlTableIndex)
{
	ASSERT(xmlTableIndex < k_MaxPossibleWeapons);

	XMLWeapon&	wpn = g_WeaponTable[xmlTableIndex];

	if (!wpn.IsValid)
		return false;

	g_Log << "Initializing " << wpn.name << "...\n";

	m_Type				= xmlTableIndex;

	m_uDamage			= wpn.damage;
	m_fDelay			= wpn.delay;
	m_uSplashDamage		= wpn.splashdmg;
	m_fSplashRadius		= wpn.splashradius;
	m_uShotCount		= wpn.shotcount;
	m_fSpread			= wpn.spread;
	m_fBulletSpeed		= wpn.speed;
	m_nHitSound			= wpn.hitsound;
	m_nFireSound		= wpn.firesound;
	m_BulletImage		= wpn.bulletImage;
	m_BulletSize		= wpn.bulletSize;

	m_tDelayTimer.Init();

	return true;
}

//////////////////////////////////////////////////////////////////////////
				
Bool Weapon::IsReady()
{
	return (m_tDelayTimer.Get() >= m_fDelay);
}

//////////////////////////////////////////////////////////////////////////

void Weapon::Shoot(Vec3 pos, Vec3 target, EntityPtr parent)
{
	m_tDelayTimer.Reset();

	g_SoundMgr->PlaySound(m_nFireSound);

	ASSERT(parent);

	BulletPtr	b = NULL;
	Float		angle = 0.0f;

	for (UInt32 i = 0; i < m_uShotCount; i++)
	{
		b = Bullet::Create();
		ASSERT(b);

		b->Init(m_Type, parent, pos);

		angle = AngleBetween(pos.x, pos.z, target.x, target.y);

		const CCameraView& cam = g_D3D->GetCamera();

		angle += fabs(cam.GetDirection() - 270.0f);

		if (m_fSpread)
			angle += Random(m_fSpread /*+ fabsf(randomness)*/, -m_fSpread /*+ -fabsf(randomness)*/);

		b->m_vVelocity.SetSpeedDir(m_fBulletSpeed, angle, 90.0f);
		b->m_vVelocity.y = 0.0f;

		g_EntityMgr->Add(b);
	}
}

//////////////////////////////////////////////////////////////////////////