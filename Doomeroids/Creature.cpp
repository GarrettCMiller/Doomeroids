/////////////////////////////////////////////////////////////////////////////
//		File:			Creature.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:48:54 AM
//
//		Description:	A creature (NEED TO RENAME) is an entity with health
//						and possibly weapons
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Creature.h"
#include "Bullet.h"
#include "typeinfo.h"

#include "Game.h"
#include "GameXMLTables.h"

//////////////////////////////////////////////////////////////////////////

#define DEATH_DELAY		15

//////////////////////////////////////////////////////////////////////////
Int32 Creature::m_nSoundSplatter			= -1;
const Float	Creature::m_fCreatureScale	= 0.75f;

//Default max frames
Int32 Creature::ms_MaxFrames[NUM_ANIMATION_STATES] = 
{
	3,		//Walk
	1,		//Shoot
	1,		//Hurt
	4,		//Die
	8,		//Splatter
	2		//Dead
};

String Creature::TypeIdNames[k_MaxPossibleEntityTypes];

//////////////////////////////////////////////////////////////////////////

ECreatureType Creature::NameToType(String  name)
{
	for (Int32 i = 0; i < k_MaxPossibleEntityTypes; i++)
	{
		if (stricmp(name.c_str(), TypeIdNames[i].c_str()) == 0)
			return ECreatureType(i);
	}

	ASSERT(false);
	return -1;
}

//////////////////////////////////////////////////////////////////////////

void Creature::ClearAllNames()
{
	for (Int32 i = 0; i < k_MaxPossibleWeapons; i++)
	{
		TypeIdNames[i].clear();
	}
}

//////////////////////////////////////////////////////////////////////////

Creature::Creature() : Entity()
{
	m_eClassType = CT_CREATURE;
}

//////////////////////////////////////////////////////////////////////////

