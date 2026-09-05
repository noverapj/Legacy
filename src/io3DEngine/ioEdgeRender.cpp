

#include "stdafx.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"

#include "ioIndexBufferInstance.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioTextureUnitState.h"
#include "ioPass.h"
#include "ioTechnique.h"
#include "ioMaterial.h"
#include "ioMaterialManager.h"
#include "ioRenderOperation.h"

#include "ioCamera.h"
#include "ioEntityGroup.h"
#include "ioShaderParameter.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ioFileTokenDefine.h"

#include "ioEdgeRender.h"
#include "ioCPU.h"
#include "HelpFunc.h"
#include "ErrorReport.h"

ioEdgeRender::ioEdgeRender()
{
	m_pOwner = NULL;
	m_pMeshData = NULL;
	m_pMaterial = NULL;
	m_fCreaseAngle = 90.0f;

	m_EdgeColor.r = m_EdgeColor.g = m_EdgeColor.b = 0.0f;
	m_EdgeColor.a = 1.0f;

	m_iEdgeThickness = 1;

	m_pBoundCreaseIndices = NULL;
	m_iBoundCreaseIndexCount = 0;

	m_pRenderEdgeIndices = NULL;
	m_iRenderEdgeIndexCount = 0;

	m_iCurIndexStart = 0;
	m_iPreIndexEnd = 0;

	m_LoadState = RLS_NOT_LOADED;
	m_bEnable = true;

	m_bUpdateBufferOK = false;
}

ioEdgeRender::~ioEdgeRender()
{
	if( m_pMeshData )
	{
		m_pMeshData->DeleteAllBuffers();
		SAFEDELETE(m_pMeshData);
	}

	SAFEDELETEARRAY( m_pBoundCreaseIndices );
	SAFEDELETEARRAY( m_pRenderEdgeIndices );

	SAFEDELETE( m_pMaterial );
}

void ioEdgeRender::SetOwner( ioEntityGroup *pOwner )
{
	m_pOwner = pOwner;
}

void ioEdgeRender::SetFileName( const ioHashString &rkName )
{
	m_FileName = rkName;
}

void ioEdgeRender::SetCreaseAngle( float fCreaseAngle )
{
	m_fCreaseAngle = fCreaseAngle;
}

void ioEdgeRender::SetEdgeColor( float fRed, float fGreen, float fBlue, float fAlpha )
{
	D3DCOLORVALUE kColor;
	kColor.r = fRed;
	kColor.g = fGreen;
	kColor.b = fBlue;
	kColor.a = fAlpha;

	SetEdgeColor( kColor );
}

void ioEdgeRender::SetEdgeColor( const D3DCOLORVALUE &rkColor )
{
	m_EdgeColor = rkColor;

	if( !m_pMaterial )	return;

	ioTechnique *pTech = m_pMaterial->GetBestTechnique();
	if( !pTech )	return;	

	ioShaderCustomArg kColorArg( "OutLineColor" );
	kColorArg.SetValue( m_EdgeColor );

	int iPassCnt = pTech->GetNumPasses();
	for( int i=0 ; i<iPassCnt ; i++ )
	{
		ioPass *pPass = pTech->GetPass( i );
		if( pPass )
		{
			pPass->AddCustomParameter( kColorArg );
		}
	}
}

void ioEdgeRender::SetEdgeThickness( int iThickness )
{
	m_iEdgeThickness = max( 1, iThickness );

	if( !m_pMaterial )	return;

	ioTechnique *pTech = m_pMaterial->GetBestTechnique();
	if( !pTech )	return;

	int iPrePassCnt = pTech->GetNumPasses();
	if( iPrePassCnt > m_iEdgeThickness )
	{
		int iRemovePassCnt = iPrePassCnt - m_iEdgeThickness;
		for( int i=0 ; i<iRemovePassCnt ; i++ )
		{
			pTech->RemovePass( pTech->GetNumPasses() - 1 );
		}
	}
	else if( iPrePassCnt < m_iEdgeThickness )
	{
		ioPass *pDefPass = pTech->GetPass( 0 );
		if( pDefPass )
		{
			int iAddPassCnt = m_iEdgeThickness - iPrePassCnt;
			for( int i=0 ; i<iAddPassCnt ; i++ )
			{
				ioPass *pNewPass = pTech->CreatePass();
				if( pNewPass )
				{
					*pNewPass = *pDefPass;
				}
			}
		}
	}

	int iNumPass = pTech->GetNumPasses();
	for( int i=0 ; i<iNumPass ; i++ )
	{
		ioShaderCustomArg kThickness( "OutLineThickness" );
		kThickness.SetValue( FLOAT05 + (float)i );

		ioPass *pPass = pTech->GetPass( i );
		if( pPass )
		{
			pPass->AddCustomParameter( kThickness );
		}
	}
}

