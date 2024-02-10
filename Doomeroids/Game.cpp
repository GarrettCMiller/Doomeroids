/////////////////////////////////////////////////////////////////////////////
//		File:			Game.cpp
//
//		Programmer:		Garrett Miller (GM) (c) 2004
//
//		Date Created:	2/21/2006 12:52:35 AM
//
//		Description:	The game	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Game.h"

#include "D3DWrapper.h"
#include "DirectInput.h"
#include "DirectSound.h"

#include "EntityMgr.h"
#include "ScriptMgr.h"
#include "MapMgr.h"
#include "FontMgr.h"

#include "MeshMgr.h"
#include "EffectMgr.h"
#include "CollisionMgr.h"
#include "LightMgr.h"

#include "Mesh3D.h"

#include "Map.h"
#include "View.h"
#include "Monster.h"

#include "HUD.h"

#include "DPlay.h"

#include "ProfileSample.h"

//#include "XMLMgr.h"
//#include "XMLTables.h"
#include "GameXMLMgr.h"
#include "GameXMLTables.h"

#include "VectorGraphics.h"

//////////////////////////////////////////////////////////////////////////

UIText	report;
DblLinkedList<Float>	avgs;

CVectorGraphics			vg;

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(Game);

//////////////////////////////////////////////////////////////////////////

#define NETWORK_THREAD_CLOSE_TIMEOUT	5.0f

Bool	Game::Setup					= false;
Bool	Game::Closed				= false;
Bool	Game::ShutdownFlag			= false;

const 
Float	Game::DefMessageFadeOutTime	= 7.5f;

Timer	k_MemDumpTimer;

Bool	m_bCheats[NUM_CHEATS];
Bool	running = false;

UInt32 NetworkUpdate(LPVOID pData);

//////////////////////////////////////////////////////////////////////////

Game::Game(void)
{
	Windowed			= true;
	Networked			= false;
	m_bShowFPS			= false;
	Kills				= 0;
	MessageFadeOutTime	= DefMessageFadeOutTime;

	tMessageFade.Init();
	tTime.Init();
	tElapsed.Init();
}

//////////////////////////////////////////////////////////////////////////

Game::~Game(void)
{
	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Failed to shut down properly!\n");
		MessageBox(g_hWnd, "Error shutting down!", "Critical Error!", MB_ICONASTERISK);
	}
}

//////////////////////////////////////////////////////////////////////////

