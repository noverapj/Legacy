

#include "stdafx.h"

#include "io3DCommon.h"

#include "ioCamera.h"
#include "ioRenderSystem.h"

#include "ioTexture.h"
#include "ioTextureManager.h"

#include "ioVertexBufferInstance.h"
#include "ioIndexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioSkyDome.h"

ioSkyDome::ioSkyDome( const ioHashString &name ) : m_pMeshData(NULL), m_pSkyTex(NULL),
												   m_vPosition(0.0f, 0.0f, 0.0f)
{
	m_iSegment = 16;
	m_fRadius  = 10000.0f;
	m_fHeightScale = 1.0f;

	m_bFogEnable = false;
}

ioSkyDome::~ioSkyDome()
{
	DESTROY_TEXTURE( m_pSkyTex );
	Destroy();
}

void ioSkyDome::Destroy()
{
	if( m_pMeshData )
	{
		m_pMeshData->DeleteAllBuffers();
		SAFEDELETE(m_pMeshData);
	}
}

void ioSkyDome::Create( float fRadius, int iSegment, float fHeightScale )
{
	if( iSegment <= 0 )	return;
	if( fRadius <= 0.0f )	return;;

	Destroy();

	m_iSegment = (iSegment + 1 ) / 2 * 2;
	m_fRadius  = fRadius;
	m_fHeightScale = fHeightScale;

	m_pMeshData = new ioMeshData;

	GenerateVertex();
	GenerateIndex();
}

void ioSkyDome::SetSkyTexture( const ioHashString &name, bool bDefaultPath )
{
	DESTROY_TEXTURE( m_pSkyTex );

	m_pSkyTex = g_TextureMgr.CreateTexture( name,
											0,
											D3DX_DEFAULT,
											TQ_DEFAULT,
											false,
											bDefaultPath );
}

void ioSkyDome::GenerateVertex()
{
	int iVerticalCnt = ( m_iSegment / 2 ) - 1;

	float fVertAngle, fHorzAngle;
	fVertAngle = ( D3DX_PI / 2.0f ) / iVerticalCnt;
	fHorzAngle = ( D3DX_PI * 2.0f ) / m_iSegment;

	ioVtxTex *pVertex = new ioVtxTex[ 1 + m_iSegment * iVerticalCnt ];

	pVertex[0].vPos = D3DXVECTOR3( 0.0f, m_fRadius * m_fHeightScale, 0.0f );
	pVertex[0].u    = 0.0f;
	pVertex[0].v    = 0.0f;

	float fUAmt, fVAmt;
	fUAmt = 1.0f / m_iSegment;
	fVAmt = 1.0f / iVerticalCnt;

	int iVtxCnt = 1;
	float fV = fVAmt;
	for( int i=0 ; i<iVerticalCnt ; i++ )
	{
		D3DXVECTOR3 vPos( 0.0f, m_fRadius , 0.0f );

		D3DXMATRIX matRotX;
		D3DXMatrixRotationYawPitchRoll( &matRotX,
										0.0f,
										fVertAngle * ( i + 1 ),
										0.0f );
		D3DXVec3TransformCoord( &vPos, &vPos, &matRotX );
		vPos.y *= m_fHeightScale;

		float fU = 0.0f;
		for( int j=0 ; j<m_iSegment ; j++ )
		{
			D3DXMATRIX matRotY;

			D3DXMatrixRotationYawPitchRoll( &matRotY,
											fHorzAngle * j,
											0.0f,
											0.0f );

			D3DXVec3TransformCoord( &pVertex[iVtxCnt].vPos, &vPos, &matRotY );
			pVertex[iVtxCnt].u = fU;
			pVertex[iVtxCnt].v = fV;

			iVtxCnt++;
			fU += fUAmt;
		}

		fV += fVAmt;
	}

	if( (1 + m_iSegment * iVerticalCnt) != iVtxCnt )
	{
		SAFEDELETEARRAY( pVertex );
		return;
	}

	DWORD dwFVFMask = IOFVF_POSITION | IOFVF_UV0;
	m_pMeshData->InsertStreamSource( 0, dwFVFMask );

	ioVertexBufferInstance *pInstance = g_VtxBufMgr.CreateInstance( BT_MANAGED,
																	dwFVFMask,
																	iVtxCnt,
																	pVertex,
																	0,
																	ioRenderSystem::m_NoneAutoBufferUsage );

	if( pInstance )
	{
		m_pMeshData->SetBinding( 0, pInstance );
		m_pMeshData->m_iVertexCount = iVtxCnt;
	}

	SAFEDELETEARRAY( pVertex );
}

