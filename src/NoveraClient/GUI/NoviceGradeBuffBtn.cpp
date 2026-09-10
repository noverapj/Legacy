
#include "StdAfx.h"

#include "NoviceGradeBuffBtn.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NoviceGradeBuffBtn::NoviceGradeBuffBtn()
{	
	m_pIcon = NULL;
	m_pIconEffect = NULL;
	m_pIconAddEffect = NULL;

	m_fScale = 0.7f;
	m_dwCheckTime = 0;
	m_nCurAlpha = 0;
	m_bIsAddAlpha = true;
}

NoviceGradeBuffBtn::~NoviceGradeBuffBtn()
{	
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconEffect );
	SAFEDELETE( m_pIconAddEffect );
}

void NoviceGradeBuffBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconEffect" )
	{
		SAFEDELETE( m_pIconEffect );
		m_pIconEffect = pImage;
	}
	else if( szType == "IconAddEffect" )
	{
		SAFEDELETE( m_pIconAddEffect );
		m_pIconAddEffect = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );	
}

void NoviceGradeBuffBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_dwChangeTime = xElement.GetIntAttribute_e( "Change_Time" );
	m_nAddAlpha = xElement.GetIntAttribute_e( "Add_Alpha" );
}

void NoviceGradeBuffBtn::iwm_show()
{
	m_dwCheckTime = FRAMEGETTIME();
	m_nCurAlpha = 0;
	m_bIsAddAlpha = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NoviceGradeBuffBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	UpdateEffect();
}

void NoviceGradeBuffBtn::OnRender()
{
	int nNewXPos = GetDerivedPosX() + GetWidth() / 2;
	int nNewYPos = GetDerivedPosY() + GetHeight() / 2;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );
		m_pIcon->Render( nNewXPos, nNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if ( m_pIconEffect )
		m_pIconEffect->Render( nNewXPos-2, nNewYPos-4, UI_RENDER_NORMAL, TFO_BILINEAR );

	if ( m_pIconAddEffect )
		m_pIconAddEffect->Render( nNewXPos-2, nNewYPos-4, UI_RENDER_ADD, TFO_BILINEAR );
}

////////////////////////////////////////////////////////////////////spq////////////////////////////////////////////////////////////////

void NoviceGradeBuffBtn::UpdateEffect()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckTime + m_dwChangeTime > dwCurTime )
		return;

	m_dwCheckTime = dwCurTime;

	if ( m_nCurAlpha >= 255 )
		m_bIsAddAlpha = false;
	else if ( m_nCurAlpha <= 0 )
		m_bIsAddAlpha = true;

	if ( m_bIsAddAlpha )
	{
		m_nCurAlpha+=m_nAddAlpha;
		m_nCurAlpha = min( m_nCurAlpha, 255 );
	}
	else
	{
		m_nCurAlpha-=m_nAddAlpha;
		m_nCurAlpha = max( m_nCurAlpha, 0 );
	}

	if ( m_pIconAddEffect )
		m_pIconAddEffect->SetAlpha( m_nCurAlpha );
}