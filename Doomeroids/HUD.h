///////////////////////////////////////////////////////////////////////////
//		File:				HUD.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A generic HUD
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"

#include "UIImage.h"
#include "UIText.h"

#include "Weapon.h"

//////////////////////////////////////////////////////////////////////////

class HUD : public Singleton<HUD>
{
	UIImage		m_BG;
	UIImage		m_Weapons[k_MaxPossibleWeapons];

	UIText		m_Health;
	UIText		m_Armor;
	
	UIText		m_Ammo;

	UIText		m_Kills;

public:
	HUD(void);
	~HUD(void);

	Int32		Init();
	Int32		Update();
	Int32		Shutdown();
};


//And if you don't believe me, I'll put a g_HUD on you too!
#define g_HUD	HUD::GetInstance()