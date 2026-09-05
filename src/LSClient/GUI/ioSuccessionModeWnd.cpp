#include "StdAfx.h"
#include "ioSuccessionModeWnd.h"

SuccessionBattleWnd::SuccessionBattleWnd(void)
{
	m_iMatchState = STATE_NONE;
	m_pIconBack = NULL;
	m_pIcon = NULL;
	m_pMaxWinNum = NULL;
	m_pCurWinNum = NULL;
	m_bWaitShow = false;
}

SuccessionBattleWnd::~SuccessionBattleWnd(void)
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pMaxWinNum ); 
	SAFEDELETE( m_pCurWinNum );
}

void SuccessionBattleWnd::iwm_create()
{
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "실력이 비슷한 유저와 매칭이 되어 다른 유저에게" );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "방해 받지 않고 1vs1로 대전을 할 수 있는 모드" );

	m_szDescArray[2].ClearList();
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_13, "입니다. 연승을 할 수록 보상이 커지며, 진 경우엔" );

	m_szDescArray[3].ClearList();
	m_szDescArray[3].SetTextStyle( TS_NORMAL );
	m_szDescArray[3].SetBkColor( 0, 0, 0 );	
	m_szDescArray[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[3].AddTextPiece( FONT_SIZE_13, "같은 상대에게 재 대결을 신청 할 수 있습니다." );
}

void SuccessionBattleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SuccessionIconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "SuccessionIcon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "MaxWinNum" )
	{
		SAFEDELETE( m_pMaxWinNum );
		m_pMaxWinNum = pImage;
	}
	else if( szType == "CurWinNum" )
	{
		SAFEDELETE( m_pCurWinNum );
		m_pCurWinNum = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );	
}

void SuccessionBattleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo(xElement);
	m_dwMatchingSearchTime = xElement.GetIntAttribute_e( "MatchingSearchTime" );
	m_szMatchingSearchMent = xElement.GetStringAttribute_e( "MatchingSearchMent");
	m_dwMatchingCancelTime = xElement.GetIntAttribute_e( "MatchingCancelTime");
	m_szMatchingEventMent = xElement.GetStringAttribute_e( "MatchingEventMent");
}

void SuccessionBattleWnd::iwm_show()
{
	m_iMatchState = STATE_NONE;

	ShowChildWnd( ID_START );
	HideChildWnd( ID_CANCEL );
	SetChildActive( ID_CANCEL );
	ioButton *pBtn = static_cast<ioButton*>( FindChildWnd(ID_CANCEL) );
	if( pBtn )
	{
		pBtn->SetTitleText( "매칭취소" );
	}

	m_dwMatchingCancelCheckTime = 0;
}

void SuccessionBattleWnd::iwm_hide()
{
	if( g_App.IsMouseBusy() )
	{
		TCPNetwork::MouseBusy( false );
	}

	if( m_iMatchState == STATE_MATCHING )
	{
		if( P2PNetwork::IsNetworkPlaying() )
		{
			SP2Packet kPacket( CTPK_SUCCESSION_MATCHING_CANCEL );
			TCPNetwork::SendToServer( kPacket );
		}
	}
}

void SuccessionBattleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			if( HasWndStyle( IWS_MODAL ) )
				RemoveModal();
			HideWnd();
		}
		break;
	case ID_START:
		if( cmd == IOBN_BTNUP )
		{
			HideChildWnd( ID_START );
			ShowChildWnd( ID_CANCEL );
			SetChildActive( ID_CANCEL );
			ioButton *pBtn = static_cast<ioButton*>( FindChildWnd(ID_CANCEL) );
			if( pBtn )
			{
				pBtn->SetTitleText( "매칭취소" );
			}

			if( P2PNetwork::IsNetworkPlaying() )
			{
				SP2Packet kPacket( CTPK_SUCCESSION_MATCHING_REQUEST );
				TCPNetwork::SendToServer( kPacket );

				m_iMatchState = STATE_MATCHING;
			}

			WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
			if ( pEffect )
			{
				m_bWaitShow = true;
				pEffect->StartSearch( this, false, m_dwMatchingSearchTime, m_szMatchingSearchMent );
			}

			AddModal();
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_SUCCESSION_MATCHING_CANCEL );
				TCPNetwork::SendToServer( kPacket );

				m_iMatchState = STATE_NONE;

				ioButton *pBtn = static_cast<ioButton*>( pWnd );
				if( pBtn )
					pBtn->SetTitleText( "취소중.." );
			}
			else
				MatchingCancel();
		}
		break;
	}
}

