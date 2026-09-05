

#ifndef _ioMath_h_
#define _ioMath_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioRay;
class ioSphere;
class ioPlane;
class ioAxisAlignBox;
class ioOrientBox;
class ioSegment;
class ioCylinder;

#define ALMOST_ZERO 1.0e-4f

class __EX ioMath
{
public:
	static D3DXQUATERNION QUAT_IDENTITY;
	static D3DXVECTOR3 VEC3_ZERO;
	static D3DXVECTOR3 UNIT_X;
	static D3DXVECTOR3 UNIT_Y;
	static D3DXVECTOR3 UNIT_Z;
	static D3DXVECTOR3 UNIT_ALL;
	static float FLOAT_INFINITY;

public:
	static float SqrDistance( const D3DXVECTOR3 &vPoint, const ioRay &rkRay );

	static bool TestIntersection( const ioRay &rkRay, const ioSphere &rkSphere );
	static bool TestIntersection( const ioRay &rkRay, const ioAxisAlignBox &rkBox );
	static bool TestIntersection( const ioRay &rkRay, const ioOrientBox &rkBox );

	static bool TestIntersection( const ioSegment &rkSegment, const ioOrientBox &rkBox );

	static bool TestIntersection( const ioSphere &rkSphere1, const ioSphere &rkSphere2 );
	static bool TestIntersection( const ioSphere &rkSphere, const ioAxisAlignBox &rkBox );
	static bool TestIntersection( const ioSphere &rkSphere, const D3DXVECTOR3 &vPoint );
	static bool TestIntersection( const ioSphere &rkSphere, const ioCylinder &rkCylinder );

	static bool TestIntersection( const ioAxisAlignBox &rkBox1, const ioAxisAlignBox &rkBox2 );

	static bool TestIntersection( const ioOrientBox &rkBox1, const ioOrientBox &rkBox2, float *pColAmt );
	static bool TestIntersection( const ioOrientBox &rkBox, const ioSphere &rkSphere );

	// Sphere식으로 대충만 테스트한다. 충돌했다고 반드시 충돌한것은 아닐수도 있다.
	static bool SphereTestLightly( const ioOrientBox &rkBox1, const ioOrientBox &rkBox2 );
	static bool SphereTestLightly( const ioOrientBox &rkBox, const ioSphere &rkSphere );

public:
	static bool TestIntersection( const D3DXVECTOR3 &v1,
								  const D3DXVECTOR3 &v2,
								  const D3DXVECTOR3 &v3, 
								  const ioAxisAlignBox &rkBox );

	static bool TestIntersection( const ioPlane &rkPlane, const ioCylinder &rkCylinder );
	static bool TestIntersection( const ioPlane &rkPlane, const ioOrientBox &rkBox );

	static bool InBox( const D3DXVECTOR3 &vPoint,
					   const ioOrientBox &rkBox,
					   float fEpsilon = ALMOST_ZERO );

	static bool IntersectionTri( const ioRay &rkRay,
							     const D3DXVECTOR3 &v0,
							     const D3DXVECTOR3 &v1,
							     const D3DXVECTOR3 &v2,
							     float *t,
							     D3DXVECTOR3 *pvPickPos = NULL );

	static bool FindIntersection( const D3DXVECTOR3 &vOrigin,
								  const D3DXVECTOR3 &vDirection,
								  const float fExtent[3],
								  float &fT0, float &fT1 );

	static bool FindIntersection( const ioRay &rkRay,
							      const ioOrientBox &rkBox,
							      int &iQuantity,
							      D3DXVECTOR3 vPoint[2] );

	static bool FindIntersection( const ioRay &rkRay,
								  const ioPlane &rkPlane,
								  D3DXVECTOR3 &vPoint );

	static bool FindIntersection( const ioRay &rkRay,
								  const ioSphere &rkSphere,
								  D3DXVECTOR3 vPoint[2],
								  bool bNearPointSort );

	static bool FindIntersection( const ioSegment &rkSegment,
								  const ioPlane &rkPlane,
								  D3DXVECTOR3 &vPoint );

	static bool FindIntersection( const ioSegment &rkSegment,
								  const ioOrientBox &rkBox,
								  int &iQuantity,
								  D3DXVECTOR3 vPoint[2] );

	static bool FindIntersection( const D3DXVECTOR3 &v1,
								  const D3DXVECTOR3 &v2,
								  const D3DXVECTOR3 &v3,
								  const ioOrientBox &rkBox,
								  int &iQuantity,
								  D3DXVECTOR3 akP[8] );

