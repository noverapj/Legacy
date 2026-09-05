

#ifndef _ioOpcodeShape_h_
#define _ioOpcodeShape_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResource.h"

class ioSphere;
class ioRay;
class ioOrientBox;

class __EX ioOpcodeShape : public ioResource
{
public:
	enum OpcCollType
	{
		COLLTYPE_QUICK,
		COLLTYPE_CONTACT,
	};

public:
	virtual void Begin( int iNumVertices, int iNumTriangles ) = 0;

	virtual void SetVertex( int iIndex, const D3DXVECTOR3 &vVtx ) = 0;
	virtual void SetTriangle( int iIndex, DWORD p0, DWORD p1, DWORD p2 ) = 0;
	virtual void SetVertexArray( const D3DXVECTOR3 *pVtx, int iNumVertex ) = 0;
	virtual void SetTriangleArray( const WORD *pIndex, int iNumTriangles ) = 0;

	virtual void End() = 0;

	virtual void ClearShape() = 0;

public:
	virtual bool TestIntersection( const D3DXMATRIX *pOwnMat,
								   const ioSphere &rkSphere,
								   bool bFindFaceList = false ) = 0;

	virtual bool TestIntersection( const D3DXMATRIX *pOwnMat,
								   const ioRay &rkRay,
								   int *pColFaceIndex,
								   D3DXVECTOR3 *pColPoint,
								   bool bCulling = true ) = 0;

	virtual bool TestIntersection( const D3DXMATRIX *pOwnMat,
								   const ioOrientBox &rkBox ) = 0;

	virtual bool TestIntersection( OpcCollType eType,
								   const D3DXMATRIX *pOwnMat,
								   ioOpcodeShape *pOther,
								   const D3DXMATRIX *pOtherMat ) = 0;

	virtual bool FindIntersection( const D3DXMATRIX *pOwnMat,
								   const ioOrientBox &rkBox,
								   int *pColFaceIndex ) = 0;

public:
	virtual void GetTriVertexInModelSpace( int iFaceIndex, D3DXVECTOR3 &v0,
														   D3DXVECTOR3 &v1,
														   D3DXVECTOR3 &v2 ) = 0;
	
	virtual void GetTriNormalInModelSpace( int iFaceIndex, D3DXVECTOR3 &vNormal ) = 0;

	virtual void GetTriVertexIndex( int iFaceIndex, int &iV0,
													int &iV1,
													int &iV2 ) = 0;

	virtual const IntVec& GetFindIntersectionFaceList() const = 0;

public:
	ioOpcodeShape( const ioHashString &name ) : ioResource( name )
	{
	}

	virtual ~ioOpcodeShape(){};
};

#endif
