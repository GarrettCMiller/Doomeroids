/////////////////////////////////////////////////////////////////////////////
//		File:					ScriptMgr.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:38 AM
//
//		Description:		
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\scriptmgr.h"

#include "Game.h"
#include "D3D.h"
#include "Entity.h"
#include "EntityMgr.h"
#include "Map.h"
#include "MapMgr.h"
///////////////////////////////////////////////////////////////////////////////
#define GET_NEXT_TOKEN		Token = NextToken()
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SINGLETON(ScriptMgr);

static String  ScriptsBasePath = "Res\\Scripts\\";

enum EDelimiters
{
	//eDelim_NewLine,
	//eDelim_Quote,
	eDelim_WhiteSpace,
	eDelim_Semicolon,

	eDelim_Max,

	eDelim_First = eDelim_WhiteSpace

}	CurrentDelimeter;

String	k_StringToken		= "%_STRING_%",
		k_IdentifierToken	= "%_IDENTIFIER_%",
		k_ExpressionToken	= "%_EXPRESSION_%",
		k_ConstantToken		= "%_CONSTANT_%";

static char* Delimeters[] =
{
	//"\n\r",		//Newline and carriage return
	//"\"\'",		//Quotes, newline and carriage return
	" 	",		//Tab, space
};

static const int k_NumOperators = 12;
static String Operators[] = 
{
	"(",	")",	"[",	"]",	".",
	"!",
	//"*",	"/",	"%",
	//"+",	"-",
	",",
};

const String strScriptCommands[eCmd_MAX] =
{
	"Spawn",

	"Wait",

	"SendEntityMessage",

	"LoadScript",
	"RunScript",

	"MenuCommand",

	////////////////////////////
	// New ops here

	"bool",		//"VariableDeclareBool",
	"int",		//"VariableDeclareBool",
	"float",	//"VariableDeclareBool",
	"string",	//"VariableDeclareBool",

	"=",		//"VariableAssign",
	
	"++",		//"VariableIncrement",
	"--",		//"VariableDecrement",

	"+",		//"VariableAdd",
	"-",		//"VariableSubtract",
	"*",		//"VariableMultiply",
	"/",		//"VariableDivide",
	
	"+=",		//"VariableAssignAddition",
	"-=",		//"VariableAssignSubtract",
	"*=",		//"VariableAssignMultiply",
	"/=",		//"VariableAssignDivide",

	"==",		//"VariableCompareEqualTo",
	"!=",		//"VariableCompareNotEqualTo",
	">",		//"VariableCompareGreaterThan",
	"<",		//"VariableCompareLessThan",
	">=",		//"VariableCompareGreaterThanEqualTo",
	"<=",		//"VariableCompareLessThanEqualTo",
	//
	////////////////////////////
};
//////////////////////////////////////////////////////////////////////////
ScriptMgr::ScriptMgr(void)
{
}

