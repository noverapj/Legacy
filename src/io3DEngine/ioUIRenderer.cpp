

#include "stdafx.h"

#include "ioUIRenderer.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderSystem.h"
#include "ioTexture.h"

#include "HelpFunc.h"
#include "ioFontManager.h"
#include "ioCPU.h"
#include "ErrorReport.h"
#include "ioUIRenderImage.h"

#define MAX_UI_VTX_SIZE 16384
#define VERTEX_PER_QUAD 4
#define INDEX_PER_QUAD	6
#define UI_VERTEX_FVF (IOFVF_POSITIONW | IOFVF_COLOR0 | IOFVF_UV0 | IOFVF_UV1)

template<> ioUIRenderer* Singleton< ioUIRenderer >::ms_Singleton = 0;

ioQuadRenderArray::ioQuadRenderArray( int iMaxCount )
{
	m_pRenderInfo = new QuadInfo[iMaxCount];
	m_pVtxArray = NULL;

	m_iMaxCount = iMaxCount;

	m_iDirectInfoCount = 0;
	m_iRenderInfoCount = 0;
}

ioQuadRenderArray::~ioQuadRenderArray()
{
	SAFEDELETEARRAY( m_pRenderInfo );
}

void ioQuadRenderArray::SetBuffer( ioVtxRHWColorTex2 *pBuf )
{
	m_pVtxArray = pBuf;
}

void ioQuadRenderArray::AddDirectQuadInfo( IUIQuad *pQuad )
{
	int iTotalCount = GetTotalUseCount();
	if( iTotalCount < m_iMaxCount )
	{
		m_pRenderInfo[iTotalCount].m_pTex = NULL;
		m_pRenderInfo[iTotalCount].m_pDirectQuad = pQuad;
		m_iDirectInfoCount++;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioQuadRenderArray::AddDirectQuadInfo - Overflow" );
	}
}

void ioQuadRenderArray::AddRenderQuadInfo( const D3DXVECTOR2 &vPos,
										   const D3DXVECTOR2 &vSize,
										   IDirect3DTexture9 *pTex,
										   const D3DXVECTOR4 &vTexUV,
										   UIRenderType eRenderType,
										   TextureFilterOption eFilter,
										   DWORD dwColor,
										   const ioUIRenderImage *pAlphaImage )
{
	int iTotalCount = GetTotalUseCount();
	if( iTotalCount >= m_iMaxCount )
	{
		LOG.PrintTimeAndLog( 0, "ioQuadRenderArray::AddRenderQuadInfo - Overflow" );
		return;
	}

	m_pRenderInfo[iTotalCount].m_pTex = pTex;
	m_pRenderInfo[iTotalCount].m_RenderType = eRenderType;
	m_pRenderInfo[iTotalCount].m_Filter = eFilter;
	m_pRenderInfo[iTotalCount].m_pDirectQuad = NULL;
	m_pRenderInfo[iTotalCount].m_pAlphaImage = pAlphaImage;

	D3DXVECTOR4 vExtraTex( 0.0f, 0.0f, 0.0f, 0.0f );
	if( pAlphaImage )
	{
		vExtraTex = pAlphaImage->GetTexUV();
	}

	ioVtxRHWColorTex2 *pCursor = m_pVtxArray + m_iRenderInfoCount * VERTEX_PER_QUAD;

	pCursor->vPos.x = vPos.x;
	pCursor->vPos.y = vPos.y;
	pCursor->vPos.z = 0.0f;
	pCursor->fRHW   = 1.0f;
	pCursor->dwColor = dwColor;
	pCursor->u = vTexUV.x;
	pCursor->v = vTexUV.y;
	pCursor->u2 = vExtraTex.x;
	pCursor->v2 = vExtraTex.y;
	++pCursor;

	pCursor->vPos.x = vPos.x + vSize.x;
	pCursor->vPos.y = vPos.y;
	pCursor->vPos.z = 0.0f;
	pCursor->fRHW   = 1.0f;
	pCursor->dwColor = dwColor;
	pCursor->u = vTexUV.z;
	pCursor->v = vTexUV.y;
	pCursor->u2 = vExtraTex.z;
	pCursor->v2 = vExtraTex.y;
	++pCursor;

	pCursor->vPos.x = vPos.x;
	pCursor->vPos.y = vPos.y + vSize.y;
	pCursor->vPos.z = 0.0f;
	pCursor->fRHW   = 1.0f;
	pCursor->dwColor = dwColor;
	pCursor->u = vTexUV.x;
	pCursor->v = vTexUV.w;
	pCursor->u2 = vExtraTex.x;
	pCursor->v2 = vExtraTex.w;
	++pCursor;

	pCursor->vPos.x = vPos.x + vSize.x;
	pCursor->vPos.y = vPos.y + vSize.y;
	pCursor->vPos.z = 0.0f;
	pCursor->fRHW   = 1.0f;
	pCursor->dwColor = dwColor;
	pCursor->u = vTexUV.z;
	pCursor->v = vTexUV.w;
	pCursor->u2 = vExtraTex.z;
	pCursor->v2 = vExtraTex.w;
	++pCursor;

	m_iRenderInfoCount++;
}

