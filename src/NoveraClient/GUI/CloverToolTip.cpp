
#include "StdAfx.h"

#include "CloverToolTip.h"
#include "UserListWnd.h"
#include "MessengerWnd.h"

CloverToolTip::CloverToolTip()
{
	m_Type = TYPE_NONE;
	m_bFriendRegTime = false;
	m_bSendRemainTime = false;
	m_iSendCloverCount = 0;
	m_iRecvCloverCount = 0;
	m_iRecvWaitCloverCount = 0;
}

CloverToolTip::~CloverToolTip()
{

}

void CloverToolTip::iwm_show()
{
}

void CloverToolTip::iwm_hide()
{
	m_szUserName.Clear();
	m_bFriendRegTime = false;
	m_bSendRemainTime = false;
	m_iSendCloverCount = 0;
	m_iRecvCloverCount = 0;
	m_iRecvWaitCloverCount = 0;
}

void CloverToolTip::iwm_create()
{
	m_dwStyle |= IWS_PROCESS_ALWAYS;
}

void CloverToolTip::OnProcess( float fTimePerSec )
{
	if( m_Type == TYPE_NONE )
		return;

	ioLSMouse *pMouse = g_App.GetMouse();
	POINT pt = pMouse->GetMousePos();
	SetScreenWndPos( pt.x - GetWidth() - 1, pt.y - GetHeight() - 1 );
}

void CloverToolTip::OnRender()
{
	if( m_Type == TYPE_NONE )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_Type == TYPE_SEND )
		RenderSendInfoString( iXPos, iYPos );
	else if( m_Type == TYPE_RECV )
		RenderRecvInfoString( iXPos, iYPos );
}

