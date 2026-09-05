

#include "stdafx.h"

#include "ioShader.h"
#include "ioShaderManager.h"

#include "ioRenderTexture.h"
#include "ioTextureManager.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderSystem.h"
#include "ioPostFilter.h"

#include "HelpFunc.h"

ioPostFilter::ioPostFilter( ioRenderSystem *pRenderSystem )
{
	m_pRenderSystem = pRenderSystem;
	m_pShader = NULL;

	m_pRHWMeshData = NULL;

	m_iCurWindowWidth  = 800;
	m_iCurWindowHeight = 600;

	m_AlphaValue.r = m_AlphaValue.g = m_AlphaValue.b = m_AlphaValue.a = 1.0f;
}

ioPostFilter::~ioPostFilter()
{
	ClearResource();
}

void ioPostFilter::ClearResource()
{
	if( m_pShader )
	{
		ioShaderManager::GetSingleton().Destroy( m_pShader );
		m_pShader = NULL;
	}

	if( m_pRHWMeshData )
	{
		m_pRHWMeshData->DeleteAllBuffers();
		SAFEDELETE( m_pRHWMeshData );
	}
}

bool ioPostFilter::InitFilter( const ioHashString &szShader )
{
	if( m_pRenderSystem->GetPixelShaderVersion() < 20 )
	{
		LOG.PrintTimeAndLog( 0, "ioPostFilter::InitFilter - PixelShader is Low Ver(%d)",
								m_pRenderSystem->GetPixelShaderVersion() );
		return false;
	}

	m_pShader = ioShaderManager::GetSingleton().CreateShader( szShader );
	if( !m_pShader )
	{
		ClearResource();
		LOG.PrintTimeAndLog( 0, "ioPostFilter::InitFilter - Load Shader Failed" );
		return false;
	}

	m_iCurWindowWidth  = m_pRenderSystem->GetWindowWidth();
	m_iCurWindowHeight = m_pRenderSystem->GetWindowHeight();

	int iTexWidth  = GetTwoPowAlignSize( m_iCurWindowWidth );
	int iTexHeight = GetTwoPowAlignSize( m_iCurWindowHeight );

	if( !CreateRHWBuffer( iTexWidth, iTexHeight ) )
	{
		ClearResource();
		return false;
	}

	return true;
}

void ioPostFilter::SetAlphaValue( int iAlpha )
{
	iAlpha = max( 0, min( iAlpha, 255 ) );
	m_AlphaValue.a = (float)iAlpha/255.0f;
}

bool ioPostFilter::CreateRHWBuffer( int iTexWidth, int iTexHeight )
{
	if( m_pRHWMeshData )
	{
		m_pRHWMeshData->DeleteAllBuffers();
		delete m_pRHWMeshData;
		m_pRHWMeshData = NULL;
	}

	float fMaxU = (float)m_iCurWindowWidth  / (float)iTexWidth;
	float fMaxV = (float)m_iCurWindowHeight / (float)iTexHeight;

	ioVtxRHWTex kRHWVtx[4];
	FillVtxRHWTexRectStrip( kRHWVtx,
							0.0f,
							0.0f,
							(float)m_iCurWindowWidth,
							(float)m_iCurWindowHeight,
							fMaxU,
							fMaxV );

	DWORD dwMask = IOFVF_POSITIONW | IOFVF_UV0;

	m_pRHWMeshData = new ioMeshData;
	m_pRHWMeshData->InsertStreamSource( 0, dwMask );

	ioVertexBufferInstance *pVtxInstance = g_VtxBufMgr.CreateInstance( BT_MANAGED,
																	   dwMask,
																	   4,
																	   kRHWVtx );

	if( !pVtxInstance )
	{
		delete m_pRHWMeshData;
		m_pRHWMeshData = NULL;
		return false;
	}

	m_pRHWMeshData->SetBinding( 0, pVtxInstance );
	m_pRHWMeshData->m_iVertexCount = 4;

	return true;
}

bool ioPostFilter::CheckChangedResource( int iTexWidth, int iTexHeight )
{
	int iNewWidth  = m_pRenderSystem->GetWindowWidth();
	int iNewHeight = m_pRenderSystem->GetWindowHeight();

	if( m_iCurWindowWidth == iNewWidth && m_iCurWindowHeight == iNewHeight )
		return true;

	m_iCurWindowWidth  = iNewWidth;
	m_iCurWindowHeight = iNewHeight;

	if( !CreateRHWBuffer( iTexWidth, iTexHeight ) )
	{
		ClearResource();
		return false;
	}

	return true;
}

void ioPostFilter::RenderFilter( ioRenderTexture *pTarget )
{
	if( !pTarget )	return;
	if( !CheckChangedResource( pTarget->GetWidth(), pTarget->GetHeight() ) )
		return;

	if( !m_pShader || !m_pRHWMeshData )
		return;

	m_pShader->SetColorValue( "AlphaValue", m_AlphaValue );

	RECT rcTarget;
	rcTarget.left = 0;
	rcTarget.top  = 0;
	rcTarget.right  = m_iCurWindowWidth;
	rcTarget.bottom = m_iCurWindowHeight;

	if( !m_pRenderSystem->CopyRenderTarget( pTarget->GetTextureSurface(), &rcTarget ) )
		return;

	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	m_pRenderSystem->SetVertexDeclaration( m_pRHWMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pRHWMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->SetTexture( 0, pTarget->GetTexture() );
	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );
	m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );

	if( m_AlphaValue.a < 1.0f )
		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	else
		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ZERO );

	m_pRenderSystem->DisableTextureUnitsFrom( 1 );
	m_pRenderSystem->DisableAlphaTest();

	if( m_pRenderSystem->BeginScene() )
	{
		int iPassCnt = m_pShader->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pShader->BeginPass(iPass);
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			m_pShader->EndPass();
		}
		m_pShader->End();

		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->SetTexture( 0, NULL );
}

