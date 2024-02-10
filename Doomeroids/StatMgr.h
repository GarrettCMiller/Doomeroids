///////////////////////////////////////////////////////////////////////////
//		File:				StatMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that can handle most any statistic
//							record keeping given to it.
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include "DynamicArray.h"
#include "Variable.h"

#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////

struct Statistic
{
	Statistic()
	{
	}

	explicit Statistic(const String& Name)
	{
		strStatName = Name;
	}

	explicit Statistic(const String& Name, const Var& stat) : m_vStat(stat)
	{
		strStatName = Name;
	}

	///////////////////////////////////////////////////////////////////

	const	String& GetName()	const		{ return strStatName;	}
	void	SetName(const String& Name)		{ strStatName = Name;	}

	void	Set(const Var& stat)			{ m_vStat = m_vStat; }
	void	operator = (const Var& rhs)		{ m_vStat = rhs;	}
	
	void	Inc(const Var& stat)			{ m_vStat += stat; }
	void	operator ++ ()					{ m_vStat++;	}

	void	Dec(const Var& stat)			{ m_vStat -= stat;	}
	void	operator -- ()					{ m_vStat--;	}

	const	Var& Get()	const				{ return m_vStat; }

private:
	String	strStatName;
	Var		m_vStat;
};

//////////////////////////////////////////////////////////////////////////

class StatMgr : public Singleton<StatMgr>
{
public:
	StatMgr(void);
	~StatMgr(void);

	int Init();
	int Update();
	int Shutdown();

	Statistic&		GetStat(std::string  name);
	Statistic&		GetStat(UINT index);

	void			CreateStat(std::string  name);

	DblLinkedList<Statistic>	m_Stats;
};

#define g_Stats		StatMgr::GetInstance()