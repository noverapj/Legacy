

#include "stdafx.h"

#include "ioUIRenderElement.h"

ioUIRenderElement::ioUIRenderElement()
{
	SetRectEmpty( &m_RenderRect );
	m_dwColor = 0xffffffff;
	
	m_fXScaleRate = 1.0f;
	m_fYScaleRate = 1.0f;

	m_eRenderType = UI_RENDER_NORMAL;
	m_eTextureFilterOption = TFO_NEAREST;
	m_eGrayRenderType = UI_RENDER_NORMAL;

	m_bShow = true;
}

ioUIRenderElement::ioUIRenderElement( const ioUIRenderElement &rhs )
{
	m_RenderRect = rhs.m_RenderRect;
	m_dwColor	 = rhs.m_dwColor;

	m_fXScaleRate = rhs.m_fXScaleRate;
	m_fYScaleRate = rhs.m_fYScaleRate;

	m_eRenderType = UI_RENDER_NORMAL;
	m_eTextureFilterOption = TFO_NEAREST;
	m_eGrayRenderType = UI_RENDER_NORMAL;

	m_bShow = rhs.m_bShow;
}

ioUIRenderElement::~ioUIRenderElement()
{
}

void ioUIRenderElement::SetPosition( int iXPos, int iYPos )
{
	RECT rcRect;
	rcRect.left   = iXPos;
	rcRect.top    = iYPos;
	rcRect.right  = iXPos + GetWidth();
	rcRect.bottom = iYPos + GetHeight();

	SetRenderRect( rcRect );
}

void ioUIRenderElement::SetSize( int iWidth, int iHeight )
{
	RECT rcRect;
	rcRect.left   = m_RenderRect.left;
	rcRect.top    = m_RenderRect.top;
	rcRect.right  = m_RenderRect.left + iWidth;
	rcRect.bottom = m_RenderRect.top + iHeight;

	SetRenderRect( rcRect );
}

void ioUIRenderElement::SetRenderRect( const RECT &rcRect )
{
	m_RenderRect = rcRect;
}

void ioUIRenderElement::SetScale( float fRate )
{
	m_fXScaleRate = fRate;
	m_fYScaleRate = fRate;
}

void ioUIRenderElement::SetScale( float fXScale, float fYScale )
{
	m_fXScaleRate = fXScale;
	m_fYScaleRate = fYScale;
}

void ioUIRenderElement::SetXScale( float fRate )
{
	m_fXScaleRate = fRate;
}

void ioUIRenderElement::SetYScale( float fRate )
{
	m_fYScaleRate = fRate;
}

void ioUIRenderElement::SetColor( DWORD dwColor )
{
	m_dwColor = ( m_dwColor & 0xff000000 ) | ( dwColor & 0x00ffffff );
}

void ioUIRenderElement::SetColor( int iRed, int iGreen, int iBlue )
{
	int iAlpha = m_dwColor >> 24;
	m_dwColor = D3DCOLOR_ARGB( iAlpha, iRed, iGreen, iBlue );
}

void ioUIRenderElement::SetAlpha( BYTE iAlpha )
{
	m_dwColor = ( (DWORD)iAlpha << 24 ) | ( m_dwColor & 0xffffff );
}

void ioUIRenderElement::SetAlphaWithColor( BYTE iAlpha )
{
	m_dwColor = D3DCOLOR_ARGB( iAlpha, iAlpha, iAlpha, iAlpha );
}

D3DXVECTOR2 ioUIRenderElement::GetSize() const
{
	return D3DXVECTOR2( GetWidth(), GetHeight() );
}