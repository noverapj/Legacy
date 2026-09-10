

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3DEngine/ErrorReport.h"

#include "../TextColorTable.h"
#include "../ioApplication.h"
#include "../Setting.h"
#include "../ioMyInfo.h"
#include "../WndID.h"
#include "../NetworkWrappingFunc.h"
#include "../SlangUtil.h"
#include "../ioGuildInfo.h"
#include "../ioGuildChatMgr.h"
#include "../ChatMessageFunc.h"
#include "../EtcHelpFunc.h"

#include "ChatMacro.h"
#include "MessengerWnd.h"
#include "ioChannelManager.h"
#include "ioSP2GUIManager.h"
#include "ChannelChatWnd.h"
#include "ChatMsgWnd.h"
#include "GuildWnd.h"
#include "MannerValuationWnd.h"

#include <strsafe.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------
LineChatWnd::LineChatWnd()
{
	m_iChannelIdx    = 0;
	m_iChatInfoArray = 0;
	m_bChatOver		 = false;
}

LineChatWnd::~LineChatWnd()
{

}

void LineChatWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL )
		return;

	ChatInfo *pInfo = pNode->GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return;

	if( m_bChatOver && !pInfo->m_szID.IsEmpty() )
		pInfo->m_OverPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
	else
		pInfo->m_Printer.PrintFullText( iXPos, iYPos, TAT_LEFT );
}

void LineChatWnd::SetChatInfo( int iChannelIdx, int iChatInfoArray )
{
	m_iChannelIdx    = iChannelIdx;
	m_iChatInfoArray = iChatInfoArray;
}

DWORD LineChatWnd::GetSentenceIndex()
{
	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL )
		return 0;
	ChatInfo *pInfo = pNode->GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return 0;

	return pInfo->m_Index;
}

void LineChatWnd::SetBigBtnOver( bool bOver )
{
	m_bChatOver = bOver;
}

bool LineChatWnd::IsBigBtnOver()
{
	if( !IsShow() ) return false;
	
	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL )
		return false;
	ChatInfo *pInfo = pNode->GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return false;
	if( pInfo->m_szID.IsEmpty() )
		return false;

	POINT ptCheckPos = g_App.GetMouse()->GetMousePos();
	RECT  rcRect;
	rcRect.left  = GetDerivedPosX();
	rcRect.right = rcRect.left + pInfo->m_Printer.GetFullWidth();
	rcRect.top   = GetDerivedPosY();
	rcRect.bottom= rcRect.top + GetHeight();

	if( ::PtInRect( &rcRect, ptCheckPos ) )
		return true;	
	return false;
}

void LineChatWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( IsBigBtnOver() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse->IsLBtnUp() )
		{
			OpenMannerWnd();
		}
	}
}

void LineChatWnd::OpenMannerWnd()
{
	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL )
		return;
	ChatInfo *pInfo = pNode->GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return;
	if( pInfo->m_szID.IsEmpty() ) 
		return;

	ChannelChatWnd *pParent = dynamic_cast<ChannelChatWnd*> ( m_pParent );
	if( !pParent ) return;

	MannerValuationParentWnd *pMannerWnd = dynamic_cast<MannerValuationParentWnd*>(g_GUIMgr.FindWnd( MANNER_VALUATION_WND ));
	if( !pMannerWnd ) return;

	if( !pMannerWnd->IsOpenState( pInfo->m_szID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	pMannerWnd->SetMannerValuationInfo( pInfo->m_szID , true, ioMannerTrialChatManager::TT_CHANNEL_CHAT, pParent->GetChannelIndex() );
	pMannerWnd->ShowWnd();
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
ChannelChatWnd::ChannelChatWnd() : m_pPreEdit( NULL ) , m_bSettingScrollPos( false )
{
	m_iCurPage    = 0;
	m_iChannelIdx = -1;

	m_iChannelUserFrame = 0;
	m_dwChannelUserTimer= 0;
	memset( m_szChannelUser, 0, sizeof( m_szChannelUser ) );
}

ChannelChatWnd::~ChannelChatWnd()
{
}

void ChannelChatWnd::iwm_create()
{	
}

void ChannelChatWnd::iwm_show()
{	
	SetScrollSetting( true );

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_CHAT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );	
	}		

	m_iChannelUserFrame = 0;
	m_dwChannelUserTimer= FRAMEGETTIME();

	m_bSettingScrollPos = false;
}

