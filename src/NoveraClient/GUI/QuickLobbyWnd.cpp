#include "StdAfx.h"

#include "QuickLobbyWnd.h"
#include "CampMainWnd.h"
#include "PlazaMainWnd.h"
#include "TradeMainWnd.h"
#include "TournamentMainWnd.h"
#include "QuickModeToolTip.h"
#include "PartyMenuWnd.h"
#include "LadderTeamWnd.h"

QuickLobbyWnd::QuickLobbyWnd()
{
	m_pModeHelpTooltip  = NULL;
	m_pQuickJoinTooltip = NULL;
	m_dwCurrMenu		= NONE;
}

QuickLobbyWnd::~QuickLobbyWnd()
{
}

void QuickLobbyWnd::iwm_create()
{
	QuickLobbyWndBase::iwm_create();

	//전투
	PartyCatalogueWnd* pParty = dynamic_cast<PartyCatalogueWnd*>( g_GUIMgr.AddWnd( "XML/partycataloguewnd.xml", this ) );
	if( pParty )
	{
		pParty->SetMaxPartyList( PartyCatalogueWnd::PLAZA_MAX_PARTY_LIST );
		pParty->PlazaUIMove();
	}

	//진영전
	CampMainWnd* pCamp = dynamic_cast<CampMainWnd*>( g_GUIMgr.AddWnd( "XML/campmainwnd.xml", this ) );
	if( pCamp )
	{
		LadderTeamListWnd* pTeamWnd = dynamic_cast<LadderTeamListWnd*>( pCamp->FindChildWnd( CampMainWnd::ID_CAMP_TEAM_WND ) );
		if( pTeamWnd )
		{
			pTeamWnd->SetMaxTeamList( LadderTeamListWnd::PLAZA_MAX_TEAM_LIST );
			pTeamWnd->PlazaUIMove();
		}
	}

	g_GUIMgr.AddWnd( "XML/lobbycampinfownd.xml", this );
	PlazaListWnd* pPlaza = dynamic_cast<PlazaListWnd*>( g_GUIMgr.AddWnd( "XML/plazalistwnd.xml", this ) );
	if( pPlaza )
	{
		pPlaza->SetMaxPlazaList( PlazaListWnd::PLAZA_MAX_PLAZA_LIST );
		pPlaza->PlazaUIMove();
	}

	//거래소
	g_GUIMgr.AddWnd( "XML/trademainwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/quickLobbytradeinfownd.xml", this );

	//대회모드
	g_GUIMgr.AddWnd( "XML/tournamentmainwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentinfownd.xml", this );
}

void QuickLobbyWnd::iwm_show()
{
	PartyCatalogueWnd* pParty = dynamic_cast<PartyCatalogueWnd*>( FindChildWnd( BATTLE ) );
	if( pParty )
	{
		pParty->SetMaxPartyList( PartyCatalogueWnd::PLAZA_MAX_PARTY_LIST );
		pParty->PlazaUIMove();
	}

	PlazaListWnd* pPlaza = dynamic_cast<PlazaListWnd*>( FindChildWnd( PLAZA ) );
	if( pPlaza )
	{
		pPlaza->SetMaxPlazaList( PlazaListWnd::PLAZA_MAX_PLAZA_LIST );
		pPlaza->PlazaUIMove();
	}

	CampMainWnd* pCamp = dynamic_cast<CampMainWnd*>( FindChildWnd( CAMP ) );
	if( pCamp )
	{
		LadderTeamListWnd* pTeamWnd = dynamic_cast<LadderTeamListWnd*>( pCamp->FindChildWnd( CampMainWnd::ID_CAMP_TEAM_WND ) );
		if( pTeamWnd )
		{
			pTeamWnd->SetMaxTeamList( LadderTeamListWnd::PLAZA_MAX_TEAM_LIST );
			pTeamWnd->PlazaUIMove();
		}
	}
}

void QuickLobbyWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	QuickLobbyWndBase::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( m_dwCurrMenu != NONE )
	{
		//Hide시에 메뉴 인덱스 초기화됨으로 백업
		DWORD dwPrevMenu = m_dwCurrMenu;
		HideWnd();
		ShowMenu( dwPrevMenu );
	}
}