bool SuccessionBattleWnd::iwm_spacebar()
{
	ioButton *pStartBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_START ) );
	if ( pStartBtn && pStartBtn->IsShow() )
	{
		iwm_command( pStartBtn, IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

bool SuccessionBattleWnd::iwm_esc()
{
	if( HasWndStyle( IWS_EXIT_ESC ) )
	{
		if( m_iMatchState == STATE_MATCHING && g_App.IsMouseBusy() )
			TCPNetwork::MouseBusy( false );

		if( HasWndStyle( IWS_MODAL ) )
			RemoveModal();

		HideWnd();
		return true;
	}
	return false;
}

void SuccessionBattleWnd::OnProcess( float fTimePerSec )
{
	if( m_dwMatchingCancelCheckTime > 0 )
	{
		if( m_dwMatchingCancelCheckTime + m_dwMatchingCancelTime < FRAMEGETTIME() )
		{
			m_dwMatchingCancelCheckTime = 0;

			HideChildWnd( ID_CANCEL );
			ShowChildWnd( ID_START );
			TCPNetwork::MouseBusy( false );
		}
		else
		{
			DWORD dwGap = ( m_dwMatchingCancelCheckTime + m_dwMatchingCancelTime ) - FRAMEGETTIME();
			ioButton *pBtn = static_cast<ioButton*>( FindChildWnd(ID_CANCEL) );
			if( pBtn )
			{
				char szBuf[MAX_PATH] = "";
				wsprintf( szBuf, "취소중..%d", dwGap/1000+1 );
				pBtn->SetTitleText( szBuf );
			}
		}
	}
}

void SuccessionBattleWnd::OnRender()
{
	enum 
	{
		X_ICON_BACK_OFFSET		= 54,
		Y_ICON_BACK_OFFSET		= 91,

		X_TOP_TEXT_OFFSET       = 99,
		Y_TOP_TEXT_OFFSET       = 54,
		Y_LINE_GAP				= 20,

		WIDTHCUT_SIZE			= 275,

		X_BOTTOM_TEXT_OFFSET	= 195,
		Y_BOTTOM_TEXT_OFFSET	= 268,
	};

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	//아이콘
	if ( m_pIconBack )
		m_pIconBack->Render( iXPos + X_ICON_BACK_OFFSET, iYPos + Y_ICON_BACK_OFFSET, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	if ( m_pIcon )
		m_pIcon->Render( iXPos + X_ICON_BACK_OFFSET, iYPos + Y_ICON_BACK_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

	//상단 텍스트
	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;

		m_szDescArray[i].PrintFullText( iXPos+X_TOP_TEXT_OFFSET, iYPos + ( Y_LINE_GAP * i ) + Y_TOP_TEXT_OFFSET, TAT_LEFT );
	}

	//연승 정보 출력
	//연승 진행 텍스트
	const int& rkMaxSuccessionCnt = g_MyInfo.GetMaxSuccessionCount();
	const int& rkCurSuccessionCnt = g_MyInfo.GetCurSuccessionCount();
	//int rkMaxSuccessionCnt = m_iTest;
	//int rkCurSuccessionCnt = m_iTest2;//
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if ( rkCurSuccessionCnt == 0 )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 195, iYPos + 169, FONT_SIZE_15, "연승을 세워보세요" );
	}
	else if ( rkCurSuccessionCnt == rkMaxSuccessionCnt )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 195, iYPos + 169, FONT_SIZE_15, "최대연승 달성중" );
	}
	else if ( rkCurSuccessionCnt > 0 )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		g_FontMgr.PrintText( iXPos + 195, iYPos + 169, FONT_SIZE_15, "연승진행중" );
	}

	//최대 연승
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 47, iYPos + 213, FONT_SIZE_15, "최대연승" );

	if ( m_pMaxWinNum )
	{
		//999승 이상
		if ( rkMaxSuccessionCnt > 999 )
		{
			m_pMaxWinNum->RenderNum( iXPos + 103, iYPos + 198, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pMaxWinNum->RenderNum( iXPos + 131, iYPos + 198 , 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pMaxWinNum->RenderNum( iXPos + 159, iYPos + 198 , 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
		}
		//세자리
		else if ( rkMaxSuccessionCnt >= 100 )
		{
			int iH = rkMaxSuccessionCnt/100;
			int iT = (rkMaxSuccessionCnt-iH*100)/10;
			int iO = rkMaxSuccessionCnt%10;
			m_pMaxWinNum->RenderNum( iXPos + 103, iYPos + 198, iH, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pMaxWinNum->RenderNum( iXPos + 131, iYPos + 198 , iT, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pMaxWinNum->RenderNum( iXPos + 159, iYPos + 198 , iO, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
		}
		//두자리
		else if ( rkMaxSuccessionCnt >= 10 )
		{
			int iT = rkMaxSuccessionCnt/10;
			int iO = rkMaxSuccessionCnt%10;
			m_pMaxWinNum->RenderNum( iXPos + 118, iYPos + 198, iT, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pMaxWinNum->RenderNum( iXPos + 146, iYPos + 198 , iO, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
		}
		else
			m_pMaxWinNum->RenderNum( iXPos + 132, iYPos + 198, rkMaxSuccessionCnt, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
	}

	//현재 연승
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 209, iYPos + 213, FONT_SIZE_15, "현재" );

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 295, iYPos + 213, FONT_SIZE_15, "연승중" );

	if ( m_pCurWinNum )
	{
		//999승 이상
		if ( rkCurSuccessionCnt > 999 )
		{
			m_pCurWinNum->RenderNum( iXPos + 103, iYPos + 198, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pCurWinNum->RenderNum( iXPos + 131, iYPos + 198, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pCurWinNum->RenderNum( iXPos + 159, iYPos + 198, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
		}
		//세자리
		else if ( rkCurSuccessionCnt >= 100 )
		{
			int iH = rkCurSuccessionCnt/100;
			int iT = (iH*100-rkCurSuccessionCnt)/10;
			int iO = rkCurSuccessionCnt%10;
			m_pCurWinNum->RenderNum( iXPos + 103, iYPos + 198, iH, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pCurWinNum->RenderNum( iXPos + 131, iYPos + 198, iT, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pCurWinNum->RenderNum( iXPos + 159, iYPos + 198, iO, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
		}
		//두자리
		else if ( rkCurSuccessionCnt >= 10 )
		{
			int iT = rkCurSuccessionCnt/10;
			int iO = rkCurSuccessionCnt%10;
			m_pCurWinNum->RenderNum( iXPos + 245, iYPos + 204, iT, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pCurWinNum->RenderNum( iXPos + 265, iYPos + 204, iO, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
		}
		else
			m_pCurWinNum->RenderNum( iXPos + 255, iYPos + 204, rkCurSuccessionCnt, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
	}

	//하단 텍스트
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + X_BOTTOM_TEXT_OFFSET, iYPos + Y_BOTTOM_TEXT_OFFSET, FONT_SIZE_12, "%s", m_szMatchingEventMent );
}

void SuccessionBattleWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();
	if ( m_bWaitShow )
	{
		WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
		if ( pEffect && pEffect->IsShow() )
			pEffect->RenderWaitAni();
	}
}

void SuccessionBattleWnd::MatchingSuccess()
{
	m_iMatchState = STATE_NONE;
	HideWnd();
}

void SuccessionBattleWnd::MatchingCancel()
{
	SetChildInActive( ID_CANCEL );
	m_dwMatchingCancelCheckTime = FRAMEGETTIME();
	
	if ( m_bWaitShow )
	{
		WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
		if ( pEffect )
			pEffect->HideWnd();
	}
	m_bWaitShow = false;
	
	RemoveModal();
}

void SuccessionBattleWnd::MatchingFail()
{
	m_iMatchState = STATE_NONE;

	ShowChildWnd( ID_START );
	HideChildWnd( ID_CANCEL );
	SetChildActive( ID_CANCEL );
	ioButton *pBtn = static_cast<ioButton*>( FindChildWnd(ID_CANCEL) );
	if( pBtn )
	{
		pBtn->SetTitleText( "매칭취소" );
	}

	m_dwMatchingCancelCheckTime = 0;

	if ( m_bWaitShow )
	{
		WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
		if ( pEffect )
			pEffect->HideWnd();
	}
	m_bWaitShow = false;

	RemoveModal();

	if( g_App.IsMouseBusy() )
		TCPNetwork::MouseBusy( false );
}

void SuccessionBattleWnd::AddModal()
{
	AddWndStyle( IWS_MODAL );
	HeadInsertToModalList();
}

void SuccessionBattleWnd::RemoveModal()
{
	if ( m_pTopModalWnd == this )
	{
		EraseInModalList();
		RemoveWndStyle( IWS_MODAL );
	}
}

//--------------------------------------------------------------------------------------------------
SuccessionBattleRequestWnd::SuccessionBattleRequestWnd( void )
{
	m_pIconBack = NULL;
	m_pIcon = NULL;

	m_fCurWaitTime = 0;
	m_bWaitShow = false;
}

SuccessionBattleRequestWnd::~SuccessionBattleRequestWnd( void )
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void SuccessionBattleRequestWnd::iwm_create()
{
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_14, "연승에 실패 하였습니다!" );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_14, "같은 상대에게 대결을 신청하시겠습니까?" );

	m_szDescArray[2].ClearList();
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_14, "(복수는 1회만 가능합니다)" );
}

void SuccessionBattleRequestWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconName" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void SuccessionBattleRequestWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo(xElement);
	m_iFullWaitTime = xElement.GetIntAttribute_e( "WaitTime" ); 
	m_dwReplyWaitTime = xElement.GetIntAttribute_e( "ReplyWaitTime" ); 
	m_szReplyWaitMent = xElement.GetStringAttribute_e( "ReplyWaitMent" );
}

void SuccessionBattleRequestWnd::iwm_show()
{
	m_fCurWaitTime = (float)m_iFullWaitTime;
	ShowChildWnd( ID_REQUEST );
	HideChildWnd( ID_WAIT );
	m_bWaitShow = false;
}

void SuccessionBattleRequestWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_REQUEST:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				SP2Packet kPacket( CTPK_SUCCESSION_REQUEST_REVENGE );
				kPacket << g_MyInfo.GetUserIndex() << REVENGE_REQUEST;
				TCPNetwork::SendToServer( kPacket );
			}
			HideChildWnd( ID_REQUEST );
			ShowChildWnd( ID_WAIT );
			
			WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
			if ( pEffect )
			{
				m_bWaitShow = true;
				pEffect->StartSearch( this, false, m_dwReplyWaitTime, m_szReplyWaitMent );
			}
		}
		break;
	case ID_ROBBY:
		if( cmd == IOBN_BTNUP )
			HideWnd();
	}
}

bool SuccessionBattleRequestWnd::iwm_spacebar()
{
	ioButton *pRequestBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_REQUEST ) );
	if ( pRequestBtn && pRequestBtn->IsShow() )
	{
		iwm_command( pRequestBtn, IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

void SuccessionBattleRequestWnd::OnProcess( float fTimePerSec )
{
	if ( !IsShow() )
		return;

	if ( m_bWaitShow )
	{
		WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
		if ( !pEffect || !pEffect->IsShow() )
		{
			m_bWaitShow = false;
			HideWnd();
			return;
		}
	}

	ioWnd* pRequest = FindChildWnd( ID_REQUEST );
	if ( pRequest && pRequest->IsShow() )
	{
		m_fCurWaitTime -= fTimePerSec;
		if ( m_fCurWaitTime <= 0.0f )
		{
			HideWnd();
			return;
		}
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), "재대결 신청하기(SPACE)...%d", (int)m_fCurWaitTime + 1 );
		pRequest->SetTitleText( szTitle );
		
	}
}

void SuccessionBattleRequestWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos+206, iYPos+97, UI_RENDER_MULTIPLY );
	if( m_pIcon )
		m_pIcon->Render( iXPos+206, iYPos+97, UI_RENDER_NORMAL );

	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;

		m_szDescArray[i].PrintFullText( iXPos+218, iYPos + ( 20 * i ) + 144, TAT_CENTER );
	}
}

//--------------------------------------------------------------------------------------------------
SuccessionBattleReplyWnd::SuccessionBattleReplyWnd( void )
{
	m_pIconBack = NULL;
	m_pIcon = NULL;

	m_fCurWaitTime = 0;

}

SuccessionBattleReplyWnd::~SuccessionBattleReplyWnd( void )
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void SuccessionBattleReplyWnd::iwm_create()
{
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_14, "같은 상대에게 재대결 신청이 들어왔습니다!" );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_14, "수락하시겠습니까?" );

	m_szDescArray[2].ClearList();
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_14, "(복수는 1회만 가능하며,거절시에 패널티는 없습니다)" );
}

void SuccessionBattleReplyWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconName" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void SuccessionBattleReplyWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo(xElement);
	m_iFullWaitTime = xElement.GetIntAttribute_e( "WaitTime" ); 
}

void SuccessionBattleReplyWnd::iwm_show()
{
	m_fCurWaitTime = (float)m_iFullWaitTime;
}

void SuccessionBattleReplyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_REPLY:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				SP2Packet kPacket( CTPK_SUCCESSION_REQUEST_REVENGE );
				kPacket << g_MyInfo.GetUserIndex() << REVENGE_ACCEPT;
				TCPNetwork::SendToServer( kPacket );
			}
			HideWnd();
		}
		break;
	case ID_REJECT:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				SP2Packet kPacket( CTPK_SUCCESSION_REQUEST_REVENGE );
				kPacket << g_MyInfo.GetUserIndex() << REVENGE_REJECT;
				TCPNetwork::SendToServer( kPacket );
			}
			HideWnd();
		}
		break;
	}
}

bool SuccessionBattleReplyWnd::iwm_spacebar()
{
	ioButton *pRePlyBtn= dynamic_cast<ioButton*>( FindChildWnd( ID_REPLY ) );
	if ( pRePlyBtn && pRePlyBtn->IsShow() )
	{
		iwm_command( pRePlyBtn, IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

void SuccessionBattleReplyWnd::OnProcess( float fTimePerSec )
{
	if ( !IsShow() )
		return;

	m_fCurWaitTime -= fTimePerSec;
	if ( m_fCurWaitTime <= 0.0f )
	{
		if( P2PNetwork::IsNetworkPlaying() )
		{
			SP2Packet kPacket( CTPK_SUCCESSION_REQUEST_REVENGE );
			kPacket << g_MyInfo.GetUserIndex() << REVENGE_REJECT;
			TCPNetwork::SendToServer( kPacket );
		}
		HideWnd();
		return;
	}

	ioWnd* pRequest = FindChildWnd( ID_REPLY );
	if ( pRequest )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), "수락하기(SPACE)...%d", (int)m_fCurWaitTime + 1);
		pRequest->SetTitleText( szTitle );
	}
}

void SuccessionBattleReplyWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos+206, iYPos+97, UI_RENDER_MULTIPLY );
	if( m_pIcon )
		m_pIcon->Render( iXPos+206, iYPos+97, UI_RENDER_NORMAL );
	

	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;

		m_szDescArray[i].PrintFullText( iXPos+218, iYPos + ( 20 * i ) + 144, TAT_CENTER );
	}
}

//--------------------------------------------------------------------------------------------------
SuccessionMatchVersusSceneWnd::HeroData SuccessionMatchVersusSceneWnd::m_BlueData;
SuccessionMatchVersusSceneWnd::HeroData SuccessionMatchVersusSceneWnd::m_RedData;
SuccessionMatchVersusSceneWnd::SuccessionMatchVersusSceneWnd()
{
	m_pBlueFrame	= NULL;
	m_pRedFrame		= NULL;
	m_pBackFrame    = NULL;

	m_pBlueGra		= NULL;
	m_pRedGra		= NULL;
	m_pDarkBar		= NULL;
	m_pLightBar		= NULL;
	m_pBlackStrip	= NULL;
	m_pWhiteStrip	= NULL;
	m_pSuccessionBattleMark = NULL;
	m_pCharShadow	= NULL;
	for(int i = 0;i < MAX_EFFECT_SPR;i++)
		m_pEffectSprite[i] = NULL;

	m_pBlue3DRender = NULL;
	m_pRed3DRender  = NULL;

	m_bFadeOutStart = false;
	m_pPlayMode		= NULL;
	m_iSelectChar	= 0;
	m_dwSelectTime  = 0;

	m_pBlueChar		= NULL;
	m_pRedChar		= NULL;

	m_dwStartTime = 0;
}

