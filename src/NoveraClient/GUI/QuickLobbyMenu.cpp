#include "StdAfx.h"

#include "QuickLobbyMenu.h"
#include "TimeGateSearchShuffle.h"
#include "QuickLobbyWnd.h"
#include "ioFrameTimer.h"

#include "../ioDateHelp.h"
#include "../TournamentNode.h"
#include "../ioTournamentManager.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioModeSelectInfo.h"


ioShuffleButtonEx::ioShuffleButtonEx() : ioImageButtonWndEX( g_GUIMgr.GetSingletonPtr() )
{
}

ioShuffleButtonEx::~ioShuffleButtonEx()
{
}

void ioShuffleButtonEx::OnRender()
{
	ioImageButtonWndEX::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioHashString szTitle = g_ModeSelectInfo.GetBattleModeTitle( g_ShuffleRoomMgr.GetTodayModeType( DateHelp::GetTodayOfWeek(), 0 ) );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 264, iYPos + 16, FONT_SIZE_11, "%s", szTitle.c_str() );
}

//----------------------------------------------------------------------------------------------------------

QuickLobbyMenu::QuickLobbyMenu()
{	
	m_iMyCampPos = CAMP_NONE;

	m_bCurrDisableTournament = false;

	m_iDefaultX = 0;
	m_iDefaultY = 0;

	m_dwStartTime = 0;
}

QuickLobbyMenu::~QuickLobbyMenu()
{	
}

void QuickLobbyMenu::iwm_create()
{
	QuickLobbyMenuBase::iwm_create();
	QuickLobbyTournament* pWnd = dynamic_cast<QuickLobbyTournament*>( g_GUIMgr.AddWnd( "XML/QuickLobbyTournament.xml", this ) );
	if( pWnd )
		pWnd->AddExceptionList( FindChildWnd( TOURNAMENT_BTN ) );

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();
}

void QuickLobbyMenu::iwm_show()
{
	QuickLobbyMenuBase::iwm_show();

	m_iMyCampPos             = g_MyInfo.GetUserCampPos();
	m_bCurrDisableTournament = g_TournamentMgr.IsRegularDisableTournament();

	if( m_bCurrDisableTournament )
		SetDisableTournament( m_bCurrDisableTournament );

	if( ioPlayMode::GetModeType() == MT_HEADQUARTERS )
	{
		HideChildWnd( HEAD_QUATER_BTN );
		ShowChildWnd( PLAZA_GO_BTN );
	}
	else
	{
		ShowChildWnd( HEAD_QUATER_BTN );
		HideChildWnd( PLAZA_GO_BTN );
	}

	m_dwStartTime = FRAMEGETTIME();
}

bool QuickLobbyMenu::iwm_esc()
{	
	ioWnd* pWnd = FindChildWnd( TOURNAMENT_MENU_WND );
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->iwm_esc();
		return true;
	}	

	return false;
}

void QuickLobbyMenu::OnBattleLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		HideChildWnd( TOURNAMENT_MENU_WND );

		if( pWnd->GetCurrMenu() == QuickLobbyWnd::BATTLE )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();
		}
		else
		{
			pWnd->ShowMenu( QuickLobbyWnd::BATTLE );			
		}
	}
}

void QuickLobbyMenu::OnHeadQuartersLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		HideChildWnd( TOURNAMENT_MENU_WND );
		pWnd->OnCreateHeadquarters();
		pWnd->ResetCurrMenu();
	}
}

void QuickLobbyMenu::OnPlazaGOLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	g_App.SendSearchModeJoin();
}

void QuickLobbyMenu::OnPlazaLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		HideChildWnd( TOURNAMENT_MENU_WND );

		if( pWnd->GetCurrMenu() == QuickLobbyWnd::PLAZA )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();
		}
		else
		{
			pWnd->ShowMenu( QuickLobbyWnd::PLAZA );
		}
	}
}

