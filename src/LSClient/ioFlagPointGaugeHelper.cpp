#include "StdAfx.h"
#include "ioFlagPointGaugeHelper.h"

#include "io2DCircleGauge.h"


class ioCircleGaugeQuad : public IUIQuad
{
protected:
	io2DCircleGauge *m_pGauge;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray )
	{
		pQuadArray->AddDirectQuadInfo( this );
	}

	virtual int GetQuadCount() const
	{
		return 0;
	}

public:
	virtual bool IsDirectQuad() const
	{
		return true;
	}

	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
		if( m_pGauge )
		{
			m_pGauge->Render( pRenderSystem );
		}
	}

public:
	ioCircleGaugeQuad( io2DCircleGauge *pGauge )
	{
		m_pGauge = pGauge;
	}
	virtual ~ioCircleGaugeQuad() {}
};
////////////////////////////////////////////////////////////////////////
ioFlagPointGaugeHelper::ioFlagPointGaugeHelper(void)
{
	m_pGaugeBack = NULL;
	m_pGaugeInnerBack = NULL;
	m_pTimeNumber = NULL;
	m_pScoreNumber = NULL;
	m_pScorePlus = NULL;
	m_pFlagBlue = NULL;
	m_pFlagRed = NULL;
	m_pFlagMine = NULL;
	m_pCurFlag = NULL;
	m_pCircleGauge = NULL;

	m_iCurScore = 0;
	m_fGaugeRate = 0.0f;
}


ioFlagPointGaugeHelper::~ioFlagPointGaugeHelper(void)
{
	ClearResource();
}

void ioFlagPointGaugeHelper::InitResource()
{
	m_pGaugeBack		 = g_UIImageSetMgr.CreateImage( "BaseImage042", "flagGauge_3" );
	m_pGaugeInnerBack	 = g_UIImageSetMgr.CreateImage( "BaseImage042", "flagGauge_1" );
	m_pTimeNumber		= g_UIImageSetMgr.CreateImage( "BaseImage042", "1vs1_score_yellownum" );
	m_pScoreNumber		= g_UIImageSetMgr.CreateImage( "BaseImage042", "flag_score_number" );
	m_pScorePlus		= g_UIImageSetMgr.CreateImage( "BaseImage042", "flag_score_plus" );
	m_pFlagBlue			= g_UIImageSetMgr.CreateImage( "BaseImage042", "Minimap_Flag_Blue" );
	m_pFlagRed			= g_UIImageSetMgr.CreateImage( "BaseImage042", "Minimap_Flag_Red" );
	m_pFlagMine			= g_UIImageSetMgr.CreateImage( "BaseImage042", "Minimap_Flag_Me" );

	m_pCircleGauge = new io2DCircleGauge;
	if( m_pCircleGauge)                
		m_pCircleGauge->InitResource( "flag_Gauge.dds", 360, 0, 0, io2DCircleGauge::ST_12OCLOCK );
}

void ioFlagPointGaugeHelper::ClearResource()
{
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGaugeInnerBack );
	SAFEDELETE( m_pTimeNumber );
	SAFEDELETE( m_pScoreNumber );
	SAFEDELETE( m_pScorePlus );
	SAFEDELETE( m_pFlagBlue );
	SAFEDELETE( m_pFlagRed );
	SAFEDELETE( m_pFlagMine );
	SAFEDELETE( m_pCircleGauge );
}

void ioFlagPointGaugeHelper::SetTeam( TeamType eType, bool bIsOwner )
{
	if( bIsOwner )
		m_pCurFlag = m_pFlagMine;
	else if( eType == TEAM_BLUE )
		m_pCurFlag = m_pFlagBlue;
	else if( eType == TEAM_RED )
		m_pCurFlag = m_pFlagRed;
}

void ioFlagPointGaugeHelper::UpdateFlagPointGauge( int iXPos, int iYPos, float fCurAmt, float fMaxAmt, DWORD dwPassedTime )
{
	UpdateGauge( iXPos, iYPos, fCurAmt, fMaxAmt );
	UpdatePoint( fCurAmt, dwPassedTime );
}

void ioFlagPointGaugeHelper::UpdateGauge( int iXPos, int iYPos, float fCurAmt, float fMaxAmt )
{
	m_fGaugeRate = fCurAmt / fMaxAmt;
}

void ioFlagPointGaugeHelper::UpdatePoint( float fCurAmt, DWORD dwPassedTime )
{
	m_iCurScore = (int)fCurAmt;
	m_dwPassedTime = dwPassedTime;
}

void ioFlagPointGaugeHelper::RenderFlagPointGauge( float fXPos, float fYPos )
{
	RenderGauge( fXPos, fYPos );
	RenderPoint( fXPos, fYPos );	
}

void ioFlagPointGaugeHelper::RenderGauge( float fXPos, float fYPos )
{
	if( !m_pGaugeBack || !m_pGaugeInnerBack || !m_pCircleGauge )
		return;

	m_pGaugeBack->SetAlpha( 255 );
	m_pGaugeBack->Render( fXPos, fYPos );

	m_pCircleGauge->UpdateGauge( (int)fXPos - CIRCLE_GAUGE_OFFSET_X, (int)fYPos - CIRCLE_GAUGE_OFFSET_Y, m_fGaugeRate );

	if( m_pCircleGauge->IsNeedRender() )
	{
		// 소멸은 g_UIRenderer가 해준다.
		ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleGauge );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}

	m_pGaugeInnerBack->SetAlpha( 255 );
	m_pGaugeInnerBack->Render( fXPos, fYPos );
}

void ioFlagPointGaugeHelper::RenderPoint( float fXPos, float fYPos )
{
	if( !m_pTimeNumber )
		return;

	if( !m_pScoreNumber || !m_pScorePlus )
		return;

	if( !m_pFlagMine || !m_pFlagBlue || !m_pFlagRed )
		return;

	int iTime = m_dwPassedTime / 1000;
	m_pTimeNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );

	if( iTime >= 100 )
		m_pTimeNumber->RenderNum( fXPos - 2.0f, fYPos - 12.0f, iTime, -4.0f, 0.65f );
	else if( iTime >= 10 )
		m_pTimeNumber->RenderNum( fXPos - 2.0f, fYPos - 15.0f, iTime, -4.0f, 0.8f );
	else
		m_pTimeNumber->RenderNum( fXPos - 2.0f, fYPos - 18.0f, iTime, -4.0f );

	m_pScoreNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	m_pScoreNumber->RenderNum( fXPos, fYPos + 2.0f, m_iCurScore, -4.0f );

	m_pScorePlus->Render( fXPos - 15.0f, fYPos + 2.0f );

	m_pCurFlag->Render( fXPos - 20.0f, fYPos + 15.0f );
}