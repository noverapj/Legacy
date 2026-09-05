

#include "stdafx.h"

#include "ioShader.h"
#include "ioShaderManager.h"

#include "ioRenderTexture.h"
#include "ioTextureManager.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderSystem.h"
#include "ioGlowPostProcess.h"

#include "HelpFunc.h"

ioGlowPostProcess::ioGlowPostProcess( ioRenderSystem *pRenderSystem )
{
	m_pRenderSystem = pRenderSystem;

	m_pRHWMeshData = NULL;
	m_pGaussianMeshData = NULL;
	m_pGaussian = NULL;

	m_pHBlur = NULL;
	m_pVBlur = NULL;

	m_pBlurFilter = NULL;
	m_pFilter     = NULL;

	m_iCurWindowWidth  = 800;
	m_iCurWindowHeight = 600;

	m_dwAlphaValue = 0xffffffff;
	m_dwSecondAlphaValue = 0xffffffff;

	m_fGlowness = 1.0f;
	m_fTexcel = 1.f / 256.f;
}

ioGlowPostProcess::~ioGlowPostProcess()
{
	ClearResource();
}

bool ioGlowPostProcess::InitProcess( const ioHashString &szBlurFilter, const ioHashString &szFilter )
{
	if( m_pRenderSystem->GetPixelShaderVersion() < 20 )
	{
		LOG.PrintTimeAndLog( 0, "ioGlowPostProcess::InitProcess - PixelShader is Low Ver(%d)",
								m_pRenderSystem->GetPixelShaderVersion() );
		return false;
	}

	m_pGaussian = ioShaderManager::GetSingleton().CreateShader( "GaussianBlur.fxo" );
	if( !m_pGaussian )
	{
		ClearResource();
		LOG.PrintTimeAndLog( 0, "ioGlowPostProcess::InitProcess - Load Shader Failed" );
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	m_pHBlur = ioShaderManager::GetSingleton().CreateShader( "glow_h.fxo" );
	if( !m_pHBlur )
	{
		ClearResource();
		LOG.PrintTimeAndLog( 0, "ioGlowPostProcess::InitProcess - Load Shader Failed" );
		return false;
	}
	m_pHBlur->SetFloat("fGlowness", m_fGlowness);

	m_pVBlur = ioShaderManager::GetSingleton().CreateShader( "glow_v.fxo" );
	if( !m_pHBlur )
	{
		ClearResource();
		LOG.PrintTimeAndLog( 0, "ioGlowPostProcess::InitProcess - Load Shader Failed" );
		return false;
	}
	m_pVBlur->SetFloat("fGlowness", m_fGlowness);
	//////////////////////////////////////////////////////////////////////////

	m_pBlurFilter = g_TextureMgr.CreateTexture( szBlurFilter, 0, 0 );
	if( !m_pBlurFilter )
	{
		ClearResource();
		return false;
	}

	if( !szFilter.IsEmpty() )
	{
		m_pFilter = g_TextureMgr.CreateTexture( szFilter, 0, 0 );
		if( !m_pFilter )
		{
			ClearResource();
			return false;
		}
	}

	m_iCurWindowWidth  = m_pRenderSystem->GetWindowWidth();
	m_iCurWindowHeight = m_pRenderSystem->GetWindowHeight();

	int iTexWidth  = GetTwoPowAlignSize( m_iCurWindowWidth );
	int iTexHeight = GetTwoPowAlignSize( m_iCurWindowHeight );

	if( !CreateVertexData( iTexWidth, iTexHeight ) )
	{
		ClearResource();
		return false;
	}

	return true;
}


void ioGlowPostProcess::SetGlownessAndTexcelInc( float fGlowness, float fTexcel)
{
	m_fGlowness = fGlowness;
	m_fTexcel = fTexcel;

	if( m_pHBlur )
	{
		m_pHBlur->SetFloat("fGlowness", m_fGlowness);
		m_pHBlur->SetFloat("fTexelIncrement", m_fTexcel);
	}

	if( m_pVBlur )
	{
		m_pVBlur->SetFloat("fGlowness", m_fGlowness);
		m_pVBlur->SetFloat("fTexelIncrement", m_fTexcel);
	}
}

void ioGlowPostProcess::ClearResource()
{
	if( m_pGaussian )
	{
		ioShaderManager::GetSingleton().Destroy( m_pGaussian );
		m_pGaussian = NULL;
	}

	if( m_pHBlur )
	{
		ioShaderManager::GetSingleton().Destroy( m_pHBlur );
		m_pHBlur = NULL;
	}

	if( m_pVBlur )
	{
		ioShaderManager::GetSingleton().Destroy( m_pVBlur );
		m_pVBlur = NULL;
	}

	DESTROY_TEXTURE( m_pBlurFilter );
	DESTROY_TEXTURE( m_pFilter );

	if( m_pGaussianMeshData )
	{
		m_pGaussianMeshData->DeleteAllBuffers();
		SAFEDELETE(m_pGaussianMeshData);
	}

	if( m_pRHWMeshData )
	{
		m_pRHWMeshData->DeleteAllBuffers();
		SAFEDELETE( m_pRHWMeshData );
	}
}

bool ioGlowPostProcess::CreateVertexData( int iTexWidth, int iTexHeight )
{
	if( m_pRHWMeshData )
	{
		m_pRHWMeshData->DeleteAllBuffers();
		delete m_pRHWMeshData;
		m_pRHWMeshData = NULL;
	}

	struct ioVtxRHWTex2
	{
		D3DXVECTOR3 vPos;
		float		fRHW;
		float		u, v;
		float		u2, v2;
	};

	ioVtxRHWTex2 kRHWTex2[4];

	float fMaxU = (float)m_iCurWindowWidth  / (float)iTexWidth;
	float fMaxV = (float)m_iCurWindowHeight / (float)iTexHeight;

	float fXPos = PixelAligned( 0.0f );
	float fYPos = PixelAligned( 0.0f );
	float fWidth  = (float)m_iCurWindowWidth;
	float fHeight = (float)m_iCurWindowHeight;

	kRHWTex2[0].vPos.x = fXPos;
	kRHWTex2[0].vPos.y = fYPos + fHeight;
	kRHWTex2[0].vPos.z = 0.0f;
	kRHWTex2[0].fRHW = 1.0f;
	kRHWTex2[0].u = 0.0f;
	kRHWTex2[0].v = fMaxV;
	kRHWTex2[0].u2 = 0.0f;
	kRHWTex2[0].v2 = 1.0f;

	kRHWTex2[1].vPos.x = fXPos;
	kRHWTex2[1].vPos.y = fYPos;
	kRHWTex2[1].vPos.z = 0.0f;
	kRHWTex2[1].fRHW = 1.0f;
	kRHWTex2[1].u = 0.0f;
	kRHWTex2[1].v = 0.0f;
	kRHWTex2[1].u2 = 0.0f;
	kRHWTex2[1].v2 = 0.0f;

	kRHWTex2[2].vPos.x = fXPos + fWidth;
	kRHWTex2[2].vPos.y = fYPos + fHeight;
	kRHWTex2[2].vPos.z = 0.0f;
	kRHWTex2[2].fRHW = 1.0f;
	kRHWTex2[2].u = fMaxU;
	kRHWTex2[2].v = fMaxV;
	kRHWTex2[2].u2 = 1.0f;
	kRHWTex2[2].v2 = 1.0f;

	kRHWTex2[3].vPos.x = fXPos + fWidth;
	kRHWTex2[3].vPos.y = fYPos;
	kRHWTex2[3].vPos.z = 0.0f;
	kRHWTex2[3].fRHW = 1.0f;
	kRHWTex2[3].u = fMaxU;
	kRHWTex2[3].v = 0.0f;
	kRHWTex2[3].u2 = 1.0f;
	kRHWTex2[3].v2 = 0.0f;

	m_pRHWMeshData = new ioMeshData;
	m_pRHWMeshData->InsertStreamSource( 0, IOFVF_POSITIONW | IOFVF_UV0 | IOFVF_UV1 );

	ioVertexBufferInstance *pVtxInstance = NULL;
	pVtxInstance = g_VtxBufMgr.CreateInstance( BT_MANAGED,
											   IOFVF_POSITIONW | IOFVF_UV0 | IOFVF_UV1,
											   4,
											   kRHWTex2 );

	if( !pVtxInstance )
	{
		delete m_pRHWMeshData;
		m_pRHWMeshData = NULL;
		return false;
	}

	m_pRHWMeshData->SetBinding( 0, pVtxInstance );
	m_pRHWMeshData->m_iVertexCount = 4;

	//----------------------------------------------

	if( m_pGaussianMeshData )
	{
		m_pGaussianMeshData->DeleteAllBuffers();
		delete m_pGaussianMeshData;
		m_pGaussianMeshData = NULL;
	}

	ioVtxTex kGaussianVtx[4];
	FillVtxRectFan( kGaussianVtx, fMaxU, fMaxV );

	m_pGaussianMeshData = new ioMeshData;
	m_pGaussianMeshData->InsertStreamSource( 0, IOFVF_POSITION | IOFVF_UV0 );

	pVtxInstance = g_VtxBufMgr.CreateInstance( BT_MANAGED,
											   IOFVF_POSITION | IOFVF_UV0,
											   4,
											   kGaussianVtx );

	if( !pVtxInstance )
	{
		delete m_pGaussianMeshData;
		m_pGaussianMeshData = NULL;
		return false;
	}

	m_pGaussianMeshData->SetBinding( 0, pVtxInstance );
	m_pGaussianMeshData->m_iVertexCount = 4;

	return true;
}

void ioGlowPostProcess::UpdateTexOffsetArray( int iTexWidth, int iTexHeight )
{
	ZeroMemory( m_vTexOffsetX, sizeof(D3DXVECTOR4) * 6 );
	ZeroMemory( m_vTexOffsetY, sizeof(D3DXVECTOR4) * 6 );

	float fWidth  = (float)iTexWidth;
	float fHeight = (float)iTexHeight;

	for( int i=0 ; i<3 ; i++ )
	{
		float fOffset = (float)( i - 3 );
		m_vTexOffsetX[i].x = fOffset / fWidth;
		m_vTexOffsetY[i].y = fOffset / fHeight;
	}

	for( i=0 ; i<3 ; i++ )
	{
		float fOffset = (float)( i + 1 );
		m_vTexOffsetX[i+3].x = fOffset / fWidth;
		m_vTexOffsetY[i+3].y = fOffset / fHeight;
	}
}

bool ioGlowPostProcess::CheckChangedResource( int iTexWidth, int iTexHeight )
{
	int iNewWidth  = m_pRenderSystem->GetWindowWidth();
	int iNewHeight = m_pRenderSystem->GetWindowHeight();

	if( m_iCurWindowWidth == iNewWidth && m_iCurWindowHeight == iNewHeight )
		return true;

	m_iCurWindowWidth  = iNewWidth;
	m_iCurWindowHeight = iNewHeight;

	if( !CreateVertexData( iTexWidth, iTexHeight ) )
	{
		ClearResource();
		return false;
	}

	return true;
}





void ioGlowPostProcess::ApplyHBlur( ioRenderTexture *pFirst, ioRenderTexture *pSecond )
{
	if( !m_pHBlur )
		return;

	m_pRenderSystem->SetVertexDeclaration( m_pGaussianMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pGaussianMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->DisableTextureUnitsFrom( 1 );

	m_pRenderSystem->DisableAlphaBlend();
	m_pRenderSystem->DisableAlphaTest();

	D3DVIEWPORT9 kOldViewPort, kNewViewPort;
	kOldViewPort = m_pRenderSystem->GetViewPort();

	kNewViewPort.X = 0;
	kNewViewPort.Y = 0;
	kNewViewPort.Width  = m_iCurWindowWidth;
	kNewViewPort.Height = m_iCurWindowHeight;
	kNewViewPort.MinZ = 0.0f;
	kNewViewPort.MaxZ = 1.0f;

	//iTech = max( 0, min( iTech, 2 ) );
	m_pHBlur->ValidateTechnique( 0 );

	ioHashString szSrcMap( "SrcMap" );
	ioHashString szTexOffsetArray( "TexOffsetArray" );

	m_pRenderSystem->SetRenderTarget( pSecond );
	m_pRenderSystem->SetViewPort( kNewViewPort );

	UpdateTexOffsetArray( pFirst->GetWidth(), pFirst->GetHeight() );

	if( m_pRenderSystem->BeginScene() )
	{
		m_pHBlur->SetTexture( szSrcMap, pFirst->GetTexture() );
		m_pHBlur->SetVectorArray( szTexOffsetArray, m_vTexOffsetX, 6 );

		int iPassCnt = m_pHBlur->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pHBlur->BeginPass( iPass );
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			m_pHBlur->EndPass();
		}
		m_pHBlur->End();
		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->SetRenderTarget( pFirst );
	m_pRenderSystem->SetViewPort( kNewViewPort );

	if( m_pRenderSystem->BeginScene() )
	{
		m_pHBlur->SetTexture( szSrcMap, pSecond->GetTexture() );
		m_pHBlur->SetVectorArray( szTexOffsetArray, m_vTexOffsetY, 6 );

		int iPassCnt = m_pHBlur->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pHBlur->BeginPass(iPass);
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			m_pHBlur->EndPass();
		}
		m_pHBlur->End();
		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->RestoreRenderTarget();
	m_pRenderSystem->SetViewPort( kOldViewPort );
}


void ioGlowPostProcess::ApplyVBlur( ioRenderTexture *pFirst, ioRenderTexture *pSecond )
{
	if( !m_pHBlur )
		return;

	m_pRenderSystem->SetVertexDeclaration( m_pGaussianMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pGaussianMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->DisableTextureUnitsFrom( 1 );

	m_pRenderSystem->DisableAlphaBlend();
	m_pRenderSystem->DisableAlphaTest();

	D3DVIEWPORT9 kOldViewPort, kNewViewPort;
	kOldViewPort = m_pRenderSystem->GetViewPort();

	kNewViewPort.X = 0;
	kNewViewPort.Y = 0;
	kNewViewPort.Width  = m_iCurWindowWidth;
	kNewViewPort.Height = m_iCurWindowHeight;
	kNewViewPort.MinZ = 0.0f;
	kNewViewPort.MaxZ = 1.0f;

	//iTech = max( 0, min( iTech, 2 ) );
	m_pVBlur->ValidateTechnique( 0 );

	ioHashString szSrcMap( "SrcMap" );
	ioHashString szTexOffsetArray( "TexOffsetArray" );

	m_pRenderSystem->SetRenderTarget( pSecond );
	m_pRenderSystem->SetViewPort( kNewViewPort );

	UpdateTexOffsetArray( pFirst->GetWidth(), pFirst->GetHeight() );

	if( m_pRenderSystem->BeginScene() )
	{
		m_pVBlur->SetTexture( szSrcMap, pFirst->GetTexture() );
		m_pVBlur->SetVectorArray( szTexOffsetArray, m_vTexOffsetX, 6 );

		int iPassCnt = m_pVBlur->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pVBlur->BeginPass( iPass );
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			m_pVBlur->EndPass();
		}
		m_pVBlur->End();
		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->SetRenderTarget( pFirst );
	m_pRenderSystem->SetViewPort( kNewViewPort );

	if( m_pRenderSystem->BeginScene() )
	{
		m_pVBlur->SetTexture( szSrcMap, pSecond->GetTexture() );
		m_pVBlur->SetVectorArray( szTexOffsetArray, m_vTexOffsetY, 6 );

		int iPassCnt = m_pVBlur->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pVBlur->BeginPass(iPass);
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			m_pVBlur->EndPass();
		}
		m_pVBlur->End();
		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->RestoreRenderTarget();
	m_pRenderSystem->SetViewPort( kOldViewPort );
}


void ioGlowPostProcess::ApplyGaussianBlur( ioRenderTexture *pFirst,
										   ioRenderTexture *pSecond,
										   int iTech )
{
	if( !m_pGaussian || !m_pGaussianMeshData )
		return;

	m_pRenderSystem->SetVertexDeclaration( m_pGaussianMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pGaussianMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->DisableTextureUnitsFrom( 1 );

	m_pRenderSystem->DisableAlphaBlend();
	m_pRenderSystem->DisableAlphaTest();

	D3DVIEWPORT9 kOldViewPort, kNewViewPort;
	kOldViewPort = m_pRenderSystem->GetViewPort();

	kNewViewPort.X = 0;
	kNewViewPort.Y = 0;
	kNewViewPort.Width  = m_iCurWindowWidth;
	kNewViewPort.Height = m_iCurWindowHeight;
	kNewViewPort.MinZ = 0.0f;
	kNewViewPort.MaxZ = 1.0f;

	iTech = max( 0, min( iTech, 2 ) );
	m_pGaussian->ValidateTechnique( iTech );

	ioHashString szSrcMap( "SrcMap" );
	ioHashString szTexOffsetArray( "TexOffsetArray" );

	m_pRenderSystem->SetRenderTarget( pSecond );
	m_pRenderSystem->SetViewPort( kNewViewPort );

	UpdateTexOffsetArray( pFirst->GetWidth(), pFirst->GetHeight() );

	if( m_pRenderSystem->BeginScene() )
	{
		m_pGaussian->SetTexture( szSrcMap, pFirst->GetTexture() );
		m_pGaussian->SetVectorArray( szTexOffsetArray, m_vTexOffsetX, 6 );

		int iPassCnt = m_pGaussian->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pGaussian->BeginPass( iPass );
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			m_pGaussian->EndPass();
		}
		m_pGaussian->End();
		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->SetRenderTarget( pFirst );
	m_pRenderSystem->SetViewPort( kNewViewPort );

	if( m_pRenderSystem->BeginScene() )
	{
		m_pGaussian->SetTexture( szSrcMap, pSecond->GetTexture() );
		m_pGaussian->SetVectorArray( szTexOffsetArray, m_vTexOffsetY, 6 );

		int iPassCnt = m_pGaussian->Begin();
		for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
		{
			m_pGaussian->BeginPass(iPass);
			m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			m_pGaussian->EndPass();
		}
		m_pGaussian->End();
		m_pRenderSystem->EndScene();
	}

	m_pRenderSystem->RestoreRenderTarget();
	m_pRenderSystem->SetViewPort( kOldViewPort );
}

void ioGlowPostProcess::SetAlphaValue( int iAlpha )
{
	iAlpha = max( 0, min( iAlpha, 255 ) );
	m_dwAlphaValue = D3DCOLOR_ARGB( iAlpha, 255, 255, 255 );
}

void ioGlowPostProcess::SetSecondAlphaValue( int iAlpha )
{
	iAlpha = max( 0, min( iAlpha, 255 ) );
	m_dwSecondAlphaValue = D3DCOLOR_ARGB( iAlpha, 255, 255, 255 );
}

void ioGlowPostProcess::CalcLocalGlow( ioRenderTexture *pFirst, ioRenderTexture *pSecond )
{
	if( !pFirst || !pSecond )	return;

	if( !CheckChangedResource( pFirst->GetWidth(), pFirst->GetHeight() ) )
		return;

	if( !m_pGaussian || !m_pFilter || !m_pBlurFilter )
		return;

	RECT rcTarget;
	rcTarget.left = 0;
	rcTarget.top  = 0;
	rcTarget.right  = m_iCurWindowWidth;
	rcTarget.bottom = m_iCurWindowHeight;

	//D3DXSaveSurfaceToFile( "C:/TEST/first1.BMP", D3DXIFF_BMP, pFirst->GetTextureSurface(), NULL, NULL );

	if( !m_pRenderSystem->CopyRenderTarget( pFirst->GetTextureSurface(), &rcTarget ) )
		return;

	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_TRILINEAR );

	ApplyGaussianBlur( pFirst, pSecond, 0 );
	ApplyHBlur(pFirst, pSecond);
	ApplyVBlur(pFirst, pSecond);

	return;
}

void ioGlowPostProcess::RenderProcess( ioRenderTexture *pFirst, ioRenderTexture *pSecond )
{
	if( !pFirst || !pSecond )	return;

	if( !CheckChangedResource( pFirst->GetWidth(), pFirst->GetHeight() ) )
		return;

	if( !m_pGaussian || !m_pFilter || !m_pBlurFilter )
		return;

	RECT rcTarget;
	rcTarget.left = 0;
	rcTarget.top  = 0;
	rcTarget.right  = m_iCurWindowWidth;
	rcTarget.bottom = m_iCurWindowHeight;

	if( !m_pRenderSystem->CopyRenderTarget( pFirst->GetTextureSurface(), &rcTarget ) )
		return;

	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );

	ApplyGaussianBlur( pFirst, pSecond, 0 );

	m_pRenderSystem->SetVertexDeclaration( m_pRHWMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pRHWMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->DisableShader();
	m_pRenderSystem->DisableAlphaTest();

	if( m_pRenderSystem->BeginScene() )
	{
		// Gaussian Filter
		m_pRenderSystem->SetTexture( 0, pFirst->GetTexture() );
		m_pRenderSystem->SetTexture( 1, m_pBlurFilter->GetTexture() );

		m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
		m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );
		m_pRenderSystem->SetTextureAddressMode( 1, TAM_CLAMP );
		m_pRenderSystem->SetTextureFiltering( 1, TFO_BILINEAR );

		m_pRenderSystem->SetTextureFactor( m_dwAlphaValue );
		m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_CURRENT );
		m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_CURRENT );
		m_pRenderSystem->SetTextureStageColorBlend( 1, D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TFACTOR );
		m_pRenderSystem->SetTextureStageAlphaBlend( 1, D3DTOP_MODULATE,   D3DTA_TEXTURE, D3DTA_TFACTOR );
		m_pRenderSystem->SetTextureCoordIndex( 1, 1 );

		m_pRenderSystem->DisableTextureUnitsFrom( 2 );

		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		
		m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		// Multiply Filter
		m_pRenderSystem->SetTexture( 0, pFirst->GetTexture() );
		m_pRenderSystem->SetTexture( 1, m_pFilter->GetTexture() );

		m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE,   D3DTA_TEXTURE, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureStageColorBlend( 1, D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureStageAlphaBlend( 1, D3DTOP_SELECTARG2, D3DTA_CURRENT, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureCoordIndex( 1, 1 );

		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
		m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_pRenderSystem->EndScene();

		m_pRenderSystem->SetTexture( 0, NULL );
		m_pRenderSystem->SetTexture( 1, NULL );
		m_pRenderSystem->SetTextureCoordIndex( 1, 0 );
	}

	if( ( m_dwSecondAlphaValue & 0xff000000 ) == 0 )
		return;

	// 2Â÷ ºí·¯
	if( !m_pRenderSystem->CopyRenderTarget( pFirst->GetTextureSurface(), &rcTarget ) )
		return;

	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );

	ApplyGaussianBlur( pFirst, pSecond, 0 );

	m_pRenderSystem->SetVertexDeclaration( m_pRHWMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pRHWMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->DisableShader();
	m_pRenderSystem->DisableAlphaTest();

	if( m_pRenderSystem->BeginScene() )
	{
		m_pRenderSystem->SetTexture( 0, pFirst->GetTexture() );

		m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
		m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );

		m_pRenderSystem->SetTextureFactor( m_dwSecondAlphaValue );
		m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_CURRENT );
		m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_TFACTOR );
		m_pRenderSystem->DisableTextureUnitsFrom( 1 );

		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
		m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		m_pRenderSystem->EndScene();

		m_pRenderSystem->SetTexture( 0, NULL );
	}
}


