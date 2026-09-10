

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUITitle.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"

#include "../TextColorTable.h"
#include "../DirectInput.h"
#include "../ioKeyManager.h"
#include "../Setting.h"
#include "../ioApplication.h"
#include "../NetworkWrappingFunc.h"
#include "../WndID.h"
#include "../ioPlayStage.h"
#include "../ioMyInfo.h"
#include "../ioMyRoomMode.h"
#include "../ioBattleRoomMgr.h"
#include "../ioLadderTeamMgr.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioBaseChar.h"
#include "../ioQuestManager.h"
#include "../ioExtendSoundManager.h"

#include "TimeGateSearch.h"
#include "PartyMenuWnd.h"

#include "TotalResultMainWnd.h"
#include "ioSP2GUIManager.h"
#include "ioExpansionBox.h"
#include "GameChatInput.h"

#include <strsafe.h>

#include "../TournamentNode.h"
#include "../ioTournamentManager.h"

ioExpansionBox::ioExpansionBox()
{
	m_pLine  = NULL;

	m_iMinWidth  = 300;
	m_iMinHeight = 200;

	m_iTopBottomTextGap = 10;
	m_iSideTextGap = 10;

	m_iBoxType = MB_YESNO;
	m_pPrevWnd = NULL;
	m_pReturnWnd = NULL;
}

ioExpansionBox::~ioExpansionBox()
{
	SAFEDELETE( m_pLine );
}

void ioExpansionBox::SetMessage( int iType, const char *szBuf, ioWnd *pPrevWnd, ioWnd *pReturnWnd )
{
	m_pPrevWnd		= pPrevWnd;
	m_pReturnWnd = pReturnWnd;

	m_MessageList.clear();
	m_MessageList = ioStringConverter::Split( szBuf, "#" );

	int iMaxWidth  = m_iMinWidth;
	int iMaxHeight = m_iMinHeight;

	StringVector::iterator iter;
	for( iter = m_MessageList.begin() ; iter!=m_MessageList.end() ; ++iter )
	{
		int iCurLineWidth = g_FontMgr.GetTextWidth( iter->c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_13  );
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
	}
	iMaxWidth += m_iSideTextGap * 2;
	iMaxHeight = HEIGHT_TOP_GAP + m_iTopBottomTextGap * 2 + m_MessageList.size() * TEXT_LINE_GAP + HEIGHT_BOTTOM_GAP;

	SetSize( iMaxWidth, iMaxHeight );
	SetWndPos( (Setting::Width() - iMaxWidth ) / 2,
		(Setting::Height() - iMaxHeight ) / 2 );

	if( m_pLine )
	{
		m_pLine->SetSize( iMaxWidth-2, m_pLine->GetHeight() );
	}

	m_iBoxType = iType;
	ReBatchButton( iType );

	if( !IsShow() )
		ShowWnd();
}

void ioExpansionBox::ReBatchButton( int iType )
{
	if( iType == MB_OK )
	{
		ioWnd *pWnd = FindChildWnd( ID_YES );
		ioWnd *pXBtn = FindChildWnd( ID_X );
		if( pWnd && pXBtn )
		{
			int iXPos = ( GetWidth() - pWnd->GetWidth() ) / 2;
			int iYPos = GetHeight() - pWnd->GetHeight() - BTN_YPOS_OFFSET;

			pWnd->SetTitleColor( 0xFFFFB400, 0xFF000000 );
			pWnd->SetTitleText( STR(1) );

			pWnd->SetWndPos( iXPos, iYPos );
			pXBtn->SetWndPos( GetWidth() - 31, 6 );
		}

		HideChildWnd( ID_NO );
		HideChildWnd( ID_OTHER );
		ShowChildWnd( ID_X );
		SetChildInActive( ID_X );
	}
	else if( iType == MB_YESNO )
	{
		ioWnd *pYes = FindChildWnd( ID_YES );
		ioWnd *pNo  = FindChildWnd( ID_NO );
		ioWnd *pXBtn = FindChildWnd( ID_X );
		if( pYes && pNo && pXBtn )
		{
			int iBtnWidth = pYes->GetWidth() + pNo->GetWidth() + 10;

			int iXPos = ( GetWidth() - iBtnWidth ) / 2;
			int iYPos = GetHeight() - pYes->GetHeight() - BTN_YPOS_OFFSET;

			pYes->SetTitleColor( 0xFFFFB400, 0xFF000000 );
			pYes->SetTitleText( STR(2) );

			pYes->SetWndPos( iXPos, iYPos );
			pNo->SetWndPos( iXPos + pYes->GetWidth() + 10, iYPos );
			pXBtn->SetWndPos( GetWidth() - 31, 6 );
		}

		ShowChildWnd( ID_NO );
		ShowChildWnd( ID_X );
		SetChildInActive( ID_X );

		HideChildWnd( ID_OTHER );
	}
	else if( iType == MB_YESNOOTHER )
	{
		ioWnd *pYes = FindChildWnd( ID_YES );
		ioWnd *pNo  = FindChildWnd( ID_NO );
		ioWnd *pXBtn = FindChildWnd( ID_X );
		ioWnd *pOther = FindChildWnd( ID_OTHER );

		if( pYes && pNo && pXBtn && pOther )
		{
			int iBtnWidth = pYes->GetWidth() + pNo->GetWidth() + pOther->GetWidth() + 10;

			int iXPos = ( GetWidth() - iBtnWidth ) / 2;
			int iYPos = GetHeight() - pYes->GetHeight() - BTN_YPOS_OFFSET;

			pYes->SetTitleColor( 0xFFFF8200, 0xFF000000 );
			pYes->SetTitleText( STR(3) );

			pOther->SetWndPos( iXPos, iYPos );
			pNo->SetWndPos( iXPos + pOther->GetWidth() + 5, iYPos );
			pYes->SetWndPos( iXPos + pOther->GetWidth() + pNo->GetWidth() + 10, iYPos );
			pXBtn->SetWndPos( GetWidth() - 31, 6 );
		}

		ShowChildWnd( ID_NO );
		ShowChildWnd( ID_X );
		ShowChildWnd( ID_OTHER );
		SetChildInActive( ID_X );
	}
}

void ioExpansionBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlackLine" )
	{
		m_pLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioExpansionBox::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iMinWidth  = xElement.GetIntAttribute_e( "MinWidth" );
	m_iMinHeight = xElement.GetIntAttribute_e( "MinHeight" );

	m_iTopBottomTextGap = xElement.GetIntAttribute_e( "TopBottomTextGap" );
	m_iSideTextGap = xElement.GetIntAttribute_e( "SideTextGap" );
}

void ioExpansionBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_YES:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDYES );
			}
		}
		break;
	case ID_OTHER:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDOTHER );
			}
		}
		break;
	case ID_NO:
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDNO );
			}
		}
		break;
	}
}

void ioExpansionBox::iwm_hide()
{
	if( m_pPrevWnd && !m_pPrevWnd->IsShow() )
		m_pPrevWnd->ShowWnd();
}

void ioExpansionBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + GetWidth() / 2;
	int iYPos = GetDerivedPosY() + TOP_TEXT_GAP + m_iTopBottomTextGap;

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.SetAlignType( TAT_CENTER );

	int iLineCnt = m_MessageList.size();
	for( int i=0 ; i<iLineCnt ; i++ )
	{
		if( !m_MessageList[i].empty() )
		{
			g_FontMgr.PrintText( iXPos, iYPos + TEXT_LINE_GAP * i, FONT_SIZE_13, m_MessageList[i].c_str() );
		}
	}

	if( m_pLine )
	{
		m_pLine->Render( GetDerivedPosX(), GetDerivedPosY(), UI_RENDER_NORMAL );
	}

}
//////////////////////////////////////////////////////////////////////////
ioExitMsgBox::ioExitMsgBox()
{
	m_pIconBack	= NULL;
	m_pPenaltyIcon = NULL;
	m_pNonePenaltyIcon = NULL;
	m_pPlayStage = NULL;

	m_bPenaltyExit= false;
	m_dwStartTime = 0xFFFFFFFF;
	m_dwMsgType   = MSG_LOSA_EXIT;
}

ioExitMsgBox::~ioExitMsgBox()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pPenaltyIcon );
	SAFEDELETE( m_pNonePenaltyIcon );
}

void ioExitMsgBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "PenaltyIcon" )
	{
		SAFEDELETE( m_pPenaltyIcon );
		m_pPenaltyIcon = pImage;
	}
	else if( szType == "NonePenaltyIcon" )
	{
		SAFEDELETE( m_pNonePenaltyIcon );
		m_pNonePenaltyIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioExitMsgBox::SetMsgInfo( ioPlayStage *pPlayStage )
{
	if( IsShow() )
		HideWnd();

	m_bPenaltyExit = false;
	m_pPlayStage = pPlayStage;
	if( m_pPlayStage )
	{
		if( m_pPlayStage->GetModeType() == MT_MYROOM && ioMyRoomMode::GetMyRoomType() == MRT_LOBBY )
		{
			g_ExSoundMgr.PlaySound( ExSound::EST_LOGOUT );
			m_dwMsgType   = MSG_LOSA_EXIT;
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(1) );	
		}
		else if( m_pPlayStage->GetModeType() == MT_TRAINING && ioMyRoomMode::GetMyRoomType() == MRT_FIND_MODE )
		{
			m_dwMsgType   = MSG_LOSA_EXIT;
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(1) );
		}
		else if( g_BattleRoomMgr.IsBattleRoom() && ( !g_BattleRoomMgr.IsBattleModePlaying() || TotalResultMainWnd::IsNoPenalty() ) )
		{
			m_dwMsgType   = MSG_BATTLE_LEAVE;
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(2)  );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "광장으로 이동하시겠습니까?"  );
			}
		}
		else if( g_LadderTeamMgr.IsLadderTeam() && ( !g_LadderTeamMgr.IsLadderBattleModePlaying() || TotalResultMainWnd::IsNoPenalty() ) )
		{
			m_dwMsgType   = MSG_LADDER_LEAVE;
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(3)  );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "광장으로 이동하시겠습니까?"  );				
			}
		}
		else if( g_BattleRoomMgr.IsBattleRoom() && g_GUIMgr.IsShow( TOURNAMENT_BATTLE_INVITE_WND ) )
		{
			m_dwMsgType   = MSG_BATTLE_LEAVE;
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(2)  );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "광장으로 이동하시겠습니까?"  );				
			}
		}
		else if( g_LadderTeamMgr.IsLadderTeam() && g_GUIMgr.IsShow( TOURNAMENT_BATTLE_INVITE_WND ) )
		{
			m_dwMsgType   = MSG_LADDER_LEAVE;
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(3)  );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "광장으로 이동하시겠습니까?"  );				
			}
		}
		else if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			m_dwStartTime = 0;
			m_dwMsgType   = MSG_ROOM_EXIT;
			ShowWnd();
		}
		else if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY && ( m_pPlayStage->GetModeType() == MT_MYROOM || m_pPlayStage->GetModeType() == MT_HEADQUARTERS || m_pPlayStage->GetModeType() == MT_HOUSE ) )
		{
			m_dwMsgType   = MSG_LOSA_EXIT;
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(1) );
		}
		else if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY &&
				( m_pPlayStage->GetModeType() == MT_TRAINING || m_pPlayStage->GetModeType() == MT_MYROOM || m_pPlayStage->GetModeType() == MT_HEADQUARTERS || m_pPlayStage->GetModeType() == MT_HOUSE ) )
		{
			m_dwMsgType   = MSG_ENTER_LOBBY;
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4) );
		}
		else if( TotalResultMainWnd::IsNoPenalty() || m_pPlayStage->GetModeType() == MT_SURVIVAL || m_pPlayStage->GetModeType() == MT_MONSTER_SURVIVAL || m_pPlayStage->GetModeType() == MT_DUNGEON_A || m_pPlayStage->GetModeType() == MT_FIGHT_CLUB )
		{
			m_dwStartTime = 0;
			m_dwMsgType   = MSG_ROOM_EXIT;
			ShowWnd();	
		}
		else if( m_pPlayStage->GetRoomStyle() == RSTYLE_LADDERBATTLE )
		{
			m_bPenaltyExit= true;
			m_dwMsgType   = MSG_ROOM_EXIT;
			ShowWnd();
		}
		else 
		{
			if( !IsMyCharPenaltyState() || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_dwStartTime == 0xFFFFFFFF )
				m_bPenaltyExit = false;
			else if( FRAMEGETTIME() - m_dwStartTime < 30000 )
				m_bPenaltyExit = false;
			else
				m_bPenaltyExit = true;

			m_dwMsgType = MSG_ROOM_EXIT;
			ShowWnd();
		}
	}

	ioWnd *pButton = FindChildWnd( ID_QUICK_LEAVE );
	if( pButton )
	{
		if( m_bPenaltyExit )
			pButton->SetTitleText( "즉시나가기(벌금)" );
		else
			pButton->SetTitleText( "즉시나가기" );
	}
}

void ioExitMsgBox::SetPlayJoin()
{
	if( m_dwStartTime == 0xFFFFFFFF )
		m_dwStartTime = FRAMEGETTIME();
}

void ioExitMsgBox::iwm_show()
{
	if( !m_pPlayStage )
		return;

	HideChildWnd( ID_LEAVE_RESERVE );
	HideChildWnd( ID_RESERVE_CANCEL );
	HideChildWnd(ID_SURRENDER);
	HideChildWnd(ID_NEVER_SURRENDER);

	if( Help::IsMonsterDungeonMode( ioPlayMode::GetModeType() ) )
	{
		if( m_pPlayStage->IsReservedExit() )
		{
			ShowChildWnd( ID_NEVER_SURRENDER );
		}
		else
		{
			ShowChildWnd( ID_SURRENDER );
		}
	}
	else
	{
		if( m_pPlayStage->IsReservedExit() )
		{
			ShowChildWnd( ID_RESERVE_CANCEL );	
		}
		else
		{
			ShowChildWnd( ID_LEAVE_RESERVE );
		}		
	}
}

