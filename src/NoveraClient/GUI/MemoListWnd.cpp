

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"

#include "../ioApplication.h"
#include "../TextColorTable.h"
#include "../ioMyInfo.h"
#include "../WndID.h"
#include "../NetworkWrappingFunc.h"
#include "../ioGuildInfo.h"
#include "../SlangUtil.h"
#include "../EtcHelpFunc.h"

#include "ioFriendManager.h"
#include "ioMemoManager.h"
#include "ioSP2GUIManager.h"
#include "MannerValuationWnd.h"
#include "MessengerWnd.h"
#include "MemoListWnd.h"
#include "../io3DEngine/Safesprintf.h"

MemoInfoListWnd::MemoInfoListWnd()
{
	m_pBottomFrm	= NULL;
	m_pGreenstate	= NULL;
	m_pRedState		= NULL;
	m_pIconBack		= NULL;
	m_pRecvIcon		= NULL;
	m_pSendIcon		= NULL;

	m_bOnline  = false;
	m_bViewMemo= false;
	m_dwOnlineTimer = 0;
	m_bFromMemo = false;
	m_dwMemoInfoArray = 0;
}

MemoInfoListWnd::~MemoInfoListWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pGreenstate );
	SAFEDELETE( m_pRedState );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pRecvIcon );
	SAFEDELETE( m_pSendIcon );
	m_MemoList.clear();
}

void MemoInfoListWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else if( szType == "GreenState" )
	{
		SAFEDELETE( m_pGreenstate );
		m_pGreenstate = pFrame;
	}
	else if( szType == "RedState" )
	{
		SAFEDELETE( m_pRedState );
		m_pRedState = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void MemoInfoListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "RecvIcon" )
	{
		SAFEDELETE( m_pRecvIcon );
		m_pRecvIcon = pImage;
	}
	else if( szType == "SendIcon" )
	{
		SAFEDELETE( m_pSendIcon );
		m_pSendIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MemoInfoListWnd::SplitMemo()
{
	m_MemoList.clear();

	float fWidhtSize = 221.0f;
	char szMemo[MAX_PATH] = "";
	sprintf_e( szMemo, "%s", m_szMemo.c_str() );

	if( g_FontMgr.GetTextWidth( szMemo, TS_NORMAL, FONT_SIZE_12 ) < fWidhtSize )
	{
		m_MemoList.push_back( m_szMemo );
		return;
	}	

	int iLen = strlen( szMemo );
	char szCopy[MAX_PATH], szTemp[MAX_PATH];
	memset( szCopy, 0, sizeof( szCopy ) );
	memset( szTemp, 0, sizeof( szTemp ) );
	int  iTemp = 0;
	for(int i = 0;i < iLen;)
	{
		int iBack = i;
		szTemp[iTemp++]   = szMemo[i++];

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szTemp[iTemp - 1] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szTemp[iTemp - 1] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szTemp[iTemp - 1] ) )
#endif

			szTemp[iTemp++] = szMemo[i++];

		if( g_FontMgr.GetTextWidth( szTemp, TS_NORMAL, FONT_SIZE_12 ) >= fWidhtSize )
		{
			ioHashString vMemo = szCopy;
			m_MemoList.push_back( vMemo );			
			memset( szCopy, 0, sizeof( szCopy ) );
			memset( szTemp, 0, sizeof( szTemp ) );
			iTemp = 0;
			i = iBack;
		}
		else
			memcpy( szCopy, szTemp, sizeof( szTemp ) );
	}

	if( strcmp( szCopy, "" ) != 0 )
	{
		ioHashString vMemo = szCopy;
		m_MemoList.push_back( vMemo );		
	}
}

