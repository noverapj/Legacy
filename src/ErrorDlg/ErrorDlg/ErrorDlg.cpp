// ErrorDlg.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ErrorDlg.h"
#include "StringManager\ioHashString.h"
#include "StringManager/Safesprintf.h"
#include "StringManager/ioStringManager.h"
#include <stdio.h>
#include <conio.h>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.


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
ioLocalManager* gLocalMgr = NULL;
ioStringManager* gioStringMgr = NULL;
HWND	g_hWnd;
BOOL CALLBACK ErrorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK XProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SolutionProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ExitProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void ShowErrorDlg( HINSTANCE hInstance );
void OnDrawItem( LPARAM lParam, WPARAM wParam );

char g_szErrorLog[MAX_PATH*MAX_PATH] = {0,};

char g_szClientErrorLog[MAX_PATH]	= {0,};
char g_szLocalType[MAX_PATH]	= {0,};
char g_szSetting[MAX_PATH]	= {0,};
char g_szError[MAX_PATH]	= {0,};


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	char szErrorLog[2048] = "Version ";
	strcat_s(szErrorLog, lpCmdLine);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ERRORDLG, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	int i = 0;
	int iCnt = 0;
	bool bReadOk = false;
	int TokenCnt = 0;
	
	while( true )
	{
		if( TokenCnt == 0 )
			g_szSetting[iCnt]		= szErrorLog[i];
		else if( TokenCnt == 1)
			g_szClientErrorLog[iCnt]= szErrorLog[i];
		else if( TokenCnt == 2)
			g_szLocalType[iCnt]		= szErrorLog[i];
		else
			break;

		i++;
		iCnt++;

		if( szErrorLog[i] == '^' ){
			iCnt = 0;
			i++;
			TokenCnt++;
		}
	}

	gLocalMgr = new ioLocalManager;
	gLocalMgr->Init();
	if( !gLocalMgr->ParseCmd(g_szLocalType) ){
		SAFEDELETE(gLocalMgr);
		return FALSE;
	}

	ioLocalParent*	pLocal = gLocalMgr->GetLocal( ioLocalManager::GetLocalType() );

	gioStringMgr = new ioStringManager;
	if( gioStringMgr )
		gioStringMgr->LoadData( "", "", pLocal->GetMemTextList(), true  );
	else
		return false;

	ShowErrorDlg(hInstance);
	PostQuitMessage(0);

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ERRORDLG));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ERRORDLG);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	g_hWnd = hWnd;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

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
				g_szSetting,
				g_szClientErrorLog
				);

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
			SAFEDELETE(gioStringMgr);
			SAFEDELETE(gLocalMgr);
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
//						StringCbPrintf( szURL, sizeof(szURL), pLocal->GetErrorSolutionURL(), g_iErrorNumber );
//						Help::ExcuteWeb( szURL );
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
	//HWND hDlg;
	//hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_ERROR_DLG),NULL,ErrorDlgProc);
	//ShowWindow(hDlg,SW_SHOW);

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