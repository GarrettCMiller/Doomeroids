///////////////////////////////////////////////////////////////////////////
//		File:				Light.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		The structural representation of a light
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "D3DWrapper.h"

//////////////////////////////////////////////////////////////////////////

class Light
{
public:
	enum ELightIntensity
	{
		eLightIntensity_NoLight,
		eLightIntensity_VeryLowLight,
		eLightIntensity_LowLight,
		eLightIntensity_MediumLight,
		eLightIntensity_FullLight,

		eLightIntensity_IntensityCount
	};

	static const Float	ms_NoLight[3];
	static const Float	ms_VeryLowLight[3];
	static const Float	ms_LowLight[3];
	static const Float	ms_MediumLight[3];
	static const Float	ms_FullLight[3];

	static const Float	ms_DefaultUmbra;
	static const Float	ms_DefaultPenumbra;

	static const Int32	ms_MaxLights = 4;

private:
	Vec2			m_Position;
	D3DXCOLOR		m_Color;

	//Distance from position where light begins to fade
	Float			m_Penumbra;

	//Distance from position where light fully fades
	Float			m_Umbra;

public:
	Light(Vec2 pos = k_v2Zero, D3DXCOLOR color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), Float umbra = ms_DefaultUmbra, Float penumbra = ms_DefaultPenumbra);
	~Light(void);

	Float	GetUmbra() const				{ return m_Umbra; }
	void	SetUmbra(Float umbra)			{ ASSERT(umbra > 0.0f); ASSERT(umbra > m_Penumbra); m_Umbra = umbra; }

	Float	GetPenumbra() const				{ return m_Penumbra; }
	void	SetPenumbra(Float penumbra)		{ ASSERT(penumbra > 0.0f); ASSERT(penumbra < m_Umbra); m_Penumbra = penumbra; }

	Vec2	GetPosition() const				{ return m_Position; }
	void	SetPosition(Vec2 pos)			{ m_Position = pos; }

	D3DXCOLOR	GetColor() const			{ return m_Color; }
	void		SetColor(D3DXCOLOR color)	{ m_Color = color;}

	//For storage in a list
	Bool		operator != (const Light& rhs) const
	{
		return memcmp(this, &rhs, sizeof(Light));
	}

	Bool		operator == (const Light& rhs) const
	{
		return !(*this != rhs);
	}
};
