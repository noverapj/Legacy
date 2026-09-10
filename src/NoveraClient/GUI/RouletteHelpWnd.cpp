
#include "StdAfx.h"

#include "RouletteHelpWnd.h"
#include "../ioRouletteManager.h"

RouletteHelpWnd::RouletteHelpWnd()
{
	m_pIcon = NULL;
	m_pIconBack = NULL;
}

RouletteHelpWnd::~RouletteHelpWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void RouletteHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon= pImage;
	}
	else if( szType == "icon_back" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void RouletteHelpWnd::iwm_show()
{
}

void RouletteHelpWnd::iwm_hide()
{
}

void RouletteHelpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void RouletteHelpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pIconBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pIconBack->SetScale( 0.6f );
		m_pIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pIcon )
	{
		m_pIcon->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pIcon->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pIcon->SetScale( 0.6f );
		m_pIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 65, iYPos + 55, FONT_SIZE_13, "쿠폰으로 누구나 돌림판을 돌릴 수 있습니다." );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 65, iYPos + 75, FONT_SIZE_11, "쿠폰은 광장의 곰을 잡으면 얻을 수 있습니다." );

	g_FontMgr.SetTextColor( 184, 37, 37 );
	g_FontMgr.PrintText( iXPos + 18, iYPos + 101, FONT_SIZE_13, "참여 방법 : 돌림판의 버튼 위에 올라가서 A(점프)키를" );
	g_FontMgr.PrintText( iXPos + 18, iYPos + 121, FONT_SIZE_13, "누르면 자동으로 참여됩니다." );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 18, iYPos + 151, FONT_SIZE_13, "●여러 사람이 동시에 참여할 수 있습니다" );
	g_FontMgr.PrintText( iXPos + 18, iYPos + 171, FONT_SIZE_13, "●동시 참여 인원이 많을수록 1등 확률이 높아집니다" );
	g_FontMgr.PrintText( iXPos + 18, iYPos + 191, FONT_SIZE_13, "●1등에 당첨되면 참여한 모두가 상품을 획득합니다" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 25, iYPos + 228, FONT_SIZE_13, "동시 참여 인원" );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( iXPos + 301, iYPos + 228, FONT_SIZE_13, "1등 확률" );

	int iSize = g_RouletteMgr.GetResultGroupMaxArray();
	for( int i=0; i<iSize; ++i )
	{
		int iMax = 0, iMin = 0, iRate = 0;
		g_RouletteMgr.GetResultGroupInfoByArray( i, iMax, iMin, iRate );
		
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetAlignType( TAT_LEFT );
		if( i==0 )
			g_FontMgr.PrintText( iXPos + 25, iYPos + 254 + i * 24, FONT_SIZE_13, "%d명이하", iMax );
		else
			g_FontMgr.PrintText( iXPos + 25, iYPos + 254 + i * 24, FONT_SIZE_13, "%d명~%d명", iMin, iMax );

		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.PrintText( iXPos + 301, iYPos + 254 + i * 24, FONT_SIZE_13, "%d%%", iRate );
	}
}

//////////////////////////////////////////////////////////////////////////
RouletteCenterHelpWnd::RouletteCenterHelpWnd()
{
	m_iAlphaRate = 0;
	m_State = US_NONE;

	m_pTextBack = NULL;
}

RouletteCenterHelpWnd::~RouletteCenterHelpWnd()
{
	m_iAlphaRate = 0;
	m_State = US_NONE;

	SAFEDELETE( m_pTextBack );
}

void RouletteCenterHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TextBack" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void RouletteCenterHelpWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false */ )
{
	SetWndPos( Setting::Width() / 2, (Setting::Height() * 0.2 ) );
}

void RouletteCenterHelpWnd::iwm_show()
{
	SetPositionToDefault( Setting::Width(), Setting::Height(), true );
}

void RouletteCenterHelpWnd::iwm_hide()
{
	m_iAlphaRate = 0;
	m_State = US_NONE;
}

void RouletteCenterHelpWnd::OnProcess( float fTimePerSec )
{
	float fRate = static_cast<float>( ( fTimePerSec * 1000 ) / 300 );

	switch( m_State )
	{
	case US_START:
		m_iAlphaRate += static_cast<int>( MAX_ALPHA_RATE * fRate );
		m_iAlphaRate = min( m_iAlphaRate, MAX_ALPHA_RATE );
		if( m_iAlphaRate >= MAX_ALPHA_RATE )
			m_State = US_STAY;
		break;
	case US_STAY:
		if( !g_RouletteMgr.CheckCharRange( g_MyInfo.GetPublicID() ) || g_RouletteMgr.GetState() != ioRouletteManager::RS_READY )
			m_State = US_END;
		break;
	case US_END:
		m_iAlphaRate -= static_cast<int>( MAX_ALPHA_RATE * fRate );
		m_iAlphaRate = max( m_iAlphaRate, 0 );
		if( m_iAlphaRate <= 0 )
			HideWnd();
		break;
	}
}

void RouletteCenterHelpWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pTextBack )
	{
		int iXOffset = m_pTextBack->GetWidth()/2;
		m_pTextBack->SetAlpha( m_iAlphaRate );
		m_pTextBack->Render( iXPos - iXOffset, iYPos );
	}
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintTextAlpha( iXPos, iYPos+2, FONT_SIZE_14, m_iAlphaRate, "A(점프)키를 누르면 돌림판 이벤트에 참여됩니다." );
}

void RouletteCenterHelpWnd::SetInfo()
{
	if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN ) <= 0 )
		return;

	if( m_State == US_START || m_State == US_STAY )
		return;

	m_State = US_START;
	SetWndPos( Setting::Width() / 2, (Setting::Height() * 0.2 ) );

	if( !IsShow() )
		ShowWnd();
}