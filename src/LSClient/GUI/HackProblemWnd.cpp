

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"

#include "../TextColorTable.h"
#include "../SP2Packet.h"
#include "../NetworkWrappingFunc.h"
#include "../ioApplication.h"
#include "../ioComplexStringPrinter.h"
#include "../ioMyInfo.h"
#include "../EtcHelpFunc.h"

#include "ioMessageBox.h"
#include "HackProblemWnd.h"
#include "ioSP2GUIManager.h"
#include <strsafe.h>

HackProblemWnd::HackProblemWnd()
{
	m_QuizType = QT_SPEED;
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
	
	m_pPreEdit       = NULL;
}

HackProblemWnd::~HackProblemWnd()
{
}

void HackProblemWnd::SetHackAnnounce( SP2Packet &rkPacket )
{
	int iTempBuf;
	rkPacket >> iTempBuf;
	m_QuizType = (QuizType)iTempBuf;

	rkPacket >> m_iMaxQuizChance;
	rkPacket >> m_dwQuizLimitTime;

	rkPacket >> m_iFirstOperand;
	rkPacket >> m_iSecondOperand;
	rkPacket >> iTempBuf;
	m_Operator = (QuizOperator)iTempBuf;

	m_dwQuizStartTime = REALGETTIME();
	m_iCurQuizChance  = 0;

	m_iSendedAnwser = 0;
	g_MyInfo.ClearKickedInfo();

	ShowWnd();
	SetAnswerEditFocus();

	g_SoundMgr.PlaySound( "interface\\abuze_qa_alert_00.wav" );
}

bool HackProblemWnd::SetHackQuiz( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;

	int iTemp = 0;
	char szKickSubInfo[MAX_PATH];

	switch( iResult )
	{
	case ID_QUIZ_OK:
		m_dwQuizStartTime = 0;
		return true;
	case ID_QUIZ_RETRY:
		m_iPreQuizFirst  = m_iFirstOperand;
		m_iPreQuizSecond = m_iSecondOperand;

		rkPacket >> m_iSendedAnwser;

		rkPacket >> m_iCurQuizChance;
		rkPacket >> m_iFirstOperand;
		rkPacket >> m_iSecondOperand;
		rkPacket >> iTemp;
		m_Operator = (QuizOperator)iTemp;

		ShowWnd();
		SetAnswerEditFocus();
		return true;
	case ID_QUIZ_KICK:
		m_dwQuizStartTime = 0;

		rkPacket >> m_iSendedAnwser;

		SafeSprintf( szKickSubInfo, sizeof( szKickSubInfo ), STR(1),
								(int)m_iMaxQuizChance,
								(int)m_iFirstOperand,
								(int)m_iSecondOperand,
								(int)(m_iFirstOperand + m_iSecondOperand),
								m_iSendedAnwser );

		g_MyInfo.SetKickedInfo( szKickSubInfo );
		return false;
	case ID_QUIZ_SERVER_TIMEOVER:
		m_dwQuizStartTime = 0;
		g_MyInfo.SetKickedInfo( STR(2) );
		return false;
	}

	return false;
}

void HackProblemWnd::SetAnswerEditFocus()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
	}
}

void HackProblemWnd::SendQuizAnswer()
{
	if( m_dwQuizStartTime == 0 )
		return;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_ANSWER_EDIT );
	if( !pEdit )	return;
	if( strlen( pEdit->GetText() ) <= 0 )	return;

	m_iSendedAnwser = atoi( pEdit->GetText() );
	pEdit->KillKeyFocus();

	SP2Packet kPacket( CTPK_HACK_QUIZ );
	kPacket << ID_QUIZ_ANSWER;
	kPacket << m_iSendedAnwser;
	TCPNetwork::SendToServer( kPacket );

	HideWnd();
}

void HackProblemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void HackProblemWnd::OnProcess( float fTimePerSec )
{
	// 중간에 메세지 전송으로 인해 숨겨지더라도 시간체크는 계속 한다.
	if( m_dwQuizStartTime == 0 )
		return;

	DWORD dwPastTime = REALGETTIME() - m_dwQuizStartTime;
	if( dwPastTime < m_dwQuizLimitTime )
	{
		char szBuf[MAX_PATH];
		int iRemainSecond = ( m_dwQuizLimitTime - dwPastTime ) / 1000;

		if( m_QuizType == QT_SPEED )
		{
			SafeSprintf( szBuf, sizeof(szBuf), STR(1), iRemainSecond );
			SetTitleText( szBuf );
		}
	}
	else
	{
		SP2Packet kPacket( CTPK_HACK_QUIZ );
		kPacket << ID_QUIZ_TIMEOVER;
		TCPNetwork::SendToServer( kPacket );

		m_dwQuizStartTime = 0;
		HideWnd();

		g_App.KickToHeadQuater( EXIT_ROOM_SPEEDHACK );
		g_MyInfo.SetKickedInfo( STR(2) );
	}
}

void HackProblemWnd::OnRender()
{
	ioWnd::OnRender();

	int xPos = GetDerivedPosX();
	int yPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_QuizType == QT_SPEED )
	{
		if( m_iCurQuizChance == 0 || m_iMaxQuizChance == 1 )	// 최초
		{
			g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(1) );
		}
		else if( m_iCurQuizChance + 1 >= m_iMaxQuizChance ) // 마지막
		{
			g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(2),
															(int)m_iPreQuizFirst,
															(int)m_iPreQuizSecond,
															(int)( m_iPreQuizFirst + m_iPreQuizSecond ),
															m_iSendedAnwser );
		}
		else // 중간
		{
			g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(3),
															(int)m_iPreQuizFirst,
															(int)m_iPreQuizSecond,
															(int)(m_iPreQuizFirst + m_iPreQuizSecond),
															m_iSendedAnwser );
		}
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_RIGHT );

	switch( m_Operator )
	{
	case QP_ADD:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(8), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	case QP_MINUS:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(9), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	case QP_MULTIPLY:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(10), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	case QP_DIVIDE:
		g_FontMgr.PrintText( xPos + 94, yPos + 64, FONT_SIZE_14, STR(7), (int)m_iFirstOperand, (int)m_iSecondOperand );
		break;
	}
}

void HackProblemWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
}

void HackProblemWnd::iwm_hide()
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