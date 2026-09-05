

#include "StdAfx.h"

#include "../ioGuildChatMgr.h"

#include "ioMemoManager.h"
#include "UserListWnd.h"
#include "MemoListWnd.h"
#include "HelpWnd.h"
#include "ChannelChatWnd.h"
#include "MessengerWnd.h"
//////////////////////////////////////////////////////////////////////////
MessengerPopup::MessengerPopup()
{
	m_pTooltipLeft = NULL;
	m_pTooltipLeftIn = NULL;
	m_pTooltipCenter = NULL;
	m_pTooltipRightIn = NULL;
	m_pTooltipRight = NULL;

	m_iPopupType = 0;
	m_iChannelID = -1;
	m_fTextWidth = 0.0f;
	m_dwShowTime = 0;
}

MessengerPopup::~MessengerPopup()
{
	SAFEDELETE( m_pTooltipLeft );
	SAFEDELETE( m_pTooltipLeftIn );
	SAFEDELETE( m_pTooltipCenter );
	SAFEDELETE( m_pTooltipRightIn );
	SAFEDELETE( m_pTooltipRight );
}

void MessengerPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TooltipLeft" )
	{
		SAFEDELETE( m_pTooltipLeft );
		m_pTooltipLeft = pImage;
	}
	else if( szType == "TooltipLeftIn" )
	{
		SAFEDELETE( m_pTooltipLeftIn );
		m_pTooltipLeftIn = pImage;
	}
	else if( szType == "TooltipCenter" )
	{
		SAFEDELETE( m_pTooltipCenter );
		m_pTooltipCenter = pImage;
	}
	else if( szType == "TooltipRightIn" )
	{
		SAFEDELETE( m_pTooltipRightIn );
		m_pTooltipRightIn = pImage;
	}
	else if( szType == "TooltipRight" )
	{
		SAFEDELETE( m_pTooltipRight );
		m_pTooltipRight = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MessengerPopup::ShowPopup( int iType, ioHashString szID, ioHashString szContents, int iChannelID /* = -1  */ )
{
	if( g_MyInfo.IsTutorialUser() ) return;

	if( iChannelID != -1 )
	{
		// 채널 채팅 윈도우를 보고 있으면 팝업을 띄우지 않는다.
		MessengerWnd *pWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
		if( pWnd && pWnd->IsViewChannel( iChannelID ) )
			return;
	}

	if( iType == GUILD_POPUP )
	{
		// 길드 채팅을 보고 있으면 팝업을 띄우지 않는다.
		MessengerWnd *pWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
		if( pWnd && pWnd->IsViewGuild() )
			return;
	}

	m_iPopupType = iType;
	m_szID       = szID;
	m_iChannelID = iChannelID;	

	switch( m_iPopupType )
	{
	case FRIEND_POPUP:
		{
			char szFirstContents[MAX_PATH] = "";
			sprintf_e( szFirstContents, "%s", m_szID.c_str() );
			m_szFirstContents = szFirstContents;

			char szLastContents[MAX_PATH] = "";
			SafeSprintf( szLastContents, sizeof( szLastContents ), STR(1) );
			m_szLastContents = szLastContents;
		}
		break;
	case MEMO_POPUP:
		{
			char szFirstContents[MAX_PATH] = "";
			SafeSprintf( szFirstContents, sizeof( szFirstContents ), STR(2), g_MemoManager.GetNotViewMemoCount() );
			m_szFirstContents = szFirstContents;

			char szLastContents[MAX_PATH] = "";
			SafeSprintf( szLastContents, sizeof( szLastContents ), STR(3) );
			m_szLastContents = szLastContents;
		}
		break;
	case CHANNEL_POPUP:
		{			
			if( strcmp( szContents.c_str(), "" ) == 0 )
			{
				// 초대
				char szFirstContents[MAX_PATH] = "";
				SafeSprintf( szFirstContents, sizeof( szFirstContents ), STR(4), m_iChannelID );
				m_szFirstContents = szFirstContents;

				char szLastContents[MAX_PATH] = "";
				SafeSprintf( szLastContents, sizeof( szLastContents ), STR(5) );
				m_szLastContents = szLastContents;
			}
			else
			{
				// 채팅
				char szFirstContents[MAX_PATH] = "";
				sprintf_e( szFirstContents, "%s : ", m_szID.c_str() );
				m_szFirstContents = szFirstContents;

				char szLastContents[MAX_PATH] = "";
				sprintf_e( szLastContents, "%s", szContents.c_str() );
				m_szLastContents = szLastContents;
			}
		}
		break;			
	case GUILD_POPUP:
		{
			// 채팅
			char szFirstContents[MAX_PATH] = "";
			sprintf_e( szFirstContents, "%s : ", m_szID.c_str() );
			m_szFirstContents = szFirstContents;

			char szLastContents[MAX_PATH] = "";
			sprintf_e( szLastContents, "%s", szContents.c_str() );
			m_szLastContents = szLastContents;
		}
		break;
	case BEST_FRIEND_POPUP:
		{
			char szFirstContents[MAX_PATH] = "";
			sprintf_e( szFirstContents, "%s", m_szID.c_str() );
			m_szFirstContents = szFirstContents;

			char szLastContents[MAX_PATH] = "";
			SafeSprintf( szLastContents, sizeof( szLastContents ), STR(6) );
			m_szLastContents = szLastContents;
		}
		break;
	default:
		return;
	}

	ShowWnd();
}

void MessengerPopup::iwm_show()
{
	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		HideWnd();
		return;
	}

	if( g_MyInfo.IsTutorialUser() )
	{
		HideWnd();
		return;
	}

	m_dwShowTime = FRAMEGETTIME();
	char szContents[MAX_PATH * 2] = "";
	sprintf_e( szContents, "%s%s", m_szFirstContents.c_str(), m_szLastContents.c_str() );
	m_fTextWidth = min( 303.0f, g_FontMgr.GetTextWidth( szContents, TS_NORMAL, FONT_SIZE_13 ) );
	SetSize( m_fTextWidth + 20, GetHeight() );

	int iXPos = HelpWnd::GetMessengerXPos() - ( m_fTextWidth - 15 );
	int iYPos = min( HelpWnd::GetMessengerYPos() - 28, Setting::Height() - GetHeight() );
	SetWndPos( iXPos, iYPos );

	ioWnd *pCommandBtn = FindChildWnd( ID_COMMAND );
	if( pCommandBtn )
		pCommandBtn->SetSize( GetWidth(), GetHeight() );
}

void MessengerPopup::iwm_hide()
{
	m_iPopupType = 0;
	m_iChannelID = -1;
	m_dwShowTime = 0;
	m_fTextWidth = 0.0f;
}

void MessengerPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMMAND:
		if( cmd == IOBN_BTNUP )
		{
			switch( m_iPopupType )
			{
			case MEMO_POPUP:
				{
					MessengerWnd *pMessengerMemo = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
					if( pMessengerMemo )
						pMessengerMemo->MoveMemo( (char*)m_szID.c_str() );
					HideWnd();
				}
				break;
			case CHANNEL_POPUP:	
			case GUILD_POPUP:
				{
					MessengerWnd *pMessengerMemo = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
					if( pMessengerMemo )
					{
						if( m_iPopupType == CHANNEL_POPUP )
							pMessengerMemo->MoveChannel( m_iChannelID );
						else
							pMessengerMemo->MoveGuild();
					}
					HideWnd();
				}
				break;
			}
		}
		break;
	}
}

void MessengerPopup::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwShowTime > 6000 )
		HideWnd();
	else
	{
		int iXPos = HelpWnd::GetMessengerXPos() - ( m_fTextWidth - 15 );
		int iYPos = min( HelpWnd::GetMessengerYPos() - 28, Setting::Height() - GetHeight() );
		if( GetXPos() != iXPos || GetYPos() != iYPos )
			SetWndPos( iXPos, iYPos );
	}
}

void MessengerPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderTooltip( iXPos, iYPos - 6 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( TCT_DEFAULT_DARKGRAY );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), m_szFirstContents.c_str() );
	
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( TCT_DEFAULT_GRAY );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), m_szLastContents.c_str() );
	kPrinter.PrintFullTextWidthCut( iXPos + 9, iYPos + 10, TAT_LEFT, 303.0f );
	kPrinter.ClearList();
}

void MessengerPopup::OnRenderTooltip( int iXPos, int iYPos )
{
	if( !m_pTooltipLeft || !m_pTooltipLeftIn || !m_pTooltipCenter || !m_pTooltipRightIn || !m_pTooltipRight ) return;

	m_pTooltipLeft->Render( iXPos - 7, iYPos );
	m_pTooltipLeftIn->SetSize( m_fTextWidth + 4, m_pTooltipLeftIn->GetHeight() );
	m_pTooltipLeftIn->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pTooltipLeftIn->Render( iXPos + 8, iYPos );
	m_pTooltipCenter->Render( iXPos + m_fTextWidth - 7, iYPos );
	m_pTooltipRight->Render( iXPos + m_fTextWidth + 12, iYPos );
}
//////////////////////////////////////////////////////////////////////////
MessengerListBtn::MessengerListBtn()
{
}