bool ioEdgeRender::LoadEdgeFile( const char *szFullName )
{
	bool bLoaded = false;

	ioBinaryStream kStream;
	if( g_ResourceLoader.LoadStream( szFullName, &kStream ) )
	{
		bLoaded = LoadFileInternal( &kStream );

		if( bLoaded )
			SetLoadState( RLS_LOADED );
		else
			SetLoadState( RLS_FAILED );
	}
	else
	{
		SetLoadState( RLS_FAILED );
		LOG.PrintTimeAndLog( 0, "ioEdgeRender::LoadEdgeFile - %s Load Failed", szFullName );
	}

	return bLoaded;
}

bool ioEdgeRender::IsEdgeFile( const char *szToken )
{
	if( !strcmp( szToken, OUTLINE_EDGE_TOKEN ) )
		return true;

	return false;
}

bool ioEdgeRender::LoadFileInternal( ioBinaryStream *pStream )
{
	char szToken[8];
	ZeroMemory( szToken, 8 );
	pStream->Read( szToken, 4 );
	if( !IsEdgeFile( szToken ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEdgeRender::LoadFileInternal - %s is Not Edge File", m_FileName.c_str() );
		return false;
	}

	// Mesh Version
	int iVersion = 0;
	pStream->Read( &iVersion, sizeof(int) );

	int iVertexCnt;
	pStream->Read( &iVertexCnt, sizeof(int) );

	m_vVtxList.clear();
	m_vVtxList.resize( iVertexCnt );
	pStream->Read( &m_vVtxList[0], sizeof(D3DXVECTOR3)*iVertexCnt );

	m_vNrmList.clear();
	m_vNrmList.resize( iVertexCnt );
	pStream->Read( &m_vNrmList[0], sizeof(D3DXVECTOR3)*iVertexCnt );

	int iFaceCnt;
	pStream->Read( &iFaceCnt, sizeof(int) );

	m_vFaceNormals.clear();
	m_vFaceNormals.resize( iFaceCnt );
	pStream->Read( &m_vFaceNormals[0], sizeof(D3DXVECTOR3)*iFaceCnt );

	int iEdgeCnt;
	pStream->Read( &iEdgeCnt, sizeof(int) );

	Edge kEdge;
	WORD wIndexA, wIndexB;

	m_vEdgeList.clear();
	m_vEdgeList.reserve( iEdgeCnt );
	for( int i=0 ; i<iEdgeCnt ; i++ )
	{
		pStream->Read( &wIndexA, sizeof(WORD) );
		pStream->Read( &wIndexB, sizeof(WORD) );
		kEdge.SetVertexIndices( wIndexA, wIndexB );

		pStream->Read( &wIndexA, sizeof(WORD) );
		pStream->Read( &wIndexB, sizeof(WORD) );
		kEdge.SetFaceIndices( wIndexA, wIndexB );

		m_vEdgeList.push_back( kEdge );
	}

	OptimizeEdgeList();
	return true;
}

void ioEdgeRender::OptimizeEdgeList()
{
	EdgeList vTempList;
	vTempList.reserve( m_vEdgeList.size() + m_vBoundCreaseList.size() );

	if( !m_vEdgeList.empty() )
	{
		std::copy( m_vEdgeList.begin(), m_vEdgeList.end(), std::back_inserter( vTempList ) );
	}

	if( !m_vBoundCreaseList.empty() )
	{
		std::copy( m_vBoundCreaseList.begin(), m_vBoundCreaseList.end(), std::back_inserter( vTempList ) );
	}

	m_vEdgeList.erase( m_vEdgeList.begin(), m_vEdgeList.end() );
	m_vBoundCreaseList.erase( m_vBoundCreaseList.begin(), m_vBoundCreaseList.end() );

	float fDotProduct;
	float fCreaseCosine = cosf( DEGtoRAD( m_fCreaseAngle ) );
	WORD wFaceSlot1, wFaceSlot2;

	int iNumEdges = vTempList.size();
	for( int i=0 ; i<iNumEdges ; i++ )
	{
		Edge &rkEdge = vTempList[i];

		wFaceSlot1 = rkEdge.GetFaceIndex(0);
		wFaceSlot2 = rkEdge.GetFaceIndex(1);

		if( (wFaceSlot1 == wFaceSlot2) ||
			(wFaceSlot1 == EMPTY_FACE_SLOT) ||
			(wFaceSlot2 == EMPTY_FACE_SLOT) )
		{
			m_vBoundCreaseList.push_back( rkEdge );
		}
		else
		{
			fDotProduct = D3DXVec3Dot( &m_vFaceNormals[wFaceSlot1],
									   &m_vFaceNormals[wFaceSlot2] );

			if( fDotProduct <= fCreaseCosine )
			{
				m_vBoundCreaseList.push_back( rkEdge );
			}
			else
			{
				m_vEdgeList.push_back( rkEdge );
			}
		}
	}

	SAFEDELETEARRAY( m_pBoundCreaseIndices );

	int iBoundCreaseEdgeCount = m_vBoundCreaseList.size();
	m_pBoundCreaseIndices    = new WORD[ iBoundCreaseEdgeCount * 2 ];
	m_iBoundCreaseIndexCount = 0;

	for( i=0 ; i<iBoundCreaseEdgeCount ; i++ )
	{
		m_pBoundCreaseIndices[ m_iBoundCreaseIndexCount++ ] = m_vBoundCreaseList[i].GetVertexIndex(0);
		m_pBoundCreaseIndices[ m_iBoundCreaseIndexCount++ ] = m_vBoundCreaseList[i].GetVertexIndex(1);
	}

	int iOptimizedEdgeCount = m_vEdgeList.size();

	m_vCachedVtxList.clear();
	m_vCachedVtxList.reserve( iOptimizedEdgeCount );

	m_vCachedNormalList.clear();
	m_vCachedNormalList.reserve( iOptimizedEdgeCount * 2 );

	for( i=0 ; i<iOptimizedEdgeCount ; i++ )
	{
		const Edge &rkEdge = m_vEdgeList[i];

		m_vCachedVtxList.push_back( m_vVtxList[ rkEdge.GetVertexIndex(0) ] );

		m_vCachedNormalList.push_back( m_vFaceNormals[ rkEdge.GetFaceIndex(0) ] );
		m_vCachedNormalList.push_back( m_vFaceNormals[ rkEdge.GetFaceIndex(1) ] );
	}

	SAFEDELETEARRAY( m_pRenderEdgeIndices );
	m_pRenderEdgeIndices = new WORD[ iOptimizedEdgeCount * 2 ];
	m_iRenderEdgeIndexCount = 0;
}

void ioEdgeRender::NotifyCurrentCamera( ioCamera *pCamera )
{
	m_bUpdateBufferOK = true;

	if( !BuildRenderingData() )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	D3DXMATRIX matInvWorld;
	GetWorldTransform( &matInvWorld );
	D3DXMatrixInverse( &matInvWorld, NULL, &matInvWorld );

	D3DXVECTOR3 vModelSpaceEyePos;
	D3DXVec3TransformCoord( &vModelSpaceEyePos, &pCamera->GetPosition(), &matInvWorld );

	DetectEdges( vModelSpaceEyePos );

	int iTotalIndices = m_iBoundCreaseIndexCount + m_iRenderEdgeIndexCount;

	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;
	ioIndexBufferInstance *pIdxBuf = m_pMeshData->m_pIndexBufferInstance;
	if( m_iPreIndexEnd + iTotalIndices > pIdxBuf->GetIndexCount() )
	{
		dwLockFlags = D3DLOCK_DISCARD;
		m_iPreIndexEnd = 0;
	}

	int iStartBytes = m_iPreIndexEnd * sizeof(WORD);
	int iLockBytes  = iTotalIndices * sizeof(WORD);
	if( !pIdxBuf->Lock( dwLockFlags, iStartBytes, iLockBytes ) )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	__try
	{
		WORD *pBuf = (WORD*)pIdxBuf->GetBuffer();

		if( m_iBoundCreaseIndexCount > 0 )
		{
			ioCPU::FastMemcpy( pBuf, m_pBoundCreaseIndices, m_iBoundCreaseIndexCount * sizeof(WORD) );
			pBuf += m_iBoundCreaseIndexCount;
		}

		if( m_iRenderEdgeIndexCount > 0 )
		{
			ioCPU::FastMemcpy( pBuf, m_pRenderEdgeIndices, m_iRenderEdgeIndexCount * sizeof(WORD) );
		}
	}
	__except( ErrorReport::DynamicBufferFilter( GetExceptionCode() ) )
	{
		m_bUpdateBufferOK = false;
	}

	pIdxBuf->UnLock();
	m_pMeshData->m_iIndexCount = iTotalIndices;
	m_iCurIndexStart = m_iPreIndexEnd;
	m_iPreIndexEnd  += iTotalIndices;
}

bool ioEdgeRender::BuildRenderingData()
{
	if( m_pMeshData )
		return true;

	if( m_LoadState != RLS_LOADED )
		return false;

	// Build MeshData...
	int iVtxCount   = m_vVtxList.size();
	DWORD dwFVFMask = IOFVF_POSITION | IOFVF_NORMAL;

	ioVertexBufferInstance *pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_MANAGED, dwFVFMask, iVtxCount );
	if( !pVtxInstance )
		return false;

	if( !pVtxInstance->Lock() )
	{
		g_VtxBufMgr.DestroyInstance( pVtxInstance );
		return false;
	}

	ioVtxNrm *pBuf = (ioVtxNrm*)pVtxInstance->GetBuffer();
	for( int i=0 ; i<iVtxCount ; i++ )
	{
		pBuf->vPos = m_vVtxList[i];
		pBuf->vNrm = m_vNrmList[i];

		pBuf++;
	}

	pVtxInstance->UnLock();

	m_pMeshData = new ioMeshData;
	m_pMeshData->InsertStreamSource( 0, dwFVFMask );
	m_pMeshData->SetBinding( 0, pVtxInstance );
	m_pMeshData->m_iVertexCount = iVtxCount;

	int iMaxIndexCount = ( m_vEdgeList.size() + m_vBoundCreaseList.size() ) * 2;
	m_pMeshData->m_pIndexBufferInstance = g_IdxBufMgr.CreateExclusiveInstance( BT_DYNAMIC, iMaxIndexCount );
	m_pMeshData->m_iIndexCount = 0;

	BuildMaterial();

	return true;
}

