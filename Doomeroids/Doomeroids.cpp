/////////////////////////////////////////////////////////////////////////////
//		File:			Doomeroids.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:29 AM
//
//		Description:	THE FILE
//
/////////////////////////////////////////////////////////////////////////////
// Doomeroids.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
//#include <vld.h>
#include "Doomeroids.h"

#include "Game.h"

#include "D3DWrapper.h"
#include "DirectInput.h"
#include "Timer.h"
#include "XMLTables.h"
#include "ProfileSample.h"

#include "GameXMLMgr.h"

//////////////////////////////////////////////////////////////////////////

//#ifdef _DEBUG
	//#define USE_CONSOLE
//#endif

//////////////////////////////////////////////////////////////////////////

#define MAX_LOADSTRING 100
//#define USE_TITLEBAR

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
Bool				InitInstance(HINSTANCE, Int32);
LRESULT CALLBACK	WndProc(HWND, UInt32, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UInt32, WPARAM, LPARAM);

//////////////////////////////////////////////////////////////////////////

Int32 APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     Int32       nCmdShow)
{
	Timer::ms_GlobalTimer.Init();

	//CProfileSample parent("Root");

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DOOMEROIDS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DOOMEROIDS);

	CProfileSample::ms_bProfilerEnabled = g_Settings.debug.profiler.enabled;

	//PROFILE("Windows Message Loop");

		// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (KEYDOWN(VK_CONTROL) && KEYDOWN('B') && !CProfileSample::ms_bProfilerEnabled)
		{
			CProfileSample::ms_bProfilerEnabled = true;
			CProfileSample::ResetAll();
		}
		else if (KEYDOWN(VK_CONTROL) && KEYDOWN('N') && CProfileSample::ms_bProfilerEnabled)
		{
			CProfileSample::ms_bProfilerEnabled = false;
		}
		else if (KEYDOWN(VK_CONTROL) && KEYDOWN('M'))
		{
			CProfileSample::ResetAll();
		}

		if (Game::Exists() && Game::IsSetup() && !Game::IsClosed())
		{
			PROFILE("Game::Update()", 0);
			g_Game->Update();
		}

		CProfileSample::Output();
	}

#ifdef USE_CONSOLE
	if (!FreeConsole())
		MessageBox(NULL, "Couldn't free the console!!\n", "ERROR", MB_OK);
#endif

	g_Game->DeleteInstance();

#ifdef USE_PROFILING
	//Make sure we finish the profiling correctly before we output
	//profSamp.~CProfileSample();

	CProfileSample::Output();
#endif

	g_ProfileHandler->DeleteInstance();

	return (Int32) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_CLASSDC;//CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DOOMEROIDS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName	= NULL;//(LPCTSTR)IDC_DOOMEROIDS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, Int32)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
Bool InitInstance(HINSTANCE hInstance, Int32 nCmdShow)
{
	HWND hWnd;

#ifdef USE_CONSOLE
	if (!AllocConsole())
	{
		//g_Log.LOG_ERROR("Couldn't create console window!\n");
		MessageBox(NULL, "Couldn't create console window!", "ERROR", MB_OK);
	}
	//g_NetLog.m_bPrintToConsole = true;
#endif

	DWORD style = WS_POPUP;

#ifdef USE_TITLEBAR
	style = WS_OVERLAPPEDWINDOW;
#endif

	g_GameXMLMgr->LoadGameDefinition();
	g_GameXMLMgr->ReadSettings();

	hWnd = CreateWindow(szWindowClass, szTitle, style,
						0,					0,
						/*DEF_WINDOW_WIDTH*/	g_Settings.videoRes.resolution.width,
						/*DEF_WINDOW_HEIGHT*/	g_Settings.videoRes.resolution.height,
						NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//_cprintf("Window created.\r\n");

	g_hInst = hInstance;
   
	Game *theGame = g_Game;

	g_Game->Windowed = true;

	if (!(theGame->Init(hWnd)))
	{
		g_Log.LOG_ERROR("Error initializing game!\n");

		g_Game->DeleteInstance();

		return FALSE;
	}

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UInt32 message, WPARAM wParam, LPARAM lParam)
{
	if (Game::FlaggedForClose())
		PostQuitMessage(0);

	switch (message) 
	{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//WM_PAINT: MUST BE IN TO PREVENT WINDOWS FROM OVERRIDING THE OnPaint MSG AND SLOWING DOWN MAJORLY!!
	case WM_PAINT:
		break;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UInt32 message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////