void ioSkyDome::GenerateIndex()
{
	int i, j;
	int iIdxCnt = 0;
	WORD *pIdx = new WORD[ m_iSegment * m_iSegment ];	// brute force

//HARDCODE: 반구라는 특성상 스트립을 연결함에도 추가 인덱스 필요 없음

	// 반구 제일 윗부분
	for( i=0 ; i<m_iSegment-2 ; i+=2 )
	{
		pIdx[iIdxCnt++] = i+1;
		pIdx[iIdxCnt++] = i+2;
		pIdx[iIdxCnt++] = 0;
		pIdx[iIdxCnt++] = i+3;
	}

	// 반구 제일 윗부분 마지막 부분
	pIdx[iIdxCnt++] = m_iSegment - 1;
	pIdx[iIdxCnt++] = m_iSegment;
	pIdx[iIdxCnt++] = 0;
	pIdx[iIdxCnt++] = 1;

	// 윗부분 아래의 각 링모양
	int iRingCnt = (m_iSegment/2) - 2;
	for( i=0 ; i<iRingCnt ; i++ )
	{
		int iInnerStart, iOuterStart;
		iInnerStart = i * m_iSegment + 1;
		iOuterStart = iInnerStart + m_iSegment;

		for( j=0 ; j<m_iSegment ; j++ )
		{
			pIdx[iIdxCnt++] = iInnerStart + j;
			pIdx[iIdxCnt++] = iOuterStart + j;
		}

		pIdx[iIdxCnt++] = iInnerStart;
		pIdx[iIdxCnt++] = iOuterStart;
	}

	if( iIdxCnt >= (m_iSegment*m_iSegment ) )
	{
		SAFEDELETEARRAY(pIdx);
		return;
	}

	ioIndexBufferInstance *pInstance = g_IdxBufMgr.CreateInstance( BT_MANAGED,
																   iIdxCnt,
																   pIdx,
																   0,
																   ioRenderSystem::m_NoneAutoBufferUsage );
	m_pMeshData->m_pIndexBufferInstance = pInstance;
	m_pMeshData->m_iIndexCount = iIdxCnt;

	SAFEDELETEARRAY(pIdx);
}

void ioSkyDome::Render( bool bWorldFog )
{
	ioRenderSystem &rkRSystem = RenderSystem();

	rkRSystem.SetVertexDeclaration( m_pMeshData->m_pVertexDeclaration );
	rkRSystem.SetVertexBufferBinder( m_pMeshData->m_pVertexBufferBinder );
	rkRSystem.DisableShader();

	rkRSystem.SetioTexture( 0, m_pSkyTex );

	D3DXMATRIX matWorld;
	D3DXMatrixTranslation( &matWorld, m_vPosition.x, m_vPosition.y, m_vPosition.z );

	rkRSystem.SetModelMatrix( &matWorld );
	rkRSystem.SetTextureAddressMode( 0, TAM_WRAP );
	rkRSystem.SetTextureTransformFlag( 0, D3DTTFF_DISABLE );
	rkRSystem.SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rkRSystem.DisableAlphaBlend();
	rkRSystem.DisableAlphaTest();

	rkRSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	rkRSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	rkRSystem.DisableTextureUnitsFrom( 1 );

	ioIndexBufferInstance *pIdxInstance = m_pMeshData->m_pIndexBufferInstance;
	rkRSystem.SetIndexBufferInstance( pIdxInstance );

	rkRSystem.SetSoftwareVertexProcessing( false );

	if( bWorldFog && !m_bFogEnable )
		rkRSystem.SetFogEnable( false );

	IDirect3DDevice9 *pDevice = rkRSystem.GetDevice();
	pDevice->SetRenderState( D3DRS_WRAP0, D3DWRAPCOORD_0 );
	pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP,
								   0,
								   0,
								   m_pMeshData->m_iVertexCount,
								   pIdxInstance->GetOffsetIndex(),
								   m_pMeshData->m_iIndexCount - 2 );

	pDevice->SetRenderState( D3DRS_WRAP0, 0 );

	rkRSystem.SetSoftwareVertexProcessing( true );
	if( bWorldFog && !m_bFogEnable )
		rkRSystem.SetFogEnable( true );
}

void ioSkyDome::SetFogEnable( bool bEnable )
{
	m_bFogEnable = bEnable;
}

void ioSkyDome::SetPosition( const D3DXVECTOR3 &vPosition )
{
	m_vPosition = vPosition;
}

void ioSkyDome::NotifyCurrentCamera( ioCamera *pCamera )
{
	D3DXVECTOR3 vCameraPos = pCamera->GetPosition();

	D3DXVECTOR3 vNewPos;
	vNewPos.x = vCameraPos.x;
	vNewPos.y = m_vPosition.y;
	vNewPos.z = vCameraPos.z;

	SetPosition( vNewPos );
}

const ioHashString& ioSkyDome::GetName() const
{
	return m_Name;
}

float ioSkyDome::GetDomeRadius() const
{
	return m_fRadius;
}

float ioSkyDome::GetDomeHeightScale() const
{
	return m_fHeightScale;
}

int ioSkyDome::GetDomeSegment() const
{
	return m_iSegment;
}

bool ioSkyDome::IsFogEnable() const
{
	return m_bFogEnable;
}
