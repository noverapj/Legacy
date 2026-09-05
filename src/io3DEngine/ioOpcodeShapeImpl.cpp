

#include "stdafx.h"

#include "ioSphere.h"
#include "ioRay.h"
#include "ioOrientBox.h"
#include "ioPlane.h"

#include "ioResource.h"
#include "ioOpcodeShape.h"
#include "ioOpcodeShapeImpl.h"

#include "ioResourceManager.h"
#include "ioOpcodeManager.h"
#include "ioOpcodeManagerImpl.h"
#include "HelpFunc.h"

#include "ioMath.h"
#include "ioCPU.h"
#include "ErrorReport.h"

ioOpcodeShapeImpl::ioOpcodeShapeImpl( const ioHashString &name ) : ioOpcodeShape( name )
{
	m_iNumVertices = 0;
	m_iNumFaces	   = 0;
	m_pVertexData  = NULL;
	m_pFaceData	   = NULL;
}

ioOpcodeShapeImpl::~ioOpcodeShapeImpl()
{
	SAFEDELETEARRAY( m_pVertexData );
	SAFEDELETEARRAY( m_pFaceData );
}

void ioOpcodeShapeImpl::Begin( int iNumVertices, int iNumTriangles )
{
	assert( !m_pVertexData );
	assert( !m_pFaceData );

	m_iNumVertices = iNumVertices;
	m_iNumFaces	   = iNumTriangles;

	m_pVertexData = new float[ iNumVertices * 3 ];
	m_pFaceData   = new DWORD[ iNumTriangles * 3 ];
}

void ioOpcodeShapeImpl::SetVertex( int iIndex, const D3DXVECTOR3 &vVtx )
{
	assert( m_pVertexData );
	assert( COMPARE( iIndex, 0, m_iNumVertices ) );

	float *pPtr = m_pVertexData + 3 * iIndex;
	pPtr[0] = vVtx.x;
	pPtr[1] = vVtx.y;
	pPtr[2] = vVtx.z;
}

void ioOpcodeShapeImpl::SetTriangle( int iIndex, DWORD p0, DWORD p1, DWORD p2 )
{
	assert( m_pFaceData );
	assert( COMPARE( iIndex, 0, m_iNumFaces ) );
	assert( COMPARE( p0, 0, (DWORD)m_iNumVertices ) );
	assert( COMPARE( p1, 0, (DWORD)m_iNumVertices ) );
	assert( COMPARE( p2, 0, (DWORD)m_iNumVertices ) );

	DWORD *pPtr = m_pFaceData + 3 * iIndex;
	pPtr[0] = p0;
	pPtr[1] = p1;
	pPtr[2] = p2;
}

void ioOpcodeShapeImpl::SetVertexArray( const D3DXVECTOR3 *pVtx, int iNumVertex )
{
	assert( m_pVertexData );
	assert( m_iNumVertices == iNumVertex );

	ioCPU::FastMemcpy( m_pVertexData, (const float*)pVtx, sizeof(D3DXVECTOR3) * iNumVertex );
}

void ioOpcodeShapeImpl::SetTriangleArray( const WORD *pIndex, int iNumTriangles )
{
	assert( m_pFaceData );
	assert( m_iNumFaces == iNumTriangles );

	int iNumIndices = iNumTriangles * 3;
	for( int i=0 ; i<iNumIndices ; i++ )
	{
		m_pFaceData[i] = pIndex[i];
	}
}

void ioOpcodeShapeImpl::End()
{
	assert( m_iNumVertices > 0 && m_iNumFaces > 0 );
	assert( m_pVertexData && m_pFaceData );

	m_OpcMeshAccess.SetNbTriangles( m_iNumFaces );
	m_OpcMeshAccess.SetNbVertices( m_iNumVertices );
	m_OpcMeshAccess.SetPointers( (IndexedTriangle*)m_pFaceData, (Point*)m_pVertexData );

	OPCODECREATE opcc;
	opcc.mIMesh = &m_OpcMeshAccess;

	m_OpcModel.Build( opcc );
}

void ioOpcodeShapeImpl::ClearShape()
{
	m_iNumVertices = 0;
	m_iNumFaces	   = 0;
		
	SAFEDELETEARRAY( m_pVertexData );
	SAFEDELETEARRAY( m_pFaceData );
}

