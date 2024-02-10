///////////////////////////////////////////////////////////////////////////
//		File:				Entity.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		8/7/2005
//
//		Description:		Your basic entity
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Entity.h"

//#include "Globals.h"

#include "SpriteMgr.h"
#include "DirectInput.h"
#include "Creature.h"
#include "EntityMgr.h"
#include "Bullet.h"
#include "CollisionMgr.h"

//HACK (for self-referencing shooting)
#include "Player.h"

#include "MeshMgr.h"

#include "Game.h"

///////////////////////////////////////////////////////////////////////////////

UInt32 Entity::m_suNextID		= 0;
//UInt32 Entity::m_uBoxImage		= UInt32(-1);
Bool Entity::m_bDrawBBoxes		= true;

const Float	Entity::mk_fFriction = 0.9f;

CVectorGraphics Entity::mk_vg;

const Vec3 Entity::ms_RunSpeed	= Vec3(0.1f, 0.0f, 0.1f);

//////////////////////////////////////////////////////////////////////////

const char* EntityMessageText[NUM_ENTITY_MESSAGES] = 
{
	"Move",
	"MoveToEntity",
	"MoveToLocation",
	
	"Attack",

	"Blast",

	"Hurt",
	"Die",

	"Collide",

	"Remove",
};

const char* EntityAnimationStates[NUM_ANIMATION_STATES] = 
{
	"walk",
	"shoot",
	"hurt",
	"die",
	"splatter",
	"dead"
};

Vec3 Entity::m_svMaxVelocity;

//////////////////////////////////////////////////////////////////////////

EEntityAnimationState Entity::NameToAnimState(String  name)
{
	for (Int32 i = 0; i < NUM_ANIMATION_STATES; i++)
	{
		if (stricmp(name.c_str(), EntityAnimationStates[i]) == 0)
			return EEntityAnimationState(i);
	}

	return NUM_ANIMATION_STATES;
}

//////////////////////////////////////////////////////////////////////////

Entity::Entity() : m_eClassType(CT_ENTITY)
{
	Init();
}

//////////////////////////////////////////////////////////////////////////

Entity::~Entity(void)
{
	m_MsgQueue.Clear();
}

//////////////////////////////////////////////////////////////////////////

