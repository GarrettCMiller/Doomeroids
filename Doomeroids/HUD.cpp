///////////////////////////////////////////////////////////////////////////
//		File:				HUD.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A generic HUD
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\hud.h"
#include "D3DWrapper.h"
#include "Player.h"
#include "Game.h"
#include "FontMgr.h"

#include "ProfileSample.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(HUD);

const char * WeaponImages[] =
{
	"DoubleShotgun.bmp", //"Pistol.bmp",
	"Shotgun.bmp",
	"Chaingun.bmp",
	"RocketLauncher.bmp",
	"PlasmaRifle.bmp",
	"BFG9000.bmp",
};

//////////////////////////////////////////////////////////////////////////

HUD::HUD(void)
{
}

//////////////////////////////////////////////////////////////////////////

HUD::~HUD(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 HUD::Init()
{
	Vec2 pos = Vec2(50, g_D3D->GetScreenHeight() - 250);

#if 1
	//i = 1, skipping pistol
	for (Int32 i = 0; i < /*k_MaxPossibleWeapons - 1*/6; i++)
	{
		m_Weapons[i].LoadImage(WeaponImages[i]);
		m_Weapons[i].SetPosition(pos);
	}
#endif

	/*m_BG.LoadImage("HUDBar.tga");
	m_BG.SetPosition(pos - Vec2(0.0f, 150.0f));
	m_BG.SetAlpha(192);*/

	UInt32 fontID = g_FontMgr->LoadFontSet(48, true);

	//m_Ammo = "";	//g_Player->m_Weapons[g_Player->m_nCurrentWeapon]
	//m_Ammo.SetPosition(275, pos.Y - 32);
	//m_Ammo.SetColor(255, 0, 0);
	//m_Ammo.SetFont(fontID);

	m_Health = g_Player->m_nHP;
	//m_Health += "%";
	m_Health.SetPosition(10, 825);
	m_Health.SetColor(255, 0, 0);
	m_Health.SetAlignment(UIText::Left);
	m_Health.SetFont(fontID);

	m_Kills = "Kills: ";
	m_Kills += Int32(g_Game->Kills);
	m_Kills.SetPosition(10, 800);
	m_Kills.SetColor(255, 0, 0);
	m_Kills.SetAlignment(UIText::Left);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 HUD::Update()
{
	//PROFILE_THIS;

	m_Health = "Health: ";
	m_Health += g_Player->m_nHP;
	//m_Health += "%";

	m_Kills = "Kills: ";
	m_Kills += Int32(g_Game->Kills);
	
	//m_Ammo.Update();
	m_Health.Update();
	m_Kills.Update();

	//m_BG.Update();
	m_Weapons[g_Player->m_nCurrentWeapon - 1].Update();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 HUD::Shutdown()
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////