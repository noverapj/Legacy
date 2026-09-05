

#include "stdafx.h"
#include "ioPlane.h"

ioPlane::ioPlane()
{
	ZeroMemory( &m_Plane, sizeof(m_Plane) );
}

ioPlane::ioPlane( const ioPlane &rhs )
{
	m_Plane = rhs.m_Plane;
}

ioPlane::ioPlane( const D3DXVECTOR3 &vNormal, float fConstant )
{
	SetPlane( vNormal, fConstant );
}

ioPlane::ioPlane( const D3DXVECTOR3 &vNormal, const D3DXVECTOR3 &vPoint )
{
	SetPlane( vNormal, vPoint );
}

ioPlane::ioPlane( const D3DXVECTOR3 &vPoint1,
				  const D3DXVECTOR3 &vPoint2,
				  const D3DXVECTOR3 &vPoint3 )
{
	SetPlane( vPoint1, vPoint2, vPoint3 );
}

ioPlane::~ioPlane()
{
}

void ioPlane::SetPlane( float a, float b, float c, float d )
{
	m_Plane.a = a;
	m_Plane.b = b;
	m_Plane.c = c;
	m_Plane.d = d;

	D3DXPlaneNormalize( &m_Plane, &m_Plane );
}

void ioPlane::SetPlane( const D3DXVECTOR3 &vNrm, float fDist )
{
	m_Plane.a = vNrm.x;
	m_Plane.b = vNrm.y;
	m_Plane.c = vNrm.z;
	m_Plane.d = fDist;
	
	D3DXPlaneNormalize( &m_Plane, &m_Plane );
}

void ioPlane::SetPlane( const D3DXVECTOR3 &vNrm, const D3DXVECTOR3 &vPoint )
{
	D3DXPlaneFromPointNormal( &m_Plane, &vPoint, &vNrm );
	D3DXPlaneNormalize( &m_Plane, &m_Plane );
}

void ioPlane::SetPlane( const D3DXVECTOR3 &vPoint1,
						const D3DXVECTOR3 &vPoint2,
						const D3DXVECTOR3 &vPoint3 )
{
	D3DXPlaneFromPoints( &m_Plane, &vPoint1, &vPoint2, &vPoint3 );
	D3DXPlaneNormalize( &m_Plane, &m_Plane );
}

void ioPlane::Transform( const D3DXMATRIX &mat )
{
	D3DXMATRIX	matTemp;

	D3DXMatrixInverse( &matTemp, NULL, &mat );
	D3DXMatrixTranspose( &matTemp, &matTemp );
	
	D3DXPlaneTransform( &m_Plane, &m_Plane, &matTemp );
}

ioPlane::Side ioPlane::GetSide( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax ) const
{
	D3DXVECTOR3 vTestPos;

	vTestPos.x = ( m_Plane.a > 0.0f ) ? vMax.x : vMin.x;
	vTestPos.y = ( m_Plane.b > 0.0f ) ? vMax.y : vMin.y;
	vTestPos.z = ( m_Plane.c > 0.0f ) ? vMax.z : vMin.z;

	return GetSide( vTestPos );
}

ioPlane::Side ioPlane::GetSide( const D3DXVECTOR3 &vPoint ) const
{
	float fDist = GetDistance( vPoint );	

	if( fDist < 0.0f )
		return NEGATIVE_SIDE;

	if( fDist > 0.0f )
		return POSITIVE_SIDE;

	return NO_SIDE;
}

