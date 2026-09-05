#include "StdAfx.h"

#include "SmallTabButton.h"

SmallTabButton::SmallTabButton()
{

}

SmallTabButton::~SmallTabButton()
{
	m_bDsiable = false;
}

void SmallTabButton::SetDisable( bool bDisable )
{
	m_bDsiable = bDisable;
}

void SmallTabButton::OnDrawNormal( int iXPos, int iYPos )
{
	if( !m_bDsiable )
		ioRadioButton::OnDrawNormal( iXPos, iYPos );
}

void SmallTabButton::OnDrawOvered( int iXPos, int iYPos )
{
	if( !m_bDsiable )
		ioRadioButton::OnDrawOvered( iXPos, iYPos );
}

void SmallTabButton::OnDrawOveredAdd( int iXPos, int iYPos )
{
	if( !m_bDsiable )
		ioRadioButton::OnDrawOveredAdd( iXPos, iYPos );
}

void SmallTabButton::OnDrawPushed( int iXPos, int iYPos )
{
	if( !m_bDsiable )
		ioRadioButton::OnDrawPushed( iXPos, iYPos );
}

void SmallTabButton::OnDrawDisabled( int iXPos, int iYPos )
{
	if( !m_bDsiable )
		ioRadioButton::OnDrawDisabled( iXPos, iYPos );
}