SuccessionMatchVersusSceneWnd::~SuccessionMatchVersusSceneWnd()
{
	SAFEDELETE( m_pBlueFrame );
	SAFEDELETE( m_pRedFrame );
	SAFEDELETE( m_pBackFrame );

	SAFEDELETE( m_pBlueGra );
	SAFEDELETE( m_pRedGra );
	SAFEDELETE( m_pDarkBar );
	SAFEDELETE( m_pLightBar );
	SAFEDELETE( m_pBlackStrip );
	SAFEDELETE( m_pWhiteStrip );
	SAFEDELETE( m_pSuccessionBattleMark );
	SAFEDELETE( m_pCharShadow );
	for(int i = 0;i < MAX_EFFECT_SPR;i++)
		SAFEDELETE( m_pEffectSprite[i] );

	SAFEDELETE( m_pBlueChar );
	SAFEDELETE( m_pRedChar );

	SAFEDELETE( m_pBlue3DRender );
	SAFEDELETE( m_pRed3DRender );
	
	
}

void SuccessionMatchVersusSceneWnd::iwm_create()
{
	SAFEDELETE( m_pBlue3DRender );
	m_pBlue3DRender = new ioUI3DRender;
	m_pBlue3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
	SAFEDELETE( m_pRed3DRender );
	m_pRed3DRender = new ioUI3DRender;
	m_pRed3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void SuccessionMatchVersusSceneWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueBar" )
	{
		SAFEDELETE( m_pBlueFrame );
		m_pBlueFrame = pFrame;
	}
	else if( szType == "RedBar" )
	{
		SAFEDELETE( m_pRedFrame );
		m_pRedFrame = pFrame;
	}
	else if( szType == "BackFrame" )
	{
		SAFEDELETE( m_pBackFrame );
		m_pBackFrame = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void SuccessionMatchVersusSceneWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueGra" )
	{
		SAFEDELETE( m_pBlueGra );
		m_pBlueGra = pImage;
	}
	else if( szType == "RedGra" )
	{
		SAFEDELETE( m_pRedGra );
		m_pRedGra = pImage;
	}
	else if( szType == "DarkBar" )
	{
		SAFEDELETE( m_pDarkBar );
		m_pDarkBar = pImage;
	}
	else if( szType == "LightBar" )
	{
		SAFEDELETE( m_pLightBar );
		m_pLightBar = pImage;
	}
	else if( szType == "BlackStrip" )
	{
		SAFEDELETE( m_pBlackStrip );
		m_pBlackStrip = pImage;
	}
	else if( szType == "WhiteStrip" )
	{
		SAFEDELETE( m_pWhiteStrip );
		m_pWhiteStrip = pImage;
	}
	else if( szType == "SuccessionBattleMark" )
	{
		SAFEDELETE( m_pSuccessionBattleMark );
		m_pSuccessionBattleMark = pImage;
	}
	else if( szType == "CharShadow" )
	{
		SAFEDELETE( m_pCharShadow );
		m_pCharShadow = pImage;
	}
	else if( szType == "EffectSprite0" )
	{
		SAFEDELETE( m_pEffectSprite[0] );
		m_pEffectSprite[0] = pImage;
	}
	else if( szType == "EffectSprite1" )
	{
		SAFEDELETE( m_pEffectSprite[1] );
		m_pEffectSprite[1] = pImage;
	}
	else if( szType == "EffectSprite2" )
	{
		SAFEDELETE( m_pEffectSprite[2] );
		m_pEffectSprite[2] = pImage;
	}
	else if( szType == "EffectSprite3" )
	{
		SAFEDELETE( m_pEffectSprite[3] );
		m_pEffectSprite[3] = pImage;
	}
	else if( szType == "EffectSprite4" )
	{
		SAFEDELETE( m_pEffectSprite[4] );
		m_pEffectSprite[4] = pImage;
	}
	else if( szType == "EffectSprite5" )
	{
		SAFEDELETE( m_pEffectSprite[5] );
		m_pEffectSprite[5] = pImage;
	}
	else if( szType == "EffectSprite6" )
	{
		SAFEDELETE( m_pEffectSprite[6] );
		m_pEffectSprite[6] = pImage;
	}
	else if( szType == "EffectSprite7" )
	{
		SAFEDELETE( m_pEffectSprite[7] );
		m_pEffectSprite[7] = pImage;
	}
	else if( szType == "EffectSprite8" )
	{
		SAFEDELETE( m_pEffectSprite[8] );
		m_pEffectSprite[8] = pImage;
	}
	else if( szType == "EffectSprite9" )
	{
		SAFEDELETE( m_pEffectSprite[9] );
		m_pEffectSprite[9] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SuccessionMatchVersusSceneWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			SetSize( Setting::Width(), Setting::Height() - 42 );
		}
	}
}

void SuccessionMatchVersusSceneWnd::StartFadeOut( ioPlayMode *pPlayMode, int now_select_char, DWORD dwSelectTime )
{
	m_bFadeOutStart = true;

	m_pPlayMode		= pPlayMode;
	m_iSelectChar	= now_select_char;
	m_dwSelectTime  = dwSelectTime;
}

void SuccessionMatchVersusSceneWnd::OtherHeroInfoUpdate( int iOtherTeamType, SP2Packet &rkPacket )
{
	if ( iOtherTeamType == TEAM_BLUE )
	{
		m_BlueData.Init();
		rkPacket >> m_BlueData.m_szUserID >> m_BlueData.m_iMMRPoint >> m_BlueData.m_iCurSuccessionWin >> m_BlueData.m_iMaxSuccessionWin;
		m_BlueData.m_CharInfo.ApplyData( rkPacket );

		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			rkPacket >> m_BlueData.m_EquipItem[i].m_item_code;
			rkPacket >> m_BlueData.m_EquipItem[i].m_item_reinforce;
			rkPacket >> m_BlueData.m_EquipItem[i].m_item_male_custom;
			rkPacket >> m_BlueData.m_EquipItem[i].m_item_female_custom;
		}
	}
	else if ( iOtherTeamType == TEAM_RED )
	{
		m_RedData.Init();
		rkPacket >> m_RedData.m_szUserID >> m_RedData.m_iMMRPoint >> m_RedData.m_iCurSuccessionWin >> m_RedData.m_iMaxSuccessionWin;
		m_RedData.m_CharInfo.ApplyData( rkPacket );
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			rkPacket >> m_RedData.m_EquipItem[i].m_item_code;
			rkPacket >> m_RedData.m_EquipItem[i].m_item_reinforce;
			rkPacket >> m_RedData.m_EquipItem[i].m_item_male_custom;
			rkPacket >> m_RedData.m_EquipItem[i].m_item_female_custom;
		}
	}
}