void ioExitMsgBox::iwm_hide()
{
}

bool ioExitMsgBox::iwm_spacebar()
{
	ioButton *pBtnLeaveReserve = dynamic_cast<ioButton*>( FindChildWnd( ID_LEAVE_RESERVE ) );
	if ( pBtnLeaveReserve && pBtnLeaveReserve->IsShow() )
	{
		iwm_command( pBtnLeaveReserve, IOBN_BTNUP, 0 );
		return true;
	}

	ioButton *pBtnReserveCancel = dynamic_cast<ioButton*>( FindChildWnd( ID_RESERVE_CANCEL ) );
	if ( pBtnReserveCancel && pBtnReserveCancel->IsShow() )
	{
		iwm_command( pBtnReserveCancel, IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

void ioExitMsgBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_LEAVE_RESERVE:
	case ID_RESERVE_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( m_pPlayStage )
				{
					m_pPlayStage->SendOwnerDamageList();

					SP2Packet kPacket( CTPK_EXIT_ROOM );
					if( !m_pPlayStage->IsReservedExit() )
						kPacket << EXIT_ROOM_RESERVED;
					else
						kPacket << EXIT_ROOM_CANCELED;
					kPacket << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			HideWnd();
		}
		break;
	case ID_SURRENDER:
	case ID_NEVER_SURRENDER:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if(m_pPlayStage && m_pPlayStage->GetPlayMode() && m_pPlayStage->GetPlayMode()->GetModeState() == ioPlayMode::MS_PLAY )
				{
					m_pPlayStage->SendOwnerDamageList();

					SP2Packet kPacket( CTPK_EXIT_ROOM );
					if( !m_pPlayStage->IsReservedExit() )
						kPacket << EXIT_ROOM_RESERVED;
					else
						kPacket << EXIT_ROOM_CANCELED;
					kPacket << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			HideWnd();
		}
		break;

	case ID_QUICK_LEAVE:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( m_pPlayStage )
					m_pPlayStage->SendOwnerDamageList();

				// 페널티
				if( m_bPenaltyExit )
				{
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_PENALTY << g_MyInfo.GetCurPenaltyPeso() << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
				else
				{
					// 로비로 이동
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << GetExitType() << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			HideWnd();
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				switch( m_dwMsgType )
				{
				case MSG_LOSA_EXIT:
					{
						g_QuestMgr.ProcessChangeStage();
						SP2Packet kPacket( CTPK_LOGOUT );
						TCPNetwork::SendToServer( kPacket );

						LOG.PrintTimeAndLog(0, "MSG_LOSA_EXIT" );
					}
					break;
				case MSG_ENTER_LOBBY:
					{
						if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
						{
							if( ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_HOUSE)
							{
								SP2Packet kPacket( CTPK_EXIT_ROOM );
								kPacket << EXIT_ROOM_LOBBY << 0 << true;
								TCPNetwork::SendToServer( kPacket );
								TCPNetwork::MouseBusy( true );

								// 로비
								ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
							}
							else
							{
								// 로비
								ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
								g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
								g_App.SetModeType( 0, 0, 0, PT_NONE );
							}
						}
						else
						{
							if( ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_HOUSE )
							{
								//종료 처리
								g_QuestMgr.ProcessChangeStage();
								SP2Packet kPacket( CTPK_LOGOUT );
								TCPNetwork::SendToServer( kPacket );

								LOG.PrintTimeAndLog(0, "MSG_LOSA_EXIT" );
							}
							else
							{
								//전투대기방의 경우 로비임으로 서칭
								g_App.SendSearchModeJoin();
							}
						}
					}
					break;
				case MSG_BATTLE_LEAVE:
				case MSG_LADDER_LEAVE:
					{
						if( m_pPlayStage && 
							m_pPlayStage->GetModeType() != MT_MYROOM && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
						{
							SP2Packet kPacket( CTPK_EXIT_ROOM );
							kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
							TCPNetwork::SendToServer( kPacket );
							g_BattleRoomMgr.LeaveRoom();
							g_LadderTeamMgr.LeaveTeam();
						}
						else if( m_dwMsgType == MSG_BATTLE_LEAVE )
						{
							TCPNetwork::MouseBusy( true );
							SP2Packet kPacket( CTPK_USER_BATTLEROOM_LEAVE );
							TCPNetwork::SendToServer( kPacket );
						}		
						else if( m_dwMsgType == MSG_LADDER_LEAVE )
						{
							TCPNetwork::MouseBusy( true );
							SP2Packet kPacket( CTPK_LADDERTEAM_LEAVE );
							TCPNetwork::SendToServer( kPacket );
						}		
					}
				}
			}
		}
		break;
	}
}

int ioExitMsgBox::GetExitType()
{
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		if( m_pPlayStage && ( m_pPlayStage->GetModeType() == MT_TRAINING || m_pPlayStage->GetModeType() == MT_HEADQUARTERS || m_pPlayStage->GetModeType() == MT_HOUSE ) )
			return EXIT_ROOM_LOBBY;
	}
	else
	{
		if( m_pPlayStage && m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
			return EXIT_ROOM_LOBBY;
	}

	return EXIT_ROOM_WANT;
}

bool ioExitMsgBox::IsMyCharPenaltyState()
{
	if( !m_pPlayStage ) return false;
	if( m_dwStartTime == 0 ) return true;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar ) return false;

	if( pOwnerChar->GetState() == CS_VIEW ||
		pOwnerChar->GetState() == CS_READY ||
		pOwnerChar->GetState() == CS_LOADING ||
		pOwnerChar->GetState() == CS_OBSERVER )
		return false;

	return true;
}

void ioExitMsgBox::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( !m_pPlayStage ) return;
    
	bool bPenaltyExit = m_bPenaltyExit;
	if( COMPARE( m_dwStartTime, 1, 0xFFFFFFFF ) )
	{
		if( FRAMEGETTIME() - m_dwStartTime > 30000 )
		{
			m_dwStartTime  = 0;
			m_bPenaltyExit = true;
		}
	}

	if( m_bPenaltyExit ) 
	{
		ModeType eModeType = m_pPlayStage->GetModeType();
		if( TotalResultMainWnd::IsNoPenalty() || eModeType == MT_SURVIVAL || eModeType == MT_MONSTER_SURVIVAL || eModeType == MT_DUNGEON_A || eModeType == MT_FIGHT_CLUB )
		{
			m_bPenaltyExit = false;
		}

		if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			m_bPenaltyExit = false;
		}
	}

	if( bPenaltyExit != m_bPenaltyExit )
		HideWnd();
}

void ioExitMsgBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();	

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 206, iYPos + 98, UI_RENDER_MULTIPLY );
	}

	ioComplexStringPrinter kPrinter;
	if( m_bPenaltyExit )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 33, 90, 162 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(1) );

		if( m_pPenaltyIcon )
			m_pPenaltyIcon->Render( iXPos + 206, iYPos + 98 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), g_MyInfo.GetCurPenaltyPeso() );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	}
	else if( COMPARE( m_dwStartTime, 1, 0xFFFFFFFF ) )
	{
		int iGapSec = (FRAMEGETTIME() - m_dwStartTime) / 1000;
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 33, 90, 162 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(5), max( 30 - iGapSec, 0 ) );

		if( m_pNonePenaltyIcon )
			m_pNonePenaltyIcon->Render( iXPos + 206, iYPos + 98 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 33, 90, 162 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, STR(7) );

		if( m_pNonePenaltyIcon )
			m_pNonePenaltyIcon->Render( iXPos + 206, iYPos + 98 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	}

	if( Help::IsMonsterDungeonMode(m_pPlayStage->GetModeType()) )
	{
		kPrinter.PrintFullText( iXPos + 206, iYPos + 144, TAT_CENTER );
		kPrinter.ClearList();
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_13, "도전포기" );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "를 하면 " );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, "보상획득" );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "이 가능합니다." );
		kPrinter.PrintFullText( iXPos + 206, iYPos + 164, TAT_CENTER );
	}
	else
	{
		kPrinter.PrintFullText( iXPos + 206, iYPos + 144, TAT_CENTER );
		kPrinter.ClearList();
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );

		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, "광장으로 나가시겠습니까?" );
		}

		kPrinter.PrintFullText( iXPos + 206, iYPos + 164, TAT_CENTER );
		kPrinter.PrintFullText( iXPos + 206, iYPos + 164, TAT_CENTER );
	}
}

//////////////////////////////////////////////////////////////////////////
ioBattleRoomExitMsgBox::ioBattleRoomExitMsgBox()
{
	m_pPlayStage		= NULL;

	m_pIconBack			= NULL;
	m_pPenaltyIcon		= NULL;
	m_pNonePenaltyIcon	= NULL;
	m_pBottomBar        = NULL;

	m_bPenaltyExit		= false;
	m_dwStartTime		= 0xFFFFFFFF;
	m_bOpenRoomListWnd	= false;
}

ioBattleRoomExitMsgBox::~ioBattleRoomExitMsgBox()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pPenaltyIcon );
	SAFEDELETE( m_pNonePenaltyIcon );
	SAFEDELETE( m_pBottomBar );
}

void ioBattleRoomExitMsgBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "PenaltyIcon" )
	{
		SAFEDELETE( m_pPenaltyIcon );
		m_pPenaltyIcon = pImage;
	}
	else if( szType == "NonePenaltyIcon" )
	{
		SAFEDELETE( m_pNonePenaltyIcon );
		m_pNonePenaltyIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioBattleRoomExitMsgBox::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomBar" )
	{
		SAFEDELETE( m_pBottomBar );
		m_pBottomBar = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ioBattleRoomExitMsgBox::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
}

void ioBattleRoomExitMsgBox::SetMsgInfo( ioPlayStage *pPlayStage )
{
	if( IsShow() )
	{
		HideWnd();
	}

	if( !g_BattleRoomMgr.IsBattleRoom() )
	{
		LOG.PrintTimeAndLog( 0, "ioBattleRoomExitMsgBox::SetMsgInfo BattleRoom None" );
		return;
	}

	m_bPenaltyExit	= false;
	m_pPlayStage	= pPlayStage;

	if( m_pPlayStage )
	{
		if( !g_BattleRoomMgr.IsBattleModePlaying() || TotalResultMainWnd::IsNoPenalty() || g_GUIMgr.IsShow( TOURNAMENT_BATTLE_INVITE_WND ) )
		{
			m_dwStartTime = 0;
			m_bPenaltyExit= false;
		}
		else if( TotalResultMainWnd::IsNoPenalty() || m_pPlayStage->GetModeType() == MT_SURVIVAL || m_pPlayStage->GetModeType() == MT_MONSTER_SURVIVAL || m_pPlayStage->GetModeType() == MT_DUNGEON_A || m_pPlayStage->GetModeType() == MT_FIGHT_CLUB|| m_pPlayStage->GetModeType() == MT_SURVIVAL )
		{
			m_dwStartTime = 0;
			m_bPenaltyExit= false;
		}
		else 
		{
			if( !IsMyCharPenaltyState() || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_dwStartTime == 0xFFFFFFFF )
				m_bPenaltyExit = false;
			else if( FRAMEGETTIME() - m_dwStartTime < 30000 )
				m_bPenaltyExit = false;
			else
				m_bPenaltyExit = true;
		}
	}
	ShowWnd();	
}

void ioBattleRoomExitMsgBox::SetPlayJoin()
{
	if( m_dwStartTime == 0xFFFFFFFF )
		m_dwStartTime = FRAMEGETTIME();
}

void ioBattleRoomExitMsgBox::iwm_show()
{
	if( !m_pPlayStage )
		return;

	HideChildWnd( ID_ROOM_LEAVE_BLUE );
	HideChildWnd( ID_ROOM_LEAVE_RED );
	HideChildWnd( ID_ROOM_SURRENDER_BLUE );
	HideChildWnd( ID_ROOM_SURRENDER_RED );

	m_bObserberUser = false;
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		m_bObserberUser = true;

	if( m_bPenaltyExit )
	{
		if( Help::IsMonsterDungeonMode( ioPlayMode::GetModeType() ) )
		{
			ShowChildWnd( ID_ROOM_SURRENDER_RED );
		}
		else
		{
			ShowChildWnd( ID_ROOM_LEAVE_RED );	
		}
	}
	else
	{
		if( m_dwStartTime != 0 && Help::IsMonsterDungeonMode( ioPlayMode::GetModeType() ) && !m_bObserberUser )
		{
			ShowChildWnd( ID_ROOM_SURRENDER_BLUE );
		}
		else
		{
			ShowChildWnd( ID_ROOM_LEAVE_BLUE );	
		}
	}
	OpenRoomListWnd( false );
}

void ioBattleRoomExitMsgBox::iwm_hide()
{
	g_GUIMgr.HideWnd( BATTLEROOM_EXIT_POPUP );
	g_GUIMgr.HideWnd( BATTLE_QUICKSTART_OPTION );
}

void ioBattleRoomExitMsgBox::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/battleroomexitroomlistwnd.xml", this );
}

void ioBattleRoomExitMsgBox::OpenRoomListWnd( bool bOpen )
{
	m_bOpenRoomListWnd = bOpen;
	if( !m_bOpenRoomListWnd )
	{
		HideChildWnd( ID_ROOM_LIST_WND );

		SetSize( GetWidth(), NORMAL_WND_HEIGHT );
	}
	else
	{
		ShowChildWnd( ID_ROOM_LIST_WND );

		SetSize( GetWidth(), ROOM_SEARCH_WND_HEIGHT );
	}
	SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ), ( ( Setting::Height() / 2 ) - ( GetHeight() / 2) ) - 20 );
}

