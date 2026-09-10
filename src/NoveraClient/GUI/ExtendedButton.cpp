

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioXMLElement.h"

#include "ExtendedButton.h"

TwinkleButton::TwinkleButton()
{
	m_dwScreenTimer = 0;
	m_dwCurTimer    = 0;
	m_bScreen       = false;
	m_bTwinkle      = true;
}

TwinkleButton::~TwinkleButton()
{
}

void TwinkleButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_dwScreenTimer = xElement.GetIntAttribute_e( "ScreenTime" );
	m_dwCurTimer    = FRAMEGETTIME();
}

void TwinkleButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( !m_bTwinkle ) return;

	if( FRAMEGETTIME() - m_dwCurTimer > m_dwScreenTimer )
	{
		m_dwCurTimer = FRAMEGETTIME();
		m_bScreen = !m_bScreen;
	}
}

void TwinkleButton::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bScreen )
		ioButton::OnDrawOvered( iXPos, iYPos );
	else
		ioButton::OnDrawNormal( iXPos, iYPos );
}

void TwinkleButton::SetTwinkle( bool bTwinkle )
{
	m_bTwinkle = bTwinkle;
	if( !m_bTwinkle )
		m_bScreen = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ComplexTitleBtn::ComplexTitleBtn()
{
	m_TextHorzAlign = TAT_LEFT;
	m_iXOffset = 0;
	m_iYOffset = 0;
}

ComplexTitleBtn::~ComplexTitleBtn()
{

}

void ComplexTitleBtn::OnRender()
{
	ioButton::OnRender();
	float fXPos, fYPos;
	fXPos = GetDerivedPosX();
	fYPos = GetDerivedPosY();

	if( m_TitlePrinter.IsEmpty() ) return;

	m_TitlePrinter.PrintFullText( fXPos + m_iXOffset, fYPos + m_iYOffset, m_TextHorzAlign );
}

void ComplexTitleBtn::SetComplexTitle( ioComplexStringPrinter &rkPrinter , TextAlignType eTextHorzAlign, int iXOffset, int iYOffset )
{
	rkPrinter.CopyFullText( m_TitlePrinter );
	m_TextHorzAlign = eTextHorzAlign;
	m_iXOffset      = iXOffset;
	m_iYOffset      = iYOffset;
}