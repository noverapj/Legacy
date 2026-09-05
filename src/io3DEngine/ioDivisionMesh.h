

#ifndef _ioDivisionMesh_h_
#define _ioDivisionMesh_h_

#include "io3DCommon.h"

// GPG3 4.7
// "Portions Copyright (C) William Leeson, 2001"

struct heVertex;
struct heFace;
struct heEdge;

typedef std::vector< heVertex > heVertexList;
typedef heVertexList::iterator heVertexIter;

typedef std::vector< heFace > heFaceList;
typedef heFaceList::iterator heFaceIter;

typedef std::vector< heEdge > heEdgeList;
typedef heEdgeList::iterator heEdgeIter;

struct heVertex
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNrm;
	heEdgeIter iEdge;	// half edge containg vertex
};

struct heFace
{
	heEdgeIter iEdge;	// edges in face
	D3DXVECTOR3 vNrm;
};

struct heEdge
{
	heVertexIter iVertex;
	heEdgeIter iPair;	// next edge in graph i.e. at head
	heFaceIter iFace;	// next edge clockwise
	heEdgeIter iNext; // left and right faces
	heEdgeIter iPrev;
	bool bCrease;
};

class ioDivisionMesh
{
protected:
	heVertexList m_vVtxList;
	heEdgeList m_vEdgeList;
	heFaceList m_vFaceList;
	WordVec m_vIdxList;

public:
	void Initialize( const Vector3Vec &vVtxList,
					 const Vector3Vec &vNrmList,
					 const WordVec &vIdxList );

protected:
	virtual heEdgeIter CreateHalfEdge( heVertexIter iVtx );
	virtual heEdgeIter CreateHalfEdge( heVertexIter iVtx,
									   heEdgeIter iPrev,
									   heEdgeIter iNext,
									   heEdgeIter iPair,
									   bool bCrease );

	virtual heFaceIter CreateFace( heEdgeIter iEdge );
	virtual heFaceIter CreateFace( heEdgeIter iEdge1,
								   heEdgeIter iEdge2,
								   heEdgeIter iEdge3 );

	virtual heFaceIter SplitFace( heFaceIter iFace,
								  heEdgeIter iEdge1,
								  heEdgeIter iEdge2 );

	heEdgeIter FindEdge( heVertexIter v1, heVertexIter v2 );
	void SplitEdge( heEdgeIter iEdge, heVertexIter iVertex );


protected:
	virtual void Normals();
	void FaceNormal( heFaceIter iFace, D3DXVECTOR3 &vNrm );

public:
	inline heVertexList* GetVertices() { return &m_vVtxList; }
	inline heEdgeList* GetEdges() { return &m_vEdgeList; }
	inline heFaceList* GetFaces() { return &m_vFaceList; }
	inline WordVec* GetIndices() { return &m_vIdxList; }

	int GetVertexCount() const;
	int GetFaceCount() const;

public:
	ioDivisionMesh();
	virtual ~ioDivisionMesh();
};

#endif