bool ioBattleRoomExitMsgBox::iwm_spacebar()
{
	ioButton *pBtnRoomLeaveBlue = dynamic_cast<ioButton*>( FindChildWnd( ID_ROOM_LEAVE_BLUE ) );
	if ( pBtnRoomLeaveBlue && pBtnRoomLeaveBlue->IsShow() )
	{
		iwm_command( pBtnRoomLeaveBlue, IOBN_BTNUP, 0 );
		return true;
	}

	ioButton *pBtnRoomLeaveRed = dynamic_cast<ioButton*>( FindChildWnd( ID_ROOM_LEAVE_RED ) );
	if ( pBtnRoomLeaveRed && pBtnRoomLeaveRed->IsShow() )
	{
		iwm_command( pBtnRoomLeaveRed, IOBN_BTNUP, 0 );
		return true;
	}

	ioButton *pBtnRoomSurrenderBlue = dynamic_cast<ioButton*>( FindChildWnd( ID_ROOM_SURRENDER_BLUE ) );
	if ( pBtnRoomSurrenderBlue  && pBtnRoomSurrenderBlue->IsShow() )
	{
		iwm_command( pBtnRoomSurrenderBlue, IOBN_BTNUP, 0 );
		return true;
	}
	
	ioButton *pBtnRoomSurrenderRed = dynamic_cast<ioButton*>( FindChildWnd( ID_ROOM_SURRENDER_RED ) );
	if ( pBtnRoomSurrenderRed && pBtnRoomSurrenderRed->IsShow())
	{
		iwm_command( pBtnRoomSurrenderRed, IOBN_BTNUP, 0 );
		return true;
	}
	return false;
}

void ioBattleRoomExitMsgBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	//배틀룸
	case ID_ROOM_LEAVE_BLUE:
		if( cmd == IOBN_BTNUP )
		{
			if( !m_bPenaltyExit )
			{
				if( m_pPlayStage && 
					m_pPlayStage->GetModeType() != MT_MYROOM && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
				{
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
					TCPNetwork::SendToServer( kPacket );

					g_BattleRoomMgr.LeaveRoom();
					ExitLogByRegularTournament();
				}
				else
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_USER_BATTLEROOM_LEAVE );
					TCPNetwork::SendToServer( kPacket );
					ExitLogByRegularTournament();
				}		
			}
		}
		break;
	//게임중
	case ID_ROOM_LEAVE_RED:
		if( cmd == IOBN_BTNUP )
		{
			ioBattleRoomExitPopup *pExitPopup = dynamic_cast< ioBattleRoomExitPopup * >(g_GUIMgr.FindWnd( BATTLEROOM_EXIT_POPUP ));
			if( pExitPopup )
			{
				pExitPopup->SetPopupType( m_pPlayStage, ioBattleRoomExitPopup::POPUP_TYPE_LEAVE, m_bPenaltyExit );
			}
		}
		break;
	case ID_ROOM_SURRENDER_BLUE:
	case ID_ROOM_SURRENDER_RED:
		if( cmd == IOBN_BTNUP )
		{
			ioBattleRoomExitPopup *pExitPopup = dynamic_cast< ioBattleRoomExitPopup * >(g_GUIMgr.FindWnd( BATTLEROOM_EXIT_POPUP ));
			if( pExitPopup )
			{
				pExitPopup->SetPopupType( m_pPlayStage, ioBattleRoomExitPopup::POPUP_TYPE_SURRENDER, m_bPenaltyExit );
			}
		}
		break;
	case ID_ROOM_LIST:
		if( cmd == IOBN_BTNUP )
		{
			OpenRoomListWnd( !m_bOpenRoomListWnd );
		}
		break;
	case ID_ROOM_QUICK_JOIN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else if( g_BattleRoomMgr.GetQuickStartModeType() == BMT_USER_CUSTOM )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "유저모드는 빠른시작이 불가능합니다" );
			}
			else if( !g_MyInfo.IsTutorialUser() )
			{
				if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영전 중에는 빠른전투를 할 수 없습니다." );
				}
				else
				{
					TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH ));
					if( pSearchWnd )
					{
						if( pSearchWnd->IsShow() && pSearchWnd->GetSearchType() == TimeGateSearch::ST_PARTY_JOIN )
							pSearchWnd->HideWnd();
						else
						{
							pSearchWnd->SetSearching( "조건에 맞는 전투방을 검색중 . . .", TimeGateSearch::ST_PARTY_JOIN );		
						}
					}
				}		
			}
		}
		break;
	case ID_ROOM_SETTING:
		if( cmd == IOBN_BTNUP )
		{
			BattleQuickStartOption *pSelectWnd = dynamic_cast<BattleQuickStartOption*>(g_GUIMgr.FindWnd( BATTLE_QUICKSTART_OPTION ));
			if( pSelectWnd )
			{
				pSelectWnd->SetQuickStart( this );
				pSelectWnd->ShowWnd();
			}
		}
		break;
	case BATTLE_QUICKSTART_OPTION:
		if( cmd == IOBN_BTNUP )
		{
			ioBattleRoomExitRoomListWnd *pBattleRoomList = dynamic_cast< ioBattleRoomExitRoomListWnd * >( FindChildWnd( ID_ROOM_LIST_WND ) );
			if( pBattleRoomList && pBattleRoomList->IsShow() )
				pBattleRoomList->Refresh();
		}
		break;
	}
}

void ioBattleRoomExitMsgBox::ExitLogByRegularTournament()
{
	if( !g_BattleRoomMgr.IsTournamentModeType() )
		return;

	TournamentNode* pNode = g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
	if( !pNode )
		return;

	if( pNode->GetType() != TournamentNode::TYPE_REGULAR )
		return;

	if( !pNode->IsLogSend() )
		return;

	char szLog[MAX_PATH] = "";	
	sprintf( szLog, "[대회로그] 대회방 나가기(전투방 나가기 버튼) - %s", g_MyInfo.GetPublicID().c_str() );

	SP2Packet kPacket2( LUPK_LOG );
	kPacket2 << "TournamentLog";  // 로그 파일 타입
	kPacket2 << szLog;
	kPacket2 << 4000;   // 오류번호:4000
	kPacket2 << false;  // write db
	P2PNetwork::SendLog(kPacket2);

	if( g_App.IsMeDeveloper() )
		LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
}

bool ioBattleRoomExitMsgBox::IsMyCharPenaltyState()
{
	if( !m_pPlayStage ) return false;
	if( m_dwStartTime == 0 ) return true;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar ) return false;

	if( pOwnerChar->GetState() == CS_VIEW ||
		pOwnerChar->GetState() == CS_READY ||
		pOwnerChar->GetState() == CS_LOADING ||
		pOwnerChar->GetState() == CS_OBSERVER )
		return false;

	return true;
}

void ioBattleRoomExitMsgBox::ApplyBattleRoomList( SP2Packet &rkPacket )
{
	ioBattleRoomExitRoomListWnd *pBattleRoomList = dynamic_cast< ioBattleRoomExitRoomListWnd * >( FindChildWnd( ID_ROOM_LIST_WND ) );
	if( pBattleRoomList && pBattleRoomList->IsShow() )
	{
		pBattleRoomList->SetPartyList( rkPacket );
	}
}