Bool Entity::Init(Vec3 pos /* = k_v3Zero */)
{
	//ZeroMemory(m_uMaxFrames, sizeof(UInt32) * NUM_ANIMATION_STATES);
	//memset(&m_uImage, -1, sizeof(UInt32) * NUM_ANIMATION_STATES * MAX_ANIMATION_FRAMES);

	m_pMesh				= NULL;

	m_nID				= -1;

	m_fCurrentFrame		= 0.0f;
	m_eCurrentAnimState	= EAS_WALK;
	m_fAngle			= 270.0f;
	m_fScale			= 1.0f;
	
	m_pTargetEntity		= NULL;
	m_pParentEntity		= NULL;

	m_ShaderID			= 0;
	m_ShaderHandle		= NULL;

	m_vPosition			= pos;
	m_vOrientation		= k_v3Zero;

	m_vForward			= k_v3ZAxis;
	m_vRight			= k_v3XAxis;
	m_vUp				= k_v3YAxis;

	m_bValid			= true;
	m_bVisible			= true;
	m_bBlasted			= false;
	m_bDestroy			= false;

	char number[16];
	itoa(m_suNextID++, number, 10);
	m_strName = "Entity";
	m_strName += number;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Entity::InitMesh(UInt32 meshID)
{
	m_pMesh = g_MeshMgr->GetMeshByID(meshID);

	if (!m_pMesh)
	{
		g_Log << "Failed to get mesh by id! id: " << meshID << "\n";
		return false;
	}

	m_BBox = m_pMesh->GenerateBoundingBox();

	if (m_BBox.tlx > 1000)	m_BBox.tlx = 10;
	if (m_BBox.tlx < -1000)	m_BBox.tlx = -10;
	if (m_BBox.tly > 1000)	m_BBox.tly = 10;
	if (m_BBox.tly < -1000)	m_BBox.tly = -10;
	if (m_BBox.tlz > 1000)	m_BBox.tlz = 10;
	if (m_BBox.tlz < -1000)	m_BBox.tlz = -10;
	if (m_BBox.tlz > 1000)	m_BBox.tlz = 10;
	if (m_BBox.tlz < -1000)	m_BBox.tlz = -10;
	if (m_BBox.brx > 1000)	m_BBox.brx = 10;
	if (m_BBox.brx < -1000)	m_BBox.brx = -10;
	if (m_BBox.bry > 1000)	m_BBox.bry = 10;
	if (m_BBox.bry < -1000)	m_BBox.bry = -10;
	if (m_BBox.brz > 1000)	m_BBox.brz = 10;
	if (m_BBox.brz < -1000)	m_BBox.brz = -10;
	if (m_BBox.brz > 1000)	m_BBox.brz = 10;
	if (m_BBox.brz < -1000)	m_BBox.brz = -10;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

void Entity::Update(Float dt)
{
	//If the entity manager is not paused
	if (dt > 0.0f)
	{
		//Check the message queue
		if (!ProcMessages())
			return;

		//Only collide when "live"
		if (m_bValid)
			CheckCollisions();

		//Move the entity based on velocity, then apply friction
		m_vPosition	+= m_vVelocity * dt;	
		m_vVelocity *= mk_fFriction;

		//Animate
		HandleAnimation(dt);
	}

	//Draw if visible
	if (m_bVisible)
	{
		//PROFILE("Entity::Draw", 13);
		Draw();
	}
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleAnimation(Float dt)
{
	static const Float Scale = 0.333f;

	switch (m_eCurrentAnimState)
	{
	case EAS_WALK:
		m_fCurrentFrame += m_vVelocity.Magnitude() * 0.5f * dt;
		break;

	case EAS_DIE:
		m_fCurrentFrame += Scale  * dt;
		break;

	case EAS_DEAD:
		break;

	default:
		m_fCurrentFrame += Scale * dt;
	}

	/*if (m_fCurrentFrame > m_uMaxFrames[m_eCurrentAnimState])
	{
		if (m_eCurrentAnimState == EAS_DIE)
		{
			SetAnimation(EAS_DEAD);
		}
		else if (m_eCurrentAnimState == EAS_SPLATTER)
		{
			m_fCurrentFrame			= 1.0f;
			m_eCurrentAnimState		= EAS_DEAD;
		}
		else if (m_eCurrentAnimState == EAS_HURT)
		{
			SetAnimation(EAS_WALK);
		}
		else if (m_eCurrentAnimState == EAS_SHOOT)
		{
			if (m_eClassType == CT_PLAYER && static_cast<Player*>(this)->bIsShooting)
				m_fCurrentFrame	= 0.0f;
			else
				SetAnimation(EAS_WALK);
				
		}
		else
			m_fCurrentFrame			= 0.0f;
	}*/
}

//////////////////////////////////////////////////////////////////////////

void Entity::SetAnimation(EEntityAnimationState eas)
{
	m_fCurrentFrame			= 0.0f;
	m_eCurrentAnimState		= eas;
}

//////////////////////////////////////////////////////////////////////////

void Entity::Draw()
{
	if (m_pMesh)
	{
		D3DMatrix matPos;
		D3DQuaternion qRot;
		
		D3DXQuaternionRotationAxis(&qRot, &k_v3YAxis, PI + m_fAngle);
		D3DXMatrixAffineTransformation(&matPos, 1.0f, NULL, &qRot, &m_vPosition);
		
		m_pMesh->FrameMove(g_Game->tElapsed, matPos);
		
		m_pMesh->AddInstanceData(matPos);
	}
	else
	{
		if (m_eCurrentAnimState >= EAS_DIE)
			m_uDirection = 0;

		Int32 frame		= static_cast<Int32>(m_fCurrentFrame) + (m_uDirection * m_uMaxFrames[m_eCurrentAnimState]);
		UInt32 Image	= m_uImage[m_eCurrentAnimState][frame];

		//g_SpriteMgr->DrawSpriteEffect(Image, m_ShaderID, m_ShaderHandle, m_vPosition.X, m_vPosition.Y, z, m_fAngle, m_fScale);
		//g_SpriteMgr->DrawSpriteEffect(Image, 2, "Entity", m_vPosition.x, m_vPosition.y, z, m_fAngle, m_fScale);
		g_SpriteMgr->DrawSprite(Image, m_vPosition.x, m_vPosition.y, m_vPosition.z, m_fAngle, m_fScale);
	}

	//DrawBBox();
}

//////////////////////////////////////////////////////////////////////////

void Entity::DrawBBox()
{
	GLib::Rect3D	r1(m_BBox);
	Vec3			p1 = Vec3(m_vPosition);

	r1.SetCenter(p1);
	r1.MoveTo(Vec3(0.0f, 10.0f, 0.0f));

	CMatrix temp;
	g_D3D->SetWorldMatrix(temp);
	g_D3D->PushRenderState(D3DRS_LIGHTING, FALSE);
	mk_vg.DrawCube(r1);
	g_D3D->PopRenderState(D3DRS_LIGHTING);
}

//////////////////////////////////////////////////////////////////////////

void Entity::LookAt(Entity &ent)
{
	LookAt(ent.m_vPosition.x, ent.m_vPosition.z);
}

//////////////////////////////////////////////////////////////////////////

void Entity::LookAt(Vec2 pos)
{
	LookAt(pos.X, pos.Y);
}

//////////////////////////////////////////////////////////////////////////

void Entity::LookAt(Float x, Float y)
{
	Float angle = AngleBetween(m_vPosition.x, -m_vPosition.z, x, -y);

	//angle -= 90.0f;
	//m_uDirection = UInt32((angle - 90.0f) / 45) % 8;

	if (m_pMesh)
		m_fAngle = angle;
}

//////////////////////////////////////////////////////////////////////////

void Entity::MoveForward(Float dz)
{
	m_vVelocity += m_vForward * dz;
}

//////////////////////////////////////////////////////////////////////////

void Entity::MoveUp(Float dy)
{
	m_vVelocity += m_vUp * dy;
}

//////////////////////////////////////////////////////////////////////////

void Entity::MoveRight(Float dx)
{
	m_vVelocity += m_vRight * dx;
}

//////////////////////////////////////////////////////////////////////////

void Entity::RotateForward(Float theta)
{
	theta *= Deg2Rad;

	static D3DMatrix rotMat;

	D3DXMatrixRotationAxis(&rotMat, &m_vRight, theta);

	D3DXVec3TransformCoord(&m_vForward, &m_vForward, &rotMat);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &rotMat);

	m_vForward.Normalize();
	m_vUp.Normalize();
}

//////////////////////////////////////////////////////////////////////////

void Entity::TurnRight(Float theta)
{
	theta *= Deg2Rad;

	static D3DMatrix rotMat;

	D3DXMatrixRotationAxis(&rotMat, &m_vUp, theta);

	D3DXVec3TransformCoord(&m_vForward, &m_vForward, &rotMat);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotMat);

	m_vForward.Normalize();
	m_vRight.Normalize();

	m_fAngle += theta;
}

//////////////////////////////////////////////////////////////////////////

void Entity::RotateRight(Float theta)
{
	theta *= Deg2Rad;

	static D3DMatrix rotMat;

	D3DXMatrixRotationAxis(&rotMat, &m_vForward, theta);

	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &rotMat);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotMat);

	m_vUp.Normalize();
	m_vRight.Normalize();
}

