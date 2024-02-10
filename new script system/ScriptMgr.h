/////////////////////////////////////////////////////////////////////////////
//		File:					ScriptMgr.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:34 AM
//
//		Description:		
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Entity.h"
#include "Singleton.h"

#include "UIObject.h"
#include "UIImage.h"
#include "UIButton.h"

#include "DynamicArray.h"
#include "Stack.h"

#include "Functor.h"
#include "Dator.h"

#include "Tree.h"

#include <map>

#define MAX_BUFFER_SIZE						256
#define MAX_TOKEN_SIZE						128
#define MAX_MSG_DATA_SIZE					4
#define MAX_SCRIPT_NAME_LENGTH				MAX_TOKEN_SIZE

#define MAX_SCRIPT_ARGUMENTS				5
#define MAX_SCRIPT_LOCAL_VARS				5

#define MAX_PARSE_TOKENS					16

#define MAX_PARSE_TREE_CHILDREN				MAX_PARSE_TOKENS
///////////////////////////////////////////////////////////////////////////////

enum EScriptCommand
{
	eCmd_Spawn,

	eCmd_Wait,

	eCmd_Send_Entity_Msg,

	eCmd_Script_Load,
	eCmd_Script_Run,

	eCmd_Menu_Cmd,

	////////////////////////////
	// Begin New Script Commands
	eCmd_Variable_Declare_Bool,
	eCmd_Variable_Declare_Int,
	eCmd_Variable_Declare_Float,
	eCmd_Variable_Declare_String,

	eCmd_Variable_Assign,

	eCmd_Variable_Increment,
	eCmd_Variable_Decrement,

	eCmd_Variable_Add,
	eCmd_Variable_Subtract,
	eCmd_Variable_Multiply,
	eCmd_Variable_Divide,
	
	eCmd_Variable_AssignAdd,
	eCmd_Variable_AssignSubtract,
	eCmd_Variable_AssignMultiply,
	eCmd_Variable_AssignDivide,

	eCmd_Variable_Compare_EqualTo,
	eCmd_Variable_Compare_NotEqualTo,
	eCmd_Variable_Compare_GreaterThan,
	eCmd_Variable_Compare_LessThan,
	eCmd_Variable_Compare_GreaterThanEqualTo,
	eCmd_Variable_Compare_LessThanEqualTo,

	// End New Script Commands
	////////////////////////////

	eCmd_MAX
};

class ScriptCommand
{
public:
	ScriptCommand()
	{ 
		ZeroMemory(strData1, MAX_BUFFER_SIZE * MAX_MSG_DATA_SIZE); 
		ZeroMemory(strData2, MAX_BUFFER_SIZE * MAX_MSG_DATA_SIZE); 
	}

	bool operator == (const ScriptCommand& sc) const
	{
		return (m_eMsg == sc.m_eMsg 
				&& 
				memcmp((void *) strData1[0], (void *) sc.strData1[0], MAX_MSG_DATA_SIZE * MAX_BUFFER_SIZE) == 0 
				&&
				memcmp((void *) strData2[0], (void *) sc.strData2[0], MAX_MSG_DATA_SIZE * MAX_BUFFER_SIZE) == 0);
	}

	bool operator != (const ScriptCommand& sc) const	{	return !((*this) == sc);	}

public:
	EScriptCommand				m_eMsg;
	
	union	
	{
		bool					bData1[MAX_MSG_DATA_SIZE];
		float					fData1[MAX_MSG_DATA_SIZE];
		UINT					uData1[MAX_MSG_DATA_SIZE];
		int						nData1[MAX_MSG_DATA_SIZE];
		char					strData1[MAX_MSG_DATA_SIZE][MAX_BUFFER_SIZE];
		EntityPtr				pData1[MAX_MSG_DATA_SIZE];
		ENTITY_MESSAGE_ITEM		eData1[MAX_MSG_DATA_SIZE];
	};

