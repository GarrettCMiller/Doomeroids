///////////////////////////////////////////////////////////////////////////
//		File:				Map.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A map is the level or area that you are on/in
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Map.h"

//#include "Globals.h"

#include "SpriteMgr.h"
#include "View.h"
#include "EntityMgr.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Game.h"
#include "HUD.h"
#include "Prop.h"
#include "DirectInput.h"
#include "EffectMgr.h"
#include "CollisionMgr.h"
#include "XMLTables.h"
#include "LightMgr.h"

#include "VectorGraphics.h"

//#include "EnemyDesc.h"

//////////////////////////////////////////////////////////////////////////

Bool Map::m_bAutoSpawn	=	//true;
							false;

Timer Map::SpawnDelay;

Timer tSlowMo;

CVectorGraphics k_MapVG;

static const Int32 kMaxLights = 4;

//////////////////////////////////////////////////////////////////////////
Float	fUmbra[kMaxLights] = 
{
	100.0f,
	150.0f,
	150.0f,
	150.0f
};

Float	fPenumbra[kMaxLights] =
{
	2.0f,
	25.0f,
	25.0f,
	25.0f,	
};

Vec2	vLightVec[kMaxLights] =
{
	Vec2(512,	450),		//Center
	Vec2(512,	128),		//Top
	Vec2(896,	384),		//Right
	Vec2(512,	640)		//Bottom
};

D3DXCOLOR	vLightColor[kMaxLights] =
{
	D3DXCOLOR(0.5f,	0.5f,	0.5f,	1.0f),
	D3DXCOLOR(1.0f,	1.0f,	1.0f,	1.0f),
	D3DXCOLOR(1.0f,	1.0f,	1.0f,	1.0f),
	D3DXCOLOR(1.0f,	1.0f,	1.0f,	1.0f),
};

Float	vAmbientLight[3];/* = Light::ms_LowLight;*/

//////////////////////////////////////////////////////////////////////////

Map::Map()
{
	ZeroMemory(m_strName, 64);
	ZeroMemory(m_strDescription, MAX_DESCRIPTION_LENGTH);
	boss = NULL;

	m_Walls = NULL;
	
	SpawnDelay.Init();

	memcpy(vAmbientLight, Light::ms_LowLight, sizeof(Float) * 3);

#ifdef _DOOMEROIDS_
	m_bAutoSpawn = (g_Settings.debug.mapInfo.autospawn ? !g_Game->IsNetworked() : false);
#endif
}

//////////////////////////////////////////////////////////////////////////

Map::~Map()
{
}

//////////////////////////////////////////////////////////////////////////

