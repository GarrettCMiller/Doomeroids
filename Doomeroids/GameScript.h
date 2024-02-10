#pragma once
#include "ScriptMgr.h"

class CGameScript :	public Script
{
public:
	CGameScript(void);
	CGameScript(String name);
	virtual ~CGameScript(void);

	CGameScript& operator = (const CGameScript& rhs);

	virtual Int32 Update();
};
