///////////////////////////////////////////////////////////////////////////
//		File:				GameXMLTables.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Database-like tables that define entities and stuff
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "XMLTables.h"

#include "Creature.h"
#include "Weapon.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct XMLEntity
{
	XMLEntity()
	{
		//ZeroMemory(name,		16);
		ZeroMemory(weapons,		k_MaxPossibleWeapons * sizeof(Bool));
		ZeroMemory(maxframes,	NUM_ANIMATION_STATES * sizeof(Int32));
		ZeroMemory(images,		NUM_ANIMATION_STATES * MAX_ANIMATION_FRAMES * sizeof(Int32));
		memcpy(maxframes,		Creature::ms_MaxFrames, sizeof(Int32) * NUM_ANIMATION_STATES);
		shaderID				= DEFAULT_ENTITY_SHADER_ID;
		shaderTech				= DEFAULT_ENTITY_SHADER_TECH;
		currentweapon			= 0;
		maxhp					= 1;
		hp						= maxhp;
		hurtsound				= -1;
		diesound				= -1;
		alertsound				= -1;
		footpos					= k_v2Zero;
		bbox					= k_v2Zero;
		meshID					= -1;

		IsValid					= false;
	}

	Bool		IsValid;

	String		name;
	Bool		weapons[k_MaxPossibleWeapons];
	Int32		currentweapon;
	Int32		maxhp;
	Int32		hp;
	Int32		hurtsound;
	Int32		diesound;
	Int32		alertsound;
	Int32		maxframes[NUM_ANIMATION_STATES];
	Int32		images[NUM_ANIMATION_STATES][MAX_ANIMATION_FRAMES];
	Int32		meshID;
	Int32		shaderID;
	D3DXHANDLE	shaderTech;
	Vec2		footpos;
	Vec2		bbox;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class XMLWeapon
{
public:
	XMLWeapon()
	{
		//ZeroMemory(name, 16);
		damage			= 1;
		splashdmg		= 0;
		shotcount		= 1;
		bulletImage		= -1;
		hitsound		= -1;
		firesound		= -1;
		spread			= 0.0f;
		speed			= 25.0f;
		splashradius	= 0.0f;
		delay			= 1.0f;
		bulletSize		= k_v2Zero;

		IsValid			= false;
	}

	Bool	IsValid;

	String	name;
	Int32	damage;			//Per bullet
	Int32	splashdmg;
	Int32	shotcount;
	Int32	bulletImage;
	Int32	hitsound;
	Int32	firesound;
	Float	spread;
	Float	speed;			//Bullet speed
	Float	splashradius;
	Float	delay;			//Between shootings (in seconds)
	Vec2	bulletSize;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
extern XMLEntity	g_EntityTable[k_MaxPossibleEntityTypes];
extern XMLWeapon	g_WeaponTable[k_MaxPossibleWeapons];