void ioBattleRoomExitMsgBox::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( !m_pPlayStage ) return;

	bool bPenaltyExit = m_bPenaltyExit;
	if( COMPARE( m_dwStartTime, 1, 0xFFFFFFFF ) )
	{
		if( FRAMEGETTIME() - m_dwStartTime > 30000 )
		{
			m_dwStartTime  = 0;
			m_bPenaltyExit = true;
		}
	}

	if( m_bPenaltyExit ) 
	{
		ModeType eModeType = m_pPlayStage->GetModeType();
		if( TotalResultMainWnd::IsNoPenalty() || eModeType == MT_SURVIVAL || eModeType == MT_MONSTER_SURVIVAL || eModeType == MT_DUNGEON_A || eModeType == MT_FIGHT_CLUB )
		{
			m_bPenaltyExit = false;
		}
	}

	if( bPenaltyExit != m_bPenaltyExit )
	{
		HideWnd();
	}
}

void ioBattleRoomExitMsgBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();	

	if( m_pBottomBar )
	{
		m_pBottomBar->SetSize( GetWidth(), GetHeight() - BOTTOM_BAR_START_Y  );
		m_pBottomBar->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 206, iYPos + 98, UI_RENDER_MULTIPLY );
	}

	ioComplexStringPrinter kPrinter;
	if( m_bPenaltyExit )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 33, 90, 162 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, "나가기" );

		if( m_pPenaltyIcon )
			m_pPenaltyIcon->Render( iXPos + 206, iYPos + 98 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "지금 나가면 도중이탈벌금" );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, "%d페소", g_MyInfo.GetCurPenaltyPeso() );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "가 징수됩니다." );
	}
	else if( COMPARE( m_dwStartTime, 1, 0xFFFFFFFF ) )
	{
		int iGapSec = (FRAMEGETTIME() - m_dwStartTime) / 1000;
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 33, 90, 162 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, "나가기 . . %d", max( 30 - iGapSec, 0 ) );

		if( m_pNonePenaltyIcon )
			m_pNonePenaltyIcon->Render( iXPos + 206, iYPos + 98 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "지금 나가면 페널티가 적용되지 않습니다." );
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 33, 90, 162 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, "나가기" );

		if( m_pNonePenaltyIcon )
			m_pNonePenaltyIcon->Render( iXPos + 206, iYPos + 98 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "지금 나가면 페널티가 적용되지 않습니다." );
	}

	if( Help::IsMonsterDungeonMode( m_pPlayStage->GetModeType() ) && !m_bObserberUser )
	{
		kPrinter.PrintFullText( iXPos + 206, iYPos + 144, TAT_CENTER );
		kPrinter.ClearList();
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_13, "도전포기" );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "를 하면 " );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, "보상획득" );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "이 가능합니다." );
		kPrinter.PrintFullText( iXPos + 206, iYPos + 164, TAT_CENTER );
	}
	else
	{
		kPrinter.PrintFullText( iXPos + 206, iYPos + 144, TAT_CENTER );
		kPrinter.ClearList();
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "이 방을 나가시겠습니까?" );
		kPrinter.PrintFullText( iXPos + 206, iYPos + 164, TAT_CENTER );
	}
}

//////////////////////////////////////////////////////////////////////////

ioBattleRoomExitMsgBoxBtn::ioBattleRoomExitMsgBoxBtn()
{
	m_pLiner		= NULL;
	m_pSawtoothIcon = NULL;
}

ioBattleRoomExitMsgBoxBtn::~ioBattleRoomExitMsgBoxBtn()
{
	SAFEDELETE( m_pLiner );
	SAFEDELETE( m_pSawtoothIcon );
}

void ioBattleRoomExitMsgBoxBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Liner" )
	{
		SAFEDELETE( m_pLiner );
		m_pLiner = pImage;
		if( m_pLiner )
		{
			m_pLiner->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLiner->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLiner->SetSize( m_pLiner->GetWidth(), 25 );
		}
	}
	else if( szType == "SawtoothIcon" )
	{
		SAFEDELETE( m_pSawtoothIcon );
		m_pSawtoothIcon = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ioBattleRoomExitMsgBoxBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pLiner )
	{
		m_pLiner->Render( iXPos, iYPos + 3 );
	}

	if( m_pSawtoothIcon )
	{
		m_pSawtoothIcon->SetScale( 0.7f );
		m_pSawtoothIcon->Render( iXPos + 1, iYPos + 2, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

//////////////////////////////////////////////////////////////////////////
ioBattleRoomExitPopup::ioBattleRoomExitPopup()
{
	m_pBottomBar	= NULL;

	m_pPlayStage	= NULL;
	m_bPenaltyExit	= false;
	m_iPopupType	= POPUP_TYPE_LEAVE;
}

ioBattleRoomExitPopup::~ioBattleRoomExitPopup()
{
	SAFEDELETE( m_pBottomBar );
}

void ioBattleRoomExitPopup::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomBar" )
	{
		SAFEDELETE( m_pBottomBar );
		m_pBottomBar = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ioBattleRoomExitPopup::SetPopupType( ioPlayStage *pPlayStage, int iPopupType, bool bPenaltyExit )
{
	m_iPopupType	= iPopupType;
	m_pPlayStage	= pPlayStage;
	m_bPenaltyExit	= bPenaltyExit;

	HideChildWnd( ID_LEAVE_RESERVE );
	HideChildWnd( ID_RESERVE_CANCEL );
	HideChildWnd( ID_SURRENDER );
	HideChildWnd( ID_NEVER_SURRENDER );

	switch( m_iPopupType )
	{
	case POPUP_TYPE_LEAVE:
		{
			SetSize( GetWidth(), LEAVE_HEIGHT );
			if( m_pPlayStage )
			{
				if( !m_pPlayStage->IsReservedExit() )
				{
					ioWnd *pButton = FindChildWnd( ID_LEAVE_RESERVE );
					if( pButton )
					{
						pButton->ShowWnd();
						pButton->SetWndPos( pButton->GetXPos(), GetHeight() - BTN_BOTTOM_Y );
					}
				}
				else
				{
					ioWnd *pButton = FindChildWnd( ID_RESERVE_CANCEL );
					if( pButton )
					{
						pButton->ShowWnd();
						pButton->SetWndPos( pButton->GetXPos(), GetHeight() - BTN_BOTTOM_Y );
					}
				}
			}
		}
		break;
	case POPUP_TYPE_SURRENDER:
		{
			if( m_bPenaltyExit )
				SetSize( GetWidth(), SURRENDER_HEIGHT );
			else
				SetSize( GetWidth(), LEAVE_HEIGHT );

			if( m_pPlayStage )
			{
				if( !m_pPlayStage->IsReservedExit() )
				{
					ioWnd *pButton = FindChildWnd( ID_SURRENDER );
					if( pButton )
					{
						pButton->ShowWnd();
						pButton->SetWndPos( pButton->GetXPos(), GetHeight() - BTN_BOTTOM_Y );
					}
				}
				else
				{
					ioWnd *pButton = FindChildWnd( ID_NEVER_SURRENDER );
					if( pButton )
					{
						pButton->ShowWnd();
						pButton->SetWndPos( pButton->GetXPos(), GetHeight() - BTN_BOTTOM_Y );
					}
				}
			}
		}
		break;
	}

	ioWnd *pButton = FindChildWnd( ID_QUICK_LEAVE );
	if( pButton )
	{
		pButton->SetWndPos( pButton->GetXPos(), GetHeight() - BTN_BOTTOM_Y );

		if( m_bPenaltyExit )
			pButton->SetTitleText( "즉시나가기(벌금)" );
		else
			pButton->SetTitleText( "즉시나가기" );
	}
	ShowWnd();
}

bool ioBattleRoomExitPopup::iwm_spacebar()
{
	ioButton *pBtnLeaveReserve = dynamic_cast<ioButton*>( FindChildWnd( ID_LEAVE_RESERVE ) );
	if ( pBtnLeaveReserve && pBtnLeaveReserve->IsShow() )
	{
		iwm_command( pBtnLeaveReserve, IOBN_BTNUP, 0 );
		return true;
	}

	ioButton *pBtnReserveCancel = dynamic_cast<ioButton*>( FindChildWnd( ID_RESERVE_CANCEL ) );
	if ( pBtnReserveCancel && pBtnReserveCancel->IsShow() )
	{
		iwm_command( pBtnReserveCancel, IOBN_BTNUP, 0 );
		return true;
	}
	
	ioButton *pBtnSurrender = dynamic_cast<ioButton*>( FindChildWnd( ID_SURRENDER ) );
	if ( pBtnSurrender && pBtnSurrender->IsShow() )
	{
		iwm_command( pBtnSurrender, IOBN_BTNUP, 0 );
		return true;
	}
	
	ioButton *pBtnNeverSurrender = dynamic_cast<ioButton*>( FindChildWnd( ID_NEVER_SURRENDER ) );
	if ( pBtnNeverSurrender && pBtnNeverSurrender->IsShow() )
	{
		iwm_command( pBtnNeverSurrender, IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

void ioBattleRoomExitPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_LEAVE_RESERVE:
	case ID_RESERVE_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( m_pPlayStage )
				{
					m_pPlayStage->SendOwnerDamageList();

					SP2Packet kPacket( CTPK_EXIT_ROOM );
					if( !m_pPlayStage->IsReservedExit() )
						kPacket << EXIT_ROOM_RESERVED;
					else
						kPacket << EXIT_ROOM_CANCELED;
					kPacket << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
					ExitLogByRegularTournament( dwID );
				}
			}
			HideWnd();
			g_GUIMgr.HideWnd( BATTLEROOM_EXIT_MSG_BOX );
		}
		break;
	case ID_SURRENDER:
	case ID_NEVER_SURRENDER:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if(m_pPlayStage && m_pPlayStage->GetPlayMode() && 
					m_pPlayStage->GetPlayMode()->GetModeState() == ioPlayMode::MS_PLAY)
				{
					m_pPlayStage->SendOwnerDamageList();

					SP2Packet kPacket( CTPK_EXIT_ROOM );
					if( !m_pPlayStage->IsReservedExit() )
						kPacket << EXIT_ROOM_RESERVED;
					else
						kPacket << EXIT_ROOM_CANCELED;
					kPacket << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
					ExitLogByRegularTournament( dwID );
				}
			}
			HideWnd();
			g_GUIMgr.HideWnd( BATTLEROOM_EXIT_MSG_BOX );
		}
		break;
	case ID_QUICK_LEAVE:
		if( cmd == IOBN_BTNUP )
		{
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( m_pPlayStage )
					m_pPlayStage->SendOwnerDamageList();

				// 페널티
				if( m_bPenaltyExit )
				{
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_PENALTY << g_MyInfo.GetCurPenaltyPeso() << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
					ExitLogByRegularTournament( dwID );
				}
				else
				{
					// 로비로 이동
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_WANT << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
					ExitLogByRegularTournament( dwID );
				}
			}
			HideWnd();
		}
		break;
	}
}

