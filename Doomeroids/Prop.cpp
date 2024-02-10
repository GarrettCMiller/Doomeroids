///////////////////////////////////////////////////////////////////////////
//		File:				Prop.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A basic prop class (for semi-static objects)
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "prop.h"
#include "EntityMgr.h"
#include "Bullet.h"
#include "MiscMath.h"
#include "Game.h"
#include "GameXMLTables.h"

//////////////////////////////////////////////////////////////////////////

Prop::Prop() 
		:	Creature(),
			m_bDynamic(false),
			m_ePropType(eProp_Barrel),
			m_pAggressor(NULL)
{
	m_eClassType = CT_PROP;
}

//////////////////////////////////////////////////////////////////////////

Prop::~Prop(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 Prop::Init(EPropType ept, Vec3 pos, Bool bDynamic)
{
	if (!Creature::Init(NameToType("prop"), pos))
		return false;

	m_bDynamic = bDynamic;
	m_ePropType = ept;

	char number[16];
	itoa(m_suNextID++, number, 10);
	m_strName = "Prop";
	m_strName += number;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

void Prop::Draw()
{
	if (m_eCurrentAnimState == EAS_DEAD)
		m_bDestroy = true;
	else
		Entity::Draw();
}

//////////////////////////////////////////////////////////////////////////

Bool Prop::Hurt(Int32 damage)
{
	m_nHP -= damage;

	if (m_nHP <= 0)
	{
		m_bValid = false;

		SendMessage(EM_DIE);

		DoSplashDamage();

		return true;
	}
	else if (m_nHP > m_nHPMax)
		m_nHP = m_nHPMax;

	return false;
}

//////////////////////////////////////////////////////////////////////////

void Prop::HandleAnimation(Float dt)
{
	Entity::HandleAnimation(dt);
}

//////////////////////////////////////////////////////////////////////////

void Prop::HandleCollision(EntityPtr ent)
{
	ASSERT(ent);

	BulletPtr bullet = NULL;

	switch (ent->m_eClassType)
	{
	case CT_BULLET:
		bullet = static_cast<BulletPtr > (ent);

		Hurt(g_WeaponTable[bullet->m_eType].damage);

		m_bBlasted = (g_WeaponTable[bullet->m_eType].splashdmg > 0);
		m_pAggressor = bullet->m_pParentEntity;

		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void Prop::HandleBlast(Float x, Float y)
{	
	g_SoundMgr->PlaySound(m_nSoundDie);

	Float angle = AngleBetween(m_vPosition.x, m_vPosition.z, x, y);

	m_vVelocity.SetSpeedDir(2.5f, angle + 180.0f);
}

//////////////////////////////////////////////////////////////////////////

void Prop::DoSplashDamage()
{
	DblLinkedList<EntityPtr >::Iterator i(g_EntityMgr->m_Ents);

	CreaturePtr	temp		= NULL;

	Float		dist		= 0.0f,
				RadiusSqd	= m_nSplashRange * m_nSplashRange,
				pos[2]		= {0.0f, 0.0f};

	for (i.Begin(); !i.End(); ++i)
	{
		temp = (CreaturePtr) i.Current();

		//Check only for live ones
		if (!temp->m_bValid)
			continue;

		//If its a bullet or item, or if its a friendly bullet, skip it
		if (temp->m_eClassType == CT_BULLET || temp->m_eClassType == CT_ITEM)
			continue;

		dist = DistanceSquared<Float>(m_vPosition, temp->m_vPosition);

		//If it's not in the blast radius, skip it
		if (dist > RadiusSqd)
			continue;

		//
		//This is a success!
		//

		//Set the blasted flag
		temp->m_bBlasted = true;

		//Actual splash damage
		if (temp->Hurt(m_nDamage))
		{
			pos[0] = this->m_vPosition.x;
			pos[1] = this->m_vPosition.z;

			temp->SendMessage(EM_BLAST, pos, 2);

			if (m_pAggressor == g_Player && temp->m_eClassType != CT_PROP)
				g_Game->Kills++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////