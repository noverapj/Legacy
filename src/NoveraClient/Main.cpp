
#include "stdafx.h"

#include "resource.h"
#include "Login.h"
#include "skin.h"
#include "ioAISymbolTable.h"

#ifdef NPROTECT
#include "nProtect/ioNProtect.h"
#endif 
 
//#define _USE_TCMALLOC
#ifdef _USE_TCMALLOC
#include "../gperftools-2.1/src/windows/gperftools/tcmalloc.h"
#endif

#include <io.h>

#define CLIENTMUTEXNAME  "Global\\_LOSTSAGA_CLIENT_3894738393994848484939308770494"

// For StickKeys Disable
STICKYKEYS g_StartupStickyKeys = {sizeof(STICKYKEYS), 0}; //Except Extracting Hangeul
FILTERKEYS g_StartupFilterKeys = {sizeof(FILTERKEYS), 0}; //Except Extracting Hangeul

void AllowAccessibilityShortcutKeys( bool bAllowKeys );

void InitOnWinMain( ioHashString &szOptions );
void RelaseOnWinMain( ioApplication *pApp );
void SendMsgToAutoUpgrade( DWORD dwWMTYPE );

extern bool g_bHappenCrash;
extern bool g_bHappenError;
extern int  g_iErrorNumber;
extern char g_szErrorLog[2048];

CLog g_MemoryLOG;

#if defined( USE_GA )
CLog g_GALog;
#endif

// error dlg skin
#define ERROR_DLG_WIDTH       410
#define ERROR_DLG_HEIGHT      298
#define ERROR_EDIT_WIDTH      376
#define ERROR_EDIT_HEIGHT     191
#define ERROR_X_WIDTH         25
#define ERROR_X_HEIGHT        26
#define ERROR_SOLUTION_WIDTH  286
#define ERROR_SOLUTION_HEIGHT 31
#define ERROR_EXIT_WIDTH      88
#define ERROR_EXIT_HEIGHT     31

#define ERROR_EDIT_BK_RGB   RGB(235, 235, 235)
#define ERROR_EDIT_TEXT_RGB RGB(58,58,58)
#define BTN_BITMAP_SIZE     3

CSkin  *g_pErrorDlgSkin = NULL;
HWND    g_hEdit         = NULL;
HBRUSH  g_hBrushEdit    = NULL;
HBITMAP g_hBitmapX[BTN_BITMAP_SIZE]={NULL,NULL,NULL};
HBITMAP g_hBitmapSolution[BTN_BITMAP_SIZE]={NULL,NULL,NULL};
HBITMAP g_hBitmapExit[BTN_BITMAP_SIZE]={NULL,NULL,NULL};
bool    g_bXOver        = false;
bool    g_bSolutionOver = false;
bool    g_bExitOver     = false;
WNDPROC	g_OldXProc      = NULL;
WNDPROC	g_OldSolutionProc= NULL;
WNDPROC	g_OldExitProc    = NULL;

BOOL CALLBACK ErrorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK XProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SolutionProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ExitProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void ShowErrorDlg( HINSTANCE hInstance );
void OnDrawItem( LPARAM lParam, WPARAM wParam );
 
