

#include "stdafx.h"

#include "ioGaugeHelper.h"

ioGaugeHelper::ioGaugeHelper()
{
	m_pGaugeBack = NULL;
	m_pGauge = NULL;
	m_pGaugeScreenLeft = NULL;
	m_pGaugeScreenCenter = NULL;
	m_pGaugeScreenRight = NULL;

	m_pObserverGaugeBack = NULL;
	m_pObserverGauge = NULL;
	m_pObserverGaugeScreenLeft = NULL;
	m_pObserverGaugeScreenCenter = NULL;
	m_pObserverGaugeScreenRight = NULL;

	m_TeamType	 = TEAM_NONE;
	m_iRenderAmt = 0;
	m_bSetAlpha  = false;
}

ioGaugeHelper::~ioGaugeHelper()
{
	ClearResource();
}

void ioGaugeHelper::ClearResource()
{
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pGaugeScreenLeft );
	SAFEDELETE( m_pGaugeScreenCenter );
	SAFEDELETE( m_pGaugeScreenRight );

	SAFEDELETE( m_pObserverGaugeBack );
	SAFEDELETE( m_pObserverGauge );
	SAFEDELETE( m_pObserverGaugeScreenLeft );
	SAFEDELETE( m_pObserverGaugeScreenCenter );
	SAFEDELETE( m_pObserverGaugeScreenRight );
}

bool ioGaugeHelper::IsResouceOK() const
{
	if( !m_pGaugeBack ||
		!m_pGauge ||
		!m_pGaugeScreenLeft ||
		!m_pGaugeScreenCenter ||
		!m_pGaugeScreenRight )
		return false;

	if( !m_pObserverGaugeBack ||
		!m_pObserverGauge ||
		!m_pObserverGaugeScreenLeft ||
		!m_pObserverGaugeScreenCenter ||
		!m_pObserverGaugeScreenRight )
		return false;

	return true;
}

void ioGaugeHelper::InitResource()
{
	m_pGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenHPGaugeBack" );
	m_pGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenHPGauge" );
	m_pGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenScreenLeft" );
	m_pGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenScreenCenter" );
	m_pGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenScreenRight" );

	m_pObserverGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenhp_gaugeback" );
	m_pObserverGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenhp_gauge" );
	m_pObserverGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenscreen_left" );
	m_pObserverGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenscreen_center" );
	m_pObserverGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenscreen_right" );
}

void ioGaugeHelper::SetTeam( TeamType eTeam )
{
	if( m_TeamType == eTeam )	return;

	m_TeamType = eTeam;

	ClearResource();

	switch( eTeam )
	{
	case TEAM_BLUE:
		m_pGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "BlueHPGaugeBack" );
		m_pGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "BlueHPGauge" );
		m_pGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "BlueScreenLeft" );
		m_pGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "BlueScreenCenter" );
		m_pGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "BlueScreenRight" );

		m_pObserverGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_bluehp_gaugeback" );
		m_pObserverGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_bluehp_gauge" );
		m_pObserverGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_bluescreen_left" );
		m_pObserverGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_bluescreen_center" );
		m_pObserverGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_bluescreen_right" );
		break;
	case TEAM_RED:
		m_pGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "RedHPGaugeBack" );
		m_pGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "RedHPGauge" );
		m_pGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "RedScreenLeft" );
		m_pGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "RedScreenCenter" );
		m_pGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "RedScreenRight" );

		m_pObserverGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_redhp_gaugeback" );
		m_pObserverGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_redhp_gauge" );
		m_pObserverGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_redscreen_left" );
		m_pObserverGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_redscreen_center" );
		m_pObserverGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_redscreen_right" );
		break;
	default:
		m_pGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenHPGaugeBack" );
		m_pGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenHPGauge" );
		m_pGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenScreenLeft" );
		m_pGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenScreenCenter" );
		m_pGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "GreenScreenRight" );

		m_pObserverGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenhp_gaugeback" );
		m_pObserverGauge			 = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenhp_gauge" );
		m_pObserverGaugeScreenLeft   = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenscreen_left" );
		m_pObserverGaugeScreenCenter = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenscreen_center" );
		m_pObserverGaugeScreenRight  = g_UIImageSetMgr.CreateImage( "BaseImage010", "watch_greenscreen_right" );
		break;
	}
}

int ioGaugeHelper::GetGaugeBackWidth( bool bObserver ) const
{
	if( bObserver )
	{
		if( m_pObserverGaugeBack )
			return m_pObserverGaugeBack->GetWidth();
	}
	else
	{
		if( m_pGaugeBack )
			return m_pGaugeBack->GetWidth();
	}

	return 0;
}

int ioGaugeHelper::GetGaugeBackHeight( bool bObserver ) const
{
	if( bObserver )
	{
		if( m_pObserverGaugeBack )
			return m_pObserverGaugeBack->GetHeight();
	}
	else
	{
		if( m_pGaugeBack )
			return m_pGaugeBack->GetHeight();
	}

	return 0;
}

void ioGaugeHelper::SetGaugeAlpha( bool bSetAlpha )
{
	m_bSetAlpha = bSetAlpha;
}