void ioEdgeRender::BuildMaterial()
{
	SAFEDELETE( m_pMaterial );

	m_pMaterial = g_MaterialMgr.CreateMaterial();

	ioPass *pPass = m_pMaterial->CreateTechnique()->CreatePass();
	pPass->SetLightingEnable( false );
	pPass->SetDepthFunction( D3DCMP_LESSEQUAL );
	pPass->SetDepthCheckEnable( true );
	pPass->SetDepthWriteEnable( true );

	pPass->SetShaderGroup( "OutLineEdge" );

	ioTextureUnitState *pState = pPass->CreateTextureUnitState();
	pState->SetColorStageBlendOp( D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pState->SetAlphaStageBlendOp( D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_DIFFUSE );

	SetEdgeColor( m_EdgeColor );
	SetEdgeThickness( m_iEdgeThickness );
}

void ioEdgeRender::DetectEdges( const D3DXVECTOR3 &vEyePosition )
{
	D3DXVECTOR3 vViewVector;
	float fDotProduct1, fDotProduct2;

	m_iRenderEdgeIndexCount = 0;

	int iNumEdges = m_vEdgeList.size();
	for( int i=0, iNrm=0 ; i<iNumEdges ; i++ )
	{
		// 부호 판별만이 필요하기 때문에 vViewVector를 Normalize시키지 않음
		vViewVector = vEyePosition - m_vCachedVtxList[i];

		fDotProduct1 = D3DXVec3Dot( &vViewVector, &m_vCachedNormalList[ iNrm++ ] );
		fDotProduct2 = D3DXVec3Dot( &vViewVector, &m_vCachedNormalList[ iNrm++ ] );

		if( fDotProduct1 * fDotProduct2 < 0.000001f )
		{
			m_pRenderEdgeIndices[ m_iRenderEdgeIndexCount++ ] = m_vEdgeList[i].GetVertexIndex(0);
			m_pRenderEdgeIndices[ m_iRenderEdgeIndexCount++ ] = m_vEdgeList[i].GetVertexIndex(1);
		}
	}
}

ioMaterial* ioEdgeRender::GetMaterial() const
{
	return m_pMaterial;
}

float ioEdgeRender::GetSquaredViewDepth( const ioCamera *pCamera )
{
	if( m_pOwner )
		return m_pOwner->GetSquaredViewDepth( pCamera );

	return 0.0f;
}

void ioEdgeRender::GetRenderOperation( ioRenderOperation &rkRenderOp )
{
	rkRenderOp.m_pMeshData = m_pMeshData;
	rkRenderOp.m_PrimitiveType = D3DPT_LINELIST;

	rkRenderOp.m_iMinIndex    = 0;
	rkRenderOp.m_iVertexCount = m_pMeshData->m_iVertexCount;
	rkRenderOp.m_iIndexStart  = m_iCurIndexStart;
	rkRenderOp.m_iFaceCount   = m_pMeshData->m_iIndexCount/2;
	rkRenderOp.m_bUseIndex    = true;
}

void ioEdgeRender::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	if( m_pOwner )
	{
		*pMatWorld = *m_pOwner->GetTransform();
	}
}

