/////////////////////////////////////////////////////////////////////////////
//		File:			Bullet.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:06 AM
//
//		Description:	A bullet (projectile) class	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\bullet.h"
#include "D3DWrapper.h"
#include "EntityMgr.h"
#include "Game.h"
#include "SoundMgr.h"
#include "GameXMLTables.h"

//////////////////////////////////////////////////////////////////////////

Bullet::BulletFactory Bullet::ms_Factory;

static const Float k_MaxDistSqd = 120.0f * 120.0f;

//////////////////////////////////////////////////////////////////////////

Bullet::Bullet() : Entity()
{
	m_eClassType = CT_BULLET;
}

//////////////////////////////////////////////////////////////////////////

Bullet::~Bullet(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool Bullet::Init(EWeaponType type, EntityPtr parent, Vec3 pos)
{
	static Int32 BulletNum = 0;

	if (!(Entity::Init(pos)))
		return false;

	m_pParentEntity			= parent;
	m_eType					= type;

	/*if (m_vSize.X == 1)
		m_vSize.X = 2;
	if (m_vSize.Y == 1)
		m_vSize.Y = 2;*/

	//m_uImage[EAS_WALK][0]	= g_WeaponTable[type].bulletImage;

	if (g_WeaponTable[type].bulletMeshID >= 0)
		InitMesh(g_WeaponTable[type].bulletMeshID);

//	ASSERT(m_uImage[EAS_WALK][0] >= 0);

	//m_ShaderHandle = "Emit";

#ifdef _DEBUG
	char number[16];
	itoa(BulletNum++, number, 10);
	m_strName = "Bullet";
	m_strName += number;
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Bullet::Update(Float dt)
{
	if (!ProcMessages())
		return;

	if (m_bValid)
	{
		CheckCollisions();

		m_vPosition += m_vVelocity * dt;

		//m_uDirection = 0;
		//HandleAnimation();

		if (/*m_pParentEntity == g_Player &&*/ DistanceSquared(m_vPosition, g_Player->m_vPosition) > k_MaxDistSqd)
		{
			m_bDestroy = true;
			return;
		}

		Draw();
	}
}

//////////////////////////////////////////////////////////////////////////

void Bullet::Draw()
{
	//if (m_pParentEntity == g_Player)
	//	m_fScale = 4.0f;

	//m_fAngle = m_vVelocity.Theta();
	Entity::Draw();
}

//////////////////////////////////////////////////////////////////////////

void Bullet::HandleCollision(EntityPtr ent)
{
	if (g_WeaponTable[m_eType].hitsound >= 0)
		g_SoundMgr->PlaySound(g_WeaponTable[m_eType].hitsound);
	
	m_bDestroy = true;

	/*if (g_WeaponTable[m_eType].splashdmg > 0)
		DoSplashDamage();*/
}

//////////////////////////////////////////////////////////////////////////

void Bullet::DoSplashDamage()
{
	DblLinkedList<EntityPtr >::Iterator i(g_EntityMgr->m_Ents);

	CreaturePtr	temp = NULL;

	Float		dist	= 0.0f,
				Radius	= g_WeaponTable[m_eType].splashradius,
				RadSqd	= Radius * Radius,
				pos[2]	= {0.0f, 0.0f};

	Int32			damage	= 0;

	//Invalid or no splash radius
	if (Radius <= 0.0f)
		return;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = (CreaturePtr) i.Current();

		//Check only for live ones
		if (!temp->m_bValid)
			continue;

		//If its a bullet or item, or if its a friendly bullet, skip it
		if (temp->m_eClassType >= CT_BULLET)
			continue;

		if (m_pParentEntity == g_Player && temp == g_Player)
			continue;

		dist = DistanceSquared<Float>(m_vPosition, temp->m_vPosition);

		//If it's not in the blast radius, skip it
		if (dist > RadSqd)
			continue;

		//
		//This is a success!
		//
		
		//Set the blasted flag
		temp->m_bBlasted = true;

		damage = g_WeaponTable[m_eType].splashdmg;
		//damage *= Radius / (dist == 0.0f ? 0.0000001f : dist);

		//Actual splash damage
		if (temp->Hurt(damage))
		{
			pos[0] = m_vPosition.x;
			pos[1] = m_vPosition.z;

			temp->SendMessage(EM_BLAST, pos, 2);

			if (m_pParentEntity == g_Player && temp->m_eClassType != CT_PROP)
				g_Game->Kills++;
		}
	}

	g_Player->SetRad(m_vPosition);
}

//////////////////////////////////////////////////////////////////////////

BulletPtr Bullet::Create()
{
	return ms_Factory.Create();
}

//////////////////////////////////////////////////////////////////////////

void Bullet::Remove()
{
	ms_Factory.Release(this);
	Entity::Remove();
}

//////////////////////////////////////////////////////////////////////////