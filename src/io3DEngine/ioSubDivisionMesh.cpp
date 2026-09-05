

#include "stdafx.h"

#include "ioSubDivisionMesh.h"

ioSubDivisionMesh::ioSubDivisionMesh()
{
	m_pChildVertices = NULL;
	m_pChildEdges = NULL;
	m_pChildFaces = NULL;
}

ioSubDivisionMesh::~ioSubDivisionMesh()
{
}

void ioSubDivisionMesh::SubDivide( ioDivisionMesh *pSrc )
{
	m_pChildVertices = pSrc->GetVertices();
	m_pChildEdges = pSrc->GetEdges();
	m_pChildFaces = pSrc->GetFaces();
	
	m_vVtxList.clear();
	m_vVtxList.reserve( m_pChildVertices->size() + m_pChildFaces->size() * 3 + 1000 );

	m_vEdgeList.clear();
	m_vEdgeList.reserve( m_pChildEdges->size() * 2 + m_pChildFaces->size() * 2 * 3 + 1000 );

	m_vFaceList.clear();
	m_vFaceList.reserve( m_pChildFaces->size() * 4 + 1000 );

	this->Even( m_pChildFaces, m_pChildEdges, m_pChildVertices );
	this->Recreate( m_pChildFaces, m_pChildEdges, m_pChildVertices );
	this->Odd( m_pChildFaces, m_pChildEdges, m_pChildVertices );
	this->Faces( m_pChildFaces, m_pChildEdges, m_pChildVertices );
}

/*
 * odd vertex masks - just halfway between 2
 *     1/2 o---x---o 1/2
 */
void ioSubDivisionMesh::Odd( heFaceList *pFaces,
					     heEdgeList *pEdges,
					     heVertexList *pVertices )
{
	heVertex kVertex;
	heVertexIter iVtxIter;

	heEdgeIter iEdge, iEdgeEnd;
	iEdgeEnd = pEdges->end();
	for( iEdge = pEdges->begin() ; iEdge!=iEdgeEnd ; ++iEdge )
	{
		if( iEdge < iEdge->iPair || iEdge->iPair == m_vEdgeList.end() )
		{
			kVertex.vPos = ( iEdge->iVertex->vPos + iEdge->iNext->iVertex->vPos ) * FLOAT05;
			kVertex.vNrm = ( iEdge->iVertex->vNrm + iEdge->iNext->iVertex->vNrm ) * FLOAT05;
			// kVertex.vUV = ( iEdge->iVertex->vUV + iEdge->iNext->iVertex->vUV ) * FLOAT05;
			m_vVtxList.push_back( kVertex );

			iVtxIter = m_vVtxList.end() - 1;

			SplitEdge( m_vEdgeList.begin() + (iEdge - pEdges->begin()), iVtxIter);
//				&m_vEdgeList[ iEdge - pEdges->begin() ], iVtxIter );
		}
	}
}

/*
 * even vertex masks - points just stay them same
 */
void ioSubDivisionMesh::Even( heFaceList *pFaces,
					          heEdgeList *pEdges,
					          heVertexList *pVertices )
{
	heVertexIter iter, iEnd;
	iEnd = pVertices->end();
	for( iter=pVertices->begin() ; iter!=iEnd ; ++iter )
	{
		m_vVtxList.push_back( *iter );
	}
}

/*
 * faces - construct new faces
 */
void ioSubDivisionMesh::Faces( heFaceList *pFaces,
							   heEdgeList *pEdges,
							   heVertexList *pVertices )
{
	heEdgeIter e1, e2;

	heFaceIter iFace, iFaceEnd, iNewFace;
	iFaceEnd = m_vFaceList.end();
	for( iFace=m_vFaceList.begin() ; iFace!=iFaceEnd ; ++iFace )
	{
		e1 = iFace->iEdge;
		e2 = e1->iNext;
		e2 = e2->iNext;
		iNewFace = SplitFace( iFace, e1, e2 );

		e1 = iFace->iEdge;
		e2 = e1->iPrev;
		e2 = e2->iPrev;
		iNewFace = SplitFace( iFace, e2, e1 );

		e1 = iFace->iEdge;
		e2 = e1->iNext;
		e2 = e2->iNext;
		iNewFace = SplitFace( iFace, e1, e2 );
	}
}