MessengerListBtn::~MessengerListBtn()
{
}

void MessengerListBtn::SetActive()
{
	ioRadioButton::SetActive();
	switch( GetID() )
	{
	case MessengerWnd::ID_CHANNEL_1:		
	case MessengerWnd::ID_CHANNEL_2:
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), ( GetID() - MessengerWnd::ID_CHANNEL_1 ) + 1 );
			SetTitleText( szTitle );
		}
		break;
	}	
}

void MessengerListBtn::SetInActive()
{
	ioRadioButton::SetInActive();
	switch( GetID() )
	{
	case MessengerWnd::ID_CHANNEL_1:		
	case MessengerWnd::ID_CHANNEL_2:
		{
			SetTitleText( STR(1) );
		}
		break;
	}	
}

void MessengerListBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	switch( GetID() )
	{
	case MessengerWnd::ID_USER_LIST:
		break;
	case MessengerWnd::ID_MEMO_LIST:
		break;
	case MessengerWnd::ID_CHANNEL_1:		
	case MessengerWnd::ID_CHANNEL_2:
		{
			ChannelNode *pNode = g_ChannelManager.GetNodeArray( GetID() - MessengerWnd::ID_CHANNEL_1 );
			if( pNode )
			{
				if( IsDisabled() )
				{
					SetActive();
				}
			}
			else if( !IsDisabled() )
			{
				SetInActive();
			}
		}
		break;
	case MessengerWnd::ID_GUILD:
		{
			if( g_GuildInfo.IsGuild() )
			{
				if( IsDisabled() )
					SetActive();
			}
			else if( !IsDisabled() )
			{
				SetInActive();
			}
		}
		break;
	}	
}

void MessengerListBtn::OnRender()
{
	ioRadioButton::OnRender();

	if( GetID() == MessengerWnd::ID_MEMO_LIST )
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();

		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
        
		int iCount = g_MemoManager.GetNotViewMemoCount();
		char szNum[MAX_PATH] = "";
		SafeSprintf( szNum, sizeof( szNum ), STR(2), iCount );
		if( iCount > 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
		}
		kPrinter.AddTextPiece( FONT_SIZE_13, szNum );
		kPrinter.PrintFullText( iXPos + 34, iYPos + 5, TAT_CENTER );
	}
}
//////////////////////////////////////////////////////////////////////////
int MessengerWnd::sg_iTabID = 0;
MessengerWnd::MessengerWnd()
{
	m_iFriendHelpID = 0;
	m_iBestFriendHelpID = 0;
	m_iCloverHelpID = 0;

	m_iSendCloverMovingAniTime = 350;
	m_iRecvCloverMovingAniTime = 350;
	m_iSendCloverEndAniTime = 200;
	m_iRecvCloverEndAniTime = 200;

	m_pCloverInfoBox    = NULL;

	m_pCloverAniIconGold  = NULL;
	m_pCloverAniIconGreen = NULL;

	m_pCloverIconGold   = NULL;

	m_pCloverIconGreen1 = NULL;
	m_pCloverIconGreen2 = NULL;
	m_pCloverIconGreen3 = NULL;
	m_pCloverIconGreen4 = NULL;
	m_pCloverIconGreen5 = NULL;
	m_pCloverIconEmpty1 = NULL;
	m_pCloverIconEmpty2 = NULL;
	m_pCloverIconEmpty3 = NULL;
	m_pCloverIconEmpty4 = NULL;
	m_pCloverIconEmpty5 = NULL;

	m_MyCloverAniList.clear();
	m_GiftCloverAniList.clear();
}

MessengerWnd::~MessengerWnd()
{
	SAFEDELETE( m_pCloverInfoBox );
	SAFEDELETE( m_pCloverAniIconGold );
	SAFEDELETE( m_pCloverAniIconGreen );
	SAFEDELETE( m_pCloverIconGold );
	SAFEDELETE( m_pCloverIconGreen1 );
	SAFEDELETE( m_pCloverIconGreen2 );
	SAFEDELETE( m_pCloverIconGreen3 );
	SAFEDELETE( m_pCloverIconGreen4 );
	SAFEDELETE( m_pCloverIconGreen5 );
	SAFEDELETE( m_pCloverIconEmpty1 );
	SAFEDELETE( m_pCloverIconEmpty2 );
	SAFEDELETE( m_pCloverIconEmpty3 );
	SAFEDELETE( m_pCloverIconEmpty4 );
	SAFEDELETE( m_pCloverIconEmpty5 );
}

void MessengerWnd::iwm_create()
{	
	g_GUIMgr.AddWnd( "XML/UserListWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/MemoListWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ChannelChatWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/GuildChatWnd.xml", this );
}

void MessengerWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CloverInfoBox" )
	{
		SAFEDELETE( m_pCloverInfoBox );
		m_pCloverInfoBox = pImage;
	}
	else if( szType == "CloverAniIconGold" )
	{
		SAFEDELETE( m_pCloverAniIconGold );
		m_pCloverAniIconGold = pImage;
	}
	else if( szType == "CloverAniIconGreen" )
	{
		SAFEDELETE( m_pCloverAniIconGreen );
		m_pCloverAniIconGreen = pImage;
	}
	else if( szType == "CloverIconGold" )
	{
		SAFEDELETE( m_pCloverIconGold );
		m_pCloverIconGold = pImage;
	}
	else if( szType == "CloverIconGreen1" )
	{
		SAFEDELETE( m_pCloverIconGreen1 );
		m_pCloverIconGreen1 = pImage;
	}
	else if( szType == "CloverIconGreen2" )
	{
		SAFEDELETE( m_pCloverIconGreen2 );
		m_pCloverIconGreen2 = pImage;
	}
	else if( szType == "CloverIconGreen3" )
	{
		SAFEDELETE( m_pCloverIconGreen3 );
		m_pCloverIconGreen3 = pImage;
	}
	else if( szType == "CloverIconGreen4" )
	{
		SAFEDELETE( m_pCloverIconGreen4 );
		m_pCloverIconGreen4 = pImage;
	}
	else if( szType == "CloverIconGreen5" )
	{
		SAFEDELETE( m_pCloverIconGreen5 );
		m_pCloverIconGreen5 = pImage;
	}
	else if( szType == "CloverIconEmpty1" )
	{
		SAFEDELETE( m_pCloverIconEmpty1 );
		m_pCloverIconEmpty1 = pImage;
	}
	else if( szType == "CloverIconEmpty2" )
	{
		SAFEDELETE( m_pCloverIconEmpty2 );
		m_pCloverIconEmpty2 = pImage;
	}
	else if( szType == "CloverIconEmpty3" )
	{
		SAFEDELETE( m_pCloverIconEmpty3 );
		m_pCloverIconEmpty3 = pImage;
	}
	else if( szType == "CloverIconEmpty4" )
	{
		SAFEDELETE( m_pCloverIconEmpty4 );
		m_pCloverIconEmpty4 = pImage;
	}
	else if( szType == "CloverIconEmpty5" )
	{
		SAFEDELETE( m_pCloverIconEmpty5 );
		m_pCloverIconEmpty5 = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MessengerWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iFriendHelpID = xElement.GetIntAttribute_e( "FriendHelpID" );
	m_iBestFriendHelpID = xElement.GetIntAttribute_e( "BestFriendHelpID" );
	m_iCloverHelpID = xElement.GetIntAttribute_e( "CloverHelpID" );
	m_iSendCloverMovingAniTime = xElement.GetIntAttribute_e( "SendCloverMovingAniTime" );
	m_iRecvCloverMovingAniTime = xElement.GetIntAttribute_e( "RecvCloverMovingAniTime" );
	int m_iSendCloverEndAniTime = xElement.GetIntAttribute_e( "SendCloverEndAniTime" );
	int m_iRecvCloverEndAniTime = xElement.GetIntAttribute_e( "RecvCloverEndAniTime" );
}

void MessengerWnd::iwm_show()
{	
	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		HideWnd();
		return;
	}

	Init();
}

void MessengerWnd::iwm_hide()
{	
	HideChildWnd( ID_USER_LIST_WND );
	HideChildWnd( ID_MEMO_LIST_WND );
	HideChildWnd( ID_CHANNEL_CHAT_WND );
	HideChildWnd( ID_GUILD_CHAT_WND );
	m_MyCloverAniList.clear();
	m_GiftCloverAniList.clear();
}

void MessengerWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		switch( sg_iTabID )
		{
		case ID_CHANNEL_1:
		case ID_CHANNEL_2:
			{
				ioWnd *pWnd = FindChildWnd( ID_CHANNEL_CHAT_WND );
				if( pWnd && pWnd->IsShow() )
					pWnd->iwm_wheel( zDelta );
			}
			break;
		case ID_GUILD:
			{
				ioWnd *pWnd = FindChildWnd( ID_GUILD_CHAT_WND );
				if( pWnd && pWnd->IsShow() )
					pWnd->iwm_wheel( zDelta );
			}
			break;
		}
	}
}

void MessengerWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	ioMovingWnd::iwm_lbuttondown( mouse );
	switch( sg_iTabID )
	{	
	case ID_CHANNEL_1:
	case ID_CHANNEL_2:
		{
			ChannelChatWnd *pWnd = dynamic_cast<ChannelChatWnd*>( FindChildWnd( ID_CHANNEL_CHAT_WND ) );
			if( pWnd && pWnd->IsShow() )
				pWnd->SetChatFocus();
		}
		break;
	case ID_GUILD:
		{
			GuildChatWnd *pWnd = dynamic_cast<GuildChatWnd*>( FindChildWnd( ID_GUILD_CHAT_WND ) );
			if( pWnd && pWnd->IsShow() )
				pWnd->SetChatFocus();
		}
		break;
	}
}

void MessengerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_FRIEND_HELP:
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_iFriendHelpID );
		}
		break;
	case ID_BESTFRIEND_HELP:
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_iBestFriendHelpID );
		}
		break;
	case ID_CLOVER_HELP:
		if( cmd == IOBN_BTNUP )
		{
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_iCloverHelpID );
		}
		break;
	case ID_USER_LIST:
	case ID_MEMO_LIST:
	case ID_CHANNEL_1:
	case ID_CHANNEL_2:
	case ID_GUILD:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckButton( dwID );
		}
		break;
	case ID_USER_LIST_WND:
	case ID_MEMO_LIST_WND:
		if( cmd == CMD_MEMO )
		{
			if( param != 0 )
			{
				char szName[MAX_PATH] = "";
				sprintf_e( szName, "%s", param );
				MoveMemo( szName );
			}			
		}
		else if( cmd == CMD_INVITE )
		{
			if( param != 0 )
			{
				char szName[MAX_PATH] = "";
				sprintf_e( szName, "%s", param );			
				
				if( !QuickChannelInvite( szName ) )
					ChannelCreate( CHANNEL_MY_CHANNEL_FULL, szName );
			}
		}
		break;
	}
}

bool MessengerWnd::QuickChannelInvite( ioHashString szInviteName )
{
	if( g_ChannelManager.GetNodeUser( szInviteName, 2 ) )
	{
		ChannelNode *pNode = g_ChannelManager.GetNodeUser( szInviteName, 2 );
		if( pNode )
		{
			if( !IsShow() )
				ShowWnd();
			MoveChannel( pNode->GetIndex() );			
			return true;
		}
	}
	else if( g_ChannelManager.GetNodeSize() < MAX_CHANNEL )
	{				
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_CHANNEL_CREATE );
		kPacket << szInviteName;
		TCPNetwork::SendToServer( kPacket );					
		return true;
	}
	return false;
}

void MessengerWnd::CheckButton( DWORD dwID )
{
	sg_iTabID = dwID;
	CheckRadioButton( ID_USER_LIST, ID_GUILD, sg_iTabID );
    
	switch( sg_iTabID )
	{
	case ID_USER_LIST:
		ShowChildWnd( ID_USER_LIST_WND );

		HideChildWnd( ID_MEMO_LIST_WND );
		HideChildWnd( ID_CHANNEL_CHAT_WND );
		HideChildWnd( ID_GUILD_CHAT_WND );
		break;
	case ID_MEMO_LIST:
		HideChildWnd( ID_USER_LIST_WND );
		ShowChildWnd( ID_MEMO_LIST_WND );
		HideChildWnd( ID_CHANNEL_CHAT_WND );
		HideChildWnd( ID_GUILD_CHAT_WND );
		break;
	case ID_CHANNEL_1:
	case ID_CHANNEL_2:
		{
			HideChildWnd( ID_USER_LIST_WND );
			HideChildWnd( ID_MEMO_LIST_WND );
			HideChildWnd( ID_GUILD_CHAT_WND );
			ChannelChatWnd *pWnd = (ChannelChatWnd *)FindChildWnd( ID_CHANNEL_CHAT_WND );
			if( pWnd )
			{
				ChannelNode *pNode = g_ChannelManager.GetNodeArray( sg_iTabID - ID_CHANNEL_1 );
				if( pNode )
				{
					pWnd->SetChannelIndex( pNode->GetIndex() );
					pWnd->ShowWnd();
				}
			}
		}
		break;
	case ID_GUILD:
		HideChildWnd( ID_USER_LIST_WND );
		HideChildWnd( ID_MEMO_LIST_WND );
		HideChildWnd( ID_CHANNEL_CHAT_WND );
		ShowChildWnd( ID_GUILD_CHAT_WND );
		break;
	}
}

void MessengerWnd::MoveChannel( int iIndex )
{
	int iArray = g_ChannelManager.GetIndexToArray( iIndex );
	if( iArray == -1 )
		return;

	if( !IsShow() )
		ShowWnd();
	CheckButton( ID_CHANNEL_1 + iArray );
}

void MessengerWnd::MoveMemo( const char *szID )
{
	if( !IsShow() )
		ShowWnd();

	CheckButton( ID_MEMO_LIST );
	MemoListWnd *pWnd = dynamic_cast<MemoListWnd*>( FindChildWnd( ID_MEMO_LIST_WND ) );
	if( pWnd )
		pWnd->SetUserID( szID );
}

void MessengerWnd::MoveGuild()
{
	if( !IsShow() )
		ShowWnd();
	CheckButton( ID_GUILD );
}

bool MessengerWnd::IsViewChannel( int iIndex )
{
	if( !IsShow() )
		return false;

	if( COMPARE( sg_iTabID, ID_CHANNEL_1, ID_CHANNEL_2 + 1 ) )
	{
		ChannelNode *pNode = g_ChannelManager.GetNodeArray( sg_iTabID - ID_CHANNEL_1 );
		if( pNode && pNode->GetIndex() == iIndex )
			return true;
	}

	return false;
}

bool MessengerWnd::IsViewGuild()
{
	if( !IsShow() )
		return false;

	if( sg_iTabID == ID_GUILD )
		return true;
	return false;
}

void MessengerWnd::ChangeChannelIndex( int iPrevIndex, int iChangeIndex )
{
	ChannelChatWnd *pWnd = (ChannelChatWnd *)FindChildWnd( ID_CHANNEL_CHAT_WND );
	if( pWnd && pWnd->GetChannelIndex() == iPrevIndex )
		pWnd->SetChannelIndex( iChangeIndex );
}

void MessengerWnd::InviteResult( int iResult, ioHashString szInvitedName )
{
	switch( sg_iTabID )
	{
	case ID_USER_LIST:
		{
			UserListWnd *pWnd = dynamic_cast<UserListWnd*>( FindChildWnd( ID_USER_LIST_WND ) );
			if( pWnd )
				pWnd->InviteResult( iResult, szInvitedName );
		}
		break;
	case ID_MEMO_LIST:
		{
			MemoListWnd *pWnd = dynamic_cast<MemoListWnd*>( FindChildWnd( ID_MEMO_LIST_WND ) );
			if( pWnd )
				pWnd->InviteResult( iResult, szInvitedName );
		}		
		break;
	case ID_CHANNEL_1:
	case ID_CHANNEL_2:
		{
			ChannelChatWnd *pWnd = dynamic_cast<ChannelChatWnd*>( FindChildWnd( ID_CHANNEL_CHAT_WND ) );
			if( pWnd )
				pWnd->InviteResult( iResult, szInvitedName );
		}
		break;;
	}
}

void MessengerWnd::ChannelCreate( int iResult, ioHashString szManToManName )
{
	switch( sg_iTabID )
	{
	case ID_USER_LIST:
		{
			UserListWnd *pWnd = dynamic_cast<UserListWnd*>( FindChildWnd( ID_USER_LIST_WND ) );
			if( pWnd )
				pWnd->ChannelCreate( iResult, szManToManName );
		}
		break;
	case ID_MEMO_LIST:
		{
			MemoListWnd *pWnd = dynamic_cast<MemoListWnd*>(FindChildWnd( ID_MEMO_LIST_WND ) );
			if( pWnd )
				pWnd->ChannelCreate( iResult, szManToManName );
		}		
		break;
	}
}