void ChannelChatWnd::iwm_hide()
{	
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_CHAT );
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

	for(int i = ID_LINE_CHAT_WND_1;i < ID_LINE_CHAT_WND_1 + MAX_LIST_SIZE;i++)
	{
		HideChildWnd( i );
	}

	m_bSettingScrollPos = false;
}

void ChannelChatWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	m_iCurPage = curPos;
}

void ChannelChatWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );
		}
	}
}

void ChannelChatWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );
	SetChatFocus();
}

void ChannelChatWnd::SetChannelIndex( int iIndex )
{
	m_iChannelIdx = iIndex;
	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL )
	{
		return;
	}

	float fScale = FONT_SIZE_12;
	float fWidhtSize = 208.0f;

	char szChannelUser[2048] = "";
	memset( szChannelUser, 0, sizeof( szChannelUser ) );
	pNode->GetUserName( szChannelUser, 2000 );
	Help::StringCutFun( fScale, fWidhtSize, TS_NORMAL, m_szChannelUser, sizeof(m_szChannelUser), szChannelUser );
}

void ChannelChatWnd::SetScrollSetting( bool bSettingScrollPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
		if( pNode )
		{
			int iTotalList = pNode->GetChatInfoSize();
			int iBeforMaxPos = pScroll->GetMaxPos();
			pScroll->SetScrollRange( 0, iTotalList );
			if ( pScroll->GetCurPos() == iBeforMaxPos || bSettingScrollPos )
				pScroll->SetScrollPos( iTotalList );
		}		
	}
}

void ChannelChatWnd::ErrorEdit( char *szError )
{
	g_ChannelManager.AddAnnounce( m_iChannelIdx, szError );	
}

void ChannelChatWnd::SendChat()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_CHAT );
	if( !pEdit )	return;

	if( g_MyInfo.GetBlockType() == BKT_LIMITATION || g_MyInfo.GetBlockType() == BKT_WARNNING )
	{
		SetChildInActive( ID_EDIT_CHAT );

		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( STR(1) );	
		pEdit->KillKeyFocus();
		return;		
	}
	else 
		SetChildActive( ID_EDIT_CHAT );

	char szChat[MAX_PATH] = "";
	sprintf( szChat, "%s", pEdit->GetText() );
	if( strcmp( szChat, "" ) == 0 )	return;
	if( MacroProcess( szChat ) ) return;

	char szChatBuf[MAX_PATH] = "";
	ioHashString szOwnerChat = szChat;
	if( !Setting::IsSlangFilterOther() )
	{
		StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat ) );
		szOwnerChat = szChatBuf;
	}

	g_ChannelManager.OnChat( m_iChannelIdx, g_MyInfo.GetPublicID(), szOwnerChat.c_str() );

	m_bSettingScrollPos = true;
	SP2Packet kPacket( CTPK_CHANNEL_CHAT );
	kPacket << m_iChannelIdx << szChat;
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_CHANNEL_CHAT, g_MyInfo.GetPublicID().c_str(), szChat, MT_NONE, m_iChannelIdx );
}

