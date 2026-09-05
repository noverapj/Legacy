#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"

#include "../ioApplication.h"
#include "../ioMyInfo.h"
#include "../SlangUtil.h"
#include "../NetworkWrappingFunc.h"
#include "../WndID.h"
#include "../TextColorTable.h"
#include "../ChatMessageFunc.h"
#include "../ioPlayStage.h"
#include "../ioBattleRoomMgr.h"
#include "../EtcHelpFunc.h"

#include "ioMessageBox.h"
#include "ioSP2GUIManager.h"
#include "KickUserVoteWnd.h"
#include <strsafe.h>
#include "../Local/ioLocalManager.h"

KickVoteReasonEdit::KickVoteReasonEdit()
{

}

KickVoteReasonEdit::~KickVoteReasonEdit()
{

}

void KickVoteReasonEdit::OnProcess( float fTimePerSec )
{
	if( !m_bKeyFocus )	return;

	char szText[MAX_PATH] = "";
	sprintf( szText, "%s", g_InputBox.GetString() );	
	float fMaxLineSize = ioWnd::GetWidth() - 6; //6은 커서
	if( g_FontMgr.GetTextWidth( szText, m_TextStyle, m_fScale ) >= fMaxLineSize )
	{
		char szDst[MAX_PATH];
		memset( szDst, 0, sizeof( szDst ) );
		Help::StringCutFun( m_fScale, fMaxLineSize, m_TextStyle, szDst, sizeof(szDst), szText );
		sprintf( szText, "%s", szDst );
		g_InputBox.SetString( szText );
	}	
	SetText( g_InputBox.GetString() );

	if( m_pParent && m_pParent->IsShow() )
	{
		if( g_InputBox.IsEnter() )
		{
			KillKeyFocus();
			m_pParent->iwm_command( this, IOED_INPUT, true );
		}	
	}
}

void KickVoteReasonEdit::OnRender()
{
	int xPos, yPos;
	xPos = GetDerivedPosX();
	yPos = GetDerivedPosY();

	g_FontMgr.SetFont( m_FontName );
	g_FontMgr.SetAlignType( m_AlignType );
	g_FontMgr.SetTextStyle( m_TextStyle );
	g_FontMgr.SetTextColor( m_dwTextColor );
	g_FontMgr.SetBkColor( m_dwBkColor );

	switch( m_AlignType )
	{
	case TAT_CENTER:
		xPos += ( ( m_rcPos.right - m_rcPos.left ) / 2 );
		break;
	case TAT_RIGHT:
		xPos += ( m_rcPos.right - m_rcPos.left ) - 8;
		break;
	}

	char szText[MAX_PATH] = "";
	if( m_bKeyFocus )
		sprintf( szText, "%s_", m_szText );	
	else
		sprintf( szText, "%s", m_szText );	

	char szDst[MAX_PATH] = "";
	memset( szDst, 0, sizeof( szDst ) );
	int iLen = Help::StringCutFun( m_fScale, ioWnd::GetWidth(), m_TextStyle, szDst, sizeof(szDst), szText );		
	g_FontMgr.PrintText( xPos, yPos, m_fScale, szDst );
}