void ioBattleRoomExitPopup::ExitLogByRegularTournament( int iType )
{
	if( !g_BattleRoomMgr.IsTournamentModeType() )
		return;

	TournamentNode* pNode = g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
	if( !pNode )
		return;

	if( pNode->GetType() != TournamentNode::TYPE_REGULAR )
		return;

	if( !pNode->IsLogSend() )
		return;


	char szLog[MAX_PATH] = "";	
	switch( iType )
	{
		case ID_LEAVE_RESERVE:
			{
				sprintf( szLog, "[대회로그] 대회방 나가기(나가기 예약) - %s", g_MyInfo.GetPublicID().c_str() );
			}
			break;
		case ID_RESERVE_CANCEL:
			{
				sprintf( szLog, "[대회로그] 대회방 나가기(나가기 취소) - %s", g_MyInfo.GetPublicID().c_str() );
			}
			break;
		case ID_SURRENDER:
			{
				sprintf( szLog, "[대회로그] 대회방 나가기(항복) - %s", g_MyInfo.GetPublicID().c_str() );
			}
			break;
		case ID_NEVER_SURRENDER:
			{
				sprintf( szLog, "[대회로그] 대회방 나가기(항복) - %s", g_MyInfo.GetPublicID().c_str() );
			}
			break;
		case ID_QUICK_LEAVE:
			{
				sprintf( szLog, "[대회로그] 대회방 나가기(빠른 나가기) - %s", g_MyInfo.GetPublicID().c_str() );
			}
			break;
		default:
			{
				sprintf( szLog, "[대회로그] 대회방 나가기(알수 없는 나가기) - %s", g_MyInfo.GetPublicID().c_str() );
			}
			break;
		
	}

	SP2Packet kPacket2( LUPK_LOG );
	kPacket2 << "TournamentLog";  // 로그 파일 타입
	kPacket2 << szLog;
	kPacket2 << 4000;   // 오류번호:4000
	kPacket2 << false;  // write db
	P2PNetwork::SendLog(kPacket2);

	if( g_App.IsMeDeveloper() )
		LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
}

void ioBattleRoomExitPopup::iwm_show()
{
	SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ), ( ( Setting::Height() / 2 ) - ( GetHeight() / 2) ) - 20 );
}

void ioBattleRoomExitPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pBottomBar )
	{
		m_pBottomBar->Render( iXPos, iYPos + (GetHeight() - BOTTOM_GRAY_Y), UI_RENDER_MULTIPLY );
	}
	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 33, 90, 162 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 13, iYPos + 11, FONT_SIZE_17, "알림" );

	ioComplexStringPrinter kPrinter;

	int iCurrentY = 53;
	if( m_bPenaltyExit )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "지금 나가면 도중이탈벌금" );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, "%d페소", g_MyInfo.GetCurPenaltyPeso() );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "가 징수됩니다." );
		kPrinter.PrintFullText( iXPos + 156, iYPos + iCurrentY, TAT_CENTER );
		kPrinter.ClearList();

		iCurrentY += 20;
	}

	if( m_iPopupType == POPUP_TYPE_SURRENDER )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_13, "도전포기" );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "를 하면 " );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_13, "보상획득" );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "이 가능합니다." );
		kPrinter.PrintFullText( iXPos + 156, iYPos + iCurrentY, TAT_CENTER );
		kPrinter.ClearList();
	}
}