bool ioOpcodeShapeImpl::TestIntersection( const D3DXMATRIX *pOwnMat,
										  const ioSphere &rkSphere,
										  bool bFindFaceList )
{
	SphereCollider kCollider;
	kCollider.SetFirstContact( true );
	kCollider.SetTemporalCoherence( false );

	float fRadius = rkSphere.GetRadius();
	D3DXVECTOR3 vCenter = rkSphere.GetCenter();
	if( pOwnMat )
	{
		D3DXMATRIX matInvModel;
		D3DXMatrixInverse( &matInvModel, NULL, pOwnMat );
		D3DXVec3TransformCoord( &vCenter, &vCenter, &matInvModel );

		D3DXVECTOR3 vScale = CalcScaleFactor( pOwnMat );
		fRadius *= max( 1.0f/vScale.x, max( 1.0f/vScale.y, 1.0f/vScale.z ) );
	}

	SphereCache kSphereCache;
	Sphere kOpcSphere( Point( vCenter.x, vCenter.y, vCenter.z ), fRadius );
	kCollider.Collide( kSphereCache, kOpcSphere, m_OpcModel, NULL, NULL );

	if( !kCollider.GetContactStatus() )
		return false;

	int iNumFaces = kCollider.GetNbTouchedPrimitives();
	if( iNumFaces <= 0 )
		return false;

	if( bFindFaceList )
	{
		m_vIntersectionFaceList.erase( m_vIntersectionFaceList.begin(), m_vIntersectionFaceList.end() );
		m_vIntersectionFaceList.reserve( iNumFaces );

		const udword *pCollFaces = kCollider.GetTouchedPrimitives();
		for( int i=0 ; i<iNumFaces ; i++ )
		{
			m_vIntersectionFaceList.push_back( (int)pCollFaces[i] );
		}
	}

	return true;
}

bool ioOpcodeShapeImpl::TestIntersection( const D3DXMATRIX *pOwnMat,
										  const ioRay &rkRay,
										  int *pColFaceIndex,
										  D3DXVECTOR3 *pColPoint,
										  bool bCulling )
{
	RayCollider kCollider;
	kCollider.SetFirstContact( false );
	kCollider.SetTemporalCoherence( false );
	kCollider.SetClosestHit( true );
	kCollider.SetCulling( bCulling );
	kCollider.SetMaxDist();

	CollisionFaces kCF;	
	kCollider.SetDestination( &kCF );

	D3DXVECTOR3 vOrig, vDir;
	vOrig = rkRay.GetOrigin();
	vDir  = rkRay.GetDirection();

	if( pOwnMat )
	{
		D3DXMATRIX matInvModel;
		D3DXMatrixInverse( &matInvModel, NULL, pOwnMat );

		D3DXVec3TransformCoord( &vOrig, &vOrig, &matInvModel );
		D3DXVec3TransformNormal( &vDir, &vDir, &matInvModel );
	}

	Ray kRay;
	kRay.mOrig.Set( vOrig.x, vOrig.y, vOrig.z );
	kRay.mDir.Set( vDir.x, vDir.y, vDir.z );

	udword uFaceIndex = -1;
	if( !kCollider.Collide( kRay, m_OpcModel, NULL, &uFaceIndex ) )
		return false;

	if( uFaceIndex == -1 || !kCollider.GetContactStatus() )
		return false;

	if( !COMPARE( uFaceIndex, 0, (udword)m_iNumFaces ) )
	{
		LOG.PrintTimeAndLog( 0, "ioOpcodeShapeImpl::TestIntersection - Overflow %d/%d", uFaceIndex, m_iNumFaces );
		return false;
	}

	if( pColFaceIndex )
	{
		*pColFaceIndex = uFaceIndex;
	}

	if( pColPoint )
	{
		D3DXVECTOR3 v1, v2, v3;
		GetTriVertexInModelSpace( uFaceIndex, v1, v2, v3 );

		if( pOwnMat )
		{
			D3DXVec3TransformCoord( &v1, &v1, pOwnMat );
			D3DXVec3TransformCoord( &v2, &v2, pOwnMat );
			D3DXVec3TransformCoord( &v3, &v3, pOwnMat );
		}

		float fDistance;
		if( !ioMath::IntersectionTri( rkRay, v1, v2, v3, &fDistance, pColPoint ) )
		{
			//HARDCODE: 충돌이 일어나서 삼각형을 구했는데 실제 충돌점을 찾으려니 없는 경우를 대비함
			*pColPoint = ( v1 + v2 + v3 ) * 0.33333f;
		}
	}

	return true;
}

