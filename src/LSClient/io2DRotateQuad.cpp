

#include "stdafx.h"

#include "../io3DEngine/ioMeshData.h"
#include "../io3DEngine/ioVertexBufferInstance.h"

#include "io2DRotateQuad.h"

//////////////////////////////////////////////////////////////////////////
ioRotateQuad::ioRotateQuad( io2DRotateQuad *pQuad ) : m_pQuad( pQuad ) {}
ioRotateQuad::~ioRotateQuad() {}

void ioRotateQuad::AddInfo( ioQuadRenderArray *pQuadArray )
{
	pQuadArray->AddDirectQuadInfo( this );
}

void ioRotateQuad::RenderDirect( ioRenderSystem *pRenderSystem )
{
	if( m_pQuad )
		m_pQuad->Render( pRenderSystem );
}

//////////////////////////////////////////////////////////////////////////
io2DRotateQuad::io2DRotateQuad()
{
	m_pMeshData = NULL;
	m_vTexList.clear();

	m_iCurArray = 0;
	m_iRenderX = -1;
	m_iRenderY = -1;

	D3DXQuaternionIdentity( &m_qtOriginal );
}

io2DRotateQuad::~io2DRotateQuad()
{
	if( m_pMeshData )
	{
		m_pMeshData->DeleteAllBindingBuffers();

		delete m_pMeshData;
		m_pMeshData = NULL;
	}

	std::vector<ioTexture*>::iterator iter = m_vTexList.begin();
	while( iter != m_vTexList.end() )
	{
		ioTexture *pTex = *iter;
		if( pTex )
		{
			DESTROY_TEXTURE( pTex );
			iter = m_vTexList.erase( iter );
		}
	}
	m_vTexList.clear();
}

void io2DRotateQuad::InitResource( ioHashStringVec vTexNameList )
{
	int iSize = vTexNameList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioTexture *pTexture = g_TextureMgr.CreateTexture( vTexNameList[i], 0, 1, TQ_HIGH );
		if( pTexture )
			m_vTexList.push_back( pTexture );
	}

	if( m_vTexList.empty() )
		return;

	SAFEDELETE( m_pMeshData );

	m_pMeshData = new ioMeshData;
	m_pMeshData->InsertStreamSource( 0, IOFVF_POSITIONW | IOFVF_UV0 );

	ioVertexBufferInstance *pVtxInstance = NULL;
	pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_MANAGED,
														IOFVF_POSITIONW | IOFVF_UV0,
														4 );

	m_pMeshData->SetBinding( 0, pVtxInstance );
	m_pMeshData->m_iVertexCount = 4;
}

void io2DRotateQuad::Update( int iXPos, int iYPos, float fAngle, int iArray )
{
	if( m_vTexList.empty() )
		return;

	if( (int)m_vTexList.size() <= iArray )
		return;

	m_iCurArray = iArray;

	ioVertexBufferInstance *pInstance = m_pMeshData->GetVtxInstance(0);
	if( !pInstance )	return;
	if( !pInstance->Lock() )	return;

	int iTexWidth  = m_vTexList[m_iCurArray]->GetWidth();
	int iTexHeight = m_vTexList[m_iCurArray]->GetHeight();

	float fHalfWidth = static_cast<float>( iTexWidth / 2 );
	float fHalfHeight = static_cast<float>( iTexHeight / 2 );

	float fXPos = static_cast<float>(iXPos);
	float fYPos = static_cast<float>(iYPos);
	D3DXVECTOR3 vOffset = D3DXVECTOR3( fXPos, fYPos, 0.0f );
	
	D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
	vPos = D3DXVECTOR3( -fHalfWidth, -fHalfHeight, 0.01f );
	
	ioVtxRHWTex *pBuf = (ioVtxRHWTex*)pInstance->GetBuffer();

	float fAngle2 = ioMath::ArrangeHead( fAngle );
	D3DXQUATERNION qtRot, qtResult;
	D3DXQuaternionRotationAxis( &qtRot, &-ioMath::UNIT_Z, DEGtoRAD(fAngle2) );
	qtResult = m_qtOriginal * qtRot;

	vPos = D3DXVECTOR3( -fHalfWidth, fHalfHeight, 0.01f );
	vPos = qtResult * vPos;
	pBuf[0].vPos = vPos + vOffset;
	pBuf[0].fRHW = FLOAT1;
	pBuf[0].u = 0.0f;
	pBuf[0].v = FLOAT1;

	vPos = D3DXVECTOR3( -fHalfWidth, -fHalfHeight, 0.01f );
	vPos = qtResult * vPos;
	pBuf[1].vPos = vPos + vOffset;
	pBuf[1].fRHW = FLOAT1;
	pBuf[1].u = 0.0f;
	pBuf[1].v = 0.0f;

	vPos = D3DXVECTOR3( fHalfWidth, -fHalfHeight, 0.01f );
	vPos = qtResult * vPos;
	pBuf[2].vPos = vPos + vOffset;
	pBuf[2].fRHW = FLOAT1;
	pBuf[2].u = FLOAT1;
	pBuf[2].v = 0.0f;

	vPos = D3DXVECTOR3( fHalfWidth, fHalfHeight, 0.01f );
	vPos = qtResult * vPos;
	pBuf[3].vPos = vPos + vOffset;
	pBuf[3].fRHW = FLOAT1;
	pBuf[3].u = FLOAT1;
	pBuf[3].v = FLOAT1;

	pInstance->UnLock();

	m_iRenderX = iXPos;
	m_iRenderY = iYPos;
}

bool io2DRotateQuad::IsNeedRender() const
{
	if( !m_pMeshData )
		return false;

	if( m_vTexList.empty() )
		return false;

	if( (int)m_vTexList.size() <= m_iCurArray )
		return false;

	if( !m_vTexList[m_iCurArray] )
		return false;

	return true;
}

void io2DRotateQuad::Render( ioRenderSystem *pSystem )
{
	if( !IsNeedRender() )
		return;

	pSystem->SetModelMatrixIdentity();
	pSystem->SetVertexDeclaration( m_pMeshData->m_pVertexDeclaration );
	pSystem->SetVertexBufferBinder( m_pMeshData->m_pVertexBufferBinder );
	pSystem->SetIndexBufferInstance( NULL );
	pSystem->DisableShader();

	pSystem->SetDepthFunction( D3DCMP_LESSEQUAL, false, true );
	pSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	pSystem->SetCullingMode( CM_CCW );
	pSystem->SetAlphaTest( D3DCMP_GREATER, 0 );

	pSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	pSystem->SetTextureFiltering( 0, TFO_BILINEAR );
	pSystem->SetTextureTransformFlag( 0, D3DTTFF_DISABLE );

	pSystem->SetTexture( 0, m_vTexList[m_iCurArray]->GetTexture() );
	pSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pSystem->DisableTextureUnitsFrom( 1 );

	pSystem->SetFillMode( FM_SOLID );
	pSystem->RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
}

int io2DRotateQuad::GetWidth()
{
	if( m_vTexList.empty() )
		return 0;

	if( (int)m_vTexList.size() <= m_iCurArray )
		return 0;

	if( m_vTexList[m_iCurArray] )
		return m_vTexList[m_iCurArray]->GetWidth();

	return 0;
}

int io2DRotateQuad::GetHeight()
{
	if( m_vTexList.empty() )
		return 0;

	if( (int)m_vTexList.size() <= m_iCurArray )
		return 0;

	if( m_vTexList[m_iCurArray] )
		return m_vTexList[m_iCurArray]->GetHeight();

	return 0;
}