//////////////////////////////////////////////////////////////////////////
ioBattleRoomExitRoomListWnd::ioBattleRoomExitRoomListWnd()
{
	m_iCurPage = m_iMaxPage = 0;
	m_dwSearchTime			= 0;
	m_pModeHelpTooltip      = NULL;
}

ioBattleRoomExitRoomListWnd::~ioBattleRoomExitRoomListWnd()
{
	for(vBattleRoomInfo::iterator iter = m_vBattleRoomList.begin(); iter != m_vBattleRoomList.end(); ++iter)
		SAFEDELETE( *iter );
	m_vBattleRoomList.clear();
}

void ioBattleRoomExitRoomListWnd::StartSend( bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;

	SP2Packet kPacket( CTPK_JOIN_BATTLEROOM_LIST );
	kPacket << m_iCurPage << MAX_PARTY_LIST << g_BattleRoomMgr.GetQuickStartModeType() 
		    << g_BattleRoomMgr.GetQuickStartMinPlayer( g_BattleRoomMgr.GetQuickStartModeType() ) 
		    << g_BattleRoomMgr.GetQuickStartMaxPlayer( g_BattleRoomMgr.GetQuickStartModeType() )  
		    << g_BattleRoomMgr.IsQuickStartSameUser( g_BattleRoomMgr.GetQuickStartModeType() );
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

bool ioBattleRoomExitRoomListWnd::IsIndexToList( DWORD dwIndex )
{
	for(int i = 0;i < (int)m_vBattleRoomList.size();i++)
	{
		BattleRoomInfo *pInfo = m_vBattleRoomList[i];
		if( !pInfo )
			continue;
		if( pInfo->m_dwIndex == dwIndex )
			return true;
	}
	return false;
}

void ioBattleRoomExitRoomListWnd::ProcessSearch()
{
	if( GetParent() && GetParent()->IsShow() == false )
		return;

	if( FRAMEGETTIME() - m_dwSearchTime > PARTY_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void ioBattleRoomExitRoomListWnd::iwm_show()
{	
	m_iCurPage = m_iMaxPage = 0;
	for(vBattleRoomInfo::iterator iter = m_vBattleRoomList.begin(); iter != m_vBattleRoomList.end(); ++iter)
		SAFEDELETE( *iter );
	m_vBattleRoomList.clear();

	StartSend();

	for(int i = ID_LIST_1;i < ID_LIST_6 + 1;i++)
	{
		PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
		if( pBtn )
		{
			BattleRoomInfo kInfo;
			pBtn->SetBattleRoomInfo( kInfo );
			pBtn->AddWndStyle( IWS_EXACTIVE );
		}
	}	
}

void ioBattleRoomExitRoomListWnd::iwm_hide()
{
}

void ioBattleRoomExitRoomListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_PAGE_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_PAGE_RIGHT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)pWnd;
				if( pBtn->GetBattleRoomIndex() == 0 )
				{
					return;
				}
				
				PartyInfoWnd *pPartyInfoWnd = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
				if( pPartyInfoWnd )
				{
					pPartyInfoWnd->HideWnd();
					pPartyInfoWnd->SetBackupData( pBtn->GetBattleRoomIndex(), pBtn->IsPassword(), pBtn->GetBattleRoomState() );

					SP2Packet kPacket( CTPK_BATTLEROOM_JOIN_INFO );
					kPacket << pBtn->GetBattleRoomIndex();
					TCPNetwork::SendToServer( kPacket );					
				}
			}
		}
		break;
	case ID_REFRESH:
		if( cmd == IOBN_BTNUP )
		{
			Refresh();
		}
		break;
	}
}

void ioBattleRoomExitRoomListWnd::Refresh()
{
	SetChildWndStyleAdd( ID_REFRESH, IWS_EXACTIVE );	
	StartSend( false );
}

void ioBattleRoomExitRoomListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessSearch();		
	ProcessModeHelpTooltip();
}

void ioBattleRoomExitRoomListWnd::ProcessModeHelpTooltip()
{
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );
	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );

	for(int i = ID_LIST_1;i < ID_LIST_6 + 1;i++)
	{
		PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
		if( !pBtn ) continue;
		if( !pBtn->isIconOver() ) continue;

		if( pModeHelpTooltip )
		{
			pModeHelpTooltip->SetModeHelpTooltip(  this, g_ModeSelectInfo.GetSelectQuickStartModeTitle( pBtn->GetBattleRoomSelectIndex() ), 
														 g_ModeSelectInfo.GetQuickStartIndexToHelpIndex( pBtn->GetBattleRoomSelectIndex() ) );
		}	
		return;  // 리턴 시키지 않으면 툴팁 닫힘
	}

	if( pModeHelpTooltip )
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
}

void ioBattleRoomExitRoomListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 201, iYPos + 258, FONT_SIZE_12, "%d", m_iCurPage + 1 );
}

void ioBattleRoomExitRoomListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend();

	int iListSIze = m_vBattleRoomList.size();
	int iStartPos = 0;
	for(int i = ID_LIST_1;i < ID_LIST_6 + 1;i++,iStartPos++)
	{
		PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				pBtn->SetBattleRoomInfo( *m_vBattleRoomList[iStartPos] );
				pBtn->SetActive();
			}
			else
			{
				BattleRoomInfo kInfo;
				pBtn->SetBattleRoomInfo( kInfo );
				pBtn->SetInActive();
			}
		}
	}	
}

void ioBattleRoomExitRoomListWnd::SetPartyList( SP2Packet &rkPacket )
{	
	for(vBattleRoomInfo::iterator iter = m_vBattleRoomList.begin(); iter != m_vBattleRoomList.end(); ++iter)
		SAFEDELETE( *iter );
	m_vBattleRoomList.clear();

	PartyInfoWnd *pPartyInfoWnd = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
	if( !pPartyInfoWnd ) return;

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;		
	for(i = 0;i < iCurList;i++)
	{
		BattleRoomInfo *pInfo = new BattleRoomInfo;
		if( !pInfo )
			continue;
		rkPacket >> pInfo->m_dwIndex >> pInfo->m_iAverageLevel >> pInfo->m_szName >> pInfo->m_iCurJoiner >> pInfo->m_iCurPlayer
				 >> pInfo->m_iMaxPlayerBlue >> pInfo->m_iMaxPlayerRed >> pInfo->m_iMaxObserver >> pInfo->m_iModeSelectIndex >> pInfo->m_bPassword >> pInfo->m_iState
				 >> pInfo->m_bUseExtraOption >> pInfo->m_bNoChallenger;
	
		pInfo->m_iMaxPlayer = pInfo->m_iMaxPlayerBlue + pInfo->m_iMaxPlayerRed;
		pInfo->m_iNumber = ( m_iCurPage * MAX_PARTY_LIST ) + i;
		m_vBattleRoomList.push_back( pInfo );
		pPartyInfoWnd->UpdataBattleRoomState( pInfo->m_dwIndex, pInfo->m_iState );
	}
    
	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_PARTY_LIST;
	UpdateSetPage( m_iCurPage );
	SetChildWndStyleRemove( ID_REFRESH, IWS_EXACTIVE );
}