

#include "stdafx.h"

#include "ioDivisionMesh.h"

ioDivisionMesh::ioDivisionMesh()
{
}

ioDivisionMesh::~ioDivisionMesh()
{
}

void ioDivisionMesh::Initialize( const Vector3Vec &vVtxList,
								 const Vector3Vec &vNrmList,
								 const WordVec &vIdxList )
{
	int i;
	int iVtxCnt = vVtxList.size();
	int iFaceCnt = vIdxList.size() / 3;

	m_vVtxList.clear();
	m_vFaceList.clear();
	m_vEdgeList.clear();

	m_vVtxList.reserve( iVtxCnt + 10 );
	m_vFaceList.reserve( iFaceCnt + 10 );
	m_vEdgeList.reserve( iFaceCnt * 4 + 10 );

	heVertex kVertex;
	for( i=0 ; i<iVtxCnt ; i++ )
	{
		kVertex.vPos = vVtxList[i];
		kVertex.vNrm = vNrmList[i];
		kVertex.iEdge._Ptr = NULL;

		m_vVtxList.push_back( kVertex );
	}

	int iFaceIdx = 0;
	WORD w1, w2, w3;
	heFaceIter iFace;
	heEdgeIter iEdge1, iEdge2, iEdge3, iPair;
	heVertexIter itrVtx = m_vVtxList.begin();
	
	for( i=0 ; i<iFaceCnt ; i++, iFaceIdx+=3 )
	{
		w1 = vIdxList[ iFaceIdx + 0 ];
		w2 = vIdxList[ iFaceIdx + 1 ];
		w3 = vIdxList[ iFaceIdx + 2 ];

		iEdge1 = CreateHalfEdge( itrVtx + w1);
		iEdge2 = CreateHalfEdge( itrVtx + w2);
		iEdge3 = CreateHalfEdge( itrVtx + w3);

		iFace = CreateFace( iEdge1, iEdge2, iEdge3 );
		
		iPair = FindEdge( itrVtx + w2, itrVtx + w1 );
		if( iPair !=  m_vEdgeList.end()	)
		{
			iEdge1->iPair = iPair;
			iPair->iPair = iEdge1;
		}

		iPair = FindEdge( itrVtx + w3, itrVtx + w2 );
		if( iPair !=  m_vEdgeList.end()	)
		{
			iEdge2->iPair = iPair;
			iPair->iPair = iEdge2;
		}

		iPair = FindEdge( itrVtx + w1, itrVtx + w3);
		if( iPair !=  m_vEdgeList.end()	)
		{
			iEdge3->iPair = iPair;
			iPair->iPair = iEdge3;
		}
	}
}

heEdgeIter ioDivisionMesh::CreateHalfEdge( heVertexIter iVtx )
{
	heEdge kEdge;

	kEdge.iVertex = iVtx;
// 	kEdge.iPrev._Ptr = NULL;
// 	kEdge.iNext._Ptr = NULL;
// 	kEdge.iPair._Ptr = NULL;
	kEdge.bCrease = false;
	m_vEdgeList.push_back( kEdge );
	iVtx->iEdge = m_vEdgeList.end() - 1;

	return iVtx->iEdge;
}

heEdgeIter ioDivisionMesh::CreateHalfEdge( heVertexIter iVtx,
										   heEdgeIter iPrev,
										   heEdgeIter iNext,
										   heEdgeIter iPair,
										   bool bCrease )
{
	heEdge kEdge;

	kEdge.iVertex = iVtx;
	kEdge.iPrev = iPrev;
	kEdge.iNext = iNext;
	kEdge.iPair = iPair;
	kEdge.bCrease = bCrease;
	m_vEdgeList.push_back( kEdge );

	iVtx->iEdge = m_vEdgeList.end() - 1;

	return iVtx->iEdge;
}

/*
 *  fill in an already existing face element
 */
heFaceIter ioDivisionMesh::CreateFace( heEdgeIter iEdge )
{
	heFace kFace;
	kFace.iEdge = iEdge;
	m_vFaceList.push_back( kFace );

	heFaceIter iEndFace = m_vFaceList.end() - 1;
	heEdgeIter iCursor = iEdge;
	do
	{
		iCursor->iFace = iEndFace;
		iCursor = iCursor->iNext;
	} while( iEdge != iCursor );

	return iEndFace;
}

heFaceIter ioDivisionMesh::CreateFace( heEdgeIter iEdge1,
								 	   heEdgeIter iEdge2,
									   heEdgeIter iEdge3 )
{
	heFace kFace;
	iEdge1->iNext = iEdge2;
	iEdge2->iNext = iEdge3;
	iEdge3->iNext = iEdge1;

	iEdge1->iPrev = iEdge3;
	iEdge2->iPrev = iEdge1;
	iEdge3->iPrev = iEdge2;

	kFace.iEdge = iEdge1;
	m_vFaceList.push_back( kFace );

	heFaceIter iCurFace = m_vFaceList.end() - 1;
	iEdge1->iFace = iCurFace;
	iEdge2->iFace = iCurFace;
	iEdge3->iFace = iCurFace;

	return iCurFace;
}

