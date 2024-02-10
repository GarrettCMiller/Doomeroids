///////////////////////////////////////////////////////////////////////////
//		File:				IntrusivePtr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GameMacros.h"
#include "MemObj.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class CIntrusivePtr
{
private:
	typedef CIntrusivePtr<_Type> Type;

protected:
	_Type*	m_pObject;

public:

	CIntrusivePtr(void)
	{
		m_pObject = NULL;
	}

	CIntrusivePtr(_Type* obj)
	{
		ASSERT(!BAD_PTR(obj));
		m_pObject = NULL;
		*this = obj;
	}

	CIntrusivePtr(const CIntrusivePtr<_Type>& ptr)
	{
		ASSERT(!BAD_PTR(ptr.m_pObject));
		m_pObject = NULL;
		*this = ptr;
	}

	~CIntrusivePtr(void)
	{
		if (m_pObject)
		{
			m_pObject->Release();
			m_pObject = NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	CIntrusivePtr& operator = (_Type* obj)
	{
		ASSERT(!BAD_PTR(obj));

		if (m_pObject)
			m_pObject->Release();

		m_pObject = obj;

		if (m_pObject)
			m_pObject->AddRef();

		return *this;
	}

	CIntrusivePtr& operator = (const CIntrusivePtr<_Type>& ptr)
	{
		ASSERT(!BAD_PTR(ptr.m_pObject));

		if (m_pObject)
			m_pObject->Release();

		m_pObject = ptr.m_pObject;

		if (m_pObject)
			m_pObject->AddRef();

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

	bool operator == (const CIntrusivePtr<_Type>& ptr) const
	{
		return (m_pObject == ptr.m_pObject);
	}

	bool operator == (const _Type* obj) const
	{
		return (m_pObject == obj);
	}
};