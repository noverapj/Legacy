
#include "stdafx.h"

#include "../io3DEngine/ioRenderOperation.h"

#include "ioScreenBlind.h"

ioScreenBlind::ioScreenBlind()
{
	m_pMaterial = NULL;
	m_dwTFactor = 0xff000000;
}

ioScreenBlind::~ioScreenBlind()
{
	SAFEDELETE( m_pMaterial );
}

void ioScreenBlind::InitResource()
{
	m_pMaterial = new ioMaterial( "_ScreenBlend" );

	ioPass *pPass = m_pMaterial->CreateTechnique()->CreatePass();
	pPass->SetUseQuadBuffer( true );
	pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	pPass->SetTextureFactor( m_dwTFactor );
	pPass->SetLightingEnable( false );
	pPass->SetDepthWriteEnable( false );
	pPass->SetDepthCheckEnable( false );

	pPass->SetShaderGroup( "GlowQuad" );

	ioTextureUnitState *pState = pPass->CreateTextureUnitState();
	pState->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	pState->SetAlphaStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
}

void ioScreenBlind::SetAlphaRate( int iAlpha )
{
	m_dwTFactor = ( (DWORD)iAlpha << 24 ) | ( m_dwTFactor & 0xffffff );
}

void ioScreenBlind::SetColor( int iRed, int iGreen, int iBlue )
{
	m_dwTFactor = D3DCOLOR_ARGB( (m_dwTFactor >> 24), iRed, iGreen, iBlue );
}

void ioScreenBlind::SetTextureFactor( DWORD dwFactor )
{
	m_dwTFactor = dwFactor;

	if( m_pMaterial )
	{
		ioPass *pPass = m_pMaterial->GetTechnique(0)->GetPass(0);
		if( pPass )
		{
			pPass->SetTextureFactor( m_dwTFactor );
		}
	}
}

ioMaterial* ioScreenBlind::GetMaterial() const
{
	return m_pMaterial;
}

float ioScreenBlind::GetSquaredViewDepth( const ioCamera *pCamera )
{
	return 0.0f;
}

void ioScreenBlind::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	D3DXMatrixIdentity( pMatWorld );
}

void ioScreenBlind::GetRenderOperation( ioRenderOperation &rkRenderOp )
{
	rkRenderOp.m_pMeshData = NULL;
	rkRenderOp.m_PrimitiveType = D3DPT_TRIANGLEFAN;
	rkRenderOp.m_iMinIndex = 0;
	rkRenderOp.m_iVertexCount = 4;
	rkRenderOp.m_iBaseVertexIndex = 0;
	rkRenderOp.m_iIndexStart = 0;
	rkRenderOp.m_iFaceCount = 2;
	rkRenderOp.m_bUseIndex = false;
}
