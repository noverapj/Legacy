

#include "stdafx.h"

#include "ioRay.h"
#include "ioSphere.h"
#include "ioPlane.h"
#include "ioAxisAlignBox.h"
#include "ioOrientBox.h"
#include "ioSegment.h"
#include "ioCylinder.h"

#include "ioMath.h"
#include "HelpFunc.h"
#include "IntersectionUtility.h"
#include "FastQuaternionSlerp.h"

D3DXQUATERNION ioMath::QUAT_IDENTITY( 0.0f, 0.0f, 0.0f, 1.0f );

D3DXVECTOR3 ioMath::VEC3_ZERO( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 ioMath::UNIT_X( 1.0f, 0.0f, 0.0f );
D3DXVECTOR3 ioMath::UNIT_Y( 0.0f, 1.0f, 0.0f );
D3DXVECTOR3 ioMath::UNIT_Z( 0.0f, 0.0f, 1.0f );
D3DXVECTOR3 ioMath::UNIT_ALL( 1.0f, 1.0f, 1.0f );
float ioMath::FLOAT_INFINITY = std::numeric_limits<float>::infinity();

float ioMath::SqrDistance( const D3DXVECTOR3 &vPoint, const ioRay &rkRay )
{
	D3DXVECTOR3 vDiff = vPoint - rkRay.GetOrigin();
	float fT = D3DXVec3Dot( &vDiff, &rkRay.GetDirection() );

	if( fT > 0.0f )
	{
		fT /= D3DXVec3Dot( &rkRay.GetDirection(), &rkRay.GetDirection() );	// rkRay.Direction * rkRay.Direction
		vDiff -= fT * rkRay.GetDirection();
	}

	return D3DXVec3Dot( &vDiff, &vDiff );
}

bool ioMath::TestIntersection( const ioRay &rkRay, const ioSphere &rkSphere )
{
	float fSqrDist = SqrDistance( rkSphere.GetCenter(), rkRay );
	return fSqrDist <= rkSphere.GetSqrRadius();
}

bool ioMath::TestIntersection( const ioRay &rkRay, const ioAxisAlignBox &rkBox )
{
	if( rkBox.IsNull() )
		return false;

	float fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fRhs;

	D3DXVECTOR3	kDiff = rkRay.GetOrigin() - rkBox.GetCenter();
	D3DXVECTOR3 kBoxExtent = rkBox.GetMaxPos() - rkBox.GetCenter();

	fWdU[0]  = rkRay.GetDirection().x;
	fAWdU[0] = fabs( fWdU[0] );
	fDdU[0]  = kDiff.x;
	fADdU[0] = fabs( fDdU[0] );
	if( fADdU[0] > kBoxExtent.x && fDdU[0]*fWdU[0] >= 0.0f )
		return false;

	fWdU[1]  = rkRay.GetDirection().y;
	fAWdU[1] = fabs( fWdU[1] );
	fDdU[1]  = kDiff.y;
	fADdU[1] = fabs( fDdU[1] );
	if( fADdU[1] > kBoxExtent.y && fDdU[1]*fWdU[1] >= 0.0f )
		return false;

	fWdU[2]  = rkRay.GetDirection().z;
	fAWdU[2] = fabs( fWdU[2] );
	fDdU[2]  = kDiff.z;
	fADdU[2] = fabs( fDdU[2] );
	if( fADdU[2] > kBoxExtent.z && fDdU[2]*fWdU[2] >= 0.0f )
		return false;

	D3DXVECTOR3 kWxD;
	D3DXVec3Cross( &kWxD, &rkRay.GetDirection(), &kDiff );

	fRhs = kBoxExtent.y * fAWdU[2] + kBoxExtent.z * fAWdU[1];
	if( fabs(kWxD.x) > fRhs )
		return false;

	fRhs = kBoxExtent.x * fAWdU[2] + kBoxExtent.z * fAWdU[0];
	if( fabs(kWxD.y) > fRhs )
		return false;

	fRhs = kBoxExtent.x * fAWdU[1] + kBoxExtent.y * fAWdU[0];
	if( fabs(kWxD.z) > fRhs )
		return false;	

	return true;
}

bool ioMath::TestIntersection( const ioRay &rkRay, const ioOrientBox &rkBox )
{
	if( rkBox.IsNull() )
		return false;

	float fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;

	D3DXVECTOR3 vDiff = rkRay.GetOrigin() - rkBox.GetCenter();

	fWdU[0]  = D3DXVec3Dot( &rkRay.GetDirection(), &rkBox.GetAxis(0) );
    fAWdU[0] = fabs( fWdU[0] );
    fDdU[0]  = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(0) );
    fADdU[0] = fabs( fDdU[0] );
    if ( fADdU[0] > rkBox.GetExtents(0) && fDdU[0]*fWdU[0] >= 0.0f )
        return false;

	fWdU[1]  = D3DXVec3Dot( &rkRay.GetDirection(), &rkBox.GetAxis(1) );
    fAWdU[1] = fabs( fWdU[1] );
    fDdU[1]  = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(1) );
    fADdU[1] = fabs( fDdU[1] );
    if ( fADdU[1] > rkBox.GetExtents(1) && fDdU[1]*fWdU[1] >= 0.0f )
        return false;

    fWdU[2]  = D3DXVec3Dot( &rkRay.GetDirection(), &rkBox.GetAxis(2) );
    fAWdU[2] = fabs( fWdU[2] );
    fDdU[2]  = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(2) );
    fADdU[2] = fabs( fDdU[2] );
    if ( fADdU[2] > rkBox.GetExtents(2) && fDdU[2]*fWdU[2] >= 0.0f )
        return false;

	D3DXVECTOR3 vWxD;
	D3DXVec3Cross( &vWxD, &rkRay.GetDirection(), &vDiff );

	fAWxDdU[0] = fabs( D3DXVec3Dot( &vWxD, &rkBox.GetAxis(0) ) );
    fRhs = rkBox.GetExtents(1)*fAWdU[2] + rkBox.GetExtents(2)*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = fabs( D3DXVec3Dot( &vWxD, &rkBox.GetAxis(1) ) );
    fRhs = rkBox.GetExtents(0)*fAWdU[2] + rkBox.GetExtents(2)*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = fabs( D3DXVec3Dot( &vWxD, &rkBox.GetAxis(2) ) );
    fRhs = rkBox.GetExtents(0)*fAWdU[1] + rkBox.GetExtents(1)*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

	return true;
}

