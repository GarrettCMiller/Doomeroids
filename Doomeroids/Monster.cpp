/////////////////////////////////////////////////////////////////////////////
//		File:			Monster.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:48:26 AM
//
//		Description:	A monster/bad guy/enemy/etc...
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\monster.h"
#include "Game.h"
#include "Map.h"
#include "EntityMgr.h"
#include "MapMgr.h"
#include "GameXMLTables.h"

//////////////////////////////////////////////////////////////////////////

Int32 Monster::MonsterCount = 0;

const Float Monster::MinAtkDelay	= 2.0f;			//Two second minimum delay
const Float Monster::MinDist		= 2.0f;
const Float Monster::MinDistSqd		= 4.0f;
const Float Monster::MinActionDelay = 1.0f;			//Every second at most

//////////////////////////////////////////////////////////////////////////

Monster::Monster() : Creature()
{
	m_eClassType = CT_MONSTER;

	hitcount = 0;
	MonsterCount++;
}

//////////////////////////////////////////////////////////////////////////

Monster::~Monster(void)
{
	MonsterCount--;
}

//////////////////////////////////////////////////////////////////////////

Bool Monster::Init(ECreatureType id, Vec3 pos /* = k_v3Zero */)
{
	if (!Creature::Init(id, pos))
		return false;

	m_eState = ES_ATTACK;
	m_tAttackTimer.Init();

	char number[16];
	itoa(m_suNextID, number, 10);
	m_strName = "Monster";
	m_strName += number;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Monster::Hurt(Int32 damage)
{
	return Creature::Hurt(damage);
}

//////////////////////////////////////////////////////////////////////////

void Monster::HandleDeath()
{
	if (Random(25) == 0)
		Pickup::CreateRandomPickup(m_vPosition);

	Creature::HandleDeath();
}

//////////////////////////////////////////////////////////////////////////

void Monster::Update(Float dt)
{
	if (m_bValid && m_eState < ES_DEAD && dt > 0.0f)
		DoAI(dt);

	Creature::Update(dt);
}

//////////////////////////////////////////////////////////////////////////

void Monster::DoAI(Float dt)
{
	if (m_pTargetEntity && static_cast<CreaturePtr >((EntityPtr) m_pTargetEntity)->m_eState == ES_DEAD)
	{
		m_pTargetEntity = g_Player;

		if (!m_pTargetEntity || g_Player->m_eState == ES_DEAD)
			m_eState = ES_IDLE;
	}

	/*if (m_tActionTimer >= MinActionDelay && Random(NewActionChance) == NewActionChance)
	{
		m_tActionTimer.Reset();


	}*/

	switch (m_eState)
	{
	case ES_MOVE:
		Move(dt);
		break;

	case ES_ATTACK:
		Attack(dt);
	case ES_FOLLOW:
		Follow(dt);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void Monster::Follow(Float dt)
{
	if (!m_pTargetEntity)
		return;

	LookAt(m_pTargetEntity->m_vPosition.x, m_pTargetEntity->m_vPosition.z);

	Float dist = DistanceSquared<Float>(m_vPosition, m_pTargetEntity->m_vPosition);

	if (dist <= MinDistSqd)
		return;

	Float		x = m_vPosition.x,
				y = m_vPosition.z,
				tx = m_pTargetEntity->m_vPosition.x,
				ty = m_pTargetEntity->m_vPosition.z;

	if (x > tx + MinDist)
		m_vVelocity.x -= m_vAcceleration.x;
	else if (x < tx - MinDist)
		m_vVelocity.x += m_vAcceleration.x;

	if (y > ty + MinDist)
		m_vVelocity.z -= m_vAcceleration.z;
	else if (y < ty - MinDist)
		m_vVelocity.z += m_vAcceleration.z;
}

//////////////////////////////////////////////////////////////////////////

void Monster::Attack(Float dt)
{
	if (!m_pTargetEntity)
		return;

	/*Float dist = DistanceSquared<Float>(m_vPosition, m_pTargetEntity->m_vPosition);

	if (dist >= MinDistSqd)
		return;*/

	if (m_nCurrentWeapon >= 0 && m_tAttackTimer.Get() >= MinAtkDelay && Random(AtkChance) == AtkChance)
	{
		m_tAttackTimer.Reset();

		//Stop the enemy to "aim" and shoot
		m_vVelocity			= k_v2Zero;
		m_eCurrentAnimState	= EAS_SHOOT;
		m_vTargetPosition	= m_pTargetEntity->m_vPosition;

		Weapon::m_Weapons[m_nCurrentWeapon].Shoot(m_vPosition, m_vTargetPosition, this);
	}
}

//////////////////////////////////////////////////////////////////////////

void Monster::Move(Float dt)
{

}

//////////////////////////////////////////////////////////////////////////

void Monster::Spawn(ECreatureType type, Bool bRandom, Vec2 pos)
{
	//if (!Monster::CanSpawn())
	//	return;

	if (!g_EntityTable[type].IsValid)
		return;

	if (bRandom)
	{
		switch (Random(Int32(2), Int32(0)))
		{
		case 0:	//Top
			pos.X = 25.0f + Random(10.0f, -10.0f);
			pos.Y = 40.0f + Random(10.0f, -10.0f);
			break;

		case 1:	//Right
			pos.X = 50.0f + Random(10.0f, -10.0f);	
			pos.Y = 0.0f + Random(10.0f, -10.0f);
			break;

		case 2:	//Bottom
			pos.X = 25.0f + Random(10.0f, -10.0f);
			pos.Y = -40.0f + Random(10.0f, -10.0f);
			break;

		case 3:	//Left
			pos.X = -60.0f + Random(10.0f, -10.0f);
			pos.Y = 0.0f + Random(10.0f, -10.0f);
			break;
		}
	}

	MonsterPtr m = new Monster;
	ASSERT(m);

	m->Init(type, Vec3(pos.X, 0.0f, pos.Y));

	m->m_vAcceleration = ms_RunSpeed * 0.25f;

	if (type == NameToType("cyberdemon"))
	{
		Map& map = g_MapMgr->GetCurrent();
		map.SetBoss(m);
	}

	m->m_pTargetEntity = g_Player;

	g_EntityMgr->Add(m);
}

//////////////////////////////////////////////////////////////////////////

void Monster::Spawn(String type, Bool bRandom, Vec2 pos)
{
	Spawn(NameToType(type), bRandom, pos);
}

//////////////////////////////////////////////////////////////////////////