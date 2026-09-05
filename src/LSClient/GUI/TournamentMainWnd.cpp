

#include "StdAfx.h"

#include "../ioShuffleRoomMgr.h"
#include "../ioRankBattleManager.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioGuildMarkMgr.h"
#include "../ioLadderTeamMgr.h"
#include "../ioDateHelp.h"

#include "CustomTournamentCreateWnd.h"
#include "TournamentMainWnd.h"
#include "TournamentDetailMainWnd.h"
#include "UserInfoToolTip.h"

TournamentListBtn::TournamentListBtn()
{
	m_pEmptyBack = NULL;
	m_pSelectBack= NULL;
	m_pBannerMark= NULL;

	m_dwIndex = 0;
	m_Type    = 0;
}

TournamentListBtn::~TournamentListBtn()
{
	SAFEDELETE( m_pEmptyBack );
	SAFEDELETE( m_pSelectBack );
}

void TournamentListBtn::iwm_show()
{

}

void TournamentListBtn::iwm_hide()
{

}

void TournamentListBtn::SetTournament( DWORD dwIndex, DWORD dwOwnerIndex, BYTE Type )
{
	m_dwIndex	   = dwIndex;
	m_Type		   = Type;
	m_dwOwnerIndex = dwOwnerIndex;

	if( m_dwIndex == 0 )
	{
		m_pBannerMark = NULL;

		SetInActive();
	}
	else
	{
		SetActive();

		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwIndex, false );
		if( pTournament )
		{
			m_pBannerMark = pTournament->GetResBannerMark();
		}
	}
}

void TournamentListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "EmptyBack" )
	{
		SAFEDELETE( m_pEmptyBack );
		m_pEmptyBack = pImage;
	}
	else if( szType == "SelectBack" )
	{
		SAFEDELETE( m_pSelectBack );
		m_pSelectBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TournamentListBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void TournamentListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bSelectBtn = false;
	TournamentMainWnd *pParent = dynamic_cast< TournamentMainWnd * >( GetParent() );
	if( pParent )
	{
		if( pParent->GetTournamentIndex() == m_dwIndex )
		{
			if( m_pSelectBack )
				m_pSelectBack->Render( iXPos, iYPos );
			bSelectBtn = true;
		}
	}

	if( m_pBannerMark )
	{
		m_pBannerMark->Render( iXPos, iYPos );

		if( (!bSelectBtn && m_bOver) || (TournamentMainWnd::m_bScreen && m_Type != TournamentNode::TYPE_REGULAR) )
			m_pBannerMark->Render( iXPos, iYPos, UI_RENDER_SCREEN );

	}
	else if( m_pEmptyBack )
	{
		m_pEmptyBack->Render( iXPos, iYPos );
	}
}

bool TournamentMainWnd::m_bScreen = false;

//////////////////////////////////////////////////////////////////////////
TournamentMainWnd::TournamentMainWnd()
{
	m_pMainBannerFrm		= NULL;
	m_pScheduleOpenGray		= NULL;
	m_pScheduleOpenOrange	= NULL;
	m_pScheduleOpenPink		= NULL;
	m_pScheduleCloseGray	= NULL;
	m_pScheduleCloseGreen	= NULL;
	m_pScheduleCloseOrange	= NULL;
	
	m_pHorzLine = NULL;
	m_pVertLine = NULL;
	m_pTextBack = NULL;
	m_pTodayMark= NULL;

	m_pMyTeamArrow = NULL;

	m_pBanner = NULL;

	m_dwSelectIndex		 = 0;
	m_dwWantTourIndex	 = 0;
	m_Type				 = 0;
	m_State				 = 0;
	m_CustomProgressType = 0;
	m_dwOwnerIdx		 = 0;
	m_StateEndMonth		 = 0;
	m_StateEndDay		 = 0;
	m_StateEndHour		 = 0;
	m_StateEndMinute	 = 0;
	m_MaxPlayer			 = 0;

	m_TournamentStartTeamCount = 0;
	m_TournamentMaxRound       = 0;

	m_iTournamentPage	= 0;
	m_dwTournamentRequestTime = 0;

	m_dwScreenTimer    = 0;

	m_dwMyTeamArrowTimer = 0;
	m_fArrowYPos = 0.0f;
}

TournamentMainWnd::~TournamentMainWnd()
{
	SAFEDELETE( m_pMainBannerFrm );
	SAFEDELETE( m_pScheduleOpenGray );
	SAFEDELETE( m_pScheduleOpenOrange );
	SAFEDELETE( m_pScheduleOpenPink );
	SAFEDELETE( m_pScheduleCloseGray );
	SAFEDELETE( m_pScheduleCloseOrange );
	SAFEDELETE( m_pScheduleCloseGreen );

	SAFEDELETE( m_pHorzLine );
	SAFEDELETE( m_pVertLine );
	SAFEDELETE( m_pTextBack );
	SAFEDELETE( m_pTodayMark );

	SAFEDELETE( m_pMyTeamArrow );
}

void TournamentMainWnd::SetTournamentBtn()
{
	m_TournamentBtnSet.clear();

	// 버튼 초기화
	for(int i = ID_TOURNAMENT_BTN1;i < ID_TOURNAMENT_BTN13 + 1;i++)
	{
		TournamentListBtn *pTournament = dynamic_cast< TournamentListBtn * >( FindChildWnd( i ) );
		if( pTournament == NULL ) continue;

		if( m_iTournamentPage == 0 )
		{
			// 첫 페이지는 무조건 정규 대회
			if( COMPARE( i, ID_TOURNAMENT_BTN2, ID_TOURNAMENT_BTN5 + 1 ) )
			{
				pTournament->HideWnd();
			}
			else
			{
				pTournament->ShowWnd();
				m_TournamentBtnSet.push_back( pTournament );
			}
		}
		else
		{
			if( i == ID_TOURNAMENT_BTN1 ) // 정규 대회 감추기
				pTournament->HideWnd();
			else
			{
				pTournament->ShowWnd();
				m_TournamentBtnSet.push_back( pTournament );
			}
		}
	}
}

void TournamentMainWnd::SendTournamentList( int iCurrentPage )
{
	m_dwTournamentRequestTime = FRAMEGETTIME();

	int iStartCount   = 0;
	int iRequestCount = MAX_FIRST_PAGE_LIST;
	if( iCurrentPage > 0 )
	{
		iRequestCount = MAX_NEXT_PAGE_LIST;
		iStartCount   = MAX_FIRST_PAGE_LIST + (( iCurrentPage - 1 ) * MAX_NEXT_PAGE_LIST);
	}

	SP2Packet kPacket( CTPK_TOURNAMENT_LIST_REQUEST );
	kPacket << iCurrentPage << iStartCount << iRequestCount << m_dwWantTourIndex;
	TCPNetwork::SendToServer( kPacket );
	m_dwWantTourIndex = 0;
}

void TournamentMainWnd::ApplyTournamentList( SP2Packet &rkPacket )
{	
	int iListSize = 0;
	int iCurrPage = 0;
	DWORD dwTourIndex = 0;
	rkPacket >> dwTourIndex >> m_iTournamentPage >> iListSize;

	if( dwTourIndex != 0 )
	{
		m_dwSelectIndex = dwTourIndex;
		UpdateMainUI();
	}	

	SetTournamentBtn();
	if( iListSize == 0 )
	{
		ioWndList::iterator iter = m_TournamentBtnSet.begin();
		for(;iter != m_TournamentBtnSet.end();++iter)
		{
			TournamentListBtn *pTournament = dynamic_cast< TournamentListBtn * >( *iter );
			if( pTournament == NULL ) continue;

			pTournament->SetTournament( 0, 0, 0 );
		}		
		SetChildInActive( ID_TOURNAMENT_RIGHT_BTN );
	}
	else
	{
		// 일단 초기화
		int i = 0;
		ioWndList::iterator iter = m_TournamentBtnSet.begin();
		for(;iter != m_TournamentBtnSet.end();++iter)
		{
			TournamentListBtn *pTournament = dynamic_cast< TournamentListBtn * >( *iter );
			if( pTournament == NULL ) continue;

			pTournament->SetTournament( 0, 0, 0 );
		}
		
		iter = m_TournamentBtnSet.begin();
		for(i = 0;i < iListSize;i++)
		{
			BYTE Type;
			DWORD dwIndex;
			DWORD dwOwnerIndex = 0;
			rkPacket >> dwIndex >> Type;

			if( Type == TournamentNode::TYPE_REGULAR )
			{
				int iRegularResource;
				rkPacket >> iRegularResource;
				g_TournamentMgr.CreateTournamentRegular( dwIndex, iRegularResource );
			}
			else
			{
				DWORD dwBannerBig, dwBannerSmall;
				rkPacket >> dwOwnerIndex >> dwBannerBig >> dwBannerSmall;
				g_TournamentMgr.CreateTournamentCustom( dwIndex, dwOwnerIndex, dwBannerBig, dwBannerSmall );
			}

			if( iter != m_TournamentBtnSet.end() )
			{
				TournamentListBtn *pTournament = dynamic_cast< TournamentListBtn * >( *iter );
				if( pTournament )
				{
					pTournament->SetTournament( dwIndex, dwOwnerIndex, Type );
				}
				++iter;
			}
		}

		bool bLastPage = false;
		rkPacket >> bLastPage;
		if( bLastPage )
		{
			SetChildInActive( ID_TOURNAMENT_RIGHT_BTN );
		}
		else
		{
			SetChildActive( ID_TOURNAMENT_RIGHT_BTN );
		}
	}

	if( m_iTournamentPage == 0 )
		SetChildInActive( ID_TOURNAMENT_LEFT_BTN );
	else
		SetChildActive( ID_TOURNAMENT_LEFT_BTN );
}

void TournamentMainWnd::ApplyTournamentConfirmCheck( SP2Packet &rkPacket )
{
	DWORD dwTourIndex;
	int iResult;
	rkPacket >> iResult >> dwTourIndex;

	TCPNetwork::MouseBusy( false );

	switch( iResult )
	{
	//대회 참가 인증 성공
	case TOURNAMENT_JOIN_CONFIRM_OK:
		{
			g_MyInfo.AddTournamentConfirm( dwTourIndex );
			TournamentTeamCreateWnd *pCreateWnd = dynamic_cast< TournamentTeamCreateWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_CREATE ) );
			if( pCreateWnd )
			{
				pCreateWnd->SetTournamentIndex( dwTourIndex );
			}
		}
		break;
	// 대회가 종료되었음
	case TOURNAMENT_JOIN_CONFIRM_CLOSE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회가 종료 되었습니다." );
		}
		break;
	// 대회 인증 실패
	case TOURNAMENT_JOIN_CONFIRM_FAILED:
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "참가하실 대회는");
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "오프라인형 대회");
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "이기 때문에");

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "대회팀 생성시");
			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "주최자의 승인이 필요" );
			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "합니다." );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
		}
		break;
	}
}

void TournamentMainWnd::ApplyTournamentConfirmRequest( SP2Packet &rkPacket )
{
	DWORD dwTourIndex;
	int iResult;
	rkPacket >> iResult >> dwTourIndex;

	TCPNetwork::MouseBusy( false );

	switch( iResult )
	{
	case TOURNAMENT_JOIN_CONFIRM_REQUEST_OK: // 팀생성 권한 요청완료
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀 생성권한 요청을 완료 하였습니다." );
		}
		break;
	case TOURNAMENT_JOIN_CONFIRM_REQUEST_AGREE: //팀 생성 권한 획득
		{
			g_MyInfo.RemoveTournamentConfirmRequest( dwTourIndex );
			g_MyInfo.AddTournamentConfirm( dwTourIndex );
			TournamentJoinConfirmAgreeWnd* pWnd = dynamic_cast<TournamentJoinConfirmAgreeWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_JOIN_CONFIRM_AGREE_WND ) );
			if( pWnd )
				pWnd->SetTournament( dwTourIndex, TournamentJoinConfirmAgreeWnd::TOUR_JOIN_REQUEST );
		}
		break;
	case TOURNAMENT_JOIN_CONFIRM_REQUEST_OFFLINE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "주최자가 오프라인입니다.#주최자가 온라인 일 경우에만 요청이 가능합니다" );
		}
		break;
	case TOURNAMENT_JOIN_CONFIRM_REQUEST_REJECT:
		{
			//g_GUIMgr.SetMsgBox( MB_OK, NULL, "주최자가 참가 신청을 거절 하였습니다." );
		}
		break;
	}
}

void TournamentMainWnd::iwm_show()
{	
	m_pBanner = NULL;

	if( m_dwSelectIndex == 0 )
		m_dwSelectIndex = g_TournamentMgr.GetRegularIndex();
	
	UpdateMainUI();
	SetChildInActive( ID_TOURNAMENT_LEFT_BTN );
	SetChildInActive( ID_TOURNAMENT_RIGHT_BTN );

	SetTournamentBtn();

	SendTournamentList( m_iTournamentPage );
}

void TournamentMainWnd::iwm_hide()
{
	m_iTournamentPage = 0;
}

void TournamentMainWnd::iwm_create()
{
}

void TournamentMainWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MainBannerFrm" )
	{
		SAFEDELETE( m_pMainBannerFrm );
		m_pMainBannerFrm = pFrame;
	}
	else if( szType == "ScheduleOpenGray" )
	{
		SAFEDELETE( m_pScheduleOpenGray );
		m_pScheduleOpenGray = pFrame;
	}
	else if( szType == "ScheduleOpenOrange" )
	{
		SAFEDELETE( m_pScheduleOpenOrange );
		m_pScheduleOpenOrange = pFrame;
	}
	else if( szType == "ScheduleOpenPink" )
	{
		SAFEDELETE( m_pScheduleOpenPink );
		m_pScheduleOpenPink = pFrame;
	}
	else if( szType == "ScheduleCloseGray" )
	{
		SAFEDELETE( m_pScheduleCloseGray );
		m_pScheduleCloseGray = pFrame;
	}
	else if( szType == "ScheduleCloseGreen" )
	{
		SAFEDELETE( m_pScheduleCloseGreen );
		m_pScheduleCloseGreen = pFrame;
	}
	else if( szType == "ScheduleCloseOrange" )
	{
		SAFEDELETE( m_pScheduleCloseOrange );
		m_pScheduleCloseOrange = pFrame;
	}
	else 
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void TournamentMainWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HorzLine" )
	{
		SAFEDELETE( m_pHorzLine );
		m_pHorzLine = pImage;
	}
	else if( szType == "VertLine" )
	{
		SAFEDELETE( m_pVertLine );
		m_pVertLine = pImage;
	}
	else if( szType == "TextBack" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else if( szType == "TodayMark" )
	{
		SAFEDELETE( m_pTodayMark );
		m_pTodayMark = pImage;
	}
	else if( szType == "MyTeamArrow" )
	{
		SAFEDELETE( m_pMyTeamArrow );
		m_pMyTeamArrow = pImage;
		m_pMyTeamArrow->SetScale( FLOAT05, FLOAT05 );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_TOURNAMENT_INFO:
		if( cmd == IOBN_BTNUP )
		{
			TournamentDetailMainWnd *pInfoWnd = dynamic_cast< TournamentDetailMainWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_INFO ) );
			if( pInfoWnd )
			{				
				pInfoWnd->SetTournamentData( m_dwSelectIndex );
			}
		}
		break;
	case ID_TOURNAMENT_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			IwmCommandEnterPlayer( m_dwSelectIndex );
		}
		break;
	case ID_TOURNAMENT_VIEW:
		if( cmd == IOBN_BTNUP )
		{
			TournamentDetailMainWnd *pInfoWnd = dynamic_cast< TournamentDetailMainWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_INFO ) );
			if( pInfoWnd )
			{
				pInfoWnd->SetTournamentData( m_dwSelectIndex );
			}
		}
		break;
	case ID_TOURNAMENT_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendTournamentList( m_iTournamentPage - 1 );
		}
		break;
	case ID_TOURNAMENT_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendTournamentList( m_iTournamentPage + 1 );
		}
		break;
	case ID_TOURNAMENT_BTN1:
	case ID_TOURNAMENT_BTN2:
	case ID_TOURNAMENT_BTN3:
	case ID_TOURNAMENT_BTN4:
	case ID_TOURNAMENT_BTN5:
	case ID_TOURNAMENT_BTN6:
	case ID_TOURNAMENT_BTN7:
	case ID_TOURNAMENT_BTN8:
	case ID_TOURNAMENT_BTN9:
	case ID_TOURNAMENT_BTN10:
	case ID_TOURNAMENT_BTN11:
	case ID_TOURNAMENT_BTN12:
	case ID_TOURNAMENT_BTN13:
		if( cmd == IOBN_BTNUP )
		{
			TournamentListBtn *pTournament = dynamic_cast< TournamentListBtn * >( pWnd );
			if( pTournament && pTournament->GetIndex() != 0 )
			{
				if( m_dwSelectIndex != pTournament->GetIndex() )
				{
					m_dwSelectIndex = pTournament->GetIndex();
					UpdateMainUI();
				}
			}
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_TOURNAMENT_JOIN_CONFIRM_REQUEST );
				kPacket << m_dwSelectIndex;
				kPacket << m_dwOwnerIdx;
				TCPNetwork::SendToServer( kPacket );
				g_MyInfo.AddTournamentConfirmRequest( m_dwSelectIndex );
			}
		}
		break;
	}
}

void TournamentMainWnd::IwmCommandEnterPlayer( DWORD dwTourIndex )
{
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsTourTeam( dwTourIndex ) )
		{
			ShowTeamInfo( dwTourIndex );
		}
		else
		{
			IwmCommandTeamCreate( dwTourIndex );
		}
	}
}

void TournamentMainWnd::ShowTeamInfo( DWORD dwTourIndex )
{
	// 팀 정보 창
	TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
	if( pTeamInfo )
	{
		ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
		if( pUserTournament )
		{
			DWORD dwTeamIndex = pUserTournament->GetTeamIndex( dwTourIndex );
			if( dwTeamIndex != 0 )
				pTeamInfo->SetTournamentTeam( m_dwSelectIndex, dwTeamIndex );
		}
	}	
}

void TournamentMainWnd::ShowTeamEntryList( DWORD dwTourIndex )
{
	CustomTournamentEntryListWnd* pWnd = dynamic_cast<CustomTournamentEntryListWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_ENTRY_LIST_WND ) );
	if( pWnd )
		pWnd->SetTournament( dwTourIndex );
}

void TournamentMainWnd::IwmCommandTeamCreate( DWORD dwTourIndex )
{	
	if( m_dwOwnerIdx == g_MyInfo.GetUserIndex() )
	{
		ShowTeamEntryList( dwTourIndex );
	}
	else
	{
		if( m_State != TournamentNode::STATE_TEAM_APP ) return;
		if( m_Type == TournamentNode::TYPE_CUSTOM && m_CustomProgressType == TournamentNode::CPT_OFFLINE )
		{
			IwmTournamentConfirmCheck( dwTourIndex );
		}
		else
		{
			TournamentTeamCreateWnd *pCreateWnd = dynamic_cast< TournamentTeamCreateWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_CREATE ) );
			if( pCreateWnd )
				pCreateWnd->SetTournamentIndex( dwTourIndex );
		}
	}	
}

void TournamentMainWnd::IwmTournamentConfirmCheck( DWORD dwTourIndex )
{
	if( g_MyInfo.IsTournamentConfirm( dwTourIndex ) )
	{
		TournamentTeamCreateWnd *pCreateWnd = dynamic_cast< TournamentTeamCreateWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_CREATE ) );
		if( pCreateWnd )			
			pCreateWnd->SetTournamentIndex( dwTourIndex );
	}
	else
	{
		if( g_MyInfo.IsTournamentConfirmRequest( dwTourIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 팀 생성 권한을 요청한 대회입니다. # 5분간 재요청이 불가능합니다." );
		}
		else
		{
			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_TOURNAMENT_JOIN_CONFIRM_CHECK );
			kPacket << m_dwSelectIndex;
			TCPNetwork::SendToServer( kPacket );			
		}
	}
}