ScriptMgr::~ScriptMgr(void)
{
	if (!Shutdown())
		g_Log.LOG_ERROR("Error shutting Script Manager down!\n");
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int ScriptMgr::Init()
{
	g_Log << "ScriptMgr::Init() begin...\n";

	InitParsers();

	g_Log << "ScriptMgr::Init() end!\n";

	return 1;
}

int ScriptMgr::Update(float dt)
{
	PROFILE_THIS;

	Script	*scr = NULL;
	MenuScript *menu = NULL;

	int retVal = 0;

	if (!m_MenuScriptsToRun.IsEmpty())
	{
		int index = m_MenuScriptsToRun.Size() - 1;

		menu = m_MenuScriptsToRun[index];

		if (menu->Update() < 0)
		{
			//menu = m_MenuScriptsToRun.Pop();
			m_MenuScriptsToRun.Dequeue(menu);
			delete menu;
		}
	}
	else
		retVal = 1;
	
	if (!m_ScriptsToRun.IsEmpty())
	{
		for (UINT s = 0; s < m_ScriptsToRun.Size(); s++)
		{
			scr = m_ScriptsToRun[s];

			if (scr->Update() < 0)
			{
				m_ScriptsToRun.Remove(scr);
				delete scr;
			}
			else
				retVal = 0;
		}
	}

	return retVal;
}

int ScriptMgr::Shutdown()
{
	g_Log << "ScriptMgr::Shutdown() begin...\n";

	char scrname[80] = {'\0'};

	DblLinkedList<Script *>::Iterator		i(m_ScriptsLoaded);
	Script*									script = NULL;

	int count = m_MenuScriptsToRun.Size();
	for (int it = 0; it < count; it++)
	{
		script = m_MenuScriptsToRun[it];
		delete script;
	}

	count = m_ScriptsToRun.Size();
	for (int it = 0; it < count; it++)
	{
		script = m_ScriptsToRun[it];
		delete script;
	}

	count = m_ScriptsLoaded.Size();

	for (i.Begin(); !i.End(); ++i)
	{
		script = i.Current();

		strncpy(scrname, script->m_strFile, 80);
		
		if (script->m_bMenuScript)
			(static_cast<MenuScript *>(script))->Clear();

		delete script;

		g_ResLog << "Freed script " << scrname << "!\n";
	}

	g_Log << "Freed " << count << " scripts!\n";
	g_ResLog << "Freed " << count << " scripts!\n";

	m_ScriptsLoaded.Clear();
	m_ScriptsToRun.Clear();

	m_ParserHandlerMap.clear();

	g_Log << "ScriptMgr::Shutdown() end!\n";

	return 1;
}

int ScriptMgr::RunScript(Script *script, bool RunOnce)
{
	DblLinkedList<Script *>::Iterator		i(m_ScriptsLoaded);
	Script						*temp = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = i.Current();

		//Compare names
		if (script == temp && !temp->m_bQueued && (RunOnce ? !temp->m_bRun : true))
		{
			Script* nc;

			if (temp->m_bMenuScript)
			{
				nc = new MenuScript;

				ASSERT(nc);

				*((MenuScript*) nc) = *((MenuScript*) temp);
			}
			else
			{
				nc = new Script;

				ASSERT(nc);

				(*nc) = (*temp);
			}

			temp->m_bRun = true;
			nc->m_bQueued = true;

			//Init the script
			nc->Init();
			
			if (nc->m_bMenuScript)
				m_MenuScriptsToRun.Enqueue((MenuScript *) nc);//Push((MenuScript *) nc);
			else
				m_ScriptsToRun.Enqueue(nc);

			g_Log << "Running script " << script->m_strName << "...\n";
			
			return 1;
		}
	}

	return 0;
}

int ScriptMgr::RunScript(std::string ScriptName, bool RunOnce)
{
	DblLinkedList<Script *>::Iterator	i(m_ScriptsLoaded);
	Script						*temp = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = i.Current();

		//Compare names
		if (!strcmp(ScriptName.c_str(), temp->m_strName) && !temp->m_bQueued && (RunOnce ? !temp->m_bRun : true))
		{
			Script* nc;

			if (temp->m_bMenuScript)
			{
				nc = new MenuScript;

				if (!nc)
					return -1;

				*((MenuScript*) nc) = *((MenuScript*) temp);
			}
			else
			{
				nc = new Script;

				if (!nc)
					return -1;

				(*nc) = (*temp);
			}

			temp->m_bRun = true;
			nc->m_bQueued = true;

			nc->Init();
			
			if (nc->m_bMenuScript)
			{
				if (!m_MenuScriptsToRun.IsEmpty() && !strcmp(m_MenuScriptsToRun[int(m_MenuScriptsToRun.Size()-1)]->m_strName, nc->m_strName))
					return -1;
				
				m_MenuScriptsToRun.Enqueue((MenuScript *) nc);//Push((MenuScript *) nc);
			}
			else
			{
				if (!m_ScriptsToRun.IsEmpty() && !strcmp(m_ScriptsToRun[int(m_ScriptsToRun.Size()-1)]->m_strName, nc->m_strName))
					return -1;

				m_ScriptsToRun.Enqueue(nc);
			}
			
			g_Log << "Running script " << ScriptName << "...\n";

			return 1;
		}
	}

	return 0;
}