Bool Map::Init(String  FileName)
{
	if (!FileName.empty())
		if (!LoadMap(FileName))
			return false;

#ifdef _DOOMEROIDS_
	if (!g_Game->IsNetworked())
		CreateRandomBarrels();
#endif

	g_D3D->SetClipRange(10.0f, 0.1f, true);
	g_D3D->SetLookAt(Vec3(512, 384));
	g_D3D->SetEyePt(Vec3(521, 384));
	//g_D3D->CenterCameraOn()

    return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Map::Init(UInt32 Width, UInt32 Height)
{
	//if (!CreateTiles(Width, Height))
	//	return false;
	
	LoadTileSet();

	g_D3D->SetClipRange(500.0f, 0.01f, true);

	if (!m_Terrain.Init(Width, Height, 8.0f))
		return false;

	if (!g_D3D->CreateRenderTarget(m_Terrain.m_MainTexture, 1280, 1024))
		return false;

	if (!g_D3D->PushRenderTarget(m_Terrain.m_MainTexture.surface))
		return false;

	if (!g_D3D->BeginScene(true))
		return false;

	if (!g_D3D->BeginSpriteDevice(false))
		return false;

	String strmap =
			"3355555555533""3355555555533"
			"3335555555333""3335555555333" 
			"0000004000224""4000004000222"
			"0000004000004""4000004000002"
			"0000444440444""4440444440222"
			"0000004000444""4440004000002"
			"0000004000224""4000004000222"
			"3330000000334""4330000000333"
			"3330000000334""4330000000333"

			"3355555555533""3355555555533"
			"3335555555333""3335555555333" 
			"0000004000222""0000004000222"
			"0000004000002""0000004000002"
			"0000444440222""0000444440222"
			"0000004000002""0000004000002"
			"0000004000222""0000004000222"
			"3330000000333""3330000000333"
			"3333333333333""3333333333333"
			;

	UInt32 i = 0;
	Float sScale = 1.0f;

	String strTile;

	for (UInt32 y = 0; y < Height /*+ 1*/; y++)
	{
		for (UInt32 x = 0; x < Width /*+ 1*/; x++)
		{
			//i = Random(m_TileSet.GetNumEntries() - 1);
			i = (y * (Width /*+ 1*/)) + (x);
			strTile = strmap[i];
			i = atoi(strTile.c_str());
			g_SpriteMgr->RenderSprite(m_TileSet.GetTile(i),
											x * 64.0f * sScale, y * 64.0f * sScale, 0.0f, 0.0f, 
											(i == 0 ? 0.5f : 1.0f) * sScale);
		}
	}

	//g_SpriteMgr->RenderSprite(0, 512, 512, 0.0f, 0.0f, 10.0f);

	if (!g_D3D->EndSpriteDevice())
		return false;

	if (!g_D3D->EndScene())
		return false;

	g_D3D->PopRenderTarget();

	//m_Terrain.Set2DVertices();

	//m_Camera.Set(&k_v3Zero, &k_v3ZAxis, &k_v3YAxis, &k_v3XAxis);	
	//m_Camera.RotateX(-45.0f);
	//m_Camera.MoveForward(-100.0f);

	g_D3D->SetCamera(m_Camera);

	CMatrix mat;
	mat.PerspectiveFOV(PI * 0.333f, g_D3D->GetAspectRatio(), 200.0f, 0.5f);
	g_D3D->SetPerspective(mat);

	//D3DXCreateBox(g_D3D->GetDevice(), 25.0f, 25.0f, 5.0f, &m_Walls, NULL);
	//D3DXCreateSphere(g_D3D->GetDevice(), 15.0f, 10.0f, 10.0f, &m_Walls, NULL);

#if 0
	Light light;

	for (Int32 i = 0; i < Light::ms_MaxLights; i++)
	{
		light.SetColor(vLightColor[i]);
		light.SetPosition(vLightVec[i]);
		light.SetPenumbra(fPenumbra[i]);
		light.SetUmbra(fUmbra[i]);
		
		g_LightMgr->Add(light);
	}

	if (!g_Game->IsNetworked())
		CreateRandomBarrels();
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Map::LoadTileSet()
{
	TileSetChunk tsc;

	tsc.Entries = 6;
	strcpy(tsc.Textures[0], "MetalFloor.bmp");
	strcpy(tsc.Textures[1], "Brick.bmp");
	strcpy(tsc.Textures[2], "Lava1.bmp");
	strcpy(tsc.Textures[3], "blood1.bmp");
	strcpy(tsc.Textures[4], "fwater1.bmp");
	strcpy(tsc.Textures[5], "nukage1.bmp");

	m_TileSet.LoadTileSet(tsc);

	m_uTileSize = m_TileSet.GetTextureSize();
}

//////////////////////////////////////////////////////////////////////////

Bool Map::CreateTiles(UInt32 Width, UInt32 Height)
{
	m_uWidth = Width;
	m_uHeight = Height;

#if 0
	Int32 tileId;

	m_Tiles.Resize(Width, Height);

	for (UInt32 y = 0; y < m_uHeight; y++)
	{
		for (UInt32 x = 0; x < m_uWidth; x++)
		{
			tileId				= 1;
			m_Tiles(x, y)		= Tile(tileId, eFlag_Default);
		}
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Map::Load()
{
}

//////////////////////////////////////////////////////////////////////////

Bool Map::Update(Float dt)
{
	//PROFILE_THIS(16);

	static Float	TimeDelay	= 3.0f,
					Rate		= 0.75f;

	static Int32 num2 = 5;

	static Timer InputTimer(true);

	if (!g_Player)
		return true;

	if (InputTimer > 0.25f)
	{
		if (g_Input->MappedKeyDown(eAction_ChangeSpawnType))	//Zero 0
		{
			if (++num2 > 5)
				num2 = 1;

			InputTimer.Reset();
		}

		if (g_Input->MappedKeyDown(eAction_DecreaseSpawnTime))
		{
			TimeDelay *= 0.8f;
			InputTimer.Reset();
		}
		else if (g_Input->MappedKeyDown(eAction_IncreaseSpawnTime))
		{
			TimeDelay *= 1.25f;
			InputTimer.Reset();
		}
	}
	
	if (InputTimer >= 0.5f)
	{
		if (g_Input->MappedKeyDown(eAction_StartBulletTime) && !g_Input->KeyDown(DIK_LCONTROL))
		{
			Timer::SetTimeScale(0.5f);
			tSlowMo.Reset();
			InputTimer.Reset();
		}
		else if (g_Input->MappedKeyDown(eAction_EndBulletTime))
		{
			Timer::SetTimeScale(1.0f);
			InputTimer.Reset();
		}

		if (g_Input->MappedKeyDown(eAction_Toggle_GodMode))
		{
			if (g_Game->m_Cheats.ToggleFlag(eCheat_GodMode))
				g_Game->SetMessage("God Mode Is On!!");
			else
				g_Game->SetMessage("God Mode Is Off!!");

			InputTimer.Reset();
		}

		if (g_Input->MappedKeyDown(eAction_Toggle_FPS))
		{
			g_Game->m_bShowFPS = !g_Game->m_bShowFPS;
			InputTimer.Reset();
		}

		if (g_Input->MappedKeyDown(eAction_CreateRandomBarrels))
		{
			CreateRandomBarrels();
			InputTimer.Reset();
		}

		static Int32 LightMode = 0;
		if (g_Input->MappedKeyDown(eAction_CycleAmbientLightUp))
		{
			if (++LightMode > 3)
				LightMode = 0;

			switch (LightMode)
			{
			case 0:
				g_Game->SetMessage("Very Low Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_VeryLowLight);
				break;
				
			case 1:
				g_Game->SetMessage("Low Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_LowLight);
				break;

			case 2:
				g_Game->SetMessage("Medium Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_MediumLight);
				break;

			case 3:
				g_Game->SetMessage("Full Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_FullLight);
				break;

			default:
				g_Game->SetMessage("No Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_NoLight);
				break;
			}

			InputTimer.Reset();
		}
		else if (g_Input->MappedKeyDown(eAction_CycleAmbientLightDown))
		{
			if (--LightMode < 0)
				LightMode = 3;

			switch (LightMode)
			{
			case 0:
				g_Game->SetMessage("Very Low Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_VeryLowLight);
				break;

			case 1:
				g_Game->SetMessage("Low Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_LowLight);
				break;

			case 2:
				g_Game->SetMessage("Medium Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_MediumLight);
				break;

			case 3:
				g_Game->SetMessage("Full Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_FullLight);
				break;

			default:
				g_Game->SetMessage("No Light mode", 2.5f);
				g_LightMgr->SetAmbientLight(Light::ms_NoLight);
				break;
			}

			InputTimer.Reset();
		}

		if (g_Input->MappedKeyDown(eAction_Pause) && !g_EntityMgr->IsPaused())
		{
			g_Game->SetMessage("PAUSED", -1.0f);
			g_EntityMgr->Pause();
			InputTimer.Reset();
		}
		else if (g_Input->MappedKeyDown(eAction_Pause) && g_EntityMgr->IsPaused())
		{
			g_Game->SetMessage(" ", 1.0f);
			g_EntityMgr->UnPause();
			InputTimer.Reset();
		}
	}

	if (InputTimer >= 1.0f)
	{
		if (g_Input->MappedKeyDown(eAction_Spawn4Soldiers))
		{
			Vec2 vpos(45.0f, 0.0f);
			Monster::Spawn(1, false, vpos);
			Monster::Spawn(1, false, vpos);
			Monster::Spawn(1, false, vpos);
			Monster::Spawn(1, false, vpos);
			InputTimer.Reset();
		}
	}

	if (tSlowMo >= 10.0f)
		Timer::SetTimeScale(1.0f);

	if (g_Player->m_eState != ES_DEAD && m_bAutoSpawn && SpawnDelay.Get() > TimeDelay)
	{
		for (Int32 t = 0, times = Random(3, 1); t < times; t++)
		{
			Int32 num = num2;

			if (num2 == 5)
			{
				num = Random(10, 1);

				if (num <= 4)
					Monster::Spawn(1);
				else if (num > 4 && num <= 7)
					Monster::Spawn(2);
				else if (num > 7 && num <= 9)
					Monster::Spawn(3);
				else
					Monster::Spawn(4);
			}
			else
				Monster::Spawn(ECreatureType(num));
		}


		SpawnDelay.Reset();
	}

	//////////////////////////////////////////////////////////////////////////
	//HACK
	static Bool bWave2 = false, bWave3 = false;

	if (g_Game->Kills >= 25 && g_Game->Kills < 50 && !bWave2)
	{
		if (g_ScriptMgr->RunScript("Spawn 2 Barons") != 0)
			bWave2 = true;
	}

	if (g_Game->Kills >= 75 && !bWave3)
	{
		if (!boss)
		{
			if (g_ScriptMgr->RunScript("Spawn Cyberdemon") != 0)
			{
				m_bAutoSpawn = false;
				bWave3 = true;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	if (!g_EntityMgr->IsPaused())
	{
		//g_LightMgr->Update();
		
		g_EntityMgr->Update(dt);
		
		g_CollisionMgr->Update(dt);
	}
	else
	{
		g_EntityMgr->Update(0.0f);
	}

	if (!g_Game->IsNetworked())
	{
		//Draw();
		Render();
		g_HUD->Update();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Map::Shutdown()
{
	m_Terrain.Shutdown();

	SAFE_RELEASE(m_Walls);

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Map::Draw()
{
	//PROFILE_THIS(17);

	if (!g_Settings.debug.mapInfo.render)
		return;

	UInt32 id = m_TileSet.GetTile(0);

	LPD3DXSPRITE	spriteDevice = g_D3D->GetSpriteObject();
	Effect			effect;

	SpriteMgr*		pSpriteMgr = g_SpriteMgr;
	D3D*			pD3D = g_D3D;

	D3DXMATRIX		spriteMatrix, effectMatrix;

	if (pD3D->AllowShaders())
	{
		effect = g_EffectMgr->GetEffect(0);

		pD3D->BeginSpriteDevice();

		//spriteDevice->Flush();
	}

	Float	tsx = m_uTileSize / 2,
			tsy = m_uTileSize / 2,
			px = tsx,
			py = tsy;

/*
	for (UInt32 ty = initialY;		ty < m_uHeight;		py += m_uTileSize,	ty++)
	{
		px = tsx;

		for (UInt32 tx = initialX;	tx < m_uWidth;		px += m_uTileSize,	tx++)
		{
			//id = 0;//m_Tiles(tx, ty).GetID();
			//id = m_TileSet.GetTile(0);

			if (pD3D->AllowShaders())
				pSpriteMgr->RenderSprite(id, px, py, 1.000f);
			else
				pSpriteMgr->DrawSprite(id, px, py, 1.000f);
		}
	}
*/
	if (pD3D->AllowShaders())
	{
		effect.Shader->SetMatrix("matWorldViewProj", &effectMatrix);
		effect.Shader->SetFloat("time", g_Game->tElapsed.Get());

		if (FAILS(effect.Shader->SetTechnique("PerPixelLighting")))
		{
			//g_Log.LOG_ERROR_DX(hr);
			DxErr;
			ASSERT(false);
			return;
		}

		UInt32 numPasses = 0;

		g_LightMgr->SetupLightsInShader(effect, GLib::Rect(0, 0, pD3D->GetScreenWidth(), pD3D->GetScreenHeight()));

		effect.Shader->Begin(&numPasses, 0);
		{
			for (UInt32 i = 0; i < numPasses; i++) 
			{
				g_hr = effect.Shader->BeginPass(i);
				{
					if (FAILED(g_hr))
					{
						g_Log.LOG_ERROR_DX(g_hr);;
						ASSERT(false);
						return;
					}
					else
					{
						spriteDevice->Flush();
					}
				}
				effect.Shader->EndPass();
			}
		}
		effect.Shader->End();

		pD3D->EndSpriteDevice();
	}
}

//////////////////////////////////////////////////////////////////////////

void Map::RenderTile(UInt32 id, Float px, Float py)
{
	//PROFILE_THIS(21);
	//g_SpriteMgr->RenderSprite(id, px, py, 1.000f, 0.0f, 1.0f, 0xFFFFFFFF);
	g_SpriteMgr->DrawSprite(id, px, py, 1.000f, 0.0f, 1.0f, 0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////////

void Map::Render()
{
	if (!g_Settings.debug.mapInfo.render)
		return;

#if 0
	UInt32 numPasses = 0;

	Effect& e = g_EffectMgr->GetEffect(1);
	
	e.Shader->SetTechnique("RenderMapObjects");

	e.Shader->SetTexture("pWallTexture", g_SpriteMgr->GetTexture(m_TileSet.GetTile(1)));
	
	D3DMatrix effectMatrix = g_D3D->matView * g_D3D->matProj;
	e.Shader->SetMatrix("matWorldViewProj", &effectMatrix);

	e.Shader->Begin(&numPasses, 0);

	e.Shader->BeginPass(0);
	m_Walls->DrawSubset(0);
	e.Shader->EndPass();

	e.Shader->End();
#endif

	m_Terrain.Render();
}

//////////////////////////////////////////////////////////////////////////

Bool Map::LoadMap(String  FileName)
{
#if 0
	ASSERT(!FileName.empty());

	//FILE *f = fopen(FileName, "rb");

	//if (!f)
	{
#ifdef _DOOMEROIDS_
		g_Log.LOG_ERROR("Map not found!!\n");
		g_ResLog.LOG_ERROR("Map not found!!\n");
#endif
		return false;
	}

	//MapFileHeader mfh = {0};

	//fread(&mfh, sizeof(MapFileHeader), 1, f);

	//if (mfh.Version != CURRENT_MAP_VERSION)
	{
#ifdef _DOOMEROIDS_
		g_ResLog.LOG_ERROR("The map version is not supported!\n");
#endif

		return false;
	}

	//Int32	mx	= mfh.Width,
	//	my	= mfh.Height;

	//strncpy(mfh.Name, m_strName, MAX_FILENAME_LENGTH);
	//strncpy(mfh.Description, m_strDescription, MAX_DESCRIPTION_LENGTH);

	//m_TileSet.LoadTileSet(mfh.TileSetInfo);

	//TODO: load map tiles and copy them over
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////

void Map::CopyFrom(const Map& rhs)
{
	strcpy(m_strName, rhs.m_strName);
	strcpy(m_strDescription, rhs.m_strDescription);
	
	m_nID		= rhs.m_nID;
	
	m_uWidth	= rhs.m_uWidth;
	m_uHeight	= rhs.m_uHeight;
	
	m_uTileSize	= rhs.m_uTileSize;
	
	m_ShaderID	= rhs.m_ShaderID;
	
	m_TileSet	= rhs.m_TileSet;
	m_Tiles		= rhs.m_Tiles;
	
	boss		= rhs.boss;

	m_Terrain	= rhs.m_Terrain;

	m_Walls		= rhs.m_Walls;
}

//////////////////////////////////////////////////////////////////////////

void Map::CreateRandomBarrels()
{
#ifdef _DOOMEROIDS_
	//Create random barrels
	for (Int32 b = 0; b < 10; b++)
	{
		Vec2 vpos =	Vec2(Random(30.0f, -30.0f), Random(30.0f, -30.0f));
		
		Prop* prop = new Prop;
		ASSERT(prop);
		prop->Init(eProp_Barrel, vpos, true);
		g_EntityMgr->Add(prop);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////