void QuickLobbyMenu::OnShuffleLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	TimeGateSearchShuffle *pSearchWnd = dynamic_cast<TimeGateSearchShuffle*>( g_GUIMgr.FindWnd( TIMEGATE_SEARCH_SHUFFLE ) );
	if( pSearchWnd )
	{
		HideChildWnd( TOURNAMENT_MENU_WND );
		pSearchWnd->SetSearching();
	}
}

void QuickLobbyMenu::OnTournamentLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	ioWnd* pTourMenu = FindChildWnd( TOURNAMENT_MENU_WND );
	if( pTourMenu && pWnd )
	{
		if( pTourMenu->IsShow() )
		{
			HideChildWnd( TOURNAMENT_MENU_WND );

			if( pWnd )
			{
				pWnd->HideWnd();
				pWnd->ResetCurrMenu();
			}
		}
		else
		{
			ShowChildWnd( TOURNAMENT_MENU_WND );

			if( pWnd )
			{
				pWnd->HideWnd();
				pWnd->ResetCurrMenu();
			}
		}
	}
}

void QuickLobbyMenu::OnTradeLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		if( pWnd->GetCurrMenu() == QuickLobbyWnd::TRADE )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();
		}
		else
		{
			pWnd->ShowMenu( QuickLobbyWnd::TRADE );
		}
	}
}

void QuickLobbyMenu::OnCampLButtonUP( bool breShow )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		if( pWnd->GetCurrMenu() == QuickLobbyWnd::CAMP )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();

			if( breShow )
				pWnd->ShowMenu( QuickLobbyWnd::CAMP );
		}
		else
		{
			pWnd->ShowMenu( QuickLobbyWnd::CAMP );
		}
	}
}

void QuickLobbyMenu::OnTournamentMainLButtonUP()
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		if( pWnd->GetCurrMenu() == QuickLobbyWnd::TOURNAMENT )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();
		}
		else
		{
			pWnd->ShowMenu( QuickLobbyWnd::TOURNAMENT );
		}
	}
}

void QuickLobbyMenu::RefreshTournamentTab()
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
	{
		if( pWnd->GetCurrMenu() == QuickLobbyWnd::TOURNAMENT )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();
			pWnd->ShowMenu( QuickLobbyWnd::TOURNAMENT );
		}
		else if( pWnd->GetCurrMenu() == QuickLobbyWnd::CAMP )
		{
			pWnd->HideWnd();
			pWnd->ResetCurrMenu();
			pWnd->ShowMenu( QuickLobbyWnd::CAMP );
		}
	}
}

void QuickLobbyMenu::SetTournamentLink( DWORD dwTourIndex )
{
	QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
	if( pWnd )
		pWnd->SetTournamentLink(  dwTourIndex );
	
	ShowChildWnd( TOURNAMENT_MENU_WND );
}

void QuickLobbyMenu::SetDisableTournament( bool bDisable )
{
	QuickLobbyTournament *pWnd = dynamic_cast<QuickLobbyTournament*>( FindChildWnd( TOURNAMENT_MENU_WND ) );
	if( pWnd )	
		pWnd->SetDisableTournament( bDisable );
}

void QuickLobbyMenu::OnProcess( float fTimePerSec )
{
	if( IsShow() )
	{
		//진영 정보가 바뀌면 UI 갱신
		int iPrevMyCampPos = m_iMyCampPos;
		if( m_iMyCampPos != g_MyInfo.GetUserCampPos() )
		{
			m_iMyCampPos = g_MyInfo.GetUserCampPos();
			QuickLobbyWnd *pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
			if( pWnd )
				pWnd->RefreshCampInfo( iPrevMyCampPos );
		}

		//대회 블럭 기능
		if( m_bCurrDisableTournament != g_TournamentMgr.IsRegularDisableTournament() )
		{
			m_bCurrDisableTournament = g_TournamentMgr.IsRegularDisableTournament();
			SetDisableTournament( m_bCurrDisableTournament );
		}
	}
	

	if( !g_MyInfo.IsBankruptcy() && !g_MyInfo.IsCharBankruptcy() && !g_MyInfo.IsTutorialUser() && !IsShow() )
		ShowWnd();

	if( IsShow() && g_MyInfo.IsBankruptcy() && !g_MyInfo.IsCharBankruptcy() )
		HideWnd();

	if( IsShow() && g_MyInfo.IsCharBankruptcy() && !g_MyInfo.IsBankruptcy() )
		HideWnd();

	if( IsShow() && g_MyInfo.IsTutorialUser() )
		HideWnd();

}