Int32 Game::Init(HWND hWnd)
{
	Timer::ms_GlobalTimer.Reset();

	g_Log.AddHeading("Game::Init()");

	g_Log.SetMode(eLM_UList);

	g_Log << "Game::Init() begin...\n";

	srand(time(time_t(NULL)));

	g_ResLog.OpenNewLog("Resources");

	g_ResLog.SetMode(eLM_OList);

	g_hWnd = hWnd;

	g_PakFile.ReadPakFile();

	g_PakFile.SetBasePath(".\\Res");
	
	g_PakFile.AddSearchDirectory(".\\Res");
	g_PakFile.AddSearchDirectory(".\\Res\\Effects");
	g_PakFile.AddSearchDirectory(".\\Res\\Images");
	g_PakFile.AddSearchDirectory(".\\Res\\Sounds");
	g_PakFile.AddSearchDirectory(".\\Res\\Scripts");
	g_PakFile.AddSearchDirectory(".\\Res\\Models");

	g_MeshMgr->m_ModelBasePath = "Res\\Models\\";

	/*g_GameXMLMgr->LoadGameDefinition();
	g_GameXMLMgr->ReadSettings();*/

	if (!g_MemMgr->Init(DEF_POOL_SIZE))
	{
		g_Log.LOG_ERROR("Memory Manager initialization failed!\n");
		return false;
	}

	Bool	bWindowed	= g_Settings.videoRes.resolution.windowed;
	
	UInt32	resWidth	= g_Settings.videoRes.resolution.width,
			resHeight	= g_Settings.videoRes.resolution.height;

	if (!g_D3D->Init(hWnd, resWidth, resHeight, bWindowed))
	{
		g_Log.LOG_ERROR("Direct3D initialization failed!\n");
		return false;
	}

	if (g_Settings.videoRes.advanced.enableShaders)
	{
		g_EffectMgr->LoadEffect("Entity.fx");
		g_EffectMgr->LoadEffect("Map.fx");
	}

	g_D3D->EnableSpriteDevice();

	if (!g_Input->Init())
	{
		g_Log.LOG_ERROR("DirectInput initialization failed!\n");
		return false;
	}

	InitKeyBindings();

	if (!g_Sound->Init(true))
	{
		g_Log.LOG_ERROR("DirectSound initialization failed!\n");
		return false;
	}

	if (!g_ScriptMgr->Init())
	{
		g_Log.LOG_ERROR("Script Manager initialization failed!\n");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//Add extra initialization here
	//////////////////////////////////////////////////////////////////////////

	//mm->Dump("PreLoading.log");

	g_FontMgr->LoadFontSet(20);

	g_GameXMLMgr->ReadGameDefinition();

	DebugStats.SetPosition(g_D3D->GetScreenWidth() - 150, 5);
	DebugStats.SetColor(0, 255, 0);
	DebugStats.SetAlignment(UIText::Right);
	DebugStats.SetVisible(true);
	DebugStats.SetMultiline(true);
	DebugStats.SetText("Debug Output");

	report.SetPosition(5, 5);
	report.SetColor(0, 255, 0);
	report.SetAlignment(UIText::Left);
	report.SetVisible(true);

	InitWeapons();

	g_CollisionMgr->Init();

	g_LightMgr->Init();

	g_MapMgr->Init();

	m_Cheats.SetFlag(eCheat_GodMode);

	/*g_ScriptMgr->RunScript(g_ScriptMgr->LoadScript("Script1.scr"));
	g_ScriptMgr->Update(0.0f);*/

	k_MemDumpTimer.Init();

	Message.SetPosition(g_D3D->GetScreenWidth() / 2, g_D3D->GetScreenHeight() / 2);
	Message.SetColor(255, 255, 255);
	Message.SetFont(g_FontMgr->LoadFontSet(32, true));
	Message.SetVisible(false);

	m_bShowFPS			= g_Settings.debug.showFPS;
	m_bThrottleFPS		= g_Settings.performance.throttleFPS;
	m_fThrottleLimit	= g_Settings.performance.throttleLimit;

	Setup		= true;
	uFrames		= 0;

	CProfileSample::ResetAll();

	g_Log << "Game::Init() end!\n";

	g_Log.SetMode();
	g_Log.AddHeading("Game::Main()");
	g_Log.SetMode(eLM_UList);

	vg.Init();
	
	g_D3D->GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);

	g_D3D->GetDevice()->SetRenderState(D3DRS_AMBIENT, 0xFF888888);
	
	D3DLight d3dlight;
	ZeroMemory(&d3dlight, sizeof(d3dlight));
    
	// Set up a white point light.
	d3dlight.Type			= D3DLIGHT_POINT;
	d3dlight.Diffuse.r		= 1.0f;
	d3dlight.Diffuse.g		= 1.0f;
	d3dlight.Diffuse.b		= 1.0f;
	d3dlight.Ambient.r		= 0.1f;
	d3dlight.Ambient.g		= 0.1f;
	d3dlight.Ambient.b		= 0.1f;
	d3dlight.Specular.r		= 0.0f;
	d3dlight.Specular.g		= 0.0f;
	d3dlight.Specular.b		= 0.0f;
	
	d3dlight.Attenuation0	= 0.0f;
	d3dlight.Attenuation1	= 0.001f;
	d3dlight.Attenuation2	= 0.00001f;
	d3dlight.Range			= 130.0f;

	d3dlight.Position		= Vec3(-128.0f, 40.0f, -128.0f);
	
	g_D3D->GetDevice()->SetLight(0, &d3dlight);
	g_D3D->GetDevice()->LightEnable(0, TRUE);

	d3dlight.Position		= Vec3(128.0f, 40.0f, -128.0f);

	g_D3D->GetDevice()->SetLight(1, &d3dlight);
	g_D3D->GetDevice()->LightEnable(1, TRUE);

	d3dlight.Position		= Vec3(128.0f, 40.0f, 128.0f);

	g_D3D->GetDevice()->SetLight(2, &d3dlight);
	g_D3D->GetDevice()->LightEnable(2, TRUE);

	d3dlight.Position		= Vec3(-128.0f, 40.0f, 128.0f);

	g_D3D->GetDevice()->SetLight(3, &d3dlight);
	g_D3D->GetDevice()->LightEnable(3, TRUE);
	////END HACK

	NewGame(NGM_SinglePlayer);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 Game::Shutdown()
{
	g_ResLog.SetMode();
	g_Log.SetMode();

	g_Log.AddHeading("Game::Shutdown()");

	g_Log.SetMode(eLM_UList);

	g_ResLog.SetMode(eLM_OList);

	g_Log << "Game::Shutdown() begin...\n";

	g_MemMgr->Dump("PreShutdown");

	g_MapMgr->DeleteInstance();

	g_CollisionMgr->DeleteInstance();
	g_LightMgr->DeleteInstance();
	g_MapMgr->DeleteInstance();
	g_HUD->DeleteInstance();
	g_View->DeleteInstance();
	g_ScriptMgr->DeleteInstance();
	g_EntityMgr->DeleteInstance();

	if (IsNetworked())
		EnterCriticalSection(&m_CS);
	
	g_DPlay->DeleteInstance();

	if (IsNetworked())
		LeaveCriticalSection(&m_CS);

	g_MeshMgr->DeleteInstance();
	
	g_SoundMgr->DeleteInstance();
	g_Sound->DeleteInstance();
	g_Input->DeleteInstance();
	g_D3D->DeleteInstance();
	g_GameXMLMgr->DeleteInstance();

	g_MemMgr->Dump("PostShutdown");

	g_MemMgr->DeleteInstance();

	Closed = true;
	//ShutdownFlag = false;

	avgs.Clear();

	Timer t(true);

	while (running)
	{
		if (t.Get() >= NETWORK_THREAD_CLOSE_TIMEOUT)	//Delay 5 seconds
			break;
	}

	g_Log << "Game::Shutdown() end!\n";

	g_Log.SetMode();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Game::NewGame(NewGameMode mode)
{
	switch (mode)
	{
	case NGM_SinglePlayer:	//Nothing special
		Monster::Spawn("imp", false, Vec2f(25.0f, -35.0f));
		break;

	case NGM_Server:
		if (g_DPlay->Init(true) > 0 && g_DPlay->Connect())
		{
			if (!AfxBeginThread(NetworkUpdate, NULL))
			{
				g_Log.LOG_ERROR("Couldn't begin multiplayer thread!!\n");
				CloseGame();
			}

			Networked = true;
		}
		break;

	case NGM_Client:
		if (g_DPlay->Init(false) > 0 && g_DPlay->Connect())
		{
			if (!AfxBeginThread(NetworkUpdate, NULL))
			{
				g_Log.LOG_ERROR("Couldn't begin multiplayer thread!!\n");
				CloseGame();
			}

			Networked = true;
		}
		break;
	}

	g_MapMgr->PreMapInit();

	Kills = 0;

	tElapsed.Reset();

	SetMessage("Game On!", 2.0f);
}

//////////////////////////////////////////////////////////////////////////

Int32 Game::Update()
{
	uFrames++;

	//Number of seconds * 1000 = milliseconds
	Float dt =	tTime.Get() * 1000.0f;
				//0.0166666666f;	//60 fps hardcoded
				//16.6666666f;
				//0.0333333333f;

	if (m_bThrottleFPS && dt > m_fThrottleLimit)
		dt = m_fThrottleLimit;

	if (m_bShowFPS)
		ShowFPS(dt);

	//Get the new input
	g_Input->Update();

	if (g_Player && 
		((g_Player->m_nHP <= 0 && g_Input->MappedKeyDown(eAction_Respawn))
		|| g_Input->MappedKeyDown(eAction_ReloadMap))
		)
	{
		ReloadMap();
	}


#ifdef _DEBUG
	if (g_Input->MappedKeyDown(eAction_MemoryDump) && k_MemDumpTimer > 5.0f)
	{
		g_MemMgr->Dump("RT MemDump.html");
		k_MemDumpTimer.Reset();
	}
#endif


#ifndef _NOMENU_
	if (g_Input->MappedKeyDown(eAction_Quit))
		g_ScriptMgr->RunScript("MENU Escape Menu");
#else
	if (g_Input->MappedKeyDown(eAction_Quit))
		CloseGame();
#endif

	{
		PROFILE("Game->MainLoop");
	if (g_D3D->BeginScene(true))
	{
		//Particle::ResetParticleCount();

		if (g_ScriptMgr->Update(dt) > 0)
			g_MapMgr->Update(dt);

		//UpdateMessage();
		
		g_D3D->EnableLighting(false);
		vg.DrawLine(k_v3Zero, Vec3(10.0f, 0.0f, 0.0f));
		vg.DrawLine(k_v3Zero, Vec3(0.0f, 10.0f, 0.0f));
		vg.DrawLine(k_v3Zero, Vec3(0.0f, 0.0f, 10.0f));
		g_D3D->EnableLighting();

		//g_SpriteMgr->RenderSpriteEffect(0, 1, "MapLighting", 
		
		if (!g_D3D->EndScene())
			return 1;
	}
	else 
	{
		g_Log.LOG_ERROR("BeginScene failed!\n");
		
		return 1;
	}
	}

	tTime.Reset();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void Game::SetMessage(String  Msg, Float fFadeOutTime /* = DefMessageFadeOutTime */)
{
	Message = Msg;
	Message.SetVisible(true);
	tMessageFade.Reset();
	MessageFadeOutTime = fFadeOutTime;
}

//////////////////////////////////////////////////////////////////////////

void Game::InitWeapons()
{
	Weapon::InitAll();
}

//////////////////////////////////////////////////////////////////////////

void Game::InitKeyBindings()
{
#define SET_KEYBINDING(action, key1, key2)	g_Input->m_KeyBindings[action]					= DirectInput::TKeyPair(key1, key2)
	
	SET_KEYBINDING(eAction_MoveForward,		DIK_W, 0);
	SET_KEYBINDING(eAction_MoveBackward,	DIK_S, 0);
	SET_KEYBINDING(eAction_MoveLeft,		DIK_A, 0);
	SET_KEYBINDING(eAction_MoveRight,		DIK_D, 0);
	SET_KEYBINDING(eAction_TurnLeft,		DIK_Q, 0);
	SET_KEYBINDING(eAction_TurnRight,		DIK_E, 0);

	SET_KEYBINDING(eAction_EquipWeapon1,	DIK_1, 0);
	SET_KEYBINDING(eAction_EquipWeapon2,	DIK_2, 0);
	SET_KEYBINDING(eAction_EquipWeapon3,	DIK_3, 0);
	SET_KEYBINDING(eAction_EquipWeapon4,	DIK_4, 0);
	SET_KEYBINDING(eAction_EquipWeapon5,	DIK_5, 0);
	SET_KEYBINDING(eAction_EquipWeapon6,	DIK_6, 0);
	
	SET_KEYBINDING(eAction_MoveRun,			DIK_LSHIFT, 0);
	SET_KEYBINDING(eAction_MoveJump,		DIK_SPACE, 0);
	
	SET_KEYBINDING(eAction_Toggle_GodMode,	DIK_U, 0);
	SET_KEYBINDING(eAction_Toggle_FPS,		DIK_R, DIK_LCONTROL);

	SET_KEYBINDING(eAction_Pause,			DIK_RETURN, 0);
	SET_KEYBINDING(eAction_Respawn,			DIK_F1, 0);

	SET_KEYBINDING(eAction_MemoryDump,		DIK_BACKSPACE, DIK_RCONTROL);
	SET_KEYBINDING(eAction_Quit,			DIK_ESCAPE, 0);

	SET_KEYBINDING(eAction_ReloadMap,		DIK_D, DIK_LCONTROL);

#undef SET_KEYBINDING

	/*g_Input->m_KeyBindings[eAction_StartBulletTime]			= DirectInput::TKeyPair(DIK_R, 0);
	g_Input->m_KeyBindings[eAction_EndBulletTime]			= DirectInput::TKeyPair(DIK_T, 0);*/
}

//////////////////////////////////////////////////////////////////////////

void Game::ReloadMap()
{
	g_MapMgr->PostMapShutdown();
	NewGame(NGM_SinglePlayer);
}

//////////////////////////////////////////////////////////////////////////

void Game::ShowFPS(Float dt)
{
	//PROFILE("Game::ShowFPS()");

	avgs.Add(dt);

	if (uFrames % 15 == 0)
	{
		DblLinkedList<Float>::Iterator it(avgs);

		m_fAvgDT = 0;

		for (it.Begin(); !it.End(); ++it)
			m_fAvgDT += it.Current();

		m_fAvgDT /= avgs.Size();

		avgs.Clear();

		char buffer[128] = {'\0'};

		sprintf(buffer, "%f fps     dT: %f", 1.0f / m_fAvgDT, dt);
		report.SetText(buffer);
	}

	report.Update();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UInt32 NetworkUpdate(LPVOID pData)
{
	InitializeCriticalSection(&m_CS);

	running = true;

	while (true)
	{
		try
		{
			EnterCriticalSection(&m_CS);

			if (DPlay::Exists() && g_DPlay->IsInitialized())
				g_DPlay->Update();
			else if (Game::FlaggedForClose() || Game::IsClosed())
			{
				LeaveCriticalSection(&m_CS);
				break;
			}

			LeaveCriticalSection(&m_CS);
		}
		catch (CException* e)
		{
			e->ReportError();
		}
	}

	running = false;

	DeleteCriticalSection(&m_CS);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

void Game::UpdateMessage()
{
	//Update the message item
	//Message.Update();

 //	if (Message.IsVisible() && tMessageFade >= MessageFadeOutTime && MessageFadeOutTime > 0)
 //	{
 //		//if (Message.GetColor()
 //		Message.SetVisible(false);
 //	}

	/*
	DebugStats = "UIObjects: ";
	DebugStats += g_SpriteMgr->DebugGetUIObjectListSize();
	DebugStats += "\n UISprites: ";
	DebugStats += g_SpriteMgr->DebugGetUISpriteListSize();
	DebugStats += "\n Draw list: ";
	DebugStats += g_SpriteMgr->DebugGetDrawListSize();
	DebugStats += "\n Render list: ";
	DebugStats += g_SpriteMgr->DebugGetRenderListSize();

	DebugStats.Update();
	*/

	/*DebugStats = "(Px, Py): ";
	DebugStats += g_Player->m_vPosition.X;
	DebugStats += ", ";
	DebugStats += g_Player->m_vPosition.Y;
	DebugStats += "\n(Mx, My): ";
	DebugStats += g_Input->GetMousePos().X;
	DebugStats += ", ";
	DebugStats += g_Input->GetMousePos().Y;
	DebugStats += "\n(Cx, Cy): ";
	DebugStats += g_Player->lookAt.X;
	DebugStats += ", ";
	DebugStats += g_Player->lookAt.Y;

	DebugStats.Update();*/

	const CCameraView& cam = g_D3D->GetCamera();

	DebugStats = "Position: (";
	DebugStats += g_Player->m_vPosition.x;
	DebugStats += ", ";
	DebugStats += g_Player->m_vPosition.y;
	DebugStats += ", ";
	DebugStats += g_Player->m_vPosition.z;
	DebugStats += ")";

	DebugStats.Update();
}