void MessengerWnd::Init()
{
	if( sg_iTabID == 0 )
		sg_iTabID = ID_USER_LIST;

	int i = 0;
	for(int i = ID_CHANNEL_1;i < ID_CHANNEL_2+1;i++)
	{
		ChannelNode *pNode = g_ChannelManager.GetNodeArray( i - ID_CHANNEL_1 );
		if( pNode )
			SetChildActive( i );
		else
		{
			SetChildInActive( i );
			if( i == sg_iTabID )
				sg_iTabID = ID_USER_LIST;
		}
	}

	if( !g_GuildInfo.IsGuild() )
	{
		if( sg_iTabID == ID_GUILD )
			sg_iTabID = ID_USER_LIST;
	}

	if( MessengerBtn::sg_bMessenger )
	{
		if( g_MemoManager.IsUpdate() )
			sg_iTabID = ID_MEMO_LIST;
		else if( g_GuildChatMgr.IsUpdate() )
			sg_iTabID = ID_GUILD;
		else
		{
			for(int i = ID_CHANNEL_1;i < ID_CHANNEL_2+1;i++)
			{
				ChannelNode *pNode = g_ChannelManager.GetNodeArray( i - ID_CHANNEL_1 );
				if( pNode && pNode->IsUpdate() )
					sg_iTabID = i;
			}
		}
	}

	CheckButton( sg_iTabID );

	m_MyCloverAniList.clear();
}

void MessengerWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	UserListWnd *pUserList = dynamic_cast<UserListWnd*>( FindChildWnd( ID_USER_LIST_WND ) );
	if( pUserList )
		pUserList->SetPlayStage( pPlayStage );
}

bool MessengerWnd::IsChatBlock()
{
	if( !IsShow() )
		return false;

	switch( sg_iTabID )
	{
	case ID_USER_LIST:
		{
			UserListWnd *pWnd = dynamic_cast<UserListWnd*>( FindChildWnd( ID_USER_LIST_WND ) );
			if( pWnd )
				return pWnd->IsChatBlock();
		}
		break;
	case ID_MEMO_LIST:
		{
			MemoListWnd *pWnd = dynamic_cast<MemoListWnd*>( FindChildWnd( ID_MEMO_LIST_WND ) );
			if( pWnd )
				return pWnd->IsChatBlock();
		}		
		break;
	case ID_CHANNEL_1:
	case ID_CHANNEL_2:
		{
			ChannelChatWnd *pWnd = dynamic_cast<ChannelChatWnd*>( FindChildWnd( ID_CHANNEL_CHAT_WND ) );
			if( pWnd )
				return pWnd->IsChatBlock();
		}
		break;
	case ID_GUILD:
		{
			GuildChatWnd *pWnd = dynamic_cast<GuildChatWnd*>( FindChildWnd( ID_GUILD_CHAT_WND ) );
			if( pWnd )
				return pWnd->IsChatBlock();
		}
		break;
	}
	return false;
}

void MessengerWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	MessengerBtn::sg_bMessenger = false;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void MessengerWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pCloverInfoBox )
		m_pCloverInfoBox->Render( iXPos, iYPos );

	// 나의 클로버
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( 141, 87, 4 );
	kPrinter.AddTextPiece( FONT_SIZE_12, "나의" );
	kPrinter.PrintFullText( iXPos + 23, iYPos + 61, TAT_LEFT );
	int iXGap = 23 + kPrinter.GetFullWidth();
	kPrinter.ClearList();
	
	if( m_pCloverIconGold )
	{
		m_CloverGoldPosition.x = iXPos + iXGap + 2;
		m_CloverGoldPosition.y = iYPos + 57;
		m_pCloverIconGold->SetAlpha( MAX_ALPHA_RATE );
		m_pCloverIconGold->Render( iXPos + iXGap + 2, iYPos + 57 );
		iXGap += m_pCloverIconGold->GetWidth() + 2;
	}

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( 141, 87, 4 );
	kPrinter.AddTextPiece( FONT_SIZE_12, "×%d", g_MyInfo.GetEtcCoinMoney(ioEtcItem::EIT_ETC_CLOVER) );
	kPrinter.PrintFullText( iXPos + iXGap, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();

	int iMin = 0, iSec = 0;
	ioClover *pClover = g_MyInfo.GetClover();
	// 선물용 클로버
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( 17, 114, 34 );
	if( pClover )
	{
		if( pClover->IsMaxCharge() )
		{
			kPrinter.AddTextPiece( FONT_SIZE_12, "(MAX)" );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_12, "(%02d:%02d)", pClover->GetRemainChargeMinute(), pClover->GetRemainChargeSecond() );
		}
	}
	kPrinter.PrintFullText( iXPos + 299, iYPos + 61, TAT_RIGHT );
	int iStringWidth = kPrinter.GetFullWidth();
	iXGap = 299 - iStringWidth;
	kPrinter.ClearList();

	int iIconWidth = RenderGiftClover( iXGap - 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( 17, 114, 34 );
	kPrinter.AddTextPiece( FONT_SIZE_12, "선물용" );
	kPrinter.PrintFullText( iXPos + 299 - iStringWidth - iIconWidth - 2, iYPos + 61, TAT_RIGHT );
	kPrinter.ClearList();
}

void MessengerWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	RenderMyCloverAni( m_CloverGoldPosition.x, m_CloverGoldPosition.y );
	RenderGiftCloverAni( m_CloverGreenPosition.x, m_CloverGreenPosition.y );
}

int MessengerWnd::RenderGiftClover( int iXGap )
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !m_pCloverIconGreen1 || !m_pCloverIconGreen2 || !m_pCloverIconGreen3 || !m_pCloverIconGreen4 || !m_pCloverIconGreen5 ||
		!m_pCloverIconEmpty1 || !m_pCloverIconEmpty2 || !m_pCloverIconEmpty3 || !m_pCloverIconEmpty4 || !m_pCloverIconEmpty5 )
		return 0;

	int iGiftCloverCount = g_MyInfo.GetCurGiftCloverCount();
	bool bTemp[5];
	for( int i=0; i<5; ++i )
	{
		if( i < iGiftCloverCount )
			bTemp[i] = true;
		else
			bTemp[i] = false;
	}
	
	bool bSetPos = true;

	int iWidth = m_pCloverIconGreen1->GetWidth();
	int iXGapOffset = iXGap - iWidth;
	if( bTemp[4] )
	{
		m_pCloverIconGreen5->Render( iXPos + iXGapOffset, iYPos + 57 );
		if( bSetPos )
		{
			m_CloverGreenPosition.x = iXPos + iXGapOffset;
			m_CloverGreenPosition.y = iYPos + 57;
			bSetPos = false;
		}
	}
	else
		m_pCloverIconEmpty5->Render( iXPos + iXGapOffset, iYPos + 57 );

	iXGapOffset -= iWidth - 7;
	if( bTemp[3] )
	{
		m_pCloverIconGreen4->Render( iXPos + iXGapOffset, iYPos + 57 );
		if( bSetPos )
		{
			m_CloverGreenPosition.x = iXPos + iXGapOffset;
			m_CloverGreenPosition.y = iYPos + 57;
			bSetPos = false;
		}
	}
	else
		m_pCloverIconEmpty4->Render( iXPos + iXGapOffset, iYPos + 57 );

	iXGapOffset -= iWidth - 7;
	if( bTemp[2] )
	{
		m_pCloverIconGreen3->Render( iXPos + iXGapOffset, iYPos + 57 );
		if( bSetPos )
		{
			m_CloverGreenPosition.x = iXPos + iXGapOffset;
			m_CloverGreenPosition.y = iYPos + 57;
			bSetPos = false;
		}
	}
	else
		m_pCloverIconEmpty3->Render( iXPos + iXGapOffset, iYPos + 57 );

	iXGapOffset -= iWidth - 7;
	if( bTemp[1] )
	{
		m_pCloverIconGreen2->Render( iXPos + iXGapOffset, iYPos + 57 );
		if( bSetPos )
		{
			m_CloverGreenPosition.x = iXPos + iXGapOffset;
			m_CloverGreenPosition.y = iYPos + 57;
			bSetPos = false;
		}
	}
	else
		m_pCloverIconEmpty2->Render( iXPos + iXGapOffset, iYPos + 57 );

	iXGapOffset -= iWidth - 7;
	if( bTemp[0] )
	{
		m_pCloverIconGreen1->Render( iXPos + iXGapOffset, iYPos + 57 );
		if( bSetPos )
		{
			m_CloverGreenPosition.x = iXPos + iXGapOffset;
			m_CloverGreenPosition.y = iYPos + 57;
			bSetPos = false;
		}
	}
	else
		m_pCloverIconEmpty1->Render( iXPos + iXGapOffset, iYPos + 57 );

	int iRet = iWidth * 5 - 28;
	return iRet;
}