int WINAPI WinMain( HINSTANCE hInstance,
				    HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nShowCmd )
{
#ifdef SHIPPING
	//파일의 존재여부를 확인하고, 속성 변경, 변경 실패 시, 악의적 행동으로 간주
	if( 0 == access("info/pp.log",0) )
	{
		if( !SetFileAttributes("info/pp.log",FILE_ATTRIBUTE_NORMAL))
			return 0;
	}
	LOG.OpenLog( 0, "info/pp.log");
#else
	//Admin버전은 다중 실행 될 수 있음으로 각 프로세스 별로 로그 파일을 생성 할 수 있도록 수정함
	int iProcessCount = Help::GetProcessRunningCount( "lostsaga.exe" );
	char szBuff[MAX_PATH];
	sprintf_s( szBuff, "info/pp_%d.log", iProcessCount );
	LOG.OpenLog( 0, szBuff );
#endif

	LOG.PrintTimeAndLog(0, "[Main] - Game Start");

	ioINIMemoryStatic* pStatics = new ioINIMemoryStatic;
	ioLocalManager *pLocalMgr = new ioLocalManager;
	if( pLocalMgr )
	{		
		pLocalMgr->Init();
		pLocalMgr->ParseCmd( lpCmdLine );
		LOG.PrintTimeAndLog(0, "[Main] - Local Manager Init Complete");
	}

#ifdef NPROTECT
	LOG.PrintTimeAndLog(0, "[Main] - nProtect Init Start" );
	if( !g_ioNProtect.Start() )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		ioStringManager *pStringMgr = new ioStringManager;
		if( pStringMgr && pLocal )
			pStringMgr->LoadData( "", "", pLocal->GetMemTextList(), true );

		char szError[MAX_PATH*2]="";
		g_ioNProtect.GetStartErrorMsg( szError, sizeof( szError ) );
		char szTitle[MAX_PATH*2]="";
		SafeSprintf( szTitle, sizeof( szTitle ), "nProtect Error(%d)" , g_ioNProtect.GetStartErrorType() );

		MessageBox( NULL, szError, szTitle, MB_TOPMOST );
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );
		ioNProtect::ReleaseInstance();
		LOG.PrintTimeAndLog(0, "[Main] - nProtect Error : %s:%s", szError, szTitle );
		
		//오토 업그레이드 에러 세팅
		SendMsgToAutoUpgrade( WM_MY_MSG_C_ERROR );
		Setting::Initialize("error");
		g_bHappenError = true;
		g_iErrorNumber = 707;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), "nProtect Error : %s:%s\n", szError, szTitle );
		// 소켓통신이 안되는 구간이므로 ini 기록하고 나중에 전송
		Help::SaveClientError( g_iErrorNumber, 0, 0 );
		ShowErrorDlg( hInstance );

		return 0;
	}
	LOG.PrintTimeAndLog(0, "[Main] - nProtect Init Complete");
#endif

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	ioStringManager *pStringMgr = new ioStringManager;
	if( pStringMgr && pLocal )
		pStringMgr->LoadData( "", "", pLocal->GetMemTextList(), true );


	if( pLocal && !pLocal->IsRightLicense() )
	{
		MessageBox( NULL, "Your license has com up for renewal.", "Novera", MB_OK );
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );

#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif

		LOG.PrintTimeAndLog(0, "[Main] - license Error" );
		return 0;
	}

	HANDLE hMutex = NULL;
#ifdef SHIPPING
	hMutex = CreateMutex(NULL, true, CLIENTMUTEXNAME );
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox( NULL, STR(2), "LostSaga_Client - Error", MB_TOPMOST );
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );

#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif

		LOG.PrintTimeAndLog( 0, "[Main] - Mutex Create Failed" );
		return 0;
	}
#endif

	DWORD dwVersion;
	char  szVersion[MAX_PATH];

	if( !Help::GetDXVersionInfo( &dwVersion, szVersion, MAX_PATH ) )
	{
		SendMsgToAutoUpgrade( WM_MY_MSG_C_ERROR );
		Setting::Initialize("error");
		g_bHappenError = true;
		g_iErrorNumber = 301;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(3) );
		// 소켓통신이 안되는 구간이므로 ini 기록하고 나중에 전송.
		Help::SaveClientError( g_iErrorNumber, 0, 0 );

		ShowErrorDlg( hInstance );

		if(hMutex != NULL)
			CloseHandle(hMutex);
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );

#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif

		MessageBox( NULL, "Get DX Info Error!", "Novera", MB_OK );
		LOG.PrintTimeAndLog( 0, "[Main] - Get DX Info Error" );
		return 0;
	}

	if( dwVersion < 0x00090003 )	// 9.0c
	{
		SendMsgToAutoUpgrade( WM_MY_MSG_C_ERROR );
		Setting::Initialize("error");
		g_bHappenError = true;
		g_iErrorNumber = 302;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(4) );
		// 소켓통신이 안되는 구간이므로 ini 기록하고 나중에 전송.
		Help::SaveClientError( g_iErrorNumber, 0, 0 );

		ShowErrorDlg( hInstance );
		if(hMutex != NULL)
			CloseHandle(hMutex);
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );

#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif

		MessageBox( NULL, "DX Version Error!", "Novera", MB_OK );
		LOG.PrintTimeAndLog( 0, "[Main] - DX Version Error!" );

		return 0;
	}

	Setting::SetDirectXVersion( szVersion, dwVersion );

	Setting::CheckOSVersion();
	ioFrameTimer::SetWindowsModule( !Setting::IsWin98Series() );

	ioApplication *pApp = new ioApplication;
	if( !pApp )	
	{
		if(hMutex != NULL)
			CloseHandle(hMutex);
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );
 
#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif
		
		MessageBox( NULL, "Create App Error!", "Novera", MB_OK );
		LOG.PrintTimeAndLog( 0, "[Main] - Create App Error!" );

		return 0;
	}

	int iPortOncmd = 0;

	ioHashString szKeyOnCmd, szOptionsOnCmd, szIPKeyOnCmd, szIPOnCmd;

	if(!pApp->ParseCmd(lpCmdLine, szKeyOnCmd ,szOptionsOnCmd ,szIPKeyOnCmd, szIPOnCmd ,iPortOncmd))
	{
		SendMsgToAutoUpgrade( WM_MY_MSG_C_ERROR );
		if(hMutex != NULL)
			CloseHandle(hMutex);
		SAFEDELETE( pApp );
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );

#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif
		
		MessageBox( NULL, "App Cmd Error!", "Novera", MB_OK );
		LOG.PrintTimeAndLog( 0, "[Main] - App Cmd Error!" );

		return 0;
	}

	InitOnWinMain( szOptionsOnCmd );
	
	LOG.PrintTimeAndLog( 0, "[Main] - Process Option Start" );

#ifdef _DEBUG
	if (!pApp->ProcessOptions(false, true))
#else
	if (!pApp->ProcessOptions(false, false))