bool ChannelChatWnd::MacroProcess( const char *szChat )
{
	char szMacro[MAX_EDIT_TEXT+1] = "";
	strcpy( szMacro, szChat );

	ChatMacro cm;
	int key_word   = 0;
	int macro_type = cm.IsMacro( szMacro, key_word );

	switch( macro_type )
	{
	case ChatMacro::MACRO_INVITE_CHANNEL:
		OnInvite( &szMacro[key_word] );
		return true;
	default:
		if( szChat[0] == '/')
		{
			ErrorEdit( STR(1) );
			return true;
		}
	}

	return false;
}

void ChannelChatWnd::OnInvite( char *szString )
{
	char szID[MAX_PATH];
	memset( szID, 0, sizeof( szID ) );

	char cEOF = ' ';
	int  iMacro = 0;
	int  iLen = strlen( szString );
	int  i = 0;
	//닉네임
	for(i = 0;i < iLen;i++,iMacro++)
	{
		szID[i] = szString[iMacro];
		if( szString[iMacro] == cEOF )
		{
			szID[i] = NULL;
			break;
		}
	}
	iMacro++;
	ioHashString szHasID = szID;
	if( szHasID == g_MyInfo.GetPublicID() )
	{
		ErrorEdit( STR(1) );
		return;
	}

	if( szHasID.IsEmpty() )
	{
		ErrorEdit( STR(2) );
		return;
	}

	if( g_App.IsAdminID( szHasID.c_str() ) )
	{
		ErrorEdit( STR(3) );
		return;
	}

	if( g_MyInfo.GetBlockType() == BKT_LIMITATION || g_MyInfo.GetBlockType() == BKT_WARNNING )
	{
		ErrorEdit( STR(4) );
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
		}	
		else	
			ErrorEdit( STR(5) );
	}
}


void ChannelChatWnd::InviteResult( int iResult, ioHashString szInvitedName )
{
	char szResult[MAX_PATH] = "";
	switch( iResult )
	{	
	case CHANNEL_INVITE_NOT_USER:
		SafeSprintf( szResult, sizeof( szResult ), STR(1), szInvitedName.c_str() );		
		break;
	case CHANNEL_INVITE_USER_NOT_CHANNEL:
		SafeSprintf( szResult, sizeof( szResult ), STR(2), szInvitedName.c_str() );		
		break;
	case CHANNEL_INVITE_USER_ALREADY_CHANNEL:
		SafeSprintf( szResult, sizeof( szResult ), STR(3), szInvitedName.c_str() );		
		break;
	}

	ErrorEdit( szResult );
}

bool ChannelChatWnd::IsChatBlock()
{
	ioEdit *pEdit = (ioEdit *)FindChildWnd( ID_EDIT_CHAT );
	if( pEdit == ioEdit::m_pKeyFocusEdit )
		return true;
	return false;
}

void ChannelChatWnd::SetChatFocus()
{
	ioEdit *pEdit = (ioEdit *)FindChildWnd( ID_EDIT_CHAT );
	if( !pEdit ) return;
	if( pEdit == ioEdit::m_pKeyFocusEdit ) return;
	
	pEdit->SetKeyFocus();
}

void ChannelChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_CHAT_SEND:
		if( cmd == IOBN_BTNUP )
		{
			SendChat();
			g_InputBox.SetString("");
			SetChatFocus();
		}
		break;
	case ID_CHANNEL_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			ChannelInviteWnd *pChannelInvite = dynamic_cast<ChannelInviteWnd*>( g_GUIMgr.FindWnd( CHANNEL_INVITE_WND ) );
			if( pChannelInvite )
			{
				pChannelInvite->SetChannelIndex( m_iChannelIdx );
			}
		}
		break;
	case ID_CHANNEL_LEAVE:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_CHANNEL_LEAVE );
			kPacket << m_iChannelIdx;
			TCPNetwork::SendToServer( kPacket );		

			ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
			if( pNode )
			{
				g_ChannelManager.RemoveNode( pNode );
				MessengerWnd *pMessengerWnd = dynamic_cast<MessengerWnd*>( GetParent() );
				if( pMessengerWnd )
					pMessengerWnd->Init();			
			}			
		}
		break;
	case ID_EDIT_CHAT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = (ioEdit*)pWnd;
				if( pEdit )
				{
					pEdit->SetKeyFocus();
					//Send
					iwm_command( FindChildWnd( ID_CHAT_SEND ), IOBN_BTNUP, 0 );					
				}
			}					
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( ioEdit::m_pKeyFocusEdit != NULL && ioEdit::m_pKeyFocusEdit != pWnd )
			{
				m_pPreEdit = ioEdit::m_pKeyFocusEdit;
			}

			ioEdit *pEdit = (ioEdit*)pWnd;
			pEdit->SetKeyFocus();
		}
		break;	
	case ID_LINE_CHAT_WND_1:
	case ID_LINE_CHAT_WND_2:
	case ID_LINE_CHAT_WND_3:
	case ID_LINE_CHAT_WND_4:
	case ID_LINE_CHAT_WND_5:
	case ID_LINE_CHAT_WND_6:
	case ID_LINE_CHAT_WND_7:
	case ID_LINE_CHAT_WND_8:
	case ID_LINE_CHAT_WND_9:
	case ID_LINE_CHAT_WND_10:
		if( cmd == IOBN_BTNUP )
			SetChatFocus();
		break;
	case ID_VERT_SCROLL:
		if( cmd == IOBN_BTNDOWN )
		{
			SetChatFocus();
		}
		break;
	}
}

void ChannelChatWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;	

	if( g_ChannelManager.IsUpdate( m_iChannelIdx ) )
	{
		g_ChannelManager.ClearUpdate( m_iChannelIdx );
		SetScrollSetting( m_bSettingScrollPos );
		m_bSettingScrollPos = false;
	}

	ProcessChannelUser();
	ProcessChatInfo();
}

void ChannelChatWnd::ProcessChannelUser()
{
	if( FRAMEGETTIME() - m_dwChannelUserTimer < 300 )
		return;

	m_dwChannelUserTimer = FRAMEGETTIME();
	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL ) return;

	float fScale = FONT_SIZE_12;
	float fWidhtSize = 208.0f;

	char szChannelUser[2048] = "";
	memset( szChannelUser, 0, sizeof( szChannelUser ) );
	pNode->GetUserName( szChannelUser, 2000 );

	if( g_FontMgr.GetTextWidth( szChannelUser, TS_NORMAL, fScale ) < fWidhtSize )
	{
		memset( m_szChannelUser, 0, sizeof( m_szChannelUser ) );
		memcpy( m_szChannelUser, szChannelUser, strlen( szChannelUser ) );
		return;
	}
	
	int iMaxLen = strlen( szChannelUser );
	char szDoubleUser[4096] = "";
	sprintf( szDoubleUser, "%s/ %s", szChannelUser, szChannelUser );

	if( m_iChannelUserFrame >= iMaxLen )
		m_iChannelUserFrame = 0;
	
#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
	if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szDoubleUser[m_iChannelUserFrame] ) )
#else
	if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szDoubleUser[m_iChannelUserFrame] ) )
#endif

#else
	if( IsDBCSLeadByte( (BYTE)szDoubleUser[m_iChannelUserFrame] ) )
#endif

		m_iChannelUserFrame+=2;
	else 
		m_iChannelUserFrame+=1;

	Help::StringCutFun( fScale, fWidhtSize, TS_NORMAL, m_szChannelUser, sizeof(m_szChannelUser), &szDoubleUser[m_iChannelUserFrame] );
}

void ChannelChatWnd::OnRender()
{

	ioWnd::OnRender();

	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL ) return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( pNode->GetUserSize() == 2 )
		g_FontMgr.PrintText( iXPos + 23, iYPos + 4, FONT_SIZE_12, STR(1), m_szChannelUser );
	else
		g_FontMgr.PrintText( iXPos + 23, iYPos + 4, FONT_SIZE_12, STR(2), pNode->GetUserSize(), m_szChannelUser );
}