void MessengerWnd::SetMyCloverAni( int iIndex, int iXPos, int iYPos )
{
	D3DXVECTOR2 vPos = D3DXVECTOR2( (float)iXPos + 12, (float)iYPos );
	D3DXVECTOR2 vDist = m_CloverGoldPosition - vPos;
	float fDist = D3DXVec2Length( &vDist );

	int iSize = m_MyCloverAniList.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_MyCloverAniList[i].m_iIndex == iIndex )
		{
			if( m_MyCloverAniList[i].m_State == CloverAniInfo::CLOVER_MOVE_END )
			{
				m_MyCloverAniList[i].m_vPos    = vPos;
				m_MyCloverAniList[i].m_vCurPos = vPos;
				m_MyCloverAniList[i].m_fDistance = fDist;
				m_MyCloverAniList[i].m_fCurDistance = 0.0f;
				m_MyCloverAniList[i].m_iAlpha = MAX_ALPHA_RATE;
				m_MyCloverAniList[i].m_fEndDuration = 0.0f;
				m_MyCloverAniList[i].m_State  = CloverAniInfo::CLOVER_MOVING;
				return;
			}
			else
			{
				return;
			}
		}
	}
	CloverAniInfo kInfo;
	kInfo.m_iIndex = iIndex;
	kInfo.m_vPos    = vPos;
	kInfo.m_vCurPos = vPos;
	kInfo.m_fDistance = fDist;
	kInfo.m_fCurDistance = 0.0f;
	kInfo.m_iAlpha = MAX_ALPHA_RATE;
	kInfo.m_fEndDuration = 0.0f;
	kInfo.m_State  = CloverAniInfo::CLOVER_MOVING;
	m_MyCloverAniList.push_back(kInfo);
}

void MessengerWnd::RenderMyCloverAni( int iXPos, int iYPos )
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	
	int iSize = m_MyCloverAniList.size();
	for( int i=0; i<iSize; ++i )
	{
		CloverAniInfo &kAniInfo = m_MyCloverAniList[i];
		if( kAniInfo.m_State == CloverAniInfo::CLOVER_MOVE_END )
			continue;

		if( kAniInfo.m_State == CloverAniInfo::CLOVER_MOVING )
		{
			float fDistX = kAniInfo.m_vPos.x - iXPos;
			float fDistY = kAniInfo.m_vPos.y - iYPos;

			// 진행률
			D3DXVECTOR2 vDist = m_CloverGoldPosition - kAniInfo.m_vCurPos;
			float fDist = D3DXVec2Length( &vDist );
			float fRate = fDist / kAniInfo.m_fDistance;

			D3DXVECTOR2 vPrePos = kAniInfo.m_vCurPos;

			float t = sinf( ((D3DX_PI/2) * fRate) + 0.1f );
			kAniInfo.m_vCurPos.x -= ( fDistX * ( (fTimePerSec*1000) / m_iRecvCloverMovingAniTime ) ) * t;
			kAniInfo.m_vCurPos.y -= ( fDistY * ( (fTimePerSec*1000) / m_iRecvCloverMovingAniTime ) ) * t;

			// 진행거리
			D3DXVECTOR2 vCurDist = kAniInfo.m_vCurPos - vPrePos;
			float fCurDist = D3DXVec2Length( & vCurDist );
			kAniInfo.m_fCurDistance += fCurDist;

			if( kAniInfo.m_fCurDistance > kAniInfo.m_fDistance )
			{
				kAniInfo.m_State = CloverAniInfo::CLOVER_MOVE_END_ANI;
				kAniInfo.m_vCurPos.x = iXPos;
				kAniInfo.m_vCurPos.y = iYPos;
				kAniInfo.m_fEndDuration = 0.0f;
				continue;
			}

			if( m_pCloverAniIconGold )
			{
				m_pCloverAniIconGold->SetAlpha( kAniInfo.m_iAlpha * fRate );
				m_pCloverAniIconGold->Render( kAniInfo.m_vCurPos.x, kAniInfo.m_vCurPos.y );
			}
		}
		else if( kAniInfo.m_State == CloverAniInfo::CLOVER_MOVE_END_ANI )
		{
			float fCheck = FLOAT100 / 8.0f;
			kAniInfo.m_fEndDuration += FLOAT100 * ( (fTimePerSec*1000) / m_iRecvCloverEndAniTime );
			if( kAniInfo.m_fEndDuration >= FLOAT100 )
			{
				kAniInfo.m_State = CloverAniInfo::CLOVER_MOVE_END;
				continue;
			}

			if( m_pCloverAniIconGold )
			{
				m_pCloverAniIconGold->SetAlpha( MAX_ALPHA_RATE );
				m_pCloverAniIconGold->SetColor( TCT_DEFAULT_WHITE );

				int iTemp = (int)( kAniInfo.m_fEndDuration / fCheck );
				if( iTemp%2 == 0 )
					m_pCloverAniIconGold->Render( kAniInfo.m_vCurPos.x, kAniInfo.m_vCurPos.y, UI_RENDER_NORMAL_ADD_COLOR );
				else
					m_pCloverAniIconGold->Render( kAniInfo.m_vCurPos.x, kAniInfo.m_vCurPos.y );
			}
		}
	}
}

void MessengerWnd::SetGiftCloverAni( int iIndex, int iXPos, int iYPos )
{
	D3DXVECTOR2 vPos = D3DXVECTOR2( (float)iXPos, (float)iYPos );
	D3DXVECTOR2 vDist = m_CloverGreenPosition - vPos;
	float fDist = D3DXVec2Length( &vDist );

	int iSize = m_GiftCloverAniList.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_GiftCloverAniList[i].m_iIndex == iIndex )
		{
			if( m_GiftCloverAniList[i].m_State == CloverAniInfo::CLOVER_MOVE_END )
			{
				m_GiftCloverAniList[i].m_vPos    = vPos;
				m_GiftCloverAniList[i].m_vCurPos = m_CloverGreenPosition;
				m_GiftCloverAniList[i].m_fDistance = fDist;
				m_GiftCloverAniList[i].m_fCurDistance = 0.0f;
				m_GiftCloverAniList[i].m_iAlpha = MAX_ALPHA_RATE;
				m_GiftCloverAniList[i].m_fScale = FLOAT1;
				m_GiftCloverAniList[i].m_State  = CloverAniInfo::CLOVER_MOVING;
				return;
			}
			else
			{
				return;
			}
		}
	}
	CloverAniInfo kInfo;
	kInfo.m_iIndex = iIndex;
	kInfo.m_vPos    = vPos;
	kInfo.m_vCurPos = m_CloverGreenPosition;
	kInfo.m_fDistance = fDist;
	kInfo.m_fCurDistance = 0.0f;
	kInfo.m_iAlpha = MAX_ALPHA_RATE;
	kInfo.m_fScale = FLOAT1;
	kInfo.m_State  = CloverAniInfo::CLOVER_MOVING;
	m_GiftCloverAniList.push_back(kInfo);
}

