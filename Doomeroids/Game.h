/////////////////////////////////////////////////////////////////////////////
//		File:			Game.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2005 12:52:28 AM
//
//		Description:	The game
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"

#include "Entity.h"
#include "Creature.h"
#include "Player.h"
#include "Monster.h"
#include "Flag.h"

#include "UIText.h"

//////////////////////////////////////////////////////////////////////////

enum ECheats
{
	eCheat_NoCheats,		//Must be here for the Flag class to work correctly

	eCheat_GodMode,			//IDDQD
	eCheat_AllWeapons,		//IDKFA
	eCheat_InfiniteAmmo,	//IDKFA (?)

	NUM_CHEATS
};

//////////////////////////////////////////////////////////////////////////

enum NewGameMode
{
	NGM_SinglePlayer,
	NGM_Server,
	NGM_Client,

	NewGameModeCount
};

//////////////////////////////////////////////////////////////////////////

class Game : public Singleton<Game>
{
	//Do nothing copy constructor
	Game(Game &game);

	//Do nothing assignment operator
	Game &operator = (Game &game);

private:
	Bool		Networked;
	static Bool	Setup;
	static Bool	Closed;
	static Bool	ShutdownFlag;


public:
	Bool		Windowed;
	Bool		m_bShowFPS;

	Bool		m_bThrottleFPS;
	Float		m_fThrottleLimit;

	Timer		tTime;
	Timer		tElapsed;

	Timer		tMessageFade;
	UIText		Message;

	UIText		KillTxt;

	///////////////////

	UIText		DebugStats;

	///////////////////

	static const Float		DefMessageFadeOutTime;
	Float		MessageFadeOutTime;
	Float		m_fAvgDT;

	UInt32		uFrames;
	UInt32		CheatSound;
	UInt32		BG;
	UInt32		Kills;

	String		GameName;

	Flag<ECheats>	m_Cheats;

public:
	Game(void);
	~Game(void);

	Int32	Init(HWND hWnd);
	Int32	Update();
	Int32	Shutdown();

	void SetMessage(String  Msg, Float fFadeOutTime = DefMessageFadeOutTime);

	void NewGame(NewGameMode mode = NGM_SinglePlayer);

	static void CloseGame()						{	ShutdownFlag = true;	}
	static Bool FlaggedForClose()				{	return ShutdownFlag;	}
	static Bool	IsSetup()						{	return Setup;			}
	static Bool	IsClosed()						{	return Closed;			}
	static void	SetClosed(Bool close = true)	{	Closed = close;			}
	static void	SetSetup(Bool setup = true)		{	Setup = setup;			}
	Bool		IsNetworked()					{	return Networked;		}

	void ReloadMap();

private:
	void InitWeapons();
	void InitKeyBindings();

	void UpdateMessage();
	void ShowFPS(Float dt);
};

#define g_Game  Game::GetInstance()