void ChannelChatWnd::ProcessChatInfo()
{
	ChannelNode *pNode = g_ChannelManager.GetNodeIndex( m_iChannelIdx );
	if( pNode == NULL ) return;

	int i = 0;
	int iSentenceIndex = -1;
	int iStartArray = m_iCurPage - max( MAX_LIST_SIZE - pNode->GetChatInfoSize(), 0 );

	for(i = ID_LINE_CHAT_WND_1;i < ID_LINE_CHAT_WND_1 + MAX_LIST_SIZE;i++, iStartArray++)
	{
		ChatInfo *pChatInfo = pNode->GetChatInfo( iStartArray );
		LineChatWnd *pLineChatWnd = dynamic_cast<LineChatWnd *>( FindChildWnd( i ));
		if(!pLineChatWnd) continue;
		
		pLineChatWnd->SetChatInfo( m_iChannelIdx, iStartArray);

		if( pChatInfo )
		{
			pLineChatWnd->ShowWnd();
			if( pLineChatWnd->IsBigBtnOver() )
				iSentenceIndex = (int)pLineChatWnd->GetSentenceIndex();
		}
		else
			pLineChatWnd->HideWnd();
	}	

	// Process에서 마우스 오버 처리
	// iwm_command에서 처리한듯한데 버그가 있다. 이유는 Command가 실시간이 아닌데 채팅 스크롤은 실시간으로 처리되므로 발생함.
	for(i = ID_LINE_CHAT_WND_1;i < ID_LINE_CHAT_WND_1 + MAX_LIST_SIZE;i++)
	{
		LineChatWnd *pLineChatWnd = dynamic_cast<LineChatWnd *>( FindChildWnd( i ));
		if( !pLineChatWnd ) continue;

		if( g_App.IsMouseRender() && iSentenceIndex == (int)pLineChatWnd->GetSentenceIndex() )
			pLineChatWnd->SetBigBtnOver( true );
		else 
			pLineChatWnd->SetBigBtnOver( false );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
GuildLineChatWnd::GuildLineChatWnd()
{
	m_iChatInfoArray = 0;
	m_bChatOver		 = false;
}

GuildLineChatWnd::~GuildLineChatWnd()
{

}

void GuildLineChatWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	ChatInfo *pInfo = g_GuildChatMgr.GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return;
	
	if( m_bChatOver && !pInfo->m_szID.IsEmpty() )
		pInfo->m_OverPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
	else
		pInfo->m_Printer.PrintFullText( iXPos, iYPos, TAT_LEFT );
}

void GuildLineChatWnd::SetChatInfo( int iChatInfoArray )
{
	m_iChatInfoArray = iChatInfoArray;
}

DWORD GuildLineChatWnd::GetSentenceIndex()
{
	ChatInfo *pInfo = g_GuildChatMgr.GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return 0;

	return pInfo->m_Index;
}

void GuildLineChatWnd::SetBigBtnOver( bool bOver )
{
	m_bChatOver = bOver;
}

bool GuildLineChatWnd::IsBigBtnOver()
{
	if( !IsShow() ) return false;
	
	ChatInfo *pInfo = g_GuildChatMgr.GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return false;
	if( pInfo->m_szID.IsEmpty() )
		return false;

	POINT ptCheckPos = g_App.GetMouse()->GetMousePos();
	RECT  rcRect;
	rcRect.left  = GetDerivedPosX();
	rcRect.right = rcRect.left + pInfo->m_Printer.GetFullWidth();
	rcRect.top   = GetDerivedPosY();
	rcRect.bottom= rcRect.top + GetHeight();

	if( ::PtInRect( &rcRect, ptCheckPos ) )
		return true;	
	return false;
}

void GuildLineChatWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( IsBigBtnOver() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse->IsLBtnUp() )
		{
			OpenMannerWnd();
		}
	}
}