void MessengerWnd::RenderGiftCloverAni( int iXPos, int iYPos )
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	int iSize = m_GiftCloverAniList.size();
	for( int i=0; i<iSize; ++i )
	{
		CloverAniInfo &kAniInfo = m_GiftCloverAniList[i];
		if( kAniInfo.m_State == CloverAniInfo::CLOVER_MOVE_END )
			continue;

		if( kAniInfo.m_State == CloverAniInfo::CLOVER_MOVING )
		{
			float fDistX = kAniInfo.m_vPos.x - iXPos;
			float fDistY = kAniInfo.m_vPos.y - iYPos;

			// 진행률
			D3DXVECTOR2 vDist = kAniInfo.m_vPos - kAniInfo.m_vCurPos;
			float fDist = D3DXVec2Length( &vDist );
			float fRate = fDist / kAniInfo.m_fDistance;

			D3DXVECTOR2 vPrePos = kAniInfo.m_vCurPos;

			float t = sinf( (D3DX_PI/2) - ((D3DX_PI/2) * fRate) + 0.1f );
			kAniInfo.m_vCurPos.x += ( fDistX * ( (fTimePerSec*1000) / m_iSendCloverMovingAniTime ) ) * t;
			kAniInfo.m_vCurPos.y += ( fDistY * ( (fTimePerSec*1000) / m_iSendCloverMovingAniTime ) ) * t;

			// 진행거리
			D3DXVECTOR2 vCurDist = kAniInfo.m_vCurPos - vPrePos;
			float fCurDist = D3DXVec2Length( & vCurDist );
			kAniInfo.m_fCurDistance += fCurDist;

			if( kAniInfo.m_fCurDistance > kAniInfo.m_fDistance )
			{
				kAniInfo.m_State = CloverAniInfo::CLOVER_MOVE_END_ANI;
				continue;
			}

			if( m_pCloverAniIconGreen )
			{
				m_pCloverAniIconGreen->SetScale( FLOAT1, FLOAT1 );
				m_pCloverAniIconGreen->SetAlpha( kAniInfo.m_iAlpha * fRate );
				m_pCloverAniIconGreen->Render( kAniInfo.m_vCurPos.x, kAniInfo.m_vCurPos.y );
			}
		}
		else if( kAniInfo.m_State == CloverAniInfo::CLOVER_MOVE_END_ANI )
		{
			kAniInfo.m_fScale += FLOAT1 * ( (fTimePerSec*1000) / m_iSendCloverEndAniTime );
			if( kAniInfo.m_fScale >= 2.0f )
			{
				kAniInfo.m_State = CloverAniInfo::CLOVER_MOVE_END;
				continue;
			}

			if( m_pCloverAniIconGreen )
			{
				int iAlphaRate = MAX_ALPHA_RATE * ( 2.0f - kAniInfo.m_fScale );
				m_pCloverAniIconGreen->SetAlpha( iAlphaRate );
				m_pCloverAniIconGreen->SetScale( kAniInfo.m_fScale, kAniInfo.m_fScale );
				m_pCloverAniIconGreen->Render( kAniInfo.m_vCurPos.x, kAniInfo.m_vCurPos.y );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ChannelInviteWnd::ChannelInviteWnd()
{
	m_pPreEdit = NULL;
	m_iChannelIdx = 0;
}

ChannelInviteWnd::~ChannelInviteWnd()
{
}

void ChannelInviteWnd::SetChannelIndex( int iIndex )
{
	m_iChannelIdx = iIndex;
	ShowWnd();
}

void ChannelInviteWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}	
}

void ChannelInviteWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
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

void ChannelInviteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;	
	case ID_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			OnChannelInvite();
		}
		break;
	case ID_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_INVITE ), IOBN_BTNUP, 0 );
			}			
		}
		break;
	}
}

void ChannelInviteWnd::OnChannelInvite()
{
	if( g_App.IsMouseBusy() )
		return;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->SetKeyFocus();
		ioHashString szHasID = pEdit->GetText();
		if( szHasID == g_MyInfo.GetPublicID() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}

		if( szHasID.IsEmpty() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}

		if( g_App.IsAdminID( szHasID.c_str() ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			return;
		}

		if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			return;
		}

		ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
		if( pNode )
		{
			if( !pNode->IsChannelUser( szHasID ) )
			{
				TCPNetwork::MouseBusy( true );

				if( !pNode->GetManToManID().IsEmpty() )
				{
					if( pNode->GetManToManID() != szHasID )
					{
						SP2Packet kPacket( CTPK_CHANNEL_INVITE );
						kPacket << m_iChannelIdx << pNode->GetManToManID();
						TCPNetwork::SendToServer( kPacket );		
					}							
				}

				SP2Packet kPacket( CTPK_CHANNEL_INVITE );
				kPacket << m_iChannelIdx << szHasID;
				TCPNetwork::SendToServer( kPacket );
				HideWnd();
			}	
			else	
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		}
	}
}

void ChannelInviteWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 73, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 93, FONT_SIZE_13, STR(3) );
}
//////////////////////////////////////////////////////////////////////////
BlackListInsertWnd::BlackListInsertWnd()
{
	m_pPreEdit = NULL;
}

BlackListInsertWnd::~BlackListInsertWnd()
{
}

void BlackListInsertWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}	
}

void BlackListInsertWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
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

void BlackListInsertWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;	
	case ID_INSERT:
		if( cmd == IOBN_BTNUP )
		{
			OnBlackListInsert();
		}
		break;
	case ID_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_INSERT ), IOBN_BTNUP, 0 );
			}			
		}
		break;
	}
}

void BlackListInsertWnd::OnBlackListInsert()
{
	if( g_App.IsMouseBusy() )
		return;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->SetKeyFocus();
		char szName[MAX_PATH] = "";
		sprintf_e( szName, "%s", pEdit->GetText() );
		if( !g_App.IsRightID( szName ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
		else if( g_App.IsAdminID( szName ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}

		ioHashString szIsID = szName;
		if( g_MyInfo.GetPublicID() == szIsID )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			return;
		}

		if( g_BlackListManager.IsBlackList( szIsID ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			return;
		}

		if( g_BlackListManager.GetNodeSize() >= ioBlackListManager::MAX_BLACK_LIST)
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			return;
		}

		g_BlackListManager.InsertBlackList( szIsID );		
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
	}
}

void BlackListInsertWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 73, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 93, FONT_SIZE_13, STR(3) );
}
//////////////////////////////////////////////////////////////////////////
FriendDeleteWnd::FriendDeleteWnd()
{
	m_pPreEdit = NULL;
}

FriendDeleteWnd::~FriendDeleteWnd()
{
}

void FriendDeleteWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}	
}

void FriendDeleteWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
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

void FriendDeleteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;	
	case ID_DELETE:
		if( cmd == IOBN_BTNUP )
		{
			OnDeleteFriend();
		}
		break;
	case ID_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_DELETE ), IOBN_BTNUP, 0 );
			}			
		}
		break;
	}
}

void FriendDeleteWnd::OnDeleteFriend()
{
	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->SetKeyFocus();
		char szName[MAX_PATH] = "";
		sprintf_e( szName, "%s", pEdit->GetText() );

		if( !g_FriendManager.IsFriend( szName ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}

		SP2Packet kPacket( CTPK_FRIEND_DELETE );
		kPacket << szName;
		TCPNetwork::SendToServer( kPacket );
		g_FriendManager.DeleteFriend( szName );
		HideWnd();

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}	
}

void FriendDeleteWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 73, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 93, FONT_SIZE_13, STR(3) );
}
//////////////////////////////////////////////////////////////////////////
FriendApplicationWnd::FriendApplicationWnd()
{
	m_pPreEdit = NULL;
}

FriendApplicationWnd::~FriendApplicationWnd()
{
}

void FriendApplicationWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}	
}

void FriendApplicationWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
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

void FriendApplicationWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;	
	case ID_APPLICATION:
		if( cmd == IOBN_BTNUP )
		{
			OnApplicationFriend();
		}
		break;
	case ID_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_APPLICATION ), IOBN_BTNUP, 0 );
			}			
		}
		break;
	}
}

void FriendApplicationWnd::OnApplicationFriend()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		if( ioWnd::m_pTopModalWnd != this )
		{
			pEdit->SetKeyFocus();
			return;
		}

		pEdit->SetKeyFocus();
		char szName[MAX_PATH] = "";
		sprintf_e( szName, "%s", pEdit->GetText() );
		if( !g_App.IsRightID( szName ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			if( !pLocal->IsRightNewID( szName ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				return;
			}
		}

		ioHashString szNotMakeString;
		if( g_App.IsNotMakeID( szName, szNotMakeString ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			return;
		}

		ioHashString szIsID = szName;
		if( g_MyInfo.GetPublicID() == szIsID )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			return;
		}

		if( g_FriendManager.IsFriend( szName ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			return;
		}

		if( g_App.IsAdminID( szName ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			return;
		}

		int iSlotSize = g_FriendManager.GetSlotSize();
		if( g_FriendManager.GetNodeSize() >= iSlotSize )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8), iSlotSize );
			return;
		}

		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_FRIEND_APPLICATION );
		kPacket << szName;
		TCPNetwork::SendToServer( kPacket );
		HideWnd();
	}	
}

void FriendApplicationWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.PrintFullText( iXPos + 16, iYPos + 53, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	kPrinter.PrintFullText( iXPos + 16, iYPos + 73, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
	kPrinter.PrintFullText( iXPos + 16, iYPos + 93, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos + 16, iYPos + 113, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + 16, iYPos + 140, TAT_LEFT );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
bool FriendRequestWnd::sg_bRequestIgnore = false;
bool FriendRequestWnd::sg_bFirstView = true;
FriendRequestWnd::FriendRequestWnd()
{
	m_pIconBack		= NULL;
	m_pFriendIcon	= NULL;
	m_pSearchIcon = NULL;
	m_dwHideDelayTime = 0;
}

FriendRequestWnd::~FriendRequestWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pFriendIcon );
	SAFEDELETE( m_pSearchIcon );
}

void FriendRequestWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack")
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "FriendIcon" )
	{
		SAFEDELETE( m_pFriendIcon );
		m_pFriendIcon = pImage;
	}
	else if( szType == "SearchIcon" )
	{
		SAFEDELETE( m_pSearchIcon );
		m_pSearchIcon = pImage;
		if( m_pSearchIcon )
			m_pSearchIcon->SetAlpha( (float)MAX_ALPHA_RATE * 0.3f );
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void FriendRequestWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szRequestText = xElement.GetStringAttribute_e( "RequestText" );
}

