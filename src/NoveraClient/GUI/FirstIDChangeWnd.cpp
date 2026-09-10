#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"

#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "../NetworkWrappingFunc.h"
#include "../ioComplexStringPrinter.h"
#include "../TextColorTable.h"
#include "../ioEtcItemManager.h"
#include "../SlangUtil.h"
#include "../Setting.h"
#include "../copyfolder.h"
#include "../WndID.h"
#include "../ioQuestManager.h"

#include "ioSP2GUIManager.h"
#include "ioMessageBox.h"
#include ".\firstidchangewnd.h"
#include <strsafe.h>
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalParent.h"

FirstIDChangeWnd::FirstIDChangeWnd(void)
{
	m_pIcon       = NULL;
	m_szStartEditText = STR(1);//"닉네임을 입력하세요.";
	m_szErrorEditText = STR(2);//"사용할 수 없는 닉네임입니다.";
}

FirstIDChangeWnd::~FirstIDChangeWnd(void)
{
	SAFEDELETE( m_pIcon );
}

void FirstIDChangeWnd::iwm_command( ioWnd *pWnd , int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(1) );
		}
		break;
	case ID_NEWID_EDIT:
		if( cmd == IOBN_BTNDOWN )
		{
			ioEdit *pEdit = dynamic_cast<ioEdit*> ( pWnd );
			if( pEdit )
			{
				if( ioEdit::m_pKeyFocusEdit != pEdit )
					m_pPreEdit = ioEdit::m_pKeyFocusEdit;

				if( strncmp( pEdit->GetText(), m_szStartEditText.c_str(), m_szStartEditText.Length() ) == 0 || 
					strncmp( pEdit->GetText(), m_szErrorEditText.c_str(), m_szErrorEditText.Length() ) == 0 )
				{
					pEdit->ClearString();
					pEdit->SetKeyFocus();
					g_InputBox.SetString( "" );	
				}
			}
		}
		break;
	case ID_CHANGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			CheckNewID();
		}
		break;
	case MESSAGE_BOX:   // g_GUIMgr.SetMsgBox()
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				g_QuestMgr.ProcessChangeStage();
				SP2Packet kPacket( CTPK_LOGOUT );
				TCPNetwork::SendToServer( kPacket );

				LOG.PrintTimeAndLog(0, "ID_CHANGE_TO_EXIT" );
			}
		}
		break;
	case MESSAGE_BOX3:  // g_GUIMgr.SetPrevMsgListBoxWithTitle();
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendNewID();
			}
		}
		break;
	}
}

void FirstIDChangeWnd::iwm_show()
{
	m_szNewID.Clear();
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_NEWID_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( m_szStartEditText.c_str() );
		pEdit->KillKeyFocus();
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsFirstIDControlAltCtrl() )
		g_App.DisableAltCtrl();
}

void FirstIDChangeWnd::iwm_hide()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_NEWID_EDIT ) );
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
			{
				ioEdit *pPreEdit = dynamic_cast<ioEdit*> ( m_pPreEdit );
				if( pPreEdit )
					pPreEdit->SetKeyFocus();
			}
		}
	}	
	m_pPreEdit = NULL;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsFirstIDControlAltCtrl() )
		g_App.EnableAltCtrl();
}

void FirstIDChangeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon")
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
		if( m_pIcon )
			m_pIcon->SetScale( CHANGE_WND_ICON_SCALE );
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}
void FirstIDChangeWnd::CheckNewID()
{
	if( g_App.IsMouseBusy() )
		return;

	char szTempPublicID[ID_NUM_PLUS_ONE]="";
	StringCbPrintf( szTempPublicID , sizeof( szTempPublicID ), "%s#", g_MyInfo.GetPrivateID().c_str() );
	if( g_MyInfo.GetPublicID() != szTempPublicID )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( g_MyInfo.GetUserState() != 0 )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_NEWID_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "FirstIDChangeWnd::CheckAndSend | Edit is NULL." );
		ErrorEdit( STR(3) );
		return;
	}

	m_szNewID = pEdit->GetText();
	if( m_szNewID.IsEmpty() )
	{
		ErrorEdit( m_szStartEditText.c_str() );
		return;
	}

	if( m_szNewID == m_szStartEditText )
	{
		return;
	}

	if( !IsRightFirstID( m_szNewID.c_str() ) )
	{
		ErrorEdit( m_szErrorEditText.c_str() );
		return;
	}

	ioHashString szNotMakeString;
	if( g_App.IsNotMakeID( m_szNewID.c_str(), szNotMakeString ) )
	{
		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "%s(%s)", m_szErrorEditText.c_str(), szNotMakeString.c_str() );
		ErrorEdit( szError );
		return;
	}

	char szSlangName[MAX_PATH]="";
	StringCbPrintf( szSlangName, sizeof( szSlangName ), g_SlangUtil.ConvertString( m_szNewID.c_str() ) );
	if( m_szNewID != szSlangName )
	{
		ErrorEdit( m_szErrorEditText.c_str() );
		return;
	}

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13, m_szNewID.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );
	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(5), STR(6), STR(7), STR(8) );
}

void FirstIDChangeWnd::ErrorEdit( const char *szError )
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_NEWID_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( szError );
		pEdit->KillKeyFocus();		
	}	
}

void FirstIDChangeWnd::SendNewID()
{
	enum { DELAY_SEND_GAP_TIME = 3000, };
	if( m_dwSendTime != 0 )
	{
		if( DELAY_SEND_GAP_TIME  > ( FRAMEGETTIME() - m_dwSendTime )  )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
	}

	SP2Packet kPacket( CTPK_FIRST_CHANGE_ID );
	kPacket << m_szNewID;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	m_dwSendTime = FRAMEGETTIME();
}

bool FirstIDChangeWnd::IsRightFirstID( const char *szID )
{
	enum { MIN_SIZE = 4, };

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
		return false;

	int size = strlen(szID);
	if ((size > pLocal->GetFirstIDMaxSize())||(size < MIN_SIZE))
		return false;
	for (int i=0; i<size; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) )
		{
			if( pLocal->IsCheckKorean() )
			{
				// 한글의 경우 깨진 경우
				if (i < size-1)
				{
					// 한글 깨진 경우
					if ((byte)szID[i]==0xa4&&(byte)szID[i+1] >= 0xa1 && (byte)szID[i+1] <= 0xd3)
					{
						return false;
					}
					if ((byte)szID[i]>=0xb0 && (byte)szID[i]<=0xc8 && (byte)szID[i+1] >= 0xa1 && (byte)szID[i+1] <= 0xfe)
					{
						i++;
						continue;
					}
				}
			}
			else if( pLocal->IsFirstIDCheckPass() )
			{
				i++;
				continue;
			}
			return false;
		}
	}
	return true;
}

void FirstIDChangeWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	enum 
	{
		LINE1_X_OFFSET = 78,
		LINE1_Y_OFFSET = 94,
		LINE2_X_OFFSET = 78,
		LINE2_Y_OFFSET = 114,
		ICON_X_OFFSET  = 47,
		ICON_Y_OFFSET  = 113,
	};

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	g_FontMgr.PrintText( iXPos + LINE1_X_OFFSET, iYPos + LINE1_Y_OFFSET, FONT_SIZE_15, STR(1) );
	g_FontMgr.PrintText( iXPos + LINE2_X_OFFSET, iYPos + LINE2_Y_OFFSET, FONT_SIZE_15, STR(2) );

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

}