

#include "stdafx.h"

#include "ioDecal.h"
#include "ioDecalMaker.h"

#include "ioVertexDeclaration.h"
#include "ioVertexBufferBinder.h"
#include "ioVertexBufferInstance.h"
#include "ioIndexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"
#include "ioMath.h"
#include "ErrorReport.h"

#include "ioMaterial.h"
#include "ioSubEntity.h"
#include "ioEntity.h"
#include "ioMesh.h"
#include "ioOrientBox.h"

#include "ioCPU.h"

ioDecalMaker::ioDecalMaker()
{
	m_bRecalcMinMax = false;
}

ioDecalMaker::~ioDecalMaker()
{
}

void ioDecalMaker::Initialize( const ioOrientBox &rkBound, float fEpsilon )
{
	m_vCenter   = rkBound.GetCenter();
	m_vNormal   = rkBound.GetAxis( 1 );
	m_vTangent  = rkBound.GetAxis( 0 );
	m_vBinormal = rkBound.GetAxis( 2 );

	m_fWidth  = rkBound.GetExtents(0) * 2.0f;
	m_fHeight = rkBound.GetExtents(2) * 2.0f;

	m_fBackFaceEpsilon = fEpsilon;

	m_CullPlaneList.clear();
	m_CullPlaneList.reserve( 6 );

	float fDist;
	ioPlane kPlane;

	fDist = D3DXVec3Dot( &rkBound.GetCenter(), &rkBound.GetAxis(0) );

	// left
	kPlane.SetPlane( rkBound.GetAxis(0), rkBound.GetExtents(0) - fDist );
	m_CullPlaneList.push_back( kPlane );
	// right
	kPlane.SetPlane( -rkBound.GetAxis(0), rkBound.GetExtents(0) + fDist );
	m_CullPlaneList.push_back( kPlane );

	fDist = D3DXVec3Dot( &rkBound.GetCenter(), &rkBound.GetAxis(1) );
	// bottom
	kPlane.SetPlane( rkBound.GetAxis(1), rkBound.GetExtents(1) - fDist );
	m_CullPlaneList.push_back( kPlane );
	// top
	kPlane.SetPlane( -rkBound.GetAxis(1), rkBound.GetExtents(1) + fDist );
	m_CullPlaneList.push_back( kPlane );

	fDist = D3DXVec3Dot( &rkBound.GetCenter(), &rkBound.GetAxis(2) );
	// front
	kPlane.SetPlane( -rkBound.GetAxis(2), rkBound.GetExtents(2) + fDist );
	m_CullPlaneList.push_back( kPlane );
	// back
	kPlane.SetPlane( rkBound.GetAxis(2), rkBound.GetExtents(2) - fDist );
	m_CullPlaneList.push_back( kPlane );

	m_bRecalcMinMax = false;
}

void ioDecalMaker::AddDecal( ioEntity *pEntity )
{
	ioMesh *pMesh = pEntity->GetMesh();
	if( !pMesh || !pMesh->IsLoadDone() )
		return;

	const D3DXVECTOR3 *pVtx = (const D3DXVECTOR3 *)pMesh->GetSoftVertexBuffer( IOFVF_POSITION );
	const WORD *pIdx = pMesh->GetSoftIndexBuffer();
	if( !pVtx || !pIdx )	return;

	int iSubEntityCnt = pEntity->GetNumSubEntity();
	for( int i=0 ; i<iSubEntityCnt ; i++ )
	{
		ioSubEntity *pSub = pEntity->GetSubEntity( i );
		
		if( !pSub->GetMaterial()->IsTransparent() )
		{
			AddDecal( pSub->GetFaceCount(),
					  pIdx + pSub->GetIndexStart(),
					  pVtx );
		}		
	}
}

void ioDecalMaker::AddDecal( int iFaceCnt, const WORD *pIdx, const D3DXVECTOR3 *pVtx )
{
	D3DXVECTOR3 v1, v2, v3;
	D3DXVECTOR3 vCross, vEdge21, vEdge31;
	D3DXVECTOR3 vNewVertex[9];

	for( int i=0 ; i<iFaceCnt ; i++ )
	{
		v1 = pVtx[ pIdx[0] ];
		v2 = pVtx[ pIdx[1] ];
		v3 = pVtx[ pIdx[2] ];

		vEdge21 = v2 - v1;
		vEdge31 = v3 - v1;

		D3DXVec3Cross( &vCross, &vEdge21, &vEdge31 );

		if( D3DXVec3Dot( &m_vNormal, &vCross ) > m_fBackFaceEpsilon )
		{
			vNewVertex[0] = v1;
			vNewVertex[1] = v2;
			vNewVertex[2] = v3;

			int iCount = ClipPolygon( 3, vNewVertex );
			if( iCount != 0 )
			{
				AddPolygon( iCount, vNewVertex );
			}
		}

		pIdx += 3;
	}
}

