

#include "stdafx.h"

#include "ioUIImage.h"
#include "ioUIImageSet.h"

ioUIImage::ioUIImage( ioUIImageSet *pImageSet,
					  const ioHashString &rkName,
					  const RECT &rcArea )
: m_pCreator(pImageSet),
  m_Name(rkName)
{
	m_iRefCount = 0;

	m_rcArea = rcArea;
	m_iOffsetX = 0;
	m_iOffsetY = 0;

	m_TexUV.x = m_TexUV.y = m_TexUV.z = m_TexUV.w = 0.0f;
	m_bTexUVInited = false;
}

ioUIImage::~ioUIImage()
{
}

void ioUIImage::SetAreaRect( const RECT &rcRect )
{
	m_rcArea = rcRect;
	m_bTexUVInited = false;
}

const D3DXVECTOR4& ioUIImage::GetTexUV() const
{
	if( m_bTexUVInited )	// 이전에 계산된 UV
		return m_TexUV;

	if( m_pCreator->IsLoaded() )
	{
		float fInvWidth  = 1.0f / (float)m_pCreator->GetTextureWidth();
		float fInvHeight = 1.0f / (float)m_pCreator->GetTextureHeight();

		m_TexUV.x = (float)m_rcArea.left * fInvWidth;
		m_TexUV.y = (float)m_rcArea.top * fInvHeight;
		m_TexUV.z = (float)m_rcArea.right * fInvWidth;
		m_TexUV.w = (float)m_rcArea.bottom * fInvHeight;
		m_bTexUVInited = true;
	}
	else
	{
		m_pCreator->Touch( true );
	}

	return m_TexUV;
}

bool ioUIImage::FillFileImage( const char *szFileName,
							   const int iFileWidth,
							   const int iFileHeight,
							   OUT D3DXIMAGE_INFO *pSrcInfo /*= NULL*/ ) const
{
	if( m_pCreator )
		return m_pCreator->FillFileImage( szFileName, iFileWidth, iFileHeight, this, pSrcInfo );

	return false;
}

IDirect3DTexture9* ioUIImage::GetD3DTexture() const
{
	if( m_pCreator )
		return m_pCreator->GetD3DTexture();

	return NULL;
}

IDirect3DTexture9* ioUIImage::GetGrayTexture() const
{
	if( m_pCreator )
		return m_pCreator->GetGrayTexture();

	return NULL;
}

IDirect3DTexture9* ioUIImage::GetGrayLightTexture() const
{
	if( m_pCreator )
		return m_pCreator->GetGrayLightTexture();

	return NULL;
}

void ioUIImage::NotifyAttached() const
{
	m_iRefCount++;

	if( m_pCreator )
	{
		m_pCreator->AddRef();
	}
}

void ioUIImage::NotifyDetached() const
{
	m_iRefCount--;

	if( m_pCreator )
	{
		m_pCreator->ReleaseRef();
	}
}

void ioUIImage::ForceTouch() const
{
	if( m_pCreator )
	{
		m_pCreator->Touch( false );
	}
}