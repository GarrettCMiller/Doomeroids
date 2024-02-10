///////////////////////////////////////////////////////////////////////////
//		File:				MemObj.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A memory object
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>

//////////////////////////////////////////////////////////////////////////

class CMemObj
{
private:
	UINT	m_RefCount;

	//////////////////////////////////////////////////////////////////////////
	//Mem Objects
	static std::list<CMemObj *>		m_LiveObjects;
	static std::list<CMemObj *>		m_DeadObjects;
	//////////////////////////////////////////////////////////////////////////

protected:
	CMemObj(void);
	virtual ~CMemObj(void);

public:

	void AddRef();
	void Release();

	static void CollectGarbage();
	static void CollectRemainingObjects(bool bEmitWarnings = false);

	virtual UINT MemSize() = 0;
};

#define AUTO_SIZE	UINT MemSize() { return sizeof(*this); }