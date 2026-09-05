

#include "stdafx.h"
#include "ioProgressiveMesh.h"

#define NEVER_COLLAPSE_COST 99999.9f

struct Vector3Less
{
	bool operator()(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2) const
	{
		if (v1.x < v2.x)	return true;
		if (v1.x == v2.x && v1.y < v2.y) return true;
		if (v1.x == v2.x && v1.y == v2.y && v1.z < v2.z) return true;

		return false;
	}
};

ioProgressiveMesh::ioProgressiveMesh()
{
	m_pVertex = NULL;
	m_iVertexCount = 0;

	m_pIndex = NULL;
	m_iIndexCount = 0;

	m_iNumCommonVertices = 0;
	m_iCurNumIndices = 0;
}

ioProgressiveMesh::~ioProgressiveMesh()
{
}

void ioProgressiveMesh::SetMeshInfo( const D3DXVECTOR3 *pVertex,
									 int iVertexCount,
									 const WORD *pIndex,
									 int iIndexCount,
									 int iIndexOffset )
{
	m_pVertex = pVertex;
	m_iVertexCount = iVertexCount;

	m_pIndex = pIndex;
	m_iIndexCount = iIndexCount;

	m_iIndexOffset = iIndexOffset;

	m_iNumCommonVertices = 0;
	m_iCurNumIndices = 0;

	BuildWorkingData();

	m_WorstCosts.resize( m_iVertexCount );
}

void ioProgressiveMesh::BuildWorkingData()
{
	m_WorkingData.m_FaceVertList.resize( m_iVertexCount );
	m_WorkingData.m_VertList.resize( m_iVertexCount );

	typedef std::map< D3DXVECTOR3, int, Vector3Less > CommonVertexMap;
	CommonVertexMap kCommonVertexMap;
	CommonVertexMap::iterator iCommonVertex;

	int i = 0;
	int iNumCommon = 0;

	for( i=0 ; i<m_iVertexCount ; i++ )
	{
		iCommonVertex = kCommonVertexMap.find( m_pVertex[i] );

		if( iCommonVertex == kCommonVertexMap.end() )
		{
			PMVertex *pCommonVert = &( m_WorkingData.m_VertList[iNumCommon] );
			pCommonVert->SetDetails( m_pVertex[i], iNumCommon );
			pCommonVert->m_bRemoved = false;
			pCommonVert->m_bToBeRemoved = false;
			pCommonVert->m_bSeam = false;

			kCommonVertexMap.insert( CommonVertexMap::value_type( m_pVertex[i], iNumCommon ) );
			++iNumCommon;

			m_WorkingData.m_FaceVertList[i].m_pCommonVertex = pCommonVert;
			m_WorkingData.m_FaceVertList[i].m_iRealIndex = i + m_iIndexOffset;
		}
		else
		{
			PMVertex *pExistVert = &( m_WorkingData.m_VertList[ iCommonVertex->second ] );
			m_WorkingData.m_FaceVertList[i].m_pCommonVertex = pExistVert;
			m_WorkingData.m_FaceVertList[i].m_iRealIndex = i + m_iIndexOffset;

			m_WorkingData.m_FaceVertList[i].m_pCommonVertex->m_bSeam = true;
		}
	}

	m_iNumCommonVertices = iNumCommon;

	int iNumTri = m_iIndexCount / 3;
	m_WorkingData.m_TriList.resize( iNumTri );

	WORD wIndexOffset = (WORD)m_iIndexOffset;
	WORD wCurVertIndex = 0;

	const WORD *pIndex = m_pIndex;
	for( i=0 ; i<iNumTri ; i++ )
	{
		PMFaceVertex *v0, *v1, *v2;

		wCurVertIndex = *pIndex - wIndexOffset;
		pIndex++;

		v0 = &( m_WorkingData.m_FaceVertList[ wCurVertIndex ] );

		wCurVertIndex = *pIndex - wIndexOffset;
		pIndex++;

		v1 = &( m_WorkingData.m_FaceVertList[ wCurVertIndex ] );
		
		wCurVertIndex = *pIndex - wIndexOffset;
		pIndex++;

		v2 = &( m_WorkingData.m_FaceVertList[ wCurVertIndex ] );

		m_WorkingData.m_TriList[i].SetDetails( i, v0, v1, v2 );
		m_WorkingData.m_TriList[i].m_bRemoved = false;
	}
}

