///////////////////////////////////////////////////////////////////////////
//		File:				Light.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A structuaral representation of a light
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\light.h"

//////////////////////////////////////////////////////////////////////////

const Float Light::ms_DefaultUmbra		= 150.0f;
const Float Light::ms_DefaultPenumbra	= 75.0f;

const Float	Light::ms_NoLight[3]		= { 0.0f,	0.0f,	0.0f };
const Float	Light::ms_VeryLowLight[3]	= { 0.05f,	0.05f,	0.05f };
const Float	Light::ms_LowLight[3]		= { 0.25f,	0.25f,	0.25f };
const Float	Light::ms_MediumLight[3]	= { 0.5f,	0.5f,	0.5f };
const Float	Light::ms_FullLight[3]		= { 1.0f,	1.0f,	1.0f };

//////////////////////////////////////////////////////////////////////////

Light::Light(Vec2 pos /* = k_v2Zero */, 
			 D3DXCOLOR color /* = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)*/, 
			 Float umbra /* = ms_DefaultUmbra*/, 
			 Float penumbra /* = ms_DefaultPenumbra*/) 
			 : m_Position(pos), m_Color(color), m_Umbra(umbra), m_Penumbra(penumbra)
{
}

//////////////////////////////////////////////////////////////////////////

Light::~Light(void)
{
}

//////////////////////////////////////////////////////////////////////////