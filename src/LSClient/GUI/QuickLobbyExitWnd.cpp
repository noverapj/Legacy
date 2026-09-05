#include "StdAfx.h"

#include "QuickLobbyExitWnd.h"

QuickLobbyExitWnd::QuickLobbyExitWnd()
{
}

QuickLobbyExitWnd::~QuickLobbyExitWnd()
{
}

void QuickLobbyExitWnd::iwm_create()
{
	QuickLobbyExitWndBase::iwm_create();
}

void QuickLobbyExitWnd::iwm_show()
{
	QuickLobbyExitWndBase::iwm_show();

	if( ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HEADQUARTERS )
	{
		HideChildWnd( PLAZA_BACK_BTN );
		ShowChildWnd( ALTF4_BTN );
	}
	else
	{
		ShowChildWnd( PLAZA_BACK_BTN );
		HideChildWnd( ALTF4_BTN );
	}
}

void QuickLobbyExitWnd::OnExitButtonDown( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	HideWnd();
}

void QuickLobbyExitWnd::OnGameExitBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	HideWnd();
	g_App.AltF4ExitProcess( false );
}

void QuickLobbyExitWnd::OnOptionBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	HideWnd();
	g_GUIMgr.ShowWnd( GAME_OPTION_WND );
}

void QuickLobbyExitWnd::OnPlazaBackBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	HideWnd();
	g_App.AltF4ExitProcess( false );
}