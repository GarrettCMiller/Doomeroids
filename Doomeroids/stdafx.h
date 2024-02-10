/////////////////////////////////////////////////////////////////////////////
//		File:			stdafx.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:50:17 AM
//
//		Description:	Ya know...	
//
/////////////////////////////////////////////////////////////////////////////
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0501 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0700	// Change this to the appropriate value to target IE 5.0 or later.
#endif

// Windows Header Files:
//#include <windows.h>
#include <afxwin.h>
#include <afxext.h>

//#define _CRTDBG_MAP_ALLOC

#include <string>
#include "DataTypes.h"

#ifdef _DEBUG
#pragma comment(lib, "GLibD.lib")
#else
#pragma comment(lib, "GLib.lib")
#endif

// C RunTime Header Files
// #include <stdlib.h>
// #include <malloc.h>
// #include <crtdbg.h>
// #include <memory.h>
// #include <tchar.h>
// #include <stdio.h>
// #include <conio.h>

//#define _GPOOTER_

//#include "Logger.h"
//#include "OutputDlg.h"

#define g_Output COutputDlg::m_Logger

#define OutputResult(exp)			if (exp)g_Output << "Success!\n";\
									else	g_Output << "Failed!\n";


//Disable non-standard extension (type name first seen using 'class' now seen using 'struct')
#pragma warning (disable: 4099)

#if 0
//Disable pointer truncation warnings
#pragma warning(disable: 4311)
//Disable pointer size truncation
#pragma warning(disable: 4312)
//Disable forcing value to Bool
#pragma warning(disable: 4800)
//Disable init conversion warning
#pragma warning(disable: 4244)
//Disable matching operator delete
#pragma warning(disable: 4291)
//Disable unreferenced formal parameter
#pragma warning(disable: 4100)
//Disable assignment within conditional expression
#pragma warning(disable: 4706)
//Disable too many actual parameters for macro
#pragma warning(disable: 4002)
//Disable nonstandard extention used
#pragma warning(disable: 4239)
//Disable local variable '' may be used without having been initialized
#pragma warning(disable: 4701)
#endif 

#define _NOMENU_

#include "KeyBindings.h"

#include "Globals.h"