void CloverToolTip::RenderSendInfoString( int iXPos, int iYPos )
{
	ioClover *pClover = g_MyInfo.GetClover();
	if( !pClover )
		return;

	int iWidth = 0;
	int iHeight = 0;

	if( m_bFriendRegTime )
	{
		m_Printer[0].SetTextStyle( TS_NORMAL );
		m_Printer[0].SetBkColor( 0, 0, 0 );
		m_Printer[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_Printer[0].AddTextPiece( FONT_SIZE_12, "친구가 된지 " );
		m_Printer[0].SetTextColor( TCT_DEFAULT_RED );
		if( pClover->GetAbusingTime() <= 60 )
			m_Printer[0].AddTextPiece( FONT_SIZE_12, "%d분", pClover->GetAbusingTime() );
		else
			m_Printer[0].AddTextPiece( FONT_SIZE_12, "%d시간", pClover->GetAbusingTime()/60 );
		m_Printer[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_Printer[0].AddTextPiece( FONT_SIZE_12, " 후 부터" );

		m_Printer[1].SetTextStyle( TS_NORMAL );
		m_Printer[1].SetBkColor( 0, 0, 0 );
		m_Printer[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_Printer[1].AddTextPiece( FONT_SIZE_12, "클로버를 주고 받을 수 있습니다." );

		ioHashString szRemainTime;
		pClover->GetFriendRegRemainTime( m_szUserName, szRemainTime );
		m_Printer[2].SetTextStyle( TS_NORMAL );
		m_Printer[2].SetBkColor( 0, 0, 0 );
		m_Printer[2].SetTextColor( TCT_DEFAULT_RED );
		m_Printer[2].AddTextPiece( FONT_SIZE_12, "%s", szRemainTime.c_str() );
		m_Printer[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_Printer[2].AddTextPiece( FONT_SIZE_12, " 후 부터 보낼 수 있습니다." );

		iWidth = m_Printer[2].GetFullWidth() + SIDE_TEXT_GAP;
		iHeight = WND_HEIGHT + TEXT_LINE_GAP * 2;
		SetSize( iWidth, iHeight );
	}
	else
	{
		m_Printer[0].SetTextStyle( TS_NORMAL );
		m_Printer[0].SetBkColor( TCT_DEFAULT_DARKGRAY );	
		m_Printer[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		
		char szBuf[MAX_PATH];
		sprintf_s( szBuf, "%s 님께 현재까지", m_szUserName.c_str() );
		
		if( g_FontMgr.GetTextWidth( szBuf, TS_NORMAL, FONT_SIZE_13 ) >= 169 )
		{
			char szTextWidthCut[MAX_PATH];
			Help::StringCutFun( FONT_SIZE_12, 164, TS_NORMAL, szTextWidthCut, MAX_PATH, szBuf );
			sprintf_s( szBuf, "%s...", szTextWidthCut );
		}		
		m_Printer[0].AddTextPiece( FONT_SIZE_12, "%s", szBuf );

		m_Printer[1].SetTextStyle( TS_NORMAL );
		m_Printer[1].SetBkColor( TCT_DEFAULT_DARKGRAY );	
		m_Printer[1].SetTextColor( 141, 87, 4 );
		m_Printer[1].AddTextPiece( FONT_SIZE_12, "받은 클로버 : %d", m_iRecvCloverCount );

		m_Printer[2].SetTextStyle( TS_NORMAL );
		m_Printer[2].SetBkColor( TCT_DEFAULT_DARKGRAY );	
		m_Printer[2].SetTextColor( 17, 114, 34 );
		m_Printer[2].AddTextPiece( FONT_SIZE_12, "보낸 클로버 : %d", m_iSendCloverCount );

		m_Printer[3].SetTextStyle( TS_NORMAL );
		m_Printer[3].SetBkColor( TCT_DEFAULT_DARKGRAY );

		if( m_bSendRemainTime )
		{
			ioHashString szRemainTime;
			pClover->GetSendRemainTime( m_szUserName, szRemainTime );
			m_Printer[3].SetTextColor( TCT_DEFAULT_RED );
			m_Printer[3].AddTextPiece( FONT_SIZE_12, "%s", szRemainTime.c_str() );

			m_Printer[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
			m_Printer[3].AddTextPiece( FONT_SIZE_12, " 후 다시 보낼 수 있습니다." );
		}
		else
		{
			m_Printer[3].SetTextStyle( TS_NORMAL );
			m_Printer[3].SetBkColor( TCT_DEFAULT_DARKGRAY );

			m_Printer[3].SetTextColor( TCT_DEFAULT_BLUE );
			m_Printer[3].AddTextPiece( FONT_SIZE_12, "클로버를 보내시려면 " );

			m_Printer[3].SetTextColor( TCT_DEFAULT_RED );
			m_Printer[3].AddTextPiece( FONT_SIZE_12, "클릭" );

			m_Printer[3].SetTextColor( TCT_DEFAULT_BLUE );
			m_Printer[3].AddTextPiece( FONT_SIZE_12, "하세요." );
		}
		
		iWidth = m_Printer[3].GetFullWidth() + SIDE_TEXT_GAP;
		iHeight = WND_HEIGHT + TEXT_LINE_GAP * 3;
		SetSize( iWidth, iHeight );
	}

	for( int i=0; i<MAX_MSG_LIST_SIZE; ++i )
	{
		if( m_Printer[i].IsEmpty() )
			continue;

		m_Printer[i].PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS + TEXT_LINE_GAP * i, TAT_LEFT );
		m_Printer[i].ClearList();
	}
}

void CloverToolTip::RenderRecvInfoString( int iXPos, int iYPos )
{
	ioClover *pClover = g_MyInfo.GetClover();
	if( !pClover )
		return;

	m_Printer[0].SetTextStyle( TS_NORMAL );
	m_Printer[0].SetBkColor( TCT_DEFAULT_DARKGRAY );	
	m_Printer[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Printer[0].AddTextPiece( FONT_SIZE_12, "%s 님으로부터", m_szUserName.c_str() );

	m_Printer[1].SetTextStyle( TS_NORMAL );
	m_Printer[1].SetBkColor( TCT_DEFAULT_DARKGRAY );	
	m_Printer[1].SetTextColor( 141, 87, 4 );
	m_Printer[1].AddTextPiece( FONT_SIZE_12, "클로버 %d개 도착", m_iRecvWaitCloverCount );

	ioHashString szRemainTime;
	pClover->GetRecvRemainTime( m_szUserName, szRemainTime );
	m_Printer[2].SetTextStyle( TS_NORMAL );
	m_Printer[2].SetBkColor( TCT_DEFAULT_DARKGRAY );	
	m_Printer[2].SetTextColor( TCT_DEFAULT_RED );
	m_Printer[2].AddTextPiece( FONT_SIZE_12, "%s", szRemainTime.c_str() );
	m_Printer[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Printer[2].AddTextPiece( FONT_SIZE_12, " 후 사라집니다." );

	m_Printer[3].SetTextStyle( TS_NORMAL );
	m_Printer[3].SetBkColor( TCT_DEFAULT_DARKGRAY );

	m_Printer[3].SetTextColor( TCT_DEFAULT_BLUE );
	m_Printer[3].AddTextPiece( FONT_SIZE_12, "클로버를 받으시려면 " );

	m_Printer[3].SetTextColor( TCT_DEFAULT_RED );
	m_Printer[3].AddTextPiece( FONT_SIZE_12, "클릭" );

	m_Printer[3].SetTextColor( TCT_DEFAULT_BLUE );
	m_Printer[3].AddTextPiece( FONT_SIZE_12, "하세요." );
	int iWidth = m_Printer[3].GetFullWidth();

	SetSize( iWidth + SIDE_TEXT_GAP, WND_HEIGHT + TEXT_LINE_GAP * 3 );

	for( int i=0; i<4; ++i )
	{
		m_Printer[i].PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS + TEXT_LINE_GAP * i, TAT_LEFT );
		m_Printer[i].ClearList();
	}
}

void CloverToolTip::SetInfo( const ioHashString &szUserName, int iType )
{
	m_szUserName = szUserName;
	FriendNode *pFriend = g_FriendManager.GetNode( m_szUserName );
	if( !pFriend )
		return;

	ioClover *pClover = g_MyInfo.GetClover();
	if( !pClover )
		return;

	m_iSendCloverCount = pFriend->GetCloverSendCount();
	m_iRecvCloverCount = pFriend->GetCloverRecvCount();
	m_iRecvWaitCloverCount = pFriend->GetCloverRecvWaitCount();

	if( iType == UserInfoListWnd::ID_SEND_CLOVER )
	{
		m_Type = TYPE_SEND;

		if( pClover->CheckFriendRegTime( m_szUserName ) )
			m_bFriendRegTime = true;
		else if( pClover->CheckSendRemainTime( m_szUserName ) )
			m_bSendRemainTime = true;
	}
	else
	{
		m_Type = TYPE_RECV;
	}

	if( !IsShow() )
		ShowWnd();

	MessengerWnd *pMessenger = dynamic_cast<MessengerWnd*>( g_GUIMgr.FindWnd(MESSENGER_WND) );
	if( pMessenger )
	{
		UserListWnd *pUserList = dynamic_cast<UserListWnd*>( pMessenger->FindChildWnd(MessengerWnd::ID_USER_LIST_WND) );
		if( pUserList )
		{
			pUserList->HideChildWnd( UserListWnd::ID_OPTION_WND );
		}
	}
}