bool ScriptMgr::Contains(std::string FileName, Script** scr)
{
	DblLinkedList<Script *>::Iterator		i(m_ScriptsLoaded);
	Script*									temp = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = i.Current();

		if (!strcmp(FileName.c_str(), temp->m_strFile))
		{
			*scr = temp;
			return true;
		}
	}

	/*DblLinkedList<MenuScript *>::Iterator j(m_MenuScriptsLoaded);

	for (j.Begin(); !j.End(); ++j)
	{
		temp = j.Current();

		if (!strcmp(FileName, temp->m_strFile))
		{
			*scr = temp;
			return true;
		}
	}*/

	scr = NULL;

	return false;
}

String ScriptMgr::ReadToken(String buffer, int UseDelimiterSet)
{
	static char k_Buffer[MAX_BUFFER_SIZE * 2];
	
	ZeroMemory(k_Buffer, MAX_BUFFER_SIZE * 2);

	strncpy(k_Buffer, buffer.c_str(), MAX_BUFFER_SIZE * 2);

	char* temp = strtok(k_Buffer, Delimeters[UseDelimiterSet == -1 ? CurrentDelimeter : UseDelimiterSet]);

	return String(temp ? temp : "");
}

String ScriptMgr::NextToken(int UseDelimiterSet)
{
	char* temp = strtok(NULL, Delimeters[UseDelimiterSet == -1 ? CurrentDelimeter : UseDelimiterSet]);
	return String(temp ? temp : "");
}

void ScriptMgr::Tokenize(String buffer)
{

}

Script* ScriptMgr::LoadScript(String FileName)
{
	Script*		script = NULL;
	int			ret = -1;
	DPakFile	file;

	String		Buffer;
	String		Token;

	ParserHandlerMap::iterator	func;

	ASSERT(!FileName.empty());

	if (Contains(FileName, &script))
	{
		g_Log << "Script already loaded! File: " << FileName << '\n';
		return script;
	}
	
	g_Game->pakFile.GetFileByName(file, FileName.c_str(), true);
	
	if (!file.IsValid())
		g_Game->pakFile.GetFileByName(file, String(ScriptsBasePath + FileName).c_str(), true);

	ASSERT(file.IsValid());

	char* tempbuffer = new char [file.FileSize];
	ASSERT(tempbuffer);
	ZeroMemory(tempbuffer, file.FileSize);

	file.Read(tempbuffer, file.FileSize);

	Buffer = tempbuffer;

	delete [] tempbuffer;

	//////////////////////////////////////////////////////////////////////////
	// STEP 1: TOKENIZE (lexing)
	//////////////////////////////////////////////////////////////////////////
	int firstPos = -1,
		secondPos = -1;

	while ((firstPos = Buffer.find("//")) != -1)
	{
		secondPos = Buffer.find('\n', firstPos);

		if (secondPos != -1)
		{
			Buffer.replace(firstPos, secondPos - firstPos, secondPos - firstPos, ' ');
		}
	}

	int numStringTokens = 0;

	//
	//REPLACE QUOTED STRINGS
	//
	while ((firstPos = Buffer.find('\"')) != -1)
	{
		secondPos = Buffer.find('\"', firstPos + 1);

		if (secondPos != -1)
		{
			String string = Buffer.substr(firstPos + 1, secondPos - firstPos - 1);
			String strToken = k_StringToken;
			//String temp = 6;//numStringTokens++
			Buffer.replace(firstPos, secondPos - firstPos + 1, strToken);
		}
	}

	//
	//REMOVE NEWLINES
	//
	while ((firstPos = Buffer.find('\n')) != -1)
	{
		Buffer.replace(firstPos, 1, " ");
	}

	//
	//REMOVE CARRIAGE RETURNS
	//
	while ((firstPos = Buffer.find('\r')) != -1)
	{
		Buffer.replace(firstPos, 1, " ");
	}

	//////////////////////////////////////////////////////////////////////////

	ParseToken ptoken;

	ptoken.Type = ePTT_Unknown;

	Token = ReadToken(Buffer);

	while (!Token.empty())
	{
		ptoken.Token = Token;

		if (Token == k_StringToken)
			ptoken.Type = ePTT_String;
		else
			ptoken.Type = ePTT_Unknown;

		m_ParsedTokens.Enqueue(ptoken);

		Token = NextToken();
	}

	//////////////////////////////////////////////////////////////////////////
	// STEP 2: PARSING
	//////////////////////////////////////////////////////////////////////////

	ParseStackFrame root;

	root.file		= &file;
	root.prevToken	= "";
	root.script		= NULL;

	m_ParseStackFrame.Push(root);

	ParseStackFrame frame;

	frame.file		= &file;
	frame.prevToken	= Token;
	frame.script	= root.script;

	m_ParseStackFrame.Push(frame);

	do
	{
		func = m_ParserHandlerMap.find(Token);

		if (func != m_ParserHandlerMap.end())
		{
			if (!func->second())
				ASSERT(false);
		}

	} while (!Token.empty());

	if (m_ParseStackFrame.Size() > 1)
		m_ParseStackFrame.Pop();

	m_ParseStackFrame.Pop();

	//////////////////////////////////////////////////////////////////////////
	// STEP 3: BUILD PARSE TREE
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// FINISHED!
	//////////////////////////////////////////////////////////////////////////

	g_ResLog << "Loading script \"" << script->m_strName << "\" from file " << script->m_strFile << '\n';

	m_ScriptsLoaded.Add(script);
	
	return script;
}

