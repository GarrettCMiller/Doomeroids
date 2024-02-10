#include "StdAfx.h"
#include "GameScript.h"

#include "Game.h"

CGameScript::CGameScript(void) : Script()
{
}

CGameScript::CGameScript(String name) : Script(name)
{
}

CGameScript::~CGameScript(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CGameScript::Update()
{
	static ScriptCommand	sc;

	if (!m_Commands.IsEmpty())
	{
		if (!m_bRun)
		{
			m_Commands.Dequeue(sc);
			m_bRun = true;
		}

		switch (sc.m_eMsg)
		{
		case eCmd_Spawn:
			Monster::Spawn(String(sc.m_Args[0]), false, Vec2(sc.m_Args[1], sc.m_Args[2]));
			break;

		case eCmd_Wait:
			if (sc.m_Args[0])
			{
				if (!m_bWaiting)
				{
					m_tTimer.Reset();
					m_bWaiting = true;
					return 1;
				}
				else if (m_tTimer.Get() >= Float(sc.m_Args[1]))
					m_bWaiting = false;
				else
					return 1;
			}
			else
			{
				if (!m_bWaiting)
				{
					m_uFrameCounter = 0;
					m_bWaiting = true;
					return 1;
				}
				else if (++m_uFrameCounter >= UInt32 (sc.m_Args[1]))
					m_bWaiting = false;
				else
					return 1;
			}
			break;

		case eCmd_Script_Run:
			ASSERT(sc.m_Args[0] != "");
			g_ScriptMgr->RunScript(g_ScriptMgr->LoadScript(sc.m_Args[0]));
			break;

		case eCmd_Script_Load:
			g_ScriptMgr->LoadScript(sc.m_Args[0]);
			break;

		case eCmd_Send_Entity_Msg:
			ASSERT(sc.m_Args[0]);
			//sc.m_Args[0]->SendMessage(sc.m_Args[1]);
			break;

		case eCmd_Menu_Cmd:
			if (sc.m_Args[0] == "back")
			{
				g_ScriptMgr->KillAllMenuScripts();
			}
			else if (sc.m_Args[0] == "new_game")
			{
				g_ScriptMgr->KillAllMenuScripts();
				g_Game->NewGame();
			}
			else if (sc.m_Args[0] == "new_game_server")
			{
				g_ScriptMgr->KillAllMenuScripts();
				g_Game->NewGame(NGM_Server);
			}
			else if (sc.m_Args[0] == "new_game_client")
			{
				g_ScriptMgr->KillAllMenuScripts();
				g_Game->NewGame(NGM_Client);
			}
			break;

		default:
			ASSERT(false);
			g_Log.LOG_ERROR("Unable to process command!\n");
		}

		m_bRun = false;
	}
	else
		return -1;

	return 0;
}