void MemoInfoListWnd::SetNickNameBtnTitle( const ioHashString &szName, bool bFrom )
{
	ioWnd *pNickName = FindChildWnd( ID_NICK_NAME );
	if( pNickName )
	{
		char szDst[MAX_PATH];
		memset( szDst, 0, sizeof( szDst ) );
		Help::StringCutFun( FONT_SIZE_12, 125.0f, TS_NORMAL, szDst, sizeof( szDst ), m_szUserID.c_str() );				
		char szTitle[MAX_PATH] = "";
		if( !bFrom )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), szDst );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), STR(2), szDst );
		pNickName->SetTitleText( szTitle );
		pNickName->SetSize( g_FontMgr.GetTextWidthCutSize( szTitle, TS_NORMAL, FONT_SIZE_12, 125.0f ) + 15, pNickName->GetHeight() );
	}
}

void MemoInfoListWnd::SetMemoInfo( const ioHashString &szUserID, const ioHashString &szMemo, bool bOnline, bool bViewMemo, bool bFromMemo ,DWORD dwMemoInfoArray )
{
	ClearInfo();

	m_szUserID = szUserID;
	m_szMemo   = szMemo;
	m_bOnline  = bOnline;	
	m_bViewMemo= bViewMemo;
	m_bFromMemo= bFromMemo;
	m_dwMemoInfoArray = dwMemoInfoArray;

	HideChildWnd( ID_NICK_NAME );
	HideChildWnd( ID_FRIEND_INSERT );
	HideChildWnd( ID_ANSWER );
	HideChildWnd( ID_DELETE );

	if( !m_szUserID.IsEmpty() && !m_szMemo.IsEmpty() )
	{
		SetNickNameBtnTitle( m_szUserID, bFromMemo );
		if( !bFromMemo )
		{
			ShowChildWnd( ID_NICK_NAME );
			ShowChildWnd( ID_DELETE );
			SetChildInActive( ID_NICK_NAME );
		}
		else
		{
			if( g_App.IsAdminID( m_szUserID.c_str() ) || g_MyInfo.GetBlockType() == BKT_LIMITATION )
			{
				ShowChildWnd( ID_NICK_NAME );
				ShowChildWnd( ID_ANSWER );
				ShowChildWnd( ID_DELETE );

				SetChildInActive( ID_ANSWER );
				SetChildInActive( ID_FRIEND_INSERT );
			}
			else
			{
				ShowChildWnd( ID_NICK_NAME );
				ShowChildWnd( ID_ANSWER );
				ShowChildWnd( ID_DELETE );
				SetChildActive( ID_FRIEND_INSERT );
			}

			if( !g_FriendManager.IsFriend( m_szUserID ) )
				ShowChildWnd( ID_FRIEND_INSERT );
		}
		g_MemoManager.SendUserLogin( m_szUserID );
		SplitMemo();

		m_dwOnlineTimer = FRAMEGETTIME();
	}	
}

void MemoInfoListWnd::ClearInfo()
{
	m_bOnline  = false;
	m_bViewMemo= false;
	m_bFromMemo= false;
	m_szUserID.Clear();
	m_szMemo.Clear();
	m_MemoList.clear();
}

void MemoInfoListWnd::OnApplicationFriend()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( m_szUserID.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( !g_App.IsRightID( m_szUserID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_MyInfo.GetPublicID() == m_szUserID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_FriendManager.IsFriend( m_szUserID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	if( g_App.IsAdminID( m_szUserID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	int iSlotSize = g_FriendManager.GetSlotSize();
	if( g_FriendManager.GetNodeSize() >= iSlotSize )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7), iSlotSize );
		return;
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_FRIEND_APPLICATION );
	kPacket << m_szUserID;
	TCPNetwork::SendToServer( kPacket );
}

void MemoInfoListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ANSWER:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, IOBN_BTNUP, (DWORD)m_szUserID.c_str() );
		}
		break;
	case ID_DELETE:
		if( cmd == IOBN_BTNUP )
		{
			MemoNode *pMemo = g_MemoManager.GetNode( m_dwMemoInfoArray );
			if( pMemo )
			{
				g_MemoManager.RemoveNode( pMemo );
			}
		}
		break;
	case ID_NICK_NAME:
		if( cmd == IOBN_BTNUP )
		{
			MannerValuationParentWnd *pMannerWnd = dynamic_cast<MannerValuationParentWnd*>(g_GUIMgr.FindWnd( MANNER_VALUATION_WND ));
			if( !pMannerWnd ) return;

			if( !pMannerWnd->IsOpenState( m_szUserID ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
			pMannerWnd->SetMannerValuationInfo( m_szUserID , m_bOnline, ioMannerTrialChatManager::TT_MEMO , m_dwMemoInfoArray );
			pMannerWnd->ShowWnd();
		}
		break;
	case ID_FRIEND_INSERT:
		if( cmd == IOBN_BTNUP )
		{
			OnApplicationFriend();
		}
		break;
	}
}

void MemoInfoListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwOnlineTimer > 1000 )
	{
		m_dwOnlineTimer = FRAMEGETTIME();
		m_bOnline       = g_MemoManager.IsLogIn( m_szUserID );
	}
}

void MemoInfoListWnd::OnRender() 
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_szUserID.IsEmpty() || m_szMemo.IsEmpty() )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 144, iYPos + 34, FONT_SIZE_12, STR(1) );
	}
	else
	{
		if( m_pBottomFrm )
			m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.60f );
			m_pIconBack->Render( iXPos + 31, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		
		if( !m_bFromMemo )
		{
			if( m_pSendIcon )
			{
				m_pSendIcon->SetScale( 0.60f );
				m_pSendIcon->Render( iXPos + 31, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			if( m_pRedState )
				m_pRedState->Render( iXPos, iYPos );

			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 31, iYPos + 40, FONT_SIZE_11, STR(2) );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		}
		else 
		{
			if( m_pRecvIcon )
			{
				m_pRecvIcon->SetScale( 0.60f );
				m_pRecvIcon->Render( iXPos + 31, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
			}

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			if( !g_FriendManager.IsFriend( m_szUserID ) )
			{
				g_FontMgr.PrintText( iXPos + 212, iYPos + 63, FONT_SIZE_12, "|" );
			}
			g_FontMgr.PrintText( iXPos + 248, iYPos + 63, FONT_SIZE_12, "|" );

			// 신규 쪽지
			if( !m_bViewMemo )
			{
				if( m_pGreenstate )
					m_pGreenstate->Render( iXPos, iYPos );

				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetTextColor( 255, 255, 255 );
				g_FontMgr.PrintText( iXPos + 31, iYPos + 40, FONT_SIZE_11, "NEW" );

				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			}
			else
			{
				if( m_pRedState )
					m_pRedState->Render( iXPos, iYPos );

				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetTextColor( 255, 255, 255 );
				g_FontMgr.PrintText( iXPos + 31, iYPos + 40, FONT_SIZE_11, STR(3) );

				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			}			
		}		

		g_FontMgr.SetAlignType( TAT_LEFT );
		if( m_MemoList.size() == 1 )    //한줄
		{
			g_FontMgr.PrintText( iXPos + 60, iYPos + 23, FONT_SIZE_12, m_MemoList[0].c_str() );
		}
		else
		{
			int iSize = min( 2, m_MemoList.size() );
			for(int i = 0;i < iSize;i++)
				g_FontMgr.PrintText( iXPos + 60, iYPos + 13 + (i*20), FONT_SIZE_12, m_MemoList[i].c_str() );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
MemoListWnd::MemoListWnd() : m_pPreEdit( NULL )
{
	m_iCurPage = m_iMaxPage = 0;	
}

MemoListWnd::~MemoListWnd()
{
}

void MemoListWnd::iwm_create()
{	
}

void MemoListWnd::iwm_show()
{	
	UpdateSetPage( 0 );

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_ID );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
		pEdit->KillKeyFocus();		
	}	

	ioHashString szMemoDefault = "";
	if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		szMemoDefault = STR(3);
		SetChildInActive( ID_MEMO_SEND );
	}
	else
	{
		SetChildActive( ID_MEMO_SEND );
	}

	pEdit = (ioEdit*)FindChildWnd( ID_EDIT_MEMO );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( szMemoDefault.c_str() );
		pEdit->KillKeyFocus();		
	}
}