void QuickLobbyWnd::iwm_hide()
{
	ResetCurrMenu();

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast<ModeHelpTooltip*>( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}	

	CQuickModeToolTip *pQuickTooltip = dynamic_cast<CQuickModeToolTip*>( m_pQuickJoinTooltip );
	if( pQuickTooltip )
		pQuickTooltip->SetModeHelpTooltip( NULL, -1 );

	for( DWORD dwID = BATTLE; dwID <= TOURNAMENT; ++dwID )
	{
		ioWnd* pWnd = FindChildWnd( dwID );
		if( pWnd )
			pWnd->HideWnd();
	}
}

void QuickLobbyWnd::SetModeHelpTooltip( ioWnd *pCallWnd, const ioHashString &szTitle, int iQuickstartModeHelpIndex )
{
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
		pModeHelpTooltip->SetModeHelpTooltip(  pCallWnd, szTitle, iQuickstartModeHelpIndex );
}

void QuickLobbyWnd::SetQuickJoinTooltip( ioWnd *pCallWnd, const int nMode )
{
	if( m_pQuickJoinTooltip == NULL )
		m_pQuickJoinTooltip = dynamic_cast<CQuickModeToolTip*>( FindChildWnd( ID_QUICKJOIN_TOOLTIP ) );

	CQuickModeToolTip *pQuickTooltip = dynamic_cast<CQuickModeToolTip*>( m_pQuickJoinTooltip );
	if( pQuickTooltip )
		pQuickTooltip->SetModeHelpTooltip( pCallWnd, nMode );
}

bool QuickLobbyWnd::iwm_esc()
{
	HideWnd();
	return true;
}

void QuickLobbyWnd::ShowMenu( DWORD dwWndID )
{
	ShowWnd();

	//iwm_create에 등록된 윈도우 외에 다른 윈도우가 포함되있음으로 for문으로 show/hide
	for( DWORD dwID = BATTLE; dwID <= TOURNAMENT; ++dwID )
	{
		ioWnd* pWnd = FindChildWnd( dwID );
		if( pWnd )
		{
			if( dwID == dwWndID )
			{
				SeTitle( dwID );
				CalcSize( dwID );
				pWnd->SetWndPos( 17, 50 );
				pWnd->ShowWnd();
				m_dwCurrMenu = dwWndID;
				ShowInfo( dwID );
			}
			else
			{
				pWnd->HideWnd();
				HideInfo( dwID );
			}
		}
	}
}

void QuickLobbyWnd::ShowInfo( DWORD dwWndID, bool bReCalcPos )
{
	HideChildWnd( TOUR_INFO );
	HideChildWnd( CAMP_INFO );
	HideChildWnd( TRADE_INFO );

	ioWnd* pWnd = NULL;
	switch( dwWndID )
	{
	case CAMP:
		{
			pWnd = FindChildWnd( CAMP_INFO );
		}
		break;
	case TOURNAMENT:
		{
			pWnd = FindChildWnd( TOUR_INFO );
		}
		break;
	case TRADE:
		{
			QuickLobbyTradeInfoWnd* pTradeWnd = dynamic_cast<QuickLobbyTradeInfoWnd*>( FindChildWnd( TRADE_INFO ) );
			if( bReCalcPos )
			{
				pWnd = pTradeWnd;
			}
			else
			{
				//트레이드 윈도우는 자신이 등록한 상품이 있을 경우에만 info 윈도우를 show
				if( pTradeWnd )
					pTradeWnd->OnRefreshTradeItemList();
			}				
		}
		break;
	}

	if( pWnd )
	{
		pWnd->SetWndPos( 439, 0 );
		pWnd->ShowWnd();

		//윈도우 위치 재계산(Info 윈도우를 포함한 크기를 기준으로)
		Layout( 645, 526 );
	}
	else
	{
		//윈도우 위치 재계산(기본 윈도우 크기를 기준으로)
		Layout( GetDefaultWidth(), GetDefaultHeight() );
	}
}

void QuickLobbyWnd::HideInfo( DWORD dwWndID )
{
	switch( dwWndID )
	{
	case CAMP:
		{
			HideChildWnd( CAMP_INFO );
		}
		break;
	}
}

void QuickLobbyWnd::SeTitle( DWORD dwWndID )
{
	switch( dwWndID )
	{
	case BATTLE: 
		SetTitleText( "전투" );
		break;
	case CAMP:
		SetTitleText( "대회-예선대회" );
		break;
	case PLAZA: 
		SetTitleText( "광장" );
		break;
	case TRADE:
		SetTitleText( "거래소" );
		break;
	case TOURNAMENT:
		SetTitleText( "대회-토너먼트" );
		break;
	default:
		SetTitleText( "" );
	}
}

