///////////////////////////////////////////////////////////////////////////
//		File:				LightMgr.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that stores all the lights in the game/map
//							and then when asked, finds the closest, most
//							applicable lights and then applies them to a shader
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LightMgr.h"

//#include "Globals.h"

#include "Player.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CLightMgr);

//////////////////////////////////////////////////////////////////////////

CLightMgr::CLightMgr(void)
	: m_bActive(true),
		m_bTrackTarget(false),
		m_pTarget(NULL)
{
	m_AmbientLight = D3DXCOLOR(Light::ms_LowLight);
}

//////////////////////////////////////////////////////////////////////////

CLightMgr::~CLightMgr(void)
{
	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Error!\n");
	}
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::Add(Light& light)
{
	m_Lights.Add(light);
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::Remove(Light& light)
{
	m_Lights.Remove(light);
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::RemoveAll()
{
	m_Lights.Clear();
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::Init(Bool bAutoTrackPlayer /* = true */)
{
	if (bAutoTrackPlayer)
		TrackTarget(g_Player);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::Update()
{
	if (m_bTrackTarget && m_pTarget)
		m_Lights[0].SetPosition(m_pTarget->m_vPosition);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::Shutdown()
{
	RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLightMgr::GetAffectingLights(LightList& list, GLib::Rect& rect, Bool bSort)
{
	LightList::Iterator i(m_Lights);

	Vec2	lightvec;
	Vec2	pos;
	
	Light	light;
	
	Float	rad;
	Int32		lightCount = 0;

	for (i.Begin(); !i.End(); ++i)
	{
		light = i.Current();

		for (Int32 p = 0; p < 4; p++)
		{
			switch (p)
			{
			case 0:		pos	= Vec2(rect.tlx, rect.tly);		break;
			case 1:		pos	= Vec2(rect.brx, rect.tly);		break;
			case 2:		pos	= Vec2(rect.brx, rect.bry);		break;
			case 3:		pos	= Vec2(rect.tlx, rect.bry);		break;
			}
		
			lightvec	= pos - light.GetPosition();
			rad			= light.GetUmbra();

			if (lightvec.MagnitudeSquared() <= rad * rad || rect.PointInRect(light.GetPosition()))
			{
				list.Add(light);
				lightCount++;

				//At least one point in the rect tested true, so we don't need to check the rest
				break;
			}
		}
	}

	if (bSort)
	{
		//LATER
	}

	return lightCount;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLightMgr::GetAffectingLightPositions(LightPosList& list, GLib::Rect& rect)
{
	Vec2 lightPos;
	LightList lights;

	Int32 lightCount = GetAffectingLights(lights, rect);
	
	LightList::Iterator i(lights);

	for (i.Begin(); !i.End(); ++i)
	{
		lightPos = i.Current().GetPosition();

		D3DXVECTOR4 lightVec(lightPos.X, lightPos.Y, 0.0f, 1.0f);

		list.Add(lightVec);
	}

	return lightCount;
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::SetupLightsInShader(Effect& effect, GLib::Rect& rect)
{
	LightList list;

	//Set the number of lights
	Int32 size = GetAffectingLights(list, rect);
	size = min(size, Light::ms_MaxLights);
	effect.Shader->SetInt("NumActiveLights", size);

	//Set ambient light levels
	D3DXVECTOR4	colorVec(m_AmbientLight.r, m_AmbientLight.g, m_AmbientLight.b, 1.0f);
	effect.Shader->SetVector("vAmbientLight", &colorVec);

	LightList::Iterator it(list);
	it.Begin();

	for (Int32 i = 0; i < size; ++it, i++)
		SetLightInShader(effect, it.Current(), i);

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::SetLightInShader(Effect& effect, Light& light, Int32 lightIndex /* = 0 */)
{
	ASSERT(lightIndex < Light::ms_MaxLights);

	String	lightVec		= "vLightVec",
			lightColor		= "vLightColor",
			lightUmbra		= "fUmbra",
			lightPenumbra	= "fPenumbra";

	Vec2	lightPos		= light.GetPosition();
	
	D3DXCOLOR color			= light.GetColor();
	
	D3DXVECTOR4	posVec(lightPos.X, lightPos.Y, 0.0f, 1.0f);
	D3DXVECTOR4	colorVec(color.r, color.g, color.b, 1.0f);

	switch (lightIndex)
	{
	case 0:
		lightVec		+= "0";
		lightColor		+= "0";
		lightUmbra		+= "0";
		lightPenumbra	+= "0";
		break;

	case 1:
		lightVec		+= "1";
		lightColor		+= "1";
		lightUmbra		+= "1";
		lightPenumbra	+= "1";
		break;

	case 2:
		lightVec		+= "2";
		lightColor		+= "2";
		lightUmbra		+= "2";
		lightPenumbra	+= "2";
		break;

	case 3:
		lightVec		+= "3";
		lightColor		+= "3";
		lightUmbra		+= "3";
		lightPenumbra	+= "3";
		break;
	}

	effect.Shader->SetVector(lightVec.c_str(),		&posVec);
	effect.Shader->SetVector(lightColor.c_str(),	&colorVec);

	effect.Shader->SetFloat(lightUmbra.c_str(),		light.GetUmbra());
	effect.Shader->SetFloat(lightPenumbra.c_str(),	light.GetPenumbra());
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::SetTarget(EntityPtr pTarget, Bool bTurnTrackingOn /* = true */)
{
	m_pTarget = pTarget;

	if (pTarget && bTurnTrackingOn)
		TrackTarget();
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::SetAmbientLight(const Float* vAmbient)
{
	m_AmbientLight = D3DXCOLOR(vAmbient);
}

//////////////////////////////////////////////////////////////////////////