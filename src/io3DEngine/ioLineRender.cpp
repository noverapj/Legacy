

#include "stdafx.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderSystem.h"
#include "ioLineRender.h"
#include "ErrorReport.h"
#include "ioUIRenderer.h"

#define LINE_FVF (IOFVF_POSITIONW | IOFVF_COLOR0)

class ioLineQuad : public IUIQuad
{
protected:
	ioLineRender *m_pLineRender;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray )
	{
		pQuadArray->AddDirectQuadInfo( this );
	}

	virtual int GetQuadCount() const
	{
		return 0;
	}

public:
	virtual bool IsDirectQuad() const
	{
		return true;
	}

	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
		if( m_pLineRender )
		{
			m_pLineRender->RenderNow( pRenderSystem );
		}
	}

public:
	ioLineQuad( ioLineRender *pRender )
	{
		m_pLineRender = pRender;
	}
	
	virtual ~ioLineQuad(){}
};

ioLineRender::ioLineRender()
{
	m_dwLineColor = 0xff00ff00;
	m_iRenderVtx = 0;
	m_iLockStartVertex = 0;

	m_pVtxData = NULL;
	m_bUpdateBufferOK = false;
}

ioLineRender::~ioLineRender()
{
	DestroyResource();
}

void ioLineRender::UpdateBuffers( const D3DXVECTOR2 *pSrcBuf, int iVtxCnt )
{
	if( iVtxCnt <= 0 || !CheckBufferSize( iVtxCnt ) )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	m_iRenderVtx = iVtxCnt;

	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;
	if( m_iLockStartVertex + iVtxCnt > m_pVtxData->m_iVertexCount )
	{
		m_iLockStartVertex = 0;
		dwLockFlags = D3DLOCK_DISCARD;
	}

	int iLockStartBytes = m_iLockStartVertex * sizeof(ioVtxRHWColor);
	int iLockSizeBytes  = iVtxCnt * sizeof(ioVtxRHWColor);

	ioVertexBufferInstance *pVtxInstance = m_pVtxData->GetVtxInstance( 0 );
	if( !pVtxInstance->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	__try
	{
		ioVtxRHWColor *pBuf = (ioVtxRHWColor*)pVtxInstance->GetBuffer();
		for( int i=0 ; i<m_iRenderVtx ; i++ )
		{
			pBuf[i].vPos.x = pSrcBuf[i].x;
			pBuf[i].vPos.y = pSrcBuf[i].y;
			pBuf[i].vPos.z = 1.0f;
			pBuf[i].fRHW   = 1.0f;
			pBuf[i].dwColor = m_dwLineColor;
		}

		m_bUpdateBufferOK = true;
	}
	__except( ErrorReport::DynamicBufferFilter( GetExceptionCode() ) )
	{
		m_bUpdateBufferOK = false;
	}

	pVtxInstance->UnLock();
	m_iLockStartVertex += iVtxCnt;
}

void ioLineRender::Render()
{
	if( !m_pVtxData || !m_bUpdateBufferOK )
		return;

	if( m_iRenderVtx <=0 || m_iLockStartVertex < m_iRenderVtx )
		return;

	ioLineQuad *pQuad = new ioLineQuad( this );
	if( pQuad )
	{
		g_UIRenderer.AddUIQuad( pQuad );
	}
}

void ioLineRender::RenderNow( ioRenderSystem *pSystem )
{
	if( !m_pVtxData || !m_bUpdateBufferOK )
		return;

	if( m_iRenderVtx <=0 || m_iLockStartVertex < m_iRenderVtx )
		return;

	pSystem->SetModelMatrixIdentity();
	pSystem->SetVertexDeclaration( m_pVtxData->m_pVertexDeclaration );
	pSystem->SetVertexBufferBinder( m_pVtxData->m_pVertexBufferBinder );
	pSystem->DisableShader();

	pSystem->SetLightEnable( false );
	pSystem->SetDepthFunction( D3DCMP_LESSEQUAL, false, false );
	
	pSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	pSystem->DisableAlphaTest();

	pSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pSystem->DisableTextureUnitsFrom( 1 );
	
	pSystem->RenderPrimitive( D3DPT_LINESTRIP,
							  m_iLockStartVertex - m_iRenderVtx,
							  m_iRenderVtx - 1 );
}

bool ioLineRender::CheckBufferSize( int iNeedVtx )
{
	iNeedVtx *= 3;

	if( !m_pVtxData )
	{
		m_pVtxData = new ioMeshData;
		m_pVtxData->InsertStreamSource( 0, LINE_FVF );

		ioVertexBufferInstance *pInstance = NULL;
		pInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC, LINE_FVF, iNeedVtx );
		if( pInstance )
		{
			m_pVtxData->SetBinding( 0, pInstance );
			m_pVtxData->m_iVertexCount = iNeedVtx;
			m_iLockStartVertex = 0;
		}
		else
		{
			DestroyResource();
			return false;
		}
	}
	else
	{
		if( m_pVtxData->m_iVertexCount < iNeedVtx )
		{
			ioVertexBufferInstance *pInstance = m_pVtxData->GetVtxInstance(0);
			if( pInstance )
			{
				g_VtxBufMgr.DestroyInstance( pInstance );
				pInstance = NULL;
			}

			pInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC, LINE_FVF, iNeedVtx );
			if( pInstance )
			{
				m_pVtxData->SetBinding( 0, pInstance );
				m_pVtxData->m_iVertexCount = iNeedVtx;
				m_iLockStartVertex = 0;
			}
			else
			{
				DestroyResource();
				return false;
			}
		}
	}

	return true;
}

void ioLineRender::DestroyResource()
{
	if( m_pVtxData )
	{
		m_pVtxData->DeleteAllBuffers();
		SAFEDELETE(m_pVtxData);
	}
}

void ioLineRender::SetAlpha( int iAlpha )
{
	iAlpha = max( 0, min( iAlpha, 255 ) );
	m_dwLineColor = ( iAlpha << 24 ) | ( m_dwLineColor & 0xffffff );
}

void ioLineRender::SetLineColor( int iRed, int iGreen, int iBlue )
{
	SetLineColor( D3DCOLOR_XRGB( iRed, iGreen, iBlue ) );
}

void ioLineRender::SetLineColor( DWORD dwLineColor )
{
	m_dwLineColor = ( m_dwLineColor & 0xff000000 ) | ( dwLineColor & 0xffffff );
}






