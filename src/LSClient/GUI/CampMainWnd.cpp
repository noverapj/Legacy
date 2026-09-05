

#include "StdAfx.h"

#include "../ioLadderTeamMgr.h"
#include "../ioTournamentManager.h"
#include "../ioDateHelp.h"

#include "LobbyMainWnd.h"
#include "GuildWnd.h"
#include "LadderTeamWnd.h"
#include "HeroMainWnd.h"
#include "CampMainWnd.h"
#include "QuickLobbyMenu.h"

CampMyInfoBtn::CampMyInfoBtn()
{
	m_szCampName.c_str();
}

CampMyInfoBtn::~CampMyInfoBtn()
{
}

void CampMyInfoBtn::SetCampName( ioHashString szCampName )
{
	m_szCampName = szCampName;
}

void CampMyInfoBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	switch( GetID() )
	{
	case LobbyCampInfoWnd::ID_MY_CAMP_BTN:
		OnCampRender( iXPos, iYPos );
		break;
	case LobbyCampInfoWnd::ID_MY_GUILD_BTN:
		OnGuildRender( iXPos, iYPos );
		break;
	case LobbyCampInfoWnd::ID_TOURNAMENT_POINT_BTN:
		OnTournamentPointRender( iXPos, iYPos );
		break;
	case LobbyCampInfoWnd::ID_MY_CAMP_RANK_BTN:
		OnCampRankRender( iXPos, iYPos );
		break;
	}
}

void CampMyInfoBtn::OnCampRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	
	switch( g_MyInfo.GetUserCampPos() )
	{
	case CAMP_BLUE:
		g_FontMgr.SetBkColor( 22, 70, 187 );
		g_FontMgr.SetTextColor( 22, 70, 187 );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, m_szCampName.c_str() );
		break;
	case CAMP_RED:
		g_FontMgr.SetBkColor( TCT_DEFAULT_RED );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, m_szCampName.c_str() );
		break;
	}
}

void CampMyInfoBtn::OnGuildRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );

	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( !pGuildData  )
	{
		g_FontMgr.SetBkColor( TCT_DEFAULT_GRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, STR(1) );
	}
	else
	{
		g_FontMgr.SetBkColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, STR(2), pGuildData->GetGuildName().c_str() );
	}
}

void CampMyInfoBtn::OnTournamentPointRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	
	if( g_MyInfo.GetLadderPoint() == 0 )
	{
		g_FontMgr.SetBkColor( TCT_DEFAULT_GRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, "-" );
	}
	else
	{
		g_FontMgr.SetBkColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		char szConvertText[MAX_PATH] = "";
		Help::ConvertNumToStrComma( g_MyInfo.GetLadderPoint(), szConvertText, sizeof( szConvertText ) );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, "%s 점", szConvertText );
	}
}

void CampMyInfoBtn::OnCampRankRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );

	if( g_MyInfo.GetCampRank() <= 0 )
	{
		g_FontMgr.SetBkColor( TCT_DEFAULT_GRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, "-" );
	}
	else
	{
		g_FontMgr.SetBkColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		char szConvertText[MAX_PATH] = "";
		Help::ConvertNumToStrComma( g_MyInfo.GetCampRank(), szConvertText, sizeof( szConvertText ) );
		g_FontMgr.PrintText( iXPos + 166, iYPos + 3, FONT_SIZE_12, "%s 위", szConvertText );
	}
}
//////////////////////////////////////////////////////////////////////////
LobbyCampInfoWnd::LobbyCampInfoWnd()
{
	m_pNoneCamp = NULL;
	m_pCampIcon = NULL;
	m_pCampBack = NULL;

	m_pCloseBox	= NULL;
	m_pOpenBox	= NULL;
	m_pTodayBox = NULL;

	m_dwMsgBoxID = 0;
	m_dwSelectIndex = 0;

	m_szTournamentTitle.Clear();
	m_szCampName.Clear();	
	m_szStartWeekDayOfWeek.Clear();
	m_szEndWeekDayOfWeek.Clear();
	
	m_StateStartYear = 0;
	m_StateStartMonth = 0;
	m_StateStartDay = 0;

	m_StateEndMonth = 0;
	m_StateEndMonth = 0;
	m_StateEndDay = 0;
	m_StateEndHour = 0;

	m_bScreen = false;
	m_dwScreenTime = 0;
}

LobbyCampInfoWnd::~LobbyCampInfoWnd()
{
	SAFEDELETE( m_pNoneCamp );
	SAFEDELETE( m_pCampBack );

	SAFEDELETE( m_pCloseBox );
	SAFEDELETE( m_pOpenBox );
	SAFEDELETE( m_pTodayBox );
}

void LobbyCampInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NoneCamp" )
	{
		SAFEDELETE( m_pNoneCamp );
		m_pNoneCamp = pImage;
	}
	else if( szType == "CampBack" )
	{
		SAFEDELETE( m_pCampBack );
		m_pCampBack = pImage;
	}
	else if( szType == "CloseBox" )
	{
		SAFEDELETE( m_pCloseBox );
		m_pCloseBox = pImage;
	}
	else if( szType == "OpenBox" )
	{
		SAFEDELETE( m_pOpenBox );
		m_pOpenBox = pImage;
	}
	else if( szType == "TodayBox" )
	{
		SAFEDELETE( m_pTodayBox );
		m_pTodayBox = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void LobbyCampInfoWnd::iwm_show()
{
	switch( g_MyInfo.GetUserCampPos() )
	{
	case CAMP_BLUE:
		{
			ShowChildWnd( ID_LEAVE_CAMP_BTN );
			ShowChildWnd( ID_CAMP_TOOLTIP_BTN );
			ShowChildWnd( ID_CREATE_GUILD_BTN );
			ShowChildWnd( ID_MY_CAMP_BTN );
			ShowChildWnd( ID_MY_GUILD_BTN );
			ShowChildWnd( ID_TOURNAMENT_POINT_BTN );
			ShowChildWnd( ID_MY_CAMP_RANK_BTN );
		}
		break;
	case CAMP_RED:
		{
			ShowChildWnd( ID_LEAVE_CAMP_BTN );
			ShowChildWnd( ID_CAMP_TOOLTIP_BTN );
			ShowChildWnd( ID_CREATE_GUILD_BTN );
			ShowChildWnd( ID_MY_CAMP_BTN );
			ShowChildWnd( ID_MY_GUILD_BTN );
			ShowChildWnd( ID_TOURNAMENT_POINT_BTN );
			ShowChildWnd( ID_MY_CAMP_RANK_BTN );
		}
		break;
	default:
		HideChildWnd( ID_LEAVE_CAMP_BTN );
		HideChildWnd( ID_CAMP_TOOLTIP_BTN );
		HideChildWnd( ID_CREATE_GUILD_BTN );
		HideChildWnd( ID_MY_CAMP_BTN );
		HideChildWnd( ID_MY_GUILD_BTN );
		HideChildWnd( ID_TOURNAMENT_POINT_BTN );		
		HideChildWnd( ID_MY_CAMP_RANK_BTN );
		break;
	}

	m_dwSelectIndex = g_TournamentMgr.GetRegularIndex();

	UpdateMainUI();
	UpdateGuildButton();

	m_bScreen = false;
	m_dwScreenTime = FRAMEGETTIME();	
}

void LobbyCampInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_LEAVE_CAMP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DWORD dwIndex = g_TournamentMgr.GetRegularIndex();
			ioUserTournament *pUserTournament = g_MyInfo.GetTournament();			
			if( pUserTournament->IsAlreadyTeam( pUserTournament->GetTeamIndex( dwIndex ) ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회팀이 존재 할떈 진영을 탈퇴 할 수 없습니다." );
			}
			else
			{
				if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4) );
					m_dwMsgBoxID = MB_LEAVE_CAMP;
				}
			}
		}
		break;
	case ID_CAMP_TOOLTIP_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( g_CampMgr.GetCampBonusHelpID() );
			}
		}
		break;
	case ID_CREATE_GUILD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.GetGuildIndex() != 0 )
			{
				GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
				if( pGuildMainWnd )
					pGuildMainWnd->SetGuildIndex( g_GuildInfo.GetGuildIndex() );
			}			
			else
			{
				GuildHeroRankingListWnd *pHeroList = dynamic_cast<GuildHeroRankingListWnd*>(g_GUIMgr.FindWnd( GUILD_HERO_RANKING_LIST_WND ));
				if( pHeroList )				
				{
					if( pHeroList->IsShow() )
						pHeroList->HideWnd();
					else
					{
						pHeroList->SetRankRadioBtn( GuildHeroRankingListWnd::ID_GUILD_BTN );
						pHeroList->ShowWnd();
					}
				}
			}
		}
		break;
	case MESSAGE_BOX:
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_dwMsgBoxID == MB_OPEN_GUILD_INFO )
				{
					iwm_command( FindChildWnd( ID_MY_GUILD_BTN ), IOBN_BTNUP, 0 );
				}
				else if( m_dwMsgBoxID == MB_LEAVE_CAMP )
				{
					if( g_LadderTeamMgr.IsLadderTeam() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
					}
					else 
					{
						TCPNetwork::MouseBusy( true );
						SP2Packet kPacket( CTPK_CAMP_CAHNGE_POS );
						kPacket << 0 << g_GuildInfo.IsGuildEntryDelayListDelete() << false;     // false = Result UI Hide
						TCPNetwork::SendToServer( kPacket );

						g_GUIMgr.HideWnd( LADDER_TEAM_INVITED_WND );
						g_GUIMgr.HideWnd( LADDER_TEAM_INFO_WND );
					}
				}
			}				
		}
		break;
	}
}

void LobbyCampInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
		
	UpdateMainUI();
	UpdateGauge();
	UpdateGuildButton();
}

void LobbyCampInfoWnd::UpdateMainUI()
{		
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament == NULL ) return;	

	m_pCampIcon = pTournament->GetResCampIcon( g_MyInfo.GetUserCampPos() );
	m_szTournamentTitle = pTournament->GetTournamentTitle();
	m_szCampName = pTournament->GetCampName( g_MyInfo.GetUserCampPos() );
		
	m_StateStartYear		= g_CampMgr.GetStartSeasonYear();
	m_StateStartMonth		= g_CampMgr.GetStartSeasonMonth();
	m_StateStartDay			= g_CampMgr.GetStartSeasonDay();

	m_StateEndYear			= g_CampMgr.GetNextSeasonYear();
	m_StateEndMonth			= g_CampMgr.GetNextSeasonMonth();
	m_StateEndDay			= g_CampMgr.GetNextSeasonDay();
	m_StateEndHour			= g_CampMgr.GetNextSeasonHour();

	m_szStartWeekDayOfWeek	= DateHelp::GetWeekDayString( g_CampMgr.GetStartSeasonWeekDay() ); 
	m_szEndWeekDayOfWeek	= DateHelp::GetWeekDayString( g_CampMgr.GetNextSeasonWeekDay() ); 
	
	CampMyInfoBtn* pCampButton = dynamic_cast<CampMyInfoBtn*>( FindChildWnd( ID_MY_CAMP_BTN ) );
	if( pCampButton )
		pCampButton->SetCampName( m_szCampName );
}

void LobbyCampInfoWnd::UpdateGauge()
{
	if( m_bScreen )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwScreenTime;
		if( dwGapTime < 500 )
		{
			float fRate = (float)dwGapTime / 500;
			m_pTodayBox->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
		}
		else
		{
			m_dwScreenTime = FRAMEGETTIME();
			m_bScreen = false;
			m_pTodayBox->SetAlpha( MAX_ALPHA_RATE );
		}
	}
	else
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwScreenTime;
		if( dwGapTime < 500 )
		{
			float fRate = FLOAT1 - ( (float)dwGapTime / 500 );
			m_pTodayBox->SetAlpha( (float)MAX_ALPHA_RATE * fRate );	
		}
		else
		{
			m_dwScreenTime = FRAMEGETTIME();
			m_bScreen = true;
			m_pTodayBox->SetAlpha( 0 );
		}
	}
}

void LobbyCampInfoWnd::UpdateGuildButton()
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_CREATE_GUILD_BTN ) );
	if( pButton )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( pGuildData )
		{
			pButton->SetTitleText( pGuildData->GetGuildName().c_str() );
		}
		else
		{
			pButton->SetTitleText( "길드 가입 및 생성" );
		}
	}
}

void LobbyCampInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( g_MyInfo.GetUserCampPos() )
	{
	case CAMP_NONE:
		OnRenderNoneCamp( iXPos, iYPos );
		break;
	default:
		OnRenderCampInfo( iXPos, iYPos );
		break;
	}
}

void LobbyCampInfoWnd::OnRenderNoneCamp( int iXPos, int iYPos )
{
	if( m_pNoneCamp )
		m_pNoneCamp->Render( iXPos + 58, iYPos + 195, UI_RENDER_MULTIPLY );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( TCT_DEFAULT_GRAY );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 299, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 317, FONT_SIZE_12, STR(2) );
}

void LobbyCampInfoWnd::OnRenderCampInfo( int iXPos, int iYPos )
{
	if( m_pCampBack )
	{
		m_pCampBack->Render( iXPos + 103, iYPos + 296, UI_RENDER_MULTIPLY );
	}

	if( m_pCampIcon )
	{
		m_pCampIcon->SetScale( FLOAT1, FLOAT1 );
		m_pCampIcon->Render( iXPos + 103, iYPos + 296  );
	}
	
	time_t tToday;	
	time( &tToday );
	tm* pToday = localtime( &tToday );
	
	time_t tStart	= DateHelp::ConvertSecondTime( m_StateStartYear, m_StateStartMonth, m_StateStartDay, 0, 0, 0 );
	time_t tEnd		= DateHelp::ConvertSecondTime( m_StateEndYear, m_StateEndMonth, m_StateEndDay, m_StateEndHour, 0, 0 );

	int iCampPeriod = abs(difftime( tStart, tEnd )) / ( 60 * 60 * 24 );
	int iProgressPeriod = (abs(difftime( tStart, tToday )) / ( 60 * 60 * 24 ));

	int iCurYear  = m_StateStartYear;
	int iCurMonth = m_StateStartMonth;
	int iCurtDay   = m_StateStartDay;
	int iXOffset = 0;
	int iYOffset = 0;

	for( int i = 0; i <= iCampPeriod; ++i )
	{
		if( i != 0 && i % 7 == 0 )
		{
			iXOffset = 0;
			iYOffset += 24;
		}
		
		bool bDateRender = true;
		if( i < iProgressPeriod )
		{
			if( m_pCloseBox )
				m_pCloseBox->Render( iXPos + 20 + iXOffset, iYPos + 124 + iYOffset );

			bDateRender = false;
		}
		else if( i == iProgressPeriod )
		{
			if( m_pOpenBox )
				m_pOpenBox->Render( iXPos + 20 + iXOffset, iYPos + 124 + iYOffset );

			if( m_pTodayBox )
				m_pTodayBox->Render( iXPos + 20 + iXOffset, iYPos + 124 + iYOffset );			
		}
		else
		{
			if( m_pOpenBox )
				m_pOpenBox->Render( iXPos + 20 + iXOffset, iYPos + 124 + iYOffset );
		}

		if( bDateRender )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_12, "%d", iCurtDay );
			kPrinter.PrintFullText( iXPos + 31 + iXOffset, iYPos + 128 + iYOffset, TAT_CENTER );			
		}

		DateHelp::IncreaseDay( iCurYear, iCurMonth, iCurtDay );
		iXOffset += 24;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, "%s 진영전", m_szTournamentTitle );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 62, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d/%d(%s)", m_StateStartMonth, m_StateStartDay, m_szStartWeekDayOfWeek.c_str() );
	kPrinter.AddTextPiece( FONT_SIZE_12, " ~ ", m_szTournamentTitle );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d/%d(%s) %d:00", m_StateEndMonth, m_StateEndDay, m_szEndWeekDayOfWeek.c_str(), m_StateEndHour );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 87, TAT_CENTER );
	kPrinter.ClearList();

	int iStartTick = 3;	
	if( COMPARE( g_CampMgr.GetTodayStartCampTime(), 6, 12 ) )			iStartTick = 0;
	else if( COMPARE( g_CampMgr.GetTodayStartCampTime(), 12, 18 ) )		iStartTick = 1;
	else if( COMPARE( g_CampMgr.GetTodayStartCampTime(), 18, 24 + 1 ) ) iStartTick = 2;

	int iEndTick = 3;
	if( COMPARE( g_CampMgr.GetTodayEndCampTime(), 6, 12 ) )				iEndTick = 0;
	else if( COMPARE( g_CampMgr.GetTodayEndCampTime(), 12, 18 ) )		iEndTick = 1;
	else if( COMPARE( g_CampMgr.GetTodayEndCampTime(), 18, 24 + 1 ) )	iEndTick = 2;

	//6시  ~ 12 ~ 18 ~  24
	char szTick[4][MAX_PATH];
	SafeSprintf( szTick[0], sizeof( szTick[0] ), "아침" );
	SafeSprintf( szTick[1], sizeof( szTick[1] ), "낮" );
	SafeSprintf( szTick[2], sizeof( szTick[2] ), "밤" );
	SafeSprintf( szTick[3], sizeof( szTick[3] ), "새벽" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "매일 %s %d시 ~ %s %d시", szTick[iStartTick], g_CampMgr.GetTodayStartCampTime(), szTick[iEndTick], g_CampMgr.GetTodayEndCampTime() );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 103, TAT_CENTER );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
