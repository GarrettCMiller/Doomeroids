#include "stdafx.h"

#include "GameXMLMgr.h"

//#include "Globals.h"

#include "Creature.h"
#include "Weapon.h"
#include "Game.h"

#include "MeshMgr.h"

//////////////////////////////////////////////////////////////////////////

#define SetFunctor(name)	{m_Functors.Push(TGameXMLFunctor(this, (&GameXMLMgr::Parse##name))); m_ParseModeNames.Push(#name); }

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(GameXMLMgr);

//////////////////////////////////////////////////////////////////////////

GameXMLMgr::GameXMLMgr()
{
	m_bLoadAllSets	= true;

	m_Functors.Resize(20);

	TGameXMLFunctor functor;
	UInt32 nID = m_ParseModeNames.Size();

	SetFunctor(EntitySet);
	SetFunctor(Entity);

	SetFunctor(WeaponSet);
	SetFunctor(Weapon);

	SetFunctor(ImageSet);
	SetFunctor(Image);
	SetFunctor(Set);

	SetFunctor(Mesh);

	SetFunctor(Sound);

	SetFunctor(Equipment);
	SetFunctor(Equip);
}

//////////////////////////////////////////////////////////////////////////

GameXMLMgr::~GameXMLMgr()
{

}

//////////////////////////////////////////////////////////////////////////