bool ioOpcodeShapeImpl::TestIntersection( const D3DXMATRIX *pOwnMat, const ioOrientBox &rkBox )
{
	if( rkBox.IsNull() )
		return false;

	OBBCollider kCollider;
	kCollider.SetFirstContact( true );
	kCollider.SetTemporalCoherence( false );
	kCollider.SetFullBoxBoxTest( true );

	ioOrientBox kInvBox( rkBox );
	if( pOwnMat )
	{
		D3DXMATRIX matInvModel;
		D3DXMatrixInverse( &matInvModel, NULL, pOwnMat );
		kInvBox.Transform( &matInvModel );
	}

	OBBCache kCache;
	OBB kOBB = ConvertOBB( kInvBox );
	kCollider.Collide( kCache, kOBB, m_OpcModel, NULL, NULL );

	if( kCollider.GetContactStatus() )
		return true;

	return false;
}

bool ioOpcodeShapeImpl::TestIntersection( OpcCollType eType,
										  const D3DXMATRIX *pOwnMat,
										  ioOpcodeShape *pOther,
										  const D3DXMATRIX *pOtherMat )
{
	AABBTreeCollider kColldier;

	switch( eType )
	{
	case COLLTYPE_QUICK:
		kColldier.SetFirstContact( true );
		break;
	case COLLTYPE_CONTACT:
		kColldier.SetFirstContact( false );
		break;
	}

	BVTCache kColCache;
	ioOpcodeShapeImpl *pOtherImpl = (ioOpcodeShapeImpl*)pOther;
	
	kColCache.Model0 = &m_OpcModel;
	kColCache.Model1 = &( pOtherImpl->m_OpcModel );

	const Matrix4x4 *pOpcOwn = (const Matrix4x4*)pOwnMat;
	const Matrix4x4 *pOpcOther = (const Matrix4x4*)pOtherMat;
	kColldier.Collide( kColCache, pOpcOwn, pOpcOther );

	if( kColldier.GetNbPairs() > 0 )
		return true;

	return false;
}

bool ioOpcodeShapeImpl::FindIntersection( const D3DXMATRIX *pOwnMat,
									      const ioOrientBox &rkBox,
									      int *pColFaceIndex )
{
	if( rkBox.IsNull() )
		return false;

	OBBCollider kCollider;
	kCollider.SetFirstContact( false );
	kCollider.SetTemporalCoherence( false );
	kCollider.SetFullBoxBoxTest( true );

	ioOrientBox kInvBox( rkBox );
	if( pOwnMat )
	{
		D3DXMATRIX matInvModel;
		D3DXMatrixInverse( &matInvModel, NULL, pOwnMat );
		kInvBox.Transform( &matInvModel );
	}

	m_vIntersectionFaceList.erase( m_vIntersectionFaceList.begin(), m_vIntersectionFaceList.end() );

	OBBCache kCache;
	OBB kOBB = ConvertOBB( kInvBox );
	kCollider.Collide( kCache, kOBB, m_OpcModel, NULL, NULL );

	if( !kCollider.GetContactStatus() )
		return false;

	int iNumFaces = kCollider.GetNbTouchedPrimitives();
	if( iNumFaces <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioOpcodeShapeImpl::FindIntersection - OBB Contact But No Faces!" );
		return false;
	}

	m_vIntersectionFaceList.reserve( iNumFaces );
	const udword *pCollFaces = kCollider.GetTouchedPrimitives();
	for( int i=0 ; i<iNumFaces ; i++ )
	{
		m_vIntersectionFaceList.push_back( pCollFaces[i] );
	}

	if( pColFaceIndex )
	{
		int iQuantity;
		D3DXVECTOR3 akP[12];
		D3DXVECTOR3 v1, v2, v3;

		float fMinDistSq = ioMath::FLOAT_INFINITY;
		for( int i=0 ; i<iNumFaces ; i++ )
		{
			int iFaceIndex = pCollFaces[i];
			GetTriVertexInModelSpace( iFaceIndex, v1, v2, v3 );

			if( pOwnMat )
			{
				D3DXVec3TransformCoord( &v1, &v1, pOwnMat );
				D3DXVec3TransformCoord( &v2, &v2, pOwnMat );
				D3DXVec3TransformCoord( &v3, &v3, pOwnMat );
			}

			iQuantity = 0;
			if( ioMath::FindIntersection( v1, v2, v3, rkBox, iQuantity, akP ) )
			{
				float fCurDistSq = 0.0f;
				for( int j=0 ; j<iQuantity ; j++ )
				{
					D3DXVECTOR3 vTemp = akP[j] - rkBox.GetCenter();

					fCurDistSq = D3DXVec3LengthSq( &vTemp );
					if( fCurDistSq < fMinDistSq )
					{
						fMinDistSq = fCurDistSq;
						*pColFaceIndex = iFaceIndex;
					}
				}
			}
		}
	}

	return true;
}

