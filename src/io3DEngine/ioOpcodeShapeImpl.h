

#ifndef _ioOpcodeShapeImpl_h_
#define _ioOpcodeShapeImpl_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResource.h"

#include "opcode/opcode.h"

using namespace Opcode;

class ioSphere;
class ioRay;
class ioOrientBox;
class ioOpcodeShape;

class ioOpcodeShapeImpl : public ioOpcodeShape
{
protected:
	Model	m_OpcModel;
	MeshInterface  m_OpcMeshAccess;

	BVTCache	   *m_pOpcTreeCache;

	int m_iNumVertices;
	int	m_iNumFaces;
	float	*m_pVertexData;
	DWORD	*m_pFaceData;

	// FindIntersection을 호출했을 경우에만 값이 지정됨
	IntVec m_vIntersectionFaceList;

public:
	virtual void Begin( int iNumVertices, int iNumTriangles );

	virtual void SetVertex( int iIndex, const D3DXVECTOR3 &vVtx );
	virtual void SetTriangle( int iIndex, DWORD p0, DWORD p1, DWORD p2 );
	virtual void SetVertexArray( const D3DXVECTOR3 *pVtx, int iNumVertex );
	virtual void SetTriangleArray( const WORD *pIndex, int iNumTriangles );

	virtual void End();

	virtual void ClearShape();

public:
	virtual bool TestIntersection( const D3DXMATRIX *pOwnMat,
								   const ioSphere &rkSphere,
								   bool bFindFaceList = false );

	virtual bool TestIntersection( const D3DXMATRIX *pOwnMat,
								   const ioRay &rkRay,
								   int *pColFaceIndex,
								   D3DXVECTOR3 *pColPoint,
								   bool bCulling = true );

	virtual bool TestIntersection( const D3DXMATRIX *pOwnMat,
								   const ioOrientBox &rkBox );

	virtual bool TestIntersection( OpcCollType eType,
								   const D3DXMATRIX *pOwnMat,
								   ioOpcodeShape *pOther,
								   const D3DXMATRIX *pOtherMat );
	
	virtual bool FindIntersection( const D3DXMATRIX *pOwnMat,
								   const ioOrientBox &rkBox,
								   int *pColFaceIndex );

public:
	virtual void GetTriVertexInModelSpace( int iFaceIndex, D3DXVECTOR3 &v0,
														   D3DXVECTOR3 &v1,
														   D3DXVECTOR3 &v2 );
	
	virtual void GetTriNormalInModelSpace( int iFaceIndex, D3DXVECTOR3 &vNormal );
	virtual void GetTriVertexIndex( int iFaceIndex, int &iV0, int &iV1, int &iV2 );

	virtual const IntVec& GetFindIntersectionFaceList() const;

protected:
	D3DXVECTOR3 CalcScaleFactor( const D3DXMATRIX *pMat );

	D3DXVECTOR3 GetTriMidPoint( int iFaceIndex );
	OBB ConvertOBB( const ioOrientBox &rkBox );

public:
	ioOpcodeShapeImpl( const ioHashString &name );
	virtual ~ioOpcodeShapeImpl();
};

#endif