void KickVoteReasonEdit::iwm_create()
{
	m_dwStyle |= IWS_PROCESS_ALWAYS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
KickVoteReasonWnd::KickVoteReasonWnd() : m_pPlayStage( NULL )
{
	m_dwCurReasonBtn = ID_REASON_BTN1;
	m_eKickUserTeam  = TEAM_BLUE;
}

KickVoteReasonWnd::~KickVoteReasonWnd()
{
}

void KickVoteReasonWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void KickVoteReasonWnd::SetKickUserInfo( const ioHashString &rkName )
{
	m_szKickName = rkName;
	
	if( m_pPlayStage )
	{	
		m_eKickUserTeam = m_pPlayStage->GetBaseCharTeam( rkName );
	}

	ShowWnd();
}

void KickVoteReasonWnd::OnVoteStart()
{
	if( !m_pPlayStage ) return;
	if( m_szKickName.IsEmpty() ) return;
	if( m_szKickName == g_MyInfo.GetPublicID() )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_1, "", 0, 0, 0 );
		return;
	}

	if( g_App.IsAdminID( m_szKickName.c_str() ) )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_2, "", 0, 0, 0 );
		return;
	}

	// 유저가 동일한 룸에 있는지 확인
	if( !m_pPlayStage->IsBaseChar( m_szKickName ) )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_7, "", 0, 0, 0 );
		return;
	}

	// 차단 상태가 정상인지 확인
	if( g_MyInfo.GetBlockType() != BKT_NORMAL )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_3, "", 0, 0, 0 );
		return;
	}	

	// 현재 투표가 진행중인지 확인
	if( g_GUIMgr.IsShow( KICK_VOTE_PROGRESS_WND ) )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_5, "", 0, 0, 0 );
		return;
	}

	// 1회 투표 제안을 했던 유저 처리
	if( m_pPlayStage->GetKickOutVoteProposal() != 0 )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_6, "", 0, 0, 0 );
		return;
	}

	// 보스 유저는 신고 불가
	if( m_pPlayStage->GetModeType() == MT_BOSS )
	{
		if( m_pPlayStage->GetBaseCharTeam( m_szKickName ) == TEAM_BLUE )
		{
			KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_13, "", 0, 0, 0 );
			return;
		}
	}

	//
	int iIndex = m_dwCurReasonBtn - ID_REASON_BTN1;
	if( !COMPARE( iIndex, 0, 4 ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( m_szKickReason.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	// 서버로 패킷 전송
	SP2Packet kPacket( CTPK_USER_KICK_VOTE );
	kPacket << USER_KICK_VOTE_PROPOSAL << m_szKickName << m_szKickReason;       
	TCPNetwork::SendToServer( kPacket );

	HideWnd();
}

void KickVoteReasonWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_VOTE_START:
		if( cmd == IOBN_BTNUP )
		{
			OnVoteStart();
		}
		break;
	case ID_REASON_BTN1:
	case ID_REASON_BTN2:
	case ID_REASON_BTN3:
	case ID_REASON_BTN4:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckButton( dwID );
		}
		break;
	}
}

void KickVoteReasonWnd::iwm_show()
{
	CheckButton( ID_REASON_BTN1 );
}

void KickVoteReasonWnd::iwm_hide()
{
}

bool KickVoteReasonWnd::iwm_esc()
{
	return ioWnd::iwm_esc();
}

void KickVoteReasonWnd::CheckButton( DWORD dwID )
{
	m_dwCurReasonBtn = dwID;

	CheckRadioButton( ID_REASON_BTN1, ID_REASON_BTN4, m_dwCurReasonBtn );

	ioWnd *pReasonBtn = FindChildWnd( m_dwCurReasonBtn );
	if( pReasonBtn )
	{
		ioUITitle *pTitle = pReasonBtn->GetTitle();
		if( pTitle )
		{
			pTitle->GetFirstText( m_szKickReason );
		}
	}
}

//void KickVoteReasonWnd::OnProcess( float fTimePerSec )
//{
//	if( !IsShow() )
//		return;
//}

void KickVoteReasonWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_eKickUserTeam == TEAM_BLUE )
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	else
		kPrinter.SetTextColor( TCT_DEFAULT_RED );		

	char szAddComma[MAX_PATH];
	sprintf( szAddComma, ".." );
	int iAddCommaSize = g_FontMgr.GetTextWidth( szAddComma, TS_NORMAL, FONT_SIZE_13 );

	char szDst[MAX_PATH];
	memset( szDst, 0, sizeof( szDst ) );
	int iLen = Help::StringCutFun( FONT_SIZE_13, (194.0f - iAddCommaSize), TS_NORMAL, szDst, sizeof(szDst), m_szKickName.c_str() );
	if( iLen < (int)strlen( m_szKickName.c_str() ) )
		strcat( szDst, szAddComma );

	int iCurModeType = m_pPlayStage->GetModeType();
	switch( iCurModeType )
	{
	case MT_FIRE_TEMPLE:
	case MT_DARK_XMAS:
	case MT_TOWER_DEFENSE:
	case MT_SYMBOL:
	case MT_KING:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_FOOTBALL:
	case MT_HEROMATCH:
	case MT_CATCH_RUNNINGMAN:
	case MT_DOUBLE_CROWN:
	case MT_FACTORY:
	case MT_RAID:
	case MT_FLAG_CAPTURE:
	case MT_ARENA:
	case MT_FARMING:
		{
			if( m_eKickUserTeam == TEAM_BLUE )
				kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), szDst );
			else
				kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), szDst );
		}
		break;
	default:
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), szDst );
		break;
	}

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 53, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 71, TAT_CENTER );
	kPrinter.ClearList();
}