void ScriptMgr::KillAllMenuScripts()
{
	for (UINT s = 0; s < m_MenuScriptsToRun.Size(); s++)
		m_MenuScriptsToRun[s]->m_bDone = true;
}

///////////////////////////////////////////////////////////////////////////////
bool ScriptMgr::InitParsers()
{
#define ADD_HANDLER(name)			m_ParserHandlerMap[(strScriptCommands[i++])] = ParserHandler(this, &ScriptMgr::Parse_##name)

	int i = 0;	

	ADD_HANDLER(Spawn);
	ADD_HANDLER(Wait);
	ADD_HANDLER(SendEntityMessage);
	ADD_HANDLER(LoadScript);
	ADD_HANDLER(RunScript);
	ADD_HANDLER(MenuCommand);

	ADD_HANDLER(VariableDeclareBool);
	ADD_HANDLER(VariableDeclareInt);
	ADD_HANDLER(VariableDeclareFloat);
	ADD_HANDLER(VariableDeclareString);

	ADD_HANDLER(VariableAssign);

	ADD_HANDLER(VariableIncrement);
	ADD_HANDLER(VariableDecrement);

	ADD_HANDLER(VariableAdd);
	ADD_HANDLER(VariableSubtract);
	ADD_HANDLER(VariableMultiply);
	ADD_HANDLER(VariableDivide);
	
	ADD_HANDLER(VariableAssignAdd);
	ADD_HANDLER(VariableAssignSubtract);
	ADD_HANDLER(VariableAssignMultiply);
	ADD_HANDLER(VariableAssignDivide);

	ADD_HANDLER(VariableCompareEqualTo);
	ADD_HANDLER(VariableCompareNotEqualTo);
	ADD_HANDLER(VariableCompareGreaterThan);
	ADD_HANDLER(VariableCompareLessThan);
	ADD_HANDLER(VariableCompareGreaterThanEqualTo);
	ADD_HANDLER(VariableCompareLessThanEqualTo);

#undef ADD_HANDLER

	return true;
}

