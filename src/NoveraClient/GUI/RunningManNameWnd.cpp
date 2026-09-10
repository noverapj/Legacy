

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3DEngine/InputBox.h"

#include "../SP2Packet.h"
#include "../TextColorTable.h"
#include "../NetworkWrappingFunc.h"
#include "../ioApplication.h"
#include "../ioAbuseManager.h"
#include "../ioMyInfo.h"
#include "../EtcHelpFunc.h"
#include "../ioBattleRoomMgr.h"
#include "../SlangUtil.h"

#include "ioSP2GUIManager.h"
#include "RunningManNameWnd.h"
#include <strsafe.h>

// 한번 작성한 이름은 저장
ioHashString RunningManNameWnd::m_szSaveName = "드워프";

RunningManNameWnd::RunningManNameWnd()
{
	m_dwEditStartTime = 0;
	m_dwEditLimitTime = 15000;
	m_bSelectName	  = true;
	m_pPreEdit		  = NULL;
}

RunningManNameWnd::~RunningManNameWnd()
{
}

void RunningManNameWnd::SetEditFocus()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( m_szSaveName.c_str() );
	}
}

void RunningManNameWnd::SendName()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( pEdit )
	{
		if( g_SlangUtil.IsLimitedGuildName( pEdit->GetText() ) )
		{
			//
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			SetEditFocus();
			return;
		}

		// 동기화 처리
		m_szSaveName = pEdit->GetText();
		if( m_szSaveName.IsEmpty() )
			m_szSaveName = STR(2);

		SP2Packet kPacket( CTPK_RUNNINGMAN_NAME_SYNC );
		kPacket << m_szSaveName;
		TCPNetwork::SendToServer( kPacket );

		m_bSelectName= false;
	}

	HideWnd();
}

void RunningManNameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			SendName();
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT && param == 1 )
		{
			SendName();
		}
		break;
	}
}

void RunningManNameWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_dwEditStartTime == 0 )
		return;

	DWORD dwPastTime = REALGETTIME() - m_dwEditStartTime;
	if( dwPastTime < m_dwEditLimitTime )
	{
		char szBuf[MAX_PATH];
		int iRemainSecond = ( m_dwEditLimitTime - dwPastTime ) / 1000;
		SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iRemainSecond );
		SetTitleText( szBuf );
	}
	else
	{
		SetEditFocus();    // 디폴트 이름 지정
		SendName();
	}
}

void RunningManNameWnd::OnRender()
{
	ioWnd::OnRender();

	int xPos = GetDerivedPosX();
	int yPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
    g_FontMgr.PrintText( xPos + 127, yPos + 41, FONT_SIZE_13, STR(1) );
}

void RunningManNameWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	SetEditFocus();
	m_dwEditStartTime = REALGETTIME();
}

void RunningManNameWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( pEdit )
	{
		pEdit->KillKeyFocus();
	}

	ioEdit *pPreEdit = dynamic_cast< ioEdit * >( m_pPreEdit );
	if( pPreEdit )	
	{
		ioWnd *pParent = pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			pPreEdit->SetKeyFocus();
		}
		m_pPreEdit = NULL;
	}
	m_dwEditStartTime = 0;
}