#endif
	{
		RelaseOnWinMain(pApp);
		SendMsgToAutoUpgrade( WM_MY_MSG_C_ERROR );
		if(hMutex != NULL)
			CloseHandle(hMutex);
		SAFEDELETE( pLocalMgr );
		SAFEDELETE( pStringMgr );
		SAFEDELETE( pStatics );

#ifdef NPROTECT
		ioNProtect::ReleaseInstance();
#endif

		MessageBox( NULL, "App Option Error!", "Novera", MB_OK );
		LOG.PrintTimeAndLog( 0, "[Main] - Process Option Start Failed" );
		return 0;
	}

	LOG.PrintTimeAndLog( 0, "[Main] - Process Option Start Complete" );

	// For Sticky Keys Disable
	// 초기값 백업
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
	SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);

	// 설정변경
	AllowAccessibilityShortcutKeys( false );
	
	int iRet = -1;
	//윈도우가 생성 됬을 경우에만 다음 로직을 수행하도록 변경
	if( pApp->InitWindow( hInstance, szKeyOnCmd ) )
	{
		LOG.PrintTimeAndLog( 0, "[Main] - InitWindow Complete" );

		if (!pApp->SetupNetwork(szIPOnCmd, iPortOncmd))
		{
			LOG.PrintTimeAndLog(0, "[Main] - Setup Network Failed");
			MessageBox(NULL, "Failed to setup network", "Novera", MB_OK);

		}

		//소켓 생성과 초기화에 성공한 경우에만 실행 하도록 변경
		if( pApp->SetupBeforeLogin() )
		{
			LOG.PrintTimeAndLog( 0, "[Main] - Setup Before Login Complete" );

			if( pApp->LoginLoop( szKeyOnCmd, szIPKeyOnCmd ) )
			{
#ifndef _DEBUG
				if( Help::CheckEnableTimeGapUser(1) || g_App.IsMeDeveloper() )
				{
					ioINIMemoryStatic::GetSingleton().SetSwitch( SATICS_ON );
					g_MemoryLOG.OpenLog( 0, "info/pp_mem.log");
				}
#else			
				ioINIMemoryStatic::GetSingleton().SetSwitch( SATICS_ON );
				g_MemoryLOG.OpenLog( 0, "info/pp_mem.log");
#endif
				LOG.PrintTimeAndLog( 0, "[Main] - Run Call" );
				iRet = pApp->Run();
			}
			else
			{
				LOG.PrintTimeAndLog(0, "[Main] - App LoginLoop Error" );
				MessageBox( NULL, "App LoginLoop Error!", "Novera", MB_OK );
			}
		}
		else
		{
			LOG.PrintTimeAndLog(0, "[Main] - App SetupBeforeLogin Error" );
			MessageBox( NULL, "App SetupBeforeLogin Error!", "Novera", MB_OK );
		}
	}
	else
	{
		LOG.PrintTimeAndLog(0, "[Main] - App InitWindow Error" );
		MessageBox( NULL, "App InitWindow Error!", "Novera", MB_OK );
	}
	
	// 정상종료 : DestroyWindow->ReleaseAll->PostQuitMessage
	if( iRet == -1 )
	{
		DestroyWindow( pApp->GetHWnd() );

		MSG msg;
		for ( int i = 0; i < 10000; i++ ) // 10초간 확인
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
			{
				if( !GetMessage( &msg, NULL, 0, 0 ) )
					break;

				TranslateMessage( &msg );
				DispatchMessage( &msg );
				continue;
			}
			::SleepEx(1, FALSE);
		}
	}

	RelaseOnWinMain(pApp);

	// For Sticky Keys Disable, 설정복구
	AllowAccessibilityShortcutKeys( true );

	if( g_bHappenCrash || g_bHappenError )
	{
		ShowErrorDlg( hInstance );
	}

	if(hMutex != NULL)
		CloseHandle(hMutex);

	SAFEDELETE( pLocalMgr );
	SAFEDELETE( pStringMgr );
	SAFEDELETE( pStatics );
	//SAFEDELETE( pSymtable );

//	 _CrtMemDumpAllObjectsSince(0);

    return iRet;
}