ioUIRenderer::ioUIRenderer()
{
	m_pQuadRenderArray = new ioQuadRenderArray( MAX_UI_VTX_SIZE / VERTEX_PER_QUAD );
	m_iPreVertexEnd = 0;
}

ioUIRenderer::~ioUIRenderer()
{
	SAFEDELETE( m_pQuadRenderArray );

	if( m_RenderOp.m_pMeshData )
	{
		m_RenderOp.m_pMeshData->DeleteAllBindingBuffers();
		m_RenderOp.m_pMeshData->m_pIndexBufferInstance = NULL;
		SAFEDELETE( m_RenderOp.m_pMeshData );
	}

	if( !m_UIQuadList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioUIRenderer::~ioUIRenderer - UIQuad Remained" );
	}
}

void ioUIRenderer::Initialize()
{
	ioMeshData *pMeshData = new ioMeshData;
	pMeshData->InsertStreamSource( 0, UI_VERTEX_FVF );

	ioVertexBufferInstance *pVtxInstance = NULL;
	pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC,
														UI_VERTEX_FVF,
														MAX_UI_VTX_SIZE,
														ioRenderSystem::m_NoneAutoBufferUsage );
	if( pVtxInstance )
	{
		pMeshData->SetBinding( 0, pVtxInstance );
	}

	pMeshData->m_pIndexBufferInstance = g_IdxBufMgr.NotifyQuadSharedBuffer( MAX_UI_VTX_SIZE / 4 );

	m_RenderOp.m_pMeshData = pMeshData;
	m_UIQuadList.reserve( 1600 );
}

void ioUIRenderer::AddUIQuad( IUIQuad *pUIQuad )
{
	if( pUIQuad )
	{
		m_UIQuadList.push_back( pUIQuad );
	}
}

int ioUIRenderer::GetNotRenderedQuadCount() const
{
	int iQuadCount = 0;

	UIQuadList::const_iterator iter=m_UIQuadList.begin();
	for( ; iter!=m_UIQuadList.end() ; ++iter )
	{
		iQuadCount += (*iter)->GetQuadCount();
	}

	return iQuadCount;
}

void ioUIRenderer::FillBuffers( ioVtxRHWColorTex2 *pBuf )
{
	m_pQuadRenderArray->Clear();
	m_pQuadRenderArray->SetBuffer( pBuf );

	UIQuadList::iterator iter=m_UIQuadList.begin();
	for( ; iter!=m_UIQuadList.end() ; ++iter )
	{
		(*iter)->AddInfo( m_pQuadRenderArray );
	}
}

void ioUIRenderer::ClearQuadList()
{
	UIQuadList::iterator iter=m_UIQuadList.begin();
	for( ; iter!=m_UIQuadList.end() ; ++iter )
	{
		delete *iter;
	}
	m_UIQuadList.erase( m_UIQuadList.begin(), m_UIQuadList.end() );
}

