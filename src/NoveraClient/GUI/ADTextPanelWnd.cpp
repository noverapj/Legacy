#include "StdAfx.h"

#include "../ioShopManager.h"
#include "../TextColorTable.h"

#include "ADTextPanelWnd.h"
#include "ioFrameTimer.h"

ADTextPanelWnd::ADTextPanelWnd()
{
	m_pBackGround	= NULL;
	m_pOutBorder	= NULL;

	Clear();
}

ADTextPanelWnd::~ADTextPanelWnd()
{
	SAFEDELETE( m_pBackGround );
	SAFEDELETE( m_pOutBorder );
}

void ADTextPanelWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ADBackround" )
	{
		SAFEDELETE( m_pBackGround );
		m_pBackGround = pImage;

		if( m_pBackGround )
		{
			m_pBackGround->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pBackGround->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pBackGround->SetSize( 225, 18 );
		}
	}
	else if( szType == "ADOutBorder" )
	{
		SAFEDELETE( m_pOutBorder );
		m_pOutBorder = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ADTextPanelWnd::iwm_hide()
{
	ioRenderTarget::Destroy();
}

void ADTextPanelWnd::iwm_show()
{
	Clear();
	NextAD();
}

void ADTextPanelWnd::Clear()
{
	m_ADText.clear();

	m_iCurrIndex	= 0;	
	m_dwTextWidth	= 0;

	m_fADSpeed		= 0;
	m_fCurrXPos		= 0.0f;

	m_dwColor		= TCT_DEFAULT_DARKGRAY;
}

void ADTextPanelWnd::NextAD()
{
	m_iCurrIndex++;

	//인덱스 유효성 검사
	if( !COMPARE( m_iCurrIndex, 0, g_ShopManager.GetTextADCount() ) )
	{
		m_iCurrIndex = 0;
	}

	//유효한 광고 찾기
	for( int i = m_iCurrIndex; i < g_ShopManager.GetTextADCount(); ++i )
	{		
		const ADText* pCurrADText = g_ShopManager.GetTextAD( i );
		if( !pCurrADText )
			continue;

		if( pCurrADText->m_Text.IsEmpty() )
			continue;

		//기간 이 끝난 광고는 패스
		if( g_ShopManager.IsEndAD( pCurrADText->m_ADEndPeriod ) )
			continue;

		//아직 시작할수없는 광고는 패스
		if( !g_ShopManager.IsStartAD( pCurrADText->m_ADStartPeriod ) )
			continue;

		m_iCurrIndex			= i;
		m_ADText				= pCurrADText->m_Text.c_str();				
		m_fADSpeed				= pCurrADText->m_fADSpeed;
		m_dwColor				= pCurrADText->m_dwColor;
		m_dwTextWidth			= g_FontMgr.GetTextWidth( m_ADText.c_str(), TS_NORMAL, FONT_SIZE_14 );

		//우측으로 좌측으로 이동
		m_fCurrXPos				= (float)GetWidth();

		ioRenderTarget::Create( 221, 15 );
		break;
	}
}

void ADTextPanelWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fXPos = -(float)m_dwTextWidth;
	if( m_fCurrXPos <= fXPos )
	{
		NextAD();
	}
	else
	{		
		m_fCurrXPos -= m_fADSpeed * fTimePerSec;
	}
}

void ADTextPanelWnd::RenderTarget()
{
	ioRenderTarget::RenderTarget();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( m_dwColor );
	g_FontMgr.PrintText( (int)m_fCurrXPos, 0, FONT_SIZE_14, m_ADText.c_str() );
	g_UIRenderer.Render( &RenderSystem(), false );
}

void ADTextPanelWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	if( m_pBackGround )
		m_pBackGround->Render( iXPos + 16, iYPos + 5 );

	ioRenderTarget::OnTextureRender( iXPos + 18, iYPos + 5, UI_RENDER_NORMAL, TFO_BILINEAR );

	if( m_pOutBorder )
		m_pOutBorder->Render( iXPos, iYPos );
}