void FriendRequestWnd::iwm_show()
{
	if( sg_bFirstView )
	{
		HideChildWnd( ID_REQUEST_IGNORE );

		SetSize( 207, 246 );
		RECT rcRect = { 0, 0, 207, 246 };
		SetDragRect( rcRect );
		if( m_pMultiplyFrm )
			m_pMultiplyFrm->SetSize( 207, 65 );
	}
	else
	{
		ShowChildWnd( ID_REQUEST_IGNORE );

		SetSize( 207, 267 );
		RECT rcRect = { 0, 0, 207, 267 };
		SetDragRect( rcRect );
		if( m_pMultiplyFrm )
			m_pMultiplyFrm->SetSize( 207, 86 );
	}

	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_REQUEST_IGNORE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bRequestIgnore );
}

void FriendRequestWnd::iwm_hide()
{
	m_dwHideDelayTime = FRAMEGETTIME();
	
	if( g_FriendManager.GetFriendRequestSize() == 0 )
	{
		SetFirstView( false );
	}
}

void FriendRequestWnd::OnAgree()
{
	if( g_App.IsMouseBusy() )
		return;

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
		return;
	}

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	DWORD dwTableIndex, dwUserIndex;
	const ioHashString &szTargetID = g_FriendManager.GetFriendRequestUser( dwTableIndex, dwUserIndex );	

	if( g_MyInfo.GetPublicID() == szTargetID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		g_FriendManager.DeleteRequestFriend( szTargetID );
		HideWnd();
		return;
	}

	if( g_FriendManager.IsFriend( szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		g_FriendManager.DeleteRequestFriend( szTargetID );
		HideWnd();
		return;
	}

	int iSlotSize = g_FriendManager.GetSlotSize();
	if( g_FriendManager.GetNodeSize() >= iSlotSize )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), iSlotSize );
		return;
	}

	SP2Packet kPacket( CTPK_FRIEND_COMMAND );
	kPacket << FRIEND_COMMAND_AGREE;
	kPacket << dwTableIndex << dwUserIndex << szTargetID;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void FriendRequestWnd::OnRefuse()
{
	if( g_App.IsMouseBusy() )
		return;

	DWORD dwTableIndex, dwUserIndex;
	const ioHashString &szTargetID = g_FriendManager.GetFriendRequestUser( dwTableIndex, dwUserIndex );	

	SP2Packet kPacket( CTPK_FRIEND_COMMAND );
	kPacket << FRIEND_COMMAND_REFUSE;
	kPacket << dwTableIndex << dwUserIndex << szTargetID;
	TCPNetwork::SendToServer( kPacket );
	
	g_FriendManager.DeleteRequestFriend( szTargetID );
	HideWnd();
}

void FriendRequestWnd::OnBlackList()
{
	if( g_App.IsMouseBusy() )
		return;

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
		return;
	}

	DWORD dwTableIndex, dwUserIndex;
	const ioHashString &szTargetID = g_FriendManager.GetFriendRequestUser( dwTableIndex, dwUserIndex );
	if( g_BlackListManager.IsBlackList( szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	else if( g_App.IsAdminID( szTargetID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	else if( g_BlackListManager.GetNodeSize() >= ioBlackListManager::MAX_BLACK_LIST)
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	else if( g_MyInfo.GetPublicID() == szTargetID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_BlackListManager.InsertBlackList( szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), szTargetID.c_str() );
	}

	SP2Packet kPacket( CTPK_FRIEND_COMMAND );
	kPacket << FRIEND_COMMAND_BLACKLIST;
	kPacket << dwTableIndex << dwUserIndex << szTargetID;
	TCPNetwork::SendToServer( kPacket );

	g_FriendManager.DeleteRequestFriend( szTargetID );
	HideWnd();
}

void FriendRequestWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			OnAgree();
		}
		break;
	case ID_EXIT:
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
		{
			OnRefuse();
		}
		break;
	case ID_BLACKLIST:
		if( cmd == IOBN_BTNUP )
		{
			OnBlackList();
		}
		break;
	case ID_REQUEST_IGNORE:
		if( cmd == IOBN_BTNUP )
		{
			sg_bRequestIgnore = !sg_bRequestIgnore;
			if( sg_bRequestIgnore )
                HideWnd();
		}
		break;
	case ID_FRIEND_INFO:
		if( cmd == IOBN_BTNUP )
		{
			DWORD dwTableIndex, dwUserIndex;
			const ioHashString &szTargetID = g_FriendManager.GetFriendRequestUser( dwTableIndex, dwUserIndex );
		
			g_App.OpenMannerWnd( szTargetID.c_str(), true );
		}
		break;
	}
}

void FriendRequestWnd::OnProcess( float fTimePerSec )
{
	if( sg_bRequestIgnore ) return;
	if( g_MyInfo.IsTutorialUser() ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() ) 
			return;
	}

	if( IsShow() )
	{
		if( g_FriendManager.GetFriendRequestSize() == 0 )
		{
			HideWnd();
			return;
		}
	}
	else
	{
		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 )
			return;

		if( g_FriendManager.GetFriendRequestSize() != 0 )
		{
			if( !g_MyInfo.IsTutorialUser() )
				ShowWnd();
		}
		else
			return;
	}
	ioMovingWnd::OnProcess( fTimePerSec );
}

void FriendRequestWnd::OnRender()
{
	ioWnd::OnRender();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 99, FONT_SIZE_12,  STR(2) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 117, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 30, iYPos + 135, FONT_SIZE_12, STR(4) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 153, FONT_SIZE_12, STR(5) );
}

void FriendRequestWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.55f, 0.55f );
		m_pIconBack->Render( iXPos + 37, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pFriendIcon )
		{
			m_pFriendIcon->SetScale( 0.55f, 0.55f );
			m_pFriendIcon->Render( iXPos + 37, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		if( m_pSearchIcon )
			m_pSearchIcon->Render( iXPos + 173, iYPos + 48 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );

	DWORD dwTableIndex, dwUserIndex;
	const ioHashString &szTargetID = g_FriendManager.GetFriendRequestUser( dwTableIndex, dwUserIndex );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 57, iYPos + 53, FONT_SIZE_13, 133.0f, szTargetID.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 57, iYPos + 71, FONT_SIZE_13,  m_szRequestText.c_str() );
}
//////////////////////////////////////////////////////////////////////////
ioHashString CharRentalRequestWnd::sg_szUserID;
int CharRentalRequestWnd::sg_ClassType = 0;
int CharRentalRequestWnd::sg_PowerUpChaGrade = 0;
DWORD CharRentalRequestWnd::sg_RequestTime = 0;
CharRentalRequestWnd::CharRentalRequestWnd()
{
	m_pIconBack = NULL;
	m_pCharIcon = NULL;
	m_pCharSubIcon = NULL;
	m_nGradeType = 0;
}

CharRentalRequestWnd::~CharRentalRequestWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
}