void TournamentMainWnd::SetTournamentLink( DWORD dwTourIndex )
{
	m_dwWantTourIndex = dwTourIndex;
}

bool TournamentMainWnd::IsStateEndToday()
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	if( (DWORD)st.wMonth == m_StateEndMonth && 
		(DWORD)st.wDay == m_StateEndDay )
	{
		return true;
	}
	return false;
}

void TournamentMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	OnCheckRegularTournament();
	OnScreenProcess();
	OnUpdateProcess();
	OnListSyncProcess();
	OnMyTeamArrowProcess();
}

void TournamentMainWnd::OnCheckRegularTournament()
{
	g_TournamentMgr.CheckRegularTournaemntRequest();
}

void TournamentMainWnd::OnScreenProcess()
{
	if( FRAMEGETTIME() - m_dwScreenTimer > 250 )
	{
		m_dwScreenTimer = FRAMEGETTIME();
		m_bScreen = !m_bScreen;
	}
}

void TournamentMainWnd::OnMyTeamArrowProcess()
{
	if( m_dwMyTeamArrowTimer != 0 && FRAMEGETTIME() - m_dwMyTeamArrowTimer < 300 )
	{
		float fGapTime = min( FLOAT1, (float)(FRAMEGETTIME() - m_dwMyTeamArrowTimer) / 300.0f );
		m_fArrowYPos = sin(fGapTime) * 9.0f;
	}
	else if( m_dwMyTeamArrowTimer != 0 && FRAMEGETTIME() - m_dwMyTeamArrowTimer < 600 )
	{
		float fGapTime = min( FLOAT1, (float)(FRAMEGETTIME() - m_dwMyTeamArrowTimer - 300) / 300.0f );
		m_fArrowYPos = sin(1.0f - fGapTime) * 9.0f;
	}
	else
	{
		m_dwMyTeamArrowTimer = FRAMEGETTIME();
		m_fArrowYPos = 0;
	}
}

void TournamentMainWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament )
	{
		if( pTournament->IsUpdateMainUI() )
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

void TournamentMainWnd::OnListSyncProcess()
{
	if( FRAMEGETTIME() - m_dwTournamentRequestTime > TOURNAMENT_REQUEST_TIME )
	{
		SendTournamentList( m_iTournamentPage );
	}
}

void TournamentMainWnd::UpdateMainUI()
{
	HideChildWnd( ID_TOURNAMENT_INFO );
	HideChildWnd( ID_TOURNAMENT_ENTER );
	HideChildWnd( ID_TOURNAMENT_VIEW );
	
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament == NULL ) return;

	m_Type				 = pTournament->GetType();
	m_State				 = pTournament->GetState();
	m_CustomProgressType = pTournament->GetCustomProgressType();
	m_dwOwnerIdx		 = pTournament->GetOwnerIndex();

	m_StateEndMonth = DateHelp::GetDateMonth( pTournament->GetStateEndDate() );
	m_StateEndDay   = DateHelp::GetDateDay( pTournament->GetStateEndDate() );
	m_StateEndHour  = DateHelp::GetDateHour( pTournament->GetStateEndDate() );
	m_StateEndMinute= DateHelp::GetDateMinute( pTournament->GetStateEndDate() );

	m_pBanner = pTournament->GetResBanner();              
	m_TournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
	m_TournamentMaxRound       = pTournament->GetTournamentMaxRound();
	m_szTournamentTitle        = pTournament->GetTournamentTitle();
	m_szPlayModeName		   = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pTournament->GetPlayMode() ).c_str();
	m_MaxPlayer				   = pTournament->GetMaxPlayer();

	switch( m_State )
	{
	case TournamentNode::STATE_TEAM_APP:
		{
			ShowChildWnd( ID_TOURNAMENT_INFO );
			ShowChildWnd( ID_TOURNAMENT_ENTER );

			ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
			if( pUserTournament )
			{
				ioButton *pButton = dynamic_cast< ioButton * >( FindChildWnd( ID_TOURNAMENT_ENTER ) );
				if( pButton )
				{
					if( pTournament->GetType() == TournamentNode::TYPE_CUSTOM && 
						pTournament->GetOwnerIndex() == g_MyInfo.GetUserIndex() )
					{
						//
						DWORD dwTournamentCurEntryCount = pTournament->GetExpectedTeamCount();
						char szTemp[MAX_PATH] = "";
						sprintf_s_e( szTemp, "참가현황보기 (%d팀)", dwTournamentCurEntryCount );
						pButton->SetTitleText( szTemp );
					}
					else
					{
						if( pUserTournament->IsTourTeam( m_dwSelectIndex ) )
						{
							int iCurrentUser = (int)m_MaxPlayer - pUserTournament->GetCurrentTeamUserCount( m_dwSelectIndex );
							if( iCurrentUser == 0 )
								pButton->SetTitleText( "팀정보 (팀결성 완료)" );
							else
							{
								char szTemp[MAX_PATH] = "";
								sprintf_s_e( szTemp, "팀정보 (%d인 부족)", iCurrentUser );
								pButton->SetTitleText( szTemp );
							}
						}
						else
						{
							pButton->SetTitleText( "대회 참가하기" );
						}
					}
				}
			}
		}
		break;
	case TournamentNode::STATE_TEAM_DELAY:
		{
			ShowChildWnd( ID_TOURNAMENT_VIEW );

			ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
			if( pUserTournament )
			{
				ioButton *pButton = dynamic_cast< ioButton * >( FindChildWnd( ID_TOURNAMENT_VIEW ) );
				if( pButton )
				{
					if( pTournament->GetType() == TournamentNode::TYPE_CUSTOM && pTournament->GetOwnerIndex() == g_MyInfo.GetUserIndex() )
					{
						pButton->SetTitleText( "나의 대회 보기" );
					}
					else
					{
						if( pUserTournament->IsTourTeam( m_dwSelectIndex ) )
							pButton->SetTitleText( "대회 입장" );
						else
							pButton->SetTitleText( "대회 보기" );
					}
				}
			}
		}
		break;
	default:
		{
			ShowChildWnd( ID_TOURNAMENT_VIEW );

			ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
			if( pUserTournament )
			{
				ioButton *pButton = dynamic_cast< ioButton * >( FindChildWnd( ID_TOURNAMENT_VIEW ) );
				if( pButton )
				{
					if( pTournament->GetType() == TournamentNode::TYPE_CUSTOM && pTournament->GetOwnerIndex() == g_MyInfo.GetUserIndex() )
					{
						pButton->SetTitleText( "나의 대회 보기" );
					}
					else
					{
						if( pUserTournament->IsTourTeam( m_dwSelectIndex ) )
							pButton->SetTitleText( "대회 입장" );
						else
							pButton->SetTitleText( "대회 보기" );
					}
				}
			}
		}
		break;
	}
}

void TournamentMainWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnMainBannerRender( iXPos, iYPos );
	OnExplainRender( iXPos, iYPos );
	OnMyTeamArrowRender();

	if( m_Type == TournamentNode::TYPE_CUSTOM && m_CustomProgressType == TournamentNode::CPT_OFFLINE )
	{
		OnOfflineScheduleRender( iXPos, iYPos );
	}
	else
	{
		OnScheduleRender( iXPos, iYPos );
	}

	//
	if( m_pMainBannerFrm )
	{
		m_pMainBannerFrm->Render( iXPos + 10, iYPos + 39 );
	}
}

void TournamentMainWnd::OnMainBannerRender( int iXPos, int iYPos )
{
	if( m_pBanner )
	{
		m_pBanner->Render( iXPos + 13, iYPos + 42 );
	}
}

void TournamentMainWnd::OnExplainRender( int iXPos, int iYPos )
{
	if( m_pTextBack )
	{
		m_pTextBack->Render( iXPos + 13, iYPos + 313 );
	}

	if( m_pHorzLine )
	{
		m_pHorzLine->Render( iXPos + 13, iYPos + 312 );
	}

	if( m_pVertLine )
	{
		m_pVertLine->Render( iXPos + 140, iYPos + 323 );
		m_pVertLine->Render( iXPos + 262, iYPos + 323 );
	}

	if( IsStateEndToday() )
	{
		if( m_pTodayMark )
		{
			m_pTodayMark->Render( iXPos + 296, iYPos + 305 );

			if( m_bScreen )
				m_pTodayMark->Render( iXPos + 296, iYPos + 305, UI_RENDER_SCREEN );
		}
	}

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 79, iYPos + 328, FONT_SIZE_12, "대회규모" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 180, 0 );
	g_FontMgr.PrintText( iXPos + 79, iYPos + 342, FONT_SIZE_12, "%d강 (%d회전)", (int)m_TournamentStartTeamCount, (int)m_TournamentMaxRound );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 201, iYPos + 328, FONT_SIZE_12, "대회종류" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 180, 0 );
	g_FontMgr.PrintText( iXPos + 201, iYPos + 342, FONT_SIZE_12, "%s %d인", m_szPlayModeName.c_str(), m_MaxPlayer );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	if( m_State == TournamentNode::STATE_TEAM_APP )
		g_FontMgr.PrintText( iXPos + 323, iYPos + 328, FONT_SIZE_12, "모집마감" );		
	else if( m_State == TournamentNode::STATE_TEAM_DELAY )
		if( m_Type == TournamentNode::TYPE_REGULAR )
			g_FontMgr.PrintText( iXPos + 323, iYPos + 328, FONT_SIZE_12, "응원마감" );
		else
			g_FontMgr.PrintText( iXPos + 323, iYPos + 328, FONT_SIZE_12, "대진배정" );
	else if( m_State == TournamentNode::STATE_WAITING )
		g_FontMgr.PrintText( iXPos + 323, iYPos + 328, FONT_SIZE_12, "대회종료" );
	else
	{
		int iCurrentTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, m_State - TournamentNode::STATE_TOURNAMENT );
		if( iCurrentTeam <= 2 )
			g_FontMgr.PrintText( iXPos + 323, iYPos + 328, FONT_SIZE_12, "결승" );
		else
			g_FontMgr.PrintText( iXPos + 323, iYPos + 328, FONT_SIZE_12, "%d강", (int)iCurrentTeam );
	}

	if( m_Type == TournamentNode::TYPE_CUSTOM && m_CustomProgressType == TournamentNode::CPT_OFFLINE )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 180, 0 );
		g_FontMgr.PrintText( iXPos + 323, iYPos + 342, FONT_SIZE_12, "수동진행" );
	}
	else
	{	
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 180, 0 );
		g_FontMgr.PrintText( iXPos + 323, iYPos + 342, FONT_SIZE_12, "%02d/%02d %02d:%02d", (int)m_StateEndMonth, (int)m_StateEndDay, (int)m_StateEndHour, (int)m_StateEndMinute );
	}
}

void TournamentMainWnd::OnScheduleRender( int iXPos, int iYPos )
{
	ioUIRenderFrame *pScheduleBar1 = NULL;
	ioUIRenderFrame *pScheduleBar2 = NULL;
	ioUIRenderFrame *pScheduleBar3 = NULL;

	ioUIRenderFrame *pScreenScheduleBar1 = NULL;
	ioUIRenderFrame *pScreenScheduleBar2 = NULL;
	ioUIRenderFrame *pScreenScheduleBar3 = NULL;

	switch( m_State )
	{
	case TournamentNode::STATE_TEAM_APP:
		{
			pScheduleBar1 = m_pScheduleOpenOrange;
			pScheduleBar2 = m_pScheduleOpenGray;
			pScheduleBar3 = m_pScheduleCloseGray;

			if( m_bScreen )
				pScreenScheduleBar1 = pScheduleBar1;
		}
		break;
	case TournamentNode::STATE_TEAM_DELAY:
		{
			pScheduleBar1 = m_pScheduleOpenGray;
			if( m_Type == TournamentNode::TYPE_REGULAR )
				pScheduleBar2 = m_pScheduleOpenPink;
			else
				pScheduleBar2 = m_pScheduleOpenOrange;

			pScheduleBar3 = m_pScheduleCloseGray;

			if( m_bScreen )
				pScreenScheduleBar2 = pScheduleBar2;
		}
		break;
	default:
		{
			pScheduleBar1 = m_pScheduleOpenGray;
			pScheduleBar2 = m_pScheduleOpenGray;
			pScheduleBar3 = m_pScheduleCloseGreen;

			if( m_bScreen )
				pScreenScheduleBar3 = pScheduleBar3;
		}
		break;
	}

	if( pScheduleBar3 )
	{
		pScheduleBar3->SetSize( 99, 17 );
		pScheduleBar3->Render( iXPos + 287, iYPos + 369 );

		if( pScreenScheduleBar3 )
			pScreenScheduleBar3->Render( iXPos + 287, iYPos + 369, UI_RENDER_SCREEN );
	}

	if( pScheduleBar2 )
	{
		pScheduleBar2->SetSize( 102, 17 );
		pScheduleBar2->Render( iXPos + 196, iYPos + 369 );

		if( pScreenScheduleBar2 )
			pScreenScheduleBar2->Render( iXPos + 196, iYPos + 369, UI_RENDER_SCREEN );
	}

	if( pScheduleBar1 )
	{
		pScheduleBar1->SetSize( 190, 17 );
		pScheduleBar1->Render( iXPos + 17, iYPos + 369 );

		if( pScreenScheduleBar1 )
			pScreenScheduleBar1->Render( iXPos + 17, iYPos + 369, UI_RENDER_SCREEN );
	}

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	if( m_Type == TournamentNode::TYPE_REGULAR )
		g_FontMgr.PrintTextWidthCut( iXPos + 111, iYPos + 371, FONT_SIZE_11, 173.0f, "%s 정규 대회 모집", m_szTournamentTitle.c_str() );
	else
		g_FontMgr.PrintTextWidthCut( iXPos + 111, iYPos + 371, FONT_SIZE_11, 173.0f, "%s 대회 모집", m_szTournamentTitle.c_str() );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	if( m_Type == TournamentNode::TYPE_REGULAR )
		g_FontMgr.PrintText( iXPos + 248, iYPos + 371, FONT_SIZE_11, "응원기간" );
	else
		g_FontMgr.PrintText( iXPos + 248, iYPos + 371, FONT_SIZE_11, "대진발표" );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 338, iYPos + 371, FONT_SIZE_11, "대회기간" );
}

void TournamentMainWnd::OnOfflineScheduleRender( int iXPos, int iYPos )
{
	if( m_pScheduleCloseOrange )
	{
		m_pScheduleCloseOrange->SetSize( 369, 17 );
		m_pScheduleCloseOrange->Render( iXPos + 17, iYPos + 369 );	
	}

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintTextWidthCut( iXPos + 218, iYPos + 371, FONT_SIZE_11, 355.0f, "%s 대회(오프라인형)", m_szTournamentTitle.c_str() );
	
}

void TournamentMainWnd::OnMyTeamArrowRender()
{
	if( !m_pMyTeamArrow )
		return;

	ioWndList::iterator iter = m_TournamentBtnSet.begin();
	for(;iter != m_TournamentBtnSet.end();++iter)
	{
		TournamentListBtn *pBtn = dynamic_cast< TournamentListBtn * >( *iter );
		if( pBtn && pBtn->GetOwnerIndex() == g_MyInfo.GetUserIndex())
		{
			int iXPos = pBtn->GetDerivedPosX();
			int iYPos = pBtn->GetDerivedPosY();						
			m_pMyTeamArrow->Render( iXPos + 13, iYPos - (18 + m_fArrowYPos) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
LobbyTournamentInfoWnd::LobbyTournamentInfoWnd() : m_dwEtcItem( ioEtcItem::EIT_ETC_TOURNAMENT_CREATE ), m_dwPremiumEtcItem( ioEtcItem::EIT_ETC_TOURNAMENT_PREMIUM_CREATE )
{
	m_iCurPage = m_iMaxPage = 0;	
}

LobbyTournamentInfoWnd::~LobbyTournamentInfoWnd()
{

}

void LobbyTournamentInfoWnd::UpdateTournamentTeam( int iCurPage )
{
	m_TeamDataList.clear();

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		pUserTournament->GetTeamList( m_TeamDataList );
	}

	m_iMaxPage = max( 0, (int)m_TeamDataList.size() - 1 ) / MAX_PAGE_COUNT;
	m_iCurPage = min( max( 0, iCurPage ), m_iMaxPage );
	
	int iStartPos = m_iCurPage * MAX_PAGE_COUNT;
	for(int i = ID_TEAM_BTN1;i < ID_TEAM_BTN3 + 1;i++,iStartPos++)
	{
		LobbyTournamentTeamBtn *pTeamBtn = dynamic_cast< LobbyTournamentTeamBtn * >( FindChildWnd( i ) );
		if( pTeamBtn == NULL ) continue;

		if( iStartPos < (int)m_TeamDataList.size() )
		{
			TeamData &rkData = m_TeamDataList[iStartPos];
			pTeamBtn->SetTournamentTeam( rkData.m_dwTourIndex, rkData.m_dwTeamIndex );
		}
		else
		{
			pTeamBtn->SetTournamentTeam( 0, 0 );
		}
	}
}

void LobbyTournamentInfoWnd::iwm_show()
{
	ShowChildWnd( ID_TEAM_BTN1 );
	ShowChildWnd( ID_TEAM_BTN2 );
	ShowChildWnd( ID_TEAM_BTN3 );

	m_iCurPage = m_iMaxPage = 0;
	UpdateTournamentTeam( m_iCurPage );
}

void LobbyTournamentInfoWnd::iwm_hide()
{
	HideChildWnd( ID_TEAM_BTN1 );
	HideChildWnd( ID_TEAM_BTN2 );
	HideChildWnd( ID_TEAM_BTN3 );
}

void LobbyTournamentInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_TEAM_BTN1:
	case ID_TEAM_BTN2:
	case ID_TEAM_BTN3:
		if( cmd == IOBN_BTNUP )
		{
			LobbyTournamentTeamBtn *pTeamBtn = dynamic_cast< LobbyTournamentTeamBtn * >( pWnd );
			if( pTeamBtn )
			{
				// 팀 정보 창
				TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
				if( pTeamInfo )
				{
					pTeamInfo->SetTournamentTeam( pTeamBtn->GetTournamentIndex(), pTeamBtn->GetTournamentTeamIndex() );
				}
			}
		}
		break;
	case ID_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateTournamentTeam( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateTournamentTeam( m_iCurPage + 1 );
		}
		break;
	case ID_TOURNAMENT_CREATE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			CreateTournament();
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 만들기 기능은#차후 추가될 예정입니다" );
		}
		break;
	}
}

void LobbyTournamentInfoWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioWnd::AddRenderFrame( szType, pFrame );
}

void LobbyTournamentInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );
}

void LobbyTournamentInfoWnd::CreateTournament()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem == NULL ) return;

	ioUserEtcItem::ETCITEMSLOT kCreateEtc;				 // 일반
	ioUserEtcItem::ETCITEMSLOT kPremiumCreateEtc;		 // 프리미엄

	if( !pUserEtcItem->GetEtcItem( m_dwEtcItem, kCreateEtc ) && 
		!pUserEtcItem->GetEtcItem( m_dwPremiumEtcItem, kPremiumCreateEtc ) )
	{
		// 둘다 없으면 필요 팝업
		g_GUIMgr.ShowWnd( TOURNAMENT_ETCITEM_NEED_WND );
	}
	else
	{
		// 둘중 하나라도 있으면 사용 팝업
		g_GUIMgr.ShowWnd( TOURNAMENT_ETCITEM_USE_WND );
	}
}

void LobbyTournamentInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsUpdateSubUI() )
		{
			UpdateTournamentTeam( m_iCurPage );
		}
	}
}

void LobbyTournamentInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 425, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
LobbyTournamentTeamBtn::LobbyTournamentTeamBtn()
{
	m_dwTeamIndex = 0;
	m_dwTournamentIndex = 0;

	m_pSmallBanner = NULL;

	m_Type = 0;
	m_State= 0;
	m_CampPos = 0;
	m_TourPos = 0;
	m_Position= 0;
	m_StateEndMonth = 0;
	m_StateEndDay   = 0;
	m_StateEndHour  = 0;
	m_StateEndMinute= 0;	
	m_iCurPlayer	= 0;
	m_MaxPlayer		= 0;
	m_TournamentMaxRound       = 0;
	m_TournamentStartTeamCount = 0;

	m_dwLoadingCommaTimer = 0;
	m_iLoadingCommaCount  = 0;
}

LobbyTournamentTeamBtn::~LobbyTournamentTeamBtn()
{

}

void LobbyTournamentTeamBtn::iwm_show()
{
}

void LobbyTournamentTeamBtn::iwm_hide()
{

}

void LobbyTournamentTeamBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_TOURNAMENT_BNT:
		if( cmd == IOBN_BTNUP )
		{
			TournamentDetailMainWnd *pInfoWnd = dynamic_cast< TournamentDetailMainWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_INFO ) );
			if( pInfoWnd )
			{				
				pInfoWnd->SetTournamentData( m_dwTournamentIndex );
			}
		}
		break;
	}
}

void LobbyTournamentTeamBtn::SetTournamentTeam( DWORD dwTourIndex, DWORD dwTeamIndex )
{
	m_pSmallBanner= NULL;
	m_dwTeamIndex = dwTeamIndex;
	m_dwTournamentIndex = dwTourIndex;

	if( m_dwTournamentIndex == 0 || m_dwTeamIndex == 0 )
	{
		SetInActive();
		HideChildWnd( ID_TOURNAMENT_BNT );
	}
	else
	{
		SetActive();
		ShowChildWnd( ID_TOURNAMENT_BNT );

		UpdateTeamData();
	}
}

void LobbyTournamentTeamBtn::UpdateTeamData()
{
	m_Type = 0;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		m_Type = pTournament->GetType();
		if( m_Type == TournamentNode::TYPE_REGULAR )		
			m_pSmallBanner = pTournament->GetResBannerSmall();
		else
			m_pSmallBanner = pTournament->GetResBannerMark();

		//		
		m_State= pTournament->GetState();
		m_StateEndMonth = DateHelp::GetDateMonth( pTournament->GetStateEndDate() );
		m_StateEndDay   = DateHelp::GetDateDay( pTournament->GetStateEndDate() );
		m_StateEndHour  = DateHelp::GetDateHour( pTournament->GetStateEndDate() );
		m_StateEndMinute= DateHelp::GetDateMinute( pTournament->GetStateEndDate() );
		m_MaxPlayer		= pTournament->GetMaxPlayer();
		m_szTournamentTitle = pTournament->GetTournamentTitle();
		m_TournamentMaxRound       = pTournament->GetTournamentMaxRound();
		m_TournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();

		TeamGlobalData *pGlobalTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex );
		if( pGlobalTeamData )
		{
			m_CampPos		= pGlobalTeamData->GetCampPos();
			m_TourPos       = pGlobalTeamData->GetTourPos();
			m_Position		= pGlobalTeamData->GetPosition();
			m_iCurPlayer	= pGlobalTeamData->GetUserSize();
			m_szTeamName    = pGlobalTeamData->GetTeamName();
		}
	}
	else
	{
		g_TournamentMgr.CreateTournament( m_dwTournamentIndex );
	}
}

void LobbyTournamentTeamBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioButton::AddRenderFrame( szType, pFrame );
}

void LobbyTournamentTeamBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioButton::AddRenderImage( szType, pImage );
}

void LobbyTournamentTeamBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( GetParent() && !GetParent()->IsShow() ) return;

	OnUpdateProcess();
	OnLoadingProcess();
}

void LobbyTournamentTeamBtn::OnUpdateProcess()
{
	if( m_dwTournamentIndex == 0 ) return;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		if( pTournament->IsUpdateSubUI() )
		{
			UpdateTeamData();
		}
	}

	if( m_dwTeamIndex == 0 ) return;

	TeamGlobalData *pGlobalTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex );
	if( pGlobalTeamData )
	{
		if( pGlobalTeamData->IsUpdateSubUI() )
		{
			UpdateTeamData();
		}
	}
}

void LobbyTournamentTeamBtn::OnLoadingProcess()
{
	if( m_Type == 0 )
	{
		if( FRAMEGETTIME() - m_dwLoadingCommaTimer > 500 )
		{
			m_dwLoadingCommaTimer = FRAMEGETTIME();
			m_iLoadingCommaCount++;
		}
	}
}

void LobbyTournamentTeamBtn::OnRender()
{
	ioButton::OnRender();

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		int iXPos = GetDerivedPosX() + GetWidth() / 2;
		int iYPos = GetDerivedPosY() + (GetHeight() / 2) - 14;

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 128, 128, 128 );
		g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_13, "참가중인 대회가" );
		g_FontMgr.PrintText( iXPos, iYPos + 14, FONT_SIZE_13, "없습니다" );
	}
	else
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();

		if( m_pSmallBanner )
			m_pSmallBanner->Render( iXPos + 7, iYPos + 7 );

		OnRenderData( iXPos, iYPos );
	}
}

void LobbyTournamentTeamBtn::OnRenderData( int iXPos, int iYPos )
{
	if( m_Type == 0 )
	{
		// 정보를 가져오는 중입니다.
		char szTemp[MAX_PATH] = "";
		for(int i = 0;i < m_iLoadingCommaCount % 3;i++)
			strcat_s( szTemp, " ." );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 38, iYPos + 20, FONT_SIZE_13, "팀정보 요청중%s",szTemp );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		switch( m_Type )
		{
		case TournamentNode::TYPE_REGULAR:
			{
				if( m_CampPos == CAMP_BLUE )
					g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
				else
					g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
				g_FontMgr.PrintTextWidthCut( iXPos + 38, iYPos + 6, FONT_SIZE_13, 127.0f, "%s 정규 대회팀", m_szTournamentTitle.c_str() );
			}
			break;
		case TournamentNode::TYPE_CUSTOM:
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintTextWidthCut( iXPos + 38, iYPos + 6, FONT_SIZE_13, 127.0f, "%s 대회팀", m_szTournamentTitle.c_str() );
			break;
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 38, iYPos + 20, FONT_SIZE_13, 127.0f, "%s", m_szTeamName.c_str() );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		switch( m_State )
		{
		case TournamentNode::STATE_WAITING:
			{
				if( m_TourPos == 0 || m_Position == 0 )
					g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 탈락" );
				else 
				{
					int iCurrentTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, m_TourPos - 1 );
					if( iCurrentTeam == 1 )
						g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 우승!!" );
					else if( iCurrentTeam == 2 )
						g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 준우승!!" );
					else
						g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ %d강 달성!!", iCurrentTeam );
				}
				g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 더 이상 경기가 없습니다" );
			}
			break;
		case TournamentNode::STATE_TEAM_APP:		// 팀 모집중
			{
				int iUserGap = max( 0, (int)m_MaxPlayer - m_iCurPlayer );
				if( iUserGap == 0 )
					g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 팀결성 완료" );
				else
					g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ %d인 부족", iUserGap );

				if( m_StateEndMonth == 0 && m_StateEndDay == 0 && m_StateEndHour == 0 && m_StateEndMinute == 0)
				{
					g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 주최자 수동진행" );
				}
				else
				{
					g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 모집마감 %02d/%02d %02d:%02d", 
						(int)m_StateEndMonth, (int)m_StateEndDay, (int)m_StateEndHour, (int)m_StateEndMinute );
				}
			}
			break;
		case TournamentNode::STATE_TEAM_DELAY:         // 응원중
			{
				int iUserGap = max( 0, (int)m_MaxPlayer - m_iCurPlayer );
				if( iUserGap == 0 )
					g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 팀결성 완료" );
				else
					g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ %d인 부족", iUserGap );

				if( m_StateEndMonth == 0 && m_StateEndDay == 0 && m_StateEndHour == 0 && m_StateEndMinute == 0)
				{
					g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 주최자 수동진행" );
				}
				else
				{
					g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 응원마감 %02d/%02d %02d:%02d", 
										 (int)m_StateEndMonth, (int)m_StateEndDay, (int)m_StateEndHour, (int)m_StateEndMinute );
				}
			}
			break;
		default:		// 토너먼트 중
			{
				bool bEndTeam = false;
				int iCurrentStatePos = (int)m_State - TournamentNode::STATE_TOURNAMENT;
				if( m_State == TournamentNode::STATE_TOURNAMENT && m_TourPos == 0 )
				{
					g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 예비엔트리" );
				}
				else if( (int)m_TourPos < iCurrentStatePos + 1 )
				{
					bEndTeam = true;
					if( m_Position == 0 )            
					{
						g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 탈락" );
					}
					else
					{
						int iCurrentTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, m_TourPos - 1 );
						if( iCurrentTeam == 1 )
							g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 우승!!" );
						else if( iCurrentTeam == 2 )
							g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 준우승!!" );
						else
							g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ %d강 달성!!", iCurrentTeam );
					}
				}
				else 
				{
					int iCurrentTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, iCurrentStatePos );
					if( iCurrentTeam <= 2 )
						g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ 결승전" );
					else
						g_FontMgr.PrintText( iXPos + 8, iYPos + 41, FONT_SIZE_13, "★ %d강", iCurrentTeam );
				}

				if( bEndTeam )
				{
					g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 더 이상 경기가 없습니다" );
				}
				else
				{
					if( m_StateEndMonth == 0 && m_StateEndDay == 0 && m_StateEndHour == 0 && m_StateEndMinute == 0)
					{
						g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ 주최자 수동진행" );
					}
					else
					{
						g_FontMgr.PrintText( iXPos + 8, iYPos + 55, FONT_SIZE_13, "★ %02d/%02d %02d:%02d", 
							(int)m_StateEndMonth, (int)m_StateEndDay, (int)m_StateEndHour, (int)m_StateEndMinute );
					}
				}
			}
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamCreateWnd::TournamentTeamCreateWnd()
{
	m_pIcon      = NULL;
	m_pIconBack  = NULL;

	m_pPreEdit   = NULL;
	m_pNameEdit  = NULL;

	m_MaxPlayer  = 0;
	m_dwTournamentIndex = 0;
}

TournamentTeamCreateWnd::~TournamentTeamCreateWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentTeamCreateWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamCreateWnd::SetTournamentIndex( DWORD dwIndex )
{
	m_dwTournamentIndex = dwIndex;

	if( m_dwTournamentIndex == 0 ) return;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL )
	{
		return;
	}
	
	m_MaxPlayer = pTournament->GetMaxPlayer();
	
	ShowWnd();
}

void TournamentTeamCreateWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_TEAM_NAME_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{
		m_pNameEdit = pEdit;
		InitEditWnd();
	}
}

void TournamentTeamCreateWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

bool TournamentTeamCreateWnd::IsOnlyEngHanNumText( const char *szText )
{
	int iSize = strlen( szText );
	if( iSize < 1 )	return false;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal ) 
		return false;

	for(int i = 0;i < iSize;i++)
	{
		if( !COMPARE( szText[i], 'A', 'Z'+1) && !COMPARE( szText[i], 'a', 'z'+1 ) && !COMPARE( szText[i], '0', '9'+1 ) && szText[i] != '-' )
		{
			if( i < iSize - 1 )
			{
				if( pLocal->IsCheckKorean() )
				{
					// 한글 깨진 경우
					if( (byte)szText[i] == 0xa4 && (byte)szText[i+1] >= 0xa1 && (byte)szText[i+1] <= 0xd3 )
					{
						return false;
					}
					if( (byte)szText[i] >= 0xb0 && (byte)szText[i] <= 0xc8 && (byte)szText[i+1] >= 0xa1 && (byte)szText[i+1] <= 0xfe )
					{
						i++;
						continue;
					}
				}
				else
				{
#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
					if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (byte)szText[i] ) )
#else
					if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (byte)szText[i] ) )
#endif

#else
					if( IsDBCSLeadByte( (byte)szText[i] ) )
#endif
					{
						i++;
						continue;
					}
				}
			}
			return false;
		}
	}
	return true;
}

void TournamentTeamCreateWnd::InitEditWnd()
{
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit == NULL ) return;

	pEdit->ClearString();
	pEdit->SetKeyFocus();
	g_InputBox.SetString( "" );
	pEdit->KillKeyFocus();
}

void TournamentTeamCreateWnd::SendTeamCreate()
{	
	if( g_App.IsMouseBusy() )
		return;

	if( ioWnd::m_pTopModalWnd != this )
		return;
	
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택한 대회가 존재하지 않습니다" );	
		return;
	}

	if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
	{
		if( g_MyInfo.GetUserCampPos() == CAMP_NONE )
		{
			HideWnd();
			g_GUIMgr.ShowWnd( CAMP_JOIN_NEED_POPUP );		
			g_TournamentMgr.SetRegularTeamCreateCampFailed();
			return;
		}
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsTourTeam( m_dwTournamentIndex ) )
		{
			HideWnd();
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택한 대회에 팀이 존재합니다" );	
			return;
		}
	}
	
	ioHashString szTeamName;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit )
	{
		szTeamName = pEdit->GetText();
	}
	else
		return;

	if( szTeamName.IsEmpty() )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀 이름을 입력하세요" );		
		return;
	}
	
	if( !g_App.IsRightID( szTeamName.c_str() ) )
	{	
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 팀 이름입니다" );		
		return;
	}

	ioHashString szNotMakeString;
	if( g_App.IsNotMakeID( szTeamName.c_str(), szNotMakeString ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(%s)", szNotMakeString.c_str() );		
		return;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( szTeamName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다" );
		return;
	}

	char szSlangName[MAX_PATH] = "";
	sprintf( szSlangName, "%s", g_SlangUtil.ConvertString( szTeamName.c_str() ) );
	if( szTeamName != szSlangName )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(1)" );		
		return;
	}

	if( !IsOnlyEngHanNumText( szTeamName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(2)" );			
		return;
	}

	if( g_SlangUtil.IsLimitedGuildName( szTeamName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(3)" );		
		return;
	}

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "특별 아이템 인벤토리 에러" );
		return;
	}

	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_CREATE );
	kPacket << pTournament->GetIndex() << szTeamName << pTournament->GetMaxPlayer();
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	
	HideWnd();
}

void TournamentTeamCreateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_CREATE:
		if( cmd == IOBN_BTNUP )
		{
			SendTeamCreate();
		}
		break;
	case ID_TEAM_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				SendTeamCreate();
			}
		}
		break;
	}
}

void TournamentTeamCreateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, "친구의 권유를 받아 팀원으로 참가하거나" );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "대회팀을 만들어 팀원을 모집하세요." );

	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "모집마감까지 %d인팀을 이루어야 등록됩니다.", (int)m_MaxPlayer );

	if( m_pNameEdit != ioEdit::m_pKeyFocusEdit )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 151, iYPos + 211, FONT_SIZE_12, "대회팀 이름을 입력하세요" );
	}
}

//////////////////////////////////////////////////////////////////////////
TournamentTeamCreateComplete::TournamentTeamCreateComplete()
{
	m_pIcon	= NULL;
	m_pIconBack = NULL;
	
	m_Type = 0;
	m_CampPos = 0;
	m_dwTeamIndex = 0;
	m_dwTournamentIndex = 0;
}

TournamentTeamCreateComplete::~TournamentTeamCreateComplete()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentTeamCreateComplete::SetTeamData( DWORD dwTourIndex, DWORD dwTeamIndex, ioHashString &rkTeamName, BYTE CampPos )
{
	m_CampPos			= CampPos;
	m_dwTeamIndex       = dwTeamIndex;
	m_dwTournamentIndex = dwTourIndex;
	m_szTeamName        = rkTeamName;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		m_Type = pTournament->GetType();
		m_szTournamentTitle = pTournament->GetTournamentTitle();
	}

	ShowWnd();
}

void TournamentTeamCreateComplete::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamCreateComplete::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_TEAM_VIEW:
		if( cmd == IOBN_BTNUP )
		{
			// 팀 정보 창
			TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
			if( pTeamInfo )
			{
				pTeamInfo->SetTournamentTeam( m_dwTournamentIndex, m_dwTeamIndex );
			}

			HideWnd();
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TournamentTeamCreateComplete::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY );
	}

	if( m_pIcon )
	{
		m_pIcon->Render( iXPos + 103, iYPos + 89 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	switch( m_Type )
	{
	case TournamentNode::TYPE_REGULAR:
		{
			if( m_CampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 정규 대회팀", m_szTournamentTitle.c_str() );
		}
		break;
	case TournamentNode::TYPE_CUSTOM:
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회팀", m_szTournamentTitle.c_str() );
		break;
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 153, FONT_SIZE_13, 127.0f, "%s", m_szTeamName.c_str() );
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamUserOptionWnd::TournamentTeamUserOptionWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iOverListIndex = -1;

	m_dwTourIndex = 0;
	m_dwTeamIndex = 0;
	m_dwOwnerIndex= 0;
	m_dwLeaveTableIndex = 0;
	m_dwLeaveUserIndex  = 0;
}

TournamentTeamUserOptionWnd::~TournamentTeamUserOptionWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void TournamentTeamUserOptionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void TournamentTeamUserOptionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamUserOptionWnd::CommandOption( DWORD dwCmd )
{
	switch( dwCmd )
	{
	case OPTION_INFO:
		{
			g_App.OpenMannerWnd( m_szName.c_str(), true );
		}
		break;
	case OPTION_CHANGE_OWNER:
		{
		}
		break;
	case OPTION_KICK:
		{
			if( IsKickUserOK() )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "%s님을 팀에서#탈퇴시키겠습니까?", m_szName.c_str() );	
			}
		}
		break;
	}
}

void TournamentTeamUserOptionWnd::SetOptionInfo( DWORD dwTourIndex, DWORD dwTeamIndex, const ioHashString &rkName )
{
	HideWnd();
	/*
		-내가 팀장일 경우
		  ㄴ 자기클릭 - 자기정보
      	  ㄴ 사람클릭 - 사람정보 / 팀장 변경 / 탈퇴

	    -내가 일반일 경우
		  ㄴ 자기클릭 - 자기정보
          ㄴ 사람클릭 - 사람정보

		-명칭
		  [누구누구 정보]
		  [팀장으로 임명]
		  [탈퇴 시키기]

	    -옵션 명령이 하나밖에 없으면 즉시 실행한다.
	*/
	m_OptionMap.clear();

	m_szName	  = rkName;
	m_dwTourIndex = dwTourIndex;
	m_dwTeamIndex = dwTeamIndex;
	m_dwOwnerIndex= 0;    // 내 팀이 아니면 0이다

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
			m_dwOwnerIndex = rkTeamData.m_dwTeamOwnerIndex;
	}

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		if( rkName == g_MyInfo.GetPublicID() )
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, "정보" ) );
		}
		else
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), "%s 정보", m_szName.c_str() );
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, szTitle ) );

			//팀장 임명
	//		m_OptionMap.insert( OptionMap::value_type( OPTION_CHANGE_OWNER, "팀장으로 임명" ) );

			//탈퇴 시키기
			m_OptionMap.insert( OptionMap::value_type( OPTION_KICK, "탈퇴 시키기" ) );
		}
	}
	else    //일반 - 즉시 실행되므로 타이틀에 신경쓰지 말자.
	{
		if( !rkName.IsEmpty() )
		{	
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, "정보" ) );
		}
	}

	if( m_OptionMap.empty() )
		return;

	if( m_OptionMap.size() == 1 )
	{
		//즉시 실행
		OptionMap::iterator iCreate = m_OptionMap.begin();
		if( GetParent() )
		{
			CommandOption( iCreate->first );
		}
	}
	else
	{
		// 윈도우 사이즈 결정
		int iListWidth = 0;
		OptionMap::iterator iter;
		for( iter = m_OptionMap.begin() ; iter != m_OptionMap.end() ; ++iter )
		{
			int iTextWidth = g_FontMgr.GetTextWidth( iter->second.c_str(), TS_NORMAL, FONT_SIZE_12 );
			if( iListWidth < iTextWidth )
				iListWidth = iTextWidth;
		}
		SetSize( iListWidth + m_iAddWidth + 10, (m_OptionMap.size() * m_iHeightGap) + m_iAddHeight );

		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && GetParent() )
		{
			int iOffsetX = GetParent()->GetXPos();
			int iOffsetY = GetParent()->GetYPos();
			if( pMouse->GetMousePos().x + GetWidth() > Setting::Width() )
				SetWndPos( ( pMouse->GetMousePos().x - GetWidth() ) - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
			else
				SetWndPos( pMouse->GetMousePos().x - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
		}
		ShowWnd();
	}
}

void TournamentTeamUserOptionWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void TournamentTeamUserOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

bool TournamentTeamUserOptionWnd::IsKickUserOK()
{
	TournamentNode *pTournamentNode = g_TournamentMgr.GetTournament( m_dwTourIndex );
	if( pTournamentNode == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회정보가 존재하지 않습니다" );
		return false;
	}

	if( pTournamentNode->GetState() != TournamentNode::STATE_TEAM_APP )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회가 시작되어 강퇴가 불가능합니다" );
		return false;
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament == NULL ) return false;

	TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
	if( rkTeamData.m_dwTeamIndex != m_dwTeamIndex )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀원만 강퇴가 가능합니다" );
		return false;
	}

	if( rkTeamData.m_dwTeamOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		TeamUserData kUserData = pUserTournament->GetTeamUserData( m_dwTeamIndex, m_szName );
		if( kUserData.m_dwUserIndex == 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀원만 강퇴가 가능합니다" );
			return false;
		}
		else if( kUserData.m_dwUserIndex == g_MyInfo.GetUserIndex() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "자신은 강퇴가 불가능합니다" );
			return false;
		}

		m_dwLeaveTableIndex = kUserData.m_dwTableIndex;
		m_dwLeaveUserIndex	= kUserData.m_dwUserIndex;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀장만 팀원을 강퇴 시킬 수 있습니다" );
		return false;
	}
	return true;
}

void TournamentTeamUserOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( IsKickUserOK() )
				{
					// 강퇴
					SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_LEAVE );
					kPacket << m_dwTourIndex << m_dwTeamIndex << m_dwLeaveTableIndex << m_dwLeaveUserIndex;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
		break;
	}
}

void TournamentTeamUserOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	OptionMap::iterator iter = m_OptionMap.begin();
	for(int iIndex = 0 ; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( GetWidth() - m_iAddWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex  = iIndex;
			if( pMouse->IsLBtnUp() )
			{
				CommandOption( iter->first );
				HideWnd();
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void TournamentTeamUserOptionWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	OptionMap::iterator iter = m_OptionMap.begin();
	for( int iIndex = 0; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		bDarkRender = !bDarkRender;

		if( m_iOverListIndex == iIndex )
		{
			m_pOverLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, iter->second.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamInfoBtn::TournamentTeamInfoBtn()
{
	m_pBlueNormal	= NULL;
	m_pRedNormal	= NULL;
	m_pGrayNormal	= NULL;
	m_pBlueOver		= NULL;
	m_pRedOver		= NULL;
	m_pGrayOver		= NULL;

	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;
	m_pMasterMark   = NULL;
	

	m_CampPos      = 0;
	m_iGradeLevel  = 0;
	m_iLadderPoint = 0;
	m_bMasterUser  = false;
	m_iSlotArray   = 0;
	m_iMaxSlot	   = 0;
}

TournamentTeamInfoBtn::~TournamentTeamInfoBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pGrayNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pGrayOver );

	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
	SAFEDELETE( m_pMasterMark );
}

void TournamentTeamInfoBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "GrayNormal" )
	{
		SAFEDELETE( m_pGrayNormal );
		m_pGrayNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "GrayOver" )
	{
		SAFEDELETE( m_pGrayOver );
		m_pGrayOver = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else if( szType == "MasterMark" )
	{
		SAFEDELETE( m_pMasterMark );
		m_pMasterMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamInfoBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, BYTE CampPos, int iGradeLevel, int iLadderPoint, bool bMasterUser )
{
	m_iMaxSlot     = iMaxSlot;
	m_iSlotArray   = iSlotArray;
	m_szName       = rkName;
	m_CampPos      = CampPos;
	m_iGradeLevel  = iGradeLevel;
	m_iLadderPoint = iLadderPoint;
	m_bMasterUser  = bMasterUser;

	if( m_szName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{	
		SetActive();
	}
}

void TournamentTeamInfoBtn::OnRenderBase( int iXPos, int iYPos )
{
	ioUIRenderImage *pNormal = NULL;
	ioUIRenderImage *pOver   = NULL;
	switch( m_CampPos )
	{
	case CAMP_BLUE:
		{
			pNormal = m_pBlueNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pBlueOver;
		}
		break;
	case CAMP_RED:
		{
			pNormal = m_pRedNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pRedOver;
		}
		break;
	default:
		{
			pNormal = m_pGrayNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pGrayOver;
		}
		break;
	}

	if( pNormal )
		pNormal->Render( iXPos, iYPos );
	if( pOver )
		pOver->Render( iXPos, iYPos );
}

void TournamentTeamInfoBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBase( iXPos, iYPos );

	if( m_szName.IsEmpty() )
	{
		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );

			if( m_CampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else if( m_CampPos == CAMP_RED )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 230.0f, m_szName.c_str() );
		int iRealNameSize = min( 230.0f, g_FontMgr.GetTextWidth( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12 ) );
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );
		if( dwGuildIndex != 0 )
		{
			iRealNameSize += ( g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 3 );
		}
		if( m_bMasterUser )
		{
			if( m_pMasterMark )
				m_pMasterMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
		}

		g_FontMgr.SetAlignType( TAT_RIGHT );
		char szConvertText[MAX_PATH] = "";
		Help::ConvertNumToStrComma( m_iLadderPoint, szConvertText, sizeof( szConvertText ) );
		g_FontMgr.PrintText( iXPos + 341, iYPos + 3, FONT_SIZE_12, "%s RP", szConvertText );
	}
}

//////////////////////////////////////////////////////////////////////////
TournamentTeamInfoWnd::TournamentTeamInfoWnd()
{
	m_pBannerIconBack	= NULL;
	m_pBannerIcon		= NULL;
	m_pCustomBannerIcon = NULL;
	
	m_Type				= 0;
	m_State				= 0;
	m_CampPos			= 0;
	m_MaxPlayer			= 0;
	m_iCurPlayer		= 0;
	m_dwOwnerIndex		= 0;
	m_dwTeamOwnerIndex	= 0;

	m_dwLeaveTableIndex = 0;
	m_dwLeaveUserIndex	= 0;
}

TournamentTeamInfoWnd::~TournamentTeamInfoWnd()
{
	SAFEDELETE( m_pCustomBannerIcon );
	SAFEDELETE( m_pBannerIconBack );	
}

void TournamentTeamInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CustomBannerIcon" )
	{
		SAFEDELETE( m_pCustomBannerIcon );
		m_pCustomBannerIcon = pImage;
	}
	else if( szType == "BannerIconBack" )
	{
		SAFEDELETE( m_pBannerIconBack );
		m_pBannerIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamInfoWnd::SetTournamentTeam( DWORD dwTourIndex, DWORD dwTeamIndex )
{
	m_dwTourIndex = dwTourIndex;
	m_dwTeamIndex = dwTeamIndex;
	
	m_Type		   = 0;
	m_dwOwnerIndex = 0;
	UpdateData( true );	
	ShowWnd();
}

void TournamentTeamInfoWnd::DeleteTeamCheck( DWORD dwTeamIndex )
{
	if( m_dwTeamIndex == dwTeamIndex )
		HideWnd();
}

void TournamentTeamInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwTournamentHelpID = xElement.GetFloatAttribute_e( "TournamentHelpID" );
}

void TournamentTeamInfoWnd::UpdateTournament()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( pTournament )
	{
		m_Type				= pTournament->GetType();
		m_State				= pTournament->GetState();
		m_szTournamentTitle = pTournament->GetTournamentTitle();
		m_dwOwnerIndex		= pTournament->GetOwnerIndex();

		char szTitle[MAX_PATH] = "";
		if( m_Type == TournamentNode::TYPE_REGULAR )
		{
			sprintf_s_e( szTitle, "%s 정규 대회팀", m_szTournamentTitle.c_str() );
			m_pBannerIcon = pTournament->GetResCampIcon( m_CampPos );
		}
		else
		{
			sprintf_s_e( szTitle, "%s 대회팀", m_szTournamentTitle.c_str() );			
		}
		SetTitleText( szTitle );
	}
}

void TournamentTeamInfoWnd::UpdateData( bool bDataSync )
{
	m_UserList.clear();
	m_Type = 0;
	TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, bDataSync );
	if( pTeamData )
	{
		m_szTeamName	    = pTeamData->GetTeamName();
		m_CampPos		    = pTeamData->GetCampPos();
		m_MaxPlayer		    = pTeamData->GetMaxPlayer();
		m_iCurPlayer	    = pTeamData->GetUserSize();
		m_dwTeamOwnerIndex	= pTeamData->GetOwnerIndex();

		pTeamData->GetUserList( m_UserList );
	}
	else if( bDataSync )
	{
		// 팀 정보가 없다!!
		SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_INFO );
		kPacket << m_dwTourIndex << m_dwTeamIndex;
		TCPNetwork::SendToServer( kPacket );
	}	

	UpdateChildWnd();
	UpdateUserData();
}

void TournamentTeamInfoWnd::UpdateChildWnd()
{
	// 버튼
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		HideChildWnd( ID_RECRUIT );
		HideChildWnd( ID_DELAY_JOINERS );
		HideChildWnd( ID_HELP );
		HideChildWnd( ID_TEAM_LEAVE );
		HideChildWnd( ID_JOIN_APP );

		if( pUserTournament->IsAlreadyTeam( m_dwTeamIndex ) )
		{
			ShowChildWnd( ID_RECRUIT );
			ShowChildWnd( ID_DELAY_JOINERS );
			ShowChildWnd( ID_HELP );
			ShowChildWnd( ID_TEAM_LEAVE );

			if( m_State != TournamentNode::STATE_TEAM_APP ||
				(int)m_MaxPlayer <= m_iCurPlayer )
			{
				SetChildWndStyleAdd( ID_RECRUIT, IWS_EXACTIVE );
			}
			else
			{
				SetChildWndStyleRemove( ID_RECRUIT, IWS_EXACTIVE );
			}
		}
		else
		{
			if( pUserTournament->IsTourTeam( m_dwTourIndex ) || m_State != TournamentNode::STATE_TEAM_APP || m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
			{
				ioWnd *pJoinApp = FindChildWnd( ID_JOIN_APP );
				if( pJoinApp )
				{
					pJoinApp->ShowWnd();
					pJoinApp->SetTitleText( "닫기" );
					SetChildWndStyleAdd( ID_JOIN_APP, IWS_EXACTIVE );
				}
			}
			else
			{
				ioWnd *pJoinApp = FindChildWnd( ID_JOIN_APP );
				if( pJoinApp )
				{
					pJoinApp->ShowWnd();
					pJoinApp->SetTitleText( "대회팀 가입 신청하기" );
					SetChildWndStyleRemove( ID_JOIN_APP, IWS_EXACTIVE );
				}
			}
		}
	} 
}

void TournamentTeamInfoWnd::UpdateUserData()
{
	int iStart = 0;
	for(int i = ID_LIST_1;i < ID_LIST_8 + 1;i++,iStart++)
	{
		TournamentTeamInfoBtn *pTeamInfoBtn = dynamic_cast< TournamentTeamInfoBtn * >( FindChildWnd( i ) );
		if( pTeamInfoBtn == NULL ) continue;

		if( iStart < (int)m_UserList.size() )
		{
			TeamUserData &rkUserData = m_UserList[iStart];
			UserInfoNode *pUserInfo = g_UserInfoMgr.GetUserInfoNode( rkUserData.m_szNick, false );
			if( pUserInfo )
			{
				pTeamInfoBtn->SetMemberInfo( m_MaxPlayer, iStart, rkUserData.m_szNick, m_CampPos, pUserInfo->GetGradeLevel(), pUserInfo->GetLadderPoint(), (m_dwTeamOwnerIndex == rkUserData.m_dwUserIndex) );
			}
			else
			{
				pTeamInfoBtn->SetMemberInfo( m_MaxPlayer, iStart, rkUserData.m_szNick, m_CampPos, 0, 0, (m_dwTeamOwnerIndex == rkUserData.m_dwUserIndex) );
			}
		}
		else
		{
			pTeamInfoBtn->SetMemberInfo( m_MaxPlayer, iStart, "", m_CampPos, 0, 0, false );
		}
	}
}

bool TournamentTeamInfoWnd::IsTeamLeaveOK()
{
	TournamentNode *pTournamentNode = g_TournamentMgr.GetTournament( m_dwTourIndex );
	if( pTournamentNode == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회정보가 존재하지 않습니다" );
		return false;
	}

	if( pTournamentNode->GetState() != TournamentNode::STATE_TEAM_APP )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회가 시작되어 탈퇴가 불가능합니다" );
		return false;
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament == NULL ) return false;

	TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
	if( rkTeamData.m_dwTeamIndex != m_dwTeamIndex )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀원만 탈퇴가 가능합니다" );
		return false;
	}

	if( rkTeamData.m_dwTeamOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		// 방장이 탈퇴
		if( rkTeamData.m_TeamUserList.size() == 1 )      // 자신밖에 없으면 팀 삭제
		{
			TeamUserData kUserData = pUserTournament->GetTeamUserData( m_dwTeamIndex, g_MyInfo.GetPublicID() );
			if( kUserData.m_dwUserIndex == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀원만 탈퇴가 가능합니다" );
				return false;
			}

			m_dwLeaveTableIndex = kUserData.m_dwTableIndex;
			m_dwLeaveUserIndex	= kUserData.m_dwUserIndex;
		}
		else
		{
			// 안내 멘트
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀장은 팀원이 있을 경우엔#팀 탈퇴가 불가능합니다" );
			return false;
		}
	}
	else
	{
		TeamUserData kUserData = pUserTournament->GetTeamUserData( m_dwTeamIndex, g_MyInfo.GetPublicID() );
		if( kUserData.m_dwUserIndex == 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀원만 탈퇴가 가능합니다" );
			return false;
		}

		m_dwLeaveTableIndex = kUserData.m_dwTableIndex;
		m_dwLeaveUserIndex	= kUserData.m_dwUserIndex;
	}
	return true;
}

void TournamentTeamInfoWnd::SendTeamLeave()
{
	if( IsTeamLeaveOK() == false ) 
		return;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
		g_GUIMgr.SetMsgBox( MB_YESNO, this, "[%s]팀에서#탈퇴 하시겠습니까?", rkTeamData.m_szTeamName.c_str() );	
	}
}

void TournamentTeamInfoWnd::SendTeamEntryApp()
{
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsTourTeam( m_dwTourIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회에 이미 팀이 존재합니다" );
		}
		else if( pUserTournament->IsAlreadyTeam( m_dwTeamIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 팀원입니다" );
		}
		else if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
		{
			g_App.OpenFormalityEntryPage();
		}
		else if( (int)m_MaxPlayer <= m_iCurPlayer )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀원이 가득 찼습니다" );
		}		
		else
		{
			bool bFailed = false;
			DWORD dwOwnerIndex = 0;
			TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, true );
			if( pTeamData )
			{
				dwOwnerIndex = pTeamData->GetOwnerIndex();
				if( dwOwnerIndex == 0 )
					bFailed = true;

				// 진영 체크
				if( m_Type == 0 )
				{
					bFailed = true;
				}
				else if( m_Type == TournamentNode::TYPE_REGULAR )
				{
					if( g_MyInfo.GetUserCampPos() == CAMP_NONE )
					{
						if( ioPlayMode::GetModeType() == MT_MYROOM )
						{
							if( ioMyRoomMode::GetMyRoomType() == MRT_LOBBY )
								g_GUIMgr.ShowWnd( CAMP_JOIN_NEED_POPUP );
							else
								g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영에 먼저 가입해 주세요." );
						}
						else
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영에 먼저 가입해 주세요." );
						}
						return;
					}
					else if( pTeamData->GetCampPos() != g_MyInfo.GetUserCampPos() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "같은 진영의 팀만 가입이 가능합니다" );	
						return;
					}
				}
				else if( m_Type == TournamentNode::TYPE_CUSTOM )
				{
					TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTourIndex );
					if( !pNode )
					{
						bFailed = true;
					}
					else
					{
						if( pNode->GetOwnerIndex() == g_MyInfo.GetUserIndex() )
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 주최자는#대회팀에 가입 할 수 없습니다." );
							return;
						}
					}
				}
			}
			else
			{
				bFailed = true;
			}

			if( bFailed )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀 정보가 정확하지 않습니다#다시 시도해주세요" );				
			}
			else
			{
				SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_ENTRY_APP );
				kPacket << m_dwTourIndex << m_dwTeamIndex << dwOwnerIndex;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
		}
	}
}

void TournamentTeamInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();
		}
		break;
	case ID_RECRUIT:
		if( cmd == IOBN_BTNUP )
		{			
			TournamentTeamInvitationWnd *pInvitationWnd = dynamic_cast< TournamentTeamInvitationWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INVITATION_WND ) );
			if( pInvitationWnd )
			{
				pInvitationWnd->SetTournamentInvitation( m_dwTourIndex, m_dwTeamIndex );
			}
		}
		else if( cmd == IOEX_BTNUP )
		{
			if( m_State != TournamentNode::STATE_TEAM_APP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "모집기간이 종료되었습니다." );
			}
			else if( (int)m_MaxPlayer <= m_iCurPlayer )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀결성이 완료되었습니다." );
			}
		}
		break;
	case ID_DELAY_JOINERS:
		if( cmd == IOBN_BTNUP )
		{			
			SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_ENTRY_MEMBER );
			kPacket << m_dwTourIndex << m_dwTeamIndex;
			TCPNetwork::SendToServer( kPacket );

			// UI Show
			TournamentTeamEntryDelayUserWnd *pDelayUserWnd = dynamic_cast<TournamentTeamEntryDelayUserWnd *>(g_GUIMgr.FindWnd( TOURNAMENT_TEAM_ENTRY_DELAY_USER_WND ) );
			if( pDelayUserWnd )
			{
				pDelayUserWnd->SetAutoShow( m_dwTourIndex, m_dwTeamIndex, false );
				pDelayUserWnd->ShowWnd();
			}
		}
		break;
	case ID_HELP:
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_dwTournamentHelpID );
		}
		break;
	case ID_TEAM_LEAVE:
		if( cmd == IOBN_BTNUP )
		{			
			SendTeamLeave();
		}
		break;
	case ID_JOIN_APP:
		if( cmd == IOBN_BTNUP )
		{
			SendTeamEntryApp();
		}
		else if( cmd == IOEX_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
	case ID_LIST_7:
	case ID_LIST_8:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamInfoBtn *pTeamInfoBtn = dynamic_cast< TournamentTeamInfoBtn * >( pWnd );
			if( pTeamInfoBtn )
			{
				TournamentTeamUserOptionWnd *pUserOption = dynamic_cast<TournamentTeamUserOptionWnd*>( FindChildWnd( ID_USER_OPTION ) );
				if( pUserOption )
					pUserOption->SetOptionInfo( m_dwTourIndex, m_dwTeamIndex, pTeamInfoBtn->GetUserName() );
			}
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( IsTeamLeaveOK() )
				{
					// 탈퇴
					SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_LEAVE );
					kPacket << m_dwTourIndex << m_dwTeamIndex << m_dwLeaveTableIndex << m_dwLeaveUserIndex;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
		break;
	}
}

void TournamentTeamInfoWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pBannerIconBack )
	{
		m_pBannerIconBack->SetScale( 0.60f, 0.60f );
		m_pBannerIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_Type == TournamentNode::TYPE_REGULAR )
	{
		if( m_pBannerIcon )
		{		
			m_pBannerIcon->SetScale( 0.60f, 0.60f );
			m_pBannerIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
			m_pBannerIcon->SetScale( FLOAT1, FLOAT1 );
		}
	}
	else
	{
		if( m_pCustomBannerIcon )
		{
			m_pCustomBannerIcon->SetScale( 0.60f, 0.60f );
			m_pCustomBannerIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, m_szTeamName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( (int)m_MaxPlayer == m_iCurPlayer )
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, "팀결성 완료" );
	else
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, "%d인 부족", (int)m_MaxPlayer - m_iCurPlayer );
}

void TournamentTeamInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	ioMovingWnd::OnProcess( fTimePerSec );

	UpdateSyncCheck();
}

void TournamentTeamInfoWnd::UpdateSyncCheck()
{
	if( m_dwTourIndex == 0 ) 
		return;

	if( m_Type == 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
		if( pTournament && pTournament->GetType() != 0 )
		{
			UpdateTournament();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTourIndex );
		}			
	}
	else
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
		if( pTournament )
		{
			if( pTournament->IsUpdateInfoUI() )
			{
				UpdateData( false );
			}
		}

		if( m_dwTeamIndex == 0 ) return;
		TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
		if( pTeamData )
		{
			if( pTeamData->IsUpdateInfoUI() )
			{
				UpdateData( false );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamInvitationFriendWnd::TournamentTeamInvitationFriendWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iCutWidth = 0;
	m_iCurPos   = 0;

	m_dwTournamentOwnerIndex = 0;
}

TournamentTeamInvitationFriendWnd::~TournamentTeamInvitationFriendWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void TournamentTeamInvitationFriendWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void TournamentTeamInvitationFriendWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamInvitationFriendWnd::iwm_show()
{
	// 온라인 친구 리스트
	m_szOverListUser.Clear();
	m_OnlineUserList.clear();
	for( int i = 0; i < g_FriendManager.GetNodeSize(); i++ )
	{
		FriendNode *pNode = g_FriendManager.GetNode( i );
		if( !pNode ) continue;
		if( !pNode->IsOnline() ) continue;
		if( pNode->GetUserIndex() == m_dwTournamentOwnerIndex ) continue; //주최자 초대 불가

		m_OnlineUserList.push_back( pNode->GetName() );
	}

	if( m_OnlineUserList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "접속중인 친구가 없습니다" );
		HideWnd();
		return;
	}

	m_iCurPos = 0;
	if( (int)m_OnlineUserList.size() <= MAX_PAGE_LIST )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_OnlineUserList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	// 윈도우 사이즈 결정	
	SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_OnlineUserList.size() ) * m_iHeightGap) + m_iAddHeight );
	SetCheckParentOutSide( true );
}

void TournamentTeamInvitationFriendWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void TournamentTeamInvitationFriendWnd::SetTournamentOwenrIndex( DWORD dwOwnerIndex )
{
	m_dwTournamentOwnerIndex = dwOwnerIndex;
	ShowWnd();
}

void TournamentTeamInvitationFriendWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void TournamentTeamInvitationFriendWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void TournamentTeamInvitationFriendWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_szOverListUser.Clear();

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( TournamentTeamInvitationWnd::ID_FRIEND_BTN ) &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_OnlineUserList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_OnlineUserList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - m_iAddWidth ) - iCutWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_szOverListUser  = m_OnlineUserList[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
					GetParent()->iwm_command( this, IOBN_BTNUP, 0 );
				HideWnd();
			}
			break;
		}
	}
}

void TournamentTeamInvitationFriendWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_OnlineUserList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_OnlineUserList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_szOverListUser == m_OnlineUserList[iListPos] )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, 244.0f - iCutWidth, 
			m_OnlineUserList[iListPos].c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamInvitationWnd::TournamentTeamInvitationWnd()
{
	m_pPreEdit = NULL;
	m_pNameEdit= NULL;

	m_dwTourOwnerIndex = 0;

	m_dwTourIndex = 0;
	m_dwTeamIndex = 0;
}

TournamentTeamInvitationWnd::~TournamentTeamInvitationWnd()
{

}

void TournamentTeamInvitationWnd::SetTournamentInvitation( DWORD dwTourIndex, DWORD dwTeamIndex )
{
	m_dwTourIndex = dwTourIndex;
	m_dwTeamIndex = dwTeamIndex;

	TournamentNode* pNode = g_TournamentMgr.GetTournament( dwTourIndex, false );
	if( pNode )
		m_dwTourOwnerIndex = pNode->GetOwnerIndex();

	ShowWnd();
}

void TournamentTeamInvitationWnd::SendInvitation()
{
	if( g_App.IsMouseBusy() ) return;	

	ioHashString szUserName;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit )
	{
		szUserName = pEdit->GetText();
		pEdit->SetKeyFocus();
	}

	if( szUserName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "닉네임을 입력하세요" );
		return;
	}

	if( !g_App.IsRightID( szUserName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 닉네임입니다" );
		return;
	}

	if( szUserName == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "자신의 닉네임입니다" );
		return;
	}
	
	if( g_App.IsAdminID( szUserName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "개발자나 마스터에게 가입권유는 안됩니다" );
		return;
	}

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( pTournament == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회정보가 존재하지 않습니다#잠시 후 다시 시도해주세요" );
		return;
	}

	TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
	if( pTeamData == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀정보가 존재하지 않습니다#잠시 후 다시 시도해주세요" );
		return;	
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( !pUserTournament->IsAlreadyTeam( m_dwTeamIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀정보가 존재하지 않습니다#잠시 후 다시 시도해주세요" );
			return;	
		}
		else if( (int)pTournament->GetMaxPlayer() <= pUserTournament->GetCurrentTeamUserCount( m_dwTeamIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀결성이 완료되어#팀원을 모집할 수 없습니다" );
			return;	
		}
	}
	
	TeamUserVec UserVec;
	pTeamData->GetUserList( UserVec );

	TeamUserVec::const_iterator iter = UserVec.begin();
	for( ; iter != UserVec.end(); ++iter )
	{
		const TeamUserData User = *iter;
		if( pTournament->GetOwnerIndex() == User.m_dwUserIndex )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 주최자는 초대 할 수 없습니다." );
			return;
		}
	}

	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_INVITATION );
	kPacket << szUserName << m_dwTourIndex << m_dwTeamIndex << g_MyInfo.GetPublicID() << pTournament->GetType() 
		    << pTournament->GetTournamentTitle() << pTeamData->GetTeamName() << pTeamData->GetCampPos();
	TCPNetwork::SendToServer( kPacket );
		
	HideWnd();
	g_GUIMgr.SetMsgBox( MB_OK, NULL, "가입권유 메시지가 발송되었습니다." );
}

void TournamentTeamInvitationWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_SEND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendInvitation();			
		}
		break;
	case ID_FRIEND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamInvitationFriendWnd *pFriendListWnd = dynamic_cast<TournamentTeamInvitationFriendWnd*>( FindChildWnd( ID_FRIEND_LIST ) );
			if( pFriendListWnd )
			{
				if( pFriendListWnd->IsShow() )
					pFriendListWnd->HideWnd();
				else
					pFriendListWnd->SetTournamentOwenrIndex( m_dwTourOwnerIndex );
			}
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				SendInvitation();				
			}			
		}
		break;
	case ID_FRIEND_LIST:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamInvitationFriendWnd *pFriendWnd = dynamic_cast<TournamentTeamInvitationFriendWnd*>( pWnd );
			if( pFriendWnd )
			{
				g_InputBox.SetString( pFriendWnd->GetOverUser().c_str() );
			}
		}
		break;
	}
}

void TournamentTeamInvitationWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	m_pNameEdit = FindChildWnd( ID_NAME_EDIT );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void TournamentTeamInvitationWnd::iwm_hide()
{
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

bool TournamentTeamInvitationWnd::iwm_esc()
{
	TournamentTeamInvitationFriendWnd *pFriendListWnd = dynamic_cast<TournamentTeamInvitationFriendWnd*>( FindChildWnd( ID_FRIEND_LIST ) );	
	if( pFriendListWnd && pFriendListWnd->IsShow() )
	{
		pFriendListWnd->HideWnd();
		return true;
	}
	return ioWnd::iwm_esc();
}

void TournamentTeamInvitationWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void TournamentTeamInvitationWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, "가입권유 메시지를 받을 닉네임을 입력하세요" );
}
//////////////////////////////////////////////////////////////////////////	
DWORD TournamentTeamInvitedWnd::m_dwTournamentIndex;
DWORD TournamentTeamInvitedWnd::m_dwTeamIndex;
ioHashString TournamentTeamInvitedWnd::m_szUserName;
ioHashString TournamentTeamInvitedWnd::m_szTeamName;
ioHashString TournamentTeamInvitedWnd::m_szTourTitle;
BYTE TournamentTeamInvitedWnd::m_Type;
BYTE TournamentTeamInvitedWnd::m_CampPos;
bool TournamentTeamInvitedWnd::sg_bInvitedIgnore;
TournamentTeamInvitedWnd::TournamentTeamInvitedWnd()
{
	m_pIcon = NULL;
	m_pIconBack = NULL;
}

TournamentTeamInvitedWnd::~TournamentTeamInvitedWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentTeamInvitedWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamInvitedWnd::SetInvitedInfo( SP2Packet &rkPacket )
{
	ioHashString kTargetName;
	DWORD dwTourIndex, dwTeamIndex;
	rkPacket >> kTargetName >> dwTourIndex >> dwTeamIndex;

	TournamentNode* pTournament = g_TournamentMgr.GetTournament( dwTourIndex, false );
	if( pTournament && pTournament->GetOwnerIndex() == g_MyInfo.GetUserIndex() ) //주최자에게 팀 초대창 안보이기
	{
		return;
	}

	if( kTargetName != g_MyInfo.GetPublicID() )
		return;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		// 팀이 존재한다.
		if( pUserTournament->IsTourTeam( dwTourIndex ) )
			return;
	}

	m_dwTeamIndex = dwTeamIndex;
	m_dwTournamentIndex = dwTourIndex;
	rkPacket >> m_szUserName >> m_Type >> m_szTourTitle >> m_szTeamName >> m_CampPos;

	if( sg_bInvitedIgnore )
	{
		m_dwTeamIndex = 0;
	}

	if( m_dwTeamIndex == 0 )
	{
		HideWnd();
	}
	else
	{
		ShowWnd();

		// 정보를 보유하고 있는지 확인
		TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
		if( pTeamData == NULL )
		{
			// 팀 정보 요청
			SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_INFO );
			kPacket << m_dwTournamentIndex << m_dwTeamIndex;
			TCPNetwork::SendToServer( kPacket );
		}
	}
}

bool TournamentTeamInvitedWnd::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		m_dwTeamIndex = 0;
		return true;
	}
	return false;
}

void TournamentTeamInvitedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			m_dwTeamIndex = 0;
		}
		break;
	case ID_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 팀 정보 창
			TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
			if( pTeamInfo )
			{
				pTeamInfo->SetTournamentTeam( m_dwTournamentIndex, m_dwTeamIndex );
			}

			HideWnd();
			m_dwTeamIndex = 0;
		}
		break;
	case ID_REFUSE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			m_dwTeamIndex = 0;
		}
		break;
	case ID_INVITED_IGNORE:
		if( cmd == IOBN_BTNUP )
		{
			sg_bInvitedIgnore = !sg_bInvitedIgnore;
			if( sg_bInvitedIgnore )
			{
				HideWnd();
				m_dwTeamIndex = 0;
			}
		}
		break;
	}
}

void TournamentTeamInvitedWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( m_dwTeamIndex != 0 )
			ShowWnd();
		else
			return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
}

void TournamentTeamInvitedWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.55f );
		m_pIconBack->Render( iXPos + 33, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pIcon )
		{
			m_pIcon->SetScale( 0.55f );
			m_pIcon->Render( iXPos + 33, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	switch( m_CampPos )
	{
	case CAMP_BLUE:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 정규 대회팀", m_szTourTitle.c_str() );
		}
		break;
	case CAMP_RED:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 정규 대회팀", m_szTourTitle.c_str() );
		}
		break;
	default:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 대회팀", m_szTourTitle.c_str() );
		}
		break;
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 71, FONT_SIZE_13, 133.0f, m_szTeamName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 99, FONT_SIZE_12, "대회팀 가입 신청을 부탁드립니다." );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 117, FONT_SIZE_12, "보낸이 : " );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 63, iYPos + 117, FONT_SIZE_12, 127.0f, m_szUserName.c_str() );
}

void TournamentTeamInvitedWnd::iwm_show()
{
	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_INVITED_IGNORE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bInvitedIgnore );
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamEntryDelayUserBtn::TournamentTeamEntryDelayUserBtn()
{
	m_dwMsgBoxID     = 0;
	m_dwTourIndex    = 0;
	m_dwTeamIndex    = 0;
	m_dwTableIndex   = 0;
	m_dwUserIndex    = 0;
	m_iUserLevel     = 0;
	m_iMaxList		 = 0;
	m_CampPos		 = 0;
}

TournamentTeamEntryDelayUserBtn::~TournamentTeamEntryDelayUserBtn()
{
}

void TournamentTeamEntryDelayUserBtn::MemberInfo( int iMaxList, DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwTableIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID, BYTE CampPos )
{
	m_iMaxList      = iMaxList;
	m_dwTourIndex   = dwTourIndex;
	m_dwTeamIndex   = dwTeamIndex;
	m_dwTableIndex	= dwTableIndex;
	m_dwUserIndex	= dwUserIndex;
	m_iUserLevel    = iUserLevel;
	m_szUserID      = szUserID;
	m_CampPos		= CampPos;

	if( m_dwTourIndex == 0 || m_dwTeamIndex == 0 || m_szUserID.IsEmpty() )
	{
		SetInActive();
		HideChildWnd( ID_AGREE );
		HideChildWnd( ID_REFUSE );
	}
	else 
	{
		SetActive();
		ShowChildWnd( ID_AGREE );
		ShowChildWnd( ID_REFUSE );

		DWORD dwOwnerIndex = 0;
		ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
		if( pUserTournament )
		{
			TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
			dwOwnerIndex = rkTeamData.m_dwTeamOwnerIndex;
		}

		if( dwOwnerIndex != g_MyInfo.GetUserIndex() )
		{
			SetChildInActive( ID_AGREE );
			SetChildInActive( ID_REFUSE );
		}
		else
		{
			SetChildActive( ID_AGREE );
			SetChildActive( ID_REFUSE );
		}
	}
	m_dwMsgBoxID = 0;
}

bool TournamentTeamEntryDelayUserBtn::IsError( DWORD dwID )
{
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( !pUserTournament ) return true;

	TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
	if( rkTeamData.m_dwTeamIndex != m_dwTeamIndex )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "자신의 팀만 가능합니다" );
		return true;
	}
	else if( rkTeamData.m_dwTeamOwnerIndex != g_MyInfo.GetUserIndex() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀장만이 수락이 가능합니다" );
		return true;
	}


	if( dwID == ID_AGREE )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
		if( pTournament )
		{
			if( pTournament->GetState() != TournamentNode::STATE_TEAM_APP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀모집 기간이 종료되었습니다." );
				return true;
			}

			if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
			{
				TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( rkTeamData.m_dwTeamIndex, false );
				if( pTeamData )
				{
					// 정규 대회 진영 타입 추가
					if( m_CampPos != pTeamData->GetCampPos() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "다른 진영의 유저는#팀원이 될 수 없습니다" );
						return true;
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀 정보가 없습니다#잠시 후 다시 시도해주세요" );
					return true;
				}
			}
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 정보가 없습니다#잠시 후 다시 시도해주세요" );
			return true;
		}
	}

	return false;
}

void TournamentTeamEntryDelayUserBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			if( !IsError( dwID ) )
			{
				m_dwMsgBoxID = ID_AGREE;
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "%s님의 가입 신청을#수락 하시겠습니까?", m_szUserID.c_str() );			
			}
		}
		break;
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
		{
			if( !IsError( dwID ) )
			{
				m_dwMsgBoxID = ID_REFUSE;
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "%s님의 가입 신청을#거절 하시겠습니까?", m_szUserID.c_str() );		
			}
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_dwMsgBoxID == ID_AGREE )
				{
					if( !IsError( m_dwMsgBoxID ) )
					{
						SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_ENTRY_AGREE );
						kPacket << m_dwTableIndex << m_dwUserIndex << m_dwTourIndex << m_dwTeamIndex;
						
						bool bCampCheck = false;
						TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
						if( pTournament )
						{
							if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
								bCampCheck = true;
						}
						kPacket << bCampCheck;
						TCPNetwork::SendToServer( kPacket );

						// UI 처리 
						TournamentTeamEntryDelayUserWnd *pParent = dynamic_cast<TournamentTeamEntryDelayUserWnd*>(GetParent());
						if( pParent )
							pParent->DeleteUserList( m_dwTableIndex );
					}
				}
				else if( m_dwMsgBoxID == ID_REFUSE )
				{
					if( !IsError( m_dwMsgBoxID ) )
					{
						SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_ENTRY_REFUSE );
						kPacket << m_dwTableIndex;
						TCPNetwork::SendToServer( kPacket );

						// UI 처리 
						TournamentTeamEntryDelayUserWnd *pParent = dynamic_cast<TournamentTeamEntryDelayUserWnd*>(GetParent());
						if( pParent )
							pParent->DeleteUserList( m_dwTableIndex );
					}
				}
			}
		}
		break;
	}
}

void TournamentTeamEntryDelayUserBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_szUserID.IsEmpty() )
	{
		if( m_iMaxList == 0 && GetID() == TournamentTeamEntryDelayUserWnd::ID_LIST_3 )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintText( iXPos + 86, iYPos + 3, FONT_SIZE_12, "대기자가 없습니다" );
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );

			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 79.0f, m_szUserID.c_str() );
	}	
}

void TournamentTeamEntryDelayUserBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szUserID.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szUserID, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
bool TournamentTeamEntryDelayUserWnd::sg_bAutoShowIgnore = false;
TournamentTeamEntryDelayUserWnd::TournamentTeamEntryDelayUserWnd()
{
	m_pBottomFrm = NULL;
	m_pIcon      = NULL;
	m_pIconBack	 = NULL;

	m_bAutoShow = true;

	m_Type = 0; 
	m_CampPos = 0;
	m_dwTourIndex = 0;
	m_dwTeamIndex = 0;
	
	m_iCurPage = m_iMaxPage = 0;
}

TournamentTeamEntryDelayUserWnd::~TournamentTeamEntryDelayUserWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentTeamEntryDelayUserWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void TournamentTeamEntryDelayUserWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamEntryDelayUserWnd::SetAutoShow( DWORD dwTourIndex, DWORD dwTeamIndex, bool bAutoShow )
{
	m_dwTourIndex = dwTourIndex;
	m_dwTeamIndex = dwTeamIndex;
	m_bAutoShow   = bAutoShow;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
	if( pTournament )
	{
		m_Type = pTournament->GetType();
		m_szTournamentTitle = pTournament->GetTournamentTitle();
	}

	TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
	if( pTeamData )
	{
		m_CampPos = pTeamData->GetCampPos();
		m_szTeamName = pTeamData->GetTeamName();
	}
}