//////////////////////////////////////////////////////////////////////////

void Entity::CheckCollisions()
{
	//PROFILE_THIS(14);

	if (!m_bValid)
		return;

	g_CollisionMgr->Add(this);
}

//////////////////////////////////////////////////////////////////////////

Bool Entity::Collision(EntityPtr ent)
{
	GLib::Rect3D	r1(m_BBox),
					r2(ent->m_BBox);

	Vec3	p1 = Vec3(m_vPosition),
			p2 = Vec3(ent->m_vPosition);

	r1.SetCenter(p1);
	r2.SetCenter(p2);
	//r1.MoveTo(Vec3(0.0f, 10.0f, 0.0f));
	//r2.MoveTo(Vec3(0.0f, 10.0f, 0.0f));

	return r1.Intersect(r2);
}

//////////////////////////////////////////////////////////////////////////

void Entity::SendMessage(ENTITY_MESSAGE_ITEM& emi)
{
	m_MsgQueue.Enqueue(emi);
}

//////////////////////////////////////////////////////////////////////////

void Entity::SendMessage(ENTITY_MESSAGE msg)
{
	EMI emi;	
	emi.Init(msg);
	m_MsgQueue.Enqueue(emi);
}

//////////////////////////////////////////////////////////////////////////

void Entity::SendMessage(ENTITY_MESSAGE msg, Float *MiscData, UInt32 DataCount)
{
	ASSERT(DataCount);

	EMI emi;
	emi.Init(msg, (Float *) MiscData, DataCount);
	m_MsgQueue.Enqueue(emi);
}

