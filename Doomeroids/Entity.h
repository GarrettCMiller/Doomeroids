///////////////////////////////////////////////////////////////////////////
//		File:				Entity.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		8/7/2005
//
//		Description:		Your basic entity
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "MemMgr.h"

#include "Vec.h"
#include "Rect.h"
#include "SoundMgr.h"
#include "PriorityQueue.h"
//#include "SmartPointer.h"
//#include "MemObj.h"
#include "SpriteMgr.h"
#include "EffectMgr.h"

#include "VectorGraphics.h"

#include "AnimMesh.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_ANIMATIONS					9
#define MAX_ANIMATION_FRAMES			32

//Number of sprite directions (360 / 45 = 8)
#define NUM_DIRECTIONS					8

//Initial defaults
#define MAX_IDLE_FRAMES					3
#define MAX_WALK_FRAMES					3
#define MAX_RUN_FRAMES					3
#define MAX_ATTACK_FRAMES				5

//Max number of data slots to use in the entity message system
#define MAX_MSG_DATA_SIZE				4

#define MAX_ENTITY_NAME_LENGTH			32

#define DEFAULT_ENTITY_SHADER_ID		0

#if (DEFAULT_ENTITY_SHADER_ID == 0)
	#define DEFAULT_ENTITY_SHADER_TECH	DEFAULT_PER_PIXEL_LIGHTING_SHADER_TECH
#else
	#define DEFAULT_ENTITY_SHADER_TECH	NULL
#endif

//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Used to translate a scripted entity message to a usable form
struct MessageTranslator
{
	//Number of data slots expected to be filled
	UInt32	m_uCount;

	//Type of those data slots
	enum DataType
	{
		DT_FLOAT,
		DT_UINT,
		DT_INT,
		DT_ENTITY,
		DT_BOOL,

		NUM_DATA_TYPES,

		DT_NONE			//No data expected for current message
	} m_eDataType;
};

//REMOVE PREVIOUS DEFINITION
#undef EM_INVALID

/////////////////////////////////////////////////
//New Message Checklist
/////////////////////////////////////////////////
//
//		*		Add to enum below
//
//		*		Add text below that
//
//		*		Add message translator
//
//		*		Handle message in ProcMessages
//
//		*		For entity-derivatives, add handler function
//
/////////////////////////////////////////////////

enum ENTITY_MESSAGE
{
	EM_MOVE,
	EM_MOVETOENTITY,
	EM_MOVETOLOCATION,
	
	EM_ATTACK,
	
	EM_BLAST,

	EM_HURT,
	EM_DIE,

	EM_COLLIDE,

	EM_REMOVE,

	NUM_ENTITY_MESSAGES,

	EM_INVALID
};

//////////////////////////////////////////////////////////////////////////

extern const char* EntityMessageText[NUM_ENTITY_MESSAGES];

const MessageTranslator EntityMessageTranslators[NUM_ENTITY_MESSAGES] = 
{
	{ 2,	MessageTranslator::DT_FLOAT},			//MOVE
	{ 1,	MessageTranslator::DT_ENTITY},			//MOVETOENTITY
	{ 2,	MessageTranslator::DT_FLOAT},			//MOVETOLOCATION
	
	{ 1,	MessageTranslator::DT_ENTITY},			//ATTACK

	{ 2,	MessageTranslator::DT_FLOAT},			//BLAST

	{ 1,	MessageTranslator::DT_INT},				//HURT
	{ 0,	MessageTranslator::DT_NONE},			//DIE

	{ 1,	MessageTranslator::DT_ENTITY},			//COLLIDE
	
	{ 0,	MessageTranslator::DT_NONE},			//REMOVE
};

///////////////////////////////////////////////////////////////////////////////

enum		EEntityAnimationState
{
	EAS_IDLE,
	EAS_WALK,
	EAS_RUN,
	EAS_ATTACK,
	EAS_SHOOT,
	EAS_HURT,
	EAS_DIE,
	EAS_SPLATTER,
	EAS_DEAD,

	NUM_ANIMATION_STATES,

	EAS_ALL			//Specify in LoadFrames() to attempt to load all possible animation sets
};

extern const char*  EntityAnimationStates[NUM_ANIMATION_STATES];

enum		EEntityState
{
	ES_IDLE,
	ES_MOVE,

	ES_ATTACK,
	ES_FOLLOW,
	ES_FIND,
	ES_FLEE,

	ES_DEAD,