bool ioMath::TestIntersection( const ioSegment &rkSegment, const ioOrientBox &rkBox )
{
	if( rkBox.IsNull() )
		return false;

	float fAWdU[3], fDdU, fWxDdU, fRhs;

	D3DXVECTOR3 vSDir = FLOAT05 * rkSegment.GetDirection();
	D3DXVECTOR3 vScen = rkSegment.GetOrigin() + vSDir;

	D3DXVECTOR3 vDiff = vScen - rkBox.GetCenter();

	fAWdU[0] = fabs( D3DXVec3Dot( &vSDir, &rkBox.GetAxis(0) ) );
	fDdU = fabs( D3DXVec3Dot( &vDiff, &rkBox.GetAxis(0) ) );
	fRhs = rkBox.GetExtents(0) + fAWdU[0];
	if( fDdU > fRhs )
		return false;

	fAWdU[1] = fabs( D3DXVec3Dot( &vSDir, &rkBox.GetAxis(1) ) );
	fDdU = fabs( D3DXVec3Dot( &vDiff, &rkBox.GetAxis(1) ) );
	fRhs = rkBox.GetExtents(1) + fAWdU[1];
	if( fDdU > fRhs )
		return false;

	fAWdU[2] = fabs( D3DXVec3Dot( &vSDir, &rkBox.GetAxis(2) ) );
	fDdU = fabs( D3DXVec3Dot( &vDiff, &rkBox.GetAxis(2) ) );
	fRhs = rkBox.GetExtents(2) + fAWdU[2];
	if( fDdU > fRhs )
		return false;

	D3DXVECTOR3 vWxD;
	D3DXVec3Cross( &vWxD, &vSDir, &vDiff );

	fWxDdU = fabs( D3DXVec3Dot( &vWxD, &rkBox.GetAxis(0) ) );
	fRhs = rkBox.GetExtents(1) * fAWdU[2] + rkBox.GetExtents(2)*fAWdU[1];
	if( fWxDdU > fRhs )
		return false;

	fWxDdU = fabs( D3DXVec3Dot( &vWxD, &rkBox.GetAxis(1) ) );
	fRhs = rkBox.GetExtents(0) * fAWdU[2] + rkBox.GetExtents(2)*fAWdU[0];
	if( fWxDdU > fRhs )
		return false;

	fWxDdU = fabs( D3DXVec3Dot( &vWxD, &rkBox.GetAxis(2) ) );
	fRhs = rkBox.GetExtents(0) * fAWdU[1] + rkBox.GetExtents(1)*fAWdU[0];
	if( fWxDdU > fRhs )
		return false;

	return true;
}

bool ioMath::TestIntersection( const ioSphere &rkSphere1, const ioSphere &rkSphere2 )
{
	float fRadiusSum = rkSphere1.GetRadius() + rkSphere2.GetRadius();
	D3DXVECTOR3 vLength = rkSphere1.GetCenter() - rkSphere2.GetCenter();

	if( D3DXVec3LengthSq( &vLength ) < fRadiusSum * fRadiusSum )
		return true;

	return false;
}

bool ioMath::TestIntersection( const ioSphere &rkSphere, const ioAxisAlignBox &rkBox )
{
	if( rkBox.IsNull() )
		return false;

	D3DXVECTOR3 vMin, vMax;
	vMin = rkBox.GetMinPos();
	vMax = rkBox.GetMaxPos();

	D3DXVECTOR3 vSphCenter = rkSphere.GetCenter();

	float fTotalDist = 0.0f;
	float fTempDist;
	for( int i=0 ; i<3 ; i++ )
	{
		if( vSphCenter[i] < vMin[i] )
		{
			fTempDist = vSphCenter[i] - vMin[i];
			fTotalDist += fTempDist * fTempDist;
		}
		else if( vSphCenter[i] > vMax[i] )
		{
			fTempDist = vSphCenter[i] - vMax[i];
			fTotalDist += fTempDist * fTempDist;
		}
	}

	if( fTotalDist <= rkSphere.GetSqrRadius() )
		return true;

	return false;
}

bool ioMath::TestIntersection( const ioSphere &rkSphere, const D3DXVECTOR3 &vPoint )
{
	D3DXVECTOR3 vLength = rkSphere.GetCenter() - vPoint;

	if( D3DXVec3LengthSq( &vLength ) < rkSphere.GetSqrRadius() )
		return true;

	return false;
}

bool ioMath::TestIntersection( const ioSphere &rkSphere, const ioCylinder &rkCylinder )
{
	float fRadius1 = rkCylinder.GetRadius();
	float fRadius2 = rkSphere.GetRadius();

	float fLength1 = rkCylinder.GetHeight();
	float fHalfLength1 = fLength1 * FLOAT05;

	D3DXVECTOR3 vDir1 = rkCylinder.GetDirection();
	D3DXVECTOR3 vGPos1 = rkCylinder.GetCenter() - vDir1 * fHalfLength1;

	D3DXVECTOR3 vDiff = rkSphere.GetCenter() - vGPos1;
	float s = D3DXVec3Dot( &vDiff, &rkCylinder.GetDirection() );
	if( s < -fRadius2 || s > (fLength1 + fRadius2 ) )
		return false;

	D3DXVECTOR3 C = s * vDir1 + vGPos1 - rkSphere.GetCenter();
	float t = D3DXVec3Length( &C );
	if( t > fRadius1 + fRadius2 )
		return false;

	if( t > fRadius1 && ( s < 0.0f || s > fLength1 ) )
	{
		if( s <= 0.0f )
		{
			float fDepth = fRadius2 - sqrt( s*s + (t-fRadius1)*(t-fRadius1) );
			if( fDepth < 0.0f )
				return false;
		}
		else
		{
			float fDepth = fRadius2 - sqrt( (s-fLength1)*(s-fLength1) + (t-fRadius1)*(t-fRadius1) );
			if( fDepth < 0.0f )
				return false;
		}
	}
	else if( (fRadius1 - t ) <= s && (fRadius1 - t ) <= ( fLength1 - s ) )
	{
		float fDepth = fRadius2 + fRadius1 - t;
		if( fDepth < 0.0f )
			return false;	// should never happen, but just for safeness
	}
	else
	{
		if( s <= fHalfLength1 )
		{
			float fDepth = s + fRadius2;
			if( fDepth < 0.0f )
				return false;	// should never happen, but just for safeness
		}
		else
		{
			float fDepth = fRadius2 + fLength1 - s;
			if( fDepth < 0.0f )
				return false;	// should never happen, but just for safeness
		}
	}

	return true;
}

bool ioMath::TestIntersection( const ioAxisAlignBox &rkBox1, const ioAxisAlignBox &rkBox2 )
{
	D3DXVECTOR3 vMin1, vMax1, vMin2, vMax2;
	vMin1 = rkBox1.GetMinPos();
	vMax1 = rkBox1.GetMaxPos();

	vMin2 = rkBox2.GetMinPos();
	vMax2 = rkBox2.GetMaxPos();

	if( vMin2.x > vMax1.x || vMin1.x > vMax2.x )
		return false;

	if( vMin2.y > vMax1.y || vMin1.y > vMax2.y )
		return false;

	if( vMin2.z > vMax1.z || vMin1.z > vMax2.z )
		return false;

	return true;
}