int ioDecalMaker::ClipPolygon( int iVtxCnt, D3DXVECTOR3 *pNewVtx )
{
	D3DXVECTOR3 vTempVtx[9];

	int iCount = iVtxCnt;
	int iPlaneCnt = m_CullPlaneList.size();
	for( int i=0 ; i<iPlaneCnt && iCount!=0 ; i++ )
	{
		const ioPlane &rkPlane = m_CullPlaneList[i];

		if( i & 1 )	// 1, 3, 5...
		{
			iCount = ClipPolygonByPlane( rkPlane,
										 iCount,
										 vTempVtx,
										 pNewVtx );
		}
		else // 0, 2, 4, 6 ...
		{
			iCount = ClipPolygonByPlane( rkPlane,
										 iCount,
										 pNewVtx,
										 vTempVtx );
		}
	}

	return iCount;
}

int ioDecalMaker::ClipPolygonByPlane( const ioPlane &rkPlane,
									  int iVtxCnt,
									  const D3DXVECTOR3 *pVtx,
									  D3DXVECTOR3 *pNewVtx )
{
	int i;
	bool bNegative[10];

	int iNegativeCount = 0;
	for( i=0 ; i<iVtxCnt ; i++ )
	{
		if( rkPlane.GetSide( pVtx[i] ) == ioPlane::NEGATIVE_SIDE )
		{
			bNegative[i] = true;
			iNegativeCount++;
		}
		else
		{
			bNegative[i] = false;
		}
	}

	if( iNegativeCount == iVtxCnt )
		return 0;

	int iCount = 0;
	for( i=0 ; i<iVtxCnt ; i++ )
	{
		int iCurVtx;

		if( i!=0 )
			iCurVtx = i-1;
		else
			iCurVtx = iVtxCnt - 1;

		if( bNegative[i] )
		{
			if( !bNegative[iCurVtx] )
			{
				const D3DXVECTOR3 &v1 = pVtx[iCurVtx];
				const D3DXVECTOR3 &v2 = pVtx[i];

				float t = rkPlane.GetDistance( v1 ) / rkPlane.GetPlaneDotNormal( v1 - v2 );
				pNewVtx[iCount++] = ioMath::Lerp( t, v1, v2 );
			}
		}
		else
		{
			if( bNegative[iCurVtx] )
			{
				const D3DXVECTOR3 &v1 = pVtx[i];
				const D3DXVECTOR3 &v2 = pVtx[iCurVtx];

				float t = rkPlane.GetDistance( v1 ) / rkPlane.GetPlaneDotNormal( v1 - v2 );
				pNewVtx[iCount++] = ioMath::Lerp( t, v1, v2 );
			}

			pNewVtx[iCount] = pVtx[i];
			iCount++;
		}
	}

	return iCount;
}

void ioDecalMaker::AddPolygon( int iVtxCnt, const D3DXVECTOR3 *pVtx )
{
	int iCount = m_vVtxList.size();
	for( int i=2 ; i<iVtxCnt ; i++ )
	{
		// as a Triangle Pan
		m_vIdxList.push_back( iCount );
		m_vIdxList.push_back( iCount + i - 1 );
		m_vIdxList.push_back( iCount + i );
	}

	std::copy( pVtx, pVtx + iVtxCnt, std::back_inserter( m_vVtxList ) );
}

void ioDecalMaker::GenerateAutoUV()
{
	int iVtxCnt = m_vVtxList.size();

	m_vUVList.clear();
	m_vUVList.reserve( iVtxCnt );

	D3DXVECTOR3 vCenter;
	float fOneOverW, fOneOverH;
	if( m_bRecalcMinMax )
	{
		D3DXVECTOR3 vMin, vMax;
		ioMath::CalcMinMaxPoint( vMin, vMax, m_vVtxList );
		vCenter = ( vMax + vMin ) * FLOAT05;

		D3DXVECTOR3 vDiff = vMax - vCenter;

		float fWidth  = D3DXVec3Dot( &vDiff, &m_vTangent ) + 0.00001f;
		float fHeight = D3DXVec3Dot( &vDiff, &m_vBinormal ) + 0.00001f;

		fOneOverW = 1.0f / fWidth;
		fOneOverH = 1.0f / fHeight;
	}
	else
	{
		vCenter = m_vCenter;
		fOneOverW = 1.0f / m_fWidth;
		fOneOverH = 1.0f / m_fHeight;
	}

	D3DXVECTOR2 vUV;
	D3DXVECTOR3 vDiff;
	for( int i=0 ; i<iVtxCnt ; i++ )
	{
		vDiff = m_vVtxList[i] - vCenter;
		vUV.x = D3DXVec3Dot( &vDiff, &m_vTangent ) * fOneOverW + FLOAT05;
		vUV.y = D3DXVec3Dot( &vDiff, &m_vBinormal ) * fOneOverH + FLOAT05;
		m_vUVList.push_back( vUV );
	}
}

ioDecal* ioDecalMaker::BuildDecal( const ioHashString &rkName, int iDivLv )
{
	if( m_vVtxList.empty() )	// 영역에 걸리는 폴리곤이 없다.
		return NULL;

	GenerateAutoUV();

	ioDecal *pDecal = new ioDecal( rkName );
	if( pDecal->Build( m_vVtxList, m_vUVList, m_vIdxList ) )
		return pDecal;

	return NULL;
}