void SuccessionMatchVersusSceneWnd::MyHeroInfoUpdate( int iOtherTeamType )
{
	//적팀이 Red라면
	if ( iOtherTeamType == TEAM_RED )
	{
		m_BlueData.Init();
		
		m_BlueData.m_szUserID = g_MyInfo.GetPublicID();
		m_BlueData.m_iMaxSuccessionWin = g_MyInfo.GetMaxSuccessionCount();
		m_BlueData.m_iCurSuccessionWin = g_MyInfo.GetCurSuccessionCount();
		DWORD dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
		m_BlueData.m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( g_MyInfo.GetPublicID(), dwCharIndex );
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			m_BlueData.m_EquipItem[i].m_item_code = g_GlobalSoldierInfo.GetSoldierItemCode( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_BlueData.m_EquipItem[i].m_item_male_custom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_BlueData.m_EquipItem[i].m_item_female_custom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
		}
	}
	else if ( iOtherTeamType == TEAM_BLUE )
	{
		m_RedData.Init();
		m_RedData.m_szUserID = g_MyInfo.GetPublicID();
		m_RedData.m_iMaxSuccessionWin = g_MyInfo.GetMaxSuccessionCount();
		m_RedData.m_iCurSuccessionWin = g_MyInfo.GetCurSuccessionCount();

		DWORD dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
		m_RedData.m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( g_MyInfo.GetPublicID(), dwCharIndex );
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			m_RedData.m_EquipItem[i].m_item_code = g_GlobalSoldierInfo.GetSoldierItemCode( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_RedData.m_EquipItem[i].m_item_male_custom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_RedData.m_EquipItem[i].m_item_female_custom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
		}
	}

	if( m_BlueData.m_iCurSuccessionWin < m_RedData.m_iCurSuccessionWin )
	{
		m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 133;
		m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 113;
	}
	else if( m_BlueData.m_iCurSuccessionWin > m_RedData.m_iCurSuccessionWin )
	{
		m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 113;
		m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 133;
	}
	else
	{
		m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 123;
		m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 123;
	}
}

void SuccessionMatchVersusSceneWnd::iwm_show()
{
	//데이터 초기화
	m_pPlayMode		= NULL;
	m_iSelectChar	= 0;
	if ( m_pSuccessionBattleMark )
		m_pSuccessionBattleMark->SetScale( 1.0f );
	m_bFadeOutStart = false;
	m_VersusAni.Init();
	m_dwSelectTime  = 0;
	m_dwStartTime = FRAMEGETTIME();

	SetSize( Setting::Width(), Setting::Height() - 42 );

	//캐릭터 설정
	SAFEDELETE( m_pBlueChar );
	m_pBlueChar = GetCreateCharacter( m_pBlue3DRender, m_BlueData );
	SAFEDELETE( m_pRedChar );
	m_pRedChar = GetCreateCharacter( m_pRed3DRender, m_RedData );
	
	// 좌표 및 애니 설정
	m_BlueData.m_dwAniState = CARD_ANI_START;
	m_BlueData.m_dwMoveTime = 0;  // 시간을 먼저 넣지않은 이유는 Show된 시점이 로딩중일 수 있다
	m_BlueData.m_iStartXPos = m_BlueData.m_iCurrentXPos = -147;	
	m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 123;

	m_RedData.m_dwAniState = CARD_ANI_START;
	m_RedData.m_dwMoveTime = 0;
	m_RedData.m_iStartXPos = m_RedData.m_iCurrentXPos = Setting::Width() + 147;
	m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 123;
}

ioCreateChar *SuccessionMatchVersusSceneWnd::GetCreateCharacter( ioUI3DRender *p3DRender, HeroData &rkData )
{
	if( !p3DRender ) return NULL;
	if( rkData.m_CharInfo.m_class_type == 0 ) return NULL;

	ioCreateChar *pChar = p3DRender->CreateChar( rkData.m_CharInfo, "ResultChar" );
	if( !pChar ) return NULL;

	pChar->SetPosition( 0.0f, -FLOAT90, 900.0f );
	pChar->SetYaw( FLOAT10 );
	p3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	p3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	// 코스튬 관련 (??)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = rkData.m_CharInfo.m_EquipCostume[i].m_CostumeCode;

		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && rkData.m_szUserID != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( rkData.m_CharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{
			DWORD dwItemCode = rkData.m_EquipItem[i].m_item_code;
			DWORD dwItemMaleCustom = rkData.m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwItemFemaleCustom = rkData.m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( TempCostume[i] <= 0 )
				{
					dwItemMaleCustom = 0;
					dwItemFemaleCustom = 0;
				}

				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom, TempCostume[i] );
			}
			else
			{
				if ( TempCostume[i] <= 0 )
				{
					dwItemMaleCustom = rkData.m_EquipItem[i].m_item_male_custom;
					dwItemFemaleCustom = rkData.m_EquipItem[i].m_item_female_custom;
				}
				pChar->EquipItem( rkData.m_EquipItem[i].m_item_code, dwItemMaleCustom, dwItemFemaleCustom, TempCostume[i] );
			}
		}
	}

	if( !pChar->SetWeaponShopMotion() )
	{
		ioHashString szUIAnimation = p3DRender->GetModelAnimation( 0, pChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			pChar->SetLoopAni( szUIAnimation );
		}
	}
	return pChar;
}

void SuccessionMatchVersusSceneWnd::iwm_hide()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->StartChangeCharacter( m_iSelectChar, m_dwSelectTime );
	}
}

void SuccessionMatchVersusSceneWnd::OnProcess( float fTimePerSec )
{
	if ( !IsShow() )
		return;

	OnProcessChar();
	OnProcessVS();

	if( m_pRedChar )
		m_pRedChar->Update( fTimePerSec );
	if( m_pBlueChar )
		m_pBlueChar->Update( fTimePerSec );
}