bool ioMath::TestIntersection( const ioOrientBox &rkBox1, const ioOrientBox &rkBox2, float *pColAmt )
{
	if( rkBox1.IsNull() || rkBox2.IsNull() )
		return false;

	const D3DXVECTOR3 *akA = rkBox1.GetAxis();
	const D3DXVECTOR3 *akB = rkBox2.GetAxis();
	const float *afEA = rkBox1.GetExtents();
	const float *afEB = rkBox2.GetExtents();

	D3DXVECTOR3 vDir = rkBox2.GetCenter() - rkBox1.GetCenter();

	float aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float aafAbsC[3][3];  // |c_{ij}|
    float afAD[3];        // Dot(A_i,D)
    float fR0, fR1, fR;   // interval radii and distance between centers
    float fR01;           // = R0 + R1

	// axis C0+t*A0
    aafC[0][0] = D3DXVec3Dot( &akA[0], &akB[0] );
    aafC[0][1] = D3DXVec3Dot( &akA[0], &akB[1] );
    aafC[0][2] = D3DXVec3Dot( &akA[0], &akB[2] );
    afAD[0] = D3DXVec3Dot( &akA[0], &vDir );
    aafAbsC[0][0] = fabs( aafC[0][0] );
    aafAbsC[0][1] = fabs( aafC[0][1] );
    aafAbsC[0][2] = fabs( aafC[0][2] );
    fR = fabs(  afAD[0] );
    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
    fR01 = afEA[0] + fR1;
    if ( fR > fR01 )
        return false;

	if( pColAmt )
	{
		*pColAmt = fR01 - fR;
	}

	// axis C0+t*A1
    aafC[1][0] = D3DXVec3Dot( &akA[1], &akB[0] );
    aafC[1][1] = D3DXVec3Dot( &akA[1], &akB[1] );
    aafC[1][2] = D3DXVec3Dot( &akA[1], &akB[2] );
    afAD[1] = D3DXVec3Dot( &akA[1], &vDir );
    aafAbsC[1][0] = fabs( aafC[1][0] );
    aafAbsC[1][1] = fabs( aafC[1][1] );
    aafAbsC[1][2] = fabs( aafC[1][2] );
    fR = fabs( afAD[1] );
    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
    fR01 = afEA[1] + fR1;
    if ( fR > fR01 )
        return false;

	// axis C0+t*A2
    aafC[2][0] = D3DXVec3Dot( &akA[2], &akB[0] );
    aafC[2][1] = D3DXVec3Dot( &akA[2], &akB[1] );
    aafC[2][2] = D3DXVec3Dot( &akA[2], &akB[2] );
    afAD[2] = D3DXVec3Dot( &akA[2], &vDir );
    aafAbsC[2][0] = fabs( aafC[2][0] );
    aafAbsC[2][1] = fabs( aafC[2][1] );
    aafAbsC[2][2] = fabs( aafC[2][2] );
    fR = fabs( afAD[2] );
    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
    fR01 = afEA[2] + fR1;
    if ( fR > fR01 )
        return false;

	if( pColAmt )
	{
		if( *pColAmt > ( fR01 - fR ) )
		{
			*pColAmt = fR01 - fR;
		}
	}

	// axis C0+t*B0
    fR = fabs( D3DXVec3Dot( &akB[0], &vDir ) );
    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
    fR01 = fR0 + afEB[0];
    if ( fR > fR01 )
        return false;

	// axis C0+t*B1
    fR = fabs( D3DXVec3Dot( &akB[1], &vDir ) );
    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
    fR01 = fR0 + afEB[1];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B2
    fR = fabs( D3DXVec3Dot( &akB[2], &vDir ) );
    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
    fR01 = fR0 + afEB[2];
    if ( fR > fR01 )
        return false;

	// axis C0+t*A0xB0
    fR = fabs( afAD[2]*aafC[1][0]-afAD[1]*aafC[2][0] );
    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

	// axis C0+t*A0xB1
    fR = fabs( afAD[2]*aafC[1][1]-afAD[1]*aafC[2][1] );
    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB2
    fR = fabs( afAD[2]*aafC[1][2]-afAD[1]*aafC[2][2] );
    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB0
    fR = fabs( afAD[0]*aafC[2][0]-afAD[2]*aafC[0][0] );
    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB1
    fR = fabs( afAD[0]*aafC[2][1]-afAD[2]*aafC[0][1] );
    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB2
    fR = fabs( afAD[0]*aafC[2][2]-afAD[2]*aafC[0][2] );
    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB0
    fR = fabs( afAD[1]*aafC[0][0]-afAD[0]*aafC[1][0] );
    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB1
    fR = fabs( afAD[1]*aafC[0][1]-afAD[0]*aafC[1][1] );
    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB2
    fR = fabs( afAD[1]*aafC[0][2]-afAD[0]*aafC[1][2] );
    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

	return true;
}

bool ioMath::TestIntersection( const ioOrientBox &rkBox, const ioSphere &rkSphere )
{
	if( rkBox.IsNull() )
		return false;

	D3DXVECTOR3 kCDiff = rkSphere.GetCenter() - rkBox.GetCenter();

	float fAx = fabs( D3DXVec3Dot( &kCDiff, &rkBox.GetAxis(0) ) );
	float fAy = fabs( D3DXVec3Dot( &kCDiff, &rkBox.GetAxis(1) ) );
	float fAz = fabs( D3DXVec3Dot( &kCDiff, &rkBox.GetAxis(2) ) );

	float fDx = fAx - rkBox.GetExtents( 0 );
	float fDy = fAy - rkBox.GetExtents( 1 );
	float fDz = fAz - rkBox.GetExtents( 2 );

	if( fAx <= rkBox.GetExtents(0) )
	{
		if( fAy <= rkBox.GetExtents(1) )
		{
			if( fAz <= rkBox.GetExtents(2) )
			{
				// sphere center inside box
				return true;
			}
			else
			{
				// potential sphere-face intersection with face z
				return fDz <= rkSphere.GetRadius();
			}
		}
		else
		{
			if( fAz <= rkBox.GetExtents(2) )
			{
				// potential sphere-face intersection with face y
				return fDy <= rkSphere.GetRadius();
			}
			else
			{
				// potential sphere-edge intersection with edge formed
                // by faces y and z
				return fDy*fDy + fDz*fDz <= rkSphere.GetSqrRadius();
			}
		}
	}
	else
	{
		if( fAy <= rkBox.GetExtents(1) )
		{
			if( fAz <= rkBox.GetExtents(2) )
			{
				// potential sphere-face intersection with face x
				return fDx <= rkSphere.GetRadius();
			}
			else
			{
				// potential sphere-edge intersection with edge formed
                // by faces x and z
				return fDx*fDx + fDz*fDz <= rkSphere.GetSqrRadius();
			}
		}
		else
		{
			if( fAz <= rkBox.GetExtents(2) )
			{
				// potential sphere-edge intersection with edge formed
                // by faces x and y
				return fDx*fDx + fDy*fDy <= rkSphere.GetSqrRadius();
			}
			else
			{
				// potential sphere-vertex intersection at corner formed
                // by faces x,y,z
				return fDx*fDx + fDy*fDy + fDz*fDz <= rkSphere.GetSqrRadius();
			}
		}
	}

	return false;
}

bool ioMath::SphereTestLightly( const ioOrientBox &rkBox1, const ioOrientBox &rkBox2 )
{
	float fRadiusSum = rkBox1.GetBoxRadius() + rkBox2.GetBoxRadius();
	D3DXVECTOR3 vLength = rkBox1.GetCenter() - rkBox2.GetCenter();

	if( D3DXVec3LengthSq( &vLength ) < fRadiusSum * fRadiusSum )
		return true;

	return false;
}

bool ioMath::SphereTestLightly( const ioOrientBox &rkBox, const ioSphere &rkSphere )
{
	float fRadiusSum = rkBox.GetBoxRadius() + rkSphere.GetRadius();
	D3DXVECTOR3 vLength = rkBox.GetCenter() - rkSphere.GetCenter();

	if( D3DXVec3LengthSq( &vLength ) < fRadiusSum * fRadiusSum )
		return true;

	return false;
}

bool ioMath::TestIntersection( const D3DXVECTOR3 &v1,
							   const D3DXVECTOR3 &v2,
							   const D3DXVECTOR3 &v3, 
							   const ioAxisAlignBox &rkBox )
{
	D3DXVECTOR3 vBoxCenter = rkBox.GetCenter();
	D3DXVECTOR3 vBoxHalfSize = ( rkBox.GetMaxPos() - rkBox.GetMinPos() ) * FLOAT05;

	float aTriVerts[3][3];
	aTriVerts[0][0] = v1.x;
	aTriVerts[0][1] = v1.y;
	aTriVerts[0][2] = v1.z;

	aTriVerts[1][0] = v2.x;
	aTriVerts[1][1] = v2.y;
	aTriVerts[1][2] = v2.z;

	aTriVerts[2][0] = v3.x;
	aTriVerts[2][1] = v3.y;
	aTriVerts[2][2] = v3.z;

	if( triBoxOverlap( vBoxCenter, vBoxHalfSize, aTriVerts ) == 0 )
		return false;

	return true;
}