///////////////////////////////////////////////////////////
BOOL CALLBACK ErrorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static char szEditText[4096 * 2];

	switch(message)
	{
	case WM_INITDIALOG:
		{
			// back skin
			if( g_pErrorDlgSkin )
			{
				g_pErrorDlgSkin->Hook(hDlg);
				g_pErrorDlgSkin->Enable(true);
			}
			//

			// edit skin
			g_hEdit = GetDlgItem( hDlg, IDC_ERROR_EDIT );
			MoveWindow(g_hEdit, 18, 47, ERROR_EDIT_WIDTH, ERROR_EDIT_HEIGHT, TRUE);
			//

			// btn 
			HWND hWndCtrl = GetDlgItem( hDlg, IDC_X );
			MoveWindow(hWndCtrl, 380, 5, ERROR_X_WIDTH, ERROR_X_HEIGHT, TRUE);
			g_OldXProc = (WNDPROC)GetWindowLong(hWndCtrl, GWL_WNDPROC);
			SetWindowLong(hWndCtrl, GWL_WNDPROC, (LONG)XProc);

			hWndCtrl = GetDlgItem( hDlg, IDC_SOLUTION );
			MoveWindow(hWndCtrl, 17, 252, ERROR_SOLUTION_WIDTH, ERROR_SOLUTION_HEIGHT, TRUE);
			g_OldSolutionProc = (WNDPROC)GetWindowLong(hWndCtrl, GWL_WNDPROC);
			SetWindowLong(hWndCtrl, GWL_WNDPROC, (LONG)SolutionProc);

			hWndCtrl = GetDlgItem( hDlg, IDOK );
			MoveWindow(hWndCtrl, 305, 252, ERROR_EXIT_WIDTH, ERROR_EXIT_HEIGHT, TRUE);
			g_OldExitProc = (WNDPROC)GetWindowLong(hWndCtrl, GWL_WNDPROC);
			SetWindowLong(hWndCtrl, GWL_WNDPROC, (LONG)ExitProc);
			//

			g_hBrushEdit = CreateSolidBrush( ERROR_EDIT_BK_RGB );

			//----------창 중간에 띄우기 ---------------------
			RECT workrect;
			// Find how large the desktop work area is
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
			int workwidth  = workrect.right -  workrect.left;
			int workheight = workrect.bottom - workrect.top;

			// And how big the window is
			int winwidth  = ERROR_DLG_WIDTH;
			int winheight = ERROR_DLG_HEIGHT;
			// Make sure it"s not bigger than the work area
			winwidth  = min(winwidth, workwidth);
			winheight = min(winheight, workheight);

			// Now center it
			SetWindowPos( hDlg,
						  HWND_TOP,
						  workrect.left + (workwidth-winwidth) / 2,
						  workrect.top + (workheight-winheight) / 2,
						  winwidth, winheight, 
						  SWP_SHOWWINDOW);
			//----------------------------------------------------------

			SafeSprintf(    szEditText,
				            sizeof( szEditText ),
					        STR(1),
					        Setting::GetFullSystemInfo(),
					        g_szErrorLog );

			SetDlgItemText( hDlg, IDC_ERROR_EDIT, szEditText );
			SetForegroundWindow( hDlg );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->GetResourceIDErrorSolutionN() == -1 )
			{
				HWND hChildWnd = GetDlgItem( hDlg, IDC_SOLUTION );
				EnableWindow( hChildWnd, false );
				ShowWindow( hChildWnd, SW_HIDE );
			}

			SetWindowText( hDlg, "Lost Saga" ); 
		}
		return TRUE;
	case WM_CTLCOLORSTATIC:
		if( (HWND) lParam == g_hEdit )
		{
			SetTextColor( (HDC)wParam, ERROR_EDIT_TEXT_RGB );
			SetBkColor( (HDC)wParam, ERROR_EDIT_BK_RGB );
			return (BOOL) g_hBrushEdit;
		}
		else
			return TRUE;
	case WM_DRAWITEM:	
		OnDrawItem(lParam, wParam);
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
		case IDCANCEL:
		case IDC_X:
			EndDialog( hDlg, 0 );
			DeleteObject( g_hBrushEdit );
			return TRUE;
		case IDC_SOLUTION:
			{
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal )
				{
					//기능블럭
					//if( g_iErrorNumber == 328 )
					//{
					//	pLocal->ShowHackShieldErrorDlg( g_MyInfo.GetPrivateID() );
					//}
					//else
					{
						char szURL[MAX_PATH]="";
						StringCbPrintf( szURL, sizeof(szURL), pLocal->GetErrorSolutionURL(), g_iErrorNumber );
						Help::ExcuteWeb( szURL );
					}
				}
			}
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}