	NUM_ENTITY_STATES
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Entity;

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<Entity>		EntityPtr;
#else
typedef Entity*						EntityPtr;
#endif

//////////////////////////////////////////////////////////////////////////

struct	ENTITY_MESSAGE_ITEM/* : public CMemObj*/
{
	/*ENTITY_MESSAGE_ITEM()
	{
		ZeroMemory(fData, sizeof(UInt32) * MAX_MSG_DATA_SIZE);
		eMsg = EM_INVALID;
	}*/

	void Init(ENTITY_MESSAGE em)
	{
		eMsg = em;
		ZeroMemory(fData, sizeof(UInt32) * MAX_MSG_DATA_SIZE);
	}

	void Init(ENTITY_MESSAGE em, Float* pfdata, Int32 numargs = 1)
	{
		ZeroMemory(fData, sizeof(UInt32) * MAX_MSG_DATA_SIZE);
		eMsg = em;
		memcpy(fData, pfdata, sizeof(Float) * (numargs > MAX_MSG_DATA_SIZE ? MAX_MSG_DATA_SIZE : numargs));
	}

	void Init(ENTITY_MESSAGE em, UInt32* pudata, Int32 numargs = 1)
	{
		ZeroMemory(fData, sizeof(UInt32) * MAX_MSG_DATA_SIZE);
		eMsg = em;
		memcpy(uData, pudata, sizeof(UInt32) * (numargs > MAX_MSG_DATA_SIZE ? MAX_MSG_DATA_SIZE : numargs));
	}

	void Init(ENTITY_MESSAGE em, Int32* pndata, Int32 numargs = 1)
	{
		ZeroMemory(fData, sizeof(Int32) * MAX_MSG_DATA_SIZE);
		eMsg = em;
		memcpy(nData, pndata, sizeof(Int32) * (numargs > MAX_MSG_DATA_SIZE ? MAX_MSG_DATA_SIZE : numargs));
	}

	void Init(ENTITY_MESSAGE em, EntityPtr& pdata, Int32 numargs = 1)
	{
		ZeroMemory(fData, sizeof(EntityPtr) * MAX_MSG_DATA_SIZE);
		eMsg = em;
		memcpy(&pData, &pdata, sizeof(EntityPtr) * (numargs > MAX_MSG_DATA_SIZE ? MAX_MSG_DATA_SIZE : numargs));
	}

	void Init(ENTITY_MESSAGE_ITEM &emi)
	{
		ZeroMemory(fData, sizeof(Float) * MAX_MSG_DATA_SIZE);
		eMsg = emi.eMsg;
		memcpy(fData, emi.fData, sizeof(Float) * MAX_MSG_DATA_SIZE);
	}

	ENTITY_MESSAGE	eMsg;

	union	
	{
		Float		fData[MAX_MSG_DATA_SIZE];
		UInt32		uData[MAX_MSG_DATA_SIZE];
		Int32		nData[MAX_MSG_DATA_SIZE];
		EntityPtr	pData[MAX_MSG_DATA_SIZE];
	};
	
	Bool operator == (ENTITY_MESSAGE_ITEM& rhs) const
	{
		return (eMsg == rhs.eMsg);
	}

	Bool operator != (ENTITY_MESSAGE_ITEM& rhs) const
	{
		return !((*this) == rhs);
	}
};

typedef ENTITY_MESSAGE_ITEM EMI;

#ifdef USING_SMART_POINTERS
typedef CIntrusivePtr<EMI>		EMIPtr;
#else
typedef EMI*					EMIPtr;
#endif

///////////////////////////////////////////////////////////////////////////////

enum CLASS_TYPE
{
	CT_ENTITY,
		CT_CREATURE,
			CT_PLAYER,
			CT_MONSTER,
			CT_PROP,
		CT_BULLET,
		CT_ITEM,
	//CT_WEAPON,
};

///////////////////////////////////////////////////////////////////////////////

class Entity// : public CMemObj
{
	//
	//Static
	//
protected:
	static UInt32		m_suNextID;
	static UInt32		m_uBoxImage;
	
	static Bool			m_bDrawBBoxes;

	static const Vec3	ms_RunSpeed;
	
public:
	static Vec3			m_svMaxVelocity;

	//Member vars
public:
	CLASS_TYPE		m_eClassType;

	String			m_strName;
	
	Int32			m_nID;
	Int32			m_ShaderID;
	D3DXHANDLE		m_ShaderHandle;