bool ioMath::TestIntersection( const ioPlane &rkPlane, const ioCylinder &rkCylinder )
{
	// Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|

	float fSDist = D3DXVec3Dot( &rkPlane.GetNormal(), &rkCylinder.GetCenter() ) - rkPlane.GetD();
	float fAbsNdW = fabsf( D3DXVec3Dot( &rkPlane.GetNormal(), &rkCylinder.GetDirection() ) );
	float fRoot = sqrt( fabsf( 1.0f - fAbsNdW*fAbsNdW ) );

	float fTerm = rkCylinder.GetRadius() * fRoot + FLOAT05 * rkCylinder.GetHeight() * fAbsNdW;

	return fabsf( fSDist ) <= fTerm;
}

bool ioMath::TestIntersection( const ioPlane &rkPlane, const ioOrientBox &rkBox )
{
	D3DXVECTOR3 vNormal = rkPlane.GetNormal();
	float fTemp0 = rkBox.GetExtents(0) * ( D3DXVec3Dot( &vNormal, &rkBox.GetAxis(0) ) );
	float fTemp1 = rkBox.GetExtents(1) * ( D3DXVec3Dot( &vNormal, &rkBox.GetAxis(1) ) );
	float fTemp2 = rkBox.GetExtents(2) * ( D3DXVec3Dot( &vNormal, &rkBox.GetAxis(2) ) );

	float fRadius = fabsf( fTemp0 ) + fabsf( fTemp1 ) + fabsf( fTemp2 );
	float fPseudoDistance = rkPlane.GetDistance( rkBox.GetCenter() );

	if( fabsf( fPseudoDistance ) <= fRadius )
		return true;

	return false;
}

bool ioMath::InBox( const D3DXVECTOR3 &vPoint, const ioOrientBox &rkBox, float fEpsilon )
{
	D3DXVECTOR3 vDiff = vPoint - rkBox.GetCenter();
	for( int i=0 ; i<3 ; i++ )
	{
		float fCoeff = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(i) );
		if( fabs( fCoeff ) > rkBox.GetExtents(i) + fEpsilon )
			return false;
	}

	return true;
}

bool ioMath::FindIntersection( const D3DXVECTOR3 &vOrigin,
							   const D3DXVECTOR3 &vDirection,
							   const float fExtent[3],
							   float &fT0, float &fT1 )
{
	float fSaveT0 = fT0, fSaveT1 = fT1;

	bool bNotEntirelyClipped = Clip( +vDirection.x, -vOrigin.x-fExtent[0], fT0, fT1) &&
							   Clip( -vDirection.x, +vOrigin.x-fExtent[0], fT0, fT1) &&
							   Clip( +vDirection.y, -vOrigin.y-fExtent[1], fT0, fT1) &&
							   Clip( -vDirection.y, +vOrigin.y-fExtent[1], fT0, fT1) &&
							   Clip( +vDirection.z, -vOrigin.z-fExtent[2], fT0, fT1) &&
							   Clip( -vDirection.z, +vOrigin.z-fExtent[2], fT0, fT1);

	return bNotEntirelyClipped && ( fT0 != fSaveT0 || fT1 != fSaveT1 );
}

bool ioMath::FindIntersection( const ioRay &rkRay,
							   const ioOrientBox &rkBox,
							   int &iQuantity,
							   D3DXVECTOR3 vPoint[2] )
{
	if( rkBox.IsNull() )
		return false;

	D3DXVECTOR3 vDiff = rkRay.GetOrigin() - rkBox.GetCenter();

	D3DXVECTOR3 vOrigin;
	vOrigin.x = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(0) );
	vOrigin.y = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(1) );
	vOrigin.z = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(2) );

	D3DXVECTOR3 vDirection;
	vDirection.x = D3DXVec3Dot( &rkRay.GetDirection(), &rkBox.GetAxis(0) );
	vDirection.y = D3DXVec3Dot( &rkRay.GetDirection(), &rkBox.GetAxis(1) );
	vDirection.z = D3DXVec3Dot( &rkRay.GetDirection(), &rkBox.GetAxis(2) );

	float INFINITY = std::numeric_limits<float>::infinity();
	float fT0 = 0.0f, fT1 = INFINITY;

	bool bIntersects = FindIntersection( vOrigin, vDirection, rkBox.GetExtents(), fT0, fT1 );

	if( !bIntersects )
	{
		iQuantity = 0;
		return false;
	}

	if( fT0 > 0.0f )
	{
		if( fT1 < INFINITY )
		{
			iQuantity = 2;
			vPoint[0] = rkRay.GetOrigin() + fT0 * rkRay.GetDirection();
			vPoint[1] = rkRay.GetOrigin() + fT1 * rkRay.GetDirection();
		}
		else
		{
			iQuantity = 1;
			vPoint[0] = rkRay.GetOrigin() + fT0 * rkRay.GetDirection();
		}
	}
	else	// fT0 == 0.0f
	{
		if( fT1 < INFINITY )
		{
			iQuantity = 1;
			vPoint[0] = rkRay.GetOrigin() + fT1 * rkRay.GetDirection();
		}
		else // fT1 == INFINITY
		{
			// assert: should not get here
			iQuantity = 0;
		}
	}

	return true;
}

bool ioMath::FindIntersection( const ioRay &rkRay,
							   const ioPlane &rkPlane,
							   D3DXVECTOR3 &vPoint )
{
	D3DXVECTOR3 vRayDir   = rkRay.GetDirection();
	D3DXVECTOR3 vPlaneNrm = rkPlane.GetNormal();

	float fNV = D3DXVec3Dot( &vPlaneNrm, &vRayDir );
	if( fabsf(fNV) < ALMOST_ZERO )
		return false;

	D3DXVECTOR3 vRayOrigin = rkRay.GetOrigin();
	float fNQ = D3DXVec3Dot( &vPlaneNrm, &vRayOrigin );

	float fT = - ( fNQ + rkPlane.GetD() ) / fNV;

	vPoint = vRayOrigin + fT * vRayDir;

	return true;
}

bool ioMath::FindIntersection( const ioSegment &rkSegment,
							   const ioPlane &rkPlane,
							   D3DXVECTOR3 &vPoint )
{
	D3DXVECTOR3 vOrigin = rkSegment.GetOrigin();
	D3DXVECTOR3 vPlaneNrm = rkPlane.GetNormal();
	
	float fT0 = D3DXVec3Dot( &vOrigin, &vPlaneNrm );

	D3DXVECTOR3 vEnd = vOrigin + rkSegment.GetDirection();
	float fT1 = D3DXVec3Dot( &vEnd, &vPlaneNrm );

	if( fT0 == fT1 )
		return false;

	float fFinalT = -( fT1 + rkPlane.GetD() ) / ( fT0 - fT1 );
	if( fFinalT < 0.0f || fFinalT > 1.0f )
		return false;

	vPoint = vOrigin * fFinalT + vEnd * ( 1.0f - fFinalT );
	return true;
}