//////////////////////////////////////////////////////////////////////////

void Entity::SendMessage(ENTITY_MESSAGE msg, UInt32 *MiscData, UInt32 DataCount)
{
	ASSERT(DataCount);

	EMI emi;
	emi.Init(msg, (UInt32 *) MiscData, DataCount);
	m_MsgQueue.Enqueue(emi);
}

//////////////////////////////////////////////////////////////////////////

void Entity::SendMessage(ENTITY_MESSAGE msg, Int32 *MiscData, UInt32 DataCount)
{
	ASSERT(DataCount);

	EMI emi;
	emi.Init(msg, (Int32 *) MiscData, DataCount);
	m_MsgQueue.Enqueue(emi);
}

//////////////////////////////////////////////////////////////////////////

void Entity::SendMessage(ENTITY_MESSAGE msg, EntityPtr& MiscData, UInt32 DataCount)
{
	ASSERT(DataCount);

	EMI emi;
	emi.Init(msg, MiscData, DataCount);
	m_MsgQueue.Enqueue(emi);
}

//////////////////////////////////////////////////////////////////////////

void Entity::CheckDestroy()
{
	if (m_bDestroy)
	{
		SendMessage(EM_REMOVE);
		m_bDestroy = false;
		m_bValid = false;
	}
}

//////////////////////////////////////////////////////////////////////////