void ioOpcodeShapeImpl::GetTriVertexInModelSpace( int iFaceIndex,
												  D3DXVECTOR3 &v0,
												  D3DXVECTOR3 &v1,
												  D3DXVECTOR3 &v2 )
{
	assert( COMPARE( iFaceIndex, 0, m_iNumFaces ) );

	DWORD *pIndex = &( m_pFaceData[ 3 * iFaceIndex ] );

	float *pV0 = &( m_pVertexData[ 3 * pIndex[0] ] );
	float *pV1 = &( m_pVertexData[ 3 * pIndex[1] ] );
	float *pV2 = &( m_pVertexData[ 3 * pIndex[2] ] );

	v0.x = pV0[0], v0.y = pV0[1], v0.z = pV0[2];
	v1.x = pV1[0], v1.y = pV1[1], v1.z = pV1[2];
	v2.x = pV2[0], v2.y = pV2[1], v2.z = pV2[2];
}

void ioOpcodeShapeImpl::GetTriNormalInModelSpace( int iFaceIndex, D3DXVECTOR3 &vNormal )
{
	D3DXVECTOR3 v0, v1, v2;
	GetTriVertexInModelSpace( iFaceIndex, v0, v1, v2 );

	ioPlane kPlane( v0, v1, v2 );
	vNormal = kPlane.GetNormal();
}

void ioOpcodeShapeImpl::GetTriVertexIndex( int iFaceIndex, int &iV0, int &iV1, int &iV2 )
{
	assert( COMPARE( iFaceIndex, 0, m_iNumFaces ) );

	DWORD *pIndex = &( m_pFaceData[ 3 * iFaceIndex ] );

	iV0 = pIndex[0];
	iV1 = pIndex[1];
	iV2 = pIndex[2];
}

const IntVec& ioOpcodeShapeImpl::GetFindIntersectionFaceList() const
{
	return m_vIntersectionFaceList;
}

D3DXVECTOR3 ioOpcodeShapeImpl::CalcScaleFactor( const D3DXMATRIX *pMat )
{
	D3DXVECTOR3 vXAxis, vYAxis, vZAxis;
	D3DXVec3TransformNormal( &vXAxis, &ioMath::UNIT_X, pMat );
	D3DXVec3TransformNormal( &vYAxis, &ioMath::UNIT_Y, pMat );
	D3DXVec3TransformNormal( &vZAxis, &ioMath::UNIT_Z, pMat );

	D3DXVECTOR3 vScale;
	vScale.x = D3DXVec3Length( &vXAxis );
	vScale.y = D3DXVec3Length( &vYAxis );
	vScale.z = D3DXVec3Length( &vZAxis );

	return vScale;
}

D3DXVECTOR3 ioOpcodeShapeImpl::GetTriMidPoint( int iFaceIndex )
{
	assert( COMPARE( iFaceIndex, 0, m_iNumFaces ) );

	D3DXVECTOR3 v0, v1, v2;
	GetTriVertexInModelSpace( iFaceIndex, v0, v1, v2 );

	float fDiv = 1.0f / 3.0f;

	return ( v0 + v1 + v2 ) * fDiv;
}

OBB ioOpcodeShapeImpl::ConvertOBB( const ioOrientBox &rkBox )
{
	D3DXVECTOR3 vCenter = rkBox.GetCenter();

	Point kCenter, kExtents;
	kCenter.x = vCenter.x;
	kCenter.y = vCenter.y;
	kCenter.z = vCenter.z;
	kExtents.x = rkBox.GetExtents( 0 );
	kExtents.y = rkBox.GetExtents( 1 );
	kExtents.z = rkBox.GetExtents( 2 );

	const D3DXVECTOR3 *apAxis = rkBox.GetAxis();

	Matrix3x3 kMat;
	for( int i=0 ; i<3 ; i++ )
	{
		kMat.m[i][0] = apAxis[i].x;
		kMat.m[i][1] = apAxis[i].y;
		kMat.m[i][2] = apAxis[i].z;
	}

	return OBB( kCenter, kExtents, kMat );
}
