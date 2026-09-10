

#include "StdAfx.h"

#include "../ioMyInfo.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"

#include "../TextColorTable.h"
#include "../ioApplication.h"
#include "../WndID.h"
#include "../NetworkWrappingFunc.h"
#include "../ioQuestManager.h"

#include "ioSP2GUIManager.h"
#include "DisconnectWnd.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIRenderElement.h"


DisconnectWnd::DisconnectWnd()
{
	m_dwShowTime = 0;
	m_iType = TYPE_LOCAL_EXIT;
	m_dwMainIndex = 0;
	m_dwSubIndex  = 0;
}

DisconnectWnd::~DisconnectWnd()
{

}

void DisconnectWnd::SetQuestData( DWORD dwMainIndex, DWORD dwSubIndex )
{
	m_dwMainIndex = dwMainIndex;
	m_dwSubIndex  = dwSubIndex;
}

void DisconnectWnd::iwm_hide()
{
	if(m_iType == TYPE_LOCAL_EXIT  ||
	   m_iType == TYPE_NPROTECT_EXIT || 
	   m_iType == TYPE_SHUT_DOWN_EXIT || 
	   m_iType == TYPE_SELECT_SHUT_DOWN_EXIT )
	{
		LOG.PrintTimeAndLog( 0, "ExitProgram - 14 ( %d )", m_iType );

#if defined( USE_GA )
		if( g_App.GetGAStart() == true )
		{
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d_%d", 14, m_iType );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1_%2", 14, m_iType );

			// GAME_END_ERR_POS
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Game"
				, "Error"
				, chLabel
				, 1
				, "%2FGAME%2FOVER%2FERR"
				, 1 );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
		}		
#endif

		g_App.SetExitProgram();
	}
	else if( m_iType == TYPE_NETWORK_EXIT ||
			 m_iType == TYPE_LANCABLE_EXIT ||
			 m_iType == TYPE_SERVER_TO_CLIENT_PING_EXIT )
	{
		g_QuestMgr.ProcessChangeStage();
		SP2Packet kPacket( CTPK_LOGOUT );
		TCPNetwork::SendToServer( kPacket );

		if( m_iType == TYPE_LANCABLE_EXIT )
		{
			char szLogMessage[MAX_PATH];
			wsprintf( szLogMessage, "%s LanCable UnPluged", g_MyInfo.GetPublicID().c_str() );

			SP2Packet kPacket( LUPK_LOG );
			kPacket << "CheckError";
			kPacket << szLogMessage;
			kPacket << 319;  // 오류번호
			kPacket << true; // write db
			P2PNetwork::SendLog( kPacket );

			LOG.PrintTimeAndLog(0, "%s", szLogMessage );
		}
		else if( m_iType == TYPE_SERVER_TO_CLIENT_PING_EXIT )
		{
			char szLogMessage[MAX_PATH];
			wsprintf( szLogMessage, "%s Server To Client Ping Failed", g_MyInfo.GetPublicID().c_str() );

			SP2Packet kPacket( LUPK_LOG );
			kPacket << "CheckError";
			kPacket << szLogMessage;
			kPacket << 319;  // 오류번호
			kPacket << true; // write db
			P2PNetwork::SendLog( kPacket );

			LOG.PrintTimeAndLog(0, "%s", szLogMessage );
		}
	}
	else if( m_iType == TYPE_XTRAP_EXIT )
	{
		g_QuestMgr.ProcessChangeStage();
		SP2Packet kPacket( CTPK_LOGOUT );
		TCPNetwork::SendToServer( kPacket );

		char szLogMessage[MAX_PATH];
		wsprintf( szLogMessage, "%s Xtrap Check Fail", g_MyInfo.GetPublicID().c_str() );

		SP2Packet kLogPacket( LUPK_LOG );
		kLogPacket << "CheckError";
		kLogPacket << szLogMessage;
		kLogPacket << 320;  // 오류번호
		kLogPacket << true; // write db
		P2PNetwork::SendLog( kLogPacket );

		LOG.PrintTimeAndLog(0, "%s", szLogMessage );
	}
	else if( m_iType == TYPE_BLOCK_EXIT )
	{
		g_QuestMgr.ProcessChangeStage();
		SP2Packet kPacket( CTPK_LOGOUT );
		TCPNetwork::SendToServer( kPacket );

		char szLogMessage[MAX_PATH];
		wsprintf( szLogMessage, "%d:%s Block User", g_MyInfo.GetUserIndex(), g_MyInfo.GetPublicID().c_str() );

		SP2Packet kLogPacket( LUPK_LOG );
		kLogPacket << "CheckError";
		kLogPacket << szLogMessage;
		kLogPacket << 322;  // 오류번호
		kLogPacket << true; // write db
		P2PNetwork::SendLog( kLogPacket );

		LOG.PrintTimeAndLog(0, "%s", szLogMessage );
	}
	else if( m_iType == TYPE_QUEST_ABUSE_EXIT )
	{
		g_QuestMgr.ProcessChangeStage();
		SP2Packet kPacket( CTPK_LOGOUT );
		TCPNetwork::SendToServer( kPacket );

		LOG.PrintTimeAndLog(0, "TYPE_QUEST_ABUSE_EXIT" );
	}

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d_%d", 14, m_iType );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1_%2", 14, m_iType );

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
	}		
#endif

	m_iType = TYPE_NONE;
}

void DisconnectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void DisconnectWnd::OnRender()
{
 	ioWnd::OnRender();
 
	int iXPos = GetDerivedPosX() + GetWidth() / 2;
	int iYPos = GetDerivedPosY() + 144;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.PrintFullText( iXPos, iYPos, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	int iRemainSec = 10 - (FRAMEGETTIME() - m_dwShowTime) / 1000;
	iRemainSec = max( 0, iRemainSec );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), iRemainSec );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos, iYPos + 20, TAT_CENTER );
	kPrinter.ClearList();
    
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_iType == TYPE_LANCABLE_EXIT )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	}
	else if ( m_iType == TYPE_XTRAP_EXIT )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	}
	else if( m_iType == TYPE_BLOCK_EXIT )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	}
	else if( m_iType == TYPE_SERVER_TO_CLIENT_PING_EXIT )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	}
	else if( m_iType == TYPE_NPROTECT_EXIT )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
	}
	else if( m_iType == TYPE_QUEST_ABUSE_EXIT )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(13) );
	}
	else if( m_iType == TYPE_SHUT_DOWN_EXIT )
	{
		if( g_MyInfo.GetChannelingType() == CNT_NEXON )
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, "셧다운제에 의해 게임 이용이 차단됩니다." );
			kPrinter.PrintFullText( iXPos, iYPos + 40, TAT_CENTER );
			kPrinter.ClearList();
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
			kPrinter.PrintFullText( iXPos, iYPos + 40, TAT_CENTER );
			kPrinter.ClearList();
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(11) );
			kPrinter.PrintFullText( iXPos, iYPos + 60, TAT_CENTER );
			kPrinter.ClearList();
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(12) );
			kPrinter.PrintFullText( iXPos, iYPos + 80, TAT_CENTER );
			kPrinter.ClearList();
		}
	}
	else if( m_iType == TYPE_SELECT_SHUT_DOWN_EXIT )
	{
		if( g_MyInfo.GetChannelingType() == CNT_NEXON )
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, "셧다운제에 의해 게임 이용이 차단됩니다." );
			kPrinter.PrintFullText( iXPos, iYPos + 40, TAT_CENTER );
			kPrinter.ClearList();
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(14) );
			kPrinter.PrintFullText( iXPos, iYPos + 40, TAT_CENTER );
			kPrinter.ClearList();
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(15) );
			kPrinter.PrintFullText( iXPos, iYPos + 60, TAT_CENTER );
			kPrinter.ClearList();
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(16) );
			kPrinter.PrintFullText( iXPos, iYPos + 80, TAT_CENTER );
			kPrinter.ClearList();
		}
	}
	else
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	}

	if( m_iType != TYPE_SHUT_DOWN_EXIT && m_iType != TYPE_SELECT_SHUT_DOWN_EXIT )
	{
		kPrinter.PrintFullText( iXPos, iYPos + 40, TAT_CENTER );
		kPrinter.ClearList();
	}

/*	if( m_iType == TYPE_LANCABLE_EXIT )
	{
	//	g_FontMgr.PrintText( iXPos, iYPos , FONT_SIZE_13, STR(1));
	//	g_FontMgr.PrintText( iXPos, iYPos + 24 , FONT_SIZE_13, STR(2) );
	}
	else if ( m_iType == TYPE_XTRAP_EXIT )
	{
	//	g_FontMgr.PrintText( iXPos, iYPos , FONT_SIZE_13, STR(3));
	//	g_FontMgr.PrintText( iXPos, iYPos + 24 , FONT_SIZE_13, STR(4) );
	}
	else if( m_iType == TYPE_BLOCK_EXIT )
	{
	//	g_FontMgr.PrintText( iXPos, iYPos , FONT_SIZE_13,      STR(5));
	//	g_FontMgr.PrintText( iXPos, iYPos + 24 , FONT_SIZE_13, STR(6) );
	}
	else if( m_iType == TYPE_SERVER_TO_CLIENT_PING_EXIT )
	{
	//	g_FontMgr.PrintText( iXPos, iYPos , FONT_SIZE_13, STR(10));
	//	g_FontMgr.PrintText( iXPos, iYPos + 24 , FONT_SIZE_13, STR(11) );
	}
	else
	{
	//	g_FontMgr.PrintText( iXPos, iYPos , FONT_SIZE_13, STR(7));
	//	g_FontMgr.PrintText( iXPos, iYPos + 24 , FONT_SIZE_13, STR(8) );
	}
*/
}

void DisconnectWnd::iwm_show()
{
	m_dwShowTime = FRAMEGETTIME();
	
	if( g_GUIMgr.IsNoneUIMode() )
		g_GUIMgr.SetNoneUIMode( false );
}

void DisconnectWnd::iwm_create()
{
	m_dwStyle |= IWS_PROCESS_ALWAYS;
}

void DisconnectWnd::OnProcess( float fTimePerSec )
{
	if(m_dwShowTime == 0) return;

	if(FRAMEGETTIME() - m_dwShowTime > 10000)
	{
		HideWnd();
		m_dwShowTime = 0;
	}
}

void DisconnectWnd::SetType( int iType )
{
	if( m_iType != TYPE_NPROTECT_EXIT ) // 서버에서 nprotect exit 메세지 전송후 disconnect을 해서 nprotect 메세지를 보호하기 위해
		m_iType = iType;

	enum { DEFAULT_HEIGHT = 278, SHUT_DOWN_HEIGHT = 318, BTN_GAP = 48};
	int iHeight = 0;
	if( m_iType == TYPE_SHUT_DOWN_EXIT || m_iType == TYPE_SELECT_SHUT_DOWN_EXIT )
	{
		iHeight = SHUT_DOWN_HEIGHT;
	}
	else
	{
		iHeight = DEFAULT_HEIGHT;
	}

	SetSize( GetWidth(), iHeight );
	ioWnd *pWnd = FindChildWnd( ID_OK );
	if( pWnd )
		pWnd->SetWndPos( pWnd->GetXPos(), GetHeight() - BTN_GAP );

	// 하단바
	if( m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetPosition( 0, GetHeight() - m_pMultiplyFrm->GetHeight() );
	}
}