void GuildLineChatWnd::OpenMannerWnd()
{
	ChatInfo *pInfo =g_GuildChatMgr.GetChatInfo( m_iChatInfoArray );
	if( pInfo == NULL ) 
		return;
	if( pInfo->m_szID.IsEmpty() ) 
		return;

	GuildChatWnd *pParent = dynamic_cast<GuildChatWnd*> ( m_pParent );
	if( !pParent ) return;

	MannerValuationParentWnd *pMannerWnd = dynamic_cast<MannerValuationParentWnd*>(g_GUIMgr.FindWnd( MANNER_VALUATION_WND ));
	if( !pMannerWnd ) return;

	if( !pMannerWnd->IsOpenState( pInfo->m_szID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	pMannerWnd->SetMannerValuationInfo( pInfo->m_szID , true, ioMannerTrialChatManager::TT_GUILD_CHAT );
	pMannerWnd->ShowWnd();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
GuildChatWnd::GuildChatWnd() : m_pPreEdit( NULL ) , m_bSettingScrollPos( false )
{
	m_iCurPage = 0;

	m_iGuildUserFrame = 0;
	m_dwGuildUserTimer= 0;
	memset( m_szGuildUser, 0, sizeof( m_szGuildUser ) );
}

GuildChatWnd::~GuildChatWnd()
{
}

void GuildChatWnd::iwm_create()
{	
}

void GuildChatWnd::iwm_show()
{	
	SetScrollSetting( true );

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_CHAT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");	
	}		

	m_iGuildUserFrame = 0;
	m_dwGuildUserTimer= FRAMEGETTIME();

	float fScale = FONT_SIZE_12;
	float fWidhtSize = 208.0f;

	char szGuildUser[2048] = "";
	memset( szGuildUser, 0, sizeof( szGuildUser ) );
	g_GuildChatMgr.GetUserName( szGuildUser, 2000 );
	Help::StringCutFun( fScale, fWidhtSize, TS_NORMAL, m_szGuildUser, sizeof(m_szGuildUser), szGuildUser );

	m_bSettingScrollPos = false;
}

void GuildChatWnd::iwm_hide()
{	
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_CHAT );
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

	for(int i = ID_LINE_CHAT_WND_1;i < ID_LINE_CHAT_WND_1 + MAX_LIST_SIZE;i++)
	{
		HideChildWnd( i );
	}

	m_bSettingScrollPos = false;
}

void GuildChatWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	m_iCurPage = curPos;
}

void GuildChatWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );
		}
	}
}

void GuildChatWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );
	SetChatFocus();
}

void GuildChatWnd::SetScrollSetting( bool bSettingScrollPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iTotalList = g_GuildChatMgr.GetChatInfoSize();
		int iBeforMaxPos = pScroll->GetMaxPos();
		pScroll->SetScrollRange( 0, iTotalList );
		if ( pScroll->GetCurPos() == iBeforMaxPos || bSettingScrollPos )
			pScroll->SetScrollPos( iTotalList );
	}
}