void SuccessionMatchVersusSceneWnd::OnProcessChar()
{
	// 블루 - 가운데 vs 마크와 배경은 블루 기준으로 작동한다.
	switch( m_BlueData.m_dwAniState )
	{
	case CARD_ANI_START:
		{
			if( m_BlueData.m_dwMoveTime == 0 )
				m_BlueData.m_dwMoveTime = FRAMEGETTIME();

			DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
			if( dwGapTime <= START_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / START_MOVE_TIME ) );
				float fMoveGap = Setting::HalfWidth();
				m_BlueData.m_iCurrentXPos = m_BlueData.m_iStartXPos + ( fMoveGap * fRate );
			}
			else
			{
				m_BlueData.m_iStartXPos = m_BlueData.m_iCurrentXPos = ( Setting::HalfWidth() - 147.0f );
				m_BlueData.m_fAniRate   = FLOAT1;
				m_BlueData.m_dwAniState = CARD_ANI_BACK;
				m_BlueData.m_dwMoveTime = FRAMEGETTIME();

				// VS Ani Start
				m_VersusAni.m_dwAniState = VS_ANI_START;
				m_VersusAni.m_dwAniTime  = FRAMEGETTIME();
				m_VersusAni.m_fAniRate   = FLOAT1;
				m_VersusAni.m_dwEffectTime =  FRAMEGETTIME();
				m_VersusAni.m_iEffectArray = 0;
			}
		}
		break;
	case CARD_ANI_BACK:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
			if( dwGapTime <= BACK_MOVE_TIME )
			{
				float fRate = min( FLOAT1, sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / BACK_MOVE_TIME ) );
				float fMoveGap = 17.0f;
				m_BlueData.m_iCurrentXPos = m_BlueData.m_iStartXPos - ( fMoveGap * fRate );
				m_BlueData.m_fAniRate   = min( FLOAT1, FLOAT1 - (float) dwGapTime / BACK_MOVE_TIME );
			}
			else
			{
				m_BlueData.m_iStartXPos = m_BlueData.m_iCurrentXPos = ( Setting::HalfWidth() - 164.0f );
				m_BlueData.m_fAniRate   = 0.0f;
				m_BlueData.m_dwAniState = CARD_ANI_DELAY;
				m_BlueData.m_dwMoveTime = FRAMEGETTIME();
			}
		}
		break;
	case CARD_ANI_DELAY:
		{
			if( m_bFadeOutStart )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
				if( dwGapTime > DELAY_TIME )
				{
					m_BlueData.m_dwAniState = CARD_ANI_FADEOUT;
					m_BlueData.m_dwMoveTime = FRAMEGETTIME();

					// VS Ani FADEOUT
					m_VersusAni.m_dwAniState = VS_ANI_FADEOUT;
					m_VersusAni.m_dwAniTime  = FRAMEGETTIME();
					m_VersusAni.m_fAniRate   = 0.0f;
				}
			}
		}
		break;
	case CARD_ANI_FADEOUT:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
			if( dwGapTime <= FADEOUT_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / FADEOUT_MOVE_TIME ) );
				float fMoveGap = Setting::HalfWidth();
				m_BlueData.m_iCurrentXPos = m_BlueData.m_iStartXPos - ( fMoveGap * fRate );

				//없어질 때 백그라운드 알파
				m_BlueData.m_fAniRate   = min( FLOAT1, FLOAT1 - (float) dwGapTime / FADEOUT_MOVE_TIME );
			}
			else
			{				
				HideWnd();
			}
		}
		break;
	}

	// 레드
	switch( m_RedData.m_dwAniState )
	{
	case CARD_ANI_START:
		{
			if( m_RedData.m_dwMoveTime == 0 )
				m_RedData.m_dwMoveTime = FRAMEGETTIME();

			DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
			if( dwGapTime <= START_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / START_MOVE_TIME ) );
				float fMoveGap = 147.0f + Setting::HalfWidth();
				m_RedData.m_iCurrentXPos = m_RedData.m_iStartXPos - ( fMoveGap * fRate );
			}
			else
			{
				m_RedData.m_iStartXPos = m_RedData.m_iCurrentXPos = Setting::HalfWidth();
				m_RedData.m_fAniRate   = FLOAT1;
				m_RedData.m_dwAniState = CARD_ANI_BACK;
				m_RedData.m_dwMoveTime = FRAMEGETTIME();
			}
		}
		break;
	case CARD_ANI_BACK:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
			if( dwGapTime <= BACK_MOVE_TIME )
			{
				float fRate = min( FLOAT1, sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / BACK_MOVE_TIME ) );
				float fMoveGap = 17.0f;
				m_RedData.m_iCurrentXPos = m_RedData.m_iStartXPos + ( fMoveGap * fRate );
				m_RedData.m_fAniRate   = min( FLOAT1, FLOAT1 - (float) dwGapTime / BACK_MOVE_TIME );
			}
			else
			{
				m_RedData.m_iStartXPos = m_RedData.m_iCurrentXPos = ( Setting::HalfWidth() + 17.0f );
				m_RedData.m_fAniRate   = 0.0f;
				m_RedData.m_dwAniState = CARD_ANI_DELAY;
				m_RedData.m_dwMoveTime = FRAMEGETTIME();
			}
		}
		break;
	case CARD_ANI_DELAY:
		{
			if( m_bFadeOutStart )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
				if( dwGapTime > DELAY_TIME )
				{
					m_RedData.m_dwAniState = CARD_ANI_FADEOUT;
					m_RedData.m_dwMoveTime = FRAMEGETTIME();
				}
			}
		}
		break;
	case CARD_ANI_FADEOUT:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
			if( dwGapTime <= FADEOUT_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / FADEOUT_MOVE_TIME ) );
				float fMoveGap = Setting::HalfWidth();
				m_RedData.m_iCurrentXPos = m_RedData.m_iStartXPos + ( fMoveGap * fRate );
			}
		}
		break;
	}

	if( !m_pBlueChar && !m_BlueData.m_szUserID.IsEmpty() )
	{
		m_pBlueChar = GetCreateCharacter( m_pBlue3DRender, m_BlueData );
	}

	if( !m_pRedChar && !m_RedData.m_szUserID.IsEmpty() )
	{
		m_pRedChar = GetCreateCharacter( m_pRed3DRender, m_RedData );
	}
}

void SuccessionMatchVersusSceneWnd::OnProcessVS()
{
	switch( m_VersusAni.m_dwAniState )
	{
	case VS_ANI_START:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_VersusAni.m_dwAniTime;
			if( dwGapTime <= VS_START_TIME )
			{
				m_VersusAni.m_fAniRate = min( FLOAT1, FLOAT1 - (float)dwGapTime / VS_START_TIME );

				if( FRAMEGETTIME() - m_VersusAni.m_dwEffectTime > 50 )
				{
					m_VersusAni.m_dwEffectTime =  FRAMEGETTIME();
					m_VersusAni.m_iEffectArray++;
				}
			}
			else
			{
				m_VersusAni.m_fAniRate = 0.0f;
				m_VersusAni.m_dwEffectTime = 0;
				m_VersusAni.m_iEffectArray = MAX_EFFECT_SPR;
			}
		}
		break;
	case VS_ANI_FADEOUT:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_VersusAni.m_dwAniTime;
			if( dwGapTime <= VS_FADEOUT_TIME )
			{
				m_VersusAni.m_fAniRate = (float)dwGapTime / VS_FADEOUT_TIME;
			}
		}
		break;
	}

}

void SuccessionMatchVersusSceneWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	{   // 백 그라운드 
		OnRenderBack( iXPos, iYPos );
	}

	{   // 캐릭터 카드
		OnRenderBlueChar( iXPos + m_BlueData.m_iCurrentXPos, iYPos + m_BlueData.m_iCurrentYPos );
		OnRenderRedChar( iXPos + m_RedData.m_iCurrentXPos, iYPos + m_RedData.m_iCurrentYPos );
	}
	
	{   // VERSUS
		OnRenderVersus( iXPos + Setting::HalfWidth(), iYPos + Setting::HalfHeight() - 20 );
	}
}