bool ioMath::FindIntersection( const ioSegment &rkSegment,
							   const ioOrientBox &rkBox,
							   int &iQuantity,
							   D3DXVECTOR3 vPoint[2] )
{
	if( rkBox.IsNull() )
		return false;

	D3DXVECTOR3 vDiff = rkSegment.GetOrigin() - rkBox.GetCenter();

	D3DXVECTOR3 vOrigin;
	vOrigin.x = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(0) );
	vOrigin.y = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(1) );
	vOrigin.z = D3DXVec3Dot( &vDiff, &rkBox.GetAxis(2) );

	D3DXVECTOR3 vDirection;
	vDirection.x = D3DXVec3Dot( &rkSegment.GetDirection(), &rkBox.GetAxis(0) );
	vDirection.y = D3DXVec3Dot( &rkSegment.GetDirection(), &rkBox.GetAxis(1) );
	vDirection.z = D3DXVec3Dot( &rkSegment.GetDirection(), &rkBox.GetAxis(2) );

	float fT0 = 0.0f;
	float fT1 = 1.0f;

	bool bIntersects = FindIntersection( vOrigin, vDirection, rkBox.GetExtents(), fT0, fT1 );

	if( bIntersects )
	{
		if( fT0 > 0.0f )
		{
			if( fT1 < 1.0f )
			{
				iQuantity = 2;
				vPoint[0] = rkSegment.GetOrigin() + fT0*rkSegment.GetDirection();
				vPoint[1] = rkSegment.GetOrigin() + fT1*rkSegment.GetDirection();
			}
			else
			{
				iQuantity = 1;
				vPoint[0] = rkSegment.GetOrigin() + fT0*rkSegment.GetDirection();
			}
		}
		else	// fT0 == 0.0f
		{
			if( fT1 < 1.0f )
			{
				iQuantity = 1;
				vPoint[0] = rkSegment.GetOrigin() + fT1*rkSegment.GetDirection();
			}
			else	// fT1 == 1.0f
			{
				// segment entirely in box
				iQuantity = 0;
			}
		}
	}
	else
	{
		iQuantity = 0;
	}

	return bIntersects;
}

bool ioMath::FindIntersection( const ioRay &rkRay,
							   const ioSphere &rkSphere,
							   D3DXVECTOR3 vPoint[2],
							   bool bNearPointSort )
{
	D3DXVECTOR3 vDiff = rkRay.GetOrigin() - rkSphere.GetCenter();

	float fA = D3DXVec3LengthSq( &rkRay.GetDirection() );
	float fB = D3DXVec3Dot( &vDiff, &rkRay.GetDirection() );
	float fC = D3DXVec3LengthSq( &vDiff ) - rkSphere.GetSqrRadius();

	float fDiscr = fB*fB - fA*fC;
	if( fDiscr > 0.0f )
	{
		float fRoot = sqrt( fDiscr );
		float fInvA = 1.0f/fA;

		float afT[2];
		afT[0] = ( -fB - fRoot ) * fInvA;
		afT[1] = ( -fB + fRoot ) * fInvA;

		if( afT[0] >= 0.0f )
		{
			if( !bNearPointSort )
			{
				vPoint[0] = rkRay.GetOrigin() + afT[0]*rkRay.GetDirection();
				vPoint[1] = rkRay.GetOrigin() + afT[1]*rkRay.GetDirection();
			}
			else
			{
				if( fabsf( afT[0] ) < fabsf( afT[1] ) )
				{
					vPoint[0] = rkRay.GetOrigin() + afT[0] * rkRay.GetDirection();
					vPoint[1] = rkRay.GetOrigin() + afT[1] * rkRay.GetDirection();
				}
				else
				{
					vPoint[0] = rkRay.GetOrigin() + afT[1] * rkRay.GetDirection();
					vPoint[1] = rkRay.GetOrigin() + afT[0] * rkRay.GetDirection();
				}
			}
			
			return true;
		}
	}

	return false;
}

bool ioMath::FindIntersection( const D3DXVECTOR3 &v1,
							   const D3DXVECTOR3 &v2,
							   const D3DXVECTOR3 &v3,
							   const ioOrientBox &rkBox,
							   int &iQuantity,
							   D3DXVECTOR3 akP[8] )
{
	if( rkBox.IsNull() )
		return false;

	iQuantity = 3;
	akP[0] = v1;
	akP[1] = v2;
	akP[2] = v3;

	float fConstant;
	D3DXVECTOR3 kInnerNormal;
	for( int iDir=-1 ; iDir<=1 ; iDir += 2 )
	{
		for( int iSide=0 ; iSide<3 ; iSide++ )
		{
			kInnerNormal = ( (float)iDir ) * rkBox.GetAxis( iSide );
			fConstant = D3DXVec3Dot( &kInnerNormal, &rkBox.GetCenter() ) - rkBox.GetExtents( iSide );
			ClipConvexPolygonAgainstPlane( kInnerNormal, fConstant, iQuantity, akP );
		}
	}

	if( iQuantity > 0 )
		return true;

	return false;
}

void ioMath::ClipConvexPolygonAgainstPlane( const D3DXVECTOR3& rkNormal,
											float fConstant,
											int &riQuantity,
											D3DXVECTOR3 *akP )
{
	int iPositive = 0, iNegative = 0, iPIndex = -1;
	int iQuantity = riQuantity;

	float afTest[8];
	int i;

	for( i=0 ; i<riQuantity ; i++ )
	{
		afTest[i] = D3DXVec3Dot( &rkNormal, &akP[i] ) - fConstant + fabsf(fConstant) * FLT_EPSILON;

		if( afTest[i] >= 0.0f )
		{
			iPositive++;
			if( iPIndex < 0 )
				iPIndex = i;
		}
		else
		{
			iNegative++;
		}
	}

	if( riQuantity == 2 )
	{
		if( iPositive > 0 )
		{
			if( iNegative > 0 )
			{
				int iClip;

				if( iPIndex == 0 )
				{
					iClip = 1;
				}
				else
				{
					iClip = 0;
				}

				float fT = afTest[iPIndex]/( afTest[iPIndex] - afTest[iClip] );
				akP[iClip] = akP[iPIndex] + fT*( akP[iClip] - akP[iPIndex] );
			}
		}
		else
		{
			riQuantity = 0;
		}
	}
	else
	{
		if( iPositive > 0 )
		{
			if( iNegative > 0 )
			{
				D3DXVECTOR3 akCV[8];
				int iCQuantity = 0, iCur, iPrv;
				float fT;

				if( iPIndex > 0 )
				{
					iCur = iPIndex;
					iPrv = iCur-1;

					fT = afTest[iCur] / ( afTest[iCur]-afTest[iPrv] );
					akCV[iCQuantity++] = akP[iCur] + fT * ( akP[iPrv] - akP[iCur] );

					while( iCur < iQuantity && afTest[iCur] >= 0.0f )
						akCV[iCQuantity++] = akP[iCur++];

					if( iCur < iQuantity )
					{
						iPrv = iCur-1;
					}
					else
					{
						iCur = 0;
						iPrv = iQuantity - 1;
					}

					fT = afTest[iCur]/( afTest[iCur]-afTest[iPrv] );
					akCV[iCQuantity++] = akP[iCur] + fT * ( akP[iPrv] - akP[iCur] );
				}
				else	// iPIndex == 0 
				{
					iCur = 0;
					while( iCur < iQuantity && afTest[iCur] >= 0.0f )
						akCV[iCQuantity++] = akP[iCur++];

					iPrv = iCur-1;
					fT = afTest[iCur]/( afTest[iCur] - afTest[iPrv] );
					akCV[iCQuantity++] = akP[iCur] + fT * ( akP[iPrv] - akP[iCur] );

					while( iCur < iQuantity && afTest[iCur] < 0.0f )
						iCur++;

					if( iCur < iQuantity )
					{
						iPrv = iCur-1;
						fT = afTest[iCur]/( afTest[iCur] - afTest[iPrv] );
						akCV[iCQuantity++] = akP[iCur] + fT * ( akP[iPrv] - akP[iCur] );

						while( iCur < iQuantity && afTest[iCur] >= 0.0f )
						{
							akCV[iCQuantity++] = akP[iCur++];
						}
					}
					else
					{
						iPrv = iQuantity - 1;
						fT = afTest[0] / ( afTest[0] - afTest[iPrv] );
						akCV[iCQuantity++] = akP[0] + fT*(akP[iPrv] - akP[0] );
					}
				}

				iQuantity = iCQuantity;
				memcpy( akP, akCV, iCQuantity*sizeof(D3DXVECTOR3) );
			}

			riQuantity = iQuantity;
		}
		else
		{
			riQuantity = 0;
		}
	}
}