void MemoListWnd::iwm_hide()
{	
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_ID );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();		
	}

	pEdit = (ioEdit*)FindChildWnd( ID_EDIT_MEMO );
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

	for(int i = ID_MEMO_LIST_1;i < ID_MEMO_LIST_2+1;i++)
	{
		MemoInfoListWnd *pWnd = dynamic_cast<MemoInfoListWnd*>( FindChildWnd( i ) );
		if( pWnd )
			pWnd->ClearInfo();
	}
}

void MemoListWnd::UpdateSetPage( int iNewPage )
{
	m_iMaxPage = max( 0, (int)g_MemoManager.GetNodeSize() - 1 ) / MAX_LIST_SIZE;
	m_iCurPage = max( 0, iNewPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );
	int iStartArray = m_iCurPage * MAX_LIST_SIZE;
	for(int i = ID_MEMO_LIST_1;i < ID_MEMO_LIST_2+1;i++)
	{
		MemoInfoListWnd *pWnd = dynamic_cast<MemoInfoListWnd*>( FindChildWnd( i ) );
		if( pWnd )
		{
			MemoNode *pNode = g_MemoManager.GetNode( iStartArray++ );
			if( pNode )
			{
				pWnd->ShowWnd();
				if( pWnd->GetUserID() == pNode->GetUserID() &&
					pWnd->GetMemo()   == pNode->GetMemo() )
				{
					pWnd->SetOnline( pNode->IsOnline() );
				}
				else
					pWnd->SetMemoInfo( pNode->GetUserID(), pNode->GetMemo(), pNode->IsOnline(), pNode->IsViewMemo(), pNode->IsFromMemo(), iStartArray - 1 );
				pNode->SetViewMemo( true );
			}
			else
			{
				pWnd->SetMemoInfo( "", "", false, false, false, iStartArray - 1 );
			}
		}
	}
}