void SuccessionMatchVersusSceneWnd::OnRenderBack( int iXPos, int iYPos )
{
	if( m_pBlackStrip )
	{
		m_pBlackStrip->SetSize( Setting::Width(), Setting::Height() );
		m_pBlackStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBlackStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

		if( m_BlueData.m_dwAniState == CARD_ANI_FADEOUT )
		{
			m_pBlackStrip->SetAlpha( (float)MAX_ALPHA_RATE * m_BlueData.m_fAniRate );
		}
		m_pBlackStrip->Render( iXPos, iYPos );
	}
}

void SuccessionMatchVersusSceneWnd::OnRenderBlueChar( int iXPos, int iYPos )
{
	if( m_pBackFrame )
	{
		m_pBackFrame->SetAlpha( MAX_ALPHA_RATE );
		m_pBackFrame->Render( iXPos, iYPos );
	}

	if( m_pBlueFrame )
		m_pBlueFrame->Render( iXPos, iYPos );

	// 
	if( m_pBlue3DRender && m_pBlueChar )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 3;
		rcD3D.y1 = iYPos + 3;
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pBlue3DRender->RenderViewPort( &rcD3D, true );
	}
	else if( m_pCharShadow )
	{
		m_pCharShadow->Render( iXPos, iYPos );
	}

    //
	if( m_pBlueGra )
		m_pBlueGra->Render( iXPos, iYPos );

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	//
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( TCT_DEFAULT_BLUE );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 182, FONT_SIZE_13, "%s", m_BlueData.m_szUserID.c_str() );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 203, FONT_SIZE_13, "최대연승 %d", m_BlueData.m_iMaxSuccessionWin );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 224, FONT_SIZE_13, "(현재연승 %d)", m_BlueData.m_iCurSuccessionWin );
	
	if( m_BlueData.m_dwAniState == CARD_ANI_BACK && m_pBackFrame && m_BlueData.m_fAniRate > 0.0f )
	{
		m_pBackFrame->SetColor( TCT_DEFAULT_WHITE );
		m_pBackFrame->SetAlpha( (float)MAX_ALPHA_RATE * m_BlueData.m_fAniRate );
		m_pBackFrame->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
	}
}

void SuccessionMatchVersusSceneWnd::OnRenderRedChar( int iXPos, int iYPos )
{
	if( m_pBackFrame )
	{
		m_pBackFrame->SetAlpha( MAX_ALPHA_RATE );
		m_pBackFrame->Render( iXPos, iYPos );
	}

	if( m_pRedFrame )
		m_pRedFrame->Render( iXPos, iYPos );

	// 
	if( m_pRed3DRender && m_pRedChar )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 3;
		rcD3D.y1 = iYPos + 3;
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pRed3DRender->RenderViewPort( &rcD3D, true );
	}
	else if( m_pCharShadow )
	{
		m_pCharShadow->Render( iXPos, iYPos );
	}

	//
	if( m_pRedGra )
		m_pRedGra->Render( iXPos, iYPos );

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	//
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( TCT_DEFAULT_RED );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 182, FONT_SIZE_13, "%s", m_RedData.m_szUserID.c_str() );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 203, FONT_SIZE_13, "최대연승 %d", m_RedData.m_iMaxSuccessionWin );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 224, FONT_SIZE_13, "(현재연승 %d)", m_RedData.m_iCurSuccessionWin );

	if( m_RedData.m_dwAniState == CARD_ANI_BACK && m_pBackFrame && m_RedData.m_fAniRate > 0.0f )
	{
		m_pBackFrame->SetColor( TCT_DEFAULT_WHITE );
		m_pBackFrame->SetAlpha( (float)MAX_ALPHA_RATE * m_RedData.m_fAniRate );
		m_pBackFrame->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
	}
}

void SuccessionMatchVersusSceneWnd::OnRenderVersus( int iXPos, int iYPos )
{
	if( m_VersusAni.m_dwAniState == VS_ANI_NONE ) 
		return;

	if( m_pSuccessionBattleMark )
	{
		if( m_VersusAni.m_dwAniState == VS_ANI_FADEOUT )
		{
			m_pSuccessionBattleMark->SetScale( 4.0f * m_VersusAni.m_fAniRate );
			m_pSuccessionBattleMark->SetAlpha( (float)MAX_ALPHA_RATE * ( FLOAT1 - m_VersusAni.m_fAniRate ) );
			m_pSuccessionBattleMark->Render( iXPos, iYPos );
		}
		else
		{
			m_pSuccessionBattleMark->SetAlpha( MAX_ALPHA_RATE );
			m_pSuccessionBattleMark->Render( iXPos, iYPos );

			if( m_VersusAni.m_dwAniState == VS_ANI_START && m_VersusAni.m_fAniRate > 0.0f )
			{
				m_pSuccessionBattleMark->SetColor( TCT_DEFAULT_WHITE );
				m_pSuccessionBattleMark->SetAlpha( (float)MAX_ALPHA_RATE * m_VersusAni.m_fAniRate );
				m_pSuccessionBattleMark->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
			}
		}
	}

	// 이펙트
	if( m_VersusAni.m_dwAniState == VS_ANI_START )
	{
		if( COMPARE( m_VersusAni.m_iEffectArray, 0, MAX_EFFECT_SPR ) )
		{
			if( m_pEffectSprite[m_VersusAni.m_iEffectArray] )
			{
				m_pEffectSprite[m_VersusAni.m_iEffectArray]->SetScale( 4.0f );
				m_pEffectSprite[m_VersusAni.m_iEffectArray]->Render( iXPos, iYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}
	}

	// 화면 전체 허여멀건
	if( m_VersusAni.m_dwAniState == VS_ANI_START )
	{
		if( m_pWhiteStrip && m_VersusAni.m_fAniRate > 0.0f )
		{
			m_pWhiteStrip->SetSize( Setting::Width(), Setting::Height() );
			m_pWhiteStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pWhiteStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pWhiteStrip->SetAlpha( (float)MAX_ALPHA_RATE * ( FLOAT05 * m_VersusAni.m_fAniRate ) );
			m_pWhiteStrip->Render( 0, 0 );
		}
	}	
}

//--------------------------------------------------------------------------------------------------
WaitEffectWnd::WaitEffectWnd( void )
{
	m_pWarCountCircle = NULL;
	m_pWarCountWait = NULL;
	m_pTextBack = NULL;

	m_dwCurSearchTime = 0;
	m_dwFullSearchTime= 0;
	m_iCurSearchSec   = 0;
	m_dwChangeCountTime = 0;

	m_bStart = false;
	m_pParent = NULL;
	m_bNullParent = false;
}

WaitEffectWnd::~WaitEffectWnd(void)
{
	SAFEDELETE( m_pWarCountCircle );
	SAFEDELETE( m_pWarCountWait );
	SAFEDELETE( m_pTextBack );

}

void WaitEffectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "count_circle" )
	{
		SAFEDELETE( m_pWarCountCircle );
		m_pWarCountCircle = pImage;
	}
	else if( szType == "count_wait" )
	{
		SAFEDELETE( m_pWarCountWait );
		m_pWarCountWait = pImage;
	}
	else if( szType == "text_back" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}


void WaitEffectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );
}

