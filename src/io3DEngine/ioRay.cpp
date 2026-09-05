

#include "stdafx.h"

#include "ioRay.h"
#include "ioMath.h"

ioRay::ioRay()
{
	m_vOrigin = m_vDirection = ioMath::VEC3_ZERO;
}

ioRay::ioRay( const D3DXVECTOR3 &vOrig, const D3DXVECTOR3 &vDir ) 
: m_vOrigin(vOrig), m_vDirection(vDir)
{
}

ioRay::ioRay( const ioRay &rhs, const D3DXMATRIX *pMat )
{
	D3DXVec3TransformCoord( &m_vOrigin, &rhs.GetOrigin(), pMat );
	D3DXVec3TransformNormal( &m_vDirection, &rhs.GetDirection(), pMat );
}

ioRay::~ioRay()
{
}

void ioRay::SetOrigin( const D3DXVECTOR3 &vOrig )
{
	m_vOrigin = vOrig;
}

void ioRay::SetDirection( const D3DXVECTOR3 &vDir )
{
	m_vDirection = vDir;
}

void ioRay::SetXAxisDirection( bool bInverse )
{
	if( bInverse )
		m_vDirection.x = -1.0f;
	else
		m_vDirection.x = 1.0f;

	m_vDirection.y = 0.0f;
	m_vDirection.z = 0.0f;
}

void ioRay::SetYAxisDirection( bool bInverse )
{
	m_vDirection.x = 0.0f;

	if( bInverse )
		m_vDirection.y = -1.0f;
	else
		m_vDirection.y = 1.0f;

	m_vDirection.z = 0.0f;
}

void ioRay::SetZAxisDirection( bool bInverse )
{
	m_vDirection.x = 0.0f;
	m_vDirection.y = 0.0f;

	if( bInverse )
		m_vDirection.z = -1.0f;
	else
		m_vDirection.z = 1.0f;
}

const D3DXVECTOR3& ioRay::GetOrigin() const
{
	return m_vOrigin;
}

const D3DXVECTOR3& ioRay::GetDirection() const
{
	return m_vDirection;
}