bool ioUIRenderer::UpdateBuffers()
{
	g_FontMgr.Flush();

	int iNeedQuadCount = GetNotRenderedQuadCount();
	if( iNeedQuadCount == 0 )
		return false;

	int iNeedVtxCnt = iNeedQuadCount * VERTEX_PER_QUAD;
	if( iNeedVtxCnt > MAX_UI_VTX_SIZE )
	{
		LOG.PrintTimeAndLog( 0, "ioUIRenderer::UpdateBuffers - Need More UI Buffers(%d)", iNeedVtxCnt );
		return false;
	}

	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;
	if( m_iPreVertexEnd + iNeedVtxCnt > MAX_UI_VTX_SIZE )
	{
		m_iPreVertexEnd = 0;
		dwLockFlags = D3DLOCK_DISCARD;
	}

	int iLockSizeBytes  = iNeedVtxCnt * sizeof(ioVtxRHWColorTex2);
	int iLockStartBytes = m_iPreVertexEnd * sizeof(ioVtxRHWColorTex2);

	ioVertexBufferInstance *pVtxBuf = m_RenderOp.m_pMeshData->GetVtxInstance(0);
	if( !pVtxBuf || !pVtxBuf->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
		return false;

	bool bCopyOK = true;

	__try
	{
		FillBuffers( (ioVtxRHWColorTex2*)pVtxBuf->GetBuffer() );
	}
	__except( ErrorReport::DynamicBufferFilter( GetExceptionCode() ) )
	{
		bCopyOK = false;
	}

	pVtxBuf->UnLock();

	if( bCopyOK )
	{
		m_RenderOp.m_iBaseVertexIndex = m_iPreVertexEnd;
		m_RenderOp.m_pMeshData->m_pIndexBufferInstance = g_IdxBufMgr.NotifyQuadSharedBuffer( m_pQuadRenderArray->GetRenderInfoCount() );

		m_iPreVertexEnd += iNeedVtxCnt;
	}

	return bCopyOK;
}

void ioUIRenderer::Render( ioRenderSystem *pRenderSystem, bool bSceneCall /* = true */ )
{
	if( !UpdateBuffers() )
	{
		ClearQuadList();
		return;
	}

	if( bSceneCall && !pRenderSystem->BeginScene() )
	{
		ClearQuadList();
		return;
	}

	int iIndexPos = 0;
	int iCurUsedIndex = 0;

	int iCurQuad = 0;
	int iQuadCount = m_pQuadRenderArray->GetTotalUseCount();
	
	bool bPreQuadIsDirect = true;
	const ioUIRenderImage *pLastAlphaImage = NULL;

	ioQuadRenderArray::QuadInfo kCurInfo;

	while( iCurQuad < iQuadCount )
	{
		kCurInfo = m_pQuadRenderArray->GetAt( iCurQuad );

		if( kCurInfo.m_pDirectQuad )
		{
			pRenderSystem->SetSoftwareVertexProcessing( true );

			kCurInfo.m_pDirectQuad->RenderDirect( pRenderSystem );
			iCurQuad++;
			bPreQuadIsDirect = true;
		}
		else
		{
			if( bPreQuadIsDirect )
			{
				InitCommonRenderState( pRenderSystem );
				pRenderSystem->SetSoftwareVertexProcessing( false );
				
				bPreQuadIsDirect = false;
				pLastAlphaImage = NULL;
			}

			m_RenderOp.m_iMinIndex   = ( iIndexPos / 3 ) * 2;
			m_RenderOp.m_iIndexStart = iIndexPos;

			iCurQuad++;
			iCurUsedIndex = INDEX_PER_QUAD;

			for( ; iCurQuad<iQuadCount ; iCurQuad++ )
			{
				if( kCurInfo.IsSameSetting( m_pQuadRenderArray->GetAt( iCurQuad ) ) )
				{
					iCurUsedIndex += INDEX_PER_QUAD;
				}
				else
				{
					break;
				}
			}

			m_RenderOp.m_iFaceCount = iCurUsedIndex / 3;
			m_RenderOp.m_iVertexCount = m_RenderOp.m_iFaceCount * 2;
			iIndexPos += iCurUsedIndex;

			pRenderSystem->SetTexture( 0, kCurInfo.m_pTex );
			pRenderSystem->SetTextureFiltering( 0, kCurInfo.m_Filter );

			switch( kCurInfo.m_RenderType )
			{
			case UI_RENDER_NORMAL:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_SCREEN:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_INVDESTCOLOR, D3DBLEND_ONE );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_ADD:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_NORMAL_ADD_COLOR:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_ADD, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_COLOR_ALPHA:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_INVSRCCOLOR );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_LIGHTEN:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_MAX, D3DBLEND_ONE, D3DBLEND_ONE );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_DARKEN:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_MIN, D3DBLEND_ONE, D3DBLEND_ONE );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_MULTIPLY:
				pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_ZERO, D3DBLEND_SRCCOLOR );
				pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
				break;
			case UI_RENDER_ADD_ALPHA:
				{
					pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
					pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
					pRenderSystem->SetTexture( 1, kCurInfo.m_pTex );
					pRenderSystem->SetTextureCoordIndex( 1, 1 );
					pRenderSystem->SetTextureStageColorBlend( 1, D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_CURRENT );
					pRenderSystem->SetTextureStageAlphaBlend( 1, D3DTOP_MODULATE, D3DTA_TFACTOR, D3DTA_CURRENT );
					//일단 용도 하나니깐 고정값으로 하고, 나중에 필요하면 외부로 빼서 쓰면 됨
					DWORD dwAlphaValue = 40;
					pRenderSystem->SetTextureFactor( D3DCOLOR_ARGB(dwAlphaValue,dwAlphaValue,dwAlphaValue,dwAlphaValue) );
					break;
				}
			}

			pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );

			if( kCurInfo.m_pAlphaImage )
			{
				pRenderSystem->SetTexture( 1, kCurInfo.m_pAlphaImage->GetTexture( UI_RENDER_NORMAL) );
				pRenderSystem->SetTextureFiltering( 1, kCurInfo.m_Filter );

				if( !pLastAlphaImage )	// 방금전까지 AlphaImage없었다.
				{
					pRenderSystem->SetTextureCoordIndex( 1, 1 );
					pRenderSystem->SetTextureStageColorBlend( 1, D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_CURRENT );
					pRenderSystem->SetTextureStageAlphaBlend( 1, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_CURRENT );
				}
			}
			else
			{
				if( pLastAlphaImage )
				{
					pRenderSystem->SetTextureCoordIndex( 1, 0 );
					pRenderSystem->DisableTextureUnitsFrom( 1 );
				}
			}

			pLastAlphaImage = kCurInfo.m_pAlphaImage;

			pRenderSystem->Render( m_RenderOp, NULL );

			if( kCurInfo.m_RenderType == UI_RENDER_ADD_ALPHA )
			{
				pRenderSystem->SetTextureCoordIndex( 1, 0 );
				pRenderSystem->DisableTextureUnitsFrom( 1 );
			}
		}
	}

	if( bSceneCall )
	{
		pRenderSystem->EndScene();
	}

	m_pQuadRenderArray->Clear();
	ClearQuadList();
}

void ioUIRenderer::InitCommonRenderState( ioRenderSystem *pRenderSystem )
{
	pRenderSystem->SetModelMatrixIdentity();

	pRenderSystem->DisableShader();
	pRenderSystem->SetLightEnable( false );
	pRenderSystem->SetDepthFunction( D3DCMP_ALWAYS, false, false );

	pRenderSystem->SetAlphaTest( D3DCMP_GREATER, 0 );
	pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	pRenderSystem->SetTextureTransformFlag( 0, D3DTTFF_DISABLE );
	pRenderSystem->SetCullingMode( CM_NO );
	pRenderSystem->DisableTextureUnitsFrom( 1 );
}

ioUIRenderer& ioUIRenderer::GetSingleton()
{
	return Singleton< ioUIRenderer >::GetSingleton();
}

ioUIRenderer* ioUIRenderer::GetSingletonPtr()
{
	return Singleton< ioUIRenderer >::ms_Singleton;
}