void ioProgressiveMesh::Build( VertexReduceMethod eMethod, float fReduceValue, LODUsage &rkResult )
{
	ComputeAllCosts();

	int iNumVerts = m_iNumCommonVertices;
	m_iCurNumIndices = m_iIndexCount;

	int iNumCollapses = 0;
	if( eMethod == VRM_RATE )
		iNumCollapses = iNumVerts * fReduceValue;
	else
		iNumCollapses = (int)fReduceValue;

	if( iNumVerts - iNumCollapses < 3 )
		iNumCollapses = iNumVerts - 3;

	iNumVerts = iNumVerts - iNumCollapses;

	while( iNumCollapses-- )
	{
		int iNextIndex = GetNextCollapser();

		PMVertex *pCollapser = &( m_WorkingData.m_VertList.at( iNextIndex ) );
		if( pCollapser->m_pCollapseTo )
		{
			assert( pCollapser->m_pCollapseTo->m_bRemoved == false );
			Collapse( pCollapser );
		}
		else
		{
			break;
		}
	}

	assert( m_iCurNumIndices > 0 );

	rkResult.pIndex = new WORD[m_iCurNumIndices];
	rkResult.iIndexCount = m_iCurNumIndices;

	BakeNewLOD( rkResult.pIndex );
}

void ioProgressiveMesh::InitialiseEdgeCollapseCosts()
{
	CommonVertexList::iterator iter, iEnd;
	iEnd = m_WorkingData.m_VertList.end();
	for( iter=m_WorkingData.m_VertList.begin() ; iter!=iEnd ; ++iter )
	{
		iter->m_pCollapseTo = NULL;
		iter->m_fCollapseCost = NEVER_COLLAPSE_COST;
	}
}

