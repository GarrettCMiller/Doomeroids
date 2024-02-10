///////////////////////////////////////////////////////////////////////////
//		File:				MemObj.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A memory object
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\memobj.h"
#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////
//Mem Objects
std::list<CMemObj *>		CMemObj::m_LiveObjects;
std::list<CMemObj *>		CMemObj::m_DeadObjects;
//////////////////////////////////////////////////////////////////////////

CMemObj::CMemObj(void)
{
	//m_LiveObjects.push_back(this);

	m_RefCount = 0;
}

//////////////////////////////////////////////////////////////////////////

CMemObj::~CMemObj(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CMemObj::AddRef()
{
	m_RefCount++;
}

//////////////////////////////////////////////////////////////////////////

void CMemObj::Release()
{
	m_RefCount--;

	/*if (m_RefCount <= 0)
	{
		m_LiveObjects.remove(this);
		m_DeadObjects.push_back(this);
	}*/
}

//////////////////////////////////////////////////////////////////////////

void CMemObj::CollectGarbage()
{
	for (std::list<CMemObj *>::iterator it = m_DeadObjects.begin(); 
		it != m_DeadObjects.end(); ++it)
	{
		delete (*it);
	}

	m_DeadObjects.clear();
}

//////////////////////////////////////////////////////////////////////////

void CMemObj::CollectRemainingObjects(bool bEmitWarnings /* = false */)
{
	CollectGarbage();

	for (std::list<CMemObj *>::iterator it = m_LiveObjects.begin(); 
		it != m_LiveObjects.end(); ++it)
	{
		CMemObj* obj = *it;

		if (bEmitWarnings)
		{
#ifdef _DOOMEROIDS_
			g_Log << "** Live object still around!!\n";
#endif
		}

		delete obj;
	}

	m_LiveObjects.clear();
}

//////////////////////////////////////////////////////////////////////////