int Script::Init()
{
	 m_tTimer.Init();

#define ADD_HANDLER(cmd, func)	m_CommandHandlerMap[cmd] = CommandHandler(this, &Script::Command_##func)

	 ADD_HANDLER(eCmd_Spawn,								Spawn);
	 ADD_HANDLER(eCmd_Wait,									Wait);
	 ADD_HANDLER(eCmd_Send_Entity_Msg,						SendEntityMessage);
	 ADD_HANDLER(eCmd_Script_Load,							LoadScript);
	 ADD_HANDLER(eCmd_Script_Run,							RunScript);
	 ADD_HANDLER(eCmd_Menu_Cmd,								MenuCommand);

	 ADD_HANDLER(eCmd_Variable_Assign,						VariableAssign);

	 ADD_HANDLER(eCmd_Variable_Increment,					VariableIncrement);
	 ADD_HANDLER(eCmd_Variable_Decrement,					VariableDecrement);

	 ADD_HANDLER(eCmd_Variable_Add,							VariableAdd);
	 ADD_HANDLER(eCmd_Variable_Subtract,					VariableSubtract);
	 ADD_HANDLER(eCmd_Variable_Multiply,					VariableMultiply);
	 ADD_HANDLER(eCmd_Variable_Divide,						VariableDivide);

	 ADD_HANDLER(eCmd_Variable_AssignAdd,					VariableAssignAdd);
	 ADD_HANDLER(eCmd_Variable_AssignSubtract,				VariableAssignSubtract);
	 ADD_HANDLER(eCmd_Variable_AssignMultiply,				VariableAssignMultiply);
	 ADD_HANDLER(eCmd_Variable_AssignDivide,				VariableAssignDivide);

	 ADD_HANDLER(eCmd_Variable_Compare_EqualTo,				VariableCompareEqualTo);
	 ADD_HANDLER(eCmd_Variable_Compare_NotEqualTo,			VariableCompareNotEqualTo);
	 ADD_HANDLER(eCmd_Variable_Compare_GreaterThan,			VariableCompareGreaterThan);
	 ADD_HANDLER(eCmd_Variable_Compare_LessThan,			VariableCompareLessThan);
	 ADD_HANDLER(eCmd_Variable_Compare_GreaterThanEqualTo,	VariableCompareGreaterThanEqualTo);
	 ADD_HANDLER(eCmd_Variable_Compare_LessThanEqualTo,		VariableCompareLessThanEqualTo);

#undef ADD_HANDLER

	 return 1;
}

int Script::Update()
{
	static ScriptCommand	sc;
	static CommandHandlerMap::iterator func;

	if (!m_StackFrame.m_Commands.IsEmpty())
	{
		if (!m_bRun)
		{
			m_StackFrame.m_Commands.Dequeue(sc);
			m_bRun = true;
		}

		func = m_CommandHandlerMap.find(sc.m_eMsg);

		if (func == m_CommandHandlerMap.end())
		{
			ASSERT(false);
			g_Log.LOG_ERROR("Unable to process command!\n");
		}
		else
		{
			if (!func->second(sc))
				return 1;
		}

		m_bRun = false;
	}
	else
		//We are done, return a negative value
		return -1;

	return 0;
}

///////////////////////////////////

