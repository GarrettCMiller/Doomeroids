#pragma once

#include <crtdbg.h>

#ifdef _DEBUG

	void* operator new(size_t nSize, std::string  lpszFileName, int nLine)
	{
		return ::operator new(nSize, 1, lpszFileName, nLine);
	}

	#define DEBUG_NEW new(THIS_FILE, __LINE__)

	#define MALLOC_DBG(x) _malloc_dbg(x, 1, THIS_FILE, __LINE__);
	#define malloc(x) MALLOC_DBG(x)

#endif // _DEBUG