void ShowErrorDlg( HINSTANCE hInstance )
{
	DWORD dwBack = IDB_BACK_ERROR;
	DWORD dwSolutionN = IDB_SOLUTION_N;
	DWORD dwSolutionO = IDB_SOLUTION_O;
	DWORD dwSolutionC = IDB_SOLUTION_C;
	DWORD dwExitN = IDB_EXIT_N;
	DWORD dwExitO = IDB_EXIT_O;
	DWORD dwExitC = IDB_EXIT_C;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		if( pLocal->GetResourceIDErrorBack() != -1 )
			dwBack = pLocal->GetResourceIDErrorBack();
		if( pLocal->GetResourceIDErrorSolutionN() != -1 )
			dwSolutionN = pLocal->GetResourceIDErrorSolutionN();
		if( pLocal->GetResourceIDErrorSolutionO() != -1 )
			dwSolutionO = pLocal->GetResourceIDErrorSolutionO();
		if( pLocal->GetResourceIDErrorSolutionC() != -1 )
			dwSolutionC = pLocal->GetResourceIDErrorSolutionC();
		if( pLocal->GetResourceIDErrorExitN() != -1 )
			dwExitN = pLocal->GetResourceIDErrorExitN();
		if( pLocal->GetResourceIDErrorExitO() != -1 )
			dwExitO = pLocal->GetResourceIDErrorExitO();
		if( pLocal->GetResourceIDErrorExitC() != -1 )
			dwExitC = pLocal->GetResourceIDErrorExitC();
	}

	g_hBitmapX[0] = LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_X_N) );
	g_hBitmapX[1] = LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_X_O) );
	g_hBitmapX[2] = LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_X_C) );
	g_hBitmapSolution[0] = LoadBitmap( hInstance, MAKEINTRESOURCE(dwSolutionN) );
	g_hBitmapSolution[1] = LoadBitmap( hInstance, MAKEINTRESOURCE(dwSolutionO) );
	g_hBitmapSolution[2] = LoadBitmap( hInstance, MAKEINTRESOURCE(dwSolutionC) );
	g_hBitmapExit[0] = LoadBitmap( hInstance, MAKEINTRESOURCE(dwExitN) );
	g_hBitmapExit[1] = LoadBitmap( hInstance, MAKEINTRESOURCE(dwExitO) );
	g_hBitmapExit[2] = LoadBitmap( hInstance, MAKEINTRESOURCE(dwExitC) );

	ShowCursor( TRUE );
	g_pErrorDlgSkin = new CSkin(IDR_BINARY1, dwBack );

	DialogBox( hInstance, MAKEINTRESOURCE(IDD_ERROR_DLG), NULL, ErrorDlgProc );
	
	SAFEDELETE( g_pErrorDlgSkin );
	for( int i=0 ; i< BTN_BITMAP_SIZE ; i++)
	{
		DeleteObject( g_hBitmapX[i] );			
		DeleteObject( g_hBitmapSolution[i] );
		DeleteObject( g_hBitmapExit[i] );
	}
}

void OnDrawItem( LPARAM lParam, WPARAM wParam )
{
	HDC	hdc;
	DRAWITEMSTRUCT *lpDi = NULL;
	lpDi = (LPDRAWITEMSTRUCT)lParam;	
	if( !lpDi )
		return;

	switch(wParam)
	{
	case IDC_X:
		hdc = CreateCompatibleDC(lpDi->hDC);
		if( lpDi->itemState & ODS_SELECTED)
			SelectObject(hdc, g_hBitmapX[2]);
		else if(g_bXOver == true)
			SelectObject(hdc, g_hBitmapX[1]);		
		else
			SelectObject(hdc, g_hBitmapX[0]);

		BitBlt(lpDi->hDC,0,0, ERROR_X_WIDTH, ERROR_X_HEIGHT, hdc, 0, 0, SRCCOPY);				
		DeleteDC(hdc);
		break;
	case IDC_SOLUTION:
		hdc = CreateCompatibleDC(lpDi->hDC);
		if( lpDi->itemState & ODS_SELECTED)
			SelectObject(hdc, g_hBitmapSolution[2]);
		else if(g_bSolutionOver == true)
			SelectObject(hdc, g_hBitmapSolution[1]);		
		else
			SelectObject(hdc, g_hBitmapSolution[0]);

		BitBlt(lpDi->hDC,0,0, ERROR_SOLUTION_WIDTH, ERROR_SOLUTION_HEIGHT, hdc, 0, 0, SRCCOPY);				
		DeleteDC(hdc);
		break;
	case IDOK:
		hdc = CreateCompatibleDC(lpDi->hDC);
		if( lpDi->itemState & ODS_SELECTED)
			SelectObject(hdc, g_hBitmapExit[2]);
		else if(g_bExitOver == true)
			SelectObject(hdc, g_hBitmapExit[1]);		
		else
			SelectObject(hdc, g_hBitmapExit[0]);

		BitBlt(lpDi->hDC,0,0, ERROR_EXIT_WIDTH, ERROR_EXIT_HEIGHT, hdc, 0, 0, SRCCOPY);				
		DeleteDC(hdc);
		break;
	}
}

