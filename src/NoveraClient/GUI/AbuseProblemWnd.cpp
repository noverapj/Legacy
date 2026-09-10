

#include "StdAfx.h"

#include "AbuseProblemWnd.h"

#include "../ioShuffleRoomMgr.h"
#include "../ioRankBattleManager.h"

AbuseProblemWnd::AbuseProblemWnd()
{
	m_AbuseType = AT_KEY;
	m_dwQuizStartTime = 0;
	m_dwQuizLimitTime = 1000;

	m_iFirstOperand  = 0;
	m_iSecondOperand = 1;
	m_Operator = QP_DIVIDE;

	m_iCurQuizChance = 0;
	m_iMaxQuizChance = 1;

	m_iPreQuizFirst  = 0;
	m_iPreQuizSecond = 0;
	m_iSendedAnwser  = 0;

	m_bNowOpenSpace = false;
	m_pPreEdit      = NULL;

	m_eModeType = MT_NONE;
}

AbuseProblemWnd::~AbuseProblemWnd()
{
}

void AbuseProblemWnd::SetAbuseAnnounce( SP2Packet &rkPacket, bool bOpenSpace, ModeType eModeType )
{
	int iTempBuf;
	rkPacket >> iTempBuf;
	m_AbuseType = (AbuseType)iTempBuf;

	rkPacket >> m_iMaxQuizChance;
	rkPacket >> m_dwQuizLimitTime;

	rkPacket >> m_iFirstOperand;
	rkPacket >> m_iSecondOperand;
	rkPacket >> iTempBuf;
	m_Operator = (QuizOperator)iTempBuf;

	m_dwQuizStartTime = REALGETTIME();
	m_iCurQuizChance  = 0;
	m_iSendedAnwser   = 0;
	m_bNowOpenSpace   = bOpenSpace;
	m_eModeType		  = eModeType;

	g_MyInfo.ClearKickedInfo();

	ShowWnd();
	SetAnswerEditFocus();
	g_SoundMgr.PlaySound( "interface\\abuze_qa_alert_00.wav" );
}

bool AbuseProblemWnd::SetAbuseQuiz( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;

	char szKickSubInfo[MAX_PATH];

	switch( iResult )
	{
	case ID_QUIZ_OK:
		m_dwQuizStartTime = 0;
		g_AbuseMgr.Initialize();         // 어뷰즈 초기화
		return true;
	case ID_QUIZ_RETRY:
		{
			m_iPreQuizFirst  = m_iFirstOperand;
			m_iPreQuizSecond = m_iSecondOperand;

			rkPacket >> m_iSendedAnwser;

			rkPacket >> m_iCurQuizChance;
			rkPacket >> m_iFirstOperand;
			rkPacket >> m_iSecondOperand;

			int iTemp;
			rkPacket >> iTemp;
			m_Operator = (QuizOperator)iTemp;

			ShowWnd();
			SetAnswerEditFocus();
		}
		return true;
	case ID_QUIZ_KICK:
		rkPacket >> m_iSendedAnwser;

		m_dwQuizStartTime = 0;

		SafeSprintf( szKickSubInfo, sizeof( szKickSubInfo ), STR(1),
								(int)m_iMaxQuizChance,
								(int)m_iFirstOperand,
								(int)m_iSecondOperand,
								(int)( m_iFirstOperand + m_iSecondOperand ),
								m_iSendedAnwser );

		g_MyInfo.SetKickedInfo( szKickSubInfo );

		if( m_AbuseType == AT_DAMAGE )
			g_App.KickToHeadQuater( EXIT_ROOM_DAMAGE_ABUSE );
		else
			g_App.KickToHeadQuater( EXIT_ROOM_KEY_ABUSE, m_bNowOpenSpace );
		return false;
	}

	return false;
}

void AbuseProblemWnd::SetAnswerEditFocus()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
	}
}