////////////////////////////////////////////////////////////////////////////////////////////////
KickVoteProgressWnd::KickVoteProgressWnd()
{
	m_pPlayStage= NULL;

	memset( m_szMainMent, 0, sizeof( m_szMainMent ) );
	m_iVoteType = 0;
	m_dwVoteTime   = 20000;
	m_dwVoteCurTime= 0;

	m_eProposalTeam = m_eKickUserTeam = TEAM_NONE;
}

KickVoteProgressWnd::~KickVoteProgressWnd()
{
}

void KickVoteProgressWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void KickVoteProgressWnd::SetKickVoteProgress( const ioHashString &rkProposalName, const ioHashString &rkKickUserName, const ioHashString &rkKickUserReason, DWORD dwProcessTime )
{
	m_szProposalName   = rkProposalName;
	m_szKickUserName   = rkKickUserName;
	m_szKickUserReason = rkKickUserReason;
	m_dwVoteTime       = dwProcessTime;

	SafeSprintf( m_szMainMent, sizeof( m_szMainMent ), STR(1), m_szKickUserName.c_str(), m_szProposalName.c_str() );

	ShowWnd();

	//
	m_eProposalTeam = m_eKickUserTeam = TEAM_NONE;
	if( m_pPlayStage )
	{
		m_eProposalTeam = m_pPlayStage->GetBaseCharTeam( m_szProposalName );
		m_eKickUserTeam = m_pPlayStage->GetBaseCharTeam( m_szKickUserName );
	}
}

void KickVoteProgressWnd::OnVote( int iVoteType )
{
	m_iVoteType = iVoteType;
	HideWnd();
}

void KickVoteProgressWnd::iwm_show()
{
	m_dwVoteCurTime = FRAMEGETTIME();
	m_iVoteType = 0;
}

void KickVoteProgressWnd::iwm_hide()
{
	if( m_iVoteType != 0 )
	{
		// 투표 전송 > 자신에게만 보이게 ‘대상자’님에 대한 강제퇴장 투표에 ‘찬성/반대/기권’하셨습니다. 메세지 출력 
		switch( m_iVoteType )
		{
		case USER_KICK_VOTE_APPROVE:
		case USER_KICK_VOTE_OPPOSITION:
		case USER_KICK_VOTE_ABSTENTION:
			KickVoteResultHelp( m_pPlayStage, m_iVoteType, m_szKickUserName, 0, 0, 0 );
			break;
		default:
			return;
		}		

		SP2Packet kPacket( CTPK_USER_KICK_VOTE );
		kPacket << m_iVoteType << m_szKickUserName;
		TCPNetwork::SendToServer( kPacket );
	}
	m_iVoteType = 0;
}

bool KickVoteProgressWnd::iwm_esc()
{
	OnVote( USER_KICK_VOTE_OPPOSITION );
	return true;
}

bool KickVoteProgressWnd::iwm_spacebar()
{
	if( !IsShow() ) return false;

	OnVote( USER_KICK_VOTE_APPROVE );
	return true;
}

void KickVoteProgressWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_APPROVE:
		if( cmd == IOBN_BTNUP )
		{
			OnVote( USER_KICK_VOTE_APPROVE );
		}
		break;
	case ID_OPPOSITION:
		if( cmd == IOBN_BTNUP )
		{
			OnVote( USER_KICK_VOTE_OPPOSITION );
		}
		break;
	}
}

void KickVoteProgressWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwVoteCurTime > m_dwVoteTime )
	{
		OnVote( USER_KICK_VOTE_ABSTENTION );
	}
	else
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwVoteCurTime;
		char szTitle[MAX_PATH];
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), max( 0, (int)m_dwVoteTime - (int)dwGap ) / 1000 );
		SetTitleText( szTitle );
	}
}

void KickVoteProgressWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 15, iYPos + 41, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 15, iYPos + 61, FONT_SIZE_12, STR(2) );

	// 대상자
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	DWORD dwTeamColor = 0xFF3A3A3A;
	if( m_eKickUserTeam == TEAM_BLUE )
	{
		dwTeamColor = 0xFF1646B2;
		kPrinter.SetTextColor( dwTeamColor );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	}
	else if( m_eKickUserTeam == TEAM_RED )
	{
		dwTeamColor = 0xFFB91600;
		kPrinter.SetTextColor( dwTeamColor );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	}
	char szCutName[MAX_PATH] = "";
	memset( szCutName, 0, sizeof( szCutName ) );
	Help::StringCutFun( FONT_SIZE_13, 130.0f, TS_NORMAL, szCutName, sizeof( szCutName ), m_szKickUserName.c_str() );
	kPrinter.SetTextColor( dwTeamColor );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5), szCutName );

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "- " );

	kPrinter.SetTextColor( dwTeamColor );
	kPrinter.AddTextPiece( FONT_SIZE_13, m_szKickUserReason.c_str() );
	kPrinter.PrintFullText( iXPos + 62, iYPos + 41, TAT_LEFT );
	kPrinter.ClearList();

	// 신고자
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_eProposalTeam == TEAM_BLUE )
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	else if( m_eProposalTeam == TEAM_RED )
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );

	memset( szCutName, 0, sizeof( szCutName ) );
	Help::StringCutFun( FONT_SIZE_13, 230.0f, TS_NORMAL, szCutName, sizeof( szCutName ), m_szProposalName.c_str() );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8), szCutName );
	kPrinter.PrintFullText( iXPos + 62, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
void KickVoteResultHelp( ioPlayStage *pPlayStage, int iVoteType, const ioHashString &rkTargetName, int iValue1, int iValue2, int iValue3 )
{
	if( !pPlayStage ) return;

	switch( iVoteType )
	{
	case USER_KICK_VOTE_PROPOSAL:  //투표 제안
		{
			if( rkTargetName != g_MyInfo.GetPublicID() )
			{
				ModeType eModeType = pPlayStage->GetModeType();
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				if( eModeType != MT_BOSS && pPlayStage->GetBaseCharTeam( rkTargetName ) == TEAM_BLUE )
                    kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
				else
					kPrinter.SetTextColor( TCT_DEFAULT_RED );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(35), rkTargetName.c_str() );
				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2), iValue1 );
				g_ChatMgr.SetChatComplexString( STR(1), kPrinter );
				kPrinter.ClearList();

				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), iValue1 );
				g_ChatMgr.SetChatComplexString( STR(4), kPrinter );
			}			
		}
		break;
	case USER_KICK_VOTE_ADOPTION:       //투표 가결
		{
			if( rkTargetName != g_MyInfo.GetPublicID() )			
			{
				ModeType eModeType = pPlayStage->GetModeType();
				switch( eModeType )
				{
				case MT_TRAINING:
				case MT_SURVIVAL:
				case MT_BOSS:
				case MT_HEADQUARTERS:
				case MT_HOUSE:
				case MT_FIGHT_CLUB:
				case MT_SHUFFLE_BONUS:
					{
						// Free For All
						ioComplexStringPrinter kPrinter;
						g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(6) );
						kPrinter.SetTextColor( TCT_DEFAULT_RED );
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(36), rkTargetName.c_str() );
						kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5) );
						g_ChatMgr.SetChatComplexString( STR(8), kPrinter );
					}
					break;
				default:
					{
						// Team Mode
						ioComplexStringPrinter kPrinter;
						g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(10) );
						if( pPlayStage->GetBaseCharTeam( rkTargetName ) == TEAM_BLUE )
							kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
						else
							kPrinter.SetTextColor( TCT_DEFAULT_RED );
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(37), rkTargetName.c_str() );
						kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
						kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7) );
						g_ChatMgr.SetChatComplexString( STR(12), kPrinter );
					}
					break;
				}
			}
		}
		break;
	case USER_KICK_VOTE_REJECTION:       //투표 부결
		{
			ModeType eModeType = pPlayStage->GetModeType();

			switch( eModeType )
			{
			case MT_TRAINING:
			case MT_SURVIVAL:
			case MT_BOSS:
			case MT_HEADQUARTERS:
			case MT_HOUSE:
			case MT_FIGHT_CLUB:
			case MT_SHUFFLE_BONUS:
				{
					// Free For All
					ioComplexStringPrinter kPrinter;
					g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(14) );
					kPrinter.SetTextColor( TCT_DEFAULT_RED );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(38), rkTargetName.c_str() );
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(9) );
					g_ChatMgr.SetChatComplexString( STR(16), kPrinter );
				}
				break;
			default:
				{
					// Team Mode
					ioComplexStringPrinter kPrinter;
					g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(18) );
					if( pPlayStage->GetBaseCharTeam( rkTargetName ) == TEAM_BLUE )
						kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
					else
						kPrinter.SetTextColor( TCT_DEFAULT_RED );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(39), rkTargetName.c_str() );
					kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
					kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(11) );
					g_ChatMgr.SetChatComplexString( STR(20), kPrinter );
				}
				break;
			}
		}
		break;
	case USER_KICK_VOTE_CANCEL_PROPOSAL:       //투표 취소 : 제안자 이탈
		{
			ModeType eModeType = pPlayStage->GetModeType();
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(45) );
			if( eModeType != MT_BOSS && (TeamType)iValue1 == TEAM_BLUE )
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			else
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(40), rkTargetName.c_str() );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(13) );
			g_ChatMgr.SetChatComplexString( STR(46), kPrinter );
		}
		break;
	case USER_KICK_VOTE_CANCEL_TARGET:         //투표 취소 : 대상자 이탈
		{
			ModeType eModeType = pPlayStage->GetModeType();
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(47) );
			if( eModeType != MT_BOSS && (TeamType)iValue1 == TEAM_BLUE )
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			else
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(41), rkTargetName.c_str() );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(15) );
			g_ChatMgr.SetChatComplexString( STR(48), kPrinter );
		}		
		break;
	case USER_KICK_VOTE_APPROVE:
		{
			ModeType eModeType = pPlayStage->GetModeType();
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
			if( eModeType != MT_BOSS && pPlayStage->GetBaseCharTeam( rkTargetName ) == TEAM_BLUE )
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			else
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(42), rkTargetName.c_str() );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(17) );
			g_ChatMgr.SetChatComplexString( STR(49), kPrinter );
		}		
		break;
	case USER_KICK_VOTE_OPPOSITION:
		{
			ModeType eModeType = pPlayStage->GetModeType();
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );;
			if( eModeType != MT_BOSS && pPlayStage->GetBaseCharTeam( rkTargetName ) == TEAM_BLUE )
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			else
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(43), rkTargetName.c_str() );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(19) );
			g_ChatMgr.SetChatComplexString( STR(50), kPrinter );
		}		
		break;
	case USER_KICK_VOTE_ABSTENTION:
		{
			ModeType eModeType = pPlayStage->GetModeType();
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
			if( eModeType != MT_BOSS && pPlayStage->GetBaseCharTeam( rkTargetName ) == TEAM_BLUE )
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			else
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(44), rkTargetName.c_str() );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(21) );
			g_ChatMgr.SetChatComplexString( STR(51), kPrinter );
		}		
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_1:       //투표 제안 자신의 닉네임
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(22) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_2:       //투표 제안 개발자 & 마스터 & 매니저
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(23) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_3:       //투표 제안 명성이 90% 미만
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(24) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_4:       //투표 제안 관전자는 투표 제안 X
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(25) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_5:       //투표 제안 현재 투표가 진행중
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(26) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_6:       //투표 제안 1회 투표 제안을 했던 유저
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(52) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_7:       //투표 제안 대상자가 동일한 룸에 없음
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(28) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_8:       //투표 제안 대상자가 MC
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(29) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_9:      //투표 제안 강퇴 건의 시간 지남( 싱글 )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(30), iValue1 );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_10:    //투표 제안 강퇴 건의 라운드 지남( 팀전 )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(31), iValue1 );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_11:     //투표 제안 투표 참여자 부족( 싱글 )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(32) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_12:       //투표 제안 투표 참여자 부족( 팀전 )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(33) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_13:       //투표 제안 보스유저 강퇴 불가( 보스레이드모드 )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(34) );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_14:       //오늘의 모드 투표 제안시간 지남
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "강퇴 가능 시간이 지나갔습니다." );
		break;
	}
}