float ioProgressiveMesh::ComputeEdgeCollapseCost( PMVertex *pSrc, PMVertex *pDest )
{
	PMVertex::FaceList vSideList;
	PMVertex::FaceList::iterator iSrcFace, iSrcFaceEnd;
	iSrcFaceEnd = pSrc->m_Face.end();
	for( iSrcFace = pSrc->m_Face.begin() ; iSrcFace!=iSrcFaceEnd ; ++iSrcFace )
	{
		if( (*iSrcFace)->HasCommonVertex( pDest ) )
		{
			vSideList.insert( *iSrcFace );
		}
	}

	float fCost = 1.0f;

	if( pSrc->IsBorder() )
	{
		if( vSideList.size() > 1 )
		{
			fCost = 1.0f;
		}
		else
		{
			float fKinkiness, fMaxKinkiness;
			fMaxKinkiness = 0.0f;

			D3DXVECTOR3 vCollapseEdge = pSrc->m_vPosition - pDest->m_vPosition;
			D3DXVec3Normalize( &vCollapseEdge, &vCollapseEdge );

			D3DXVECTOR3 vOtherBorderEdge;

			PMVertex::NeighborList::iterator iter, iEnd;
			iEnd = pSrc->m_Neighbors.end();
			for( iter=pSrc->m_Neighbors.begin() ; iter!=iEnd ; ++iter )
			{
				if( *iter != pDest && (*iter)->IsManifoldEdgeWith(pSrc) )
				{
					vOtherBorderEdge = pSrc->m_vPosition - (*iter)->m_vPosition;
					D3DXVec3Normalize( &vOtherBorderEdge, &vOtherBorderEdge );

					fKinkiness = ( D3DXVec3Dot( &vOtherBorderEdge, &vCollapseEdge ) + 1.002f ) * FLOAT05;
					fMaxKinkiness = max( fKinkiness, fMaxKinkiness );
				}
			}

			fCost = fMaxKinkiness;
		}
	}
	else
	{
		float fCurVature = 0.001f;
		for( iSrcFace = pSrc->m_Face.begin() ; iSrcFace != iSrcFaceEnd ; ++iSrcFace )
		{
			float fMinCurv = 1.0f;

			PMVertex::FaceList::iterator iSideFace, iSideFaceEnd;
			iSideFaceEnd = vSideList.end();
			for( iSideFace=vSideList.begin() ; iSideFace!=iSideFaceEnd ; ++iSideFace )
			{
				float fDotProd = D3DXVec3Dot( &((*iSrcFace)->m_vNormal), &((*iSideFace)->m_vNormal) );
				fMinCurv = min( fMinCurv, ( 1.002f - fDotProd ) * FLOAT05 );
			}

			fCurVature = max( fCurVature, fMinCurv );
		}

		fCost = fCurVature;
	}

	if( pSrc->m_bSeam && !pDest->m_bSeam )
	{
		fCost = 1.0f;
	}

	if( pSrc->m_Face.size() == 1 && pDest->m_Face.size() == 1 )
	{
		fCost = NEVER_COLLAPSE_COST;
	}

	for( iSrcFace=pSrc->m_Face.begin() ; iSrcFace!=iSrcFaceEnd ; ++iSrcFace )
	{
		PMTriangle *pCurFace = *iSrcFace;

		if( pCurFace->HasCommonVertex( pDest ) )
			continue;

		PMVertex *v0, *v1, *v2;
		if( pCurFace->m_pVertex[0]->m_pCommonVertex == pSrc )
			v0 = pDest;
		else
			v0 = pCurFace->m_pVertex[0]->m_pCommonVertex;

		if( pCurFace->m_pVertex[1]->m_pCommonVertex == pSrc )
			v1 = pDest;
		else
			v1 = pCurFace->m_pVertex[1]->m_pCommonVertex;

		if( pCurFace->m_pVertex[2]->m_pCommonVertex == pSrc )
			v2 = pDest;
		else
			v2 = pCurFace->m_pVertex[2]->m_pCommonVertex;

		D3DXVECTOR3 vEdge1 = v1->m_vPosition - v0->m_vPosition;
		D3DXVECTOR3 vEdge2 = v2->m_vPosition - v1->m_vPosition;

		D3DXVECTOR3 vNewNormal;
		D3DXVec3Cross( &vNewNormal, &vEdge1, &vEdge2 );
		D3DXVec3Normalize( &vNewNormal, &vNewNormal );

		if( D3DXVec3Dot( &vNewNormal, &pCurFace->m_vNormal ) < 0.0f )
		{
			fCost = NEVER_COLLAPSE_COST;
			break;
		}
	}

	assert( fCost >= 0.0f );
	return fCost;
}

float ioProgressiveMesh::ComputeEdgeCostAtVertexForBuffer( int iVert )
{
	CommonVertexList::iterator v = m_WorkingData.m_VertList.begin() + iVert;

	if( v->m_Neighbors.empty() )
	{
		v->NotifyRemoved();
		return v->m_fCollapseCost;
	}

	v->m_fCollapseCost = NEVER_COLLAPSE_COST;
	v->m_pCollapseTo = NULL;

	PMVertex::NeighborList::iterator iter, iEnd;
	iEnd = v->m_Neighbors.end();
	for( iter=v->m_Neighbors.begin() ; iter!=iEnd ; ++iter )
	{
		float fCost = ComputeEdgeCollapseCost( &(*v), *iter );

		if( (!v->m_pCollapseTo ) || fCost < v->m_fCollapseCost )
		{
			v->m_fCollapseCost = fCost;
			v->m_pCollapseTo = *iter;
		}
	}

	return v->m_fCollapseCost;
}

void ioProgressiveMesh::ComputeEdgeCostAtVertex( int iVert )
{
	m_WorstCosts[ iVert ] = max( -0.01f, ComputeEdgeCostAtVertexForBuffer( iVert ) );
}

void ioProgressiveMesh::ComputeAllCosts()
{
	InitialiseEdgeCollapseCosts();

	for( int i=0 ; i<m_iVertexCount ; i++ )
	{
		ComputeEdgeCostAtVertex( i );
	}
}

