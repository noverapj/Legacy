#include "StdAfx.h"
#include "QuickModeToolTip.h"


CQuickModeToolTip::CQuickModeToolTip(void)
{
	m_nMaxPlayer = 0;
	m_nMinPlayer = 0;
	m_bSamePlayer = false;
	m_pCallWnd = NULL;
}


CQuickModeToolTip::~CQuickModeToolTip(void)
{

}

void CQuickModeToolTip::SetModeHelpTooltip( ioWnd *pCallWnd, const int nMode )
{
	m_pCallWnd = pCallWnd;

	if( nMode == -1 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	m_szModeTitle = g_ModeSelectInfo.GetSelectQuickStartModeTitle(nMode);
	m_nMinPlayer = g_BattleRoomMgr.GetQuickStartMinPlayer(nMode);
	m_nMaxPlayer = g_BattleRoomMgr.GetQuickStartMaxPlayer(nMode);
	m_bSamePlayer = g_BattleRoomMgr.IsQuickStartSameUser(nMode);

	if( m_bSamePlayer)
		SetSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	else
		SetSize(DEFAULT_WIDTH, DEFAULT_HEIGHT - LINE_GAP_HEIGHT);

	ProcessPosition();
}

void CQuickModeToolTip::ProcessPosition()
{
	if( m_pCallWnd )
	{
		if( !m_pCallWnd->IsShow() )
		{
			SetModeHelpTooltip( NULL, -1 );
			return;
		}
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void CQuickModeToolTip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessPosition();	
}

void CQuickModeToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );

	// 모드명
	g_FontMgr.SetTextColor(TCT_DEFAULT_GREEN);		
	g_FontMgr.PrintTextWidthCut( iXPos + START_TITLE_X, iYPos + START_TITLE_Y, FONT_SIZE_12, 150.0f, m_szModeTitle.c_str() );	

	
	char szPlayer[MAX_PATH];
	sprintf(szPlayer, "최소 %d명 ~ 최대 %d명", m_nMinPlayer, m_nMaxPlayer);

	g_FontMgr.SetTextColor(TCT_DEFAULT_GRAY);
	g_FontMgr.PrintText(iXPos + START_TITLE_X, iYPos + START_HELP_Y, FONT_SIZE_12, szPlayer);

	if(m_bSamePlayer)
		g_FontMgr.PrintText(iXPos + START_TITLE_X, iYPos + START_HELP_Y + LINE_GAP_HEIGHT, FONT_SIZE_12, "양팀 같은 인원");

}