void ioGaugeHelper::UpdateGauge( float fCurAmt, float fMaxAmt, bool bObserver )
{
	if( IsResouceOK() )
	{
		float fGaugeRate = 0.0f;
		if( fMaxAmt > 0.0f )
		{
			fGaugeRate = max( 0.0f, min( fCurAmt / fMaxAmt, FLOAT1 ) );
		}

		if( bObserver )
		{
			if( m_pObserverGauge )
				m_iRenderAmt = m_pObserverGauge->GetWidth() * fGaugeRate;
		}
		else
		{
			if( m_pGauge )
				m_iRenderAmt = m_pGauge->GetWidth() * fGaugeRate;
		}
	}
	else
	{
		m_iRenderAmt = 0;
	}
}

void ioGaugeHelper::RenderGauge( float fXPos, float fYPos, bool bObserver )
{
	if( !IsResouceOK() )	return;

	if( bObserver )
		RenderOvserver( fXPos, fYPos );
	else
		RenderNormal( fXPos, fYPos );
}

void ioGaugeHelper::SetScale(float fScaleX, float fScaleY)
{
	if(m_pGaugeBack)
		m_pGaugeBack->SetScale(1.0f, fScaleY);

	if(m_pGauge)
		m_pGauge->SetScale(1.0f, fScaleY);

	if(m_pGaugeScreenLeft)
		m_pGaugeScreenLeft->SetScale(1.0f, fScaleY);

	if(m_pGaugeScreenCenter)
		m_pGaugeScreenCenter->SetScale(1.0f, fScaleY);

	if(m_pGaugeScreenRight)
		m_pGaugeScreenRight->SetScale(1.0f, fScaleY);
}

void ioGaugeHelper::RenderNormal( float fXPos, float fYPos )
{
	BYTE iAlpha = 255;
	if( m_bSetAlpha )
		iAlpha = 160;

	fXPos -= m_pGaugeBack->GetWidth() * FLOAT05;

	m_pGaugeBack->SetAlpha( iAlpha );
	m_pGaugeBack->Render( fXPos, fYPos );

	//  ======   : 게이지백
	//  ------   : 게이지
	// <------>  : 게이지 스크린 < : left, > : right  11X24 1X24 11X24

	if( m_iRenderAmt > 0 )
	{
		enum { GAUGE_X_OFFSET = 1, GAUGE_Y_OFFSET = 1 };
		m_pGauge->SetAlpha( iAlpha );
		m_pGauge->RenderWidthCut( fXPos + GAUGE_X_OFFSET, fYPos + GAUGE_Y_OFFSET, 0 , m_iRenderAmt );

		enum 
		{
			SCREEN_LEFT_X_OFFSET   = -10,
			SCREEN_CENTER_X_OFFSET =  1,
			SCREEN_RIGHT_X_OFFSET  =  1,
			SCREEN_Y_OFFSET        = -8,
		};

		m_pGaugeScreenLeft->Render( fXPos + SCREEN_LEFT_X_OFFSET, fYPos + SCREEN_Y_OFFSET, UI_RENDER_SCREEN );

		m_pGaugeScreenCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGaugeScreenCenter->SetSize( m_iRenderAmt, m_pGaugeScreenCenter->GetHeight() );
		m_pGaugeScreenCenter->Render( fXPos + SCREEN_CENTER_X_OFFSET, fYPos + SCREEN_Y_OFFSET, UI_RENDER_SCREEN );

		m_pGaugeScreenRight->Render( fXPos + m_iRenderAmt + SCREEN_RIGHT_X_OFFSET, fYPos + SCREEN_Y_OFFSET, UI_RENDER_SCREEN );
	}
}

void ioGaugeHelper::RenderOvserver( float fXPos, float fYPos )
{
	BYTE iAlpha = 255;
	if( m_bSetAlpha )
		iAlpha = 160;

	fXPos -= m_pObserverGaugeBack->GetWidth() * FLOAT05;

	m_pObserverGaugeBack->SetAlpha( iAlpha );
	m_pObserverGaugeBack->Render( fXPos, fYPos );

	//  ======   : 게이지백
	//  ------   : 게이지
	// <------>  : 게이지 스크린 < : left, > : right  11X24 1X24 11X24

	if( m_iRenderAmt > 0 )
	{
		enum { GAUGE_X_OFFSET = 1, GAUGE_Y_OFFSET = 1 };
		m_pObserverGauge->SetAlpha( iAlpha );
		m_pObserverGauge->RenderWidthCut( fXPos + GAUGE_X_OFFSET, fYPos + GAUGE_Y_OFFSET, 0 , m_iRenderAmt );

		enum 
		{
			SCREEN_LEFT_X_OFFSET   = -10,
			SCREEN_CENTER_X_OFFSET =  1,
			SCREEN_RIGHT_X_OFFSET  =  1,
			SCREEN_Y_OFFSET        = -10,
		};
		m_pObserverGaugeScreenLeft->Render( fXPos + SCREEN_LEFT_X_OFFSET, fYPos + SCREEN_Y_OFFSET, UI_RENDER_SCREEN );

		m_pObserverGaugeScreenCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pObserverGaugeScreenCenter->SetSize( m_iRenderAmt, m_pObserverGaugeScreenCenter->GetHeight() );
		m_pObserverGaugeScreenCenter->Render( fXPos + SCREEN_CENTER_X_OFFSET, fYPos + SCREEN_Y_OFFSET, UI_RENDER_SCREEN );

		m_pObserverGaugeScreenRight->Render( fXPos + m_iRenderAmt + SCREEN_RIGHT_X_OFFSET, fYPos + SCREEN_Y_OFFSET, UI_RENDER_SCREEN );
	}
}

