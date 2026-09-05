

#ifndef _ioSubDivisionMesh_h_
#define _ioSubDivisionMesh_h_

// GPG3 4.7
// "Portions Copyright (C) William Leeson, 2001"

#include "ioDivisionMesh.h"

class ioSubDivisionMesh : public ioDivisionMesh
{
protected:
	heVertexList *m_pChildVertices;
	heEdgeList *m_pChildEdges;
	heFaceList *m_pChildFaces;

public:
	virtual void SubDivide( ioDivisionMesh *pSrc );
	virtual void Compile();

protected:
	virtual void Odd( heFaceList *pFaces,
					  heEdgeList *pEdges,
					  heVertexList *pVertices );	// create odd vertices using odd mask

	virtual void Even( heFaceList *pFaces,
					   heEdgeList *pEdges,
					   heVertexList *pVertices );	// create even vertices using even mask

	virtual void Faces( heFaceList *pFaces,
					  heEdgeList *pEdges,
					  heVertexList *pVertices );	// create sub-faces

	virtual void Recreate( heFaceList *pFaces,
						   heEdgeList *pEdges,
						   heVertexList *pVertices );	// recreate mesh conectivity

public:
	int GetRespectVtxCnt() const;
	int GetRespectFaceCnt() const;

public:
	ioSubDivisionMesh();
	virtual ~ioSubDivisionMesh();
};

#endif