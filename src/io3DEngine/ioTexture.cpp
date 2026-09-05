
#include "stdafx.h"

#include "ioCPU.h"
#include "ioTexture.h"
#include "ioTextureManager.h"
#include "HelpFunc.h"
#include "ErrorReport.h"

ioTexture::ioTexture( const ioHashString &name ) : ioResource( name )
{
	m_pTexture   = NULL;
	m_pGrayTexture = NULL;
	m_pGrayLightTexture = NULL;
	m_iSrcWidth  = 0;
	m_iSrcHeight = 0;
	m_Format	 = D3DFMT_UNKNOWN;
	m_dwUsage	 = 0;
	m_iMipMapLevelCnt = 0;

	m_MinimumQuality = TQ_DEFAULT;
	m_bDefaultPath = false;
	m_bLoadPiece   = false;
	m_bLoadedFromFile = true;
}

ioTexture::~ioTexture()
{
	SAFERELEASE( m_pTexture );
	SAFERELEASE( m_pGrayTexture );
	SAFERELEASE( m_pGrayLightTexture );

	g_TextureMgr.RemoveMem( GetName() );
}

void ioTexture::SetTexture( IDirect3DTexture9 *pTex )
{
	SAFERELEASE( m_pTexture );
	m_pTexture = pTex;
}

void ioTexture::SetGrayTexture( IDirect3DTexture9 *pGrayTex ) const
{
	SAFERELEASE( m_pGrayTexture );
	m_pGrayTexture = pGrayTex;
}

void ioTexture::SetGrayLightTexture( IDirect3DTexture9 *pGrayLightTex ) const
{
	SAFERELEASE( m_pGrayLightTexture );
	m_pGrayLightTexture = pGrayLightTex;
}

void ioTexture::SetSize( int iWidth, int iHeight )
{
	m_iSrcWidth  = iWidth;
	m_iSrcHeight = iHeight;
}

void* ioTexture::Lock( int *pPitch, int iLevel, DWORD dwLockFlags, const RECT *pRect )
{
	if( !m_pTexture )
	{
		LOG.PrintTimeAndLog( 0, "ioTexture::Lock - Texture(%s) is NULL", GetName().c_str() );
		return NULL;
	}

	D3DLOCKED_RECT rcRect;

	HRESULT hr = m_pTexture->LockRect( iLevel, &rcRect, pRect, dwLockFlags );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioTexture::Lock: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioTexture::Lock - Failed(%x), %s", hr, GetName().c_str() );
		return NULL;
	}

	if( pPitch )
	{
		*pPitch = rcRect.Pitch;
	}

	return rcRect.pBits;
}

void ioTexture::UnLock( int iLevel )
{
	if( m_pTexture )
	{
		m_pTexture->UnlockRect( iLevel );
	}
}

IDirect3DTexture9* ioTexture::GetGrayTexture() const
{
	if( !HasGrayTexture() )
	{
		g_TextureMgr.BuildGrayTexture( this );
	}

	return m_pGrayTexture;
}

IDirect3DTexture9* ioTexture::GetGrayLightTexture() const
{
	if( !HasGrayLightTexture() )
	{
		g_TextureMgr.BuildGrayLightTexture( this );
	}

	return m_pGrayLightTexture;
}

void ioTexture::ReleaseIfDefaultPool()
{
	if( m_dwUsage == D3DUSAGE_DYNAMIC || m_dwUsage == D3DUSAGE_RENDERTARGET )
	{
		SAFERELEASE( m_pTexture );
		SAFERELEASE( m_pGrayTexture );
		SAFERELEASE( m_pGrayLightTexture );
	}
}

bool ioTexture::FillImage( const RECT &rcArea, BYTE *pSrcBuf )
{
	if( !m_pTexture )
		return false;

	D3DLOCKED_RECT  kDestLock;
	m_pTexture->LockRect( 0, &kDestLock, NULL, 0 );
	if( !kDestLock.pBits )
	{
		m_pTexture->UnlockRect(0);
		return false;
	}

	int iPixelWidth = GetPixelWidth( GetFormat() ) / 8;

	int iDestPitch = kDestLock.Pitch;
	BYTE *pDestPtr = (BYTE*)kDestLock.pBits;
	pDestPtr += rcArea.top * iDestPitch + rcArea.left * iPixelWidth;

	BYTE *pSrcPtr = pSrcBuf;

	int iCopyWidth  = ( rcArea.right - rcArea.left ) * iPixelWidth;
	int iCopyHeight = ( rcArea.bottom - rcArea.top );
	for( int i=0 ; i<iCopyHeight ; i++ )
	{
		ioCPU::FastMemcpy( pDestPtr, pSrcPtr, iCopyWidth );

		pSrcPtr  += iCopyWidth;
		pDestPtr += iDestPitch;
	}

	m_pTexture->UnlockRect(0);

	if( HasGrayTexture() )
	{
		g_TextureMgr.UpdateGrayTexture( this, rcArea, pSrcBuf );
	}

	if( HasGrayLightTexture() )
	{
		g_TextureMgr.UpdateGrayLightTexture( this, rcArea, pSrcBuf );
	}

	return true;
}