GameXMLMgr::TGameXMLFunctor* GameXMLMgr::GetFunctor(String name)
{
	for (Int32 i = 0; i < m_ParseModeNames.Size(); i++)
		if (_stricmp(name.c_str(), m_ParseModeNames[i].c_str()) == 0)
			return &m_Functors[i];

	//ASSERT(false);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::Parse(TiXmlElement* e)
{
	for (TiXmlElement* element = e->FirstChildElement(); element != NULL; element = element->NextSiblingElement())
	{
		if (m_bStopReading)
			return;

		String valName = element->Value();

		TGameXMLFunctor* functor = GetFunctor(valName);

		if (functor)
			(*functor)(element);
// 		else	
// 			Parse(element);
	}
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ReadGameDefinition()
{
	g_Log << "Reading game definition...\n";

	ASSERT(m_bSettingsRead);

	m_bStopReading = false;
	m_bLoadAllSets = true;

	m_ParseModes.Push(eMode_Root);

	TiXmlElement* root = m_GameDefDoc.RootElement();

	Parse(root);

	m_ParseModes.Pop();

	ASSERT(m_ParseModes.IsEmpty());

	g_Log << "Game definition successfully read!\n";
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::LoadGameDefinition()
{
	g_Log << "Loading game definition...\n";

	if (!m_GameDefDoc.LoadFile("GameDefinition.xml"))
	{
		g_Log.LOG_ERROR("Couldn't open game definition file!\n");
		ASSERT(false);
		return;
	}

	g_Log << "Successfully loaded game definition!\n";
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseEntitySet(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_EntitySet);
	Parse(e);
	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseEntity(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_Entity);

	XMLEntity ent;
	Bool bIsProp = false;

	const char* strname = e->Attribute("class");
	const char* strtype = e->Attribute("type");

	ASSERT(strname);

	/*if (AttributeIs("class", "prop"))
	{
		bIsProp = true;
		ent.name = strtype;
	}
	else*/
		ent.name = strname;
	
	Creature::TypeIdNames[EntityNum] = ent.name;

	e->QueryIntAttribute("hp",		&ent.hp);
	e->QueryIntAttribute("maxhp",	&ent.maxhp);

	ent.maxhp		= max(ent.maxhp, ent.hp);
	ent.hp			= min(ent.maxhp, ent.hp);

	ent.IsValid		= true;

	ASSERT(EntityNum < k_MaxPossibleEntityTypes);

	//Move the data to the table
	memcpy(g_EntityTable + EntityNum, &ent, sizeof(XMLEntity));

	//Check for child items like image sets, sounds, and weapons
	Parse(e);

	//Next entity
	EntityNum++;

	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseWeaponSet(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_WeaponSet);
	Parse(e);
	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseWeapon(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_Weapon);

	XMLWeapon wpn;

	wpn.name = e->Attribute("Name");

	Weapon::WeaponNames[WeaponNum] = wpn.name;

	e->QueryIntAttribute("Damage",			&wpn.damage);
	e->QueryIntAttribute("SplashDamage",	&wpn.splashdmg);
	e->QueryIntAttribute("Shots",			&wpn.shotcount);
	e->QueryFloatAttribute("SplashRadius",	&wpn.splashradius);
	e->QueryFloatAttribute("Spread",		&wpn.spread);
	e->QueryFloatAttribute("FireDelay",		&wpn.delay);
	e->QueryFloatAttribute("BulletSpeed",	&wpn.speed);

	wpn.IsValid = true;

	ASSERT(WeaponNum < k_MaxPossibleWeapons);

	//Move the data to the table
	memcpy(g_WeaponTable + WeaponNum, &wpn, sizeof(XMLWeapon));

	//Check for any children (after the memcpy, so that the child data isn't written over)
	Parse(e);

	//Next weapon
	WeaponNum++;

	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseImageSet(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_ImageSet);

	char base[32] = {'\0'};
	m_bLoadAllSets = true;

	strncpy(base, e->Attribute("src"), 32);

	if (AttributeIs("set", "custom"))
		Parse(e);	//Check for sub-set information (frame limits)
	else if (AttributeIs("set", "partial"))
	{
		m_bLoadAllSets = false;
		m_ImageSetBase = base;
		Parse(e);
	}

	if (m_bLoadAllSets)
	{
		switch (m_ParseModes[1])	//1 because the current mode is image set
		{
		case eMode_Entity:
			Entity::LoadFrameSet(g_EntityTable[EntityNum].images, g_EntityTable[EntityNum].maxframes, base, EAS_ALL);
			break;

		default:
			ASSERT(false);
		}
	}

	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseEquipment(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_Equipment);
	Parse(e);
	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseEquip(TiXmlElement* e)
{
	Int32 index = -1;

	if (AttributeIs("type", "weapon") && m_ParseModes[1] == eMode_Entity)
	{
		index = Weapon::NameToType(e->Attribute("name"));
		g_EntityTable[EntityNum].weapons[index] = true;
		g_EntityTable[EntityNum].currentweapon = (EntityNum ? index : 1);
	}
	else
		ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseSet(TiXmlElement* e)
{
	char src[32] = {'\0'};
	Int32 maxframes = 0;
	Int32 index = -1;
	Int32 directional = 1;

	strncpy(src, e->Attribute("src"),	32);
	e->QueryIntAttribute("maxframes",	&maxframes);
	e->QueryIntAttribute("directional",	&directional);

	switch (m_ParseModes[1])
	{
	case eMode_Entity:
		index = Entity::NameToAnimState(src);
		g_EntityTable[EntityNum].maxframes[index] = maxframes;

		if (!m_bLoadAllSets || !directional)
			Entity::LoadFrameSet(g_EntityTable[EntityNum].images, g_EntityTable[EntityNum].maxframes, m_ImageSetBase, Entity::NameToAnimState(src), !(Bool(directional)));
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseMesh(TiXmlElement* e)
{
	Int32 meshID = -1;
	char path[32] = {'\0'};

	strncpy(path, e->Attribute("src"), 32);
	strncat(path, ".x", 32);

	meshID = g_MeshMgr->LoadMesh(path);

	switch (m_ParseModes[0])
	{
	case eMode_Entity:
		g_EntityTable[EntityNum].meshID = meshID;
		break;

	case eMode_Weapon:
		g_WeaponTable[WeaponNum].bulletMeshID = meshID;
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseImage(TiXmlElement* e)
{
	Float	width	= 0.0f,
			height	= 0.0f,
			image	= -1;

	char path[32] = {'\0'};

	strncpy(path, e->Attribute("src"), 32);
	strncat(path, ".bmp", 32);

	image = g_SpriteMgr->LoadSprite(path);

	switch (m_ParseModes[0])
	{
	case eMode_Weapon:
		//g_WeaponTable[WeaponNum].bulletImage = image;
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void GameXMLMgr::ParseSound(TiXmlElement* e)
{
	Int32 sound = -1;
	char src[32] = {'\0'};

	strncpy(src, e->Attribute("src"), 32);

	sound = g_SoundMgr->LoadSound(src);

	switch (m_ParseModes[0])
	{
	case eMode_Weapon:
		if (AttributeIs("type", "hit"))
			g_WeaponTable[WeaponNum].hitsound = sound;
		else
			g_WeaponTable[WeaponNum].firesound = sound;
		break;

	case eMode_Entity:
		if (AttributeIs("type", "hurt"))
			g_EntityTable[EntityNum].hurtsound = sound;
		else if (AttributeIs("type", "die"))
			g_EntityTable[EntityNum].diesound = sound;
		else
			g_EntityTable[EntityNum].alertsound = sound;
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////