int MenuScript::Update()
{
	if (m_bDone)
		return -1;

	DblLinkedList<UIObject*>::Iterator		i(m_Objects);

	UIObject *obj= NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		obj = i.Current();

		obj->Update();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool Script::Command_Spawn(ScriptCommand& sc)
{
	Monster::Spawn(sc.strData1[0], false, Vec2(sc.fData2[0], sc.fData2[1]));
	return true;
}

bool Script::Command_Wait(ScriptCommand& sc)
{
	//If waiting in seconds
	if (sc.bData2[0])
	{
		if (!m_bWaiting)
		{
			m_tTimer.Reset();
			m_bWaiting = true;
			return false;
		}
		else if (m_tTimer.Get() >= sc.fData1[0])
			m_bWaiting = false;
		else
			return false;
	}
	else
	{
		if (!m_bWaiting)
		{
			m_uFrameCounter = 0;
			m_bWaiting = true;
			return false;
		}
		else if (++m_uFrameCounter >= UINT (sc.fData1[0]))
			m_bWaiting = false;
		else
			return false;
	}

	return true;
}

bool Script::Command_SendEntityMessage(ScriptCommand& sc)
{
	ASSERT(sc.pData1[0]);
	sc.pData1[0]->SendMessage(sc.eData2[0]);
	return true;
}

bool Script::Command_LoadScript(ScriptCommand& sc)
{
	ASSERT(sc.strData1[0]);
	g_ScriptMgr->LoadScript(sc.strData1[0]);
	return true;
}

bool Script::Command_RunScript(ScriptCommand& sc)
{
	ASSERT(sc.strData1[0]);
	g_ScriptMgr->RunScript(g_ScriptMgr->LoadScript(sc.strData1[0]));
	return true;
}

bool Script::Command_MenuCommand(ScriptCommand& sc)
{
	if (!strcmp(sc.strData1[0], "back"))
	{
		g_ScriptMgr->KillAllMenuScripts();
	}
	else if (!strcmp(sc.strData1[0], "new_game"))
	{
		g_ScriptMgr->KillAllMenuScripts();
		g_Game->NewGame();
	}
	else if (!strcmp(sc.strData1[0], "new_game_server"))
	{
		g_ScriptMgr->KillAllMenuScripts();
		g_Game->NewGame(NGM_Server);
	}
	else if (!strcmp(sc.strData1[0], "new_game_client"))
	{
		g_ScriptMgr->KillAllMenuScripts();
		g_Game->NewGame(NGM_Client);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Script::Command_VariableAssign(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] = sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] = sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData2[0] = sc.bData2[1];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableIncrement(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] += 1.0f;
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0]++;
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableDecrement(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] -= 1.0f;
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0]--;
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableAssignAdd(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] += sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] += sc.nData2[1];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableAssignSubtract(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] -= sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] -= sc.nData2[1];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableAssignMultiply(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] *= sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] *= sc.nData2[1];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableAssignDivide(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] /= sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] /= sc.nData2[1];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableAdd(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] = sc.fData2[1] + sc.fData2[2];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] = sc.nData2[1] + sc.nData2[2];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableSubtract(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] = sc.fData2[1] - sc.fData2[2];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] = sc.nData2[1] - sc.nData2[2];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableMultiply(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] = sc.fData2[1] * sc.fData2[2];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] = sc.nData2[1] * sc.nData2[2];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

bool Script::Command_VariableDivide(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.fData2[0] = sc.fData2[1] / sc.fData2[2];
		break;

	case MessageTranslator::DT_INT:
		sc.nData2[0] = sc.nData2[1] / sc.nData2[2];
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Script::Command_VariableCompareEqualTo(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.bData1[0] = sc.fData2[0] == sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.bData1[0] = sc.nData2[0] == sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData1[0] = sc.bData2[0] == sc.bData2[1];
		break;

	default:
		ASSERT(false);
	}

	return true;
}

bool Script::Command_VariableCompareNotEqualTo(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.bData1[0] = sc.fData2[0] != sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.bData1[0] = sc.nData2[0] != sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData1[0] = sc.bData2[0] != sc.bData2[1];
		break;

	default:
		ASSERT(false);
	}


	return true;
}

bool Script::Command_VariableCompareGreaterThan(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.bData1[0] = sc.fData2[0] > sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.bData1[0] = sc.nData2[0] > sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData1[0] = sc.bData2[0] > sc.bData2[1];
		break;

	default:
		ASSERT(false);
	}

	return true;
}

bool Script::Command_VariableCompareLessThan(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.bData1[0] = sc.fData2[0] < sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.bData1[0] = sc.nData2[0] < sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData1[0] = sc.bData2[0] < sc.bData2[1];
		break;

	default:
		ASSERT(false);
	}

	return true;
}