bool ioMath::IntersectionTri( const ioRay &rkRay,
							  const D3DXVECTOR3 &v0,
							  const D3DXVECTOR3 &v1,
							  const D3DXVECTOR3 &v2,
							  float *t,
							  D3DXVECTOR3 *pvPickPos )
{
	float u, v;

	if( !D3DXIntersectTri( &v0, &v1, &v2, &rkRay.GetOrigin(), &rkRay.GetDirection(), &u, &v, t ) )
		return false;

	if( pvPickPos )
	{
		*pvPickPos = v0 + u * ( v1 - v0 ) + v * ( v2 - v0 );
	}

	return true;
}

/*
bool ioMath::IntersectionTri( const ioRay &rkRay,
							  const D3DXVECTOR3 &v0,
 							  const D3DXVECTOR3 &v1,
							  const D3DXVECTOR3 &v2,
							  float *t,
							  D3DXVECTOR3 *pvPickPos )
{
	D3DXVECTOR3 vEdge1 = v1 - v0;
	D3DXVECTOR3 vEdge2 = v2 - v0;

	D3DXVECTOR3 vEdge2Cross;
	D3DXVec3Cross( &vEdge2Cross, &rkRay.GetDirection(), &vEdge2 );

	float fDet = D3DXVec3Dot( &vEdge1, &vEdge2Cross );

//---------- dx9.0 sample is Modifyed -------------
// HARDCODE: dx의 함수는 이쪽을 바라보지 않는 삼각형까지 체크해서 추려낸다.
//           원칙적인 함수의 의미로 보자면 dx쪽이 맞지만 우린 백페이스까지 체크해줄 필요가 없다.

	if( fDet < ALMOST_ZERO )
		return false;

	D3DXVECTOR3 vTVec = rkRay.GetOrigin() - v0;

	float u = D3DXVec3Dot( &vTVec, &vEdge2Cross );
	if( u < 0.0f || u > fDet )
		return false;

	D3DXVECTOR3 vEdge1Cross;
	D3DXVec3Cross( &vEdge1Cross, &vTVec, &vEdge1 );

	float v = D3DXVec3Dot( &rkRay.GetDirection(), &vEdge1Cross );
	if( v < 0.0f || u + v > fDet )
		return false;

	*t = D3DXVec3Dot( &vEdge2, &vEdge1Cross );

	float fInvDet = 1.0f / fDet;
	*t *= fInvDet;

	if( pvPickPos )
	{
		u *= fInvDet;
		v *= fInvDet;
		*pvPickPos = v0 + vEdge1 * u + vEdge2 * v;
	}

	return true;
}
*/
bool ioMath::IsCulled( const ioPlane &rkPlane, const ioCylinder &rkCylinder )
{
	// Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|

	float fSDist = rkPlane.GetDistance( rkCylinder.GetCenter() );
	float fAbsNdW = fabsf( D3DXVec3Dot( &rkPlane.GetNormal(), &rkCylinder.GetDirection() ) );
	float fRoot = sqrt( fabsf( 1.0f - fAbsNdW*fAbsNdW ) );

	float fTerm = rkCylinder.GetRadius()*fRoot + FLOAT05*rkCylinder.GetHeight()*fAbsNdW;

	return fSDist + fTerm <= 0.0f;
}