/*
 * re-create - recreates mesh conectivity with out copying vertices
 */
void ioSubDivisionMesh::Recreate( heFaceList *pFaces,
								  heEdgeList *pEdges,
								  heVertexList *pVertices )
{
	heEdgeIter iEdge, iEdgeEnd;
	iEdgeEnd = pEdges->end();
	for( iEdge = pEdges->begin() ; iEdge!=iEdgeEnd ; ++iEdge )
	{
//		CreateHalfEdge( &m_vVtxList[ iEdge->iVertex - pVertices->begin() ] );
		CreateHalfEdge( m_vVtxList.begin() + (iEdge->iVertex - pVertices->begin()));
	}

	iEdgeEnd = pEdges->end();
	for( iEdge = pEdges->begin() ; iEdge!=iEdgeEnd ; ++iEdge )
	{
// 		m_vEdgeList[ iEdge - pEdges->begin() ].iNext = &m_vEdgeList[ iEdge->iNext - pEdges->begin() ];
// 		m_vEdgeList[ iEdge - pEdges->begin() ].iPrev = &m_vEdgeList[ iEdge->iNext - pEdges->begin() ];

		m_vEdgeList[ iEdge - pEdges->begin() ].iNext = m_vEdgeList.begin() + (iEdge->iNext - pEdges->begin());
		m_vEdgeList[ iEdge - pEdges->begin() ].iPrev = m_vEdgeList.begin() + (iEdge->iNext - pEdges->begin());

		if( iEdge->iPair != m_vEdgeList.end())
		{
//			m_vEdgeList[ iEdge - pEdges->begin() ].iPair = &m_vEdgeList[ iEdge->iPair - pEdges->begin() ];
			m_vEdgeList[ iEdge - pEdges->begin() ].iPair = m_vEdgeList.begin() + (iEdge->iPair - pEdges->begin());
		}
// 		else
// 		{
// 			m_vEdgeList[ iEdge - pEdges->begin() ].iPair = NULL;
// 		}

//		m_vEdgeList[ iEdge - pEdges->begin() ].iVertex = &m_vVtxList[ iEdge->iVertex - pVertices->begin() ];
		m_vEdgeList[ iEdge - pEdges->begin() ].iVertex = m_vVtxList.begin() + ( iEdge->iVertex - pVertices->begin());
	}

	// recreate faces
	heFaceIter iFace, iFaceEnd;
	iFaceEnd = pFaces->end();
	for( iFace=pFaces->begin() ; iFace!=iFaceEnd ; ++iFace )
	{
//		CreateFace( &m_vEdgeList[ iFace->iEdge - pEdges->begin() ] );
		CreateFace( m_vEdgeList.begin() + (iFace->iEdge - pEdges->begin()) );
	}

	// link vertices to edges
	heVertexIter iVtx, iVtxEnd;
	iVtxEnd = pVertices->end();
	for( iVtx=pVertices->begin() ; iVtx!=iVtxEnd ; ++iVtx )
	{
//		m_vVtxList[ iVtx - pVertices->begin() ].iEdge = &m_vEdgeList[ iVtx->iEdge - pEdges->begin() ];
		m_vVtxList[ iVtx - pVertices->begin() ].iEdge = m_vEdgeList.begin() + (iVtx->iEdge - pEdges->begin());
	}
}

void ioSubDivisionMesh::Compile()
{
	m_vIdxList.clear();
	m_vIdxList.reserve( m_vFaceList.size() * 3 );

	heEdgeIter iEdge;
	heFaceIter iFace, iFaceEnd;
	iFaceEnd = m_vFaceList.end();
	for( iFace=m_vFaceList.begin() ; iFace!=iFaceEnd ; ++iFace )
	{
		iEdge = iFace->iEdge;

		do
		{
			m_vIdxList.push_back( iEdge->iVertex - m_vVtxList.begin() );
			iEdge = iEdge->iNext;
		}while( iFace->iEdge != iEdge );
	}
}

int ioSubDivisionMesh::GetRespectVtxCnt() const
{
	return m_vVtxList.size() + m_vFaceList.size() * 3 + 1000;
}

int ioSubDivisionMesh::GetRespectFaceCnt() const
{
	return m_vFaceList.size() * 4 + 1000;
}