	Bool			m_bValid;
	Bool			m_bVisible;
	Bool			m_bDestroy;
	Bool			m_bBlasted;		//Last hit by an explosion (splash damage, i.e. in the blast radius)

	PriorityQueue<EMI>		m_MsgQueue;

	Vec3			m_vPosition;
	Vec3			m_vOrientation;
	Vec3			m_vVelocity;
	Vec3			m_vAcceleration;
	Vec3			m_vSize;
	Vec2			m_vFootPos;
	Vec3			m_vTargetPosition;

	Vec3			m_vRight;
	Vec3			m_vUp;
	Vec3			m_vForward;

	GLib::Rect3D	m_BBox;

	EEntityState	m_eState;

	EntityPtr		m_pTargetEntity;
	EntityPtr		m_pParentEntity;

	//Animation relative stuff
	//Timer			m_tAnimationTimer;

	EEntityAnimationState	m_eCurrentAnimState;

	Float			m_fCurrentFrame;
	Float			m_fAngle;
	Float			m_fScale;
	UCHAR			m_uDirection;
	
	static const Float	mk_fFriction;

	Int32				m_uMaxFrames[NUM_ANIMATION_STATES];
	Int32				m_uImage[NUM_ANIMATION_STATES][MAX_ANIMATION_FRAMES];
	//Int32*			m_uMaxFrames;
	//Int32*			m_uImage;

	CAnimMesh*		m_pMesh;

	virtual void HandleAnimation(Float dt);
	virtual void HandleCollision(EntityPtr ent);
	virtual void HandleDeath();
	virtual void HandleHurting(Int32 amt);
	virtual void HandleAttacking();
	virtual void HandleMoving();
	virtual void HandleBlast(Float x, Float y);

	void		 SetAnimation(EEntityAnimationState eas);

	virtual void Draw();
	virtual void CheckCollisions();

	virtual void DrawBBox();
	
	virtual Bool Collision(EntityPtr ent);

	virtual void CheckDestroy();
	virtual void Remove();


public:
	Entity();
	virtual ~Entity(void);

	//Initialize the entity
	virtual Bool Init(Vec3 pos = k_v3Zero);

	virtual Bool InitMesh(UInt32 meshID);

	//Called once per frame to update the entity
	virtual void Update(Float dt);

	//Load in specified animation images from file
	Int32	LoadFrames(String BasePath, 
					EEntityAnimationState state = EAS_WALK);

	static Int32 LoadFrameSet(Int32 images[NUM_ANIMATION_STATES][MAX_ANIMATION_FRAMES], 
							Int32 mymaxframes[NUM_ANIMATION_STATES],
							String  BasePath,
							EEntityAnimationState state = EAS_WALK,
							Bool bOneDirection = false);

	static EEntityAnimationState NameToAnimState(String  name);

	//Look at a specific point/entity (specifies direction of sprite)
	void	LookAt(Entity &ent);
	void	LookAt(Vec2 pos);
	void	LookAt(Float x, Float y);

	void	MoveRight(Float dx);
	void	MoveUp(Float dy);
	void	MoveForward(Float dz);

	Vec3	GetForward() const		{	return m_vForward;	}
	Vec3	GetUp() const			{	return m_vUp;		}
	Vec3	GetRight() const		{	return m_vRight;	}

	void	RotateForward(Float theta);
	void	TurnRight(Float theta);
	void	RotateRight(Float theta);

	//Send a message and optional data to the entity
	void	SendMessage(ENTITY_MESSAGE msg);
	void	SendMessage(ENTITY_MESSAGE msg, EntityPtr& MiscData, UInt32 DataCount = 1);
	void	SendMessage(ENTITY_MESSAGE msg, Float *MiscData, UInt32 DataCount = 1);
	void	SendMessage(ENTITY_MESSAGE msg, UInt32 *MiscData, UInt32 DataCount = 1);
	void	SendMessage(ENTITY_MESSAGE msg, Int32 *MiscData, UInt32 DataCount = 1);
	void	SendMessage(ENTITY_MESSAGE_ITEM &emi);

	//Returns false if deleted
	Bool	ProcMessages();

	inline
	void	SetName(String NewName)
	{
		m_strName = NewName;
	}

	void* operator new (size_t size)
	{
		return g_MemMgr->AllocateMem(size, "Entity", 49, "HERE");
	}
	
	void operator delete (void* pMem)
	{
		g_MemMgr->Release(pMem);
	}

	//AUTO_SIZE;

	static CVectorGraphics	mk_vg;
};
