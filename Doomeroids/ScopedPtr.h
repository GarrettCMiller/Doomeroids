///////////////////////////////////////////////////////////////////////////
//		File:				ScopedPtr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A scoped pointer
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GameMacros.h"
#include "MemObj.h"

//////////////////////////////////////////////////////////////////////////

//Non-shared pointer
template <typename _Type>
class CScopedPtr
{
private:
	typedef CScopedPtr<_Type> Type;

protected:
	_Type*	m_pObject;

public:

	CScopedPtr(void)
	{
		m_pObject = NULL;
	}

	CScopedPtr(_Type* obj)
	{
		ASSERT(!BAD_PTR(obj));
		m_pObject = NULL;
		*this = obj;
	}

	CScopedPtr(const CScopedPtr<_Type>& ptr)
	{
		ASSERT(!BAD_PTR(ptr.m_pObject));
		m_pObject = NULL;
		*this = ptr;
	}

	~CScopedPtr(void)
	{
		if (m_pObject)
			delete m_pObject;
	}

	//////////////////////////////////////////////////////////////////////////

	CScopedPtr& operator = (_Type* obj)
	{
		ASSERT(!BAD_PTR(obj));
		m_pObject = obj;
		return *this;
	}

	CScopedPtr& operator = (const CScopedPtr<_Type>& ptr)
	{
		ASSERT(!BAD_PTR(ptr.m_pObject));
		m_pObject = ptr.m_pObject;
		return *this;
	}

	_Type& operator * () const
	{
		ASSERT(m_pObject);
		return *m_pObject;
	}

	_Type* operator -> () const
	{
		ASSERT(m_pObject);
		return m_pObject;
	}

	operator _Type* () const
	{
		return m_pObject;
	}

	bool IsValid() const
	{
		return (m_pObject != NULL && !BAD_PTR(m_pObject));
	}

	bool operator ! ()
	{
		return (!(m_pObject));
	}

	operator bool() const
	{
		return (m_pObject != NULL);
	}

	bool operator == (const CScopedPtr<_Type>& ptr) const
	{
		return (m_pObject == ptr.m_pObject);
	}

	bool operator == (const _Type* obj) const
	{
		return (m_pObject == obj);
	}
};