void TournamentTeamEntryDelayUserWnd::SetEntryDelayUser( SP2Packet &rkPacket )
{
	if( m_bAutoShow && sg_bAutoShowIgnore )
		return;

	m_Type		= 0; 
	m_CampPos	= 0;
	m_szTeamName.Clear();
	m_szTournamentTitle.Clear();
	m_vUserList.clear();

	DWORD dwTourIndex, dwTeamIndex;
	rkPacket >> dwTourIndex >> dwTeamIndex;

	m_dwTourIndex = dwTourIndex;
	m_dwTeamIndex = dwTeamIndex;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
		if( rkTeamData.m_dwTeamIndex != m_dwTeamIndex )
		{
			return;     // 패킷이 오기전에 팀을 해체했다.
		}
	}
	else
	{
		return;
	}

	if( !IsShow() )
		ShowWnd();
	
	int iMaxUser;
	rkPacket >> iMaxUser;
	for(int i = 0;i < iMaxUser;i++)
	{
		EntryUser kInfo;
		rkPacket >> kInfo.m_dwTableIndex >> kInfo.m_dwUserIndex >> kInfo.m_iUserLevel >> kInfo.m_szUserID >> kInfo.m_CampPos;

		g_UserInfoMgr.SetGrade( kInfo.m_szUserID, kInfo.m_iUserLevel );
		m_vUserList.push_back( kInfo );
	}
	UpdateSetPage( m_iCurPage );

	UpdateEntryDelayUser();
}

void TournamentTeamEntryDelayUserWnd::UpdateEntryDelayUser()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
	if( pTournament )
	{
		m_Type = pTournament->GetType();
		m_szTournamentTitle = pTournament->GetTournamentTitle();
	}

	TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
	if( pTeamData )
	{
		m_CampPos = pTeamData->GetCampPos();
		m_szTeamName = pTeamData->GetTeamName();
	}
}

void TournamentTeamEntryDelayUserWnd::ReSizeWnd()
{
	if( m_bAutoShow )
	{
		ShowChildWnd( ID_AUTO_SHOW_IGNORE );
		SetSize( GetWidth(), AUTO_SHOW_HEIGHT );
		SetWndPos( Setting::Width() - ( GetWidth() + 1 ), 1 );
	}
	else
	{
		HideChildWnd( ID_AUTO_SHOW_IGNORE );
		SetSize( GetWidth(), CUSTOM_SHOW_HEIGHT );
		SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ), ( Setting::Height() / 2 ) - ( ( GetHeight() / 2 ) + 20 ) );
	}
}

void TournamentTeamEntryDelayUserWnd::iwm_show()
{
	m_vUserList.clear();

	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AUTO_SHOW_IGNORE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bAutoShowIgnore );

	ReSizeWnd();
	UpdateSetPage( 0 );
}

void TournamentTeamEntryDelayUserWnd::iwm_hide()
{
	m_bAutoShow = true;
}

void TournamentTeamEntryDelayUserWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage + 1 );
		}
		break;
	case ID_AUTO_SHOW_IGNORE:
		if( cmd == IOBN_BTNUP )
		{
			sg_bAutoShowIgnore = !sg_bAutoShowIgnore;
			if( sg_bAutoShowIgnore )
				HideWnd();
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamEntryDelayUserBtn *pUserBtn = dynamic_cast<TournamentTeamEntryDelayUserBtn*>( pWnd );
			if( pUserBtn )
			{
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), false );
			}
		}
		break;
	}
}

void TournamentTeamEntryDelayUserWnd::DeleteUserList( DWORD dwTableIndex )
{
	int iListSIze = m_vUserList.size();
	for(int i = 0;i < iListSIze;i++)
	{
		EntryUser &kInfo = m_vUserList[i];
		if( kInfo.m_dwTableIndex == dwTableIndex )
		{
			m_vUserList.erase( m_vUserList.begin() + i );
			break;
		}
	}
	UpdateSetPage( m_iCurPage );
}

void TournamentTeamEntryDelayUserWnd::UpdateSetPage( int iCurPage )
{
	int iUserCount = m_vUserList.size();
	m_iMaxPage = max( 0, iUserCount - 1 ) / MAX_LIST;
	m_iCurPage = max( 0, iCurPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );

	int iStartArray = m_iCurPage * MAX_LIST;
	for(int i = 0;i < MAX_LIST;i++,iStartArray++)
	{
		TournamentTeamEntryDelayUserBtn *pItem = (TournamentTeamEntryDelayUserBtn*)FindChildWnd( ID_LIST_1 + i );
		if( pItem )
		{
			if( iStartArray < iUserCount )
			{
				EntryUser &kInfo = m_vUserList[iStartArray];
				pItem->MemberInfo( iUserCount, m_dwTourIndex, m_dwTeamIndex, kInfo.m_dwTableIndex, kInfo.m_dwUserIndex, kInfo.m_iUserLevel, kInfo.m_szUserID, kInfo.m_CampPos );
			}
			else
			{
				pItem->MemberInfo( iUserCount, 0, 0, 0, 0, 0, "", 0 );
			}
		}
	}
}

void TournamentTeamEntryDelayUserWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_Type == 0 && m_dwTourIndex != 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
		if( pTournament && pTournament->GetType() != 0 )
		{
			UpdateEntryDelayUser();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTourIndex );
		}
	}
}

void TournamentTeamEntryDelayUserWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		ReSizeWnd();
	}
}

void TournamentTeamEntryDelayUserWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.55f );
		m_pIconBack->Render( iXPos + 33, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pIcon )
	{
		m_pIcon->SetScale( 0.55f );
		m_pIcon->Render( iXPos + 33, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	switch( m_Type )
	{
	case TournamentNode::TYPE_REGULAR:
		{
			if( m_CampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 정규 대회팀", m_szTournamentTitle.c_str() );
		}
		break;
	default:
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 대회팀", m_szTournamentTitle.c_str() );
		break;
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 71, FONT_SIZE_13, 133.0f, "%s", m_szTeamName.c_str() );

	//
	if( m_bAutoShow )
	{
		if( m_pBottomFrm )
			m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 214, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
TournamentTeamEventWnd::TournamentTeamEventWnd()
{
	m_pIcon		= NULL;
	m_pIconBack1= NULL;
	m_pIconBack2= NULL;
	m_pExclamationMark = NULL;

	m_dwTeamEvent = 0;
	m_dwTourIndex = 0;
	m_dwTeamIndex = 0;

	m_Type = 0;
	m_CampPos = 0;
}

TournamentTeamEventWnd::~TournamentTeamEventWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack1 );
	SAFEDELETE( m_pIconBack2 );
	SAFEDELETE( m_pExclamationMark );
}

void TournamentTeamEventWnd::iwm_show()
{

}

void TournamentTeamEventWnd::iwm_hide()
{

}

void TournamentTeamEventWnd::SetTeamEventData( DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwEventType )
{
	m_Type		  = 0;
	m_dwTourIndex = dwTourIndex;
	m_dwTeamIndex = dwTeamIndex;
	m_dwTeamEvent = dwEventType;

	TeamGlobalData *pGlobalTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
	if( pGlobalTeamData )
	{
		m_CampPos		= pGlobalTeamData->GetCampPos();
		m_szTeamName    = pGlobalTeamData->GetTeamName();
	}

	HideChildWnd( ID_CLOSE );
	HideChildWnd( ID_INFO );
	HideChildWnd( ID_OK );

	switch( m_dwTeamEvent )
	{
	case EVENT_TYPE_JOIN:
		{
			SetTitleText( "대회팀 가입 완료" );

			ShowChildWnd( ID_INFO );
			ShowChildWnd( ID_CLOSE );

			SetWndPos( Setting::Width() - ( GetWidth() + 1), 1 );
		}
		break;
	case EVENT_TYPE_TEAM_DELETE:
		{
			SetTitleText( "대회팀 해체 완료" );
			
			ShowChildWnd( ID_OK );

			SetWndPos( ( Setting::Width() ) / 2 - ( GetWidth() / 2 ), ( Setting::Height() / 2 ) - ( GetHeight() / 2 ) );

			//
			TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
			if( pTeamInfo )
			{
				pTeamInfo->DeleteTeamCheck( m_dwTeamIndex );
			}
		}
		break;
	case EVENT_TYPE_TEAM_LEAVE:
		{
			SetTitleText( "대회팀 탈퇴" );

			ShowChildWnd( ID_INFO );
			ShowChildWnd( ID_CLOSE );

			SetWndPos( Setting::Width() - ( GetWidth() + 1), 1 );
		}
		break;
	}


	ShowWnd();
}

void TournamentTeamEventWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_INFO:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
			if( pTeamInfo )
			{
				pTeamInfo->SetTournamentTeam( m_dwTourIndex, m_dwTeamIndex );
			}
			HideWnd();
		}
		break;
	}
}

void TournamentTeamEventWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioMovingWnd::AddRenderFrame( szType, pFrame );
}

void TournamentTeamEventWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack1" )
	{
		SAFEDELETE( m_pIconBack1 );
		m_pIconBack1 = pImage;
	}
	else if( szType == "IconBack2" )
	{
		SAFEDELETE( m_pIconBack2 );
		m_pIconBack2 = pImage;
	}
	else if( szType == "ExclamationMark" )
	{
		SAFEDELETE( m_pExclamationMark );
		m_pExclamationMark = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamEventWnd::UpdateTournament()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( pTournament )
	{
		m_Type = pTournament->GetType();
		m_szTourName = pTournament->GetTournamentTitle();
	}
}

void TournamentTeamEventWnd::OnProcess( float fTimePerSec )
{
	if( !ioMovingWnd::IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_Type == 0 && m_dwTourIndex != 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
		if( pTournament )
		{
			if( pTournament->GetType() != 0 )
				UpdateTournament();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTourIndex );
		}
	}
}

void TournamentTeamEventWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_dwTeamEvent )
	{
	case EVENT_TYPE_JOIN:
		OnJoinRender( iXPos, iYPos );
		break;
	case EVENT_TYPE_TEAM_DELETE:
		OnTeamDeleteRender( iXPos, iYPos );
		break;
	case EVENT_TYPE_TEAM_LEAVE:
		OnTeamLeaveRender( iXPos, iYPos );
		break;
	}
}

void TournamentTeamEventWnd::OnJoinRender( int iXPos, int iYPos )
{
	if( m_pIconBack1 )
	{
		m_pIconBack1->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY );
		if( m_pIcon )
			m_pIcon->Render( iXPos + 103, iYPos + 89 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	switch( m_Type )
	{
	case TournamentNode::TYPE_REGULAR:
		{
			if( m_CampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 정규 대회팀", m_szTourName.c_str() );
		}
		break;
	case TournamentNode::TYPE_CUSTOM:
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회팀", m_szTourName.c_str() );
		break;
	default:
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회팀", m_szTourName.c_str() );
		break;
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 153, FONT_SIZE_13, 127.0f, "%s", m_szTeamName.c_str() );
}

void TournamentTeamEventWnd::OnTeamDeleteRender( int iXPos, int iYPos )
{
	if( m_pIconBack2 )
	{
		m_pIconBack2->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY );
		if( m_pIcon )
			m_pIcon->Render( iXPos + 103, iYPos + 89 );
	}

	if( m_pExclamationMark )
	{
		m_pExclamationMark->Render( iXPos + 103, iYPos + 89 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	switch( m_Type )
	{
	case TournamentNode::TYPE_REGULAR:
		{
			if( m_CampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 정규 대회팀", m_szTourName.c_str() );
		}
		break;
	case TournamentNode::TYPE_CUSTOM:
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회팀", m_szTourName.c_str() );
		break;
	default:
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회팀", m_szTourName.c_str() );
		break;
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 153, FONT_SIZE_13, 127.0f, "%s", m_szTeamName.c_str() );
}

void TournamentTeamEventWnd::OnTeamLeaveRender( int iXPos, int iYPos )
{
	if( m_pIconBack2 )
	{
		m_pIconBack2->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY );
		if( m_pIcon )
			m_pIcon->Render( iXPos + 103, iYPos + 89 );
	}

	if( m_pExclamationMark )
	{
		m_pExclamationMark->Render( iXPos + 103, iYPos + 89 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	switch( m_Type )
	{
	case TournamentNode::TYPE_REGULAR:
		{
			if( m_CampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		}
		break;
	case TournamentNode::TYPE_CUSTOM:
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		break;
	default:
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s", m_szTeamName.c_str() );
	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, "대회팀에서 탈퇴되었습니다." );
}
//////////////////////////////////////////////////////////////////////////	
TournamentTeamAlarmWnd::TournamentTeamAlarmWnd()
{
	m_pIcon = NULL;
	m_pIconBack = NULL;

	m_dwTeamIndex		= 0;
	m_dwAlarmType		= 0;
	m_dwTournamentIndex	= 0;

	m_Type		= 0;
	m_CampPos	= 0;
}

TournamentTeamAlarmWnd::~TournamentTeamAlarmWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentTeamAlarmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamAlarmWnd::SetAlarmData( DWORD dwTeamIndex, DWORD dwUserIndex, DWORD dwAlarmType )
{
	m_Type = 0;
	m_dwTeamIndex = dwTeamIndex;
	m_dwAlarmType = dwAlarmType;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		TeamData &rkTeamData = pUserTournament->GetTeamData( m_dwTeamIndex );
		if( rkTeamData.m_dwTeamIndex != m_dwTeamIndex )
			return;

		m_dwTournamentIndex = rkTeamData.m_dwTourIndex;
		TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIndex, false );
		if( pTeamData == NULL )
			return;

		m_szTeamName  = pTeamData->GetTeamName();
		m_CampPos	  = pTeamData->GetCampPos();
		m_szUserName  = pUserTournament->GetTeamUserName( m_dwTeamIndex, dwUserIndex );
		UpdateTournament();
	}

	if( m_dwAlarmType == ALARM_TYPE_JOIN_USER )
	{
		SetTitleText( "대회팀 팀원 가입" );
	}
	else if( m_dwAlarmType == ALARM_TYPE_LEAVE_USER )
	{
		SetTitleText( "대회팀 팀원 탈퇴" );
	}

	ShowWnd();
}

void TournamentTeamAlarmWnd::UpdateTournament()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL )
		return;

	m_szTourTitle = pTournament->GetTournamentTitle();
	m_Type		  = pTournament->GetType();
}

void TournamentTeamAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 팀 정보 창
			TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
			if( pTeamInfo )
			{
				pTeamInfo->SetTournamentTeam( m_dwTournamentIndex, m_dwTeamIndex );
			}

			HideWnd();
		}
		break;
	}
}

void TournamentTeamAlarmWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_Type == 0 && m_dwTeamIndex != 0 && m_dwTournamentIndex != 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
		if( pTournament )
		{
			if( pTournament->GetType() != 0 )
				UpdateTournament();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTournamentIndex );
		}		
	}
}

void TournamentTeamAlarmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.55f );
		m_pIconBack->Render( iXPos + 33, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pIcon )
		{
			m_pIcon->SetScale( 0.55f );
			m_pIcon->Render( iXPos + 33, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	switch( m_CampPos )
	{
	case CAMP_BLUE:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 정규 대회팀", m_szTourTitle.c_str() );
		}
		break;
	case CAMP_RED:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 정규 대회팀", m_szTourTitle.c_str() );
		}
		break;
	default:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 대회팀", m_szTourTitle.c_str() );
		}
		break;
	}
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 71, FONT_SIZE_13, 133.0f, m_szTeamName.c_str() );

	if( m_dwAlarmType == ALARM_TYPE_JOIN_USER )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY ); 
		g_FontMgr.PrintText( iXPos + 16, iYPos + 99, FONT_SIZE_12, "새로운 팀원이 추가되었습니다." );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 117, FONT_SIZE_12, "가입자 : " );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 63, iYPos + 117, FONT_SIZE_12, 127.0f, m_szUserName.c_str() );
	}
	else if( m_dwAlarmType == ALARM_TYPE_LEAVE_USER )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY ); 
		g_FontMgr.PrintText( iXPos + 16, iYPos + 99, FONT_SIZE_12, "탈퇴자가 발생하였습니다." );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 117, FONT_SIZE_12, "탈퇴자 : " );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 63, iYPos + 117, FONT_SIZE_12, 127.0f, m_szUserName.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
DWORD TournamentBattleInviteWnd::m_dwRedIndex		= 0;
DWORD TournamentBattleInviteWnd::m_dwBlueIndex		= 0;
DWORD TournamentBattleInviteWnd::m_dwBattleIndex	= 0;
DWORD TournamentBattleInviteWnd::m_dwInviteTime		= 0;
bool  TournamentBattleInviteWnd::m_bLogSync			= false;

TournamentBattleInviteWnd::TournamentBattleInviteWnd()
{
	m_MyCampPos			= 0;
	m_OtherCampPos		= 0;
	m_dwOtherTeamIndex	= 0;
	m_dwMyTeamIndex		= 0;
	m_dwInviteCheckTime = 0;

	m_dwTournamentIndex = 0;	
	m_bLogSync			= false;
}

TournamentBattleInviteWnd::~TournamentBattleInviteWnd()
{

}

void TournamentBattleInviteWnd::iwm_show()
{
}

void TournamentBattleInviteWnd::ResetBattleData()
{
	m_dwOtherTeamIndex	= 0;
	m_dwMyTeamIndex		= 0;	
	m_szOtherTeamName.Clear();

	m_dwBattleIndex		= 0;
	m_dwBlueIndex		= 0;
	m_dwRedIndex		= 0;
}

void TournamentBattleInviteWnd::SetTournamentBattle( DWORD dwBattleIndex, DWORD dwBlueIndex, DWORD dwRedIndex, DWORD dwInviteTime )
{
	if( m_dwBattleIndex == dwBattleIndex ) 
		return;

	m_dwRedIndex    = dwRedIndex;
	m_dwBlueIndex   = dwBlueIndex;
	m_dwInviteTime  = dwInviteTime;
	m_dwBattleIndex = dwBattleIndex;
	m_bLogSync = false;
}

void TournamentBattleInviteWnd::UpdateTournamentTeam()
{
	if( m_dwMyTeamIndex != 0 && m_dwOtherTeamIndex != 0 )
		return;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		TeamData &rkBlueTeamData = pUserTournament->GetTeamData( m_dwBlueIndex );
		if( rkBlueTeamData.m_dwTeamIndex == m_dwBlueIndex )
		{
			// 내가 블루팀
			m_dwTournamentIndex = rkBlueTeamData.m_dwTourIndex;
			m_dwMyTeamIndex		= rkBlueTeamData.m_dwTeamIndex;
			m_dwOtherTeamIndex  = m_dwRedIndex;
			SyncGlobalTeamData();
			return;
		}

		TeamData &rkRedTeamData = pUserTournament->GetTeamData( m_dwRedIndex );
		if( rkRedTeamData.m_dwTeamIndex == m_dwRedIndex )
		{
			// 내가 레드팀
			m_dwTournamentIndex	= rkRedTeamData.m_dwTourIndex;
			m_dwMyTeamIndex		= rkRedTeamData.m_dwTeamIndex;
			m_dwOtherTeamIndex	= m_dwBlueIndex;
			SyncGlobalTeamData();
			return;
		}
	}
}

void TournamentBattleInviteWnd::SyncGlobalTeamData()
{
	if( FRAMEGETTIME() - m_dwInviteCheckTime > TICK_TIME )
	{
		TeamGlobalData*	OtherTeam = g_TournamentTeamMgr.GetTeamData( m_dwOtherTeamIndex );
		if( !OtherTeam )
		{
			SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_INFO );
			kPacket << m_dwTournamentIndex << m_dwOtherTeamIndex;
			TCPNetwork::SendToServer( kPacket );
			m_dwInviteCheckTime = FRAMEGETTIME();
		}
	}
}

