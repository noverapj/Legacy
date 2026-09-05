
#include "StdAfx.h"

#include "NamedTitleBtn.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NamedTitleBtn::NamedTitleBtn()
{
	m_pEffectFrm = NULL;

	m_dwCheckTime = 0;
	m_nCurAlpha = 0;
	m_bIsAddAlpha = false;
	m_bIsEffect = false;
}

NamedTitleBtn::~NamedTitleBtn()
{
	SAFEDELETE( m_pEffectFrm );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "EffectFrm" )
	{
		SAFEDELETE( m_pEffectFrm );
		m_pEffectFrm = pFrame;
	}
	else
		ioButton::AddRenderFrame( szType, pFrame );
}

void NamedTitleBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_dwChangeTime = xElement.GetIntAttribute_e( "Change_Time" );
	m_nAddAlpha = xElement.GetIntAttribute_e( "Add_Alpha" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioButton::OnProcess( fTimePerSec );

	if( m_bIsEffect )
		UpdateEffect();
}

void NamedTitleBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if ( m_bIsEffect )
	{
		if ( m_pEffectFrm )
			m_pEffectFrm->Render( iXPos, iYPos );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleBtn::SetStartEffect()
{
	m_dwCheckTime = FRAMEGETTIME();
	m_nCurAlpha = 0;
	m_bIsAddAlpha = true;
	m_bIsEffect = true;
}

void NamedTitleBtn::SetEndEffect()
{
	m_bIsEffect = false;
}

////////////////////////////////////////////////////////////////////spq////////////////////////////////////////////////////////////////

void NamedTitleBtn::UpdateEffect()
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

	if ( m_pEffectFrm )
		m_pEffectFrm->SetAlpha( m_nCurAlpha );
}