	union	
	{
		bool					bData2[MAX_MSG_DATA_SIZE];
		float					fData2[MAX_MSG_DATA_SIZE];
		UINT					uData2[MAX_MSG_DATA_SIZE];
		int						nData2[MAX_MSG_DATA_SIZE];
		char					strData2[MAX_MSG_DATA_SIZE][MAX_BUFFER_SIZE];
		EntityPtr				pData2[MAX_MSG_DATA_SIZE];
		ENTITY_MESSAGE_ITEM		eData2[MAX_MSG_DATA_SIZE];
	};
};

//////////////////////////////////////////////////////////////////////////

struct Script
{
public:
	typedef CBaseDator									Variable;
	typedef CDynamicArray<Variable*>					VariableList;

	typedef CFunctor1<Script, bool, ScriptCommand&>		CommandHandler;
	typedef std::map<EScriptCommand, CommandHandler>	CommandHandlerMap;
	typedef Queue<ScriptCommand>						CommandList;

protected:
	friend class ScriptMgr;

	Script() 
		: m_bRun(false), m_bWaiting(false), m_bActive(false), 
		m_bMenuScript(false), m_bQueued(false), m_uFrameCounter(0)		
	{
		m_StackFrame.m_Arguments.Resize(MAX_SCRIPT_ARGUMENTS);
		m_StackFrame.m_LocalVariables.Resize(MAX_SCRIPT_LOCAL_VARS);
	}

	Script(std::string name)
		: m_bRun(false), m_bWaiting(false), m_bActive(false), 
		m_bMenuScript(false), m_bQueued(false), m_uFrameCounter(0)
	{ 
		strncpy(m_strName, name.c_str(), MAX_SCRIPT_NAME_LENGTH);
		m_StackFrame.m_Arguments.Resize(MAX_SCRIPT_ARGUMENTS);
		m_StackFrame.m_LocalVariables.Resize(MAX_SCRIPT_LOCAL_VARS);
	}

	virtual ~Script()
	{
		SAFE_DELETE(m_StackFrame.m_ReturnType);
		m_StackFrame.m_Arguments.FreePointers();
		m_StackFrame.m_LocalVariables.FreePointers();
		m_StackFrame.m_Commands.Clear();
		m_CommandHandlerMap.clear();
	}

	Script& operator = (const Script& rhs)
	{
		strncpy(m_strName, rhs.m_strName, MAX_SCRIPT_NAME_LENGTH);
		strncpy(m_strFile, rhs.m_strFile, MAX_SCRIPT_NAME_LENGTH);

		m_bMenuScript = rhs.m_bMenuScript;

		int count = rhs.m_StackFrame.m_Commands.Size();

		for (int sci = 0; sci < count; sci++)
		{
			ScriptCommand sc;
			sc = rhs.m_StackFrame.m_Commands[sci];
			m_StackFrame.m_Commands.Enqueue(sc);
		}

		return *this;
	}

	virtual int Init();
	virtual int Update();

public:
	char	m_strName[MAX_SCRIPT_NAME_LENGTH];
	char	m_strFile[MAX_SCRIPT_NAME_LENGTH];

	bool	m_bActive;
	bool	m_bRun;
	bool	m_bQueued;
	bool	m_bWaiting;
	bool	m_bMenuScript;

	UINT	m_uFrameCounter;
	Timer	m_tTimer;

	CommandHandlerMap	m_CommandHandlerMap;
	///////////////////////
	//BEGIN STACK FRAME
	struct StackFrame
	{
		StackFrame()
			: m_ReturnType(NULL), m_ArgumentCount(0), m_LocalVariableCount(0)
		{	}

		//If m_ReturnType is NULL, the return type is void
		Variable*		m_ReturnType;
		
		VariableList	m_Arguments;
		VariableList	m_LocalVariables;

		UINT			m_ArgumentCount;
		UINT			m_LocalVariableCount;
		
		//TODO: Change to dynamic array as well?
		CommandList		m_Commands;
	} m_StackFrame;

	//END STACK FRAME
	///////////////////////

	//BYTE	m_SafetyZone[128]
protected:
	bool	Command_Spawn								(ScriptCommand& sc);
	bool	Command_Wait								(ScriptCommand& sc);
	bool	Command_SendEntityMessage					(ScriptCommand& sc);
	bool	Command_LoadScript							(ScriptCommand& sc);
	bool	Command_RunScript							(ScriptCommand& sc);
	bool	Command_MenuCommand							(ScriptCommand& sc);