bool Script::Command_VariableCompareGreaterThanEqualTo(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.bData1[0] = sc.fData2[0] >= sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.bData1[0] = sc.nData2[0] >= sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData1[0] = sc.bData2[0] >= sc.bData2[1];
		break;

	default:
		ASSERT(false);
	}

	return true;
}

bool Script::Command_VariableCompareLessThanEqualTo(ScriptCommand& sc)
{
	switch (sc.nData1[0])
	{
	case MessageTranslator::DT_FLOAT:
		sc.bData1[0] = sc.fData2[0] <= sc.fData2[1];
		break;

	case MessageTranslator::DT_INT:
		sc.bData1[0] = sc.nData2[0] <= sc.nData2[1];
		break;

	case MessageTranslator::DT_BOOL:
		sc.bData1[0] = sc.bData2[0] <= sc.bData2[1];
		break;

	default:
		ASSERT(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ScriptMgr::Parse_Function()
{
	ScriptCommand sc;
	ParseStackFrame& frame = m_ParseStackFrame[0];

	String Token = frame.prevToken;
	String scriptName = NextToken();

	frame.script = new Script(scriptName);
	
	ASSERT(frame.script);
	
	//strncpy(frame.script->m_strFile, FileName.c_str(), MAX_SCRIPT_NAME_LENGTH);

	frame.script->SetReturnType(Token);

	Token = NextToken();

	while (!Token.empty() && frame.script->m_StackFrame.m_ArgumentCount < MAX_SCRIPT_ARGUMENTS)
	{
		frame.script->AddArgument(Token, NextToken());
		Token = NextToken();
	}

	return true;
}

bool ScriptMgr::Parse_Spawn()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Spawn;

	GET_NEXT_TOKEN;

	if (Token == "soldier"		|| Token == "sergeant"	|| Token == "imp" || 
		Token == "cacodemon"	|| Token == "baron"		|| Token == "cyberdemon")
	{
		strcpy(sc.strData1[0], Token.c_str());

		GET_NEXT_TOKEN;

		float xp,	yp;

		if (Token == "width")
			xp = float(g_D3D->GetScreenWidth());
		else if (Token == "random")
			xp = Random(float(g_D3D->GetScreenWidth()), 0.0f);
		else
			xp = float(atof(Token.c_str()));

		GET_NEXT_TOKEN;

		if (Token == "height")
			yp = float(g_D3D->GetScreenHeight());
		else if (Token == "random")
			yp = Random(float(g_D3D->GetScreenHeight()), 0.0f);
		else
			yp = float(atof(Token.c_str()));

		sc.fData2[0] = xp;
		sc.fData2[1] = yp;

		script->m_StackFrame.m_Commands.Enqueue(sc);
	}
	else
	{
		sprintf(g_ErrorBuffer, "Bad spawn type! Type: %s\n", Token);
		g_Log.LOG_ERROR(g_ErrorBuffer);
		return false;
	}

	return true;
}

bool ScriptMgr::Parse_Wait()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Wait;

	GET_NEXT_TOKEN;

	if ((sc.fData1[0] = float(atof(Token.c_str()))) == 0.0f)
	{
		g_Log.LOG_ERROR("Bad wait time!\n");
		return false;
	}

	GET_NEXT_TOKEN;

	if (Token == "seconds")
		sc.bData2[0] = true;	
	else if (Token == "frames")
		sc.bData2[0] = false;
	else
	{
		sprintf(g_ErrorBuffer, "Bad wait type! Type: %s\n", Token);
		g_Log.LOG_ERROR(g_ErrorBuffer);
		return false;
	}

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_SendEntityMessage()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Send_Entity_Msg;

	GET_NEXT_TOKEN;

	sc.pData1[0] = g_EntityMgr->GetByName(Token);

	ASSERT(sc.pData1[0]);

	GET_NEXT_TOKEN;

	for (int string = 0; string < NUM_ENTITY_MESSAGES; string++)
	{
		if (Token == EntityMessageText[string])
		{
			sc.eData2[0].eMsg = ENTITY_MESSAGE(string);
			goto found;
		}
	}

	ASSERT(false);
	return false;

found:
	MessageTranslator mt = EntityMessageTranslators[sc.eData2[0].eMsg];

	if (mt.m_uCount == 0 || mt.m_eDataType == MessageTranslator::DT_NONE)
	{
		script->m_StackFrame.m_Commands.Enqueue(sc);
		return true;
	}

	for (UINT times = 0; times < mt.m_uCount; times++)
	{
		GET_NEXT_TOKEN;

		switch (mt.m_eDataType)
		{
		case MessageTranslator::DT_ENTITY:
			sc.eData2[0].pData[times] = g_EntityMgr->GetByName(Token);
			break;

		case MessageTranslator::DT_FLOAT:
			sc.eData2[0].fData[times] = float(atof(Token.c_str()));
			break;

		case MessageTranslator::DT_INT:
			sc.eData2[0].nData[times] = atoi(Token.c_str());
			break;

		case MessageTranslator::DT_UINT:
			sc.eData2[0].uData[times] = UINT(atoi(Token.c_str()));
			break;

		default:
			ASSERT(false);
			g_Log.LOG_ERROR("BAD MESSAGE TRANSLATOR DATA TYPE!\n");
			return false;
		}
	}

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_LoadScript()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Script_Load;

	GET_NEXT_TOKEN;

	strncpy(sc.strData1[0], Token.c_str(), MAX_BUFFER_SIZE);

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_RunScript()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Script_Run;

	GET_NEXT_TOKEN;

	strncpy(sc.strData1[0], Token.c_str(), MAX_BUFFER_SIZE);

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_MenuCommand()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	GET_NEXT_TOKEN;

	sc.m_eMsg = eCmd_Menu_Cmd;

	strcpy(sc.strData1[0], Token.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool ScriptMgr::Parse_VariableDeclareBool()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Variable_Declare_Bool;

	//Get the name of the variable
	GET_NEXT_TOKEN;

	//Save the name of the variable
	strcpy(sc.strData1[0], Token.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableDeclareInt()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Variable_Declare_Int;

	//Get the name of the variable
	GET_NEXT_TOKEN;

	//Save the name of the variable
	strcpy(sc.strData1[0], Token.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableDeclareFloat()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Variable_Declare_Float;
	
	//Get the name of the variable
	GET_NEXT_TOKEN;

	//Save the name of the variable
	strcpy(sc.strData1[0], Token.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableDeclareString()
{
	ScriptCommand sc;
	String Token;
	ParseStackFrame frame = m_ParseStackFrame[0];
	Script* script = frame.script;

	sc.m_eMsg = eCmd_Variable_Declare_String;

	//Get the name of the variable
	GET_NEXT_TOKEN;

	//Save the name of the variable
	strcpy(sc.strData1[0], Token.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool ScriptMgr::Parse_VariableAssign()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken,
			rhs = NextToken();

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());
	strcpy(sc.strData1[1], rhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableIncrement()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Increment;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// FROM HERE DOWN
//////////////////////////////////////////////////////////////////////////
bool ScriptMgr::Parse_VariableDecrement()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Decrement;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableAdd()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Add;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableSubtract()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableMultiply()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableDivide()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableAssignAdd()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableAssignSubtract()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableAssignMultiply()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableAssignDivide()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableCompareEqualTo()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableCompareNotEqualTo()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableCompareGreaterThan()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableCompareLessThan()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableCompareGreaterThanEqualTo()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}

bool ScriptMgr::Parse_VariableCompareLessThanEqualTo()
{
	ScriptCommand sc;
	ParseStackFrame frame	= m_ParseStackFrame[0];
	Script* script			= frame.script;

	sc.m_eMsg = eCmd_Variable_Assign;

	String	lhs = frame.prevToken;

	//Save the name of the variables
	strcpy(sc.strData1[0], lhs.c_str());

	script->m_StackFrame.m_Commands.Enqueue(sc);

	return true;
}