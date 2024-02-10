///////////////////////////////////////////////////////////////////////////
//		File:				Pickup.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A basic pickup/powerup class
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\pickup.h"

#include "SpriteMgr.h"
#include "EntityMgr.h"

#include "MiscMath.h"

//////////////////////////////////////////////////////////////////////////

Pickup::Pickup() : Entity()
{
	m_eClassType = CT_ITEM;

	m_uMaxFrames[EAS_WALK] = 1;
}

//////////////////////////////////////////////////////////////////////////

Pickup::~Pickup(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool Pickup::Init(EPickupType type /* = ePickup_HealthPackLarge */, Vec3 pos /* = k_v3Zero */)
{
	if (!Entity::Init(pos))
		return false;

	m_ePickupType = type;

	switch (type)
	{
	case ePickup_HealthPackLarge:
		m_uImage[EAS_WALK][0] = g_SpriteMgr->LoadSprite("healthpack_large.bmp");
		m_vSize = Vec2(30.0f, 20.0f);
		break;

	case ePickup_HealthPackSmall:
		m_uImage[EAS_WALK][0] = g_SpriteMgr->LoadSprite("healthpack_small.bmp");
		m_vSize = Vec2(13.0f, 16.0f);
		break;

	default:
		ASSERT(false);
	}

	m_BBox = GLib::Rectf(k_v2Zero, m_vSize);

	m_ShaderHandle = "Emit";

	char number[16];
	itoa(m_suNextID, number, 10);
	m_strName = "Item";
	m_strName += number;

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Pickup::CreatePickup(Vec2 pos, EPickupType type)
{
	PickupPtr pickup = new Pickup;
	ASSERT(pickup);
	pickup->Init(type, pos);
	g_EntityMgr->Add(pickup);
}

//////////////////////////////////////////////////////////////////////////

void Pickup::CreateRandomPickup(Vec2 pos)
{
	Pickup::CreatePickup(pos, EPickupType(Random(static_cast<UInt32>(ePickup_Max - 1))));
}

//////////////////////////////////////////////////////////////////////////

void Pickup::Update(Float dt)
{
	if (!ProcMessages())
		return;

	if (m_bValid)
		CheckCollisions();

	//HandleAnimation(dt);

	if (m_bVisible)
		Draw();
}

//////////////////////////////////////////////////////////////////////////

void Pickup::HandleCollision(EntityPtr ent)
{
	if (ent->m_eClassType == CT_PLAYER)
		m_bDestroy = true;
}

//////////////////////////////////////////////////////////////////////////