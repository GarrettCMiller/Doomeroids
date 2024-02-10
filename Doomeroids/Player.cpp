/////////////////////////////////////////////////////////////////////////////
//		File:			Player.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:48:16 AM
//
//		Description:	A player (whether local or remote)	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\player.h"
#include "DirectInput.h"
#include "SoundMgr.h"
#include "SpriteMgr.h"
#include "Game.h"
#include "GameXMLTables.h"

#include "VectorGraphics.h"

//////////////////////////////////////////////////////////////////////////

Player* Player::m_pInstance = NULL;

extern CVectorGraphics vg;

//////////////////////////////////////////////////////////////////////////

Player::Player() 
		:	Creature(),
			bLocal(true)
{
	m_eClassType = CT_PLAYER;
}

//////////////////////////////////////////////////////////////////////////

Player::~Player(void)
{
}

//////////////////////////////////////////////////////////////////////////

void Player::Draw()
{
	Entity::Draw();
	//g_SpriteMgr->DrawSprite(Image, m_vPosition.X, m_vPosition.Y, z, 0.0f, 1.0f, 15, 27);//, 0, NULL, (bLocal ? 0 : 0xFFFF00FF));
}

//////////////////////////////////////////////////////////////////////////

Bool Player::Init(Vec3 pos /* = k_v3Zero */)
{
	static Int32 PlayerID = 0;

	mk_vg.Init();

	if (!Creature::Init(NameToType("player"), pos))
		return false;

	m_ShaderHandle = "Emit";

	char number[16];
	itoa(PlayerID++, number, 10);
	m_strName = "Player";
	m_strName += number;

	Rad = g_SpriteMgr->LoadSprite("BlastRad.bmp");

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Player::SetRad(Vec2 _pos)
{
	pos = _pos;
	alpha = 255.0f;
}

//////////////////////////////////////////////////////////////////////////

void Player::HandleRad()
{
	if (alpha > 0.0f)
	{
		g_SpriteMgr->DrawSprite(Rad, pos.X, pos.Y, 0.99f, 0, g_WeaponTable[Weapon::NameToType("Rocket Launcher")].splashradius / 125.0f, ALPHA(Int32(alpha)));
		alpha *= 0.99f;
	}
}

//////////////////////////////////////////////////////////////////////////

void Player::Update(Float dt)
{
	if (bLocal && dt > 0.0f)
	{
		if (m_eCurrentAnimState < EAS_DIE)
		{
			LookWithMouse();

			HandleWeaponInput();

			HandleMovement();
		}

#ifdef _DEBUG

		//HandleRad();

		if (g_Input->MappedKeyDown(eAction_Suicide) && m_eCurrentAnimState != EAS_SPLATTER)
			SendMessage(EM_BLAST);
#endif

	}

	Creature::Update(dt);
}

//////////////////////////////////////////////////////////////////////////

void Player::HandleWeaponInput()
{
	if		(g_Input->MappedKeyDown(eAction_EquipWeapon1) && m_bWeapons[1])
		m_nCurrentWeapon = 1;
	else if	(g_Input->MappedKeyDown(eAction_EquipWeapon2) && m_bWeapons[2])
		m_nCurrentWeapon = 2;
	else if	(g_Input->MappedKeyDown(eAction_EquipWeapon3) && m_bWeapons[3])
		m_nCurrentWeapon = 3;
	else if	(g_Input->MappedKeyDown(eAction_EquipWeapon4) && m_bWeapons[4])
		m_nCurrentWeapon = 4;
	else if	(g_Input->MappedKeyDown(eAction_EquipWeapon5) && m_bWeapons[5])
		m_nCurrentWeapon = 5;
	else if	(g_Input->MappedKeyDown(eAction_EquipWeapon6) && m_bWeapons[6])
		m_nCurrentWeapon = 6;

	//
	//Handle the shooting of the gun
	//
	if (g_Input->MouseDown(g_Input->eLeftButton))
	{
		if (Weapon::m_Weapons[m_nCurrentWeapon].IsReady())
			Shoot();

		bIsShooting = true;
	}
	else
		bIsShooting = false;
}

//////////////////////////////////////////////////////////////////////////

void Player::HandleMovement()
{
	Float runSpeed = (g_Input->MappedKeyDown(eAction_MoveRun) ? 1.5f : 1.0f);

	if (Timer::GetTimeScale() != 1.0f)
		runSpeed *= 1.5f;

	if (g_Input->MappedKeyDown(eAction_MoveRight))
	{
		m_vVelocity.x += m_vAcceleration.x * runSpeed;
		m_eCurrentAnimState = EAS_WALK;
	}

	if (g_Input->MappedKeyDown(eAction_MoveLeft))
	{
		m_vVelocity.x -= m_vAcceleration.x * runSpeed;
		m_eCurrentAnimState = EAS_WALK;
	}

	if (g_Input->MappedKeyDown(eAction_MoveUp))
	{
		m_vVelocity.z += m_vAcceleration.z * runSpeed;
		m_eCurrentAnimState = EAS_WALK;
	}

	if (g_Input->MappedKeyDown(eAction_MoveDown))
	{
		m_vVelocity.z -= m_vAcceleration.z * runSpeed;
		m_eCurrentAnimState = EAS_WALK;
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

void Player::LookWithMouse()
{
	Vec2 md = g_Input->GetMouseDelta();
	TurnRight(md.X);
}

//////////////////////////////////////////////////////////////////////////

void Player::HandleCollision(EntityPtr ent)
{
	Creature::HandleCollision(ent);

	switch (ent->m_eClassType)
	{
	case CT_ITEM:
		HandlePickup(static_cast<PickupPtr>(ent));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void Player::HandlePickup(PickupPtr pickup)
{
	switch (pickup->m_ePickupType)
	{
	case ePickup_HealthPackLarge:
		Hurt(-(Pickup::HealthPackLarge_Amt));
		break;

	case ePickup_HealthPackSmall:
		Hurt(-(Pickup::HealthPackSmall_Amt));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void Player::Shoot()
{
	m_eCurrentAnimState = EAS_SHOOT;

	Vec3 vHotSpot;

	switch (m_uDirection)
	{
	case 0:
		vHotSpot = Vec3(-1.0f, -1.0f);
		break;
	case 1:
		vHotSpot = Vec3(-3.5f, 0.0f);
		break;
	case 2:
		vHotSpot = Vec3(-3.5f, 0.0f);
		break;
	case 3:
		vHotSpot = Vec3(-3.0f, 1.0f);
		break;
	case 4:
		vHotSpot = Vec3(-2.0f, 1.0f);
		break;
	case 5:
		vHotSpot = Vec3(1.0f, 1.0f);
		break;
	case 6:
		vHotSpot = Vec3(0.9f, 0.5f);
		break;
	case 7:
		vHotSpot = Vec3(1.0f, 1.0f);
		break;
	}

	vHotSpot.x = vHotSpot.z = 0.0f;//2.25f;
	vHotSpot.y = 3.0f;

	Vec3 pos = m_vPosition;

	Weapon::m_Weapons[m_nCurrentWeapon].Shoot(pos + vHotSpot, lookAt, this);
}

//////////////////////////////////////////////////////////////////////////

void Player::HandleDeath()
{
	char buffer[80] = {'\0'};

	sprintf(buffer, "YOU DIED!   Kills: %d     Lived: %.2f seconds", g_Game->Kills, g_Game->tElapsed.Get());
	g_Game->SetMessage(buffer, -1.0f);

	Creature::HandleDeath();
}

//////////////////////////////////////////////////////////////////////////