heFaceIter ioDivisionMesh::SplitFace( heFaceIter iFace,
									  heEdgeIter iEdge1,
									  heEdgeIter iEdge2 )
{
	heFace kNewFace;
	heVertexIter v1, v2;
	heEdgeIter ne1, ne2;
	heEdgeIter itrEg1;

	// create new face
	iFace->iEdge = iEdge1;
	kNewFace.iEdge = iEdge2;

	v1 = iEdge1->iNext->iVertex;
	v2 = iEdge2->iNext->iVertex;

	// create new edges to split face with
	ne1 = CreateHalfEdge( v2, iEdge2, iEdge1->iNext, itrEg1, false );
	ne2 = CreateHalfEdge( v1, iEdge1, iEdge2->iNext, ne1, false );
	ne1->iPair = ne2;

	// link in new edges
	iEdge1->iNext->iPrev = ne1;
	iEdge2->iNext->iPrev = ne2;

	// link in old edges
	iEdge1->iNext = ne2;
	iEdge2->iNext = ne1;

	// assign edges to correct face
	ne2->iFace = iFace;
	m_vFaceList.push_back( kNewFace );

	heFaceIter iCurFace = m_vFaceList.end() - 1;
	ne1->iFace = iCurFace;
	ne1->iNext->iFace = iCurFace;
	iEdge2->iFace = iCurFace;

	return iCurFace;
}

heEdgeIter ioDivisionMesh::FindEdge( heVertexIter v1, heVertexIter v2 )
{
	heEdgeIter iter, iEnd;
	iEnd = m_vEdgeList.end();
	for( iter=m_vEdgeList.begin() ; iter!=iEnd ; ++iter )
	{
		if( iter->iVertex == v1 &&
			iter->iNext != m_vEdgeList.end() &&
			iter->iNext->iVertex == v2 )
			return iter;
	}

	return iter;
}

void ioDivisionMesh::SplitEdge( heEdgeIter iEdge, heVertexIter iVertex )
{
	heEdgeIter he1, he2, he3, he4;

	he1 = iEdge;
	he2 = he1->iPair;

	// create new half-edge 1
	he3 = CreateHalfEdge( iVertex, he1, he1->iNext, he2, he1->bCrease );
	he3->iFace = he1->iFace;
	iVertex->iEdge = he3;
	he1->iNext->iPrev = he3;
	he1->iNext = he3;

	// create new half-edge 2
	if( he2 != m_vEdgeList.end())
	{
		he4 = CreateHalfEdge( iVertex, he2, he2->iNext, he1, he2->bCrease );
		he4->iFace = he2->iFace;
		he2->iNext->iPrev = he4;

		// update old edges
		he2->iPair = he3;
		he2->iNext = he4;
		he1->iPair = he4;
	}
}

void ioDivisionMesh::Normals()
{
	heFaceIter iFace, iFaceEnd;
	iFaceEnd = m_vFaceList.end();
	for( iFace=m_vFaceList.begin() ; iFace!=iFaceEnd ; ++iFace )
	{
		FaceNormal( iFace, iFace->vNrm );
	}
	
	D3DXVECTOR3 vNrm;
	heEdgeIter iEdge;

	heVertexIter iVtx, iVtxEnd;
	iVtxEnd = m_vVtxList.end();
	for( iVtx=m_vVtxList.begin() ; iVtx!=iVtxEnd ; ++iVtx )
	{
		iEdge = iVtx->iEdge;
		if( iEdge != m_vEdgeList.end() )
		{
			vNrm.x = vNrm.y = vNrm.z = 0.0f;
			do
			{
				iEdge = iEdge->iPair;
				if( iEdge != m_vEdgeList.end() )
				{
					iFace = iEdge->iFace;
					if( iFace != m_vFaceList.end() )
					{
						vNrm += iFace->vNrm;
					}

					iEdge = iEdge->iNext;
				}
			} while( iEdge != iVtx->iEdge && iEdge != m_vEdgeList.end() );

			if( iEdge == m_vEdgeList.end() )
			{
				iEdge = iVtx->iEdge;
				do
				{
					iFace = iEdge->iFace;
					if( iFace != m_vFaceList.end())
					{
						vNrm += iFace->vNrm;
					}

					iEdge = iEdge->iPrev->iPair;
				} while( iEdge!=iVtx->iEdge && iEdge != m_vEdgeList.end() );
			}

			D3DXVec3Normalize( &iVtx->vNrm, &vNrm );
		}
		else
		{
			iVtx->vNrm.x = 1.0f;
			iVtx->vNrm.y = 0.0f;
			iVtx->vNrm.z = 0.0f;
		}
	}
}

void ioDivisionMesh::FaceNormal( heFaceIter iFace, D3DXVECTOR3 &vNrm )
{
	heEdgeIter iEdge1, iEdge2, iEdge3;
	iEdge1 = iFace->iEdge;
	iEdge2 = iEdge1->iNext;
	iEdge3 = iEdge2->iNext;

	D3DXVECTOR3 v21, v31;
	v21 = iEdge2->iVertex->vPos - iEdge1->iVertex->vPos;
	v31 = iEdge3->iVertex->vPos - iEdge1->iVertex->vPos;

	D3DXVec3Cross( &vNrm, &v21, &v31 );
	D3DXVec3Normalize( &vNrm, &vNrm );
}

int ioDivisionMesh::GetVertexCount() const
{
	return m_vVtxList.size();
}

int ioDivisionMesh::GetFaceCount() const
{
	return m_vFaceList.size();
}