void WaitEffectWnd::StartSearch( ioWnd* pParent, bool bNullParent, DWORD dwFullTime, ioHashString szMent )
{
	if( dwFullTime == 0 )
		return;

	m_dwCurSearchTime = FRAMEGETTIME();
	m_dwFullSearchTime= dwFullTime;
	m_iCurSearchSec   = (int)dwFullTime / 1000;

	m_dwChangeCountTime = FRAMEGETTIME();
	m_szMent = szMent;
	m_pParent = pParent;
	m_bNullParent = bNullParent;
	ShowWnd();
}

void WaitEffectWnd::Clear()
{
	m_dwCurSearchTime = 0;
	m_dwFullSearchTime = 0;
	m_iCurSearchSec = 0;
	m_dwChangeCountTime = 0;
	m_pParent = NULL;
	m_bNullParent = false;
}

void WaitEffectWnd::iwm_hide()
{
	Clear();
}

bool WaitEffectWnd::CheckParentShow()
{
	if ( m_bNullParent )
		return true;

	if ( !m_pParent )
		return false;

	if ( !m_pParent->IsShow() )
		return false;

	return true;
}

void WaitEffectWnd::OnProcess( float fTimePerSec )
{
	if ( !IsShow() )
		return;

	if ( !CheckParentShow() )
	{
		HideWnd();
		return;
	}

	if ( m_iCurSearchSec == 0 )
	{
		HideWnd();
		return;
	}
	
	int iFullSec = m_dwFullSearchTime / 1000;
	int iCurSec  = (FRAMEGETTIME() - m_dwCurSearchTime) / 1000;
	int iNextSec = max( 0, iFullSec - iCurSec );
	if( iNextSec != m_iCurSearchSec )
	{
		m_iCurSearchSec = iNextSec;
		m_dwChangeCountTime = FRAMEGETTIME();
	}
}

void WaitEffectWnd::OnRender()
{
	if ( m_pParent && m_pParent->HasWndStyle( IWS_MODAL ) )
		return;

	RenderWaitAni();
}

void WaitEffectWnd::RenderWaitAni()
{
	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwWaitScaleTime = 150;
	DWORD dwCircleTime = 500;
	DWORD dwStartCircleTime1 = 150;
	DWORD dwStartCircleTime2 = 210;

	float fWaitScaleRate = 0.1f;
	float fStartCircleRate = FLOAT05;
	float fCircleRate = 1.5f;

	float fWaitCurScale = FLOAT1;
	float fCircleRate1 = FLOAT05;
	float fCircleRate2 = FLOAT05;
	float fCircleAlpha1 = FLOAT1;
	float fCircleAlpha2 = FLOAT1;

	bool bRenderCircle1 = false;
	bool bRenderCircle2 = false;

	DWORD dwGapTime = dwCurTime - m_dwChangeCountTime;
	if( dwGapTime < dwWaitScaleTime )
	{
		float fTimeRate = (float)dwGapTime / (float)dwWaitScaleTime;
		fWaitCurScale = FLOAT1 + fWaitScaleRate * sinf( D3DX_PI * fTimeRate );
	}
	else
	{
		bRenderCircle1 = true;
		fCircleRate1 = 2.0f;
		DWORD dwCircleGap1 = dwGapTime - dwStartCircleTime1;
		if( dwCircleGap1 < dwCircleTime )
		{
			float fTimeRate = (float)dwCircleGap1 / (float)dwCircleTime;
			fCircleRate1 = fStartCircleRate + fCircleRate * sinf( (D3DX_PI/2) * fTimeRate );
			fCircleAlpha1 = cosf( (D3DX_PI/2) * fTimeRate );
		}
		else
		{
			bRenderCircle1 = false;
		}

		if( dwGapTime > dwStartCircleTime2 )
		{
			bRenderCircle2 = true;

			fCircleRate2 = 2.0f;
			DWORD dwCircleGap2 = dwGapTime - dwStartCircleTime2;
			if( dwCircleGap2 < dwCircleTime )
			{
				float fTimeRate = (float)dwCircleGap2 / (float)dwCircleTime;
				fCircleRate2 = fStartCircleRate + fCircleRate * sinf( (D3DX_PI/2) * fTimeRate );
				fCircleAlpha2 = cosf( (D3DX_PI/2) * fTimeRate );
			}
			else
			{
				bRenderCircle2 = false;
			}
		}
	}

	enum
	{
		CIRCLE_X		= 386,
		CIRCLE_Y		= 266,

		TEXT_BACK_X		= 386,
		TEXT_BACK_Y		= 232,

		WAIT_X			= 386,
		WAIT_Y			= 266,

		BLUE_TEXT1_X	= 317,
		BLUE_TEXT1_Y	= 296,
		BLUE_TEXT2_X	= 317,
		BLUE_TEXT2_Y	= 318,

		RED_TEXT1_X		= 455,
		RED_TEXT1_Y		= 296,
		RED_TEXT2_X		= 455,
		RED_TEXT2_Y		= 318,

		MAIN_TEXT_X		= 336,
		MAIN_TEXT_Y		= 223,
	};


	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();


	int iScreenCenterPosX = Setting::Width() / 2;
	int iScreenCenterPosY = Setting::Height() / 2;

	// Circle
	if( m_pWarCountCircle )
	{
		if( bRenderCircle1 )
		{
			m_pWarCountCircle->SetAlpha((BYTE)(int)(255.0f * fCircleAlpha1));
			m_pWarCountCircle->SetScale( fCircleRate1 );
			m_pWarCountCircle->Render( iScreenCenterPosX, iScreenCenterPosY-2, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( bRenderCircle2 )
		{
			m_pWarCountCircle->SetAlpha((BYTE)(int)(255.0f * fCircleAlpha2));
			m_pWarCountCircle->SetScale( fCircleRate2 );
			m_pWarCountCircle->Render( iScreenCenterPosX, iScreenCenterPosY-2, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	// TextBack
	if( m_pTextBack )
	{
		m_pTextBack->SetSize( 162, 35 );
		m_pTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

		int iOffSetX = m_pTextBack->GetOffsetX();
		int iOffSetY = m_pTextBack->GetOffsetY();

		m_pTextBack->Render( iScreenCenterPosX-iOffSetX, iScreenCenterPosY-54+iOffSetY );
	}

	// Wait
	if( m_pWarCountWait )
	{
		m_pWarCountWait->SetScale( fWaitCurScale );
		m_pWarCountWait->Render( iScreenCenterPosX, iScreenCenterPosY-2, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Text
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iScreenCenterPosX, iScreenCenterPosY-48, FONT_SIZE_15, m_szMent.c_str(), m_iCurSearchSec );
}

const int& SuccessionMatchVersusSceneWnd::GetMMRPoint( int iTeamType )
{
	if ( iTeamType == TEAM_BLUE )
		return m_BlueData.m_iMMRPoint;
	else
		return m_RedData.m_iMMRPoint;
}