void QuickLobbyMenu::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos + fPosGap;

	QuickLobbyMenuBase::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();

		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

//----------------------------------------------------------------------------------------------------------

QuickLobbyTournament::QuickLobbyTournament()
{
	m_dwCurrActive = 0;
}

QuickLobbyTournament::~QuickLobbyTournament()
{
}

bool QuickLobbyTournament::iwm_esc()
{
	HideWnd();
	return true;
}

void QuickLobbyTournament::iwm_show()
{
	if( g_CampMgr.GetNextTodayBattleSec() )
	{
		ShowChildWnd( CAMP_PROGRESS );
		HideChildWnd( CAMP_UNPROGRESS );
	}
	else
	{
		HideChildWnd( CAMP_PROGRESS );
		ShowChildWnd( CAMP_UNPROGRESS );
	}

	if( g_TournamentMgr.IsRegularDisableTournament() )
	{
		ShowChildWnd( TOUR_REST );
		HideChildWnd( TOUR_APP );
		HideChildWnd( TOUR_CHEER );
		HideChildWnd( TOUR_BATTLE );
	}
	else
	{
		switch( g_TournamentMgr.GetRegularState() )
		{
		case TournamentNode::STATE_TEAM_APP:
			{
				HideChildWnd( TOUR_REST );
				ShowChildWnd( TOUR_APP );
				HideChildWnd( TOUR_CHEER );
				HideChildWnd( TOUR_BATTLE );
			}
			break;
		case TournamentNode::STATE_TEAM_DELAY:
			{
				HideChildWnd( TOUR_REST );
				HideChildWnd( TOUR_APP );
				ShowChildWnd( TOUR_CHEER );
				HideChildWnd( TOUR_BATTLE );
			}
			break;
		default:
			{
				HideChildWnd( TOUR_REST );
				HideChildWnd( TOUR_APP );
				HideChildWnd( TOUR_CHEER );
				ShowChildWnd( TOUR_BATTLE );
			}
			break;
		}
	}	
}

void QuickLobbyTournament::AddExceptionList( ioWnd* pWnd )
{
	m_ExceptionList.push_back( pWnd );
}

void QuickLobbyTournament::iwm_hide()
{
}

void QuickLobbyTournament::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( g_GUIMgr.IsElsewhereClick( this, m_ExceptionList ) )
	{
		HideWnd();
	}
}

void QuickLobbyTournament::OnCampMainLButtonUp( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyMenu *pWnd = dynamic_cast<QuickLobbyMenu*>( g_GUIMgr.FindWnd( LOBBY_QUICK_MENU ) );
	if( pWnd )	
		pWnd->OnCampLButtonUP();
}

void QuickLobbyTournament::OnTournamentLButtonUp( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	QuickLobbyMenu *pWnd = dynamic_cast<QuickLobbyMenu*>( g_GUIMgr.FindWnd( LOBBY_QUICK_MENU ) );
	if( pWnd )	
		pWnd->OnTournamentMainLButtonUP();
}

void QuickLobbyTournament::SetDisableTournament( bool bDisable )
{
	SetChildWndStyleRemove( ID_TOURNAMENT_BTN, IWS_EXACTIVE );
	if( bDisable )
		SetChildWndStyleAdd( ID_TOURNAMENT_BTN, IWS_EXACTIVE );

	ioWndEXMouseEvent NullEvent;
	QuickLobbyMenu *pWnd = dynamic_cast<QuickLobbyMenu*>( g_GUIMgr.FindWnd( LOBBY_QUICK_MENU ) );
	if( pWnd )
		pWnd->OnBattleLButtonUP( NULL, NullEvent );
}