LRESULT CALLBACK XProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE; 
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 1; 
		TrackMouseEvent(&tme);  
		break;
	case WM_MOUSEHOVER:
		g_bXOver = true;
		InvalidateRect(hwnd, NULL, FALSE);	
		break;
	case WM_MOUSELEAVE:		
		g_bXOver = false;
		InvalidateRect(hwnd, NULL, FALSE);	
		break;
	}

	return CallWindowProc(g_OldXProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK SolutionProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE; 
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 1; 
		TrackMouseEvent(&tme);  
		break;
	case WM_MOUSEHOVER:
		g_bSolutionOver = true;
		InvalidateRect(hwnd, NULL, FALSE);	
		break;
	case WM_MOUSELEAVE:		
		g_bSolutionOver = false;
		InvalidateRect(hwnd, NULL, FALSE);	
		break;
	}	

	return CallWindowProc(g_OldXProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ExitProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE; 
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 1; 
		TrackMouseEvent(&tme);  
		break;
	case WM_MOUSEHOVER:
		g_bExitOver = true;
		InvalidateRect(hwnd, NULL, FALSE);	
		break;
	case WM_MOUSELEAVE:		
		g_bExitOver = false;
		InvalidateRect(hwnd, NULL, FALSE);	
		break;
	}

	return CallWindowProc(g_OldXProc, hwnd, message, wParam, lParam);
}

void InitOnWinMain(ioHashString &szOptions)
{
	timeBeginPeriod(1);
	srand( timeGetTime() );	

#ifndef SHIPPING		
	if( szOptions.IsEmpty() || szOptions.At(CLogin::OT_SHOW_CONSOLWINDOW) != '0')
	{		
		LOG.OpenConsole( 1 );
	}
#endif // SHIPPING

	ioCPU::Init();
}

void RelaseOnWinMain(ioApplication *pApp)
{
	SAFEDELETE( pApp );

	ioCPU::ShotDown();
	ioResourceLoader::ReleaseInstance();

	LOG.CloseAndRelease();
	timeEndPeriod(1);
}

void SendMsgToAutoUpgrade( DWORD dwWMTYPE )
{
	HWND hWnd = FindWindow(AUTPUPGRADE_CLASS_NAME,NULL);
	if(hWnd != NULL)
	{
		static COPYDATASTRUCT cds;
		cds.dwData = dwWMTYPE;
		cds.cbData = 0;
		cds.lpData = NULL;
		SendMessage(hWnd, WM_COPYDATA, 0,(LPARAM)&cds);
	}
}

void AllowAccessibilityShortcutKeys( bool bAllowKeys )
{
    if( bAllowKeys )
    {
        // 초기값으로 복귀
        SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
        SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);
    }
    else
    {
        // 불가상태로 전환
         STICKYKEYS skOff = g_StartupStickyKeys;

		if( (skOff.dwFlags & SKF_STICKYKEYSON) == 0 )
		{
			// Disable the hotkey
			skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
	 
			SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
		}
 
        FILTERKEYS fkOff = g_StartupFilterKeys;

		if( (fkOff.dwFlags & FKF_FILTERKEYSON) == 0 )
		{
			// Disable the hotkey
			fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
	 
			SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
		}
    }
}