CampMainWnd::CampMainWnd()
{
	m_dwTabID = ID_CAMP_INFO_BTN;

	m_iMyCampPos = 0;
}

CampMainWnd::~CampMainWnd()
{	
}

void CampMainWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/ladderteamlistwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/campenterwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/influencewnd.xml", this );
	g_GUIMgr.AddWnd( "XML/heromatchwnd.xml", this );
}

void CampMainWnd::iwm_show()
{	
	g_CampMgr.SyncCampData();
	CheckCampButton( ID_CAMP_INFO_BTN );
}

void CampMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_CAMP_INFO_BTN:
	case ID_CAMP_TEAM_BTN:
	case ID_HERO_MATCH_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckCampButton( dwID );
		}
		break;
	}
}

void CampMainWnd::CheckCampButton( DWORD dwID )
{
	m_dwTabID = dwID;
	CheckRadioButton( ID_CAMP_INFO_BTN, ID_HERO_MATCH_BTN, m_dwTabID );

	HideChildWnd( ID_CAMP_TEAM_WND );
	HideChildWnd( ID_CAMP_INFO_WND );
	HideChildWnd( ID_HERO_MATCH_WND );
	HideChildWnd( ID_CAMP_ENTER_WND );

	switch( m_dwTabID )
	{
	case ID_CAMP_INFO_BTN:
		{
			switch( g_MyInfo.GetUserCampPos() )
			{
			case CAMP_NONE:
				{
					ShowChildWnd( ID_CAMP_ENTER_WND );
				}				
				break;
			default:
				{
					ShowChildWnd( ID_CAMP_INFO_WND );
				}
				break;
			}
			
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
				{
					pLobbyMainWnd->ShowChatWnd( false );
				}				
			}
			else
			{
				//광장에서는 진영전 내부 윈도우에 채팅 없음
			}
		}
		break;
	case ID_CAMP_TEAM_BTN:
		{
			ShowChildWnd( ID_CAMP_TEAM_WND );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
				{
					pLobbyMainWnd->ShowChatWnd( true );
				}
			}
			else
			{
				//광장에서는 진영전 내부 윈도우에 채팅 없음
			}
		}
		break;
	case ID_HERO_MATCH_BTN:
		{
			ShowChildWnd( ID_HERO_MATCH_WND );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
				{
					pLobbyMainWnd->ShowChatWnd( true );
				}
			}
			else
			{
				//광장에서는 진영전 내부 윈도우에 채팅 없음
			}
		}
		break;
	}
}

void CampMainWnd::SetLadderTeamList( SP2Packet &rkPacket )
{
	LadderTeamListWnd *pLadderTeamList = dynamic_cast<LadderTeamListWnd*>(FindChildWnd(ID_CAMP_TEAM_WND));
	if( pLadderTeamList && pLadderTeamList->IsShow() )
		pLadderTeamList->ApplyLadderTeamList( rkPacket );
}

void CampMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	g_TournamentMgr.CheckRegularTournaemntRequest();
}
//////////////////////////////////////////////////////////////////////////
InfluenceBtn::InfluenceBtn()
{
	m_dwRank = 0;
}

InfluenceBtn::~InfluenceBtn()
{
}

void InfluenceBtn::SetRank( DWORD dwRank )
{
	m_dwRank = dwRank;
}

void InfluenceBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_dwRank == 0 )
		kPrinter.AddTextPiece( FONT_SIZE_12, "- 위" );
	else
		kPrinter.AddTextPiece( FONT_SIZE_12, "%d 위", m_dwRank );

	kPrinter.PrintFullText( iXPos + 95, iYPos + 25, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
InfluenceWnd::InfluenceWnd()
{
	m_pMainBannerFrm = NULL;

	m_pBanner	= NULL;
	m_pHorzLine = NULL;
	m_pTextBack = NULL;
	
	m_pGagueBack		= NULL;
	m_pGagueBlue		= NULL;
	m_pGagueBlueAlpha	= NULL;
	m_pGagueRed			= NULL;
	m_pGagueRedAlpha	= NULL;

	m_szBlueCampName.Clear();
	m_szRedCampName.Clear();

	m_bScreen = false;
	m_dwScreenTime = 0;

	m_dwBlueUserCnt = 0;
	m_dwRedUserCnt = 0;
}

InfluenceWnd::~InfluenceWnd()
{
	SAFEDELETE( m_pMainBannerFrm );
	
	SAFEDELETE( m_pHorzLine );
	SAFEDELETE( m_pTextBack );

	SAFEDELETE( m_pGagueBack );
	SAFEDELETE( m_pGagueBlue );
	SAFEDELETE( m_pGagueBlueAlpha );
	SAFEDELETE( m_pGagueRed );
	SAFEDELETE( m_pGagueRedAlpha );
}

void InfluenceWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HorzLine" )
	{
		SAFEDELETE( m_pHorzLine );
		m_pHorzLine = pImage;
	}
	else if( szType == "TextBack" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else if( szType == "GagueBack" )
	{
		SAFEDELETE( m_pGagueBack );
		m_pGagueBack = pImage;
	}
	else if( szType == "GagueBlue" )
	{
		SAFEDELETE( m_pGagueBlue );
		m_pGagueBlue = pImage;
	}
	else if( szType == "GagueBlueAlpha" )
	{
		SAFEDELETE( m_pGagueBlueAlpha );
		m_pGagueBlueAlpha = pImage;
	}
	else if( szType == "GagueRed" )
	{
		SAFEDELETE( m_pGagueRed );
		m_pGagueRed = pImage;
	}
	else if( szType == "GagueRedAlpha" )
	{
		SAFEDELETE( m_pGagueRedAlpha );
		m_pGagueRedAlpha = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void InfluenceWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MainBannerFrm" )
	{
		SAFEDELETE( m_pMainBannerFrm );
		m_pMainBannerFrm = pFrame;
	}
	else 
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void InfluenceWnd::iwm_show()
{	
	g_CampMgr.SyncCampData();

	m_dwSelectIndex = g_TournamentMgr.GetRegularIndex();
	UpdateMainUI();

	m_bScreen = false;
	m_dwScreenTime = FRAMEGETTIME();
	
	DWORD dwRank = 0;
	ioGuildData *pGuild = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuild )
		dwRank = pGuild->GetGuildRank();


	InfluenceBtn* pButton = dynamic_cast< InfluenceBtn* >( FindChildWnd( ID_GUILD_RANK_BTN ) );
	if( pButton )
		pButton->SetRank( dwRank );

	dwRank = g_MyInfo.GetHeroRank();
	
	pButton = dynamic_cast< InfluenceBtn* >( FindChildWnd( ID_LADDER_RANK_BTN ) );
	if( pButton )
		pButton->SetRank( dwRank );	
}

void InfluenceWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GUILD_RANK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			GuildHeroRankingListWnd *pHeroList = dynamic_cast<GuildHeroRankingListWnd*>(g_GUIMgr.FindWnd( GUILD_HERO_RANKING_LIST_WND ));
			if( pHeroList )				
			{
				if( pHeroList->IsShow() )
					pHeroList->HideWnd();
				else
				{
					pHeroList->SetRankRadioBtn( GuildHeroRankingListWnd::ID_GUILD_BTN );
					pHeroList->ShowWnd();
				}
			}
		}
		break;
	case ID_CAMP_RANK_BTN:
		break;
	case ID_LADDER_RANK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			GuildHeroRankingListWnd *pHeroList = dynamic_cast<GuildHeroRankingListWnd*>(g_GUIMgr.FindWnd( GUILD_HERO_RANKING_LIST_WND ));
			if( pHeroList )				
			{
				if( pHeroList->IsShow() )
					pHeroList->HideWnd();
				else
				{
					pHeroList->SetRankRadioBtn( GuildHeroRankingListWnd::ID_HERO_TOP100_BTN );
					pHeroList->ShowWnd();
				}
			}
		}
		break;
	}
}

void InfluenceWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnUpdateProcess();
	OnUpdateGauge();
}

void InfluenceWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament )
	{
		if( pTournament->IsUpdateSubUI() )
		{
			UpdateMainUI();
		}
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsUpdateMainUI() )
		{
			UpdateMainUI();
		}
	}
}

void InfluenceWnd::OnUpdateGauge()
{
	if( m_bScreen )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwScreenTime;
		if( dwGapTime < 500 )
		{
			float fRate = (float)dwGapTime / 500;
			if( m_pGagueBlueAlpha )
				m_pGagueBlueAlpha->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
			if( m_pGagueRedAlpha )
				m_pGagueRedAlpha->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
		}
		else
		{
			m_dwScreenTime = FRAMEGETTIME();
			m_bScreen = false;

			if( m_pGagueBlueAlpha )
				m_pGagueBlueAlpha->SetAlpha( MAX_ALPHA_RATE );
			if( m_pGagueRedAlpha )
				m_pGagueRedAlpha->SetAlpha( MAX_ALPHA_RATE );
		}
	}
	else
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwScreenTime;
		if( dwGapTime < 500 )
		{
			float fRate = FLOAT1 - ( (float)dwGapTime / 500 );
			if( m_pGagueBlueAlpha )
				m_pGagueBlueAlpha->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
			if( m_pGagueRedAlpha )
				m_pGagueRedAlpha->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
		}
		else
		{
			m_dwScreenTime = FRAMEGETTIME();
			m_bScreen = true;

			if( m_pGagueBlueAlpha )
				m_pGagueBlueAlpha->SetAlpha( 0 );
			if( m_pGagueRedAlpha )
				m_pGagueRedAlpha->SetAlpha( 0 );
		}
	}
}

void InfluenceWnd::UpdateMainUI()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament == NULL ) return;

	m_pBanner = pTournament->GetResCampBanner();

	m_szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
	m_szRedCampName = pTournament->GetCampName( CAMP_RED );	

	m_dwBlueUserCnt = g_CampMgr.GetCampEntryUserCount( CAMP_BLUE ) + g_CampMgr.GetCampSpecialEntryUserCount(CAMP_BLUE);
	m_dwRedUserCnt = g_CampMgr.GetCampEntryUserCount( CAMP_RED ) + g_CampMgr.GetCampSpecialEntryUserCount(CAMP_RED);
}

void InfluenceWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnMainBannerRender( iXPos, iYPos );
	OnExplainRender( iXPos, iYPos );
	OnScheduleRender( iXPos, iYPos );

	if( m_pMainBannerFrm )
	{
		m_pMainBannerFrm->Render( iXPos + 10, iYPos + 7 );
	}
}

void InfluenceWnd::OnMainBannerRender( int iXPos, int iYPos )
{
	if( m_pBanner )
	{
		m_pBanner->Render( iXPos + 13, iYPos + 10 );
	}

	if( m_pHorzLine )
	{
		m_pHorzLine->Render( iXPos + 13, iYPos + 280 );
	}
}

void InfluenceWnd::OnExplainRender( int iXPos, int iYPos )
{
	if( m_pTextBack )
	{
		m_pTextBack->Render( iXPos + 13, iYPos + 281 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "위쪽 탭의 " );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( D3DCOLOR_XRGB( 0, 155, 255 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "진영전과 래더전" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "참여를 통해 대회포인트를 획득하시면" );
	kPrinter.PrintFullText( iXPos + 201, iYPos + 293, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( D3DCOLOR_XRGB( 0, 180, 0 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "진영전 종료시 보상" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "을 드립니다." );
	kPrinter.PrintFullText( iXPos + 201, iYPos + 309, TAT_CENTER );
	kPrinter.ClearList();
}

void InfluenceWnd::OnScheduleRender( int iXPos, int iYPos )
{
	if( m_pGagueBack )
		m_pGagueBack->Render( iXPos + 18, iYPos + 332 );

	int  iBlueUserCnt = m_dwBlueUserCnt;
	int  iRedUserCnt  = m_dwRedUserCnt;
	int  iTotalCnt= iBlueUserCnt + iRedUserCnt;
	float fGaugeRate  = 0.25f;
	
	if( iTotalCnt > 0 )
	{
		float fRate = 0.50f * ( (float)iBlueUserCnt / iTotalCnt );		
		fGaugeRate += fRate;
	}
	else
	{
		fGaugeRate = 0.50f;
	}

	if( m_pGagueBlue )
	{
		m_pGagueBlue->RenderWidthCut( iXPos + 18, iYPos + 332, 0, m_pGagueBlue->GetWidth() * fGaugeRate );
		if( g_MyInfo.GetUserCampPos() == CAMP_BLUE && m_pGagueBlueAlpha )
		{
			m_pGagueBlueAlpha->RenderWidthCut( iXPos + 18, iYPos + 332, 0, m_pGagueBlueAlpha->GetWidth() * fGaugeRate );
		}
	}

	if( m_pGagueRed )
	{
		m_pGagueRed->RenderWidthCut( iXPos + 18, iYPos + 332, m_pGagueRed->GetWidth() * fGaugeRate, m_pGagueRed->GetWidth() );
		if( g_MyInfo.GetUserCampPos() == CAMP_RED && m_pGagueRedAlpha )
			m_pGagueRedAlpha->RenderWidthCut( iXPos + 18, iYPos + 332, m_pGagueRedAlpha->GetWidth() * fGaugeRate, m_pGagueRedAlpha->GetWidth() );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%s ", m_szBlueCampName.c_str() );

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_dwBlueUserCnt, szConvertNum, sizeof( szConvertNum ) );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%s 참가", szConvertNum );
	
	kPrinter.PrintFullText( iXPos + 111, iYPos + 336, TAT_CENTER );
	kPrinter.ClearList();
	
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%s ", m_szRedCampName.c_str() );
	
	Help::ConvertNumToStrComma( m_dwRedUserCnt, szConvertNum, sizeof( szConvertNum ) );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%s 참가", szConvertNum );
	kPrinter.PrintFullText( iXPos + 291, iYPos + 336, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
CampEnterResultWnd::CampEnterResultWnd()
{
	m_pCampMarkBack = NULL;
	m_pBlueCampMark = NULL;
	m_pRedCampMark  = NULL;
}

CampEnterResultWnd::~CampEnterResultWnd()
{
	SAFEDELETE( m_pCampMarkBack );
	SAFEDELETE( m_pBlueCampMark );
	SAFEDELETE( m_pRedCampMark );
}

void CampEnterResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CampMarkBack" )
	{
		SAFEDELETE( m_pCampMarkBack );
		m_pCampMarkBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CampEnterResultWnd::iwm_show()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament )
	{
		m_pBlueCampMark	= pTournament->GetCampIcon( CAMP_BLUE );
		m_pRedCampMark	= pTournament->GetCampIcon( CAMP_RED );

		m_szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
		m_szRedCampName  = pTournament->GetCampName( CAMP_RED );
	}
}

void CampEnterResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CAMP:	
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( g_CampMgr.GetCampHelpID() );

			HideWnd();
		}			
		break;		
	}
}

void CampEnterResultWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pCampMarkBack )
		m_pCampMarkBack->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	switch( g_MyInfo.GetUserCampPos() )
	{
	case CAMP_BLUE:
		{
			if( m_pBlueCampMark )
			{
				m_pBlueCampMark->SetScale( FLOAT1, FLOAT1 );
				m_pBlueCampMark->Render( iXPos + 103, iYPos + 89 );
			}

			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 135, FONT_SIZE_13, m_szBlueCampName.c_str() );
		}
		break;
	case CAMP_RED:
		{
			if( m_pRedCampMark )
			{
				m_pRedCampMark->SetScale( FLOAT1, FLOAT1 );
				m_pRedCampMark->Render( iXPos + 103, iYPos + 89 );
			}

			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos + 103, iYPos + 135, FONT_SIZE_13, m_szRedCampName.c_str() );
		}
		break;
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, STR(3) );
}
//////////////////////////////////////////////////////////////////////////
CampEnterAlarmWnd::CampEnterAlarmWnd()
{
	m_pBlueCampMark = NULL;
	m_pRedCampMark	= NULL;
	m_pCampBack		= NULL;
	
	m_eCampType     = CAMP_NONE;
}

CampEnterAlarmWnd::~CampEnterAlarmWnd()
{
	SAFEDELETE( m_pCampBack );
	SAFEDELETE( m_pBlueCampMark );
	SAFEDELETE( m_pRedCampMark );
}

void CampEnterAlarmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CampBack" )
	{
		SAFEDELETE( m_pCampBack );
		m_pCampBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CampEnterAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_ENTER_DECISION:
		if( cmd == IOBN_BTNUP )
		{
			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_CAMP_CAHNGE_POS );
			kPacket << (int)m_eCampType << g_GuildInfo.IsGuildEntryDelayListDelete() << true;     // true = Result UI Show
			TCPNetwork::SendToServer( kPacket );
			HideWnd();
			g_GUIMgr.HideWnd( CAMP_ENTER_MODAL_WND );
		}
		break;
	}
}

void CampEnterAlarmWnd::SetCampType( ioUIRenderImage *pBlueCampMark,
									 ioUIRenderImage *pRedCampMark,
									 ioHashString szBlueCampName,
									 ioHashString szRedCampName,
									 CampType eCampType )
{

	m_pBlueCampMark = pBlueCampMark;
	m_pRedCampMark = pRedCampMark;

	m_szBlueCampName = szBlueCampName;
	m_szRedCampName = szRedCampName;

	m_eCampType = eCampType;
	ShowWnd();
}

void CampEnterAlarmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	if( m_pCampBack )
		m_pCampBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	switch( m_eCampType )
	{
	case CAMP_BLUE:
		{
			if( m_pBlueCampMark )
			{
				m_pBlueCampMark->Render( iXPos + 151, iYPos + 98 );
			}

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( FONT_SIZE_13, m_szBlueCampName.c_str() );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );			
			kPrinter.PrintFullTextWidthCut( iXPos + 151, iYPos + 144, TAT_CENTER, 268 );
		}
		break;
	case CAMP_RED:
		{
			if( m_pRedCampMark )
			{
				m_pRedCampMark->Render( iXPos + 151, iYPos + 98 );
			}

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_13, m_szRedCampName.c_str() );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
			kPrinter.PrintFullTextWidthCut( iXPos + 151, iYPos + 144, TAT_CENTER, 268 );
		}
		break;
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(5) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(6) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 204, FONT_SIZE_13, STR(7) );
}
//////////////////////////////////////////////////////////////////////////
CampEnterWnd::CampEnterWnd()
{
	m_pMainBannerFrm = NULL;
	m_pScheduleBack  = NULL;

	m_pBanner	= NULL;
	m_pHorzLine = NULL;
	m_pTextBack = NULL;

	m_szTournamentTitile.Clear();
	m_szBlueCampName.Clear();
	m_szRedCampName.Clear();
	m_szStartWeekDayOfWeek.Clear();
	m_szEndWeekDayOfWeek.Clear();

	m_StartMonth = 0;
	m_StartDay = 0;

	m_EndMonth = 0;
	m_EndDay = 0;
	m_EndHour = 0;
}

CampEnterWnd::~CampEnterWnd()
{
	SAFEDELETE( m_pMainBannerFrm );
	SAFEDELETE( m_pScheduleBack );

	SAFEDELETE( m_pHorzLine );
	SAFEDELETE( m_pTextBack );
}

void CampEnterWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HorzLine" )
	{
		SAFEDELETE( m_pHorzLine );
		m_pHorzLine = pImage;
	}
	else if( szType == "TextBack" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CampEnterWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MainBannerFrm" )
	{
		SAFEDELETE( m_pMainBannerFrm );
		m_pMainBannerFrm = pFrame;
	}
	else if( szType == "ScheduleBack" )
	{
		SAFEDELETE( m_pScheduleBack );
		m_pScheduleBack = pFrame;
	}
	else 
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CampEnterWnd::iwm_show()
{
	ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AGREE_CHECK ) );
	if( pCheck )
		pCheck->SetCheck( false );

	m_dwSelectIndex = g_TournamentMgr.GetRegularIndex();
	UpdateMainUI();
}

void CampEnterWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_BLUE_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
				return;
			}
			if( g_MyInfo.GetUserCampPos() != CAMP_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_CampMgr.IsCloseEntry( CAMP_BLUE ) )
			{
				char szBuff[MAX_PATH];
				sprintf_s( szBuff, "양진영의 세력비를 맞추기 위해 %s에 # 가입이 불가능합니다.", m_szBlueCampName.c_str() );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, szBuff );
			}
			else
			{
				if( !g_CampMgr.IsSeasonRecess() )
				{
					ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AGREE_CHECK ) );
					if( pCheck && pCheck->IsChecked() )
					{
						if( pTournament )
						{
							CampEnterAlarmWnd *pAlarmWnd = dynamic_cast<CampEnterAlarmWnd*>( g_GUIMgr.FindWnd( CAMP_ENTER_ALARM_WND ) );
							if( pAlarmWnd )
							{
								pAlarmWnd->SetCampType( pTournament->GetCampIcon( CAMP_BLUE ),
									pTournament->GetCampIcon( CAMP_RED ),
									pTournament->GetCampName( CAMP_BLUE ),
									pTournament->GetCampName( CAMP_RED ),
									CAMP_BLUE );
							}
						}
					}
					else
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 운영규칙에 동의하셔야 가입이 가능합니다.# [대회 운영 규칙] 클릭하시면 내용확인이 가능합니다." );
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영가입 가능한 기간이 아닙니다." );
				}
								
			}
		}
		break;
	case ID_RED_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
				return;
			}
			if( g_MyInfo.GetUserCampPos() != CAMP_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_CampMgr.IsCloseEntry( CAMP_RED ) )
			{
				char szBuff[MAX_PATH];
				sprintf_s( szBuff, "양진영의 세력비를 맞추기 위해 %s에 # 가입이 불가능합니다.", m_szRedCampName.c_str() );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, szBuff );
			}
			else
			{
				if( !g_CampMgr.IsSeasonRecess() )
				{
					ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AGREE_CHECK ) );
					if( pCheck && pCheck->IsChecked() )
					{
						if( pTournament )
						{
							CampEnterAlarmWnd *pAlarmWnd = dynamic_cast<CampEnterAlarmWnd*>(g_GUIMgr.FindWnd( CAMP_ENTER_ALARM_WND ));
							if( pAlarmWnd )
							{
								pAlarmWnd->SetCampType( pTournament->GetCampIcon( CAMP_BLUE ),
									pTournament->GetCampIcon( CAMP_RED ),
									pTournament->GetCampName( CAMP_BLUE ),
									pTournament->GetCampName( CAMP_RED ),
									CAMP_RED );
							}
						}
					}
					else
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 운영규칙에 동의하셔야 가입이 가능합니다.# [대회 운영 규칙] 클릭하시면 내용확인이 가능합니다." );
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영가입 가능한 기간이 아닙니다." );
				}
				
			}
		}
		break;
	case ID_CAMP_HELP:
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( g_CampMgr.GetCampHelpID() );
		}
		break;
	case ID_SHOW_GUILD:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( GUILD_HERO_RANKING_LIST_WND ) )
				g_GUIMgr.HideWnd( GUILD_HERO_RANKING_LIST_WND );
			else
				g_GUIMgr.ShowWnd( GUILD_HERO_RANKING_LIST_WND );
		}
		break;
	case ID_REGULATIONS:
		{
			if( cmd == IOBN_BTNUP )
			{
				g_GUIMgr.ShowWnd( TOURNAMENT_RULE_AGREE_CONFIRM_WND );
			}			
		}
		break;
	}
}

void CampEnterWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnUpdateProcess();
}
void CampEnterWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament )
	{
		if( pTournament->IsUpdateSubUI() )
		{
			UpdateMainUI();
		}
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsUpdateMainUI() )
		{
			UpdateMainUI();
		}
	}
}

void CampEnterWnd::UpdateMainUI()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament == NULL ) return;

	m_pBanner = pTournament->GetResCampBanner();
	m_szTournamentTitile = pTournament->GetTournamentTitle();

	m_szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
	m_szRedCampName = pTournament->GetCampName( CAMP_RED );

	m_StartMonth = g_CampMgr.GetStartSeasonMonth();
	m_StartDay   = g_CampMgr.GetStartSeasonDay();

	m_EndMonth = g_CampMgr.GetNextSeasonMonth();
	m_EndDay   = g_CampMgr.GetNextSeasonDay();
	m_EndHour  = g_CampMgr.GetNextSeasonHour();

	m_szStartWeekDayOfWeek	= DateHelp::GetWeekDayString( g_CampMgr.GetStartSeasonWeekDay() ); 
	m_szEndWeekDayOfWeek	= DateHelp::GetWeekDayString( g_CampMgr.GetNextSeasonWeekDay() ); 

	ioButton* pButton = dynamic_cast< ioButton* >( FindChildWnd( ID_BLUE_ENTER ) );
	if( pButton )
	{
		char szBuff[MAX_PATH];
		sprintf( szBuff, "%s 가입", m_szBlueCampName.c_str() );
		ioUITitle *pUITitle = pButton->GetTitle();
		if( pUITitle )
		{
			pUITitle->SetHorzAlign( TAT_CENTER );
			pUITitle->SetWidthCutSize( 177.0f );
			pUITitle->SetText( szBuff, "" );
		}
	}

	pButton = dynamic_cast< ioButton* >( FindChildWnd( ID_RED_ENTER ) );
	if( pButton )
	{
		char szBuff[MAX_PATH];
		sprintf( szBuff, "%s 가입", m_szRedCampName.c_str() );
		ioUITitle *pUITitle = pButton->GetTitle();
		if( pUITitle )
		{
			pUITitle->SetHorzAlign( TAT_CENTER );
			pUITitle->SetWidthCutSize( 177.0f );
			pUITitle->SetText( szBuff, "" );
		}
	}
}

void CampEnterWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnMainBannerRender( iXPos, iYPos );
	OnExplainRender( iXPos, iYPos );
	OnScheduleRender( iXPos, iYPos );

	if( m_pMainBannerFrm )
	{
		m_pMainBannerFrm->Render( iXPos + 10, iYPos + 7 );
	}
}

void CampEnterWnd::OnMainBannerRender( int iXPos, int iYPos )
{
	if( m_pBanner )
	{
		m_pBanner->Render( iXPos + 13, iYPos + 10 );
	}

	if( m_pHorzLine )
	{
		m_pHorzLine->Render( iXPos + 13, iYPos + 280 );
	}
}

void CampEnterWnd::OnExplainRender( int iXPos, int iYPos )
{
	if( m_pTextBack )
	{
		m_pTextBack->Render( iXPos + 13, iYPos + 281 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s의 ", m_szTournamentTitile );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( D3DCOLOR_XRGB( 0, 155, 255 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s ", m_szBlueCampName.c_str() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( D3DCOLOR_XRGB( 250, 180, 0 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "VS" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( D3DCOLOR_XRGB( 255, 60, 0 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, " %s", m_szRedCampName.c_str() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, " 진영전이 열렸습니다." );
	kPrinter.PrintFullText( iXPos + 201, iYPos + 293, TAT_CENTER );
	kPrinter.ClearList();
	
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "아래버튼을 눌러 마음에 드는 진영에 가입하세요" );
	kPrinter.PrintFullText( iXPos + 201, iYPos + 309, TAT_CENTER );
	kPrinter.ClearList();
	
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "에 동의합니다." );

	kPrinter.PrintFullText( iXPos + 125, iYPos + 336, TAT_LEFT );
	kPrinter.ClearList();
}

void CampEnterWnd::OnScheduleRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%d/%d(%s)", 
										 m_StartMonth,
										 m_StartDay,
										 m_szStartWeekDayOfWeek.c_str() );

	kPrinter.AddTextPiece( FONT_SIZE_11, " ~ " );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%d/%d(%s) %d:00",
										 m_EndMonth,
										 m_EndDay,
										 m_szEndWeekDayOfWeek.c_str(),
										 m_EndHour );
	
	if( m_pScheduleBack )
	{
		int iWidth = kPrinter.GetFullWidth() + 20;
		int iOffset = 201 - iWidth/2;

		m_pScheduleBack->SetSize( iWidth, 16 );
		m_pScheduleBack->Render( iXPos + iOffset, iYPos + 273 );
	}

	kPrinter.PrintFullText( iXPos + 201, iYPos + 275, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
CampEnterModalWnd::CampEnterModalWnd()
{
	m_pMainBannerFrm = NULL;
	m_pScheduleBack  = NULL;

	m_pBanner	= NULL;
	m_pHorzLine = NULL;
	m_pTextBack = NULL;

	m_szTournamentTitile.Clear();
	m_szBlueCampName.Clear();
	m_szRedCampName.Clear();
	m_szStartWeekDayOfWeek.Clear();
	m_szEndWeekDayOfWeek.Clear();

	m_StartMonth = 0;
	m_StartDay = 0;

	m_EndMonth = 0;
	m_EndDay = 0;
	m_EndHour = 0;
}

CampEnterModalWnd::~CampEnterModalWnd()
{
	SAFEDELETE( m_pMainBannerFrm );
	SAFEDELETE( m_pScheduleBack );

	SAFEDELETE( m_pHorzLine );
	SAFEDELETE( m_pTextBack );
}

void CampEnterModalWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HorzLine" )
	{
		SAFEDELETE( m_pHorzLine );
		m_pHorzLine = pImage;
	}
	else if( szType == "TextBack" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CampEnterModalWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MainBannerFrm" )
	{
		SAFEDELETE( m_pMainBannerFrm );
		m_pMainBannerFrm = pFrame;
	}
	else if( szType == "ScheduleBack" )
	{
		SAFEDELETE( m_pScheduleBack );
		m_pScheduleBack = pFrame;
	}
	else 
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CampEnterModalWnd::iwm_show()
{
	m_dwSelectIndex = g_TournamentMgr.GetRegularIndex();
	UpdateMainUI();
}

void CampEnterModalWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_BLUE_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetUserCampPos() != CAMP_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_CampMgr.IsCloseEntry( CAMP_BLUE ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else
			{
				TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
				if( pTournament )
				{
					CampEnterAlarmWnd *pAlarmWnd = dynamic_cast<CampEnterAlarmWnd*>(g_GUIMgr.FindWnd( CAMP_ENTER_ALARM_WND ));
					if( pAlarmWnd )
					{
						pAlarmWnd->SetCampType( pTournament->GetCampIcon( CAMP_BLUE ),
												pTournament->GetCampIcon( CAMP_RED ),
												pTournament->GetCampName( CAMP_BLUE ),
												pTournament->GetCampName( CAMP_RED ),
												CAMP_BLUE );
					}
				}
			}
		}
		break;
	case ID_RED_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetUserCampPos() != CAMP_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_CampMgr.IsCloseEntry( CAMP_RED ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}
			else
			{
				TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
				if( pTournament )
				{
					CampEnterAlarmWnd *pAlarmWnd = dynamic_cast<CampEnterAlarmWnd*>(g_GUIMgr.FindWnd( CAMP_ENTER_ALARM_WND ));
					if( pAlarmWnd )
					{
						pAlarmWnd->SetCampType( pTournament->GetCampIcon( CAMP_BLUE ),
							pTournament->GetCampIcon( CAMP_RED ),
							pTournament->GetCampName( CAMP_BLUE ),
							pTournament->GetCampName( CAMP_RED ),
							CAMP_RED );
					}
				}
			}
		}
		break;
	case ID_POSTPONE :
	case ID_EXIT :
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	}
}

void CampEnterModalWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnUpdateProcess();
}
void CampEnterModalWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament )
	{
		if( pTournament->IsUpdateSubUI() )
		{
			UpdateMainUI();
		}
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsUpdateMainUI() )
		{
			UpdateMainUI();
		}
	}
}

void CampEnterModalWnd::UpdateMainUI()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament == NULL ) return;

	m_pBanner = pTournament->GetResCampBanner();
	m_szTournamentTitile = pTournament->GetTournamentTitle();

	m_szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
	m_szRedCampName = pTournament->GetCampName( CAMP_RED );

	m_StartMonth = g_CampMgr.GetStartSeasonMonth();
	m_StartDay   = g_CampMgr.GetStartSeasonDay();

	m_EndMonth = g_CampMgr.GetNextSeasonMonth();
	m_EndDay   = g_CampMgr.GetNextSeasonDay();
	m_EndHour  = g_CampMgr.GetNextSeasonHour();

	m_szStartWeekDayOfWeek	= DateHelp::GetWeekDayString( g_CampMgr.GetStartSeasonWeekDay() ); 
	m_szEndWeekDayOfWeek	= DateHelp::GetWeekDayString( g_CampMgr.GetNextSeasonWeekDay() ); 

	ioButton* pButton = dynamic_cast< ioButton* >( FindChildWnd( ID_BLUE_ENTER ) );
	if( pButton )
	{
		char szBuff[MAX_PATH];
		sprintf( szBuff, "%s 가입", m_szBlueCampName.c_str() );
		pButton->SetTitleText( szBuff );
	}

	pButton = dynamic_cast< ioButton* >( FindChildWnd( ID_RED_ENTER ) );
	if( pButton )
	{
		char szBuff[MAX_PATH];
		sprintf( szBuff, "%s 가입", m_szRedCampName.c_str() );
		pButton->SetTitleText( szBuff );
	}
}

void CampEnterModalWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pMainBannerFrm )
	{
		m_pMainBannerFrm->Render( iXPos + 17, iYPos + 54 );
	}

	OnMainBannerRender( iXPos, iYPos );		
	OnExplainRender( iXPos, iYPos );
	OnScheduleRender( iXPos, iYPos );
}

void CampEnterModalWnd::OnMainBannerRender( int iXPos, int iYPos )
{
	if( m_pBanner )
	{
		m_pBanner->Render( iXPos + 20, iYPos + 57 );
	}

	if( m_pHorzLine )
	{
		m_pHorzLine->Render( iXPos + 20, iYPos + 327 );
	}
}

void CampEnterModalWnd::OnExplainRender( int iXPos, int iYPos )
{
	if( m_pTextBack )
	{
		m_pTextBack->Render( iXPos + 20, iYPos + 328 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s의 ", m_szTournamentTitile );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s", m_szBlueCampName.c_str() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_YELLOW );
	kPrinter.AddTextPiece( FONT_SIZE_12, "VS" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s", m_szRedCampName.c_str() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, " 진영전이 열렸습니다." );
	kPrinter.PrintFullText( iXPos + 208, iYPos + 352, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "아래버튼을 눌러 마음에 드는 진영에 가입하세요" );
	kPrinter.PrintFullText( iXPos + 208, iYPos + 368, TAT_CENTER );
	kPrinter.ClearList();
}

void CampEnterModalWnd::OnScheduleRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%d/%d(%s)", 
		m_StartMonth,
		m_StartDay,
		m_szStartWeekDayOfWeek.c_str() );

	kPrinter.AddTextPiece( FONT_SIZE_11, " ~ " );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%d/%d(%s) %d:00",
		m_EndMonth,
		m_EndDay,
		m_szEndWeekDayOfWeek.c_str(),
		m_EndHour );

	if( m_pScheduleBack )
	{
		int iWidth = kPrinter.GetFullWidth() + 20;
		int iOffset = 201 - iWidth/2;

		m_pScheduleBack->SetSize( iWidth, 16 );
		m_pScheduleBack->Render( iXPos + iOffset, iYPos + 320 );
	}

	kPrinter.PrintFullText( iXPos + 201, iYPos + 322, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
CampSeasonBonus::CampSeasonBonus() : m_iSeasonBonus( 0 ), m_iBlueCampPoint( 0 ), m_iRedCampPoint( 0 ), m_iMyCampType( 0 ), m_iMyCampPoint( 0 ), m_iMyCampRank( 0 )
{
	m_pBlueCampMark	= NULL;
	m_pRedCampMark	= NULL;
	m_pMapLeft		= NULL;
	m_pMapCenter1	= NULL;
	m_pMapCenter2	= NULL;
	m_pMapCenter3	= NULL;
	m_pMapCenter4	= NULL;
	m_pMapCenter5	= NULL;
	m_pMapCenter6	= NULL;
	m_pMapCenter7	= NULL;
	m_pMapRight		= NULL;
	m_pIconBack		= NULL;
	m_pCampPointIcon= NULL;
	m_pCampRankIcon	= NULL;
	m_pCampBonusIcon= NULL;
	m_pBlueCampText = NULL;
	m_pRedCampText  = NULL;
	m_pWinText	    = NULL;
	m_pLoseText		= NULL;
	m_pDrawText		= NULL;

	m_szBlueCampName.Clear();
	m_szRedCampName.Clear();
}

CampSeasonBonus::~CampSeasonBonus()
{
	SAFEDELETE( m_pBlueCampMark );
	SAFEDELETE( m_pRedCampMark );
	SAFEDELETE( m_pMapLeft );
	SAFEDELETE( m_pMapCenter1 );
	SAFEDELETE( m_pMapCenter2 );
	SAFEDELETE( m_pMapCenter3 );
	SAFEDELETE( m_pMapCenter4 );
	SAFEDELETE( m_pMapCenter5 );
	SAFEDELETE( m_pMapCenter6 );
	SAFEDELETE( m_pMapCenter7 );
	SAFEDELETE( m_pMapRight );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCampPointIcon );
	SAFEDELETE( m_pCampRankIcon );
	SAFEDELETE( m_pCampBonusIcon );
	SAFEDELETE( m_pBlueCampText );
	SAFEDELETE( m_pRedCampText );
	SAFEDELETE( m_pWinText );
	SAFEDELETE( m_pLoseText );
	SAFEDELETE( m_pDrawText );
}

float CampSeasonBonus::GetCampInfluencePoint( CampType eCampType )
{
	int iTotalGauge = m_iBlueCampPoint + m_iRedCampPoint;
	if( iTotalGauge <= 0 )
		return 0.500f;
	else if( eCampType == CAMP_BLUE )
		return ( (float)m_iBlueCampPoint / (float)iTotalGauge ) + 0.00005f ;
	else if( eCampType == CAMP_RED )
		return ( (float)m_iRedCampPoint / (float)iTotalGauge ) + 0.00005f;

	return 0.500f;
}

void CampSeasonBonus::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueCampMark" )
	{
		SAFEDELETE( m_pBlueCampMark );
		m_pBlueCampMark = pImage;
	}
	else if( szType == "RedCampMark" )
	{
		SAFEDELETE( m_pRedCampMark );
		m_pRedCampMark = pImage;
	}
	else if( szType == "MapLeft" )
	{
		SAFEDELETE( m_pMapLeft );
		m_pMapLeft = pImage;
	}
	else if( szType == "MapCenter1" )
	{
		SAFEDELETE( m_pMapCenter1 );
		m_pMapCenter1 = pImage;
	}
	else if( szType == "MapCenter2" )
	{
		SAFEDELETE( m_pMapCenter2 );
		m_pMapCenter2 = pImage;
	}
	else if( szType == "MapCenter3" )
	{
		SAFEDELETE( m_pMapCenter3 );
		m_pMapCenter3 = pImage;
	}
	else if( szType == "MapCenter4" )
	{
		SAFEDELETE( m_pMapCenter4 );
		m_pMapCenter4 = pImage;
	}
	else if( szType == "MapCenter5" )
	{
		SAFEDELETE( m_pMapCenter5 );
		m_pMapCenter5 = pImage;
	}
	else if( szType == "MapCenter6" )
	{
		SAFEDELETE( m_pMapCenter6 );
		m_pMapCenter6 = pImage;
	}
	else if( szType == "MapCenter7" )
	{
		SAFEDELETE( m_pMapCenter7 );
		m_pMapCenter7 = pImage;
	}
	else if( szType == "MapRight" )
	{
		SAFEDELETE( m_pMapRight );
		m_pMapRight = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "CampPointIcon" )
	{
		SAFEDELETE( m_pCampPointIcon );
		m_pCampPointIcon = pImage;
	}
	else if( szType == "CampRankIcon" )
	{
		SAFEDELETE( m_pCampRankIcon );
		m_pCampRankIcon = pImage;
	}
	else if( szType == "CampBonusIcon" )
	{
		SAFEDELETE( m_pCampBonusIcon );
		m_pCampBonusIcon = pImage;
	}
	else if( szType == "BlueCampText" )
	{
		SAFEDELETE( m_pBlueCampText );
		m_pBlueCampText = pImage;
	}
	else if( szType == "RedCampText" )
	{
		SAFEDELETE( m_pRedCampText );
		m_pRedCampText = pImage;
	}
	else if( szType == "WinText" )
	{
		SAFEDELETE( m_pWinText );
		m_pWinText = pImage;
	}
	else if( szType == "LoseText" )
	{
		SAFEDELETE( m_pLoseText );
		m_pLoseText = pImage;
	}
	else if( szType == "DrawText" )
	{
		SAFEDELETE( m_pDrawText );
		m_pDrawText = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void CampSeasonBonus::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void CampSeasonBonus::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderMent( iXPos, iYPos );
	OnRenderMap( iXPos, iYPos );
	OnRenderIcon( iXPos, iYPos );
}