void AbuseProblemWnd::SendQuizAnswer()
{
	if( m_dwQuizStartTime == 0 )
		return;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( !pEdit )	return;
	if( strlen( pEdit->GetText() ) <= 0 )	return;

	m_iSendedAnwser = atoi( pEdit->GetText() );
	pEdit->KillKeyFocus();
	DWORD dwPacketID = 0;

	if(m_AbuseType == AT_DAMAGE || m_AbuseType == AT_KEY)
	{
		SP2Packet kPacket( CTPK_ABUSE_QUIZ );
		kPacket << ID_QUIZ_ANSWER;
		kPacket << m_iSendedAnwser;
		TCPNetwork::SendToServer( kPacket );
	}
	else if(m_AbuseType == AT_MACRO)
	{
		SP2Packet kPacket( CTPK_MACRO_QUIZ );
		kPacket << ID_QUIZ_ANSWER;
		kPacket << m_iSendedAnwser;
		TCPNetwork::SendToServer( kPacket );
	}
	
	HideWnd();
}

void AbuseProblemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			SendQuizAnswer();
		}
		break;
	case ID_ANSWER_EDIT:
		if( cmd == IOED_INPUT && param == 1 )
		{
			SendQuizAnswer();
		}
		break;
	}
}

void AbuseProblemWnd::OnProcess( float fTimePerSec )
{
	// 중간에 메세지 전송으로 인해 숨겨지더라도 시간체크는 계속 한다.
	if( m_dwQuizStartTime == 0 )
		return;

	DWORD dwPastTime = REALGETTIME() - m_dwQuizStartTime;
	if( dwPastTime < m_dwQuizLimitTime )
	{
		char szBuf[MAX_PATH];
		int iRemainSecond = ( m_dwQuizLimitTime - dwPastTime ) / 1000;
		if(m_AbuseType == AT_DAMAGE || m_AbuseType == AT_KEY)
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iRemainSecond );
		else if(m_AbuseType == AT_MACRO)
			SafeSprintf( szBuf, sizeof( szBuf ), STR(2), iRemainSecond );
		SetTitleText( szBuf );
	}
	else
	{
		SP2Packet kPacket( CTPK_ABUSE_QUIZ );
		kPacket << ID_QUIZ_TIMEOVER;
		kPacket << m_AbuseType;
		TCPNetwork::SendToServer( kPacket );

		m_dwQuizStartTime = 0;
		HideWnd();

		if( m_AbuseType == AT_DAMAGE )
			g_App.KickToHeadQuater( EXIT_ROOM_DAMAGE_ABUSE );
		else
			g_App.KickToHeadQuater( EXIT_ROOM_KEY_ABUSE, m_bNowOpenSpace );

		g_MyInfo.SetKickedInfo( STR(3) );
	}
}

void AbuseProblemWnd::OnRender()
{
	ioWnd::OnRender();

	int xPos = GetDerivedPosX();
	int yPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_iCurQuizChance == 0 || m_iMaxQuizChance == 1 ) // 최초
	{
		char szBuf[MAX_PATH];
		if(m_AbuseType == AT_DAMAGE || m_AbuseType == AT_KEY)
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1) );
		else if(m_AbuseType == AT_MACRO)
			SafeSprintf( szBuf, sizeof( szBuf ), STR(2) );
		g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, szBuf );
	}
	else if( m_iCurQuizChance + 1 >= m_iMaxQuizChance )  // 마지막
	{
		g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(3),
													   (int)m_iPreQuizFirst,
													   (int)m_iPreQuizSecond,
													   (int)(m_iPreQuizFirst + m_iPreQuizSecond),
													   m_iSendedAnwser );
	}
	else // 중간
	{
		g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(4),
													   (int)m_iPreQuizFirst,
													   (int)m_iPreQuizSecond,
													   (int)(m_iPreQuizFirst + m_iPreQuizSecond),
													   m_iSendedAnwser );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_RIGHT );

	switch( m_Operator )
	{
	case QP_ADD:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(9), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	case QP_MINUS:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(10), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	case QP_MULTIPLY:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(11), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	case QP_DIVIDE:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(8), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	}
}

void AbuseProblemWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
}

void AbuseProblemWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->KillKeyFocus();
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			m_pPreEdit->SetKeyFocus();
		}

		m_pPreEdit = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
LobbyLockProblemWnd::LobbyLockProblemWnd()
{
	m_dwLockType = LT_MACRO;

	m_iFirstOperand  = 0;
	m_iSecondOperand = 0;

    m_pPreEdit = NULL;
}

LobbyLockProblemWnd::~LobbyLockProblemWnd()
{	
	m_pPreEdit = NULL;
}

void LobbyLockProblemWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}

	if( g_MyInfo.GetLobbyLockTypeMsgBoxID() > 0 )
	{
		g_GUIMgr.ShowReserveMsgBox( g_MyInfo.GetLobbyLockTypeMsgBoxID() );
	}
}

void LobbyLockProblemWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->KillKeyFocus();
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			m_pPreEdit->SetKeyFocus();
		}

		m_pPreEdit = NULL;
	}
}

void LobbyLockProblemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			CheckAnswer();
		}
		break;
	case ID_ANSWER_EDIT:
		if( cmd == IOED_INPUT && param == 1 )
		{
			CheckAnswer();
		}
		break;
	}
}

void LobbyLockProblemWnd::CheckAnswer()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( !pEdit ) return;
	if( strlen( pEdit->GetText() ) <= 0 )	return;

	int iAnswer = atoi( pEdit->GetText() );
	pEdit->KillKeyFocus();

	if( m_iFirstOperand + m_iSecondOperand == iAnswer )
	{
		g_MyInfo.SetLobbyLockType( LT_NONE, 0 );
		g_MyInfo.SetBattleLockType( LT_NONE );

		HideWnd();
	}
	else
	{
		int iRightAnswer = m_iFirstOperand + m_iSecondOperand;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1),
										  (int)m_iFirstOperand, (int)m_iSecondOperand, iRightAnswer, iAnswer );
		// 문제 다시 출제
		m_iFirstOperand = rand() % 99 + 1;
		m_iSecondOperand= rand() % 99 + 1;

		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void LobbyLockProblemWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		DWORD dwLockType = g_MyInfo.GetLobbyLockType();
		if( dwLockType == LT_NONE )
			return;
		else
		{
			SetLockType( dwLockType );
		}
	}
}

void LobbyLockProblemWnd::OnRender()
{
	ioWnd::OnRender();

	int xPos = GetDerivedPosX();
	int yPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(2), (int)m_iFirstOperand, (int)m_iSecondOperand );
}

void LobbyLockProblemWnd::SetLockType( DWORD dwLockType )
{
	m_dwLockType    = dwLockType;
	m_iFirstOperand = rand() % 99 + 1;
	m_iSecondOperand= rand() % 99 + 1;

	ShowWnd();
}
//////////////////////////////////////////////////////////////////////////
BattleLockProblemWnd::BattleLockProblemWnd()
{
	m_dwLockType = LT_MACRO;

	m_iFirstOperand  = 0;
	m_iSecondOperand = 0;

	m_iCurQuizChance = 0;
	m_iMaxQuizChance = 3;
	m_dwQuizStartTime= 0;
	m_dwQuizLimitTime= 20000;

	m_pPreEdit = NULL;
}

BattleLockProblemWnd::~BattleLockProblemWnd()
{	
	m_pPreEdit = NULL;
}

void BattleLockProblemWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}

	m_iCurQuizChance = 0;
	m_dwQuizStartTime= FRAMEGETTIME();

	char szBuf[MAX_PATH];
	int iRemainSecond = m_dwQuizLimitTime / 1000;
	SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iRemainSecond );
	SetTitleText( szBuf );
}

void BattleLockProblemWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->KillKeyFocus();
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			m_pPreEdit->SetKeyFocus();
		}

		m_pPreEdit = NULL;
	}
}

void BattleLockProblemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			CheckAnswer();
		}
		break;
	case ID_ANSWER_EDIT:
		if( cmd == IOED_INPUT && param == 1 )
		{
			CheckAnswer();
		}
		break;
	}
}

void BattleLockProblemWnd::CheckAnswer()
{
	if( m_dwQuizStartTime == 0 ) return;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( !pEdit ) return;
	if( strlen( pEdit->GetText() ) <= 0 )	return;
	if( g_App.IsMouseBusy() ) return;

	int iAnswer = atoi( pEdit->GetText() );
	pEdit->KillKeyFocus();

	if( m_iFirstOperand + m_iSecondOperand == iAnswer )
	{
		g_MyInfo.SetBattleLockType( LT_NONE );
		g_MyInfo.SetLobbyLockType( LT_NONE, 0 );

		HideWnd();
	}
	else
	{
		m_iCurQuizChance++;
		if( m_iCurQuizChance >= m_iMaxQuizChance )
		{
			//로비로 이동
			g_App.KickToHeadQuater( EXIT_ROOM_MACRO_OUT, false );
			m_dwQuizStartTime = 0;
		}
		else
		{
			int iRightAnswer = m_iFirstOperand + m_iSecondOperand;
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1),
				(int)m_iFirstOperand, (int)m_iSecondOperand, iRightAnswer, iAnswer );
			// 문제 다시 출제
			m_iFirstOperand = rand() % 8 + 1;
			m_iSecondOperand= rand() % 99 + 1;

			pEdit->ClearString();
			pEdit->SetKeyFocus();
			g_InputBox.SetString( "" );
		}
	}
}

void BattleLockProblemWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			DWORD dwLockType = g_MyInfo.GetBattleLockType();
			if( dwLockType == LT_NONE )
				return;
			else
			{
				if( dwLockType != LT_MACRO_EXTENSION && g_BattleRoomMgr.IsBattleModePlaying() ) 
					return;

				SetLockType( dwLockType );
			}
		}
		else if( g_ShuffleRoomMgr.IsShuffleRoom() || g_RankBattleMgr.IsRankBattlePlaying() )
		{
			DWORD dwLockType = g_MyInfo.GetBattleLockType();
			if( dwLockType == LT_NONE )
				return;
			else
			{
				if( dwLockType != LT_MACRO_EXTENSION ) 
					return;

				SetLockType( dwLockType );
			}
		}
		else
			return;
	}

	if( m_dwQuizStartTime != 0 )
	{
		DWORD dwPastTime = REALGETTIME() - m_dwQuizStartTime;
		if( dwPastTime < m_dwQuizLimitTime )
		{
			char szBuf[MAX_PATH];
			int iRemainSecond = ( m_dwQuizLimitTime - dwPastTime ) / 1000;
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), max( 0, iRemainSecond ) );
			SetTitleText( szBuf );
		}
		else
		{			
			//로비로 이동
			g_App.KickToHeadQuater( EXIT_ROOM_MACRO_OUT, false );
			m_dwQuizStartTime = 0;
		}
	}
}

void BattleLockProblemWnd::OnRender()
{
	ioWnd::OnRender();

	int xPos = GetDerivedPosX();
	int yPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(2), (int)m_iFirstOperand, (int)m_iSecondOperand );
}

void BattleLockProblemWnd::SetLockType( DWORD dwLockType )
{
	m_dwLockType    = dwLockType;
	m_iFirstOperand = rand() % 8 + 1;
	m_iSecondOperand= rand() % 99 + 1;

	g_MyInfo.SetBattleLockType( LT_MACRO_EXTENSION );
	g_MyInfo.SetLobbyLockType( m_dwLockType, MB_RESERVE_MACRO_OUT );
	ShowWnd();
}