void ioMath::QuaterToEuler( const D3DXQUATERNION &qt,
							float &fYaw,
							float &fPitch,
							float &fRoll )
{
	float sqw = qt.w*qt.w;
	float sqx = qt.x*qt.x;
	float sqy = qt.y*qt.y;
	float sqz = qt.z*qt.z;

	fPitch = asinf(2.0f * (qt.w*qt.x - qt.y*qt.z)); // rotation about x-axis
	fYaw   = atan2f(2.0f * (qt.x*qt.z + qt.w*qt.y),(-sqx - sqy + sqz + sqw)); // rotation about y-axis
	fRoll  = atan2f(2.0f * (qt.x*qt.y + qt.w*qt.z),(-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

D3DXMATRIX ioMath::MatrixFromDirections( const D3DXVECTOR3 &vFromDir,
										 const D3DXVECTOR3 &vToDir )
{
	D3DXVECTOR3 vUp, vSide;
	D3DXVec3Cross( &vUp, &vFromDir, &vToDir );
	D3DXVec3Cross( &vSide, &vUp, &vFromDir );

	D3DXMATRIX matFrom;
	D3DXMatrixIdentity( &matFrom );
	matFrom._11 = vFromDir.x; matFrom._12 = vFromDir.y; matFrom._13 = vFromDir.z;
	matFrom._21 = vUp.x;	  matFrom._22 = vUp.y;		matFrom._23 = vUp.z;
	matFrom._31 = vSide.x;	  matFrom._32 = vSide.y;	matFrom._33 = vSide.z;

	D3DXVec3Cross( &vSide, &vUp, &vToDir );

	D3DXMATRIX matTo;
	D3DXMatrixIdentity( &matTo );
	matTo._11 = vToDir.x;	matTo._12 = vToDir.y;	matTo._13 = vToDir.z;
	matTo._21 = vUp.x;		matTo._22 = vUp.y;		matTo._23 = vUp.z;
	matTo._31 = vSide.x;	matTo._32 = vSide.y;	matTo._33 = vSide.z;

	D3DXMatrixInverse( &matFrom, NULL, &matFrom );
	D3DXMatrixMultiply( &matFrom, &matFrom, &matTo );

	return matFrom;
}

D3DXQUATERNION ioMath::QuaternionFromDirections( const D3DXVECTOR3 &vFromDir,
												 const D3DXVECTOR3 &vToDir )
{
	D3DXMATRIX matRotate = MatrixFromDirections( vFromDir, vToDir );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationMatrix( &qtRotate, &matRotate );

	return qtRotate;
}

float ioMath::QuaterToYaw( const D3DXQUATERNION &qt )
{
	float sqx = qt.x*qt.x;
	float sqy = qt.y*qt.y;
	float sqz = qt.z*qt.z;
	float sqw = qt.w*qt.w;

	return atan2f( 2.0f * (qt.x*qt.z + qt.y*qt.w), (-sqx - sqy + sqz + sqw) ); // rotation about y-axis
}

D3DXQUATERNION ioMath::MakeQuaternionByDegree( float fHeadD, float fPitchD, float fRollD )
{
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,
										DEGtoRAD( fHeadD ),
										DEGtoRAD( fPitchD ),
										DEGtoRAD( fRollD ) );

	return qtRot;
}

float ioMath::ArrangeHead( float fHead )
{
	if( fHead < 0.0f )
	{
		fHead += (float)( (int)( -fHead / 360.0f ) + 1 ) * 360.0f;
	}
	else if( fHead >= 360.0f )
	{
		fHead -= (float)( (int)( fHead / 360.0f ) ) * 360.0f;
	}

	return fHead;
}

float ioMath::ServerToClientAngle( float fAngle )
{
	fAngle += 90.0f;

	return ArrangeHead( -fAngle );
}

float ioMath::ClientToServerAngle( float fAngle )
{
	fAngle -= 90.0f;

	return ArrangeHead( -fAngle );
}

float ioMath::AngleGap( float fStart, float fEnd )
{
	fStart = ArrangeHead( fStart );
	fEnd = ArrangeHead( fEnd );

	int iStartQuater, iEndQuater;
	iStartQuater = GetQuaterAngle( fStart );
	iEndQuater = GetQuaterAngle( fEnd );

	int iQuaterGap = abs( iStartQuater - iEndQuater );
	float fAngleGap = fEnd - fStart;

	if( iQuaterGap == 2 || iQuaterGap == 3 )
	{
		if( fabs(fAngleGap) > 180.0f )
		{
			if( fAngleGap > 0.0f )
				fAngleGap = fAngleGap - 360.0f;
			else
				fAngleGap = 360.0f + fAngleGap;
		}
	}

	return fAngleGap;
}

int ioMath::GetQuaterAngle( float fAngle )
{
	if( COMPARE( fAngle, 0.0f, 90.0f ) )
		return 1;
	
	if( COMPARE( fAngle, 90.0f, 180.0f ) )
		return 2;
	
	if( COMPARE( fAngle, 180.0f, 270.0f ) )
		return 3;

	return 4;
}

float ioMath::RotateToTargetAngle( float fCurAngle, float fTargetAngle, float fAmt )
{
	float fAngleGap = AngleGap( fCurAngle, fTargetAngle );

	if( fabs( fAngleGap ) > fAmt )
	{
		if( fAngleGap > 0.0f )
		{
			return fCurAngle + fAmt;
		}
		else
		{
			return fCurAngle - fAmt;
		}
	}

	return fTargetAngle;	
}

D3DXQUATERNION ioMath::CalcShortestArcQuater( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2 )
{
	D3DXVECTOR3 vCross;
	D3DXVec3Cross( &vCross, &v1, &v2 );

	float fDot = D3DXVec3Dot(  &v1, &v2 );
	if( fDot < -1.0f + ALMOST_ZERO )
		return D3DXQUATERNION( 0.0f, 1.0f, 0.0f, 0.0f );

	float s = sqrt( ( 1.0f + fDot ) * 2.0f );
	float rs = 1.0f / s;

	return D3DXQUATERNION( vCross.x * rs, vCross.y * rs, vCross.z * rs, s * FLOAT05 );
}

float ioMath::CalcHeadBetweenVec( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2 )
{
	D3DXVECTOR2 v1_2D, v2_2D;

	v1_2D.x = v1.x;
	v1_2D.y = v1.z;
	D3DXVec2Normalize( &v1_2D, &v1_2D );

	v2_2D.x = v2.x;
	v2_2D.y = v2.z;
	D3DXVec2Normalize( &v2_2D, &v2_2D );

	float fHead = acos( D3DXVec2Dot( &v1_2D, &v2_2D ) );
	if( D3DXVec2CCW( &v1_2D, &v2_2D ) > 0.0f )
	{
		fHead = -fHead;
	}

	return fHead;
}

float ioMath::CalcPitchVec( const D3DXVECTOR3 &vVec )
{
	D3DXVECTOR2 vSightDir2D;
	vSightDir2D.x = 1.0f;
	vSightDir2D.y = vVec.y;
	D3DXVec2Normalize( &vSightDir2D, &vSightDir2D );

	float fPitch = acos( D3DXVec2Dot( &vSightDir2D, &D3DXVECTOR2( 1.0f, 0.0f) ) );
	if( D3DXVec2CCW( &vSightDir2D, &D3DXVECTOR2( 1.0f, 0.0f) ) < 0.0f )
	{
		fPitch = -fPitch;
	}

	return fPitch;
}

bool ioMath::IsEqual( const D3DXVECTOR2 &v1, const D3DXVECTOR2 &v2, float fEpsilon )
{
	if ( fabsf(v1.x - v2.x) > fEpsilon )
		return false;
	if ( fabsf(v1.y - v2.y) > fEpsilon )
		return false;

	return true;
}

bool ioMath::IsEqual( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fEpsilon )
{
	if ( fabsf(v1.x - v2.x) > fEpsilon )
		return false;
	if ( fabsf(v1.y - v2.y) > fEpsilon )
		return false;
	if ( fabsf(v1.z - v2.z) > fEpsilon )
		return false;

	return true;
}

bool ioMath::IsEqual( const D3DXVECTOR4 &v1, const D3DXVECTOR4 &v2, float fEpsilon )
{
	if ( fabsf(v1.x - v2.x) > fEpsilon )
		return false;
	if ( fabsf(v1.y - v2.y) > fEpsilon )
		return false;
	if ( fabsf(v1.z - v2.z) > fEpsilon )
		return false;
	if ( fabsf(v1.w - v2.w) > fEpsilon )
		return false;

	return true;
}

bool ioMath::IsEqual( const D3DXQUATERNION &qt1, const D3DXQUATERNION &qt2, float fEpsilon )
{
	if ( fabsf(qt1.x - qt2.x) > fEpsilon )
		return false;
	if ( fabsf(qt1.y - qt2.y) > fEpsilon )
		return false;
	if ( fabsf(qt1.z - qt2.z) > fEpsilon )
		return false;
	if ( fabsf(qt1.w - qt2.w) > fEpsilon )
		return false;

	return true;
}

D3DXVECTOR3 ioMath::RandomDeviant( float fAngle, const D3DXVECTOR3 &vVec )
{
	float fRandAngle = UnitRandom() * D3DX_PI * 2.0f;
	return Deviant( fAngle, fRandAngle, vVec );
}

D3DXVECTOR3 ioMath::Deviant( float fAngle,
							 float fRandAngle,
							 const D3DXVECTOR3 &vVec )
{
	D3DXQUATERNION qt;
	D3DXQuaternionRotationAxis( &qt,
								&vVec,
								fRandAngle );

	D3DXVECTOR3 vUpVec = qt * PerPendicular( vVec );
	D3DXQuaternionRotationAxis( &qt,
								&vUpVec,
								fAngle );

	return qt * vVec;
}

D3DXVECTOR3 ioMath::PerPendicular( const D3DXVECTOR3 &vVec )
{
	D3DXVECTOR3 vPerp;
	D3DXVec3Cross( &vPerp, &vVec, &ioMath::UNIT_X );

	if( D3DXVec3LengthSq( &vPerp ) < ALMOST_ZERO * ALMOST_ZERO )
	{
		D3DXVec3Cross( &vPerp, &vVec, &ioMath::UNIT_Y );
	}

	return vPerp;
}

D3DXVECTOR3 ioMath::CalcPlaneNormal( const D3DXVECTOR3 &v0,
									 const D3DXVECTOR3 &v1,
									 const D3DXVECTOR3 &v2 )
{
	D3DXVECTOR3 v01 = v1 - v0;
	D3DXVECTOR3 v02 = v2 - v0;

	D3DXVec3Normalize( &v01, &v01 );
	D3DXVec3Normalize( &v02, &v02 );

	D3DXVECTOR3 vNormal;
	D3DXVec3Cross( &vNormal, &v01, &v02 );

	return vNormal;
}

void ioMath::CalcMinMaxPoint( D3DXVECTOR3 &vMin,
							  D3DXVECTOR3 &vMax,
							  const Vector3Vec &vVtxList )
{
	if( vVtxList.empty() ) return;

	vMin = vMax = vVtxList[0];

	//D3DXVECTOR3 vCurVtx;
	int iSize = vVtxList.size();
	for( int i=1 ; i<iSize ; i++ )
	{
		//UJ 130903, 최적화: min/max 비교횟수를 줄일수있음 --start
		//vCurVtx = vVtxList[i];
		//if( vMin.x > vCurVtx.x )
		//	vMin.x = vCurVtx.x;
		//if( vMin.y > vCurVtx.y )
		//	vMin.y = vCurVtx.y;
		//if( vMin.z > vCurVtx.z )
		//	vMin.z = vCurVtx.z;
		//if( vMax.x < vCurVtx.x )
		//	vMax.x = vCurVtx.x;
		//if( vMax.y < vCurVtx.y )
		//	vMax.y = vCurVtx.y;
		//if( vMax.z < vCurVtx.z )
		//	vMax.z = vCurVtx.z;
		if( vMin.x > vVtxList[i].x ) vMin.x = vVtxList[i].x; else if ( vMax.x < vVtxList[i].x ) vMax.x = vVtxList[i].x;		
		if( vMin.y > vVtxList[i].y ) vMin.y = vVtxList[i].y; else if ( vMax.y < vVtxList[i].y ) vMax.y = vVtxList[i].y;
		if( vMin.z > vVtxList[i].z ) vMin.z = vVtxList[i].z; else if ( vMax.z < vVtxList[i].z ) vMax.z = vVtxList[i].z;
		//UJ --end
	}
}

void ioMath::CalcMinMaxPoint( D3DXVECTOR3 &vMin,
							  D3DXVECTOR3 &vMax,
							  const D3DXVECTOR3 *vVtxArray,
							  int iVtxCnt )
{
	//D3DXVECTOR3 vCurVtx;
	vMin = vMax = vVtxArray[0];
	for( int i=1 ; i<iVtxCnt ; i++ )
	{
		//UJ 130903, 최적화: min/max 비교횟수를 줄일수있음 --start
		//vCurVtx = vVtxArray[i];
		//if( vMin.x > vCurVtx.x )
		//	vMin.x = vCurVtx.x;
		//if( vMin.y > vCurVtx.y )
		//	vMin.y = vCurVtx.y;
		//if( vMin.z > vCurVtx.z )
		//	vMin.z = vCurVtx.z;
		//if( vMax.x < vCurVtx.x )
		//	vMax.x = vCurVtx.x;
		//if( vMax.y < vCurVtx.y )
		//	vMax.y = vCurVtx.y;
		//if( vMax.z < vCurVtx.z )
		//	vMax.z = vCurVtx.z;
		if( vMin.x > vVtxArray[i].x ) vMin.x = vVtxArray[i].x; else if ( vMax.x < vVtxArray[i].x ) vMax.x = vVtxArray[i].x;		
		if( vMin.y > vVtxArray[i].y ) vMin.y = vVtxArray[i].y; else if ( vMax.y < vVtxArray[i].y ) vMax.y = vVtxArray[i].y;
		if( vMin.z > vVtxArray[i].z ) vMin.z = vVtxArray[i].z; else if ( vMax.z < vVtxArray[i].z ) vMax.z = vVtxArray[i].z;
		//UJ --end
	}
}

void ioMath::CalcDirectionMatrix( D3DXMATRIX &matRotate, const D3DXVECTOR3 &vDir )
{
	D3DXVECTOR3 vUnitDir;
	D3DXVec3Normalize( &vUnitDir, &vDir );

	D3DXVECTOR3 vUp = UNIT_Y;
	if( fabsf( D3DXVec3Dot( &vUnitDir, &vUp ) ) > 0.99f )
	{
		vUp = UNIT_Z;
	}

	D3DXVECTOR3 vSide;

	D3DXVec3Cross( &vSide, &vUp, &vUnitDir );
	D3DXVec3Cross( &vUp, &vUnitDir, &vSide );

	D3DXMatrixIdentity( &matRotate );
	matRotate._11 = vSide.x;
	matRotate._12 = vSide.y;
	matRotate._13 = vSide.z;

	matRotate._21 = vUp.x;
	matRotate._22 = vUp.y;
	matRotate._23 = vUp.z;

	matRotate._31 = vUnitDir.x;
	matRotate._32 = vUnitDir.y;
	matRotate._33 = vUnitDir.z;
}

void ioMath::CalcDirectionQuaternion( D3DXQUATERNION &qtRotate, const D3DXVECTOR3 &vDir )
{
	D3DXMATRIX matRotate;
	CalcDirectionMatrix( matRotate, vDir );

	D3DXQuaternionRotationMatrix( &qtRotate, &matRotate );
}

void ioMath::MakeTransform( const D3DXQUATERNION &qtOrientaton,
							const D3DXVECTOR3 &vPosition,
							const D3DXVECTOR3 &vScale,
							D3DXMATRIX &matDest )
{
	// Ordering:
    //    1. Scale
    //    2. Rotate
    //    3. Translate

	D3DXMATRIX matScale, matRotate;
	D3DXMatrixScaling( &matScale, vScale.x, vScale.y, vScale.z );
	D3DXMatrixRotationQuaternion( &matRotate, &qtOrientaton );

	D3DXMatrixMultiply( &matDest, &matScale, &matRotate );
	matDest._41 = vPosition.x;
	matDest._42 = vPosition.y;
	matDest._43 = vPosition.z;
}

void ioMath::MakeInverseTransform( const D3DXQUATERNION &qtOrientaton,
								   const D3DXVECTOR3 &vPosition,
								   const D3DXVECTOR3 &vScale,
								   D3DXMATRIX &matDest )
{
	MakeTransform( qtOrientaton, vPosition, vScale, matDest );
	D3DXMatrixInverse( &matDest, NULL, &matDest );
}

D3DXVECTOR3 ioMath::Lerp( float fFactor, const D3DXVECTOR3 &rkP, const D3DXVECTOR3 &rkQ )
{
	return D3DXVECTOR3( LerpFloat( rkP.x, rkQ.x, fFactor ),
						LerpFloat( rkP.y, rkQ.y, fFactor ),
						LerpFloat( rkP.z, rkQ.z, fFactor ) );
}


// assert:  Dot(p,q) >= 0 (guaranteed in NiRotKey::Interpolate methods)
// (but not necessarily true when coming from a Squad call)
// This algorithm is Copyright (c) 2002 Jonathan Blow, from his article 
// "Hacking Quaternions" in Game Developer Magazine, March 2002.
D3DXQUATERNION ioMath::Slerp( float fFactor, const D3DXQUATERNION &rkP, const D3DXQUATERNION &rkQ )
{
	float fCos = D3DXQuaternionDot( &rkP, &rkQ );
	
	D3DXQUATERNION kT;
	if( fCos < 0.0f )
	{
		fCos = -fCos;
		kT   = -rkQ;
	}
	else
	{
		kT = rkQ;
	}

	float fTPrime;
	if( fFactor <= FLOAT05 )
	{
		fTPrime = CounterWarp( fFactor, fCos );
	}
	else
	{
		fTPrime = 1.0f - CounterWarp( 1.0f - fFactor, fCos );
	}

	D3DXQUATERNION kResult( LerpFloat( rkP.x, kT.x, fTPrime ),
							LerpFloat( rkP.y, kT.y, fTPrime ),
							LerpFloat( rkP.z, kT.z, fTPrime ),
							LerpFloat( rkP.w, kT.w, fTPrime ) );

	FastNormalize( kResult );

	return kResult;
}