void QuickLobbyWnd::CalcSize( DWORD dwWndID )
{
	switch( dwWndID )
	{
	case BATTLE:
	case PLAZA:
		SetSize( 437, 416 );
		break;
	default:
		SetSize( m_iDefWidth, m_iDefHeight );
		break;
	}
}

void QuickLobbyWnd::OnCreateHeadquarters()
{
	if( g_App.IsMouseBusy() )
		return;

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투 중에는 훈련소 입장이 불가능합니다." );
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영팀 중에는 훈련소 입장이 불가능합니다." );
		return;
	}

	g_MyInfo.SetJoinHeadquartersRequestName( "" );
	SP2Packet kPacket( CTPK_JOIN_HEADQUARTERS );
	kPacket << g_MyInfo.GetPublicID() << g_ModeSelectInfo.CheckHeadquartersMapIndex( g_ModeSelectInfo.GetHeadquartersModeIndex(), 1 ) << false;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void QuickLobbyWnd::OnExitButtonDown( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	HideWnd();
}

void QuickLobbyWnd::Layout( int iWidth, int iHeight )
{
	if( !m_pParent )
		return;

	int parentx      = m_pParent->GetXPos();
	int parenty      = m_pParent->GetYPos();
	int parentwidth  = m_pParent->GetWidth();
	int parentheight = m_pParent->GetHeight();

	RECT rcRect = { 0, 0, 0, 0 };
	rcRect.left = m_iDefXPos;
		
	switch( m_XPosAlign )
	{
	case UI_LEFT_ALIGN:
		rcRect.left = parentx;
		break;
	case UI_CENTER_ALIGN:
		rcRect.left = parentwidth / 2 - iWidth / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.left = parentwidth * m_fScreenRateX;
		break;
	case UI_RIGHT_ALIGN:
		rcRect.left = parentx + parentwidth - iWidth;
		break;
	}

	rcRect.top = m_iDefYPos;
	switch( m_YPosAlign )
	{
	case UI_TOP_ALIGN:
		rcRect.top = parenty;
		break;
	case UI_CENTER_ALIGN:
		rcRect.top = parentheight / 2 - iHeight / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.top = parenty * m_fScreenRateY;
		break;
	case UI_BOTTOM_ALIGN:
		rcRect.top = parenty + parentheight - iHeight;
		break;
	}

	rcRect.right  = rcRect.left + GetWidth();
	rcRect.bottom = rcRect.top + GetHeight();

	m_rcPos	= rcRect;
}

void QuickLobbyWnd::ApplyPlazaList( SP2Packet &rkPacket )
{
	PlazaListWnd *pPlazaListWnd = dynamic_cast<PlazaListWnd*>( FindChildWnd( PLAZA ) );
	if( pPlazaListWnd && pPlazaListWnd->IsShow() )
		pPlazaListWnd->SetPlazaList( rkPacket );
}

void QuickLobbyWnd::ApplyTournamentList( SP2Packet &rkPacket )
{
	TournamentMainWnd *pTournamentWnd = dynamic_cast< TournamentMainWnd * >( FindChildWnd( TOURNAMENT ) );
	if( pTournamentWnd && pTournamentWnd->IsShow() )
		pTournamentWnd->ApplyTournamentList( rkPacket );
}

void QuickLobbyWnd::ApplyTournamentConfirmCheck( SP2Packet &rkPacket )
{
	TournamentMainWnd *pTournamentWnd = dynamic_cast< TournamentMainWnd * >( FindChildWnd( TOURNAMENT ) );
	if( pTournamentWnd && pTournamentWnd->IsShow() )
		pTournamentWnd->ApplyTournamentConfirmCheck( rkPacket );
}

void QuickLobbyWnd::ApplyTournamentConfirmRequest( SP2Packet &rkPacket )
{
	TournamentMainWnd *pTournamentWnd = dynamic_cast< TournamentMainWnd * >( FindChildWnd( TOURNAMENT ) );
	if( pTournamentWnd && pTournamentWnd->IsShow() )
		pTournamentWnd->ApplyTournamentConfirmRequest( rkPacket );
}