void TournamentBattleInviteWnd::CheckSyncOtherTeamData()
{
	if( FRAMEGETTIME() - m_dwInviteCheckTime > TICK_TIME )
	{
		m_dwInviteCheckTime = FRAMEGETTIME();
		if( m_dwInviteTime > TICK_TIME )
			m_dwInviteTime -= TICK_TIME;
		else
			m_dwInviteTime = 0;

		if( m_szOtherTeamName.IsEmpty() )
		{
			GetTeamData();
		}
	}
}

void TournamentBattleInviteWnd::GetTeamData()
{
	TeamGlobalData*	MyTeam = g_TournamentTeamMgr.GetTeamData( m_dwMyTeamIndex, false );
	if( MyTeam )
	{
		m_szMyTeamName = MyTeam->GetTeamName();
		m_MyCampPos    = MyTeam->GetCampPos();

		// 정규 리그 진영 체크
		if( g_TournamentMgr.GetRegularIndex() == m_dwTournamentIndex )
		{
			if( m_MyCampPos == 0 )							// 동기화가 아직 안되었다.
				m_MyCampPos = g_MyInfo.GetUserCampPos();    // 동일 진영
		}
	}

	TeamGlobalData*	OtherTeam = g_TournamentTeamMgr.GetTeamData( m_dwOtherTeamIndex, false );
	if( OtherTeam )
	{
		m_szOtherTeamName = OtherTeam->GetTeamName();
		m_OtherCampPos    = OtherTeam->GetCampPos();
	}
}

void TournamentBattleInviteWnd::UpdateTournament()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament )
	{
		char szTitle[MAX_PATH] = "";
		if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
		{				
			sprintf_s_e( szTitle, "%s 정규 대회", pTournament->GetTournamentTitle().c_str() );
		}
		else
		{
			sprintf_s_e( szTitle, "%s 대회", pTournament->GetTournamentTitle().c_str() );
		}
		SetTitleText( szTitle );

		if( pTournament->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() && !m_bLogSync )
		{
			char szLog[MAX_PATH] = "";
			sprintf( szLog, "[대회로그] 대회경기 초대 -%s- 배틀룸 인덱스 : %d, 블루팀 : %d, 레드팀 : %d, 초대시간 %d", g_MyInfo.GetPublicID().c_str(),
				m_dwRedIndex, m_dwBlueIndex, m_dwRedIndex, m_dwInviteTime );

			SP2Packet kPacket( LUPK_LOG );
			kPacket << "TournamentLog";  // 로그 파일 타입
			kPacket << szLog;
			kPacket << 4000;   // 오류번호:4000
			kPacket << false;  // write db
			P2PNetwork::SendLog(kPacket);
			m_bLogSync = true;

			if( g_App.IsMeDeveloper() )
				LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
		}
	}
}

void TournamentBattleInviteWnd::UpdateTournamentProcess()
{
	if( m_dwTournamentIndex != 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
		if( pTournament )
		{
			if( pTournament->GetType() != 0 )
				UpdateTournament();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTournamentIndex );
		}
	}
}

void TournamentBattleInviteWnd::OnProcess( float fTimePerSec )
{	
	if( !IsShow() ) 
	{
		if( m_dwBattleIndex != 0 )
			ShowWnd();
		return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
		
	UpdateTournamentTeam();	
	UpdateTournamentProcess();
	CheckSyncOtherTeamData();

	if( m_dwInviteTime == 0 )
	{
		ResetBattleData();
		HideWnd();
	}
	else if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		if( g_BattleRoomMgr.GetIndex() == m_dwBattleIndex )
		{
			ResetBattleData();
			HideWnd();
		}
	}
}

void TournamentBattleInviteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_JOIN_BATTLE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_dwBattleIndex != 0 )
			{
				if( g_BattleRoomMgr.IsBattleRoom() || g_LadderTeamMgr.IsLadderTeam() ||
					g_ShuffleRoomMgr.IsShuffleRoom() || g_RankBattleMgr.IsRankBattlePlaying() )
				{
					// 이탈을 한 후 입장
					g_App.AltF4ExitProcess( false );
				}
				else
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_USER_BATTLEROOM_JOIN );
					// Observer << Invite << PenaltyPeso << iPenaltyPeso << .... 순서로
					kPacket << false << false << false << g_MyInfo.GetCurPenaltyPeso() << m_dwBattleIndex;
					TCPNetwork::SendToServer( kPacket );

					TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
					if( pTournament && pTournament->IsLogSend() )
					{
						char szLog[MAX_PATH] = "";					
						if( m_dwBattleIndex == -1 )
							sprintf( szLog, "[대회로그] 대회방 입장 에러 - %s(배틀룸인덱스 에러 : m_dwBattleIndex == -1 )", g_MyInfo.GetPublicID().c_str() );					
						else
							sprintf( szLog, "[대회로그] 대회 배틀룸 입장 - %s - %d", g_MyInfo.GetPublicID().c_str(), m_dwBattleIndex );

						SP2Packet kPacket2( LUPK_LOG );
						kPacket2 << "TournamentLog";  // 로그 파일 타입
						kPacket2 << szLog;
						kPacket2 << 4000;   // 오류번호:4000
						kPacket2 << false;  // write db
						P2PNetwork::SendLog(kPacket2);

						if( g_App.IsMeDeveloper() )
							LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
					}
				}
			}
		}
		break;
	}
}

void TournamentBattleInviteWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_MyCampPos == CAMP_BLUE )
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	else if( m_MyCampPos == CAMP_RED )
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );

	if( m_szMyTeamName.IsEmpty() )
		kPrinter.AddTextPiece( FONT_SIZE_12, "???????" );
	else
		kPrinter.AddTextPiece( FONT_SIZE_12, "%s ", m_szMyTeamName.c_str() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "VS " );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_OtherCampPos == CAMP_BLUE )
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	else if( m_OtherCampPos == CAMP_RED )
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	if( m_szOtherTeamName.IsEmpty() )
		kPrinter.AddTextPiece( FONT_SIZE_12, "???????" );
	else
		kPrinter.AddTextPiece( FONT_SIZE_12, "%s", m_szOtherTeamName.c_str() );
	kPrinter.PrintFullText( iXPos + 177, iYPos + 41, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "경기가 " );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );

	int iMinute = m_dwInviteTime / 60000;
	int iSec    = ( m_dwInviteTime % 60000 ) / 1000;
	if( iMinute > 0 )
		kPrinter.AddTextPiece( FONT_SIZE_12, "%d분 %d초 ", iMinute, iSec );
	else
		kPrinter.AddTextPiece( FONT_SIZE_12, "%d초 ", iSec );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "남았습니다. 경기장으로 입장해주세요." );
	kPrinter.PrintFullText( iXPos + 177, iYPos + 61, TAT_CENTER );
}

//////////////////////////////////////////////////////////////////////////

TournamentRuleAgreeConfirmWnd::TournamentRuleAgreeConfirmWnd()
{
	m_iCurPos = 0;
}

TournamentRuleAgreeConfirmWnd::~TournamentRuleAgreeConfirmWnd()
{
	m_ClauseText.clear();
}

void TournamentRuleAgreeConfirmWnd::iwm_show()
{
	if( m_ClauseText.empty() )
	{
		ioINILoader_e kLoader( "config/sp2_tournament_regulations.ini" );
		kLoader.SetTitle_e( "common" );
		int iMaxClause = kLoader.LoadInt_e( "max_regulations", 0 );

		char szBuf[MAX_PATH] = "";
		char szTitle[MAX_PATH] = "";
		for(int i = 0;i < iMaxClause;i++)
		{			
			sprintf_e( szTitle, "regulations%d", i + 1 );
			kLoader.SetTitle( szTitle );

			ClauseText kClauseText;
			kLoader.LoadString_e( "text", "", szBuf, MAX_PATH );
			kClauseText.m_szText = szBuf;
			kClauseText.m_iOffSet = kLoader.LoadInt_e( "offset", 0 );
			m_ClauseText.push_back( kClauseText );
		}
	}

	m_iCurPos = 0;
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, (int)m_ClauseText.size() - ( MAX_LINE - 1 ) );
		pScroll->SetScrollPos( m_iCurPos );
	}
}

void TournamentRuleAgreeConfirmWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	m_iCurPos = curPos;
}

void TournamentRuleAgreeConfirmWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( !pScroll ) return;
		if( !pScroll->IsShow() ) return;

		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPos - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPos + 1 );
		}
	}
}

void TournamentRuleAgreeConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void TournamentRuleAgreeConfirmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iLineCnt = 0;
	for(int i = m_iCurPos;i < (int)m_ClauseText.size();i++, iLineCnt++)
	{
		if( iLineCnt >= MAX_LINE ) break;

		ClauseText &rkText = m_ClauseText[i];

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 23 + rkText.m_iOffSet, iYPos + 57 + iLineCnt * 20, FONT_SIZE_13, rkText.m_szText.c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////

TournamentMatchWinAlarmWnd::TournamentMatchWinAlarmWnd()
{
	m_pVictroyIcon = NULL;
	m_pVictoryBack = NULL;
}

TournamentMatchWinAlarmWnd::~TournamentMatchWinAlarmWnd()
{
	SAFEDELETE( m_pVictroyIcon );
	SAFEDELETE( m_pVictoryBack );
}

void TournamentMatchWinAlarmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "VictroyIcon" )
	{
		SAFEDELETE( m_pVictroyIcon );
		m_pVictroyIcon = pImage;
	}
	else if( szType == "VictroyBack" )
	{
		SAFEDELETE( m_pVictoryBack );
		m_pVictoryBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentMatchWinAlarmWnd::SetModeType( DWORD dwModeType )
{
	m_dwModeType = dwModeType;
}

void TournamentMatchWinAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CLOSE_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 한쪽팀이 입장하지 않아서 부전승 처리되며 로비로 이탈함
			if( m_dwModeType != MT_MYROOM && m_dwModeType != MT_TRAINING && m_dwModeType != MT_HEADQUARTERS && m_dwModeType != MT_HOUSE )
			{
				SP2Packet kPacket( CTPK_EXIT_ROOM );
				kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
				TCPNetwork::SendToServer( kPacket );
				g_BattleRoomMgr.LeaveRoom();
			}
			else
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_USER_BATTLEROOM_LEAVE );
				TCPNetwork::SendToServer( kPacket );
			}			
			HideWnd();
		}
		break;
	}
}

bool TournamentMatchWinAlarmWnd::iwm_esc()
{
	// 한쪽팀이 입장하지 않아서 부전승 처리되며 로비로 이탈함
	if( m_dwModeType != MT_MYROOM && m_dwModeType != MT_TRAINING && m_dwModeType != MT_HEADQUARTERS && m_dwModeType != MT_HOUSE )
	{
		SP2Packet kPacket( CTPK_EXIT_ROOM );
		kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
		TCPNetwork::SendToServer( kPacket );
		g_BattleRoomMgr.LeaveRoom();
	}
	else
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_USER_BATTLEROOM_LEAVE );
		TCPNetwork::SendToServer( kPacket );
	}
	HideWnd();

	return true;
}

void TournamentMatchWinAlarmWnd::OnVictoryRender( int iXPos, int iYPos )
{
	if( !m_pVictoryBack || !m_pVictroyIcon )
		return;

	m_pVictoryBack->Render( iXPos + 125, iYPos + 98, UI_RENDER_MULTIPLY );
	m_pVictroyIcon->Render( iXPos + 125, iYPos + 98 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, "승리를 축하합니다" );
	kPrinter.PrintFullText( iXPos + 125, iYPos + 146, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "다음 경기가 있는지 꼭 확인하세요" );
	kPrinter.PrintFullText( iXPos + 125, iYPos + 169, TAT_CENTER );

}

void TournamentMatchWinAlarmWnd::OnRender()
{
	if( !IsShow() )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnVictoryRender( iXPos, iYPos );	
}

//////////////////////////////////////////////////////////////////////////
TournamentUnearnedWinAlarmWnd::TournamentUnearnedWinAlarmWnd()
{
	m_pVictroyIcon = NULL;
	m_pVictoryBack = NULL;

	m_TournamentType  = 0;
	m_iTournamentRoundWin = 0;
}

TournamentUnearnedWinAlarmWnd::~TournamentUnearnedWinAlarmWnd()
{
	SAFEDELETE( m_pVictroyIcon );
	SAFEDELETE( m_pVictoryBack );
}

void TournamentUnearnedWinAlarmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "VictroyIcon" )
	{
		SAFEDELETE( m_pVictroyIcon );
		m_pVictroyIcon = pImage;
	}
	else if( szType == "VictroyBack" )
	{
		SAFEDELETE( m_pVictoryBack );
		m_pVictoryBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentUnearnedWinAlarmWnd::SetTournamentData( ioHashString szTournamentName, int iGoRound, BYTE TournamentType )
{
	m_szTournamentName = szTournamentName;
	m_iTournamentRoundWin = iGoRound;
	m_TournamentType = TournamentType;
}

void TournamentUnearnedWinAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CLOSE_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.IsTournamentModeType() )
				g_BattleRoomMgr.LeaveRoom();
			HideWnd();
		}
		break;
	}
}

bool TournamentUnearnedWinAlarmWnd::iwm_esc()
{
	if( g_BattleRoomMgr.IsTournamentModeType() )
		g_BattleRoomMgr.LeaveRoom();
	HideWnd();

	return true;
}

void TournamentUnearnedWinAlarmWnd::OnRender()
{
	if( !IsShow() )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnVictoryRender( iXPos, iYPos );
}

void TournamentUnearnedWinAlarmWnd::OnVictoryRender( int iXPos, int iYPos )
{
	if( !m_pVictoryBack || !m_pVictroyIcon )
		return;

	m_pVictoryBack->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY );
	m_pVictroyIcon->Render( iXPos + 103, iYPos + 89 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	
	if( m_TournamentType == TournamentNode::TYPE_REGULAR )
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "%s 정규대회", m_szTournamentName.c_str() );
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_13, "%s 대회", m_szTournamentName.c_str() );
	}

	kPrinter.PrintFullText( iXPos + 103, iYPos + 135, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	if( m_TournamentType == TournamentNode::TYPE_REGULAR )	
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );

	if( m_iTournamentRoundWin <= 2 )
		kPrinter.AddTextPiece( FONT_SIZE_13, "최종 우승!!", m_iTournamentRoundWin );
	else if( m_iTournamentRoundWin <= 4 )
		kPrinter.AddTextPiece( FONT_SIZE_13, "결승 진출!!", m_iTournamentRoundWin );
	else
		kPrinter.AddTextPiece( FONT_SIZE_13, "%d 강 부전승", m_iTournamentRoundWin );

	kPrinter.PrintFullText( iXPos + 103, iYPos + 153, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
TournamentEtcItemNeedPopup::TournamentEtcItemNeedPopup()
{
	m_pIconBack = NULL;
	m_pIcon		= NULL;
}

TournamentEtcItemNeedPopup::~TournamentEtcItemNeedPopup()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void TournamentEtcItemNeedPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_CREATE_ITEM_BUY:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_TOURNAMENT_CREATE, 0 );
			}
		}
		break;
	}
}

void TournamentEtcItemNeedPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentEtcItemNeedPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, "[대회 생성권]이 필요합니다." );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "대회를 개최할 수 있는 기능이며," );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "상점 - 특별에서 구입하실 수 있습니다." );
}
//////////////////////////////////////////////////////////////////////////
TournamentEtcItemUsePopup::TournamentEtcItemUsePopup() : m_dwEtcItem( ioEtcItem::EIT_ETC_TOURNAMENT_CREATE ), m_dwPremiumEtcItem( ioEtcItem::EIT_ETC_TOURNAMENT_PREMIUM_CREATE )
{
	m_pIconBack = NULL;
	m_pIcon		= NULL;

	m_dwRadioBtnID = 0;
}

TournamentEtcItemUsePopup::~TournamentEtcItemUsePopup()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void TournamentEtcItemUsePopup::SetRadioButton( DWORD dwID )
{
	m_dwRadioBtnID = dwID;
	CheckRadioButton( ID_RADIO_NORMAL, ID_RADIO_PREMIUM, m_dwRadioBtnID );

	switch( m_dwRadioBtnID )
	{
	case ID_RADIO_NORMAL:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItem );
			if( pEtcItem )
			{
				m_szRadioTitle = pEtcItem->GetName();
			}
		}
		break;
	case ID_RADIO_PREMIUM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwPremiumEtcItem );
			if( pEtcItem )
			{
				m_szRadioTitle = pEtcItem->GetName();
			}
		}
		break;
	}
}

void TournamentEtcItemUsePopup::iwm_show()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem == NULL ) return;

	SetChildActive( ID_RADIO_NORMAL );
	SetChildActive( ID_RADIO_PREMIUM );

	ioUserEtcItem::ETCITEMSLOT kCreateEtc;				 // 일반
	ioUserEtcItem::ETCITEMSLOT kPremiumCreateEtc;		 // 프리미엄
	if( pUserEtcItem->GetEtcItem( m_dwEtcItem, kCreateEtc ) )
	{
		SetRadioButton( ID_RADIO_NORMAL );

		if( !pUserEtcItem->GetEtcItem( m_dwPremiumEtcItem, kPremiumCreateEtc ) )
		{
			SetChildInActive( ID_RADIO_PREMIUM );
		}
	}
	else if( pUserEtcItem->GetEtcItem( m_dwPremiumEtcItem, kPremiumCreateEtc ) )
	{
		SetRadioButton( ID_RADIO_PREMIUM );
		SetChildInActive( ID_RADIO_NORMAL );
	}
	else
	{
		HideWnd();
	}
}

void TournamentEtcItemUsePopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			// 대회 생성 UI
			CustomTournamentCreateWnd* pWnd = dynamic_cast<CustomTournamentCreateWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_CREATE_WND ) );
			if( pWnd )
			{	
				if( 0 != m_dwEtcItem )
					pWnd->UseEtcItem( m_dwEtcItem );
				else
					pWnd->UseEtcItem( m_dwPremiumEtcItem );
			}
			HideWnd();			
		}
		break;
	case ID_RADIO_NORMAL:
	case ID_RADIO_PREMIUM:
		if( cmd == IOBN_BTNDOWN )
		{
			SetRadioButton( dwID );
		}
		break;
	}
}

void TournamentEtcItemUsePopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentEtcItemUsePopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, "보유 중인 [%s]을", m_szRadioTitle.c_str() );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "사용하시겠습니까?" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "대회를 개최할 수 있는 기능입니다." );
}


//////////////////////////////////////////////////////////////////////////
CusotmTournamentTeamListBtn::CusotmTournamentTeamListBtn()
{
	m_dwTeamIdx = 0;
	m_dwTourIdx = 0;
}

CusotmTournamentTeamListBtn::~CusotmTournamentTeamListBtn()
{

}

void CusotmTournamentTeamListBtn::SetTeamInfo( DWORD dwTourIdx, DWORD dwTeamIdx )
{
	m_szTeamName.Clear();
	m_dwTourIdx = dwTourIdx;
	m_dwTeamIdx = dwTeamIdx;	

	if( m_dwTeamIdx != 0 )
	{
		TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( m_dwTeamIdx, false );
		if( pTeamData )
		{
			m_szTeamName = pTeamData->GetTeamName();
		}
		SetActive();
	}
	else
	{
		SetInActive();
	}
}

void CusotmTournamentTeamListBtn::OnRender()
{
	ioButton::OnRender();

	if( m_dwTeamIdx == 0 )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 76, iYPos + 4, FONT_SIZE_12, 138, m_szTeamName.c_str() );
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentEntryListWnd::CustomTournamentEntryListWnd()
{
	m_dwSearchTime = 0;
	m_dwMaxTeamCnt = 0;

	m_iCurPage = 0;
	m_iMaxPage = 0;

	m_dwTourIndex = 0;
}

CustomTournamentEntryListWnd::~CustomTournamentEntryListWnd()
{
}