void ioProgressiveMesh::BakeNewLOD( WORD *pIndexData )
{
	int iNewIndex = 0;

	TriangleList::iterator iter, iEnd;
	iEnd = m_WorkingData.m_TriList.end();
	for( iter=m_WorkingData.m_TriList.begin() ; iter!=iEnd ; ++iter )
	{
		if( !iter->m_bRemoved )
		{
			pIndexData[ iNewIndex++ ] = iter->m_pVertex[0]->m_iRealIndex;
			pIndexData[ iNewIndex++ ] = iter->m_pVertex[1]->m_iRealIndex;
			pIndexData[ iNewIndex++ ] = iter->m_pVertex[2]->m_iRealIndex;
		}
	}
}

void ioProgressiveMesh::Collapse( PMVertex *pSrc )
{
	if( pSrc->m_fCollapseCost == NEVER_COLLAPSE_COST )
		return;

	// Save...
	PMVertex *pDest = pSrc->m_pCollapseTo;

	// Remove this vertex from the running for the next Check
	pSrc->m_pCollapseTo = NULL;
	pSrc->m_fCollapseCost = NEVER_COLLAPSE_COST;
	m_WorstCosts[pSrc->m_iIndex] = NEVER_COLLAPSE_COST;

	if( !pDest )	return;

	std::set< PMVertex* > kReComputeSet;
	kReComputeSet.insert( pDest );

	PMVertex::NeighborList::iterator iter, iEnd;
	iEnd = pSrc->m_Neighbors.end();
	for( iter=pSrc->m_Neighbors.begin() ; iter!=iEnd ; ++iter )
	{
		kReComputeSet.insert( *iter );
	}

	iEnd = pDest->m_Neighbors.end();
	for( iter=pDest->m_Neighbors.begin() ; iter!=iEnd ; ++iter )
	{
		kReComputeSet.insert( *iter );
	}

	PMVertex::FaceList vFaceRemovalList, vFaceReplacementList;

	PMVertex::FaceList::iterator iFace, iFaceEnd;
	iFaceEnd = pSrc->m_Face.end();
	for( iFace = pSrc->m_Face.begin() ; iFace!=iFaceEnd ; ++iFace )
	{
		PMTriangle *pCurFace = *iFace;

		if( pCurFace->HasCommonVertex( pDest ) )
		{
			vFaceRemovalList.insert( pCurFace );
			m_iCurNumIndices -= 3;
		}
		else
		{
			vFaceReplacementList.insert( pCurFace );
		}
	}

	pSrc->m_bToBeRemoved = true;
	for( iFace=vFaceReplacementList.begin() ; iFace!=vFaceReplacementList.end() ; ++iFace )
	{
		PMFaceVertex *pSrcFaceVert  = (*iFace)->GetFaceVertexFromCommon( pSrc );
		PMFaceVertex *pDestFaceVert = NULL;

		PMVertex::FaceList::iterator iRemoved;
		for( iRemoved=vFaceRemovalList.begin() ; iRemoved!=vFaceRemovalList.end() ; ++iRemoved )
		{
			pDestFaceVert = (*iRemoved)->GetFaceVertexFromCommon( pDest );
		}

		assert( pDestFaceVert );
		(*iFace)->ReplaceVertex( pSrcFaceVert, pDestFaceVert );
	}

	for( iFace = vFaceRemovalList.begin() ; iFace!=vFaceRemovalList.end() ; ++iFace )
	{
		(*iFace)->NotifyRemoved();
	}

	pSrc->NotifyRemoved();

	std::set< PMVertex* >::iterator iReComp, iReCompEnd;
	iReCompEnd = kReComputeSet.end();
	for( iReComp = kReComputeSet.begin() ; iReComp!=iReCompEnd ; ++iReComp )
	{
		ComputeEdgeCostAtVertex( (*iReComp)->m_iIndex );
	}
}