void CharRentalRequestWnd::OnAgree()
{
	UserInfoNode *pUserInfoNode = g_UserInfoMgr.GetUserInfoNode( g_MyInfo.GetPublicID(), false );
	if( pUserInfoNode == NULL ) return;

	UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
	CharInfoData &rkChar = rkCharData.GetSlotCharToClassType( sg_ClassType );
	if( rkChar.m_CharInfo.m_class_type != sg_ClassType )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	int iCharArray = g_MyInfo.GetClassArray( sg_ClassType );
	if( iCharArray == -1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else if( g_MyInfo.IsCharRental( iCharArray ) == false )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
	else if( g_MyInfo.GetCharRentalMinuteToArray( iCharArray ) != 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
	}
	else
	{
		SP2Packet kPacket( CTPK_USER_CHAR_RENTAL_AGREE );
		kPacket << sg_szUserID;
			
		// Rental Data
		kPacket << g_MyInfo.GetCharIndex( iCharArray ) << g_MyInfo.GetClassLevel( rkChar.m_CharInfo.m_class_type, true );

		int i = 0;
		for(i = 0;i < MAX_INVENTORY;i++)
		{
			const CHARACTER &rkCharInfo = g_MyInfo.GetCharacter( iCharArray );
			if( rkCharInfo.m_extra_item[i] > 0 )
			{
				kPacket << rkChar.m_EquipItemInfo[i].m_item_code << rkChar.m_EquipItemInfo[i].m_item_reinforce
					    << rkChar.m_EquipItemInfo[i].m_item_male_custom << rkChar.m_EquipItemInfo[i].m_item_female_custom;
			}
			else
			{
				kPacket << 0 << 0 << 0 << 0;
			}
		}

		int iTotalMedalCount = (int)rkChar.m_EquipMedalItem.size();
		int iMedalCount = 0;
		int iCustomMedalCount = 0;		
		for(i = 0;i < iTotalMedalCount;i++)
		{
			if( rkChar.m_EquipMedalItem[i].m_iCustomIndex == 0 )
				iMedalCount++;
			else
				iCustomMedalCount++;
		}
		kPacket << iMedalCount;
		for(i = 0;i < iTotalMedalCount;i++)
		{
			if( rkChar.m_EquipMedalItem[i].m_iCustomIndex == 0 )
				kPacket << rkChar.m_EquipMedalItem[i].m_iItemType;
		}
		kPacket << iCustomMedalCount;
		for(i = 0;i < iTotalMedalCount;i++)
		{
			if( rkChar.m_EquipMedalItem[i].m_iCustomIndex != 0 )
			{
				kPacket << rkChar.m_EquipMedalItem[i].m_iItemType;
				kPacket << rkChar.m_EquipMedalItem[i].m_iCustomIndex;
				for( int j = 0; j < 8; j++ )
					kPacket << rkChar.m_EquipMedalItem[i].m_iStat[j];
			}
		}

		ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
		if( pLevel)
		{
			for(i = 0;i < MAX_CHAR_GROWTH;i++)
				kPacket << BYTE(pLevel->GetCharGrowthLevel(sg_ClassType, i, false ) - pLevel->GetInstantLevel());

			for(i = 0;i < MAX_ITEM_GROWTH;i++)
				kPacket << BYTE(pLevel->GetItemGrowthLevel(sg_ClassType, i, false ) - pLevel->GetInstantLevel());
		}
		else
		{
			for(i = 0;i < MAX_CHAR_GROWTH;i++)
				kPacket << (BYTE)(rkChar.m_CharGrowth[i]);

			for(i = 0;i < MAX_ITEM_GROWTH;i++)
				kPacket << (BYTE)(rkChar.m_ItemGrowth[i]);
		}
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}

void CharRentalRequestWnd::iwm_show()
{
	if( sg_ClassType == 0 )
		HideWnd();
	else
	{
		SAFEDELETE( m_pCharIcon );
		SAFEDELETE( m_pCharSubIcon );
		m_pCharIcon = g_MyInfo.GetMySoldierIcon( sg_ClassType );
		m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( sg_ClassType );

		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( sg_ClassType );

		if( m_pCharIcon == NULL )
			HideWnd();
		else
		{
			if( sg_RequestTime == 0 )
				sg_RequestTime = FRAMEGETTIME();
			ioButton *pAgreeBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_AGREE ) );
			if( pAgreeBtn )
			{
				char szTitle[MAX_PATH] = "";
				SafeSprintf( szTitle, sizeof( szTitle ), STR(1), REQUEST_DELAY_SEC );
				pAgreeBtn->SetTitleText( szTitle );
			}
		}
	}
}

void CharRentalRequestWnd::iwm_hide()
{
	sg_ClassType = 0;
	sg_PowerUpChaGrade = 0;
	sg_szUserID.Clear();
	sg_RequestTime = 0;
}

void CharRentalRequestWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			OnAgree();
			HideWnd();
		}
		break;
	}
}

void CharRentalRequestWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

void CharRentalRequestWnd::SetRentalRequest( const ioHashString szUserID, const int iClassType, const int iPowerUpChaGrade )
{
	sg_szUserID			= szUserID;
	sg_ClassType		= iClassType;
	sg_PowerUpChaGrade	= iPowerUpChaGrade;
	sg_RequestTime		= FRAMEGETTIME();
}

void CharRentalRequestWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( sg_ClassType != 0 )
			ShowWnd();
		return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );

	if( sg_RequestTime == 0 )
		return;

	if( FRAMEGETTIME() - sg_RequestTime >= REQUEST_DELAY_SEC * 1000 )
	{
		HideWnd();	
	}
	else
	{
		ioButton *pBtn = (ioButton*)FindChildWnd( ID_AGREE );
		if( pBtn )
		{
			DWORD dwSec = ((float)(FRAMEGETTIME() - sg_RequestTime) / 1000);
			int   iSec  = max( 0, REQUEST_DELAY_SEC - dwSec );	
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iSec );
			pBtn->SetTitleText( szTitle );
		}
	}
}

void CharRentalRequestWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.55f );
		m_pIconBack->Render( iXPos + 33, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 33, iYPos + 70, 0.55f );

		if( m_pCharIcon )
		{
			m_pCharIcon->SetScale( 0.55f );
			m_pCharIcon->Render( iXPos + 33, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		if( m_pCharSubIcon )
		{
			m_pCharSubIcon->SetScale( 0.55f );
			m_pCharSubIcon->Render( iXPos + 33, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), sg_szUserID.c_str() );
	kPrinter.PrintFullTextWidthCut( iXPos + 57, iYPos + 53, TAT_LEFT, 135.0f );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4), g_MyInfo.GetClassName( sg_ClassType, sg_PowerUpChaGrade ) );
	kPrinter.PrintFullTextWidthCut( iXPos + 57, iYPos + 71, TAT_LEFT, 135.0f );
	kPrinter.ClearList();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 99, FONT_SIZE_12,  STR(5), Help::GetCharRentalMinute() );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 117, FONT_SIZE_12, STR(6), Help::GetCharRentalMinute() );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 135, FONT_SIZE_12, STR(7) );
}
//////////////////////////////////////////////////////////////////////////
int CharRentalReturnWnd::sg_ClassType = 0;
int CharRentalReturnWnd::sg_PowerUpChaGrade = 0;
CharRentalReturnWnd::CharRentalReturnWnd()
{
	m_pIconBack = NULL;
	m_pCharIcon = NULL;
	m_pCharSubIcon = NULL;
	m_nGradeType = 0;
}

CharRentalReturnWnd::~CharRentalReturnWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
}

void CharRentalReturnWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

void CharRentalReturnWnd::SetRentalReturn( const int iClassType, int iPowerUpChaGrade )
{
	sg_ClassType = iClassType;
	sg_PowerUpChaGrade = iPowerUpChaGrade;
}

void CharRentalReturnWnd::iwm_show()
{
	if( sg_ClassType == 0 )
		HideWnd();
	else
	{
		SAFEDELETE( m_pCharIcon );
		SAFEDELETE( m_pCharSubIcon );
		m_pCharIcon = g_MyInfo.GetMySoldierIcon( sg_ClassType );
		m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( sg_ClassType );

		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( sg_ClassType );
	}
}

void CharRentalReturnWnd::iwm_hide()
{
	sg_ClassType = 0;
}

void CharRentalReturnWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	}
}

void CharRentalReturnWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( sg_ClassType != 0 )
			ShowWnd();
		return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
}

void CharRentalReturnWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 103, iYPos + 98, UI_RENDER_MULTIPLY );

		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 103, iYPos + 98 );

		if( m_pCharIcon )
			m_pCharIcon->Render( iXPos + 103, iYPos + 98 );
		if( m_pCharSubIcon )
			m_pCharSubIcon->Render( iXPos + 103, iYPos + 98 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13,  STR(1), g_MyInfo.GetClassName( sg_ClassType, sg_PowerUpChaGrade ) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, STR(2) );

}
//////////////////////////////////////////////////////////////////////////
CharRentalAcquireWnd::CharRentalAcquireWnd()
{
	m_pIconBack = NULL;
	m_pCharIcon = NULL;
	m_pCharSubIcon = NULL;
	
	m_iClassType = 0;
	m_nGradeType = 0;
}

CharRentalAcquireWnd::~CharRentalAcquireWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
}

void CharRentalAcquireWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

void CharRentalAcquireWnd::iwm_show()
{
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon = g_MyInfo.GetMySoldierIcon( m_iClassType );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );
	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType );
}

void CharRentalAcquireWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	}
}

void CharRentalAcquireWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 103, iYPos + 98, UI_RENDER_MULTIPLY );

		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 103, iYPos + 98 );

		if( m_pCharIcon )
			m_pCharIcon->Render( iXPos + 103, iYPos + 98 );
		if( m_pCharSubIcon )
			m_pCharSubIcon->Render( iXPos + 103, iYPos + 98 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13,  STR(1), g_MyInfo.GetClassName( m_iClassType ) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13,  STR(2), Help::GetCharRentalMinute() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 189, FONT_SIZE_13, STR(3) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 207, FONT_SIZE_13, STR(4) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 225, FONT_SIZE_13, STR(5) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 243, FONT_SIZE_13, STR(6) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 261, FONT_SIZE_13, STR(7), m_szOwnerName.c_str() );
}

void CharRentalAcquireWnd::SetRentalAcquire( const ioHashString &rkOwnerName, int iClassType )
{
	m_szOwnerName = rkOwnerName;
	m_iClassType  = iClassType;
}