void GuildChatWnd::SendChat()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_CHAT );
	if( pEdit )
	{
		if( g_MyInfo.GetBlockType() == BKT_LIMITATION || g_MyInfo.GetBlockType() == BKT_WARNNING )
		{
			SetChildInActive( ID_EDIT_CHAT );

			pEdit->ClearString();
			pEdit->SetKeyFocus();
			g_InputBox.SetString( STR(1) );	
			pEdit->KillKeyFocus();
			return;		
		}
		else 
			SetChildActive( ID_EDIT_CHAT );

		char szChat[MAX_PATH] = "";
		sprintf( szChat, "%s", pEdit->GetText() );
		if( strcmp( szChat, "" ) == 0 )
			return;

		char szChatBuf[MAX_PATH] = "";
		ioHashString szOwnerChat = szChat;
		if( !Setting::IsSlangFilterOther() )
		{
			StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat ) );
			szOwnerChat = szChatBuf;
		}

		g_GuildChatMgr.OnChat( g_MyInfo.GetPublicID(), szOwnerChat.c_str() );
		if( g_GuildInfo.IsGuild() )
		{
			m_bSettingScrollPos = true;
			if ( g_GuildChatMgr.GetUserSize() > 1 )
			{
				SP2Packet kPacket( CTPK_GUILD_CHAT );
				kPacket << g_GuildInfo.GetGuildIndex() << szChat;
				TCPNetwork::SendToServer( kPacket );

				g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_GUILD_CHAT, g_MyInfo.GetPublicID().c_str(), szChat, MT_NONE, g_GuildInfo.GetGuildIndex() );
			}
		}
	}	
}

bool GuildChatWnd::IsChatBlock()
{
	ioEdit *pEdit = (ioEdit *)FindChildWnd( ID_EDIT_CHAT );
	if( pEdit == ioEdit::m_pKeyFocusEdit )
		return true;
	return false;
}

void GuildChatWnd::SetChatFocus()
{
	ioEdit *pEdit = (ioEdit *)FindChildWnd( ID_EDIT_CHAT );
	if( !pEdit ) return;
	if( pEdit == ioEdit::m_pKeyFocusEdit ) return;

	pEdit->SetKeyFocus();
}

void GuildChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_CHAT_SEND:
		if( cmd == IOBN_BTNUP )
		{
			SendChat();
			g_InputBox.SetString("");
			SetChatFocus();
		}
		break;
	case ID_GUILD_INFO:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.GetGuildIndex() != 0 )
			{
				GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
				if( pGuildMainWnd )
					pGuildMainWnd->SetGuildIndex( g_GuildInfo.GetGuildIndex() );
			}
		}
		break;
	case ID_GUILD_HOME_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GuildInfo.GetGuildIndex() != 0 )
			{
				g_App.OpenGuildHomePage( g_GuildInfo.GetGuildIndex() );
			}
		}
		break;
	case ID_EDIT_CHAT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = (ioEdit*)pWnd;
				if( pEdit )
				{
					pEdit->SetKeyFocus();
					//Send
					iwm_command( FindChildWnd( ID_CHAT_SEND ), IOBN_BTNUP, 0 );					
				}
			}					
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( ioEdit::m_pKeyFocusEdit != NULL && ioEdit::m_pKeyFocusEdit != pWnd )
			{
				m_pPreEdit = ioEdit::m_pKeyFocusEdit;
			}

			ioEdit *pEdit = (ioEdit*)pWnd;
			pEdit->SetKeyFocus();
		}
		break;	
	case ID_LINE_CHAT_WND_1:
	case ID_LINE_CHAT_WND_2:
	case ID_LINE_CHAT_WND_3:
	case ID_LINE_CHAT_WND_4:
	case ID_LINE_CHAT_WND_5:
	case ID_LINE_CHAT_WND_6:
	case ID_LINE_CHAT_WND_7:
	case ID_LINE_CHAT_WND_8:
	case ID_LINE_CHAT_WND_9:
	case ID_LINE_CHAT_WND_10:
		if( cmd == IOBN_BTNUP )
			SetChatFocus();
		break;
	case ID_VERT_SCROLL:
		if( cmd == IOBN_BTNDOWN )
		{
			SetChatFocus();
		}
		break;
	}
}

void GuildChatWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;	

	if( g_GuildChatMgr.IsUpdate() )
	{
		g_GuildChatMgr.ClearUpdate();
		SetScrollSetting( m_bSettingScrollPos );
		m_bSettingScrollPos = false;
	}

	ProcessGuildUser();
	ProcessChatInfo();

	if( !g_GuildInfo.IsGuild() )    //길드 탈퇴되면 메신저 닫음
		g_GUIMgr.HideWnd( MESSENGER_WND );
}