int ioProgressiveMesh::GetNextCollapser()
{
	float fBestValue = NEVER_COLLAPSE_COST;

	int iBestIndex = 0;
	for( int i=0 ; i<m_iNumCommonVertices ; i++ )
	{
		if( m_WorstCosts[i] < fBestValue )
		{
			fBestValue = m_WorstCosts[i];
			iBestIndex = i;
		}
	}

	return iBestIndex;
}

//---------------------------------------------------------------------------------------------------

ioProgressiveMesh::PMTriangle::PMTriangle()
{
	m_bRemoved = false;
}

ioProgressiveMesh::PMTriangle::~PMTriangle()
{
}

void ioProgressiveMesh::PMTriangle::SetDetails( int iIndex,
											    PMFaceVertex *v0,
												PMFaceVertex *v1,
												PMFaceVertex *v2 )
{
	assert( v0 != v1 && v1 != v2 && v2 != v0 );

	m_iIndex = iIndex;
	m_pVertex[0] = v0;
	m_pVertex[1] = v1;
	m_pVertex[2] = v2;

	ComputeNormal();

	for( int i=0 ; i<3 ; i++ )
	{
		m_pVertex[i]->m_pCommonVertex->m_Face.insert( this );

		for( int j=0 ; j<3 ; j++ )
		{
			if( i!= j )
			{
				m_pVertex[i]->m_pCommonVertex->m_Neighbors.insert( m_pVertex[j]->m_pCommonVertex );
			}
		}
	}
}

void ioProgressiveMesh::PMTriangle::NotifyRemoved()
{
	int i=0;

	for( i=0 ; i<3 ; i++ )
	{
		if( m_pVertex[i] )
		{
			m_pVertex[i]->m_pCommonVertex->m_Face.erase( this );
		}
	}

	for( i=0 ; i<3 ; i++ )
	{
		int i2 = ( i+1 ) % 3;
		if( !m_pVertex[i] || !m_pVertex[i2] )
			continue;

		m_pVertex[i]->m_pCommonVertex->RemoveIfNonNeighbor( m_pVertex[i2]->m_pCommonVertex );
		m_pVertex[i2]->m_pCommonVertex->RemoveIfNonNeighbor( m_pVertex[i]->m_pCommonVertex );
	}

	m_bRemoved = true;
}

bool ioProgressiveMesh::PMTriangle::HasCommonVertex( PMVertex *v ) const
{
	if( v == m_pVertex[0]->m_pCommonVertex ||
		v == m_pVertex[1]->m_pCommonVertex ||
		v == m_pVertex[2]->m_pCommonVertex )
		return true;

	return false;
}

bool ioProgressiveMesh::PMTriangle::HasFaceVertex( PMFaceVertex *v ) const
{
	if( v == m_pVertex[0] ||
		v == m_pVertex[1] ||
		v == m_pVertex[2] )
		return true;

	return false;
}

ioProgressiveMesh::PMFaceVertex* ioProgressiveMesh::PMTriangle::GetFaceVertexFromCommon( PMVertex *pCommonVert )
{
	if( m_pVertex[0]->m_pCommonVertex == pCommonVert )	return m_pVertex[0];
	if( m_pVertex[1]->m_pCommonVertex == pCommonVert )	return m_pVertex[1];
	if( m_pVertex[2]->m_pCommonVertex == pCommonVert )	return m_pVertex[2];

	return NULL;
}

void ioProgressiveMesh::PMTriangle::ComputeNormal()
{
	D3DXVECTOR3 v0 = m_pVertex[0]->m_pCommonVertex->m_vPosition;
	D3DXVECTOR3 v1 = m_pVertex[1]->m_pCommonVertex->m_vPosition;
	D3DXVECTOR3 v2 = m_pVertex[2]->m_pCommonVertex->m_vPosition;

	D3DXVECTOR3 vEdge1 = v1 - v0;
	D3DXVECTOR3 vEdge2 = v2 - v1;

	D3DXVec3Cross( &m_vNormal, &vEdge1, &vEdge2 );
	D3DXVec3Normalize( &m_vNormal, &m_vNormal );
}

