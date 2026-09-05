

#include "stdafx.h"

#include "ioTexture.h"
#include "ioTextureManager.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderSystem.h"
#include "ioPostOveray.h"

#include "HelpFunc.h"

ioPostOveray::ioPostOveray( ioRenderSystem *pRenderSystem )
{
	m_pRenderSystem = pRenderSystem;

	m_pOverayTex = NULL;
	m_pRHWMeshData = NULL;

	m_iCurWindowWidth  = 800;
	m_iCurWindowHeight = 600;
}

ioPostOveray::~ioPostOveray()
{
	ClearResource();
}

void ioPostOveray::ClearResource()
{
	DESTROY_TEXTURE( m_pOverayTex );

	if( m_pRHWMeshData )
	{
		m_pRHWMeshData->DeleteAllBuffers();
		SAFEDELETE( m_pRHWMeshData );
	}
}

void ioPostOveray::SetColor( int iRed, int iGreen, int iBlue )
{
	m_dwColor = D3DCOLOR_ARGB( 0xff, iRed, iGreen, iBlue );
}

bool ioPostOveray::InitOveray( const ioHashString &szOverayTex )
{
	m_pOverayTex = g_TextureMgr.CreateTexture( szOverayTex, 0, 1 );
	if( !m_pOverayTex )
	{
		ClearResource();
		return false;
	}

	m_iCurWindowWidth  = m_pRenderSystem->GetWindowWidth();
	m_iCurWindowHeight = m_pRenderSystem->GetWindowHeight();

	if( !CreateRHWBuffer() )
	{
		ClearResource();
		return false;
	}

	return true;
}

bool ioPostOveray::CreateRHWBuffer()
{
	if( m_pRHWMeshData )
	{
		m_pRHWMeshData->DeleteAllBuffers();
		delete m_pRHWMeshData;
		m_pRHWMeshData = NULL;
	}

	ioVtxRHWTex kRHWVtx[4];
	FillVtxRHWTexRectStrip( kRHWVtx,
							0.0f,
							0.0f,
							(float)m_iCurWindowWidth,
							(float)m_iCurWindowHeight );

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

bool ioPostOveray::CheckChangedResource()
{
	if( !m_pOverayTex )
		return false;

	int iNewWidth  = m_pRenderSystem->GetWindowWidth();
	int iNewHeight = m_pRenderSystem->GetWindowHeight();

	if( m_iCurWindowWidth == iNewWidth && m_iCurWindowHeight == iNewHeight )
		return true;

	m_iCurWindowWidth  = iNewWidth;
	m_iCurWindowHeight = iNewHeight;

	if( !CreateRHWBuffer() )
	{
		ClearResource();
		return false;
	}

	return true;
}

void ioPostOveray::RenderOveray()
{
	if( !m_pOverayTex )
		return;

	if( !CheckChangedResource() )
		return;

	if( !m_pRHWMeshData )
		return;

	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	m_pRenderSystem->SetVertexDeclaration( m_pRHWMeshData->m_pVertexDeclaration );
	m_pRenderSystem->SetVertexBufferBinder( m_pRHWMeshData->m_pVertexBufferBinder );

	m_pRenderSystem->SetTexture( 0, m_pOverayTex->GetTexture() );
	m_pRenderSystem->SetTextureFactor( m_dwColor );

	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );
	m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE,   D3DTA_TEXTURE, D3DTA_TFACTOR );
	m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_TFACTOR );
	
	m_pRenderSystem->DisableTextureUnitsFrom( 1 );

	m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	m_pRenderSystem->SetAlphaTest( D3DCMP_GREATER, 0 );
	m_pRenderSystem->DisableShader();

	if( m_pRenderSystem->BeginScene() )
	{
		m_pRenderSystem->RenderPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		m_pRenderSystem->EndScene();
	}
}

