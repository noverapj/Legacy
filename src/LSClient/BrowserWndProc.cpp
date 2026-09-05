

#include "stdafx.h"

#include "WellKnownURL.h"
#include "ioWebBrowser.h"
#include "ioBrowserManager.h"
#include "resource.h"

WNDPROC DefaultButtonProc = NULL;

LRESULT ExitButtonProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_MOUSEMOVE:
		if( GetCapture() != hWnd )
		{
			SetCapture( hWnd );
			InvalidateRect( hWnd, NULL, FALSE );
		}
		else
		{
			POINT ptMouse;
			ptMouse.x = LOWORD(lParam);
			ptMouse.y = HIWORD(lParam);

			RECT rcRect;
			GetClientRect(hWnd, &rcRect);

			if( !PtInRect( &rcRect, ptMouse ) )
			{		
				InvalidateRect(hWnd, NULL, FALSE);	
				ReleaseCapture();
			}
		}
		break;
	case WM_MOUSELEAVE:
		InvalidateRect( hWnd, NULL, FALSE );
		ReleaseCapture();
		break;
	}

	if( DefaultButtonProc )
		return CallWindowProc( DefaultButtonProc, hWnd, message, wParam, lParam );

	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT CALLBACK WebParentProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
		{
			RECT rcClient;
			GetClientRect( hWnd, &rcClient );

			HWND hButton = CreateWindow( "button", "Exit", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
										 rcClient.right - 26, 3, 22, 22,
										 hWnd,
										 (HMENU)IDC_WEB_EXIT,
										 g_BrowserMgr.GetHInstance(),
										 NULL );

			if( !DefaultButtonProc )
			{
				DefaultButtonProc = (WNDPROC)GetWindowLong( hButton, GWL_WNDPROC );
			}

			SetWindowLong( hButton, GWL_WNDPROC, (LONG)ExitButtonProc);
		}
		break;
	case WM_SIZE:
		{
			RECT rcClient;
			GetClientRect( hWnd, &rcClient );

			HWND hExit = FindWindowEx( hWnd, 0, "button", "Exit" );
			if( hExit )
			{
				MoveWindow( hExit,rcClient.right - 26, 3, 22, 22, TRUE );
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint( hWnd, &ps );
			if( hDC )
			{
				RECT rcClient;
				GetClientRect( hWnd, &rcClient );

				g_BrowserMgr.DrawWebParent( hWnd, hDC, rcClient );
				EndPaint( hWnd, &ps );
			}
		}
		break;
	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDi = (LPDRAWITEMSTRUCT)lParam;

			if( lpDi->itemState & ODS_SELECTED )
			{
				g_BrowserMgr.DrawExitButton( lpDi->hDC, 2 );	// click
			}
			else
			{
				HWND hExit = FindWindowEx( hWnd, 0, "button", "Exit" );
				if( hExit )
				{
					POINT ptCursor;
					GetCursorPos( &ptCursor );
					ScreenToClient( hExit, &ptCursor );

					RECT rcClient;
					GetClientRect( hExit, &rcClient );

					if( PtInRect( &rcClient, ptCursor ) )
						g_BrowserMgr.DrawExitButton( lpDi->hDC, 1 );	// over
					else
						g_BrowserMgr.DrawExitButton( lpDi->hDC, 0 );	// normal
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		SetCapture( hWnd );
		g_BrowserMgr.WebParentClicked( hWnd );
		break;
	case WM_LBUTTONUP:
		g_BrowserMgr.WebParentClickOff( hWnd );
		ReleaseCapture();
		break;
	case WM_COMMAND:
		if( LOWORD(wParam) == IDC_WEB_EXIT )
		{
			g_BrowserMgr.CloseBrowser( hWnd );
		}
		break;
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
		{
			g_BrowserMgr.CloseBrowser( hWnd );
		}
		break;
	case WM_CLOSE:
		if( !g_BrowserMgr.CloseBrowser( hWnd ) )
			return TRUE;
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}