void CampSeasonBonus::OnRenderMent( int iXPos, int iYPos )
{
	switch( m_iMyCampType )
	{
	case CAMP_BLUE:
		if( m_iBlueCampPoint > m_iRedCampPoint )
		{
			if( m_pBlueCampText )
				m_pBlueCampText->Render( iXPos, iYPos );
			if( m_pWinText )
				m_pWinText->Render( iXPos, iYPos );
		}
		else if( m_iBlueCampPoint < m_iRedCampPoint )
		{
			if( m_pBlueCampText )
				m_pBlueCampText->Render( iXPos, iYPos );
			if( m_pLoseText )
				m_pLoseText->Render( iXPos, iYPos );
		}
		else
		{
			if( m_pDrawText )
				m_pDrawText->Render( iXPos, iYPos );
		}
		break;
	case CAMP_RED:
		if( m_iRedCampPoint > m_iBlueCampPoint )
		{
			if( m_pRedCampText )
				m_pRedCampText->Render( iXPos, iYPos );
			if( m_pWinText )
				m_pWinText->Render( iXPos, iYPos );
		}
		else if( m_iRedCampPoint < m_iBlueCampPoint )
		{
			if( m_pRedCampText )
				m_pRedCampText->Render( iXPos, iYPos );
			if( m_pLoseText )
				m_pLoseText->Render( iXPos, iYPos );
		}
		else
		{
			if( m_pDrawText )
				m_pDrawText->Render( iXPos, iYPos );
		}
		break;
	}	

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	char szConvertText[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_iSeasonBonus, szConvertText, sizeof( szConvertText ) );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), szConvertText );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos + 173, iYPos + 83, TAT_CENTER );
}

void CampSeasonBonus::OnRenderMap( int iXPos, int iYPos )
{
	if( !m_pMapLeft || !m_pMapRight || !m_pMapCenter1 || !m_pMapCenter2 || !m_pMapCenter3 || 
		!m_pMapCenter4 || !m_pMapCenter5 || !m_pMapCenter6 || !m_pMapCenter7 ) return;
	
	float fBlueRate = GetCampInfluencePoint( CAMP_BLUE ) * FLOAT100;
	float fRedRate = GetCampInfluencePoint( CAMP_RED ) * FLOAT100;

	m_pMapLeft->Render( iXPos + 7, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( fBlueRate <= 48.50f )
	{	
		m_pMapCenter1->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( fBlueRate <= 49.10f )
	{	
		m_pMapCenter2->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( fBlueRate <= 49.70f )
	{	
		m_pMapCenter3->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( fBlueRate <= 50.30f )
	{	
		m_pMapCenter4->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( fBlueRate <= 50.90f )
	{	
		m_pMapCenter5->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( fBlueRate <= 51.50f )
	{	
		m_pMapCenter6->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{	
		m_pMapCenter7->Render( iXPos + 120, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	m_pMapRight->Render( iXPos + 224, iYPos + 153, UI_RENDER_NORMAL, TFO_BILINEAR );

	if( m_iBlueCampPoint > m_iRedCampPoint )
	{
		if( m_pBlueCampMark )
		{
			m_pBlueCampMark->Render( iXPos + 62, iYPos + 112 );
			m_pBlueCampMark->Render( iXPos + 62, iYPos + 112 );
		}

		if( m_pRedCampMark )
		{
			m_pRedCampMark->Render( iXPos + 200, iYPos + 112, UI_RENDER_GRAY );
			m_pRedCampMark->Render( iXPos + 200, iYPos + 112, UI_RENDER_GRAY_LIGHT );
		}
	}
	else
	{
		if( m_pBlueCampMark )
		{
			m_pBlueCampMark->Render( iXPos + 62, iYPos + 112, UI_RENDER_GRAY );
			m_pBlueCampMark->Render( iXPos + 62, iYPos + 112, UI_RENDER_GRAY_LIGHT );
		}

		if( m_pRedCampMark )
		{
			m_pRedCampMark->Render( iXPos + 200, iYPos + 112 );
			m_pRedCampMark->Render( iXPos + 200, iYPos + 112 );
		}
	}		

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 43, 102, 174 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 191, FONT_SIZE_17, STR(1), fBlueRate );

	char szConvertText[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_iBlueCampPoint, szConvertText, sizeof( szConvertText ) );
	
	g_FontMgr.PrintText( iXPos + 103, iYPos + 213, FONT_SIZE_12, "%s(%s점)", m_szBlueCampName.c_str(), szConvertText );

	g_FontMgr.SetBkColor( 171, 51, 36 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 241, iYPos + 191, FONT_SIZE_17, STR(3), fRedRate );

	Help::ConvertNumToStrComma( m_iRedCampPoint, szConvertText, sizeof( szConvertText ) );
	g_FontMgr.PrintText( iXPos + 241, iYPos + 213, FONT_SIZE_12, "%s(%s점)", m_szRedCampName.c_str(), szConvertText );
}

void CampSeasonBonus::OnRenderIcon( int iXPos, int iYPos )
{
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( FLOAT1 );
		m_pIconBack->Render( iXPos + 69, iYPos + 311, UI_RENDER_MULTIPLY );
		m_pIconBack->Render( iXPos + 173, iYPos + 311, UI_RENDER_MULTIPLY );
		m_pIconBack->Render( iXPos + 277, iYPos + 311, UI_RENDER_MULTIPLY );
	}

	if( m_pCampPointIcon )
	{
		m_pCampPointIcon->SetScale( FLOAT1 );
		m_pCampPointIcon->Render( iXPos + 69, iYPos + 311 );
	}

	if( m_pCampRankIcon )
	{
		m_pCampRankIcon->SetScale( FLOAT1 );
		m_pCampRankIcon->Render( iXPos + 173, iYPos + 311 );
	}

	if( m_pCampBonusIcon )
	{
		m_pCampBonusIcon->SetScale( FLOAT1 );
		m_pCampBonusIcon->Render( iXPos + 277, iYPos + 311 );
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER);
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 69, iYPos + 357, FONT_SIZE_13, STR(1) );
	g_FontMgr.PrintText( iXPos + 173, iYPos + 357, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 277, iYPos + 357, FONT_SIZE_13, STR(3) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	char szConvertText[MAX_PATH] = "";
	
	Help::ConvertNumToStrComma( m_iMyCampPoint, szConvertText, sizeof( szConvertText ) );
	g_FontMgr.PrintText( iXPos + 69, iYPos + 375, FONT_SIZE_13, STR(4), szConvertText );
	
	Help::ConvertNumToStrComma( m_iMyCampRank, szConvertText, sizeof( szConvertText ) );
	g_FontMgr.PrintText( iXPos + 173, iYPos + 375, FONT_SIZE_13, STR(5), szConvertText );
	
	Help::ConvertNumToStrComma( m_iSeasonBonus, szConvertText, sizeof( szConvertText ) );
	g_FontMgr.PrintText( iXPos + 277, iYPos + 375, FONT_SIZE_13, STR(6), szConvertText );
}

void CampSeasonBonus::SetSeasonBonus( int iSeasonBonus, int iBlueCampPoint, int iRedCampPoint, int iMyCampType, int iMyCampPoint, int iMyCampRank )
{
	m_iSeasonBonus	= iSeasonBonus;
	m_iBlueCampPoint= iBlueCampPoint;
	m_iRedCampPoint = iRedCampPoint;
	m_iMyCampType   = iMyCampType;
	m_iMyCampPoint  = iMyCampPoint;
	m_iMyCampRank   = iMyCampRank;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament )
	{
		m_szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
		m_szRedCampName = pTournament->GetCampName( CAMP_RED );
	}

	ShowWnd();
}
//////////////////////////////////////////////////////////////////////////
CampJoinNeedPopup::CampJoinNeedPopup()
{
}

CampJoinNeedPopup::~CampJoinNeedPopup()
{
}

void CampJoinNeedPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_LINK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd )
				{
					pLobbyMainWnd->SetCampLink();

					if( g_GUIMgr.IsShow( GUILD_CREATE_WND ) )
					{
						g_GUIMgr.HideWnd( GUILD_CREATE_WND );
						g_GUIMgr.HideWnd( MY_INVENTORY_WND );
					}
				}
				else if( g_GUIMgr.IsShow( GUILD_CREATE_WND ) )
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );
					kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );
					kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );
					kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );

					g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(4), STR(5), STR(6), STR(7) );
				}
			}
			else
			{
				QuickLobbyMenu* pMenu = dynamic_cast<QuickLobbyMenu*>( g_GUIMgr.FindWnd( LOBBY_QUICK_MENU ) );
				if( pMenu )
				{
					pMenu->OnCampLButtonUP( true );

					if( g_GUIMgr.IsShow( GUILD_CREATE_WND ) )
					{
						g_GUIMgr.HideWnd( GUILD_CREATE_WND );
						g_GUIMgr.HideWnd( MY_INVENTORY_WND );
					}
				}				
			}
		}
		break;		
	}
}

void CampJoinNeedPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(3) );
}