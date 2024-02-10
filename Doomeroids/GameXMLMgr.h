///////////////////////////////////////////////////////////////////////////
//		File:				GameXMLMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Handles reading of XML documents, including
//							the game definition
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "XMLMgr.h"
#include "GameXMLTables.h"

//////////////////////////////////////////////////////////////////////////

static Int32 WeaponNum = 0;
static Int32 EntityNum = 0;

//////////////////////////////////////////////////////////////////////////

class GameXMLMgr : public XMLMgr, public Singleton<GameXMLMgr>
{
public:
	GameXMLMgr();
	~GameXMLMgr();

	typedef CFunctor1<GameXMLMgr, void, TiXmlElement*>	TGameXMLFunctor;

	void	LoadGameDefinition();
	void	ReadGameDefinition();

protected:
	enum	EParseMode
	{
		eMode_EntitySet,
		eMode_Entity,

		eMode_WeaponSet,
		eMode_Weapon,

		eMode_ImageSet,
		eMode_Equipment,

		eMode_Max
	};

	String	m_ImageSetBase;
	Bool	m_bLoadAllSets;

	TStringArray			m_ParseModeNames;
	CDynamicArray<TGameXMLFunctor>	m_Functors;

protected:
	TGameXMLFunctor* GetFunctor(String name);

	void	Parse(TiXmlElement* e);

	void	ParseEntitySet(TiXmlElement* e);
	void	ParseEntity(TiXmlElement* e);

	void	ParseWeaponSet(TiXmlElement* e);
	void	ParseWeapon(TiXmlElement* e);

	void	ParseImageSet(TiXmlElement* e);
	void	ParseImage(TiXmlElement* e);
	void	ParseSet(TiXmlElement* e);

	void	ParseMesh(TiXmlElement* e);

	void	ParseSound(TiXmlElement* e);

	void	ParseEquipment(TiXmlElement* e);
	void	ParseEquip(TiXmlElement* e);

	//////////////////////////////////////////////////////////////////////////
	void	LoadImageSet(TiXmlElement* e);
};

#define g_GameXMLMgr GameXMLMgr::GetInstance()