	static void ClipConvexPolygonAgainstPlane( const D3DXVECTOR3& rkNormal,
											   float fConstant,
											   int &riQuantity,
											   D3DXVECTOR3 *akP );

	static bool IsCulled( const ioPlane &rkPlane, const ioCylinder &rkCylinder );

	static void QuaterToEuler( const D3DXQUATERNION &qt,
							   float &fYaw,
							   float &fPitch,
							   float &fRoll );

	static D3DXQUATERNION QuaternionFromDirections( const D3DXVECTOR3 &vFromDir,
												    const D3DXVECTOR3 &vToDir );

	static D3DXMATRIX MatrixFromDirections( const D3DXVECTOR3 &vFromDir,
										    const D3DXVECTOR3 &vToDir );

	static D3DXQUATERNION MakeQuaternionByDegree( float fHeadD, float fPitchD, float fRollD );

	static float QuaterToYaw( const D3DXQUATERNION &qt );

	static float ArrangeHead( float fHead );

	static float ServerToClientAngle( float fAngle );
	static float ClientToServerAngle( float fAngle );

	static float AngleGap( float fStart, float fEnd );
	static int GetQuaterAngle( float fAngle );

	//Game Programming Gems 2.10 최단호 사원수, v1, v2둘다 단위벡터여야한다.
	static D3DXQUATERNION CalcShortestArcQuater( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2 );

	static float CalcHeadBetweenVec( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2 );
	static float CalcPitchVec( const D3DXVECTOR3 &vVec );

	static float RotateToTargetAngle( float fCurAngle, float fTargetAngle, float fAmt );

	static bool IsEqual( const D3DXVECTOR2 &v1, const D3DXVECTOR2 &v2, float fEpsilon = ALMOST_ZERO );
	static bool IsEqual( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fEpsilon = ALMOST_ZERO );
	static bool IsEqual( const D3DXVECTOR4 &v1, const D3DXVECTOR4 &v2, float fEpsilon = ALMOST_ZERO );
	static bool IsEqual( const D3DXQUATERNION &qt1, const D3DXQUATERNION &qt2, float fEpsilon = ALMOST_ZERO );

	// 0.0f ~ 1.0f
	static float UnitRandom() { return (float)rand() / RAND_MAX; }

	// fMin ~ fMax
	static float RangeRandom( float fMin, float fMax ) { return (fMax-fMin)*UnitRandom() + fMin; }

	// -1.0f ~ 1.0f
	static float SymmetricRandom() { return 2.0f * UnitRandom() - 1.0f; }

	static D3DXVECTOR3 RandomDeviant( float fAngle, const D3DXVECTOR3 &vVec );
	static D3DXVECTOR3 Deviant( float fAngle, float fRandAngle, const D3DXVECTOR3 &vVec );

	static D3DXVECTOR3 PerPendicular( const D3DXVECTOR3 &vVec );

	static D3DXVECTOR3 CalcPlaneNormal( const D3DXVECTOR3 &v0,
									    const D3DXVECTOR3 &v1,
									    const D3DXVECTOR3 &v2 );

	static void CalcMinMaxPoint( D3DXVECTOR3 &vMin,
							     D3DXVECTOR3 &vMax,
							     const Vector3Vec &vVtxList );

	static void CalcMinMaxPoint( D3DXVECTOR3 &vMin,
							     D3DXVECTOR3 &vMax,
							     const D3DXVECTOR3 *vVtxArray,
							     int iVtxCnt );

	static void CalcDirectionMatrix( D3DXMATRIX &matRotate, const D3DXVECTOR3 &vDir );
	static void CalcDirectionQuaternion( D3DXQUATERNION &qtRotate, const D3DXVECTOR3 &vDir );

	static void MakeTransform( const D3DXQUATERNION &qtOrientaton,
							   const D3DXVECTOR3 &vPosition,
							   const D3DXVECTOR3 &vScale,
							   D3DXMATRIX &matDest );

	static void MakeInverseTransform( const D3DXQUATERNION &qtOrientaton,
								      const D3DXVECTOR3 &vPosition,
								      const D3DXVECTOR3 &vScale,
								      D3DXMATRIX &matDest );

public:
	static D3DXVECTOR3 Lerp( float fFactor, const D3DXVECTOR3 &rkP, const D3DXVECTOR3 &rkQ );
	static D3DXQUATERNION Slerp( float fFactor, const D3DXQUATERNION &rkP, const D3DXQUATERNION &rkQ );
};

#endif