	bool	Command_VariableDeclareBool					(ScriptCommand& sc);
	bool	Command_VariableDeclareInt					(ScriptCommand& sc);
	bool	Command_VariableDeclareFloat				(ScriptCommand& sc);
	bool	Command_VariableDeclareString				(ScriptCommand& sc);

	bool	Command_VariableAssign						(ScriptCommand& sc);

	bool	Command_VariableIncrement					(ScriptCommand& sc);
	bool	Command_VariableDecrement					(ScriptCommand& sc);

	bool	Command_VariableAdd							(ScriptCommand& sc);
	bool	Command_VariableSubtract					(ScriptCommand& sc);
	bool	Command_VariableMultiply					(ScriptCommand& sc);
	bool	Command_VariableDivide						(ScriptCommand& sc);

	bool	Command_VariableAssignAdd					(ScriptCommand& sc);
	bool	Command_VariableAssignSubtract				(ScriptCommand& sc);
	bool	Command_VariableAssignMultiply				(ScriptCommand& sc);
	bool	Command_VariableAssignDivide				(ScriptCommand& sc);

	bool	Command_VariableCompareEqualTo				(ScriptCommand& sc);
	bool	Command_VariableCompareNotEqualTo			(ScriptCommand& sc);
	bool	Command_VariableCompareGreaterThan			(ScriptCommand& sc);
	bool	Command_VariableCompareLessThan				(ScriptCommand& sc);
	bool	Command_VariableCompareGreaterThanEqualTo	(ScriptCommand& sc);
	bool	Command_VariableCompareLessThanEqualTo		(ScriptCommand& sc);
	//////////////////////////////////////////////////////////////////////////
	void	SetReturnType(String type)
	{
		if (type == "void")
			m_StackFrame.m_ReturnType	= NULL;
		else if (type == "int")
			m_StackFrame.m_ReturnType	= new CDator<int>("DSV_RETURNVALUE", 0);
		else if (type == "float")
			m_StackFrame.m_ReturnType	= new CDator<float>("DSV_RETURNVALUE", 0.0f);
		else if (type == "string")
			m_StackFrame.m_ReturnType	= new CDator<String>("DSV_RETURNVALUE", "");
		else if (type == "bool")
			m_StackFrame.m_ReturnType	= new CDator<bool>("DSV_RETURNVALUE", false);
		else
			ASSERT(false);
	}

	void	AddArgument(String type, String name)
	{
		Variable* var = NULL;

		if (type == "int")
			var	= new CDator<int>(name, 0);
		else if (type == "float")
			var	= new CDator<float>(name, 0.0f);
		else if (type == "string")
			var = new CDator<String>(name, "");
		else if (type == "bool")
			var = new CDator<bool>(name, false);
		else
			ASSERT(false);

		m_StackFrame.m_Arguments[m_StackFrame.m_ArgumentCount++] = var;
	}
};

struct MenuScript : public Script
{
	MenuScript() : Script(), m_bDone(false)
	{
		m_bMenuScript = true;
	}

	MenuScript(std::string name) : Script(name), m_bDone(false)
	{
		m_bMenuScript = true;
	}

	void Clear()
	{
		UIObject* obj = NULL;
		DblLinkedList<UIObject *>::Iterator	i(m_Objects);

		for (i.Begin(); !i.End(); ++i)
		{
			obj = i.Current();

			ASSERT(obj);

			delete obj;
		}

		m_Objects.Clear();
	}

	MenuScript& operator = (MenuScript& rhs)
	{
		Script::operator=(rhs);

		int count = rhs.m_Objects.Size();

		for (int obj = 0; obj < count; obj++)
		{
			UIObject* pObj = rhs.m_Objects[obj];
			ASSERT(pObj);
			m_Objects.Add(pObj);
		}

		return *this;
	}

	int		Update();

	bool	m_bDone;

	DblLinkedList<UIObject*> m_Objects;
};

class ScriptMgr : public Singleton<ScriptMgr>
{
public:
	typedef CFunctor<ScriptMgr, bool>			ParserHandler;
	typedef std::map<String, ParserHandler>		ParserHandlerMap;