void ioGlowPostProcess::RenderLocalGlow( ioRenderTexture *pFirst, ioRenderTexture *pSecond )
{
	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );

	m_pRenderSystem->SetVertexDeclaration( m_pRHWMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pRHWMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->DisableShader();
	m_pRenderSystem->DisableAlphaTest();

	if( m_pRenderSystem->BeginScene() )
	{
		// Gaussian Filter
		/*m_pRenderSystem->SetTexture( 0, pFirst->GetTexture() );
		m_pRenderSystem->SetTexture( 1, m_pBlurFilter->GetTexture() );

		m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
		m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );
		m_pRenderSystem->SetTextureAddressMode( 1, TAM_CLAMP );
		m_pRenderSystem->SetTextureFiltering( 1, TFO_BILINEAR );

		//m_pRenderSystem->SetTextureFactor( m_dwSecondAlphaValue );
		m_pRenderSystem->SetTextureFactor( m_dwLocalGlowValue );
		m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_CURRENT );
		m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_CURRENT );
		m_pRenderSystem->SetTextureStageColorBlend( 1, D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TFACTOR );
		m_pRenderSystem->SetTextureStageAlphaBlend( 1, D3DTOP_MODULATE,   D3DTA_TEXTURE, D3DTA_TFACTOR );
		m_pRenderSystem->SetTextureCoordIndex( 1, 1 );

		m_pRenderSystem->DisableTextureUnitsFrom( 2 );
		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );		
		m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );*/

		// Multiply Filter
		m_pRenderSystem->SetTexture( 0, pFirst->GetTexture() );

		//D3DXSaveSurfaceToFile( "C:/TEST/first.BMP", D3DXIFF_BMP, pFirst->GetTextureSurface(), NULL, NULL );
		//m_pRenderSystem->SetTexture( 1, m_pFilter->GetTexture() );

		//m_pRenderSystem->SetTextureFactor( m_dwLocalGlowValue );

		m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE,   D3DTA_TEXTURE, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureStageColorBlend( 1, D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureStageAlphaBlend( 1, D3DTOP_SELECTARG2, D3DTA_CURRENT, D3DTA_TEXTURE );
		m_pRenderSystem->SetTextureCoordIndex( 1, 1 );

		//m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
		//m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_DESTALPHA );
		m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_pRenderSystem->EndScene();

		m_pRenderSystem->SetTexture( 0, NULL );
		m_pRenderSystem->SetTexture( 1, NULL );
		m_pRenderSystem->SetTextureCoordIndex( 1, 0 );
	}
}
