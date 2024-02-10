///////////////////////////////////////////////////////////////////////////
//		File:				View.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		I don't know right now
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vec.h"
#include "Rect.h"
#include "Singleton.h"
#include "Entity.h"

//////////////////////////////////////////////////////////////////////////

class View : public Singleton<View>
{
	GLib::Rect			m_Rect;

	Entity*		m_pTarget;

public:
	View(void);
	virtual ~View(void);

	Int32	Update();

	const GLib::Rect& GetRect() const											{ return m_Rect; }

	void	SetRect(const GLib::Rect& rect)										{ m_Rect = rect; }
	void	SetRect(const Vec2& tl, const Vec2& br)				{ m_Rect = GLib::Rect(tl, br); }
	void	SetRect(Int32 left, Int32 top, Int32 right, Int32 bottom)		{ m_Rect = GLib::Rect(left, top, right, bottom); }

	void	SetTarget(Entity* pTarget);
	Entity* GetTarget() const												{ return m_pTarget; }
};

#define g_View		View::GetInstance()