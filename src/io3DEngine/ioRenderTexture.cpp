

#include "stdafx.h"

#include "ioRenderTexture.h"
#include "ErrorReport.h"

ioRenderTexture::ioRenderTexture( const ioHashString &name ) : ioTexture( name )
{
	m_pTextureSurf = NULL;

	m_pDepthTexture = NULL;
	m_pDepthSurf = NULL;

	m_DepthFormat = D3DFMT_UNKNOWN;
	m_bUseDepthTexture = false;
}

ioRenderTexture::~ioRenderTexture()
{
	ReleaseIfDefaultPool();
}

void ioRenderTexture::SetTexture( IDirect3DTexture9 *pTex )
{
	SAFERELEASE( m_pTextureSurf );
	SAFERELEASE( m_pTexture );

	if( pTex )
	{
		m_pTexture = pTex;

		HRESULT hr = m_pTexture->GetSurfaceLevel( 0, &m_pTextureSurf );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "SetTexture: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderTexture::SetTexture - %s Failed(%x)", m_Name.c_str(), hr );
		}
	}
}

void ioRenderTexture::ReleaseIfDefaultPool()
{
	SAFERELEASE( m_pTextureSurf );
	SAFERELEASE( m_pTexture );

	SAFERELEASE( m_pDepthSurf );
	SAFERELEASE( m_pDepthTexture );
}

void ioRenderTexture::SetDepthTexture( IDirect3DTexture9 *pDepthTex )
{
	SAFERELEASE( m_pDepthSurf );
	SAFERELEASE( m_pDepthTexture );

	if( pDepthTex )
	{
		m_pDepthTexture = pDepthTex;

		HRESULT hr = m_pDepthTexture->GetSurfaceLevel( 0, &m_pDepthSurf );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "SetDepthTexture: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderTexture::SetDepthTexture - %s Failed(%x)", m_Name.c_str(), hr );
		}

		m_bUseDepthTexture = true;
	}
}

void ioRenderTexture::SetDepthSurface( IDirect3DSurface9 *pSurface )
{
	SAFERELEASE( m_pDepthSurf );
	m_pDepthSurf = pSurface;
}