Creature::~Creature(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool Creature::Init(ECreatureType type, Vec3 pos /* = k_v3Zero */)
{
	if (!Entity::Init(pos))
		return false;

	XMLEntity& ent		= g_EntityTable[type];

	m_uTypeID			= type;

	//Weapons
	memcpy(m_bWeapons,	ent.weapons, sizeof(Bool) * k_MaxPossibleWeapons);
	m_nCurrentWeapon	= ent.currentweapon;

    m_nHPMax			= ent.maxhp;
	m_nHP				= (ent.hp < ent.maxhp ? ent.hp : ent.maxhp);

	m_fScale			= m_fCreatureScale;

	m_nSoundHurt		= ent.hurtsound;
	m_nSoundDie			= ent.diesound;
	m_nSoundAlert		= ent.alertsound;

	m_vAcceleration		= ms_RunSpeed;

	if (ent.meshID >= 0)
		InitMesh(ent.meshID);

	InitBlood();

	//Unnecessary?
	/*char number[16];
	itoa(m_suNextID, number, 10);
	m_strName = "Creature";
	m_strName += number;*/

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Creature::InitBlood()
{
#if 0
	Blood.Active		= false;
	Blood.NumParticles	= 125;

	Blood.Base.Pos		= m_vPosition;
	Blood.Base.Vel		= Vec2(0.1f, 0.1f);
	Blood.Base.Life		= 35;

	Blood.Base.Red		= 255;
	Blood.Base.Green	= 0;
	Blood.Base.Blue		= 0;

	Blood.PointSprites	= false;
	//Blood.InitEmitter();
#endif
}

//////////////////////////////////////////////////////////////////////////

void Creature::HandleCollision(EntityPtr ent)
{
	if (!m_bValid)
		return;

	BulletPtr bullet	= NULL;
	Float angle			= 0.0f;

	Vec2	p1 = Vec2(m_vPosition.x, m_vPosition.z),
			p2 = Vec2(ent->m_vPosition.x, ent->m_vPosition.z);

	ASSERT(ent);

	switch (ent->m_eClassType)
	{
	case CT_BULLET:
		{
			bullet = static_cast<BulletPtr>(ent);

			//If a monster of the same type as me shot the bullet, ignore it (Doom tradition)
			//UNLESS it is a true bullet (pistol, shotty, and chaingun)
			if (static_cast<CreaturePtr>(bullet->m_pParentEntity)->m_uTypeID == m_uTypeID)
				return;

			//if (m_eClassType == CT_MONSTER)
			//	(static_cast<MonsterPtr >(this))->hitcount++;
		
			angle = AngleBetween(p1, p2);

			/*if (!Blood.Active)
			{
				Blood.SetActive(75);
				Blood.InitParticles();
				Blood.Base.Vel.SetSpeedDir(0.75f, angle + 180.0f);
			}*/

			m_bBlasted		= (g_WeaponTable[bullet->m_eType].splashdmg > 0);
			m_pTargetEntity = bullet->m_pParentEntity;

			if (m_bBlasted)
				bullet->DoSplashDamage();

			if (Hurt(g_WeaponTable[bullet->m_eType].damage) && bullet->m_pParentEntity == g_Player && !m_bBlasted)
				g_Game->Kills++;
		}
		break;

	case CT_PROP:
	case CT_CREATURE:
	case CT_PLAYER:
	case CT_MONSTER:
		{
			if (m_eCurrentAnimState >= EAS_DIE || m_eState == ES_DEAD || m_nHP <= 0)
				break;

			if (m_eClassType == CT_PLAYER)
				break;

			angle = AngleBetween(p1, p2);

			Vec3 offset;
			offset.SetSpeedDir(0.1f, angle, 90.0f);
			
			/*if (m_vVelocity.MagnitudeSquared() > 0.1f)
				m_vVelocity -= offset;*/

			m_vPosition -= offset;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

Bool Creature::Hurt(Int32 damage)
{
	if (m_eClassType == CT_PLAYER && g_Game->m_Cheats[eCheat_GodMode] && damage > 0)
		return false;

	ASSERT(m_eClassType != CT_PROP);
    		
	m_nHP -= damage;

	if (m_nHP <= 0)
	{
		m_bValid = false;
		
		//if (!m_bBlasted)
			SendMessage(EM_DIE);

#if 0
		Blood.Base.Life = 150;
		Blood.SetActive(0, false);
		Blood.InitParticles();	
#endif // 0
		
		m_tDeathTimer.Init();

		return true;
	}
	else if (m_nHP > m_nHPMax)
		m_nHP = m_nHPMax;

	if (damage > 0)
	{
		g_SoundMgr->PlaySound(m_nSoundHurt);
		m_eCurrentAnimState = EAS_HURT;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void Creature::HandleHurting(Int32 amt)
{
	Hurt(amt);
}

//////////////////////////////////////////////////////////////////////////

void Creature::Update(Float dt)
{
	if (m_eState == ES_DEAD && m_tDeathTimer.Get() > DEATH_DELAY)
		m_bDestroy = true;

	/*Blood.Base.Pos = m_vPosition;

	if (Blood.Active)
		Blood.Handle();*/

	Entity::Update(dt);
}

//////////////////////////////////////////////////////////////////////////

void Creature::HandleDeath()
{
	m_nHP		= 0;
	m_eState	= ES_DEAD;
	
	SetAnimation(EAS_DIE);

	g_SoundMgr->PlaySound(m_nSoundDie);
}

//////////////////////////////////////////////////////////////////////////

void Creature::HandleBlast(Float x, Float y)
{
	ASSERT(m_nHP <= 0);

	SetAnimation(EAS_SPLATTER);

	//m_nHP		= 0;
	//m_eState	= ES_DEAD;

	g_SoundMgr->PlaySound(m_nSoundSplatter);

	Float angle = AngleBetween(m_vPosition.x, m_vPosition.z, x, y);

	m_vVelocity.SetSpeedDir(15.0f, angle + 180.0f, 90.0f);
}

//////////////////////////////////////////////////////////////////////////