void GuildChatWnd::ProcessGuildUser()
{
	if( FRAMEGETTIME() - m_dwGuildUserTimer < 300 )
		return;

	m_dwGuildUserTimer = FRAMEGETTIME();

	float fScale = FONT_SIZE_12;
	float fWidhtSize = 208.0f;

	char szGuildUser[2048] = "";
	memset( szGuildUser, 0, sizeof( szGuildUser ) );
	g_GuildChatMgr.GetUserName( szGuildUser, 2000 );

	if( g_FontMgr.GetTextWidth( szGuildUser, TS_NORMAL, fScale ) < fWidhtSize )
	{
		memset( m_szGuildUser, 0, sizeof( m_szGuildUser ) );
		memcpy( m_szGuildUser, szGuildUser, strlen( szGuildUser ) );
		return;
	}

	int iMaxLen = strlen( szGuildUser );
	char szDoubleUser[4096] = "";
	sprintf( szDoubleUser, "%s/ %s", szGuildUser, szGuildUser );

	if( m_iGuildUserFrame >= iMaxLen )
		m_iGuildUserFrame = 0;

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
	if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szDoubleUser[m_iGuildUserFrame] ) )	
#else
	if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szDoubleUser[m_iGuildUserFrame] ) )
#endif

#else
	if( IsDBCSLeadByte( (BYTE)szDoubleUser[m_iGuildUserFrame] ) )
#endif

		m_iGuildUserFrame+=2;
	else 
		m_iGuildUserFrame+=1;

	Help::StringCutFun( fScale, fWidhtSize, TS_NORMAL, m_szGuildUser, sizeof(m_szGuildUser), &szDoubleUser[m_iGuildUserFrame] );
}

void GuildChatWnd::OnRender()
{
	ioWnd::OnRender();	

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 4, FONT_SIZE_12, STR(1), g_GuildChatMgr.GetUserSize(), m_szGuildUser );
}

void GuildChatWnd::ProcessChatInfo()
{
	int iSentenceIndex = -1;
	int iStartArray = m_iCurPage - max( MAX_LIST_SIZE - g_GuildChatMgr.GetChatInfoSize(), 0 );
	int i = 0;
	for(i = ID_LINE_CHAT_WND_1;i < ID_LINE_CHAT_WND_1 + MAX_LIST_SIZE;i++, iStartArray++)
	{
		ChatInfo *pChatInfo = g_GuildChatMgr.GetChatInfo( iStartArray );
		GuildLineChatWnd *pLineChatWnd = dynamic_cast<GuildLineChatWnd *>( FindChildWnd( i ));
		if(!pLineChatWnd) continue;

		pLineChatWnd->SetChatInfo( iStartArray);
		if( pChatInfo )
		{
			pLineChatWnd->ShowWnd();
			if( pLineChatWnd->IsBigBtnOver() )
				iSentenceIndex = (int)pLineChatWnd->GetSentenceIndex();
		}
		else
			pLineChatWnd->HideWnd();
	}	

	// Process에서 마우스 오버 처리
	// iwm_command에서 처리한듯한데 버그가 있다. 이유는 Command가 실시간이 아닌데 채팅 스크롤은 실시간으로 처리되므로 발생함.
	for(i = ID_LINE_CHAT_WND_1;i < ID_LINE_CHAT_WND_1 + MAX_LIST_SIZE;i++)
	{
		GuildLineChatWnd *pLineChatWnd = dynamic_cast<GuildLineChatWnd *>( FindChildWnd( i ));
		if( !pLineChatWnd ) continue;

		if( g_App.IsMouseRender() && iSentenceIndex == (int)pLineChatWnd->GetSentenceIndex() )
			pLineChatWnd->SetBigBtnOver( true );
		else 
			pLineChatWnd->SetBigBtnOver( false );
	}
}