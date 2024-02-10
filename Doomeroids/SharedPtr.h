///////////////////////////////////////////////////////////////////////////
//		File:				SharedPtr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A shared pointer
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GameMacros.h"
#include "MemObj.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class CSharedPtr
{
private:
	typedef CSharedPtr<_Type> Type;

protected:
	_Type*	m_pObject;
	UINT	m_RefCount;

public:

	CSharedPtr(void)
	{
		m_pObject = NULL;
		m_RefCount = 0;
	}

	/*template <class Y>
	explicit CSharedPtr(Y* p)
	{
		m_pObject = p;
	}*/

	CSharedPtr(_Type* obj)
	{
		ASSERT(!BAD_PTR(obj));
		m_pObject = NULL;
		*this = obj;
	}

	CSharedPtr(const CSharedPtr<_Type>& ptr)
	{
		ASSERT(!BAD_PTR(ptr.m_pObject));
		m_pObject = NULL;
		*this = ptr;
	}

	~CSharedPtr(void)
	{
		if (m_pObject)
			m_pObject->Release();
	}

	//////////////////////////////////////////////////////////////////////////

	CSharedPtr& operator = (_Type* obj)
	{
		ASSERT(!BAD_PTR(obj));

		if (m_pObject)
			m_pObject->Release();

		m_pObject = obj;

		if (m_pObject)
			m_pObject->AddRef();

		return *this;
	}

	CSharedPtr& operator = (const CSharedPtr<_Type>& ptr)
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

	bool operator == (const CSharedPtr<_Type>& ptr) const
	{
		return (m_pObject == ptr.m_pObject);
	}

	bool operator == (const _Type* obj) const
	{
		return (m_pObject == obj);
	}

private:
	void	AddRef()
	{
		m_RefCount++;
	}

	void	Release()
	{
		if (--m_RefCount <= 0)
		{
			delete m_pObject;
			m_pObject = NULL;
		}
	}
};