	ParserHandlerMap	m_ParserHandlerMap;

public:
	ScriptMgr(void);
	virtual ~ScriptMgr(void);

	int				Init();
	int				Update(float dt);
	int				Shutdown();

	bool			Contains(String FileName, Script** scr = NULL);

	int				RunScript(String ScriptName, bool RunOnce = true);
	int				RunScript(Script *script, bool RunOnce = true);

	Script*			LoadScript(String FileName);

	void			KillAllMenuScripts();

	Queue<Script *>					m_ScriptsToRun;
	DblLinkedList<Script *>			m_ScriptsLoaded;

	DblLinkedList<MenuScript *>		m_MenuScriptsLoaded;
	Queue<MenuScript *>				m_MenuScriptsToRun;
	//////////////////////////////////////////////////////////////////////////
	enum EParsingScope
	{
		ePS_Global,
			ePS_Function,
				ePS_Conditional,
					ePS_Expression,

		ePS_ParsingScopeCount
	};

	struct ParseStackFrame
	{
		DPakFile*		file;
		Script*			script;
		String			prevToken;
		EParsingScope	scope;

		bool operator == (const ParseStackFrame& rhs) const
		{
			return (file		== rhs.file			&& 
					script		== rhs.script		&& 
					prevToken	== rhs.prevToken	&&
					scope		== rhs.scope);
		}

		bool operator != (const ParseStackFrame& rhs) const
		{
			return !((*this) == rhs);
		}
	};

	Stack<ParseStackFrame>		m_ParseStackFrame;
	//////////////////////////////////////////////////////////////////////////
	enum EParseTokenType
	{
		ePTT_Identifier,
		ePTT_Operator,
		ePTT_Constant,
		ePTT_String,

		ePTT_Max,

		ePTT_Unknown = -1
	};

	struct ParseToken
	{
		EParseTokenType		Type;
		String				Token;
	};

	Queue<ParseToken>		m_ParsedTokens;
	//////////////////////////////////////////////////////////////////////////
	struct ParseTokenGroup
	{
		ParseToken Token[MAX_PARSE_TOKENS];
	};
	
	CTree<ParseTokenGroup, MAX_PARSE_TREE_CHILDREN>	m_ParseTree;


private:
	inline void		ReadString(String& Buffer, DPakFile& m_File);
	inline String	ReadToken(String buffer, int UseDelimiterSet = -1);
	inline String	NextToken(int UseDelimiterSet = -1);
	inline void		RemoveLineFeed(String& buffer);

	void			Tokenize(String buffer);

private:
	bool	InitParsers();

	int		DertermineType(String Token)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	bool	Parse_Function							();

	bool	Parse_Spawn								();
	bool	Parse_Wait								();
	bool	Parse_SendEntityMessage					();
	bool	Parse_LoadScript						();
	bool	Parse_RunScript							();
	bool	Parse_MenuCommand						();

	bool	Parse_VariableDeclareBool				();
	bool	Parse_VariableDeclareInt				();
	bool	Parse_VariableDeclareFloat				();
	bool	Parse_VariableDeclareString				();

	bool	Parse_VariableAssign					();
	
	bool	Parse_VariableIncrement					();
	bool	Parse_VariableDecrement					();

	bool	Parse_VariableAdd						();
	bool	Parse_VariableSubtract					();
	bool	Parse_VariableMultiply					();
	bool	Parse_VariableDivide					();
	
	bool	Parse_VariableAssignAdd					();
	bool	Parse_VariableAssignSubtract			();
	bool	Parse_VariableAssignMultiply			();
	bool	Parse_VariableAssignDivide				();
	
	bool	Parse_VariableCompareEqualTo			();
	bool	Parse_VariableCompareNotEqualTo			();
	bool	Parse_VariableCompareGreaterThan		();
	bool	Parse_VariableCompareLessThan			();
	bool	Parse_VariableCompareGreaterThanEqualTo	();
	bool	Parse_VariableCompareLessThanEqualTo	();
};

#define g_ScriptMgr	ScriptMgr::GetInstance()