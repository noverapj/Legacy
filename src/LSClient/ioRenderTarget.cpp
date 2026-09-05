#include "StdAfx.h"

#include "../io3DEngine/ioMemoryPool.h"
#include "../io3DEngine/ioRenderTexture.h"

#include "ioRenderTarget.h"
#include "ioRenderTargetManager.h"

class ioRenderTargetUIQuad : public IUIQuad, public ioPoolObject< ioRenderTargetUIQuad, 512 >
{
public:
	D3DXVECTOR2				m_vPos;
	D3DXVECTOR2				m_vSize;
	D3DXVECTOR4				m_vTexUV;
	IDirect3DTexture9*		m_pTex;
	DWORD					m_dwColor;
	UIRenderType			m_RenderType;
	TextureFilterOption		m_Filter;

public:
	ioRenderTargetUIQuad() :
		m_vPos( ioMath::VEC3_ZERO ),
		m_vSize( ioMath::VEC3_ZERO ),
		m_vTexUV( D3DXVECTOR4( 0.0f, 0.0f, FLOAT1, FLOAT1 ) ),
		m_pTex( NULL ),
		m_dwColor( 0 ),
		m_RenderType( UI_RENDER_NORMAL ), 
		m_Filter( TFO_BILINEAR )
	{
	}
	ioRenderTargetUIQuad( const D3DXVECTOR2& vPos, const D3DXVECTOR2& vSize, D3DXVECTOR4 vTexUV, IDirect3DTexture9* pTex, DWORD dwColor, UIRenderType eRenderType, TextureFilterOption eFilter ) : 
		m_vPos( vPos ),
		m_vSize( vSize ),
		m_vTexUV( vTexUV ), 
		m_pTex( pTex ), 
		m_dwColor( dwColor ),
		m_RenderType( eRenderType ),
		m_Filter( eFilter )
	{
	}
	virtual ~ioRenderTargetUIQuad()
	{
	}

	virtual int GetQuadCount() const
	{
		return 1; 
	}

	virtual void AddInfo( ioQuadRenderArray *pQuadArray ) 
	{
		pQuadArray->AddRenderQuadInfo( m_vPos, m_vSize, m_pTex, m_vTexUV, UI_RENDER_NORMAL, TFO_NEAREST, m_dwColor, 0 );
	}
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

ioRenderTarget::ioRenderTarget()
{
	m_pRenderTexture	= NULL;

	m_iXPos				= 0;
	m_iYPos				= 0;

	m_iWidth			= 0;
	m_iHeight			= 0;

	m_Index				= g_RenderTargetMgr.AddRenderTarget( this );
}

ioRenderTarget::~ioRenderTarget()
{
	g_RenderTargetMgr.RemoveRenderTarget( m_Index );
	Destroy();
}

void ioRenderTarget::Destroy()
{
	if( m_pRenderTexture )
	{
		g_TextureMgr.DestroyRTTIfNoRef( m_pRenderTexture );
		m_pRenderTexture = NULL;
	}
}

void ioRenderTarget::Create( int iWidth, int iHeight )
{
	m_iXPos			= 0;
	m_iYPos			= 0;
	m_iWidth		= iWidth;
	m_iHeight		= iHeight;

	Destroy();

	char szTextureName[MAX_PATH];
	sprintf_e( szTextureName, "_ADRenderTarget%d", m_Index );
	m_pRenderTexture = g_TextureMgr.CreateRTT( szTextureName, iWidth, iHeight, D3DFMT_A8R8G8B8, D3DFMT_D16, false );

	if( !m_pRenderTexture )
	{
		LOG.PrintTimeAndLog(0, "RenderTarget Crate Fail : %s", szTextureName );
	}
}

bool ioRenderTarget::BeginScene()
{
	if( !m_pRenderTexture )
		return false;

	ioRenderSystem* pSystem = &RenderSystem();
	m_OldView = pSystem->GetViewPort();

	D3DRECT D3DRect;
	D3DRect.x1	= m_iXPos;
	D3DRect.y1	= m_iYPos;
	D3DRect.x2	= m_iWidth;
	D3DRect.y2	= m_iHeight;

	D3DVIEWPORT9 kNewView;	
	kNewView.X		= D3DRect.x1;
	kNewView.Y		= D3DRect.y1;
	kNewView.Width  = D3DRect.x2 - D3DRect.x1;
	kNewView.Height = D3DRect.y2 - D3DRect.y1;
	kNewView.MinZ	= 0.0f;
	kNewView.MaxZ	= FLOAT1;

	pSystem->SetRenderTarget( m_pRenderTexture );
	pSystem->SetViewPort( kNewView );

	pSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, &D3DRect );
	if( !pSystem->BeginScene() )
	{
		return false;
	}

