

#include "stdafx.h"

#include "../io3DEngine/ioMeshData.h"
#include "../io3DEngine/ioVertexBufferInstance.h"
#include "../io3DEngine/ioVertexBufferManager.h"

#include "io2DCircleGauge.h"

io2DCircleGauge::io2DCircleGauge()
{
	m_pGaugeData = NULL;
	m_pGaugeTex  = NULL;

	m_iRenderX = -1;
	m_iRenderY = -1;

	m_iMaxCount  = 0;
	m_iFrontSkip = 0;
	m_iBackSkip  = 0;
	m_iCurRenderCount = -1;

	m_eStartType = ST_6OCLOCK;
}

io2DCircleGauge::~io2DCircleGauge()
{
	if( m_pGaugeData )
	{
		m_pGaugeData->DeleteAllBindingBuffers();

		delete m_pGaugeData;
		m_pGaugeData = NULL;
	}

	DESTROY_TEXTURE( m_pGaugeTex );
}

void io2DCircleGauge::InitResource( const char *szTexFileName,
									int iDivideCount,
									int iFrontSkip,
									int iBackSkip, 
									StartType eType )
{
	assert( iDivideCount > 0 );
	assert( iDivideCount > iFrontSkip + iBackSkip );

	DESTROY_TEXTURE( m_pGaugeTex );
	m_pGaugeTex = g_TextureMgr.CreateTexture( szTexFileName, 0, 1, TQ_HIGH );

	m_iMaxCount  = iDivideCount;
	m_iFrontSkip = iFrontSkip;
	m_iBackSkip  = iBackSkip;
	m_iCurRenderCount = 0;
	m_eStartType = eType;

	SAFEDELETE( m_pGaugeData );

	// Triangle Fan => VertexCount = FaceCount + 2
	int iNeedVtxCount = ( m_iMaxCount - m_iFrontSkip - m_iBackSkip ) + 2;

	m_pGaugeData = new ioMeshData;
	m_pGaugeData->InsertStreamSource( 0, IOFVF_POSITIONW | IOFVF_UV0 );

	ioVertexBufferInstance *pVtxInstance = NULL;
	pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_MANAGED,
														IOFVF_POSITIONW | IOFVF_UV0,
														iNeedVtxCount );

	m_pGaugeData->SetBinding( 0, pVtxInstance );
	m_pGaugeData->m_iVertexCount = iNeedVtxCount;
}

void io2DCircleGauge::UpdateGauge( int iXPos, int iYPos, float fRate )
{
	fRate = max( 0.0f, min( fRate, FLOAT1 ) );

	int iMaxRenderFace = m_iMaxCount - m_iFrontSkip - m_iBackSkip;
	m_iCurRenderCount = (int)( (float)iMaxRenderFace * fRate );

	// RHW출력이기때문에 위치가 바뀌면 버퍼를 갱신해야한다.
	if( m_iRenderX == iXPos && m_iRenderY == iYPos )
		return;

	if( !m_pGaugeTex || !m_pGaugeTex->IsLoaded() )
		return;

	ioVertexBufferInstance *pInstance = m_pGaugeData->GetVtxInstance(0);
	if( !pInstance )	return;
	if( !pInstance->Lock() )	return;

	int iTexWidth  = m_pGaugeTex->GetWidth();
	int iTexHeight = m_pGaugeTex->GetHeight();

	float fHalfWidth  = (float)iTexWidth * FLOAT05;
	float fHalfHeight = (float)iTexHeight * FLOAT05;

	float fCenterX = (float)iXPos + fHalfWidth;
	float fCenterY = (float)iYPos + fHalfHeight;

	ioVtxRHWTex *pBuf = (ioVtxRHWTex*)pInstance->GetBuffer();

	// Center
	pBuf[0].vPos.x = fCenterX;
	pBuf[0].vPos.y = fCenterY;
	pBuf[0].vPos.z = 0.01f;
	pBuf[0].fRHW = FLOAT1;
	pBuf[0].u = FLOAT05;
	pBuf[0].v = FLOAT05;

	float fStart = 270.0f;
	if( m_eStartType == ST_6OCLOCK )
		fStart = 270.0f;
	else if( m_eStartType == ST_12OCLOCK )
		fStart =FLOAT90;

	// Divide Points
	//float fRadOffset = DEGtoRAD( 360.0f / (float)( m_iMaxCount + 1 ) );
	float fRadOffset = DEGtoRAD( 360.0f / (float)m_iMaxCount );
	float fCurRad = DEGtoRAD( fStart ) - ( fRadOffset * (float)m_iFrontSkip );

	int iCurIdx = 1;
	int iLastCount = m_iMaxCount - m_iBackSkip + 1;

	for( int i=m_iFrontSkip ; i<iLastCount ; i++ )
	{
		float fXRate = cosf( fCurRad );
		float fYRate = sinf( fCurRad );

		pBuf[iCurIdx].vPos.x = fCenterX + ( fXRate * fHalfWidth );
		pBuf[iCurIdx].vPos.y = fCenterY - ( fYRate * fHalfHeight );
		pBuf[iCurIdx].vPos.z = 0.01f;
		pBuf[iCurIdx].fRHW = FLOAT1;
		pBuf[iCurIdx].u = FLOAT05 + ( fXRate * FLOAT05 );
		pBuf[iCurIdx].v = FLOAT05 - ( fYRate * FLOAT05 );

		iCurIdx++;
		fCurRad -= fRadOffset;
	}

	pInstance->UnLock();

	m_iRenderX = iXPos;
	m_iRenderY = iYPos;
}

bool io2DCircleGauge::IsNeedRender() const
{
	if( !m_pGaugeData )	return false;
	if( m_iCurRenderCount <= 0 ) return false;
	if( !m_pGaugeTex || !m_pGaugeTex->IsLoaded() )
		return false;

	return true;
}

void io2DCircleGauge::Render( ioRenderSystem *pSystem )
{
	if( !IsNeedRender() )
		return;

	pSystem->SetModelMatrixIdentity();
	pSystem->SetVertexDeclaration( m_pGaugeData->m_pVertexDeclaration );
	pSystem->SetVertexBufferBinder( m_pGaugeData->m_pVertexBufferBinder );
	pSystem->SetIndexBufferInstance( NULL );
	pSystem->DisableShader();

	pSystem->SetDepthFunction( D3DCMP_LESSEQUAL, false, true );
	pSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	pSystem->SetCullingMode( CM_CCW );
	pSystem->SetAlphaTest( D3DCMP_GREATER, 0 );

	pSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	pSystem->SetTextureFiltering( 0, TFO_BILINEAR );
	pSystem->SetTextureTransformFlag( 0, D3DTTFF_DISABLE );

	pSystem->SetTexture( 0, m_pGaugeTex->GetTexture() );
	pSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pSystem->DisableTextureUnitsFrom( 1 );

	pSystem->SetFillMode( FM_SOLID );
	pSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, m_iCurRenderCount );
/*
	pSystem->SetTextureFactor( 0xff000000 );
	pSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	pSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );

	pSystem->SetFillMode( FM_WIREFRAME );
	pSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, m_iCurRenderCount );

	pSystem->SetFillMode( FM_SOLID );
*/
}