void MemoListWnd::SendMemo()
{
	ioEdit *pIDEdit = (ioEdit*)FindChildWnd( ID_EDIT_ID );
	if( pIDEdit )
		pIDEdit->KillKeyFocus();		
	else
		return;

	ioEdit *pMemoEdit = (ioEdit*)FindChildWnd( ID_EDIT_MEMO );
	if( pMemoEdit )
		pMemoEdit->KillKeyFocus();	
	else
		return;

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

	if( g_App.IsMouseBusy() )
		return;

	if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	char szName[MAX_PATH] = "";
	char szMemo[MAX_PATH] = "";
	
	sprintf_e( szName, "%s", pIDEdit->GetText() );
	if(  !g_App.IsRightID( szName )  )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	ioHashString szHasID = szName;
	if( szHasID == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	if( g_App.IsAdminID( szHasID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	
	sprintf_e( szMemo, "%s", pMemoEdit->GetText() );
	if( strcmp( szMemo, "" ) == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	g_MemoManager.SendMemo( szName, szMemo  );
	
	pMemoEdit->ClearString();
	pMemoEdit->SetKeyFocus();
	g_InputBox.SetString( "" );
}

void MemoListWnd::SetUserID( const char *szID )
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_ID );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( szID );
		pEdit->KillKeyFocus();
	}	

	ioHashString szMemoDefault = "";
	if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
		szMemoDefault = STR(1);

	ioEdit *pMemoEdit = (ioEdit*)FindChildWnd( ID_EDIT_MEMO );
	if( pMemoEdit )
	{
		pMemoEdit->ClearString();
		pMemoEdit->SetKeyFocus();
		g_InputBox.SetString( szMemoDefault.c_str() );
	}
}

bool MemoListWnd::IsChatBlock()
{
	if( !IsShow() )
		return false;

	if( ioEdit::m_pKeyFocusEdit == FindChildWnd( ID_EDIT_ID ) )
		return true;
	if( ioEdit::m_pKeyFocusEdit == FindChildWnd( ID_EDIT_MEMO ) )
		return true;

	return false;
}

void MemoListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MEMO_SEND:
		if( cmd == IOBN_BTNUP )
			SendMemo();
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
			UpdateSetPage( m_iCurPage - 1 );
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
			UpdateSetPage( m_iCurPage + 1 );
		break;
	case ID_EDIT_ID:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_MEMO );
				if( pEdit )
				{
					pEdit->SetKeyFocus();
					MemoClearString( pEdit );
				}
			}					
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( ioEdit::m_pKeyFocusEdit != FindChildWnd( ID_EDIT_ID ) && 
				ioEdit::m_pKeyFocusEdit != FindChildWnd( ID_EDIT_MEMO ) )
			{
				m_pPreEdit = ioEdit::m_pKeyFocusEdit;
			}

			ioEdit *pEdit = (ioEdit*)pWnd;
			pEdit->SetKeyFocus();
			MemoIDClearString( pEdit );
		}
		break;
	case ID_EDIT_MEMO:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )				
			{
				ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_ID );
				if( pEdit )
				{
					pEdit->SetKeyFocus();
					MemoIDClearString( pEdit );
				}
			}
			else if( param == 1 )
			{
				iwm_command( FindChildWnd( ID_MEMO_SEND ), IOBN_BTNUP, 0 );
			}
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( ioEdit::m_pKeyFocusEdit != FindChildWnd( ID_EDIT_ID ) && 
				ioEdit::m_pKeyFocusEdit != FindChildWnd( ID_EDIT_MEMO ) )
			{
				m_pPreEdit = ioEdit::m_pKeyFocusEdit;
			}

			ioEdit *pEdit = (ioEdit*)pWnd;
			pEdit->SetKeyFocus();
			MemoClearString( pEdit );
		}
		break;
	case ID_MEMO_LIST_1:
	case ID_MEMO_LIST_2:
		if( cmd == IOBN_BTNUP )
		{
			if( param != 0 && GetParent() )
				GetParent()->iwm_command( this, MessengerWnd::CMD_MEMO, param );
		}
		break;
	}
}

void MemoListWnd::MemoIDClearString( ioWnd *pEdit )
{
	if( pEdit == NULL ) return;

	ioEdit *pIDEdit = (ioEdit*)pEdit;

	if( strcmp( pIDEdit->GetText(), STR(1) ) == 0 ||
		!g_App.IsRightID( pIDEdit->GetText() ) )
	{
		g_InputBox.SetString( "" );
	}
}

void MemoListWnd::MemoClearString( ioWnd *pEdit )
{
	if( pEdit == NULL ) return;

	ioEdit *pMemoEdit = (ioEdit*)pEdit;

	if( strcmp( pMemoEdit->GetText(), STR(1) ) == 0 ||
		strcmp( pMemoEdit->GetText(), STR(2) ) == 0 )
	{
		g_InputBox.SetString( "" );
	}
}

void MemoListWnd::InviteResult( int iResult, ioHashString szInvitedName )
{
	switch( iResult )
	{
	case CHANNEL_INVITE_NOT_USER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case CHANNEL_INVITE_USER_NOT_CHANNEL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case CHANNEL_INVITE_USER_ALREADY_CHANNEL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	}	
}

void MemoListWnd::ChannelCreate( int iResult, ioHashString szManToManName )
{
	switch( iResult )
	{
	case CHANNEL_MY_CHANNEL_FULL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	}
}

void MemoListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;	

	if( g_MemoManager.IsUpdate() )
	{
		g_MemoManager.ClearUpdate();
		UpdateSetPage( m_iCurPage );	
	}
}

void MemoListWnd::OnRender()
{
	ioWnd::OnRender();	

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 19, iYPos + 228, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 19, iYPos + 251, FONT_SIZE_12, STR(2) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(4), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 161, iYPos + 177, TAT_CENTER );	
}