	return true;
}

void ioRenderTarget::RenderTarget()
{
}

void ioRenderTarget::OnRenderTarget()
{
	if( m_pRenderTexture )
	{
		if( BeginScene() )
		{
			RenderTarget();
			EndScene();
		}
	}
}

void ioRenderTarget::EndScene()
{
	if( !m_pRenderTexture )
		return;

	ioRenderSystem* pSystem = &RenderSystem();
	if( pSystem )
	{
#ifdef _DEBUG
		if( m_pRenderTexture )
		{
			IDirect3DSurface9* pSurface = m_pRenderTexture->GetTextureSurface();
			D3DXSaveSurfaceToFile( "_DEBUG_RENDER_TARGET_IMG.jpg", D3DXIFF_JPG, pSurface, NULL, NULL );
		}
#endif
		pSystem->EndScene();
		pSystem->SetViewPort( m_OldView );
		pSystem->RestoreRenderTarget();
	}
}

void ioRenderTarget::OnTextureRender( int iXPos, int iYPos, UIRenderType eRenderType, TextureFilterOption eFilter )
{
	if( !m_pRenderTexture )
		return;

	g_UIRenderer.AddUIQuad(
		new ioRenderTargetUIQuad
		(
			D3DXVECTOR2( iXPos, iYPos ),
			D3DXVECTOR2( m_iWidth, m_iHeight ),
			D3DXVECTOR4( 0.0f, 0.0f, FLOAT1, FLOAT1 ),
			m_pRenderTexture->GetTexture(),
			0xFFFFFFFF,
			eRenderType,
			eFilter
		)
	);
}

void ioRenderTarget::OnTextureRender( int iXPos, int iYPos, const RECT &rcRect, UIRenderType eRenderType, TextureFilterOption eFilter )
{
	D3DXVECTOR2 vDrawPos;
	vDrawPos.x = iXPos + rcRect.left;
	vDrawPos.y = iYPos + rcRect.top;

	D3DXVECTOR2 vSize;
	vSize.x = rcRect.right - rcRect.left;
	vSize.y = rcRect.bottom - rcRect.top;

	float fScaleRate = 0.0f;
	D3DXVECTOR4 vTexUV;
	fScaleRate = (float)rcRect.left/(float)m_iWidth;
	vTexUV.x = max( 0.0f, min( FLOAT1, fScaleRate ) );

	fScaleRate = (float)rcRect.top/(float)m_iHeight;
	vTexUV.y = max( 0.0f, min( FLOAT1, fScaleRate ) );

	fScaleRate = (float)rcRect.right/(float)m_iWidth;
	vTexUV.z = max( 0.0f, min( FLOAT1, fScaleRate ) );

	fScaleRate = (float)rcRect.bottom/(float)m_iHeight;
	vTexUV.w = max( 0.0f, min( FLOAT1, fScaleRate ) );

	g_UIRenderer.AddUIQuad( new ioRenderTargetUIQuad( vDrawPos, vSize, vTexUV, m_pRenderTexture->GetTexture(), 0xFFFFFFFF, eRenderType, eFilter ) );
}