void QuickLobbyWnd::ApplyNormalTeamList( SP2Packet &rkPacket )
{
	PartyCatalogueWnd *pCatalogueWnd = dynamic_cast<PartyCatalogueWnd*>( FindChildWnd( BATTLE ) );
	if( pCatalogueWnd && pCatalogueWnd->IsShow() )
		pCatalogueWnd->SetPartyList( rkPacket );
}

void QuickLobbyWnd::RefreshBattleList( DWORD dwIndex )
{
	PartyCatalogueWnd *pCatalogueWnd = dynamic_cast<PartyCatalogueWnd*>( FindChildWnd( BATTLE ) );
	if( pCatalogueWnd && pCatalogueWnd->IsShow() )
	{
		if( dwIndex == 0 || pCatalogueWnd->IsIndexToList( dwIndex ) )
			pCatalogueWnd->StartSend( false );
	}
}

void QuickLobbyWnd::ApplyLadderTeamList( SP2Packet &rkPacket )
{
	CampMainWnd *pCampMainWnd = dynamic_cast<CampMainWnd*>( FindChildWnd( CAMP ) );
	if( pCampMainWnd && pCampMainWnd->IsShow() )
		pCampMainWnd->SetLadderTeamList( rkPacket );
}

void QuickLobbyWnd::RefreshTradeItemList()
{
	TradeMainWnd *pTradeWnd = dynamic_cast<TradeMainWnd*>( FindChildWnd( TRADE ) );
	if( pTradeWnd )
		pTradeWnd->OnRefreshTradeItemList();

	QuickLobbyTradeInfoWnd *pTradeInfoWnd = dynamic_cast<QuickLobbyTradeInfoWnd*>( FindChildWnd( TRADE_INFO ) );
	if( pTradeInfoWnd )
		pTradeInfoWnd->OnRefreshTradeItemList();
}

void QuickLobbyWnd::SetTournamentLink( DWORD dwTourIndex )
{
	TournamentMainWnd *pTournamentWnd = dynamic_cast< TournamentMainWnd * >( FindChildWnd( TOURNAMENT ) );
	if( pTournamentWnd )
	{
		pTournamentWnd->SetTournamentLink( dwTourIndex );
	}
}

bool QuickLobbyWnd::QuestGuide( int iModeSelectIndex, float &rfXPos, float &rfYPos )
{
	PartyCatalogueWnd *pCatalogueWnd = dynamic_cast<PartyCatalogueWnd*>( FindChildWnd( BATTLE ) );
	if( pCatalogueWnd && pCatalogueWnd->IsShow() )
	{
		return pCatalogueWnd->QuestGuide( iModeSelectIndex, rfXPos, rfYPos );
	}

	return false;
}

void QuickLobbyWnd::RefreshCampInfo( int iPrevMyCampPos )
{
	CampMainWnd *pCampMainWnd = dynamic_cast<CampMainWnd*>( FindChildWnd( CAMP ) );
	if( pCampMainWnd )
	{
		ShowMenu( CAMP );

		// 가입했을 때는 진영 정보를 보여준다.
		if( iPrevMyCampPos == CAMP_NONE )
			pCampMainWnd->CheckCampButton( CampMainWnd::ID_CAMP_INFO_BTN );
	}
}

//----------------------------------------------------------------------------------------------------------

QuickLobbyTradeInfoWnd::QuickLobbyTradeInfoWnd()
{
	m_pInfoWnd = NULL;
}

QuickLobbyTradeInfoWnd::~QuickLobbyTradeInfoWnd()
{
	m_pInfoWnd = NULL;
}

void QuickLobbyTradeInfoWnd::iwm_create()
{
	m_pInfoWnd = dynamic_cast<LobbyOwnerTradeListWnd*>( g_GUIMgr.AddWnd( "XML/LobbyOwnerTradeListWnd.xml", this ) );
	if( m_pInfoWnd )
	{
		m_pInfoWnd->SetMaxTradeList( LobbyOwnerTradeListWnd::OWNER_MAX_TRADE );
		m_pInfoWnd->PlazaUIMove();
	}
}

void QuickLobbyTradeInfoWnd::iwm_hide()
{
	if( m_pInfoWnd )
		m_pInfoWnd->HideWnd();
}

void QuickLobbyTradeInfoWnd::iwm_show()
{
	if( m_pInfoWnd )
		m_pInfoWnd->ShowWnd();
}

void QuickLobbyTradeInfoWnd::OnRefreshTradeItemList()
{
	if( m_pInfoWnd )
		m_pInfoWnd->OnRefreshTradeItemList();
}