void ioProgressiveMesh::PMTriangle::ReplaceVertex( PMFaceVertex *pOld, PMFaceVertex *pNew )
{
	assert( pOld && pNew );
	assert( pOld == m_pVertex[0] || pOld == m_pVertex[1] || pOld == m_pVertex[2] );
	assert( pNew != m_pVertex[0] && pNew != m_pVertex[1] && pNew != m_pVertex[2] );

	if( pOld == m_pVertex[0] )
		m_pVertex[0] = pNew;
	else if( pOld == m_pVertex[1] )
		m_pVertex[1] = pNew;
	else
		m_pVertex[2] = pNew;

	pOld->m_pCommonVertex->m_Face.erase( this );
	pNew->m_pCommonVertex->m_Face.insert( this );

	int i;

	for( i=0 ; i<3 ; i++ )
	{
		pOld->m_pCommonVertex->RemoveIfNonNeighbor( m_pVertex[i]->m_pCommonVertex );
		m_pVertex[i]->m_pCommonVertex->RemoveIfNonNeighbor( pOld->m_pCommonVertex );
	}

	for( i=0 ; i<3 ; i++ )
	{
		assert( m_pVertex[i]->m_pCommonVertex->m_Face.find(this) != m_pVertex[i]->m_pCommonVertex->m_Face.end() );

		for( int j=0 ; j<3 ; j++ )
		{
			if( i!= j )
			{
				m_pVertex[i]->m_pCommonVertex->m_Neighbors.insert( m_pVertex[j]->m_pCommonVertex );
			}
		}
	}
}

//----------------------------------------------------------------------------------------

ioProgressiveMesh::PMVertex::PMVertex()
{
	m_bRemoved = false;
}

ioProgressiveMesh::PMVertex::~PMVertex()
{
}

void ioProgressiveMesh::PMVertex::SetDetails( const D3DXVECTOR3 &vPos, int iIndex )
{
	m_vPosition = vPos;
	m_iIndex = iIndex;
}

void ioProgressiveMesh::PMVertex::NotifyRemoved()
{
	NeighborList::iterator iter;
	for( iter=m_Neighbors.begin() ; iter!=m_Neighbors.end() ; ++iter )
	{
		(*iter)->m_Neighbors.erase( this );
	}

	m_bRemoved = true;
	m_pCollapseTo = NULL;
	m_fCollapseCost = NEVER_COLLAPSE_COST;
}

bool ioProgressiveMesh::PMVertex::IsBorder()
{
	NeighborList::iterator iter, iEnd;
	iEnd = m_Neighbors.end();
	for( iter=m_Neighbors.begin() ; iter!=iEnd ; ++iter )
	{
		int iCount = 0;

		FaceList::iterator iFace, iFaceEnd;
		iFaceEnd = m_Face.end();
		for( iFace=m_Face.begin() ; iFace!=iFaceEnd ; ++iFace )
		{
			if( (*iFace)->HasCommonVertex( *iter ) )
			{
				iCount++;
			}
		}

		if( iCount == 1 )
			return true;
	}

	return false;
}

bool ioProgressiveMesh::PMVertex::IsManifoldEdgeWith( PMVertex* pVert )
{
	int iSidesCount = 0;

	FaceList::iterator iFace;
	for( iFace=m_Face.begin() ; iFace!=m_Face.end() ; ++iFace )
	{
		if( (*iFace)->HasCommonVertex( pVert ) )
		{
			iSidesCount++;
		}
	}

	if( iSidesCount == 1 )
		return true;

	return false;
}

void ioProgressiveMesh::PMVertex::RemoveIfNonNeighbor( PMVertex *pVert )
{
	NeighborList::iterator iter = m_Neighbors.find( pVert );
	if( iter == m_Neighbors.end() )
		return;

	FaceList::iterator iFace;
	for( iFace=m_Face.begin() ; iFace!=m_Face.end() ; ++iFace )
	{
		if( (*iFace)->HasCommonVertex( pVert ) )
			return;
	}

	m_Neighbors.erase( pVert );

	if( m_Neighbors.empty() && !m_bToBeRemoved )
	{
		NotifyRemoved();
	}
}



