Bool Entity::ProcMessages()
{
	//PROFILE_THIS(15);

	EMI	emi;

	CheckDestroy();

	while (!m_MsgQueue.IsEmpty())
	{
		m_MsgQueue.Dequeue(emi);

		if (!m_bValid)	//Must be alive and well to receive these messages
		{
			switch (emi.eMsg)
			{
			case EM_REMOVE:
				Remove();
				return false;
				break;
			
			case EM_BLAST:
				HandleBlast(emi.fData[0], emi.fData[1]);
				break;

			case EM_DIE:
				HandleDeath();
				break;
			}
		}
		else //if alive and well
		{
			switch (emi.eMsg)
			{
			case EM_MOVE:
			case EM_MOVETOLOCATION:
				m_vTargetPosition.x	= emi.fData[0];
				m_vTargetPosition.z	= emi.fData[1];
				m_eState			= ES_MOVE;
				break;

			case EM_MOVETOENTITY:
				m_pTargetEntity		= emi.pData[0];
				m_eState			= ES_MOVE;
				break;

			case EM_ATTACK:
				m_pTargetEntity		= emi.pData[0];
				m_eState			= ES_ATTACK;
				break;

			case EM_HURT:
				HandleHurting(emi.nData[0]);
				break;

			case EM_COLLIDE:
				ASSERT(g_EntityMgr->IsManaged(emi.pData[0]));
				HandleCollision(emi.pData[0]);
				break;

			default:
				ASSERT(false);
			}
		}
	}

	CheckDestroy();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleCollision(EntityPtr ent)
{
	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void Entity::Remove()
{
	VERIFY(g_EntityMgr->Remove(this));
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleHurting(Int32 amt)
{
	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleDeath()
{
	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleAttacking()
{
	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleMoving()
{
	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void Entity::HandleBlast(Float x, Float y)
{
	ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

Int32 Entity::LoadFrames(String BasePath, EEntityAnimationState state)
{
	return LoadFrameSet(m_uImage, m_uMaxFrames, BasePath, state);
}

//////////////////////////////////////////////////////////////////////////

Int32 Entity::LoadFrameSet(Int32 images[NUM_ANIMATION_STATES][MAX_ANIMATION_FRAMES], 
						 Int32 mymaxframes[NUM_ANIMATION_STATES],
						 String  BasePath,
						EEntityAnimationState state,
						Bool bOneDirection)
{
	if (state == EAS_ALL)
	{
		Int32		frames	= 0,
				sets	= 0;

		frames += LoadFrameSet(images, mymaxframes, BasePath, EAS_WALK);
		frames += LoadFrameSet(images, mymaxframes, BasePath, EAS_SHOOT);
		frames += LoadFrameSet(images, mymaxframes, BasePath, EAS_HURT);
		frames += LoadFrameSet(images, mymaxframes, BasePath, EAS_DIE,		true);
		frames += LoadFrameSet(images, mymaxframes, BasePath, EAS_SPLATTER,	true);
		frames += LoadFrameSet(images, mymaxframes, BasePath, EAS_DEAD,		true);

		g_ResLog << "Successfully loaded " << frames << " animation frames of " << sets << " sets successfully!\n";

		return frames;
	}

	Int32 maxframes = mymaxframes[state];

	if (!bOneDirection)
		maxframes *= NUM_DIRECTIONS;

	char	path[256];
	char	fullpath[256];
	char	framenumber[3];

	ZeroMemory(path, 256);
	ZeroMemory(fullpath, 256);

	strcpy(path, BasePath.c_str());

	switch (state)
	{
	case EAS_WALK:
		strcat(path, "_walk_");
		break;

		/*case EAS_RUN:
		strcat(path, "_run_");
		break;*/

		//case EAS_ATTACK:
		//	strcat(path, "_attack_");
		//	break;

	case EAS_SHOOT:
		strcat(path, "_shoot_");
		break;

	case EAS_HURT:
		strcat(path, "_hurt_");
		break;

	case EAS_DIE:
		strcat(path, "_die_");
		break;

	case EAS_SPLATTER:
		strcat(path, "_splatter_");
		break;

	case EAS_DEAD:
		strcat(path, "_dead_");
		break;

		//case EAS_IDLE:
	default:
		//strcat(path, "_idle_");
		break;
	}

	Int32 sframes = 0;

	for (Int32 frame = 0; frame < maxframes; frame++)
	{
		//Copy the path into the fullpath
		strcpy(fullpath, path);

		//Convert frame number to text
		itoa(frame, framenumber, 10);

		//Make sure it comes out to 00 instead of 0
		if (frame < 10)
			strcat(fullpath, "0");

		//Add frame number
		strcat(fullpath, framenumber);

		//Add file extension
		strcat(fullpath, ".bmp");

		if (images[state][frame] = g_SpriteMgr->LoadSprite(fullpath))
			sframes++;
	}

	return sframes;
}

//////////////////////////////////////////////////////////////////////////