void CustomTournamentEntryListWnd::StartSend( bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;

	if( m_dwTourIndex == 0 )
		return;

	SP2Packet kPacket( CTPK_TOURNAMENT_TOTAL_TEAM_LIST );
	kPacket << m_dwTourIndex << m_iCurPage << MAX_TEAM_LIST;
	TCPNetwork::SendToServer( kPacket );
	m_dwSearchTime = FRAMEGETTIME();
}

void CustomTournamentEntryListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > TEAM_SEARCH_TIME )
	{
		StartSend( true );
	}
}
void CustomTournamentEntryListWnd::UpdateSetPage( int iCurPage )
{
	if( !COMPARE( iCurPage, 0, m_iMaxPage + 1 ) )
		return;

	int iPrevPage = m_iCurPage;
	m_iCurPage    = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend();
}

void CustomTournamentEntryListWnd::ApplyCustomTournamentEntryList( SP2Packet &rkPacket )
{
	int iCurSize;
	DWORD dwTourIdx;
	rkPacket >> dwTourIdx >> m_iCurPage >> m_dwMaxTeamCnt >> iCurSize;
	
	m_iMaxPage = m_dwMaxTeamCnt / MAX_TEAM_LIST;
	
	int i = 0;
	for( int iID = ID_TEAM_01; iID <= ID_TEAM_16; ++iID, ++i )
	{
		DWORD dwTeamIndex = 0;
		ioHashString szTeamName;
		
		if( i < iCurSize )
		{
			rkPacket >> dwTeamIndex >> szTeamName;

			TeamData kTeamData;
			kTeamData.m_dwTourIndex = m_dwTourIndex;
			kTeamData.m_dwTeamIndex = dwTeamIndex;
			kTeamData.m_szTeamName  = szTeamName;
			g_TournamentTeamMgr.SetDefaultData( kTeamData, true );
		}

		CusotmTournamentTeamListBtn* pBtn = dynamic_cast<CusotmTournamentTeamListBtn*>( FindChildWnd( iID ) );
		if( pBtn )
			pBtn->SetTeamInfo( dwTourIdx, dwTeamIndex );
	}
}

void CustomTournamentEntryListWnd::SetTournament( DWORD dwTourIndex )
{
	m_dwTourIndex = dwTourIndex;

	TournamentNode* pNode = g_TournamentMgr.GetTournament( dwTourIndex, false );
	if( pNode )
	{
		m_ProgressType = pNode->GetCustomProgressType();
		ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_JOIN_REQUEST_LIST ) );

		if(pButton)
		{
			if( m_ProgressType == TournamentNode::CPT_OFFLINE )
			{
				int iUserCount = pNode->GetTouramentJoinUserCount();
				char szTtitle[MAX_PATH];
				sprintf_s_e( szTtitle, "팀 생성권한 요청자 보기 (%d팀)", iUserCount );
				pButton->SetTitleText( szTtitle );
			}
			else
			{
				pButton->SetTitleText( "닫기 (ESC)" );
			}
		}
	}

	ShowWnd();
}

void CustomTournamentEntryListWnd::iwm_show()
{
	m_iCurPage = 0;
	m_iMaxPage = 0;

	StartSend();
}

void CustomTournamentEntryListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage + 1 );
		}
		break;
	case ID_JOIN_REQUEST_LIST:
		if( cmd == IOBN_BTNUP )
		{
			if( m_ProgressType == TournamentNode::CPT_OFFLINE )
			{
				CustomTournamentDelayCreateTeamListWnd* pWnd = dynamic_cast<CustomTournamentDelayCreateTeamListWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_DELAY_CREATE_TEAM_LIST_WND ) );
				if( pWnd )
				{
					pWnd->SetAutoShow( m_dwTourIndex, false );
				}
			}
			else
			{
				HideWnd();
			}
		}
		break;
	case ID_TEAM_01:
	case ID_TEAM_02:
	case ID_TEAM_03:
	case ID_TEAM_04:
	case ID_TEAM_05:
	case ID_TEAM_06:
	case ID_TEAM_07:
	case ID_TEAM_08:
	case ID_TEAM_09:
	case ID_TEAM_10:
	case ID_TEAM_11:
	case ID_TEAM_12:
	case ID_TEAM_13:
	case ID_TEAM_14:
	case ID_TEAM_15:
	case ID_TEAM_16:
		if( cmd == IOBN_BTNUP )
		{
			CusotmTournamentTeamListBtn *pButton = dynamic_cast< CusotmTournamentTeamListBtn* >( pWnd );
			if( pButton )
			{
				// 팀 정보 창
				TournamentTeamInfoWnd *pTeamInfo = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
				if( pTeamInfo )
				{
					pTeamInfo->SetTournamentTeam( pButton->GetTourIndex(), pButton->GetTeamIndex() );
				}
			}
		}
		break;
	}
}

void CustomTournamentEntryListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessSearch();		
	ioMovingWnd::OnProcess( fTimePerSec );
}

void CustomTournamentEntryListWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 170, iYPos + 253, TAT_CENTER );
}

//////////////////////////////////////////////////////////////////////////
CustomTournamentDelayCreateTeamListBtn::CustomTournamentDelayCreateTeamListBtn()
{
	m_dwMsgBoxID     = 0;
	m_dwTourIndex    = 0;
	m_dwUserIndex    = 0;
	m_iUserLevel     = 0;
	m_iMaxList		 = 0;
}

CustomTournamentDelayCreateTeamListBtn::~CustomTournamentDelayCreateTeamListBtn()
{
}

void CustomTournamentDelayCreateTeamListBtn::SetInfo( int iMaxList, DWORD dwTourIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID )
{
	m_iMaxList      = iMaxList;
	m_dwTourIndex   = dwTourIndex;
	m_dwUserIndex	= dwUserIndex;
	m_iUserLevel    = iUserLevel;
	m_szUserID      = szUserID;

	if( m_dwTourIndex == 0 || m_szUserID.IsEmpty() )
	{
		SetInActive();
		HideChildWnd( ID_AGREE );
		HideChildWnd( ID_REFUSE );
	}
	else 
	{
		SetActive();
		ShowChildWnd( ID_AGREE );
		ShowChildWnd( ID_REFUSE );
	}
	m_dwMsgBoxID = 0;
}

bool CustomTournamentDelayCreateTeamListBtn::IsError( DWORD dwID )
{
	if( dwID == ID_AGREE )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
		if( pTournament )
		{
			if( pTournament->GetState() != TournamentNode::STATE_TEAM_APP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "팀모집 기간이 종료되었습니다." );
				return true;
			}
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 정보가 없습니다#잠시 후 다시 시도해주세요" );
			return true;
		}
	}

	return false;
}

void CustomTournamentDelayCreateTeamListBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			if( !IsError( dwID ) )
			{
				m_dwMsgBoxID = ID_AGREE;
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "%s님의 팀 생성 신청을#수락 하시겠습니까?", m_szUserID.c_str() );			
			}
		}
		break;
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
		{
			if( !IsError( dwID ) )
			{
				m_dwMsgBoxID = ID_REFUSE;
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "%s님의 팀 생성 신청을#거절 하시겠습니까?", m_szUserID.c_str() );		
			}
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_dwMsgBoxID == ID_AGREE )
				{
					if( !IsError( m_dwMsgBoxID ) )
					{
						//Send
						SP2Packet kPacket( CTPK_TOURNAMENT_JOIN_CONFIRM_COMMAND );
						kPacket << TOURNAMENT_JOIN_CONFIRM_COMMAND_AGREE << m_dwTourIndex << m_dwUserIndex << m_szUserID << m_iUserLevel;
						TCPNetwork::SendToServer( kPacket );

						CustomTournamentDelayCreateTeamListWnd* pParent = dynamic_cast<CustomTournamentDelayCreateTeamListWnd*>( GetParent() );
						if( pParent )
							pParent->DeleteDelayCreateTeamList( m_dwUserIndex );
					}
				}
				else if( m_dwMsgBoxID == ID_REFUSE )
				{
					if( !IsError( m_dwMsgBoxID ) )
					{
						//Send
						SP2Packet kPacket( CTPK_TOURNAMENT_JOIN_CONFIRM_COMMAND );
						kPacket << TOURNAMENT_JOIN_CONFIRM_COMMAND_REJECT << m_dwTourIndex << m_dwUserIndex << m_szUserID << m_iUserLevel;
						TCPNetwork::SendToServer( kPacket );

						CustomTournamentDelayCreateTeamListWnd* pParent = dynamic_cast<CustomTournamentDelayCreateTeamListWnd*>( GetParent() );
						if( pParent )
							pParent->DeleteDelayCreateTeamList( m_dwUserIndex );
					}
				}
			}
		}
		break;
	}
}

void CustomTournamentDelayCreateTeamListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_szUserID.IsEmpty() )
	{
		if( m_iMaxList == 0 && GetID() == TournamentTeamEntryDelayUserWnd::ID_LIST_3 )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintText( iXPos + 86, iYPos + 3, FONT_SIZE_12, "대기자가 없습니다" );
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( m_szUserID, iXPos + 4, iYPos + 2, m_iUserLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 79.0f, m_szUserID.c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////
bool CustomTournamentDelayCreateTeamListWnd::sg_bAutoShowIgnore = false;
CustomTournamentDelayCreateTeamListWnd::CustomTournamentDelayCreateTeamListWnd()
{
	m_pBottomFrm = NULL;
	m_pIcon      = NULL;
	m_pIconBack	 = NULL;

	m_dwTourIndex = 0;
	m_iCurPage = m_iMaxPage = 0;

	m_bAutoShow = true; 
	m_TournamentType = 0;
}

CustomTournamentDelayCreateTeamListWnd::~CustomTournamentDelayCreateTeamListWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void CustomTournamentDelayCreateTeamListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentDelayCreateTeamListWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void CustomTournamentDelayCreateTeamListWnd::iwm_show()
{
	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AUTO_SHOW_IGNORE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bAutoShowIgnore );

	ReSizeWnd();

	if( m_bAutoShow ) 
		UpdateSetPage( 0 );
	else 
		UpdateSetPage( INT_MAX );
}

void CustomTournamentDelayCreateTeamListWnd::ReSizeWnd()
{
	if( m_bAutoShow )
	{
		ShowChildWnd( ID_AUTO_SHOW_IGNORE );
		SetSize( GetWidth(), AUTO_SHOW_HEIGHT );
		SetWndPos( Setting::Width() - ( GetWidth() + 1 ), 1 );
	}
	else
	{
		HideChildWnd( ID_AUTO_SHOW_IGNORE );
		SetSize( GetWidth(), CUSTOM_SHOW_HEIGHT );
		SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ), ( Setting::Height() / 2 ) - ( ( GetHeight() / 2 ) + 20 ) );
	}
}

void CustomTournamentDelayCreateTeamListWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		ReSizeWnd();
	}
}

void CustomTournamentDelayCreateTeamListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage + 1 );
		}
		break;
	case ID_AUTO_SHOW_IGNORE:
		if( cmd == IOBN_BTNUP )
		{
			sg_bAutoShowIgnore = !sg_bAutoShowIgnore;
			if( sg_bAutoShowIgnore )
				HideWnd();
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
		if( cmd == IOBN_BTNUP )
		{
			CustomTournamentDelayCreateTeamListBtn *pUserBtn = dynamic_cast<CustomTournamentDelayCreateTeamListBtn*>( pWnd );
			if( pUserBtn )
			{
				g_App.OpenMannerWnd( pUserBtn->GetUserName().c_str(), false );
			}
		}
		break;
	}
}

void CustomTournamentDelayCreateTeamListWnd::DeleteDelayCreateTeamList( DWORD dwUserIndex )
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( !pNode )
		return;
	
	pNode->RemoveTournamentJoinConfirmUser( dwUserIndex );
	UpdateSetPage( m_iCurPage );
}

void CustomTournamentDelayCreateTeamListWnd::UpdateSetPage( int iCurPage )
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( !pNode )
		return;

	int iUserCount = pNode->GetTouramentJoinUserCount();
	m_iMaxPage = max( 0, iUserCount - 1 ) / MAX_LIST;
	m_iCurPage = max( 0, iCurPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );

	int iStartArray = m_iCurPage * MAX_LIST;
	for( int i = 0; i < MAX_LIST; i++, iStartArray++ )
	{
		CustomTournamentDelayCreateTeamListBtn *pItem = (CustomTournamentDelayCreateTeamListBtn*)FindChildWnd( ID_LIST_1 + i );
		if( pItem )
		{
			if( iStartArray < iUserCount )
			{
				const TournamentNode::TeamCreateRequestUser& kInfo = pNode->GetTournamentJoinConfirmUser( iStartArray );
				pItem->SetInfo( iUserCount, m_dwTourIndex, kInfo.m_dwUserIndex, kInfo.m_iUserLevel, kInfo.m_szUserID );
			}
			else
			{
				pItem->SetInfo( iUserCount, 0, 0, 0, "" );
			}
		}
	}
}

void CustomTournamentDelayCreateTeamListWnd::SetAutoShow( DWORD dwTourIdx, bool bAutoShow )
{
	if( sg_bAutoShowIgnore && bAutoShow )
		return;

	m_TournamentType = 0;
	m_dwTourIndex = dwTourIdx;
	m_bAutoShow   = bAutoShow;

	UpdateTournament();
	ShowWnd();
}

void CustomTournamentDelayCreateTeamListWnd::UpdateTournament()
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( pNode )
	{
		m_szTournamentTitle = pNode->GetTournamentTitle();
		m_TournamentType    = pNode->GetType();
	}
}

void CustomTournamentDelayCreateTeamListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_TournamentType == 0 && m_dwTourIndex != 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
		if( pTournament )
		{
			if( pTournament->GetType() != 0 )
				UpdateTournament();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTourIndex );
		}
	}
}

void CustomTournamentDelayCreateTeamListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.55f );
		m_pIconBack->Render( iXPos + 33, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pIcon )
	{
		m_pIcon->SetScale( 0.55f );
		m_pIcon->Render( iXPos + 33, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	
	if( m_bAutoShow )
	{
		if( m_pBottomFrm )
			m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, "%s 대회", m_szTournamentTitle.c_str() );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 68, FONT_SIZE_13, 133.0f, "팀 생성 권한요청 도착" );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 214, TAT_CENTER );
}

//////////////////////////////////////////////////////////////////////////
TournamentJoinConfirmAgreeWnd::TournamentJoinConfirmAgreeWnd()
{
	m_pIcon	= NULL;
	m_pIconBack = NULL;

	m_dwTournamentIndex		 = 0;
	m_TournamentProgressType = 0;
	m_TournamentType		 = 0;
}

TournamentJoinConfirmAgreeWnd::~TournamentJoinConfirmAgreeWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentJoinConfirmAgreeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentJoinConfirmAgreeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void TournamentJoinConfirmAgreeWnd::SetTournament( DWORD dwTourIndex, TourJoinType eType )
{
	m_TournamentType	= 0;
	m_dwTournamentIndex = dwTourIndex;

	m_eType = eType;
	if( m_eType == TOUR_JOIN_REQUEST )
	{
		SetTitleText( "팀 생성 권한 획득" );
	}
	else
	{
		SetTitleText( "대회 초대" );
	}

	ShowWnd();
}

void TournamentJoinConfirmAgreeWnd::UpdateTournament()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament )
	{	
		m_szTournamentTitle		 = pTournament->GetTournamentTitle();
		m_TournamentProgressType = pTournament->GetCustomProgressType();
		m_TournamentType		 = pTournament->GetType();
	}
}

void TournamentJoinConfirmAgreeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	if( m_TournamentType == 0 && m_dwTournamentIndex != 0 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
		if( pTournament )
		{
			if( pTournament->GetType() != 0 )
				UpdateTournament();
		}
		else
		{
			g_TournamentMgr.CreateTournament( m_dwTournamentIndex );
		}
	}
}

void TournamentJoinConfirmAgreeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 103, iYPos + 89, UI_RENDER_MULTIPLY );
	}

	if( m_pIcon )
	{
		m_pIcon->Render( iXPos + 103, iYPos + 89 );
	}

	if( m_TournamentProgressType == TournamentNode::CPT_ONLINE )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회", m_szTournamentTitle.c_str() );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, "초대 되었습니다." );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 135, FONT_SIZE_13, 127.0f, "%s 대회", m_szTournamentTitle.c_str() );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 153, FONT_SIZE_13, "팀 생성 권한을 얻었습니다." );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 171, FONT_SIZE_13, "대회-토너먼트 탭에서" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 189, FONT_SIZE_13, "팀을 생성해 주세요" );	
}

//////////////////////////////////////////////////////////////////////////
TournamentInvitationWnd::TournamentInvitationWnd()
{
	m_pPreEdit = NULL;
	m_pNameEdit= NULL;

	m_dwTourIndex = 0;
}

TournamentInvitationWnd::~TournamentInvitationWnd()
{

}

void TournamentInvitationWnd::SetTournamentInvitation( DWORD dwTourIndex, DWORD dwTourOwnerIndex )
{
	m_dwTourIndex       = dwTourIndex;
	m_dwTourOwnerIndex  = dwTourOwnerIndex;

	ShowWnd();
}

void TournamentInvitationWnd::SendInvitation()
{
	if( g_App.IsMouseBusy() ) return;	

	ioHashString szUserName;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	if( pEdit )
	{
		szUserName = pEdit->GetText();
		pEdit->SetKeyFocus();
	}

	if( szUserName.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "닉네임을 입력하세요" );
		return;
	}

	if( !g_App.IsRightID( szUserName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 닉네임입니다" );
		return;
	}

	if( szUserName == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "자신의 닉네임입니다" );
		return;
	}

	if( g_App.IsAdminID( szUserName.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "개발자나 마스터에게 가입권유는 안됩니다" );
		return;
	}

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex, false );
	if( pTournament == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회정보가 존재하지 않습니다#잠시 후 다시 시도해주세요" );
		return;
	}

	SP2Packet kPacket( CTPK_TOURNAMENT_JOIN_CONFIRM_COMMAND );
	kPacket << TOURNAMENT_JOIN_CONFIRM_COMMAND_INVITE << m_dwTourIndex << szUserName << pTournament->GetTournamentTitle();
	TCPNetwork::SendToServer( kPacket );
	HideWnd();
}

void TournamentInvitationWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_SEND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendInvitation();
		}
		break;
	case ID_FRIEND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamInvitationFriendWnd *pFriendListWnd = dynamic_cast<TournamentTeamInvitationFriendWnd*>( FindChildWnd( ID_FRIEND_LIST ) );
			if( pFriendListWnd )
			{
				if( pFriendListWnd->IsShow() )
					pFriendListWnd->HideWnd();
				else
					pFriendListWnd->SetTournamentOwenrIndex( m_dwTourOwnerIndex );
			}
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				SendInvitation();
			}
		}
		break;
	case ID_FRIEND_LIST:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamInvitationFriendWnd *pFriendWnd = dynamic_cast<TournamentTeamInvitationFriendWnd*>( pWnd );
			if( pFriendWnd )
			{
				g_InputBox.SetString( pFriendWnd->GetOverUser().c_str() );
			}
		}
		break;
	}
}

void TournamentInvitationWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	m_pNameEdit = FindChildWnd( ID_NAME_EDIT );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void TournamentInvitationWnd::iwm_hide()
{
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();
		}
	}
	m_pPreEdit = NULL;
}

bool TournamentInvitationWnd::iwm_esc()
{
	TournamentTeamInvitationFriendWnd *pFriendListWnd = dynamic_cast<TournamentTeamInvitationFriendWnd*>( FindChildWnd( ID_FRIEND_LIST ) );
	if( pFriendListWnd && pFriendListWnd->IsShow() )
	{
		pFriendListWnd->HideWnd();
		return true;
	}
	return ioWnd::iwm_esc();
}

void TournamentInvitationWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void TournamentInvitationWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, "참가 권유 메시지를 받을 닉네임을 입력하세요" );
};