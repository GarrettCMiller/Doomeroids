///////////////////////////////////////////////////////////////////////////
//		File:				CollisionMgr.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that handles collision detection
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\collisionmgr.h"
#include "D3DWrapper.h"
#include "Creature.h"
#include "Bullet.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CCollisionMgr);

//////////////////////////////////////////////////////////////////////////

CCollisionMgr::CCollisionMgr(void)
{
}

//////////////////////////////////////////////////////////////////////////

CCollisionMgr::~CCollisionMgr(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool CCollisionMgr::Init()
{
	m_Table.SetHashFunction(CCollisionMgr::HashFunction);
	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CCollisionMgr::Update(Float dt)
{
	//PROFILE_THIS(7);

	for (Int32 i = 0; i < m_Table.GetNumBuckets(); i++)
		CheckList(i);

	m_Table.Clear();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CCollisionMgr::CheckList(Int32 list)
{
	DblLinkedList<EntityPtr >& entlist = m_Table.GetList(list);
	DblLinkedList<EntityPtr >::Iterator i(entlist),
										j(entlist);

	//We need at least two candidates for collision
	if (entlist.Size() <= 1)
		return;

	EntityPtr	temp	= NULL,
				me		= NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		me = i.Current();
		ASSERT(me);

		for (j = i/*j.Begin()*/; !j.End(); ++j)
		{
			temp = j.Current();
			ASSERT(temp);

			//Don't check against yourself
			if (temp == me)
				continue;

			//Ignore bullet-bullet, prop-prop and item-item collision
			if (  temp->m_eClassType == me->m_eClassType && 
				(	me->m_eClassType == CT_BULLET 
				||	me->m_eClassType == CT_PROP 
				||	me->m_eClassType == CT_ITEM))
				continue;

			//Ignore bullet-item collisions (for now)
			if ((me->m_eClassType == CT_BULLET &&  temp->m_eClassType == CT_ITEM) || (temp->m_eClassType == CT_BULLET &&  me->m_eClassType == CT_ITEM))
				continue;

			if (temp->m_eClassType == CT_BULLET && (me->m_eClassType == CT_MONSTER || me->m_eClassType == CT_PLAYER))
			{
				//If I shot the bullet, ignore the collision
				if (me == temp->m_pParentEntity)
					continue;

				//If a monster of the same type as me shot the bullet, ignore it (Doom tradition)
				//(we wont ignore the collision here, but in the creature class itself. that way the bullets dont pass thru them)
				//if (CreaturePtr(temp->m_pParentEntity)->m_uTypeID == CreaturePtr(me)->m_uTypeID && me->m_eClassType != CT_PLAYER)
				//	continue;
			}
			else if (me->m_eClassType == CT_BULLET && (temp->m_eClassType == CT_MONSTER || temp->m_eClassType == CT_PLAYER))
			{
				//If I shot the bullet, ignore the collision
				if (temp == me->m_pParentEntity)
					continue;

				//If a monster of the same type as me shot the bullet, ignore it (Doom tradition)
				//(we wont ignore the collision here, but in the creature class itself. that way the bullets dont pass thru them)
				//if (CreaturePtr(me->m_pParentEntity)->m_uTypeID == CreaturePtr(temp)->m_uTypeID && temp->m_eClassType != CT_PLAYER)
				//	continue;
			}

			if (me->Collision(temp))
			{
				me->SendMessage(EM_COLLIDE, temp);
				temp->SendMessage(EM_COLLIDE, me);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

Bool CCollisionMgr::Shutdown()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

void CCollisionMgr::Add(EntityPtr ent)
{
	m_Table.Add(ent);
}

//////////////////////////////////////////////////////////////////////////

Int32 CCollisionMgr::HashFunction(EntityPtr& ent)
{
	Int32 bucket = 0;

	//Vec3 pos = ent->m_vPosition;
	////TEMP HACK
	//if (pos.x < 0.0f)
	//{
	//	if (pos.y < 0.0f)
	//	{
	//		if (pos.z < 0.0f)
	//			bucket = 0;
	//		else
	//			bucket = 1;
	//	}
	//	else
	//	{
	//		if (pos.z < 0.0f)
	//			bucket = 2;
	//		else
	//			bucket = 3;
	//	}
	//}
	//else
	//{
	//	if (pos.y < 0.0f)
	//	{
	//		if (pos.z < 0.0f)
	//			bucket = 4;
	//		else
	//			bucket = 5;
	//	}
	//	else
	//	{
	//		if (pos.z < 0.0f)
	//			bucket = 6;
	//		else
	//			bucket = 7;
	//	}
	//}

	//ASSERT(bucket >= 0 && bucket < 8);

	return bucket;
}

//////////////////////////////////////////////////////////////////////////